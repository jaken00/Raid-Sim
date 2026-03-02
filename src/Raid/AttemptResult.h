#include <string>
#include <vector>

class AttemptResult {
private:
    /* data */
public:
    AttemptResult(const std::string& bossName, float bossHpPercentRemaining,
                  float interruptCoveragePercent, bool healerThroughputSufficient,
                  bool dpsCheckOnTrack, const std::vector<std::string>& deathSummaries,
                  const std::vector<std::string>& suggestedAdjustments);
    ~AttemptResult();
};

AttemptResult::AttemptResult(const std::string& bossName, float bossHpPercentRemaining,
                             float interruptCoveragePercent, bool healerThroughputSufficient,
                             bool dpsCheckOnTrack, const std::vector<std::string>& deathSummaries,
                             const std::vector<std::string>& suggestedAdjustments) {}

AttemptResult::~AttemptResult() {}
