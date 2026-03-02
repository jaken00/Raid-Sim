# Guild Manager — Design Overview

**Working Title:** Guild Manager *(codename — world/title TBD)*
**Genre:** Guild Management Sim / MMO Strategy
**Platform:** C++ Desktop Application
**Elevator Pitch:** Football Manager, but you're the GM of a raiding guild in a living fantasy MMO world. Build your roster, gear your players, navigate the patch meta, and race other guilds to world first.

---

## The Core Fantasy

You are a Guild Master. You don't play the game — you manage the people who do.

You start with a small core of loyal raiders and a guild tag nobody recognizes. Over a season, you recruit talent, gear your roster, theorycraft comps, navigate a shifting meta, and compete against simulated rival guilds on a server leaderboard. When a new raid tier drops, you're watching logs, cutting underperformers, and hunting the recruits who can fill the hole.

The game never asks you to kill anything. It asks you to make decisions.

---

## The Game Loop

```
Pre-Season
└─ Recruit your starting core (8–10 players from a free agent pool)
└─ Set your raid schedule, assign roles, build your roster to 25

Content Patch Drops
└─ New raid tier releases with 6–8 bosses
└─ Pre-BIS lists generate per class/spec
└─ Rival guilds begin progression simultaneously

Progression Phase
└─ Submit your 25-man roster + comp for each attempt
└─ Sim runs — boss attempt resolves in seconds
└─ You get a full combat log: who died, why, what failed
└─ Adjust strategy, gear, or roster and re-attempt

Loot Phase
└─ Boss kills drop loot
└─ You assign loot (loot council, or set rules)
└─ Gear upgrades shift player performance and your pre-BIS priorities

Recruitment Phase (ongoing)
└─ Players from other guilds become available
└─ Free agent pool refreshes
└─ Cut underperformers, poach talent, get poached from yourself

Patch Cycle
└─ Balance patches drop mid-season — class tuning shifts the meta
└─ Tier lists update, some specs rise, some fall
└─ Pre-BIS lists change — gear that was good is now sub-optimal
└─ You adapt or fall behind rival guilds

Season End
└─ Server rankings finalize
└─ New tier announced — cycle begins again
```

---

## Roster System

### Guild Size
- **Active Raid Roster:** 25 players
- **Bench:** 5–10 players (subs, trials, alts)
- **Recruitment Pool:** Persistent free agent market

### Role Composition (25-man)
Standard raid composition enforced by the sim — comps that violate role minimums perform worse:

| Role | Slots |
|---|---|
| Main Tank | 2 |
| Off Tank | 1 |
| Healer | 5–6 |
| Ranged DPS | 6–8 |
| Melee DPS | 6–8 |
| Support | 1–2 |

### Player Profiles
Every raider is a generated character with:
- **Class & Spec** — from the original class system
- **Current ilvl** — their gear level
- **Performance Rating** — sim-derived score (DPS/HPS/survival) relative to their spec's potential
- **Attendance %** — how reliably they show up
- **Attitude** — affects morale, guild culture, drama events
- **Raid History** — previous guilds, progression kills, server rankings
- **Potential** — hidden stat; some players are raw, some are peaked

### Recruitment
- Free agents have full profiles and raid histories (AI-generated, consistent)
- Agents can be scouted — pay gold to reveal hidden stats
- Trial period: new recruits are on 2-week trials before full membership
- Players can quit, get poached by rival guilds, or go inactive

---

## The Sim Engine

The player never sees combat in real time. "Attempt Raid Boss" resolves instantly and returns a report.

### Attempt Resolution
The sim evaluates:
1. Roster comp — role coverage, class utility, buff coverage
2. Average ilvl vs. boss tuning threshold
3. Individual player performance ratings
4. RNG layer — variance simulating execution errors, bad luck, etc.
5. Boss mechanics — specific checks (interrupt coverage, dispel coverage, healer throughput)

### Combat Log Output
After each attempt:
```
Attempt 3 — Ashen Throne, Boss 2: The Undying Pyre

Result: WIPE at 34% HP

Deaths:
  - Kael (Holy Priest) — stood in Ember Ring (Phase 2)
  - Draven (Arms Warrior) — tank swap missed, lethal debuff stack

Notes:
  - Interrupt coverage: 73% (target: 85%+) — consider swapping Mira to interrupt spec
  - Healer throughput: sufficient
  - DPS check: on track if no deaths

Suggested adjustments:
  - Move Theron off main tank for this fight (low armor for this boss type)
  - Add a second interrupt-capable DPS to the priority list
```

