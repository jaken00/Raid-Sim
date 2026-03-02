# Gear & Loot System

## Gear Slots (3.3.1)

8 slots per hero:

1. Head
2. Chest
3. Legs
4. Weapon (or Main Hand + Off Hand)
5. Ring 1
6. Ring 2
7. Trinket 1
8. Trinket 2

## Item Properties (3.3.2)

Each item has:
- **Item Level (ilvl):** Determines total stat budget
- **Primary Stat:** Strength, Intellect, or Agility, Constitution (Raw HP), Expertise (Buff DPS/Shields)
- **Secondary Stats:** 2–3 of: Crit, Haste, Versatility
- **Socket:** 10% chance to generate a gem slot
- **Set Tag:** Optional — links item to a tier set for bonuses

## Secondary Stats
- Crit
- Haste
- Attack Power
- Spell Power
- Heal Power

**Stat budget formula:**

```
Total Stats   = ilvl * 2.5
Primary Stat  = 60% of total
Secondary Stats = 40% of total, split across 2–3 stats
```

## Item Rarity (3.3.3)

| Rarity | ilvl Range | Source |
|---|---|---|
| Common | 1–50 | Early dungeons |
| Uncommon | 40–100 | Normal dungeons |
| Rare | 90–150 | Heroic dungeons |
| Epic | 140–200 | Mythic dungeons, Normal raids |
| Legendary | 190–250 | Mythic raids, crafting |

## Set Bonuses (3.3.4)

Tier sets have 2-piece and 4-piece bonuses. These are class-specific and tied to the current tier.

**Example — Fire Mage Tier 1:**
- **(2) Set:** Pyroblast deals 10% more damage
- **(4) Set:** Hot Streak grants +20% crit for 8 seconds

## Loot Table System (3.3.5)

```cpp
struct LootTable {
    DungeonType dungeon_type;  // FIRE, FROST, SHADOW, etc.
    int tier_level;

    std::vector<Item*> guaranteed_drops;  // Always drops

    // Boss rolls 1–3 items from this pool
    std::vector<ItemDrop> random_pool;
};

struct ItemDrop {
    Item* item;
    float drop_chance;  // 0.0–1.0
};
```

**Legendary drops:** Specific dungeons have a guaranteed legendary. Example: "Dungeon of Eternal Flame" always drops "Staff of Inferno" (BiS Fire Mage weapon). These are the target farm dungeons.

## MVP Note

For the first playable loop: 1 hard-coded loot item drops on boss kill. No loot table, no RNG, no inventory UI. The item exists only to confirm the reward loop works end-to-end. Gear management UI comes in Phase 3.
