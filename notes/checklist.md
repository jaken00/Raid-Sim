Phase 0 — Environment (Day 1)

 - [x] CMake project compiles with SDL2 + ImGui
 - [x] Window opens, ImGui renders a hello world panel
 - [x] Git repo initialized

Phase 1 — Core Data (Week 1)

 - [x] Define Player struct (name, class, spec, role, ilvl, performance rating, attendance)
 - [x] Define Boss struct (name, hp, mechanics, ilvl threshold)
 - [x] Define AttemptResult struct (wipe/kill, deaths, log lines, loot)
 - [x] Hard-code 10 players and 1 boss in a test file
 - [x] Display one player profile in an ImGui window

Phase 2 — The Sim (Week 2)

● Yes. Right now Boss is loaded with empty phases {}
  because loadBosses in loader.cpp just passes {}.
  You need:

  1. A getBossPhases query in database.h/.cpp — takes
   a boss_id (or boss name), returns the phase rows
  2. A PhaseRow struct in database.h — mirrors the
  boss_phases table columns
  3. Build Phase objects from those rows in
  loader.cpp — convert the fight_types CSV string
  back into a vector<FightAffinityProfile>, fill the
  rest of the fields
  4. Pass the phases into the Boss constructor
  instead of {}

  The boss_phases table already has everything you
  need — phase_number, hp_start_pct, hp_end_pct,
  is_execute_phase, fight_types (stored as CSV),
  mechanic_name, mechanic_damage_value,
  mechanic_needs_interrupt.

  The trickiest part is the fight_types CSV — it's
  stored as a comma-separated string like
  "single_target,aoe" so you'll need a small helper
  to split it and convert each token to a
  FightAffinityProfile enum value.

   Summary Table

  ┌─────────────────┬──────────┬─────────────────────────────────────────────────────┬───────────────────────────────┐
  │    Category     │ Severity │                        Issue                        │           Location            │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ Critical │ Missing return — UB                                 │ loader.cpp:136, fight.cpp:108 │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ Critical │ std::clamp args swapped — wrong DPS formula         │ fight.cpp:13                  │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ Critical │ Boss phases never loaded — dead data path           │ loader.cpp:153                │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ High     │ get_fight_affinity clobbers loop result             │ fight.cpp:57                  │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ High     │ Division by zero on zero DPS                        │ fight.cpp:92                  │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Reliability     │ High     │ OOB access in AdvancePhase on empty vector          │ Boss.cpp:36                   │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Security        │ Medium   │ SQL injection in isEmpty()                          │ database.cpp:144              │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Performance     │ Medium   │ N+1 query in getAllPlayers()                        │ database.cpp:476              │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Performance     │ Low      │ Spec copied by value in hot loop                    │ fight.cpp:17,31,42,79         │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Maintainability │ High     │ 9 declared methods with no implementation           │ fight.h                       │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Maintainability │ Medium   │ Mixed naming conventions throughout                 │ project-wide                  │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Maintainability │ Medium   │ Typos in public identifiers (survivied, threshhold) │ SimTypes.h, Boss.h            │
  ├─────────────────┼──────────┼─────────────────────────────────────────────────────┼───────────────────────────────┤
  │ Maintainability │ Medium   │ Magic numbers in simulation formula                 │ fight.cpp:8-13                │
  └─────────────────┴──────────┴─────────────────────────────────────────────────────┴───────────────────────────────┘

  The three Critical reliability issues (std::clamp swap, missing returns, phases never loaded) should be fixed first as
   they mean the simulation currently produces incorrect results or undefined behavior on every run.

Phase 3 — The Loop (Week 3)

 - [ ] "Attempt Boss" button triggers sim
 - [ ] Result panel populates after attempt
 - [ ] Swap a player from bench — re-attempt
 - [ ] Boss kill → loot drops → assign to player → ilvl updates
 - [ ] That's your MVP

Phase 4 — Recruitment (Week 4-5)

 - [ ] Free agent pool (JSON file, 50 generated profiles)
 - [ ] Recruitment UI — browse, inspect, sign
 - [ ] Cut a player from roster
 - [ ] Trial system (2-week timer)

Phase 5 — Patch Cycle

 - [ ]Patch data struct (class tuning values, patch notes text)
 - [ ]Hard-code 2-3 patches, apply tuning to sim values
 - [ ]Tier list UI updates on patch
 - [ ]Pre-BIS list generates per class/spec

Phase 6 — AI Layer

 - [ ] Anthropic API call generates patch notes from tuning values
 - [ ] API generates player profile flavor text
 - [ ] Combat log death reasons get AI-written flavor

Phase 7 — Rival Guilds

 - [ ] Rival guild structs with roster quality rating
 - [ ] They progress in background on a timer
 - [ ] Server leaderboard UI
 - [ ] World first announcements