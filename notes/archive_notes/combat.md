# Combat System

## Tick System (3.1.1)

- **Tick rate:** 500ms (0.5s per combat tick)
- **Each tick, in order:**
  1. Process Global Cooldown (GCD) expiry for each character
  2. Execute ability priority list for each character
  3. Apply damage / healing / buffs / debuffs
  4. Update boss AI — check mechanic triggers
  5. Check win/loss conditions

## Damage Tyes
| Type      | Ailment                         | vs. Armored | vs. Warded | vs. Spectral | vs. Primal |
|-----------|---------------------------------|-------------|------------|--------------|------------|
| Physical  | Bleed (DoT, 5s)                 | 100%        | 50%        | 50%          | 150%       |
| Fire      | Ignite (DoT, 4s)                | 75%         | 100%       | 150%         | 75%        |
| Storm     | Shock (+15% dmg taken)          | 150%        | 75%        | 100%         | 75%        |
| Frost     | Chill (-20% action speed)       | 50%         | 150%       | 75%          | 100%       |
| Shadow    | Wither (-10% all resist)        | 100%        | 100%       | 100%         | 100%       |
| Radiant   | Cleanse (remove 1 enemy buff)   | 75%         | 75%        | 150%         | 100%       |


## Ability Priority System (3.1.2)

Each character has an ordered priority list. On each tick, the engine walks the list and fires the first ability whose conditions are met and whose cooldown is ready.
The Goal of this is to basically simualte a rotation. This is esentially what WoW's combat is a priority list. 

Healing will be disconnected from this optionally but you can set logically limits Example:
Party below 50% HP -> AOE Heal THis will be HARD to implement 

```cpp
struct AbilityPriority {
    std::vector<Ability*> priority_list;
    // Example for Fire Mage:
    // 1. Combustion       (if cooldown ready + stacks high)
    // 2. Pyroblast        (if Hot Streak proc active)
    // 3. Fireball         (filler — always usable)
};
```

Priority lists are **class-specific data**, not logic. The engine just evaluates them — no special-casing per class in the simulation loop.

## Ability Structure (3.1.4)

```cpp
struct Ability {
    std::string name;
    AbilityType type;        // INSTANT, CAST, CHANNEL
    float cast_time;         // Seconds (modified by Haste)
    float cooldown;          // Seconds
    int resource_cost;       // Mana / Energy / Rage
    DamageType damage_type;  // PHYSICAL, FIRE, FROST, SHADOW, etc.
    float base_damage;

    // Use conditions
    bool requires_buff;
    bool requires_target_debuff;
    float min_health_pct;    // Only fire when target below X% HP
};
```
## Damage Type
- Arcane
- Holy 
- Natural (Nature Damage Lightnighint Earth Etc)
- Phsycial
- Fire
- Frost
- Dark
- Bloodmagic


## Dependencies

Build order: stats → ability structs → tick loop → priority evaluation → boss mechanics.
Do not start on ability priority until the tick loop fires correctly against a dummy target.
