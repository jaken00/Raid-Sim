#include <string>

#include "PlayerStructTypes.h"

class Items {
private:
    /* data */
public:
    Items(std::string itemName, std::string slot, float ilvl, Stats stats);

    ~Items();
};

Items::Items(std::string itemName, std::string slot, float ilvl, Stats stats) {}

Items::~Items() {}
