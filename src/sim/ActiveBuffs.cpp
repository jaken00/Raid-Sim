#include "ActiveBuffs.h"


void ActiveBuffs::apply(ActiveBuff buff) {
	m_active.push_back(buff);
}

float ActiveBuffs::get_damage_modifier(Player* p, float current_time) {
    float mult = 1.0f;  // base multi no change
    for (const auto& buff : m_active) {
        if (current_time > buff.expires_at)
            continue;
        if (buff.scope == BuffScope::SELF && buff.source != p->GetName())
            continue;
        mult *= buff.damage_mult;
    }
    return mult;
}

float ActiveBuffs::get_crit_bonus(Player* p, float current_time) {
    float mult = 1.0f;  // base multi no change
    for (const auto& buff : m_active) {
        if (current_time > buff.expires_at)
            continue;
        if (buff.scope == BuffScope::SELF && buff.source != p->GetName())
            continue;
        mult *= buff.crit_bonus;
    }
    return mult;
}