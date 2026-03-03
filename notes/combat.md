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
| Siphon | SupportHealer | Provides shields, has an external cooldown |
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

---

## Design Principles

1. **Decisions over execution.** The player never plays. They manage.
2. **The meta is alive.** Patch cycles shift stat weights and spec tier lists. Adapting is the skill.
3. **Sim is invisible.** Math runs hidden. The player sees logs, not formulas.
4. **Loot is political.** Stat weights make the same drop genuinely contested by multiple players.
5. **Every spec has a reason to exist.** Unique flags and raid buffs make roster construction a real strategic problem.
6. **Phase simulation creates narrative.** Wipe reports tell a story the player can act on.