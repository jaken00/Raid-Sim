#pragma once

#include <string>
#include <vector>

#include "CharacterClass.h"
#include "Spec.h"

// Attitude represents a raider's general mindset and impact on guild culture.
enum class Attitude
{
    Positive,
    Neutral,
    Toxic
};

class Player
{
public:
    Player(int id,
           const std::string &name,
           CharacterClass *characterClass,
           const Spec &spec,
           float itemLevel,
           float performanceRating,
           float attendancePercent,
           Attitude attitude,
           float potential);

    // Accessors
    int GetId() const;
    const std::string &GetName() const;
    CharacterClass *GetCharacterClass() const;
    const Spec &GetSpec() const;
    float GetItemLevel() const;
    float GetPerformanceRating() const;
    float GetAttendancePercent() const;
    Attitude GetAttitude() const;
    float GetPotential() const;
    const std::vector<std::string> &GetRaidHistory() const;

    // Mutators / updaters
    void SetItemLevel(float newItemLevel);
    void SetPerformanceRating(float newPerformanceRating);
    void SetAttendancePercent(float newAttendancePercent);
    void SetAttitude(Attitude newAttitude);

    void AddRaidHistoryEntry(const std::string &entry);

private:
    int id;
    std::string name;
    CharacterClass *characterClass;
    Spec spec;
    float itemLevel;
    float performanceRating;
    float attendancePercent;
    Attitude attitude;
    float potential;
    std::vector<std::string> raidHistory;
};

inline Player::Player(int id,
                      const std::string &name,
                      CharacterClass *characterClass,
                      const Spec &spec,
                      float itemLevel,
                      float performanceRating,
                      float attendancePercent,
                      Attitude attitude,
                      float potential)
    : id(id),
      name(name),
      characterClass(characterClass),
      spec(spec),
      itemLevel(itemLevel),
      performanceRating(performanceRating),
      attendancePercent(attendancePercent),
      attitude(attitude),
      potential(potential)
{
    if (this->attendancePercent < 0.0f)
    {
        this->attendancePercent = 0.0f;
    }
    else if (this->attendancePercent > 100.0f)
    {
        this->attendancePercent = 100.0f;
    }
}

inline int Player::GetId() const
{
    return id;
}

inline const std::string &Player::GetName() const
{
    return name;
}

inline CharacterClass *Player::GetCharacterClass() const
{
    return characterClass;
}

inline const Spec &Player::GetSpec() const
{
    return spec;
}

inline float Player::GetItemLevel() const
{
    return itemLevel;
}

inline float Player::GetPerformanceRating() const
{
    return performanceRating;
}

inline float Player::GetAttendancePercent() const
{
    return attendancePercent;
}

inline Attitude Player::GetAttitude() const
{
    return attitude;
}

inline float Player::GetPotential() const
{
    return potential;
}

inline const std::vector<std::string> &Player::GetRaidHistory() const
{
    return raidHistory;
}

inline void Player::SetItemLevel(float newItemLevel)
{
    itemLevel = newItemLevel;
}

inline void Player::SetPerformanceRating(float newPerformanceRating)
{
    performanceRating = newPerformanceRating;
}

inline void Player::SetAttendancePercent(float newAttendancePercent)
{
    attendancePercent = newAttendancePercent;

    if (attendancePercent < 0.0f)
    {
        attendancePercent = 0.0f;
    }
    else if (attendancePercent > 100.0f)
    {
        attendancePercent = 100.0f;
    }
}

inline void Player::SetAttitude(Attitude newAttitude)
{
    attitude = newAttitude;
}

inline void Player::AddRaidHistoryEntry(const std::string &entry)
{
    raidHistory.push_back(entry);
}

