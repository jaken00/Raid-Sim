# Combat System V2 — Event-Driven Timeline

## Why V2

The V1 combat system calculates player DPS as a flat number per phase, builds a boss spell stack for a pre-estimated duration, and resolves boss spells one by one while draining boss HP proportionally. This worked as a proof of concept, but it has fundamental limitations:

- **Player damage is invisible.** DPS is a single number applied evenly — no per-spell crits, no variance between casts, no visible player actions on the timeline.
- **Fight duration is pre-calculated.** The system estimates how long a phase lasts, then builds a boss stack for that duration. But duration depends on damage, and damage depends on crits that haven't been rolled yet. Circular dependency.
- **The step-through playback is spell-granular, not time-granular.** Each step is "boss cast spell X." There's no sense of time passing — just a counter incrementing.
- **Utility buffs have nowhere to live.** A Warsinger casting Battle Hymn to buff raid crit has no mechanism to affect other players' damage mid-fight.

V2 replaces the pre-calculated stack model with an event-driven timeline where players and the boss both generate actions, resolved in chronological order until someone dies.

---

## Core Architecture: The Priority Queue

V2 has no tick rate. No fixed time intervals. No pre-calculated fight duration.

Every entity in the fight (each player and the boss) has a **cast cadence** — how often they act. At the start of the fight, everyone queues their first action at `t=0.0`. The simulation pops the earliest event, resolves it, and the source entity queues their next action. This repeats until the fight ends.

The data structure is a **min-heap priority queue** sorted by timestamp. The event with the smallest timestamp always resolves next.

### Initial State

```
Priority Queue at t=0.0:
  t=0.0  Vex (Striker)      → Mortal Strike   (next cast in 1.2s)
  t=0.0  Kael (Arcanist)    → Arcane Blast    (next cast in 1.8s)
  t=0.0  Thorn (Mender)     → Holy Light      (next cast in 2.2s)
  t=0.0  Boss               → Melee           (next cast in 2.0s)
```

### Resolution Loop

1. Pop lowest-timestamp event from the queue.
2. If source is dead, skip it.
3. Resolve the event (apply damage/healing, roll crit, check buffs).
4. Check exit conditions (boss dead, raid wiped, enrage timer).
5. Source entity queues their next cast at `current_time + cast_interval`.
6. Record a snapshot for the replay log.
7. Repeat.

### Exit Conditions

The fight ends when any of these occur:

- **Kill:** Boss HP ≤ 0. Timestamp of the killing blow is the fight duration.
- **Wipe:** Alive players < wipe threshold.
- **Enrage:** `current_time` exceeds the boss enrage timer.

Fight duration is *discovered*, not calculated. Two identical rosters can have different kill times because crit rolls went differently. This is real, visible variance.

---

## Key Data Structures

### CombatEvent (the timeline entry)

```cpp
struct CombatEvent {
    float       timestamp;
    std::string source;          // player name or "Boss"
    std::string spell_name;
    float       base_value;      // pre-crit, pre-buff damage or healing
    float       final_value;     // after all modifiers
    bool        did_crit;
    EventType   type;            // DAMAGE, HEAL, BUFF, MECHANIC
    bool        is_player_action;
    std::string target;          // who got hit/healed
};
```

### PendingCast (what lives in the priority queue)

```cpp
struct PendingCast {
    float timestamp;
    int   source_id;     // player index, or -1 for boss
    int   spell_index;   // position in rotation

    bool operator>(const PendingCast& other) const {
        return timestamp > other.timestamp;
    }
};
```

### FightLog (the complete output)

```cpp
struct FightLog {
    FightResult               result;      // KILL, WIPE, ENRAGE
    float                     end_time;
    std::vector<CombatEvent>  events;      // full timeline, in order
    std::vector<DeathRecord>  deaths;      // name, timestamp, cause
    KillingBlow               killing_blow;
    // Aggregate stats derived after simulation completes
    std::map<std::string, float> player_total_damage;
    std::map<std::string, float> player_total_healing;
};

struct DeathRecord {
    std::string player_name;
    float       timestamp;
    std::string killed_by;   // spell name that dealt the lethal hit
};
```

---

