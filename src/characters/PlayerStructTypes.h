#pragma once
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
    int armor;
    int resistance;
};

enum class Slot{
    Head,
    Neck,
    Shoulders,
    Chest,
    Legs,
    Feet,
    MainHand,
    OffHand
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