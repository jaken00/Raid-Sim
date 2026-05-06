# Combat V2 Build Checklist

## To Do (in order)

### Step 1 — Fill SimTypes.h gaps
- [x] Add `PendingCast` struct
- [x] Add `KillingBlow` struct
- [x] Add `killing_blow` to `FightLog`
- [x] Add `std::map<std::string, float> player_total_damage` to `FightLog`
- [x] Add `std::map<std::string, float> player_total_healing` to `FightLog`

### Step 2 — Player rotation (player.h + player.cpp)
- [x] Add `std::vector<PlayerSpell> m_rotation` to private members
- [x] Add `void SetRotation(const std::vector<PlayerSpell>& spells)`
- [x] Add `PlayerSpell& GetRotationSpell(int index)`
- [ ] Initialize `currentHealth` in constructor — `setCurrentHealth()` is never called during construction, so `currentHealth` starts as garbage

### Step 3 — ActiveBuffs class (add to SimTypes.h or new file)
- [ ] Class wrapping `std::vector<ActiveBuff> m_active`
- [ ] `void apply(ActiveBuff buff)`
- [ ] `float get_damage_modifier(Player* p, float current_time)`
- [ ] `float get_crit_bonus(Player* p, float current_time)`
- [ ] Update `calculate_spell_damage` and `roll_crit` signatures: `ActiveBuff&` → `ActiveBuffs&`

### Step 4 — Boss V2 methods (Boss.h)
- [ ] Add `float enrage_timer` to private members + set in constructor — declared but no backing field yet
- [ ] Implement `GetEnrageTimer()` — no implementation in Boss.cpp
- [x] `GetRotationSpell(int index)` — implemented
- [x] `GetNextSpellIndex(int current)` — implemented
- [x] `GetCastInterval(int spell_index)` — implemented, **but fix 2 bugs (see below)**

### Step 5 — Define spec spells
- [ ] At least 1 DPS spec — 3-4 spells minimum
- [ ] At least 1 Healer spec — 2-3 spells (mix of ST and AoE heal)
- [ ] Boss phase — fill `phase_spells` with 2-3 `Spell` objects, set `cooldown` = cast interval

### Step 6 — simulate_encounter() (fight.h + fight.cpp)
- [ ] Add `FightLog simulate_encounter()` declaration to fight.h
- [ ] Create `priority_queue` (min-heap using `std::greater<PendingCast>`)
- [ ] Seed: push `{0.0f, i, 0}` for each player, `{0.0f, -1, 0}` for boss
- [ ] Main loop: pop → enrage check → branch on `source_id == -1`
- [ ] Boss branch: get spell, pick targets, apply damage, log event, check wipe, requeue
- [ ] Player DAMAGE branch: roll_crit → calculate_spell_damage → boss.takeDamage → check kill → log event
- [ ] Player HEAL branch: select_heal_target → calculate heal → target->heal → log event
- [ ] Player BUFF branch: buffs.apply → log event
- [ ] End of each branch: push next `PendingCast` at `t + interval`

### Step 7 — compute_aggregates (fight.cpp)
- [ ] Iterate `log.events`
- [ ] For `DAMAGE` + `is_player_action`: add `final_value` to `player_total_damage[source]`
- [ ] For `HEAL` + `is_player_action`: add `final_value` to `player_total_healing[source]`

### Step 8 — Wire to UI (do this last)
- [ ] Replace `attemptFight()` call site with `simulate_encounter()`
- [ ] Update UI to consume `FightLog` instead of `EncounterResult`

---

## Bugs to fix before Step 6

### Bug 1 — `GetCastInterval` return type (Boss.h line 80 + Boss.cpp line 60)
`cooldown` is a `float` but the method is declared and returns `int` — this silently truncates 2.5s → 2, 4.5s → 4.
Fix: change both declaration and definition to `float GetCastInterval(int spell_index)`.

### Bug 2 — `GetCastInterval` binds reference to temporary (Boss.cpp line 61)
```cpp
auto& spell = GetRotationSpell(spell_index);  // GetRotationSpell returns by VALUE
```
`auto&` on a by-value return is a compile error (non-const ref to temporary). Fix:
```cpp
Spell spell = GetRotationSpell(spell_index);
return spell.cooldown;
```

### Bug 3 — `currentHealth` uninitialized in Player
The `Player` constructor never calls `setCurrentHealth()`, so `currentHealth` is garbage at the start of a fight. Fix: add `currentHealth(maxHp)` to the constructor initializer list in `player.h`.

---

## PlayerSpell Field Reference

> Write these down when designing spells on paper.

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Spell name, e.g. `"Mortal Strike"` |
| `base_damage` | float | Raw damage number. Set to `0` for heals or buffs. |
| `base_healing` | float | Raw healing number. Set to `0` for damage or buffs. |
| `base_cast_time` | float | Seconds between casts before haste. e.g. `1.5`, `2.0` |
| `type` | SpellType | `DAMAGE` / `HEAL` / `BUFF` |
| `damage_type` | DamageType | `Physical` / `Fire` / `Storm` / `Frost` / `Shadow` / `Radiant` |
| `is_aoe` | bool | `true` or `false` |
| `aoe_targets` | int | How many targets if AoE. `0` otherwise. |
| `buff_duration` | float | Seconds the buff lasts. `0` if not a buff. |
| `buff.crit_bonus` | float | Additive crit chance. e.g. `0.05` = +5% crit |
| `buff.damage_mult` | float | Multiplicative damage. e.g. `1.05` = +5% damage |
| `buff.haste_bonus` | float | Additive haste. e.g. `0.10` = +10% haste |
| `buff.dr_bonus` | float | Damage reduction. e.g. `0.15` = 15% less damage taken |
| `buff.scope` | BuffScope | `SELF` / `TARGET` / `RAID_WIDE` |

### Rules by type

```
DAMAGE spell:  base_damage > 0, base_healing = 0, buff_duration = 0
HEAL spell:    base_healing > 0, base_damage = 0, buff_duration = 0
BUFF spell:    base_damage = 0, base_healing = 0, buff_duration > 0, fill buff fields
```

---

## Example: Starter Striker Rotation (4 spells)

| # | name | base_damage | base_cast_time | type | damage_type |
|---|------|-------------|----------------|------|-------------|
| 1 | Mortal Strike | 450 | 1.5 | DAMAGE | Physical |
| 2 | Rend | 280 | 1.2 | DAMAGE | Physical |
| 3 | Heroic Strike | 600 | 2.0 | DAMAGE | Physical |
| 4 | Slam | 350 | 1.8 | DAMAGE | Physical |

## Example: Starter Mender Rotation (3 spells)

| # | name | base_healing | base_cast_time | type | is_aoe | aoe_targets |
|---|------|-------------|----------------|------|--------|-------------|
| 1 | Holy Light | 500 | 2.2 | HEAL | false | 0 |
| 2 | Flash Heal | 350 | 1.4 | HEAL | false | 0 |
| 3 | Circle of Light | 220 | 2.5 | HEAL | true | 5 |

## Example: Boss Phase 1 Spells (fill phase_spells)

| spell_name | damage_value | cooldown | isAoe | number_of_targets |
|------------|-------------|----------|-------|-------------------|
| Melee | 300 | 2.0 | false | 1 |
| Shadow Bolt | 450 | 4.5 | false | 1 |
| Void Burst | 600 | 12.0 | true | 3 |