## Cast Speed & Haste

Cast speed is per-spell and modified by the player's haste rating and their spec's haste weight.

```cpp
float get_cast_interval(float base_cast_time, const Player& p) {
    float haste_pct = (p.GetTotalHaste() / 100.0f) * p.GetSpec().getStatWeights().haste;
    return base_cast_time / (1.0f + haste_pct);
}
```;

A spell with a 2.5s base cast on a player with enough haste might resolve every 1.8s. More haste = more casts per fight = more total damage. Haste now produces visible effects: more events on the timeline, faster action cadence in the replay.

---

## Per-Spell Damage Calculation

Every player cast rolls its own crit and applies all modifiers individually. The existing V1 formulas are reused — they just apply per-spell instead of once per phase.

```cpp
float calculate_spell_damage(Player* p, Spell& spell, Boss& boss,
                             bool did_crit, ActiveBuffs& buffs) {

    float base = spell.base_damage
               * ilvl_factor(p->GetIlvl(), boss.GetBossilvl())
               * p->GetPerformanceRating()
               * resist_profile(*p, boss)
               * get_fight_affinity(*p, boss.getCurrentPhase());

    // Apply active buff modifiers (e.g. Battle Hymn +5% crit damage)
    base *= buffs.get_damage_modifier(p);

    // Crit doubles damage (tunable multiplier)
    float crit_mult = did_crit ? 2.0f : 1.0f;

    // Per-cast variance — tighter for high consistency players
    float variance = 0.90f + (rand_float() * 0.20f * (1.0f - p->GetConsistency()));

    return base * crit_mult * variance;
}
```

### Crit Rolling

```cpp
bool roll_crit(Player* p, ActiveBuffs& buffs) {
    float crit_chance = (p->GetTotalCritStrike() / 100.0f)
                      * p->GetSpec().getStatWeights().critStrike;

    // Add temporary crit buffs
    crit_chance += buffs.get_crit_bonus(p);

    return rand_float() < std::clamp(crit_chance, 0.0f, 0.95f);
}
```

---

## Player Spell Rotations

Each spec has 8 spells (design target). Players cycle through them in priority order. Each spell defines:

```cpp
struct PlayerSpell {
    std::string name;
    float       base_damage;      // 0 for heals/buffs
    float       base_healing;     // 0 for damage spells
    float       base_cast_time;   // seconds, before haste
    SpellType   type;             // DAMAGE, HEAL, BUFF, UTILITY
    DamageType  damage_type;      // FIRE, SHADOW, etc.
    bool        is_aoe;
    int         aoe_targets;      // if is_aoe
    // For buff spells:
    BuffEffect  buff;             // what it grants
    float       buff_duration;    // seconds
};
```

The rotation is the order these spells fire. For V2, this can be a simple cycle (spell 1, 2, 3, ... 8, repeat). Future versions can add priority logic (use Execute when boss < 20% HP, use cooldowns on pull, etc).

```cpp
int Player::GetNextSpellIndex(int current) {
    return (current + 1) % m_rotation.size();
}
```

---

## Buff System

Buffs are events on the timeline like everything else. When a player casts a buff spell, it creates an `ActiveBuff` entry with an expiration time.

```cpp
struct ActiveBuff {
    std::string source;          // who cast it
    std::string name;
    float       applied_at;
    float       expires_at;
    float       crit_bonus;      // additive crit chance
    float       damage_mult;     // multiplicative damage modifier
    float       haste_bonus;     // additive haste
    float       dr_bonus;        // damage reduction
    BuffScope   scope;           // SELF, TARGET, RAID_WIDE
};
```

When resolving any damage or heal event, the system checks active buffs:

```cpp
float ActiveBuffs::get_damage_modifier(Player* p, float current_time) {
    float mult = 1.0f;
    for (auto& buff : m_active) {
        if (buff.expires_at < current_time) continue;  // expired
        if (buff.scope == BuffScope::RAID_WIDE
            || buff.source == p->GetName()  // self-buff
        ) {
            mult *= buff.damage_mult;
        }
    }
    return mult;
}
```

