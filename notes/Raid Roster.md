# Raid Roster — Design Overview

**Genre:** Idle/Incremental RPG with Active Combat Management
**Platform:** C++ Desktop Application

**Core loop:** Build a 5-hero team → Run dungeons for loot → Progress through raid tiers → Prestige and repeat

**Key differentiator:** WoW-style ability priority system (automated, not button-mashing) meets idle progression and procedural content.

---

## Notes Index

| File | Contents |
|---|---|
| `MVP.md` | The first playable milestone — one boss, one loop, ship it |
| `combat.md` | Tick system, ability priority, Ability struct |
| `stats.md` | Stat table, damage formulas, item stat budget |
| `boss_archetypes.md` | Mechanic types, BossMechanic struct, procedural boss generation |
| `classes.md` | 5 classes, 2 specs each, rarity system, skill tree structure |
| `loot.md` | Gear slots, item properties, rarity table, LootTable struct |
| `scaling.md` | Tier progression, failure/retry, prestige system, offline gains |

---

## Tech Stack

- **Language:** C++17/20
- **Rendering:** SDL2 (window, input, 2D rendering)
- **UI:** ImGui with SDL2+SDL_Renderer backend (these are tightly coupled — set up together on day one)
- **Persistence:** SQLite3
- **Build:** CMake
- **Data:** JSON for ability/skill configs

## Project Structure

```
/RaidRoster
├── /src
│   ├── /core          # Game loop, state management
│   ├── /combat        # Combat simulation engine
│   ├── /characters    # Hero classes, specs, skill trees
│   ├── /dungeons      # Procedural generation
│   ├── /loot          # Item generation, drop tables
│   ├── /ui            # All UI screens/windows
│   ├── /data          # SQLite wrapper, save/load
│   └── main.cpp
├── /assets
│   ├── /sprites       # Character/enemy sprites
│   ├── /ui            # UI icons, elements
│   └── /data          # JSON configs (abilities, loot tables)
├── /database
│   └── schema.sql
└── CMakeLists.txt
```

---

## Database Schema

```sql
CREATE TABLE player (
    id INTEGER PRIMARY KEY,
    gold INTEGER DEFAULT 0,
    current_tier INTEGER DEFAULT 1,
    prestige_level INTEGER DEFAULT 0,
    total_skill_points INTEGER DEFAULT 15,
    last_login TIMESTAMP
);

CREATE TABLE heroes (
    id INTEGER PRIMARY KEY,
    player_id INTEGER,
    class TEXT,
    spec TEXT,
    rarity TEXT,
    level INTEGER DEFAULT 1,
    skill_points_spent INTEGER DEFAULT 0,
    FOREIGN KEY (player_id) REFERENCES player(id)
);

CREATE TABLE skill_allocations (
    id INTEGER PRIMARY KEY,
    hero_id INTEGER,
    node_id TEXT,
    FOREIGN KEY (hero_id) REFERENCES heroes(id)
);

CREATE TABLE equipped_gear (
    id INTEGER PRIMARY KEY,
    hero_id INTEGER,
    slot TEXT,
    item_id INTEGER,
    FOREIGN KEY (hero_id) REFERENCES heroes(id),
    FOREIGN KEY (item_id) REFERENCES items(id)
);

CREATE TABLE items (
    id INTEGER PRIMARY KEY,
    name TEXT,
    item_level INTEGER,
    rarity TEXT,
    slot TEXT,
    stats TEXT,        -- JSON blob: {"strength": 50, "crit": 30}
    set_name TEXT NULL,
    is_legendary BOOLEAN DEFAULT 0
);

CREATE TABLE dungeon_progress (
    id INTEGER PRIMARY KEY,
    player_id INTEGER,
    tier INTEGER,
    difficulty TEXT,
    dungeon_index INTEGER,
    cleared BOOLEAN DEFAULT 0,
    FOREIGN KEY (player_id) REFERENCES player(id)
);

CREATE TABLE materials (
    id INTEGER PRIMARY KEY,
    player_id INTEGER,
    material_type TEXT,
    quantity INTEGER,
    FOREIGN KEY (player_id) REFERENCES player(id)
);
```

---

## Development Roadmap

### Phase 1 — Playable Combat Loop (start here)
- CMake + SDL2 + ImGui setup
- 5 hard-coded hero classes, basic stats
- Combat tick engine vs dummy target
- **Deliverable:** Team fights a dummy, you can read health bars

### Phase 2 — Boss Fight
- Ability priority evaluation
- GCD + resource management
- 1 handcrafted boss with 2 mechanics
- Win/loss conditions
- **Deliverable:** Full MVP loop (see `MVP.md`)

### Phase 3 — Gear & Loot
- Item generation (stats, ilvl, rarity)
- Loot tables, drop rolls
- Gear slots + equip system
- Inventory UI
- **Deliverable:** Boss drops loot, gear affects combat

### Phase 4 — Dungeons
- Procedural dungeon generation (8 themes)
- Room types: trash → mini-boss → boss
- Dungeon progression tracking
- **Deliverable:** Full dungeon run, multiple bosses

### Phase 5 — Skill Trees
- Tree data structure (30–40 nodes)
- Allocation system + respec
- Tree UI
- **Deliverable:** Customizable builds

### Phase 6 — Polish
- Offline gains, speed controls
- SQLite save/load
- Balance pass
- **Deliverable:** V1 shippable

---

## Post-V1 Features

- Prestige system
- More classes and specs (toward 10 classes)
- Advanced boss mechanics (phases, positioning)
- Legendary crafting
- Sound + music
- Achievements
