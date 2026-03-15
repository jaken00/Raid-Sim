#include <string>

struct Resource {
    std::string resourceName;
    float maxResource;
    float resourceCostPerSecond;
    float currentResource;
};

struct Stats {
    int strength;
    int intellect;
    int agility;
    int constitution;
    int expertise;
    int criticalStrike;
    int haste;
    int attackPower;
    int spellPower;
    int healPower;
    int armor;
    int resistance;
    int penetration;
    int armorBreak;
};

enum class BuffType{
    Damage,
    Heal,
    Defensive,
    Mana_Buff,
    Stat_Buff
};

struct RaidBuff {
    std::string name;
    BuffType buffType;
};