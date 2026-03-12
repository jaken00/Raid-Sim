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

 - [ ] Write resolveAttempt() — returns result from roster + boss
    - [ ] This requires building out spec class so that we can get DPS/HPS/Defense modiers to get the parties totals
        - [x] DPS Linked
        - [x] HPS Linked
        - [x] Defense Linked
        - [ ] Totals Calculated
        - [ ] Damage Type added to each Specialization
        - [ ] Add boss Damage Reduction Calculation
        - [ ] Add movement modifers to the fight / Decide if this lives in the boss or if it lives in the RaidEncounter (It lives in the boss)
        - [ ] Add FightAffinityStruct to the boss (This lives in JSON we load up per boss?) -> Cleave, AOE, Movement, Etc
        - [ ] Add secondary stat multiplers calcualtions as well as adding to the specs them selves 
        - [ ] Add healerState for mana and effective HPS calculation 
        - [ ] Calculate Phase duration by DPS numbers
        - [ ] Add in DPS variance (determined by current skill level)
        - [ ] Add in Phases and Phase Result calculation (AOE, Burn, HP start and end)  
        - [ ] Reolve deaths and who dies in a phase
        - [ ] Create a function for a chance to roll that a random player dies or wipes the raid due to a mechanic
        - [ ] Recalculate dps on death
        - [ ] BossMechanicCheck struct that outlines that boss mechanics
        - [ ] Account for ability weights (15 total abilites select 8)
        - [ ] Need Weights and tuning for: DPS_Weight, Stats_Weights, Affinity (Cleave, AOE, Single Target), Boss Resistances and Dmg Type, BOss Fight Type, Phase AOE DMG Per Player, Boss Tuning Ilvl Floor, Player Consistancy (Varible lvl)
 - [ ] Generate human-readable log lines from result
 - [ ] Display attempt log in ImGui

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