This is how utility specs earn their roster slot. A Warsinger casting Battle Hymn at t=12.0s gives the raid +5% crit for 10 seconds. Every player spell that resolves between t=12.0 and t=22.0 benefits. The DPS meter shows the spike. The replay shows the moment. The player sees *why* utility matters.

---

## Boss Rotation & Phase Transitions

The boss has a spell rotation per phase, just like players. Instead of pre-building a damage stack for an estimated duration, the boss queues one cast at a time.

```cpp
// Boss checks phase transitions when taking damage
void Boss::TakeDamage(float dmg) {
    m_current_hp -= dmg;
    float hp_pct = m_current_hp / m_max_hp;

    // Phase transition: swap rotation when HP crosses threshold
    if (m_current_phase == 0 && hp_pct <= 0.75f) {
        m_current_phase = 1;
        m_spell_index = 0;  // reset rotation for new phase
    }
    // ... additional phase thresholds
}
```

Phase transitions happen organically during resolution. When boss HP crosses 75%, the boss's next queued spell comes from the Phase 2 rotation. No pre-calculation needed.

### Enrage Timer

The enrage timer is a hard cutoff. If `current_time > boss.enrage_timer`, the fight ends as a wipe regardless of boss HP. This prevents infinite fights when the raid lacks DPS. It replaces V1's "DPS check" — instead of passing or failing a threshold, you either kill the boss before the timer or you don't.

---

## Healing in V2

Healers operate on the same system: they have a rotation of healing spells, each with a base cast time modified by haste. Their spells target raid members instead of the boss.

### Heal Targeting (Simple V2)

```cpp
Player* select_heal_target(std::vector<Player*>& raid) {
    Player* lowest = nullptr;
    float lowest_pct = 1.0f;
    for (auto* p : raid) {
        if (p->getCurrentHealth() <= 0) continue;
        float pct = p->getCurrentHealth() / p->GetMaxHp();
        if (pct < lowest_pct) {
            lowest_pct = pct;
            lowest = p;
        }
    }
    return lowest;
}
```

Healers always heal the lowest HP player. This is a simplification — future versions can add smarter triage (prioritize tanks, ignore players above 90%, etc).

### Mana Model

Healers still use the V1 mana model, but it integrates more naturally. Each heal spell has a mana cost. When a healer runs out of mana, they stop queuing new casts (or switch to a low-cost filler spell). The effect is visible on the timeline: healer events slow down and eventually stop, healing throughput drops, raid health deteriorates, deaths follow.

```cpp
// When a healer resolves a spell:
healer->SpendMana(spell.mana_cost);

// When queuing next cast:
if (healer->GetCurrentMana() < next_spell.mana_cost) {
    // OOM — either skip or queue a cheap filler
    // This is where phase duration affecting mana pressure
    // emerges NATURALLY instead of being calculated upfront
}
```

---

## What V1 Code Carries Forward

| V1 Component | V2 Status |
|---|---|
| `ilvl_factor()` | Reused per-spell |
| `crit_multiplier()` | Replaced by per-cast crit roll |
| `haste_multiplier()` | Replaced by cast interval calculation |
| `resist_profile()` | Reused per-spell |
| `get_fight_affinity()` | Reused per-spell |
| `Phase` struct | Retained — phases are HP thresholds that swap boss rotation |
| `BossMechanicCheck` | Retained — checked at phase transitions |
| `DefensiveState` | Retained — shields/DR apply to boss damage events during resolution |
| `Spec` class | Retained — stat weights, role, damage type all still used |
| `FightDebugData` | Replaced by `CombatEvent` (richer, per-spell) |
| `damageStack()` | Replaced by boss rotation + priority queue |
| `resolveDamage()` | Replaced by `resolve_timeline()` |
| `attemptPhase()` | Replaced by `simulate_encounter()` (single function, no phase splitting) |
| `m_sim_history` / `FightStep` | Replaced by `FightLog.events` |

---

## The Simulation Loop (Pseudocode)

