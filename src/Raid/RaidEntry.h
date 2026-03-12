#pragma once

#include <string>
#include <vector>

#include "../characters/player.h"
#include "Boss.h"
#include "Raid.h"

/* This is the Raid Entry attempt of an Entire Raid. It is made up of a number of EncounterEntires
 */

class RaidEntry {
public:
    RaidEntry(int attemptId, const std::string& raidName, const std::vector<Player*>& roster,
              bool success);

    int GetAttemptId() const;
    const std::string& GetRaidName() const;
    const std::string& GetBossName() const;
    const std::vector<Player*>& GetRoster() const;
    bool WasSuccessful() const;
    float GetBossHpPercentRemaining() const;
    float GetInterruptCoveragePercent() const;
    bool IsHealerThroughputSufficient() const;
    bool IsDpsCheckOnTrack() const;
    const std::vector<std::string>& GetDeathSummaries() const;
    const std::vector<std::string>& GetSuggestedAdjustments() const;

private:
    int attemptId;
    std::string raidName;
    std::vector<Player*> roster;
    bool success;
};

inline RaidEntry::RaidEntry(int attemptId, const std::string& raidName,
                            const std::vector<Player*>& roster, bool success)
    : attemptId(attemptId), raidName(raidName), roster(roster), success(success) {}

inline int RaidEntry::GetAttemptId() const {
    return attemptId;
}

inline const std::string& RaidEntry::GetRaidName() const {
    return raidName;
}

inline const std::vector<Player*>& RaidEntry::GetRoster() const {
    return roster;
}
