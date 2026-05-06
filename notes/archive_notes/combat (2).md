# Guild Manager — Simulation Design Notes

## The Core Concept

Football Manager, but you're the GM of a raiding guild. You never play — you manage. Build a 25-man roster, gear your players, navigate a shifting patch meta, and race rival guilds to world first. The sim engine runs hidden. The player reads logs and makes decisions.

---

## What Gets Modeled vs. What Is Flavor

The key distinction inherited from Football Manager: **attributes are real, execution is invisible.**

| System | Real or Flavor? |
|---|---|
| Role (Tank, Healer, DPS, Support) | Real — gates boss attempt validity |
| ilvl | Real — feeds ilvl_factor multiplier |
| Item stats (Crit, Haste, etc.) | Real — filtered through spec stat weights |
| Performance Rating | Real — represents player skill/execution |
| Resource bars (Mana, Attunement, etc.) | Flavor — shows up in log text only |
| Ability names | Flavor — log text only (until ability build system) |
| Attack range (Melee/Ranged/Caster) | Real — mechanic compatibility check |

---

## Player Model

Every raider is a generated profile:

```cpp
struct Player {
    std::string name;
    Specialization spec;

    int    ilvl;                  // gear level
    float  performance_rating;    // 0–100, execution quality
    float  attendance;            // 0.0–1.0, reliability
    float  consistency;           // tight = low variance, loose = streaky
    // Hidden stats
    float  potential;             // ceiling for performance growth
};
```

### Performance Rating Scale

| Rating | Meaning |
|---|---|
| 90–100 | World class, near spec ceiling |
| 75–89 | Solid, reliable raider |
| 55–74 | Average, gets the job done |
| 35–54 | Underperforming, gear can paper over it |
| < 35 | Liability |

---

## Item Stats & Stat Weights

ilvl sets the total stat budget. Distribution across stats determines whether an item is actually good for a given spec.

```
Total Stat Points = ilvl * 2.5
Primary Stat      = 60% of budget
Secondary Stats   = 40% of budget, split across 2–3 stats
```

Each spec has **stat weights** — multipliers that determine how much each stat contributes to that spec's effective performance:

```cpp
struct StatWeights {
    float strength     = 0.0f;
    float agility      = 0.0f;
    float intellect    = 0.0f;
    float crit         = 0.0f;
    float haste        = 0.0f;
    float constitution = 0.0f;
    float mastery      = 0.0f;
};

// Example — Striker wants agility and crit
StatWeights striker = {
    .agility      = 1.00f,
    .crit         = 0.90f,
    .haste        = 0.50f,
    .constitution = 0.20f
};
```

**Effective item value for a player:**
```
effective_value = sum(stat_amount * spec_weight) / ilvl_budget
```

Two items at the same ilvl can have wildly different values for different specs. This is the loot council drama engine — the same drop is legitimately contested by multiple players for different reasons.

---

## Spec Profile

Every spec has four contribution weights plus unique mechanical flags. The weights feed the sim. The flags gate boss mechanic checks.

```cpp
struct SpecProfile {
    // Contribution weights (0.0–1.0)
    float dps_weight;
    float hps_weight;
    float defensive_weight;   // mitigation, shields, externals
    float utility_weight;     // buffs, debuffs, CC

    // Unique flags
    std::string raid_buff;          // e.g. "physical_amp_5"
    bool can_interrupt;
    bool can_dispel;
    bool provides_shield;
    bool provides_external_cd;      // can save a dying teammate
    float execute_bonus;            // extra DPS sub-20% boss HP
    float aoe_modifier;             // scales up/down on multi-target
    float tank_buster_resistance;
};
```

### Example Specs

| Spec | Role | Unique Value |
|---|---|---|
| Striker | PureDPS | +5% physical raid buff, strong execute |
| Exsanguinate | SupportDPS | Bleed amplifies all damage taken by boss 8% |
| Warsinger | SupportDPS | +3% haste aura to raid, can interrupt |
| Dirge | SupportDPS | Reduces boss self-healing — counters specific boss types |
| Elementalist | PureDPS | +20% AoE damage, weak single target |
| Arcanist | PureDPS | Strong single target, Intellect buff |
| Siphon | Healer | Provides shields, has an external cooldown |
| Bulwark | Tank | Highest defensive weight, physical damage reduction aura |
| Vanguard | OffTank | Lower defense than Bulwark, brings interrupt + taunt cooldown |

Every spec choice is a strategic decision, not a stat comparison.

---