```cpp
FightLog simulate_encounter(std::vector<Player*>& raid, Boss& boss) {
    FightLog log;
    ActiveBuffs buffs;

    // Min-heap: smallest timestamp pops first
    std::priority_queue<PendingCast, std::vector<PendingCast>,
                        std::greater<PendingCast>> queue;

    // Seed: everyone queues first cast at t=0
    for (int i = 0; i < (int)raid.size(); i++) {
        queue.push({0.0f, i, 0});
    }
    queue.push({0.0f, -1, 0});  // boss first cast

    while (!queue.empty()) {
        PendingCast next = queue.top();
        queue.pop();
        float t = next.timestamp;

        // --- EXIT: Enrage ---
        if (t > boss.GetEnrageTimer()) {
            log.result = FightResult::ENRAGE;
            log.end_time = t;
            break;
        }

        // --- BOSS ACTION ---
        if (next.source_id == -1) {
            Spell& spell = boss.GetRotationSpell(next.spell_index);
            auto targets = get_targeted_players(spell, raid);

            for (auto* target : targets) {
                if (target->getCurrentHealth() <= 0) continue;

                // Apply defensive layer: shields → DR → raw damage
                float mitigated = apply_defenses(spell.damage_value, target, buffs, t);
                target->takeDamage(mitigated);

                log.events.push_back({
                    t, "Boss", spell.spell_name,
                    spell.damage_value, mitigated,
                    false, EventType::DAMAGE, false,
                    target->GetName()
                });

                if (target->getCurrentHealth() <= 0) {
                    log.deaths.push_back({target->GetName(), t, spell.spell_name});
                }
            }

            // Check wipe
            if (count_alive(raid) < WIPE_THRESHOLD) {
                log.result = FightResult::WIPE;
                log.end_time = t;
                break;
            }

            // Boss queues next cast
            float interval = boss.GetCastInterval(next.spell_index);
            int next_idx = boss.GetNextSpellIndex(next.spell_index);
            queue.push({t + interval, -1, next_idx});
        }

        // --- PLAYER ACTION ---
        else {
            Player* p = raid[next.source_id];
            if (p->getCurrentHealth() <= 0) continue;

            PlayerSpell& spell = p->GetRotationSpell(next.spell_index);

            if (spell.type == SpellType::DAMAGE) {
                bool crit = roll_crit(p, buffs);
                float dmg = calculate_spell_damage(p, spell, boss, crit, buffs);
                boss.TakeDamage(dmg);

                log.events.push_back({
                    t, p->GetName(), spell.name,
                    spell.base_damage, dmg,
                    crit, EventType::DAMAGE, true,
                    "Boss"
                });

                if (boss.GetCurrentHP() <= 0) {
                    log.result = FightResult::KILL;
                    log.end_time = t;
                    log.killing_blow = {p->GetName(), spell.name};
                    break;
                }
            }
            else if (spell.type == SpellType::HEAL) {
                Player* target = select_heal_target(raid);
                if (target) {
                    bool crit = roll_crit(p, buffs);
                    float heal = calculate_heal(p, spell, crit, buffs);
                    target->ReceiveHeal(heal);

                    log.events.push_back({
                        t, p->GetName(), spell.name,
                        spell.base_healing, heal,
                        crit, EventType::HEAL, true,
                        target->GetName()
                    });
                }
            }
            else if (spell.type == SpellType::BUFF) {
                buffs.apply({
                    p->GetName(), spell.name,
                    t, t + spell.buff_duration,
                    spell.buff.crit_bonus,
                    spell.buff.damage_mult,
                    spell.buff.haste_bonus,
                    spell.buff.dr_bonus,
                    spell.buff.scope
                });

                log.events.push_back({
                    t, p->GetName(), spell.name,
                    0, 0, false, EventType::BUFF, true, ""
                });
            }

            // Player queues next cast
            float interval = get_cast_interval(spell.base_cast_time, *p);
            int next_idx = p->GetNextSpellIndex(next.spell_index);
            queue.push({t + interval, next.source_id, next_idx});
        }
    }

    // Post-simulation: aggregate stats from log.events
    compute_aggregates(log);
    return log;
}
```

---

## Replay & Visualization

The `FightLog` is the single source of truth for all visualization. Two presentation modes, same data:

### Playback Mode (watching the fight)

A float `playback_time` advances by `delta_time * playback_speed` each frame. The UI renders all events where `timestamp <= playback_time`:

