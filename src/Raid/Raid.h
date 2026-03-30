#pragma once
#include <string>
#include <vector>

class Player;
class Boss;



struct Raid {
    std::string name;
    std::vector<Boss*> boss_list;
    std::vector<Player*> player_list;
};