## DPS Contribution Formula

```
dps = ilvl_factor(player.ilvl, boss.tuning_ilvl)
    * performance_rating
    * dps_weight
    * variance()

variance() → 0.85–1.15  (tighter for high consistency players)
```

Total raid DPS = sum across all DPS players, then raid buff multipliers applied on top:

```
total_dps *= (1.0 + sum of all active raid_buff multipliers)
```

---

## Boss Structure

Bosses have thresholds, not a single number to beat. Each threshold is an independent check.

```cpp
struct Boss {
    std::string name;
    int         tuning_ilvl;         // below this, ilvl_factor penalizes

    float dps_requirement;           // total DPS units to beat enrage
    float hps_requirement;           // total healing to survive AOE phases
    int   interrupt_coverage_needed; // minimum interrupt-capable specs
    int   tank_minimum;
    int   dispel_coverage_needed;

    bool  rewards_physical_buffs;    // does physical_amp matter here?
    bool  punishes_melee_heavy;      // ground effects, cleave, etc.

    std::vector<Phase> phases;
};
```

---

## Phase-Based Simulation

The fight is a timeline broken into phases. Each phase is evaluated sequentially. Deaths carry forward.

```
Boss HP: 100% ──── 75% ──── 50% ──── 25% ──── 0%
               P1       P2       P3       P4
             Normal   AOE    Tank     Enrage
                      Pulse  Swap     Race
```

```cpp
struct PhaseResult {
    bool  survived;
    float boss_hp_at_end;
    float raid_hp_at_end;
    std::vector<Death> deaths;
};

PhaseResult simulate_phase(Phase phase, Roster roster, float boss_hp_start) {
    float dps_output   = calculate_total_dps(roster) * phase.duration;
    float boss_hp_end  = boss_hp_start - dps_output;

    float aoe_incoming = phase.aoe_damage_per_player;
    float healing      = calculate_total_hps(roster) * phase.duration;

    auto deaths = resolve_deaths(roster, aoe_incoming, healing);

    if (!deaths.empty())
        dps_output *= calculate_dps_loss_factor(deaths, roster);

    return { deaths.size() < wipe_threshold, boss_hp_end, deaths };
}
```

Deaths in Phase 1 reduce healing and DPS in Phase 2. A healer dying early cascades into raid wipes later. This is what makes wipe reports feel realistic.

---

## Wipe Report Output

```
Attempt 3 — The Undying Pyre

Result: WIPE at 41% HP

Phase 1 (100% → 75%): Survived. Kael took heavy damage.
Phase 2 (75% → 50%): Kael (Mender) died to AOE — healing reduced 18%.
Phase 3 (50% → 25%): Raid healing insufficient. Two further deaths.
Phase 4 (25% → 0%):  Enrage timer hit. DPS race failed.

Checks:
  DPS output:          PASSED (1923 / 1800 required)
  Healing throughput:  FAILED (511 / 600 required)  ← root cause
  Interrupt coverage:  PASSED (3 specs present)
  Tank coverage:       PASSED

Suggested adjustments:
  - Swap one DPS slot for a second healer
  - Consider Siphon (shields reduce healing load in Phase 2)
```

The player reads this, makes one decision, re-attempts. That loop is the game.

---

## Defensive Weight & Constitution

Tanks and shield specs contribute via `defensive_weight`, which feeds a **damage absorbed** calculation:

```
defensive_contribution = defensive_weight
                       * ilvl_factor
                       * performance_rating
                       * constitution_factor
```

Boss phases have an `incoming_damage` value. The raid's total defensive contribution reduces effective damage before healing covers the remainder. This gives Constitution a real purpose — it amplifies a tank's defensive contribution, making it genuinely valuable on tank gear and genuinely useless on a Striker's weapon.

---

## Ability Build System (V2+)

Abilities don't fire in the sim — they modify spec weights at roster-build time. Each spec gets a pool of abilities (target: 15), player picks a subset (target: 8). Each selection shifts the spec's contribution profile:

```
Warsinger base:
  dps_weight: 0.70, utility_weight: 0.75, defensive_weight: 0.20

Player build choices:
  + Battle Hymn       → utility_weight  += 0.10
  + Iron Resolve      → defensive_weight += 0.15
  + Blade Tempo       → dps_weight      += 0.12
  - skips Dissonance Strike → loses can_interrupt flag
```

The player has built an aggressive Warsinger who hits harder but cannot interrupt. That's a roster construction consequence. Balancing means adjusting delta values per ability, not touching simulation logic.

