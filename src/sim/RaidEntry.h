#pragma once

#include <string>
#include <vector>

class Player;

// RaidEntry represents a single raid attempt against a specific boss.
// It records the roster used, the outcome, and key summary stats
// that the Guild Master will see in logs and progression views.
class RaidEntry
{
public:
    RaidEntry(int attemptId,
              const std::string &raidName,
              const std::string &bossName,
              const std::vector<Player *> &roster,
              bool success,
              float bossHpPercentRemaining,
              float interruptCoveragePercent,
              bool healerThroughputSufficient,
              bool dpsCheckOnTrack,
              const std::vector<std::string> &deathSummaries,
              const std::vector<std::string> &suggestedAdjustments);

    int GetAttemptId() const;
    const std::string &GetRaidName() const;
    const std::string &GetBossName() const;
    const std::vector<Player *> &GetRoster() const;
    bool WasSuccessful() const;
    float GetBossHpPercentRemaining() const;
    float GetInterruptCoveragePercent() const;
    bool IsHealerThroughputSufficient() const;
    bool IsDpsCheckOnTrack() const;
    const std::vector<std::string> &GetDeathSummaries() const;
    const std::vector<std::string> &GetSuggestedAdjustments() const;

private:
    int attemptId;
    std::string raidName;
    std::string bossName;
    std::vector<Player *> roster;
    bool success;
    float bossHpPercentRemaining;
    float interruptCoveragePercent;
    bool healerThroughputSufficient;
    bool dpsCheckOnTrack;
    std::vector<std::string> deathSummaries;
    std::vector<std::string> suggestedAdjustments;
};

inline RaidEntry::RaidEntry(int attemptId,
                            const std::string &raidName,
                            const std::string &bossName,
                            const std::vector<Player *> &roster,
                            bool success,
                            float bossHpPercentRemaining,
                            float interruptCoveragePercent,
                            bool healerThroughputSufficient,
                            bool dpsCheckOnTrack,
                            const std::vector<std::string> &deathSummaries,
                            const std::vector<std::string> &suggestedAdjustments)
    : attemptId(attemptId),
      raidName(raidName),
      bossName(bossName),
      roster(roster),
      success(success),
      bossHpPercentRemaining(bossHpPercentRemaining),
      interruptCoveragePercent(interruptCoveragePercent),
      healerThroughputSufficient(healerThroughputSufficient),
      dpsCheckOnTrack(dpsCheckOnTrack),
      deathSummaries(deathSummaries),
      suggestedAdjustments(suggestedAdjustments)
{
}

inline int RaidEntry::GetAttemptId() const
{
    return attemptId;
}

inline const std::string &RaidEntry::GetRaidName() const
{
    return raidName;
}

inline const std::string &RaidEntry::GetBossName() const
{
    return bossName;
}

inline const std::vector<Player *> &RaidEntry::GetRoster() const
{
    return roster;
}

inline bool RaidEntry::WasSuccessful() const
{
    return success;
}

inline float RaidEntry::GetBossHpPercentRemaining() const
{
    return bossHpPercentRemaining;
}

inline float RaidEntry::GetInterruptCoveragePercent() const
{
    return interruptCoveragePercent;
}

inline bool RaidEntry::IsHealerThroughputSufficient() const
{
    return healerThroughputSufficient;
}

inline bool RaidEntry::IsDpsCheckOnTrack() const
{
    return dpsCheckOnTrack;
}

inline const std::vector<std::string> &RaidEntry::GetDeathSummaries() const
{
    return deathSummaries;
}

inline const std::vector<std::string> &RaidEntry::GetSuggestedAdjustments() const
{
    return suggestedAdjustments;
}

