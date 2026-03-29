#pragma once

#include <string>

#include "PlayerStructTypes.h"

struct Items {
    std::string name;
    Slot itemSlot;
    float ilvl;
    Stats itemStats;
};