---

## Database Schema (Core Tables)

```sql
CREATE TABLE specialization (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    parent_class        TEXT NOT NULL,
    role                TEXT NOT NULL,   -- Tank, Healer, PureDPS, SupportDPS, etc.
    name                TEXT NOT NULL,
    resource            TEXT NOT NULL,   -- flavor only
    attack_range        TEXT NOT NULL,   -- Melee / Ranged / Caster (real check)

    -- Sim weights
    dps_weight          REAL DEFAULT 0.0,
    hps_weight          REAL DEFAULT 0.0,
    defensive_weight    REAL DEFAULT 0.0,
    utility_weight      REAL DEFAULT 0.0,

    -- Primary stat this spec scales with
    primary_stat        TEXT NOT NULL,

    -- Mechanic flags
    can_interrupt       BOOLEAN DEFAULT 0,
    can_dispel          BOOLEAN DEFAULT 0,
    provides_shield     BOOLEAN DEFAULT 0,
    provides_external_cd BOOLEAN DEFAULT 0,
    raid_buff           TEXT NULL,       -- e.g. "physical_amp_5"
    execute_bonus       REAL DEFAULT 0.0,
    aoe_modifier        REAL DEFAULT 1.0,

    FOREIGN KEY (parent_class) REFERENCES classes(name)
);

CREATE TABLE items (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT,
    item_level      INTEGER,
    rarity          TEXT,
    slot            TEXT,
    strength        INTEGER DEFAULT 0,
    agility         INTEGER DEFAULT 0,
    intellect       INTEGER DEFAULT 0,
    crit            INTEGER DEFAULT 0,
    haste           INTEGER DEFAULT 0,
    constitution    INTEGER DEFAULT 0,
    mastery         INTEGER DEFAULT 0,
    set_name        TEXT NULL,
    is_legendary    BOOLEAN DEFAULT 0
);
```



## Phase Duration — Derived From DPS, Not Set Length

Phases are **boss HP threshold gates**, not timed intervals.
Phase duration is a consequence of how fast your raid burns through HP.

```
phase_duration = phase_hp_pool / total_dps_output
```

**Why this matters:** Slow DPS = longer phase = more healer mana burned = OOM before the phase ends.
DPS is now a healer problem, not just a damage problem. This creates the natural cascade:

```
Phase 2 result: Healers hit 0 mana at 18% boss HP.
Raid took lethal damage. Full wipe.
Primary cause: Phase 2 lasted 47s (target: <30s) — DPS too low.
```

Mana-restoration abilities (Innervate, Siphon drain, Mana Tide equivalents) now have real mechanical value
because they extend healer sustainability when DPS is marginal.

### Healer Mana Model

```cpp
// SimTypes.h
struct HealerState {
    float current_mana;
    float max_mana;
    float mana_per_second_cost;  // at full output
    float hps_at_full_mana;
    float hps_at_oom;            // reduced desperation heals only
};
```

```cpp
// SimEngine.cpp
float effective_hps(HealerState& h, float phase_duration) {
    float mana_needed = h.mana_per_second_cost * phase_duration;

    if (mana_needed <= h.current_mana)
        return h.hps_at_full_mana;

    // Partially OOM — output degrades midway through phase
    float full_time = h.current_mana / h.mana_per_second_cost;
    float oom_time  = phase_duration - full_time;

    return (h.hps_at_full_mana * full_time + h.hps_at_oom * oom_time)
           / phase_duration;
}
```

---

## Damage Types

Every spec deals a primary damage type. Bosses have a resistance profile per type.

```cpp
// SimTypes.h
enum class DamageType {
    PHYSICAL,
    FIRE,
    FROST,
    SHADOW,
    HOLY,
    ARCANE,
    NATURE,
    BLOOD
};

struct BossResistanceProfile {
    std::map<DamageType, float> modifiers;
    // 1.00 = neutral
    // 0.50 = 50% resistance (spec contributes half)
    // 1.35 = vulnerability (spec contributes 35% more)
    // 0.00 = full immunity (spec is useless on this boss)
};
```

**Example — Frost Elemental boss:**
```cpp
BossResistanceProfile frost_boss = {
    { DamageType::FROST,    0.10f },  // 90% resist — frost specs useless
    { DamageType::FIRE,     1.35f },  // vulnerability — fire specs pop off
    { DamageType::PHYSICAL, 1.00f },  // neutral
    { DamageType::SHADOW,   1.00f },  // neutral
};
```

