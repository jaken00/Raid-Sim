#include <string>
#include <vector>

#include "BossMechanic.h"
#include "Raid.h"

class Boss {
private:
public:
    Boss(std::string name, int id, Raid* raid, float currentHP, float maxHP,
         std::vector<BossMechanic>);
    ~Boss();
};

Boss::Boss(std::string name, int id, Raid* raid, float currentHP, float maxHP,
           std::vector<BossMechanic>) {}

Boss::~Boss() {}