- **Boss HP bar** drains as damage events accumulate.
- **Raid frames** pulse with damage/healing per snapshot.
- **DPS/HPS ticker** updates as a rolling window over recent events.
- **Death notifications** pop when a `DeathRecord` timestamp is reached.
- **Buff indicators** appear/disappear based on active buff windows.
- **Scrub bar** lets the player drag to any point — just sets `playback_time` directly.

No steps/second slider. The player picks playback speed (1x, 2x, 4x, skip to results).

### Analysis Mode (post-fight breakdown)

After the fight, the full `FightLog` is available for Warcraft Logs-style analysis:

- **DPS over time** — bucket events into 1-second windows, chart total and per-player damage.
- **Healing over time** — same bucketing for heal events.
- **Damage taken per player** — filter boss damage events by target.
- **Death timeline** — visual markers on the timeline showing when and why each death occurred.
- **Buff uptime** — horizontal bars showing when each buff was active and its coverage.
- **Per-player breakdown** — total damage, crit rate, spell distribution, damage per spell.

### From Log to Charts (Example)

```cpp
// DPS over time: bucket damage events into 1-second windows
std::map<int, float> dps_timeline;
for (auto& ev : log.events) {
    if (ev.is_player_action && ev.type == EventType::DAMAGE) {
        int second = (int)ev.timestamp;
        dps_timeline[second] += ev.final_value;
    }
}
// dps_timeline[14] = total player damage dealt during second 14-15
```

---

## Wipe Report (V2)

The wipe report evolves from V1's static text to a data-driven summary derived from the event log:

```
Attempt 3 — The Undying Pyre

Result: WIPE at 41% HP  |  Duration: 187.4s  |  Cause: Enrage

Timeline:
  0:00   Pull. Raid DPS: 1,847
  0:28   Phase 2 begins (75% HP). Shadow Bolt Volley starts.
  0:47   Kael (Mender) killed by Shadow Bolt Volley.
         Raid healing dropped from 612 HPS → 394 HPS.
  1:12   Vex (Striker) killed by Melee (no healer coverage).
         Raid DPS dropped from 1,847 → 1,203.
  1:38   Phase 3 begins (50% HP). Enrage approaching.
  2:14   Two further deaths. DPS insufficient for remaining HP.
  3:07   Enrage timer reached. Raid wiped.

Key Stats:
  Total raid damage:    346,201
  Boss HP remaining:    410,000 (41%)
  Highest DPS:          Vex (Striker) — 623 DPS (died at 1:12)
  Lowest DPS:           Dirge (Shadow) — 289 DPS (50% resist penalty)
  Healing deficit:      Phase 2 required 600 HPS, had 394 after Kael death

Root Cause: Healer death in Phase 2 cascaded into DPS deaths.
Suggestion: Add a second healer or bring shield spec to absorb P2 AOE.
```

Every number in this report is derived directly from `FightLog` data. No separate calculation needed.

---

## Build Order

### Step 1: CombatEvent & FightLog structs
Define the new data structures. These can coexist with V1 types.

### Step 2: Player spell rotations
Design 8 spells per spec (DPS and tank specs). Each spell needs: name, base damage, base cast time, damage type, spell type. Start with 2-3 specs to validate the system.

### Step 3: Priority queue simulation loop
Implement `simulate_encounter()`. Seed the queue, pop-resolve-requeue. Use the existing `ilvl_factor`, `resist_profile`, and `get_fight_affinity` functions. Boss uses existing Phase/rotation data.

### Step 4: Buff system
Implement `ActiveBuffs`. Add 1-2 buff spells to a utility spec. Verify buffs modify damage during their active window.

### Step 5: Healing integration
Port healer logic to the event system. Healers target lowest HP, spend mana per cast, degrade when OOM.

### Step 6: Replay playback
Build the time-based playback UI. Boss HP bar, raid frames, event feed. Scrub bar for random access.

### Step 7: Analysis view
Post-fight charts: DPS/HPS over time, death timeline, per-player breakdowns. Bucket `CombatEvent` data into time windows.

### Step 8: Remove V1
Once V2 is validated, remove `resolveDamage`, `attemptPhase`, `damageStack`, `FightStep`, and the old debug window.