Full immunity (0.0) is how you implement prestige mechanics like
"Frostlord Glacius immune to frost — must bench frost specs."
It's not a special case, it falls out of the resistance system naturally.

---

## Fight Type Affinity

Every boss has a fight type profile. Every spec has an affinity profile.
The match between them modifies contribution — specs that suit the fight do more.

```cpp
// SimTypes.h
struct FightAffinityProfile {
    float single_target_modifier;  // pure patchwerk DPS
    float aoe_modifier;            // 4+ targets
    float cleave_modifier;         // 2–3 targets, council/adds
    float movement_modifier;       // DoTs and instants love movement
    float execute_modifier;        // bonus below 20% boss HP
    float melee_hostile_penalty;   // bosses with ground effects, cleave, frontal
};

struct BossFightType {
    float aoe_demand;              // 0.0–1.0
    float movement_intensity;      // 0.0 = stand still, 1.0 = constant movement
    float melee_friendliness;      // 0.0 = melee hell, 1.0 = melee paradise
    bool  has_execute_phase;
    bool  has_council_adds;        // cleave/dot value goes up
};
```

### Example Spec Affinities

| Spec | Patchwerk | Council/Cleave | Movement | Melee Hostile |
|---|---|---|---|---|
| Striker | 100% | 75% | 60% | 50% |
| Elementalist | 70% | 130% | 95% | 100% |
| Exsanguinate | 90% | 140% | 110% | 85% |
| Arcanist | 100% | 80% | 65% | 100% |
| Warsinger | 85% | 110% | 100% | 90% |

Now your roster decision for a council fight is genuinely different from a single-target fight.
You might bench a higher-ilvl Striker and bring an Exsanguinate because the fight rewards DoT/cleave specs.

---

## Secondary Stat Multipliers

Crit and Haste don't simulate tick-by-tick. They modify the output number through multipliers
scaled by spec stat weights — so the same item gives different value to different specs.

```cpp
// SimEngine.cpp
float crit_multiplier(Player& p) {
    float crit_chance   = p.stats.crit_rating / 100.0f;
    float spec_scaling  = p.spec.stat_weights.crit;
    return 1.0f + (crit_chance * 1.0f * spec_scaling);
}

float haste_multiplier(Player& p) {
    float haste_value  = p.stats.haste_rating / 100.0f;
    float spec_scaling = p.spec.stat_weights.haste;
    return 1.0f + (haste_value * spec_scaling);
}
```

**Example — same Haste item, two specs:**
```
Arcanist (haste weight 0.95): +19% effective DPS
Striker  (haste weight 0.25): +5%  effective DPS
```

This makes the loot council question genuinely interesting.
The item is not just an ilvl comparison — it's a spec fit comparison.

---

## Full DPS Contribution Formula

```cpp
// SimEngine.cpp
float calculate_player_dps(Player& p, Boss& boss) {

    // Base: gear level vs boss tuning, skill, spec role
    float base = ilvl_factor(p.ilvl, boss.tuning_ilvl)
               * p.performance_rating
               * p.spec.dps_weight;

    // Secondary stat multipliers (spec-weighted)
    float crit_mult  = crit_multiplier(p);
    float haste_mult = haste_multiplier(p);

    // Fight type affinity — does this spec suit this boss?
    float affinity = fight_affinity_match(p.spec.affinity, boss.fight_type);

    // Damage type resistance — does this boss resist this spec's damage?
    float resist = boss.resistances[p.spec.damage_type];

    // Variance — execution noise, tighter for high consistency players
    float variance = 0.85f + (rand_float() * 0.30f * (1.0f - p.consistency));

    return base * crit_mult * haste_mult * affinity * resist * variance;
}
```

Every multiplier is tunable. Every multiplier creates a roster decision.

---

## Phase Simulation

```cpp
// SimTypes.h
struct Phase {
    float hp_start_pct;          // e.g. 1.00 (100%)
    float hp_end_pct;            // e.g. 0.75 (75%)
    float aoe_damage_per_player; // per second, during this phase
    bool  is_burn_phase;         // execute bonus applies
    BossMechanicCheck mechanic;  // interrupt check, dispel check, etc.
};

struct PhaseResult {
    bool  survived;
    float boss_hp_at_end;
    float raid_hp_at_end;
    float actual_duration;       // how long the phase took
    std::vector<Death> deaths;
};
```

