#include "player.h"
#include <string>
#include <vector>


enum class GuildLevel {
    Learner,
    SoftCore,
    MediumCore,
    HardCore,
    Top100,
    RaceToWorldFirstContendors
};


class Guild
{
    private:
        std::string guildName;
        std::vector<Player*> guildMembers;
        GuildLevel guildLevel;

    public:
        Guild(/* args */);
        ~Guild();
};

Guild::Guild(/* args */)
{
}

Guild::~Guild()
{
}