The player reads this, makes decisions, and re-attempts. That loop is the game.

---

## Gearing System

### Item Levels
Gear comes from boss kills and crafting. ilvl directly determines stat budget.

```
Pre-Raid (crafted / dungeon) — ilvl 100–120
Normal Raid                  — ilvl 120–140
Heroic Raid                  — ilvl 145–165
Mythic Raid                  — ilvl 170–190
```

### Pre-BIS & BIS Lists
- Dynamically generated per class/spec per patch
- Based on current stat weights (which shift with patches)
- The UI shows each player's current gear vs. their Pre-BIS targets
- Loot drops are flagged: **BIS**, **Pre-BIS**, **Upgrade**, **Off-spec**, **Vendor**

### Loot Council
When loot drops you decide who gets it:
- See each eligible player's current item in that slot
- See how much of an upgrade it is for each player
- See their current BIS progression %
- Make the call — or set loot rules to automate it

Loot decisions affect morale. Give everything to your best player and watch the bench get resentful.

---

## The Patch Cycle & Meta

Every few in-game weeks, a balance patch drops. AI-generated patch notes, real mechanical consequences.

**Example patch note:**
> *Voidweaver — Shadow specialization: Entropy Bolt base damage reduced by 12%. Developers note: Shadow Voidweavers were performing significantly above curve in single-target and have been adjusted accordingly.*

This means:
- Your Shadow Voidweaver's sim DPS drops
- The tier list updates — Shadow falls from S to A
- Pre-BIS stat weights shift slightly (less priority on Spell Power, more on Haste)
- You have a decision: respec him to Void (the other spec), recruit a better-fitting class, or ride it out

The meta is alive. Playing the meta IS the game.

---

## Rival Guilds

You are not alone on the server. 10–20 simulated rival guilds are progressing the same content.

- Each guild has a name, a reputation, a roster quality rating
- They progress at a pace determined by their roster quality + some variance
- **World first** on each boss is tracked — first guild to kill it gets a server announcement
- Rival guilds recruit from the same free agent pool you do — good players get snapped up
- Some rival GMs are aggressive recruiters and will poach your players
- Guild forums exist (flavor text, AI-generated) — trash talk, theorycrafting, drama

---

## Guild Management Layer

Beyond the sim, you manage your guild as an organization:

- **Raid Schedule** — set raid days/times; attendance is tracked against it
- **Loot Rules** — loot council (manual), EP/GP (point system), or open roll
- **Guild Bank** — gold pool for repairs, consumables, crafting materials
- **Player Morale** — affected by loot decisions, bench time, progression speed, drama events
- **Drama Events** — occasional random events: player conflict, officer drama, mass gquit threats

---

## World & Setting

Fully original fantasy world — not WoW, not derivative. Classes and lore are original.

**Design principle:** The world is a backdrop, not the focus. Players don't care about the lore of Kael'thas — they care about Kael the Holy Priest who keeps standing in fire. The world exists to give classes, bosses, and raids a coherent aesthetic identity.

Class names, raid names, boss names, patch flavor text — all original, all AI-assisted at generation time for variety and consistency.

---

## Tech Stack

- **Language:** C++17/20
- **UI:** ImGui — this is a UI-first game, lean into it
- **Persistence:** SQLite3
- **Build:** CMake
- **AI Layer:** Anthropic API — patch note generation, player profile generation, combat log flavor text, guild drama events
- **Art:** DCSS tileset for class portraits, item icons, spell icons

---

## MVP — First Playable

**Goal:** One full attempt loop against one boss.

- 10 hard-coded player profiles with stats and roles
- 1 boss with 2 mechanics (interrupt check, healer throughput check)
- Attempt resolves → log output in ImGui window
- Loot drops → manual assignment to one player
- No recruitment, no patch cycle, no rivals yet

**Deliverable:** You can read a wipe report, swap a player, and attempt again until the boss dies. The core decision loop works.

---

## Design Principles

1. **Endgame always.** The player starts at endgame. No tutorial dungeon crawl.
2. **Decisions over execution.** The player never plays. They manage.
3. **The meta is alive.** Patches shift what's optimal. Adapting is the skill.
4. **Players are people.** Roster management has human stakes, not just stat stakes.
5. **Sim is invisible.** Combat math runs hidden. The player sees logs, not formulas.
6. **AI is infrastructure.** Generated content (patches, profiles, drama) makes the world feel alive without handcrafting everything.