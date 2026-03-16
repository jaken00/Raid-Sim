

enum class DamageType { PHYSICAL, FIRE, FROST, SHADOW, HOLY, ARCANE, NATURE, BLOOD };

struct AffinityTable {
    DamageType weakness;
    DamageType resist; 
};