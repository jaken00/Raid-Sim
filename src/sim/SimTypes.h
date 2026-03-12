

struct FightAffinityProfile {
    float single_target_modifier;  // pure patchwerk DPS
    float aoe_modifier;            // 4+ targets
    float cleave_modifier;         // 2–3 targets, council/adds
    float movement_modifier;       // DoTs and instants love movement
    float execute_modifier;        // bonus below 20% boss HP
    float melee_hostile_penalty;   // bosses with ground effects, cleave, frontal
};

struct HealerState {
    float current_mana;
    float max_mana;
    float mana_per_second_cost;
    float hps_at_full_mana;
    float hps_at_oom;
};

enum class DamageType { PHYSICAL, FIRE, FROST, SHADOW, HOLY, ARCANE, NATURE, BLOOD };