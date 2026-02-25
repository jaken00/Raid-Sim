// Role.h
#pragma once

enum class Role {
    Tank,
    OffTank,
    Healer,
    SupportHealer,
    SupportDPS,
    PureDPS
};

enum class CastType {
    CAST,
    CHANNEL,
    INSTANT
};

enum class DamageType {
    Physical,
    Fire,
    Storm,
    Frost,
    Shadow,
    Radiant,
};
