#include <gtest/gtest.h>

#include "OracleAi.hpp"
#include "RandomAi.hpp"

/*
 * In theory, a highly competent AI can still lose to random chance
 * But in practice, if it beats random more often than not, we can be confident it is working well enough
 * As of writing, it wins 9 out of the 10 trials
 */
TEST(UrAi, OracleShouldPlayBetterThanRandom)
{
    urai::Oracle oracleAi(PLAYER_1);
    urai::Random randomAi;
    const int trialCount = 10;
    int wins[2] = {0, 0};
    for(int i = 0; i < trialCount; i++) {
        std::cout << "Trial " << i << std::flush;
        unsigned int winner = urai::aiVersusAi(oracleAi, randomAi, i);
        wins[urai::aiVersusAi(oracleAi, randomAi, i)] += 1;
        std::cout << ", winner: " << (winner == PLAYER_1 ? "Oracle" : "Random") << std::endl;
    }

    // Oracle should win at least 70% of the time
    ASSERT_GE(wins[PLAYER_1], 7);
}