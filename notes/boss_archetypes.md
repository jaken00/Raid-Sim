# Boss Archetypes & Mechanics

## Scripted Boss Mechanics (3.1.5)

Bosses have 2–3 scripted abilities triggered at HP thresholds.

```cpp
struct BossMechanic {
    float trigger_hp_pct;  // e.g. 0.75, 0.50, 0.25
    MechanicType type;     // See mechanic types below
    float intensity;       // Scaling factor (damage mult, add count, etc.)
};
```

**Example boss definition:**

```cpp
Boss flamelord = {
    .hp       = 100000,
    .mechanics = {
        { .trigger_hp_pct = 0.75f, .type = AOE_DAMAGE,   .intensity = 2.0f },
        { .trigger_hp_pct = 0.50f, .type = SUMMON_ADDS,  .intensity = 3.0f },
        { .trigger_hp_pct = 0.25f, .type = ENRAGE,       .intensity = 1.5f },
    }
};
```

## Available Mechanic Types

| Type | Description |
|---|---|
| **AOE_DAMAGE** | Raid-wide damage pulse — healer check |
| **SUMMON_ADDS** | Spawns smaller enemies — DPS priority check |
| **ENRAGE** | Boss damage multiplier increases — DPS race |
| **TANK_SWAP** | Debuff forces tank rotation |
| **INTERRUPT_CHECK** | Boss cast must be interrupted or wipes |
| **DISPEL_CHECK** | DoT/debuff must be cleansed |
| **GROUND_EFFECT** | Zone damage — positional check (future) |
| **DAMAGE_REFLECT** | Boss reflects a % of incoming damage |

## Procedural Boss Struct (3.4.3)

Used for generated bosses beyond the handcrafted MVP boss:

```cpp
struct ProceduralBoss {
    std::string name;         // Generated: theme prefix + archetype suffix
    BossArchetype archetype;  // CASTER, MELEE, SUMMONER, TANK_BUSTER

    std::vector<BossMechanic> mechanics;  // 2–3, selected from compatible pool

    int hp;      // Base * difficulty_mult
    int damage;  // Base * difficulty_mult

    LootTable loot;
};
```

**Mechanic selection rules:**
- 2–3 mechanics per boss, no duplicate types
- Mechanic pool filtered by archetype (e.g. SUMMONER always gets SUMMON_ADDS)
- Difficulty scales intensity values, not mechanic count

## Boss Archetypes

| Archetype | Forced Mechanic | Common Secondary |
|---|---|---|
| CASTER | INTERRUPT_CHECK | AOE_DAMAGE |
| MELEE | TANK_SWAP | ENRAGE |
| SUMMONER | SUMMON_ADDS | AOE_DAMAGE |
| TANK_BUSTER | TANK_SWAP | DAMAGE_REFLECT |

## MVP

For the first playable loop: 1 handcrafted boss, 2 mechanics only (AOE_DAMAGE at 50%, ENRAGE at 25%). No procedural generation yet.


Armored — heavily plated physical enemies. Golems, constructs, heavily armored knights, siege creatures. Strong against slashing/physical force, weak to electricity forcing its way through the gaps.
Warded — magically shielded enemies. Mages, liches, magical constructs, enchanters. Their magic shell absorbs elemental damage but struggles against cold that disrupts the magical field.
Spectral — undead, ghosts, shadow creatures, incorporeal beings. Physical weapons pass through them. Fire and Radiant (holy light) are their traditional weaknesses.
Primal — beasts, elementals, nature creatures, raw primal entities. No magical shielding, no armor plating — just raw HP and physicality. Weak to physical force.