```cpp
// SimEngine.cpp
PhaseResult simulate_phase(Phase& phase, Roster& roster,
                            float boss_hp_start, float boss_max_hp) {

    float phase_hp_pool = (phase.hp_start_pct - phase.hp_end_pct) * boss_max_hp;
    float total_dps     = calculate_total_dps(roster, boss);

    // Duration is a consequence of DPS — not a fixed timer
    float duration = phase_hp_pool / total_dps;

    // Healing check — does HPS hold up for this duration?
    float total_hps      = calculate_total_hps(roster, duration);
    float total_incoming = phase.aoe_damage_per_player * roster.size() * duration;

    auto deaths = resolve_deaths(roster, total_incoming, total_hps);

    // Deaths reduce output — cascade begins here
    if (!deaths.empty()) {
        total_dps *= calculate_dps_loss_factor(deaths, roster);
    }

    float boss_hp_end = boss_hp_start - (total_dps * duration);

    return {
        .survived        = deaths.size() < wipe_threshold,
        .boss_hp_at_end  = boss_hp_end,
        .actual_duration = duration,
        .deaths          = deaths
    };
}
```

Phases are chained. The `boss_hp_at_end` from Phase N is `boss_hp_start` for Phase N+1.
Deaths carry forward — a healer dying in Phase 1 reduces HPS in every subsequent phase.

---

## Mechanic Coverage Checks

Boss mechanics are independent checks evaluated per phase.
Failure doesn't always wipe — it applies a damage or healing penalty that can cascade into a wipe.

```cpp
// SimTypes.h
struct BossMechanicCheck {
    MechanicType type;           // INTERRUPT, DISPEL, TANK_SWAP, etc.
    int          coverage_needed;
    float        failure_penalty; // damage multiplier if check fails
};
```

The roster either covers the check (enough specs with the right flags) or it doesn't.
Log text reflects the outcome either way.

---

## Wipe Report — Sample Output

```
Attempt 3 — The Undying Pyre

Result: WIPE at 41% HP

Phase 1 (100% → 75%): Survived. Kael took heavy damage. Duration: 28s.
Phase 2 (75% → 50%): Kael (Mender) died to AOE pulse. Healing reduced 18%.
                      Phase lasted 41s — DPS marginal, mana pressure high.
Phase 3 (50% → 25%): Two further deaths. Healing critically low.
Phase 4 (25% → 0%):  Enrage timer exceeded. Raid wiped.

Checks:
  DPS output:          PASSED  (1923 / 1800 required)
  Healing throughput:  FAILED  (511 / 600 required)   ← root cause
  Interrupt coverage:  PASSED  (3 interrupt-capable specs)
  Tank coverage:       PASSED

Fight type analysis:
  Boss type: Council/Adds — AoE and cleave specs favoured
  Your melee roster underperformed on this fight type (-15% effective DPS)
  Consider: swapping Striker for Exsanguinate for this encounter

Damage type analysis:
  Boss: 50% Shadow resistance
  Dirge (Shadow) contributed 48% less than expected
  Consider: benching Dirge on this boss
```

---

## Ability Build System (V2)

Abilities don't fire in the sim. They modify spec weights at roster-build time.

Each spec gets a pool (target: 15 abilities). Player picks a subset (target: 8).
Each selection shifts the spec's contribution profile — no simulation logic changes.

```
Warsinger base profile:
  dps_weight: 0.70  utility_weight: 0.75  defensive_weight: 0.20

Player picks:
  + Battle Hymn        → utility_weight  += 0.10
  + Iron Resolve       → defensive_weight += 0.15
  + Blade Tempo        → dps_weight      += 0.12
  - skips Dissonance Strike → loses can_interrupt flag
```

Result: an aggressive Warsinger who hits harder but cannot cover interrupts.
That's a roster construction consequence — do you have another interrupter?

Balancing = adjusting delta values per ability. Simulation logic is untouched.

---

## Tuning Levers Summary

| Lever | What It Affects |
|---|---|
| `spec.dps_weight` | Base role contribution |
| `spec.stat_weights.*` | How much secondary stats matter per spec |
| `spec.affinity.*` | Performance on different fight types |
| `boss.resistances.*` | Which damage types are viable |
| `boss.fight_type.*` | Which spec affinities are rewarded |
| `phase.aoe_damage_per_player` | Healing pressure per phase |
| `boss.tuning_ilvl` | Gear floor before ilvl_factor penalizes |
| `player.consistency` | Variance range on individual contributions |

Every number is independently tunable. Rebalancing a spec = change its weights.
Rebalancing a boss = change its thresholds and fight type tags.