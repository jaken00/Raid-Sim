# Classes & Character System

## V1 Classes & Specs (3.2.1)

Extend the holy trinity into further than just simply tank healer and DPS. I want to build dynamic raids with support classes
for both Healing Tanking and DPS!
Tank / Support Tank
Healer / Shields 
DPS / Buffer
For V1 Only have basic classes

## Classes & Specializations

---
### Resources

Resolve (builds from combat engagement) → rewards being in the fight, punishes downtime
Attunement (dual gauge requiring balance) → rewards alternating between two activities
Essence Charges (independent recharging slots) → rewards planning 2-3 actions ahead
Momentum (builds from variety, lost from repetition) → rewards diverse rotations
Resonance (team-generated shared pool) → creates team interdependence


### **Warrior**
**Specs:** Bulwark | Vanguard  
**Role:** Main Tank / Off-Tank  

- **Bulwark**  
  Pure tank. Spends **Resolve** on damage mitigation and threat generation.  
  *Exclusive:* Stagger conversion (Brewmaster Monk–inspired) — converts burst damage into a manageable DoT.

- **Vanguard**  
  Off-tank / buffer hybrid. Spends **Resolve** on party-wide auras (DAOC Paladin–inspired endurance chants).  
  *Exclusive:* Team-wide damage reduction aura that scales with Resolve spent.

---

### **Conduit**
**Specs:** Mender | Siphon  
**Role:** Main Healer / Support DPS-Healer  

- **Mender**  
  Primary healer.  
  Builds **Light Attunement** from healing spells and **Dark Attunement** from damage spells.  
  At 50/50 balance, unleashes a massive burst heal.  
  Requires alternating healing and damage to maintain balance (Disc Priest / Red Mage hybrid concept).

- **Siphon**  
  Drain-based hybrid.  
  Links to an enemy (reverse Sage’s Kardia concept).  
  Damage dealt to the linked target converts into healing on the lowest-HP ally.  
  Cannot directly heal — all healing is damage-converted.

---

### **Invoker**
**Specs:** Arcanist | Elementalist  
**Role:** Caster DPS  

- **Arcanist**  
  Combines **Essence types** to determine spell outcome (Invoker-inspired system).  
  Examples:  
  - Flame + Storm = Chain Lightning  
  - Flame + Frost = Shatter  
  - Flame + Storm + Frost = Ultimate Nuke  
  Priority system focuses on optimal Essence combinations.

- **Elementalist**  
  AoE / control caster using the same Essence system.  
  Abilities create persistent zone effects (GW2-style combo fields).  
  - Flame Essence → Fire field  
  - Frost Essence → Slow field  
  - Storm Essence → Shock field  
  Overlapping fields combine for bonus effects.
---
### **Reaver**
**Specs:** Exsanguinate | Striker  
**Role:** Buffer DPS / DPS  

- **Exsanguinate**  
  **Momentum stacks** increase the duration and potency of applied DoTs and debuffs.  
  *Exclusive:* “Marking” ability that increases all team damage effectiveness on the marked target.

- **Striker**  
  Single-target burst DPS.  
  Each unique ability used in sequence grants a Momentum stack.  
  At 5 stacks, the next ability deals double damage.  
  Rewards rotational variety.

---

### **Herald**
**Specs:** Warsinger | Dirge  
**Role:** Buffer / Debuffer  

- **Warsinger**  
  Spends **Resonance** on powerful team-wide buffs.  
  Maintains 4 rotating “songs” on a 12-second cycle (automated EQ Bard-style twisting).  
  Equipped songs define the build.  
  Cannot deal meaningful direct damage.

- **Dirge**  
  Debuffer / force multiplier.  
  Spends **Resonance** on powerful enemy debuffs (slow, curses, etc.).  
  *Exclusive:* “Vulnerability” debuff that alters a boss’s damage-type weakness, enabling team exploitation.

## Abilities
Each character has 8 abilities total and 1-3 buffs/passives (Think MOTW, Int Buff, Stam Buff) -> Something they bring as a raid buff.



## Character Rarity (3.2.2)

Affects base stats and skill tree depth. Does not change the class or ability list.

| Rarity | Base Stat Mult | Skill Points | Acquisition |
|---|---|---|---|
| Common | 1.0× | 15 | Free — starting 5 heroes |
| Rare | 1.15× | 17 | 500 gold |
| Epic | 1.3× | 20 | 2000 gold + materials |
| Legendary | 1.5× | 25 | 10,000 gold + rare materials |

-> This will be to allow upgrades of characters in a non-gatcha method. These upgrades will allow them to run different gear
unlock new specs (3rd Specizliation) / new abilities.

## Skill Tree Structure (3.2.3)

- 3 trees per specialization
- 30–40 nodes per tree
- Player picks 15–20 nodes per tree based on rarity's skill point budget

**Universal nodes (available to all classes):**
- +5% Max HP
- +3% All Damage
- Reduced threat generation
- Increased consumable effectiveness

**Class-specific example (Fire Mage, Fire Tree):**

```
[Fire Tree]
├─ Fireball               (baseline ability)
├─ Improved Fireball      (+15% damage)
├─ Pyroblast              (unlocks ability)
│   └─ Hot Streak         (Pyroblast crits → instant cast)
├─ Combustion             (cooldown ability)
│   └─ Improved Combustion (+20s duration)
└─ Fire Mastery           (+10% fire damage)

[Frost Tree]
├─ Frostbolt              (baseline)
├─ Ice Lance              (unlocks ability)
└─ Shatter                (+50% crit vs frozen targets)

[Arcane Tree]
├─ Arcane Missiles        (unlocks ability)
├─ Mana Efficiency        (-10% mana cost)
└─ Evocation              (mana restore cooldown)
```

**Respec cost:** 10g → 25g → 50g → 100g (caps at 100g, resets weekly)

