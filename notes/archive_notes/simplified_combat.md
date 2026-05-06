float __calculate_player_dps__(Player& p, Boss& boss) {
    // Base: gear level vs boss tuning, skill, spec role
    float base = ilvl_factor(p.ilvl, boss.tuning_ilvl)
               * p.performance_rating
               * p.spec.dps_weight;
    // Secondary stat multipliers (spec-weighted)
    float crit_mult  = crit_multiplier(p);
    float haste_mult = haste_multiplier(p);
    // Fight type affinity — does this spec suit this boss?
    float affinity = fight_affinity_match(p.spec.affinity, boss.fight_type);
    // Damage type resistance — does this boss resist this spec's damage?
    float resist = boss.resistances[p.spec.damage_type];
    // Variance — execution noise, tighter for high consistency players
    float variance = 0.85f + (rand_float() * 0.30f * (1.0f - p.consistency));
    return base * crit_mult * haste_mult * affinity * resist * variance;
}




Step 1:
Calculate the ilvl factor: 

float ilvl_factor(int player_ilvl, int boss_tuning_ilvl) {
    float delta = (float)(player_ilvl - boss_tuning_ilvl) / boss_tuning_ilvl;
    // delta = 0.0  → exactly at tuning → returns 1.0
    // delta = -0.1 → 10% under        → returns ~0.72
    // delta = +0.1 → 10% over         → returns ~1.18 (capped at 1.25)
    float factor = 1.0f + (delta * 2.8f);
    return std::clamp(factor, 0.40f, 1.25f);
}

This is a multiplier base don how low we are 
We multiply this by performance and variance

next we do crit multiplyer:
float __calculate_player_dps__(Player& p, Boss& boss) {
    // Base: gear level vs boss tuning, skill, spec role
    float base = ilvl_factor(p.ilvl, boss.tuning_ilvl)
               * p.performance_rating
               * p.spec.dps_weight;
    // Secondary stat multipliers (spec-weighted)
    float crit_mult  = crit_multiplier(p);
    float haste_mult = haste_multiplier(p);
    // Fight type affinity — does this spec suit this boss?
    float affinity = fight_affinity_match(p.spec.affinity, boss.fight_type);
    // Damage type resistance — does this boss resist this spec's damage?
    float resist = boss.resistances[p.spec.damage_type];
    // Variance — execution noise, tighter for high consistency players
    float variance = 0.85f + (rand_float() * 0.30f * (1.0f - p.consistency));
    return base * crit_mult * haste_mult * affinity * resist * variance;
}

Next we calculate haste:

haste mult = 1 + (haste rating / 100) * spec stat weight

Next is Fight Affinity (how well spec does on fight type)
Finally is resist chance. Multiply by boss damage taken by type -> if boss resists physical multiply by 50% for example. 


# effective HPS calculation:
float effective_mana_cost_per_second(HealerState& h) {
    // Expertise reduces effective mana drain via free proc chance
    // Every proc = one free cast = saves (cast_cost / casts_per_second) mana
    float proc_chance        = h.stats.expertise_rating / 500.0f; // 50 expertise = 10% proc
    float proc_chance_capped = std::min(proc_chance, 0.35f);       // cap at 35% — no free lunch
    float mana_saved_factor  = 1.0f - (proc_chance_capped * 0.6f); // each proc saves ~60% of a cast
    return h.mana_per_second_cost * mana_saved_factor;
}

float effective_hps(HealerState& h, float phase_duration) {
    float actual_cost = effective_mana_cost_per_second(h);
    float mana_needed = actual_cost * phase_duration;

    if (mana_needed <= h.current_mana)
        return h.hps_at_full_mana;

    float full_time = h.current_mana / actual_cost;
    float oom_time  = phase_duration - full_time;

    return (h.hps_at_full_mana * full_time + h.hps_at_oom * oom_time)
           / phase_duration;
}


# New Sim Phase:
PhaseResult simulate_phase(Phase& phase, Roster& roster,
                            float boss_hp_start, float boss_max_hp) {

    float phase_hp_pool = (phase.hp_start_pct - phase.hp_end_pct) * boss_max_hp;
    float total_dps     = calculate_total_dps(roster, boss);
    float duration      = phase_hp_pool / total_dps;

    // Raw incoming damage
    float total_incoming = phase.aoe_damage_per_player 
                         * roster.size() 
                         * duration;

    // --- DEFENSIVE LAYER (new) ---
    DefensiveState defense  = calculate_defense(roster);

    // Step 1: Shields absorb first, before health is touched
    float post_shield = std::max(0.0f, total_incoming - defense.shield_absorb);

    // Step 2: Damage reduction from tank auras, defensive specs
    float post_reduction = post_shield * (1.0f - defense.damage_reduction_pct);

    // Step 3: What's left has to be healed
    float total_hps   = calculate_total_hps(roster, duration);
    auto  deaths      = resolve_deaths(roster, post_reduction, total_hps);

    // Step 4: External CDs can save players who would have died
    deaths = apply_external_cds(deaths, defense.external_cd_charges);

    if (!deaths.empty())
        total_dps *= calculate_dps_loss_factor(deaths, roster);

    float boss_hp_end = boss_hp_start - (total_dps * duration);

    return {
        .survived        = deaths.size() < wipe_threshold,
        .boss_hp_at_end  = boss_hp_end,
        .actual_duration = duration,
        .deaths          = deaths
    };
}

## Calc Defensive:
DefensiveState calculate_defense(Roster& roster) {
    DefensiveState state = {};

    for (auto& player : roster) {
        // Tanks contribute damage reduction via their defensive weight + constitution
        if (player.spec.role == TANK) {
            state.damage_reduction_pct += player.spec.defensive_weight
                                        * constitution_factor(player)
                                        * 0.04f; // each tank contributes up to ~4% DR
        }

        // Shield specs (Siphon, Shieldwarden) contribute absorb budget
        if (player.spec.provides_shield) {
            state.shield_absorb += player.spec.defensive_weight
                                 * ilvl_factor(player.ilvl, boss.tuning_ilvl)
                                 * 500.0f; // tunable flat absorb budget
        }

        // External CDs — one charge per spec that has them
        if (player.spec.provides_external_cd) {
            state.external_cd_charges += 1.0f;
        }
    }

    // Hard cap — you can't reduce incoming damage to nothing
    state.damage_reduction_pct = std::min(state.damage_reduction_pct, 0.35f);

    return state;
}
```

---

## Why This Fixes the All-DPS Problem

Now the math punishes zero-defense rosters naturally:
```
All-DPS roster (no tanks, no shields):
  total_incoming  = 180,000
  shield_absorb   = 0        ← no shield specs
  damage_reduction= 0%       ← no tanks
  post_reduction  = 180,000
  total_hps       = 95,000   ← not enough
  deaths          = 6        ← cascade wipe

Balanced roster (2 tanks, 1 shield healer):
  total_incoming  = 180,000
  shield_absorb   = 22,000   ← Siphon absorbs chunk upfront  
  damage_reduction= 7%       ← tank auras reduce remainder
  post_reduction  = 146,540
  total_hps       = 112,000  ← healers can cover it
  deaths          = 0        ← survived