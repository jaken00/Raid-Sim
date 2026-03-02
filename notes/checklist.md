Phase 0 — Environment (Day 1)

 - [x] CMake project compiles with SDL2 + ImGui
 - [x] Window opens, ImGui renders a hello world panel
 - [x] Git repo initialized

Phase 1 — Core Data (Week 1)

 - [x] Define Player struct (name, class, spec, role, ilvl, performance rating, attendance)
 - [x] Define Boss struct (name, hp, mechanics, ilvl threshold)
 - [x] Define AttemptResult struct (wipe/kill, deaths, log lines, loot)
 - [ ] Hard-code 10 players and 1 boss in a test file
 - [ ] Display one player profile in an ImGui window

Phase 2 — The Sim (Week 2)

 - [ ] Write resolveAttempt() — returns result from roster + boss
 - [ ] Implement mechanic checks (interrupt coverage, healer throughput)
 - [ ] RNG layer — variance on performance ratings
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