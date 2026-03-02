# Stats

## Core Stat Table (3.1.3)

- **Primary Stat:** Strength, Intellect, or Agility, Constitution (Raw HP), Expertise (Buff DPS/Shields)


## Secondary Stats
- Crit
- Haste
- Attack Power
- Spell Power
- Heal Power

| Type      | Ailment                         | vs. Armored | vs. Warded | vs. Spectral | vs. Primal |
|-----------|---------------------------------|-------------|------------|--------------|------------|
| Physical  | Bleed (DoT, 5s)                 | 100%        | 50%        | 50%          | 150%       |
| Fire      | Ignite (DoT, 4s)                | 75%         | 100%       | 150%         | 75%        |
| Storm     | Shock (+15% dmg taken)          | 150%        | 75%        | 100%         | 75%        |
| Frost     | Chill (-20% action speed)       | 50%         | 150%       | 75%          | 100%       |
| Shadow    | Wither (-10% all resist)        | 100%        | 100%       | 100%         | 100%       |
| Radiant   | Cleanse (remove 1 enemy buff)   | 75%         | 75%        | 150%         | 100%       |

Specific Elemental Buffs


| Stat | Effect |
|---|---|
| **Strength** | Increases physical damage |
| **Intellect** | Increases spell damage |
| **Agility** | Increases Resource Regeneration |
| **Constitution** | Increases HP Scaling |
| **Expertise** | Increases Buff/Shield Rating |
| **Critical Strike** | % chance to crit (2× damage) |
| **Haste** | Reduces cast time + GCD |
| **Attack Power** | Flat % Physical damage increase |
| **Spell Power** | Flat % Spell damage increase |
| **Heal Power** | Flat % Heal increase |
| **Armor** | Physical damage reduction |
| **Resistance** | Magical damage reduction |
| **Penetration** | Elemental Penetration |
| **Armor Break** | Armor Penetration |

## Formulas

```
Crit Chance      = Base (5%) + (Crit Rating / 40)
Haste Multiplier = 1 + (Haste Rating / 100)
Damage           = Base * (1 + Versatility / 100) * CritMultiplier
```

CritMultiplier = 2.0 on crit, 1.0 otherwise.

## Item Stat Budget

When gear generates or is hand-authored:

```
Total Stat Points = ilvl * 2.5
Primary Stat      = 60% of budget
Secondary Stats   = 40% of budget, split across 2–3 stats
```

## MVP Note

For the MVP combat loop, implement only: **HP (Stamina), damage (Strength or Intellect), Armor, and Crit**. Haste, Mastery, and Versatility can wait until combat feels good without them.
