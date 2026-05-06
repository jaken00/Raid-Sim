#pragma once
#include "SimTypes.h"
#include <vector>

class ActiveBuffs {
	public:
        void apply(ActiveBuff buff);
        float get_damage_modifier(Player* p, float current_time);
        float get_crit_bonus(Player* p, float current_time);
    private:
        std::vector<ActiveBuff> m_active;
};