#pragma once

#ifndef BUFF_H
#define BUFF_H

// Enum listing all possible buff types
enum class BuffType
{
    ExtraHealth,       // Grants additional health
    DamageBoost,       // Increases attack damage
    DamageReduction,   // Reduces incoming damage
    SpeedBoost,        // Increases movement speed
    JumpBoost,         // Increases jump strength
    HealthRegen,        // Enables health regeneration over time
    BUFF_COUNT
};

// A simple struct to hold a single buff's type and its magnitude
struct Buff
{
    BuffType type;   // The type of buff
    float value;     // The value/multiplier/delta of the buff

    Buff(BuffType t = BuffType::ExtraHealth, float v = 0.0f)
        : type(t), value(v) {}
};

inline Buff CreateBuff(BuffType type)
{
    switch (type)
    {
    case BuffType::ExtraHealth:
        return Buff{ BuffType::ExtraHealth, 25.0f };
    case BuffType::DamageBoost:
        return Buff{ BuffType::DamageBoost, 0.2f }; // +20% damage
    case BuffType::DamageReduction:
        return Buff{ BuffType::DamageReduction, 0.8f }; // 20% less taken
    case BuffType::SpeedBoost:
        return Buff{ BuffType::SpeedBoost, 1.5f }; // 1.5x speed
    case BuffType::JumpBoost:
        return Buff{ BuffType::JumpBoost, 1.5f };  // 1.5x jump
    case BuffType::HealthRegen:
        return Buff{ BuffType::HealthRegen, 0.0f }; // enable regen
    default:
        return Buff{ BuffType::ExtraHealth, 10.0f };
    }
}

#endif // BUFF_H
