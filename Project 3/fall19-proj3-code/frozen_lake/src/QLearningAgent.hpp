//
// Created by Chi Zhang on 8/27/19.
//

#ifndef FROZEN_LAKE_QLEARNINGAGENT_HPP
#define FROZEN_LAKE_QLEARNINGAGENT_HPP

#include "LearningAgent.hpp"
#include <random>

class QLearningAgent : public ValueEstimateAgent {
public:
    QLearningAgent(FrozenLakeEnv &env, double gamma, int iterations, double alpha, double epsilon);

    double getValue(const GameState &state) override;

    double getQValue(const GameState &state, const Action &action) override;

    Action getPolicy(const GameState &state) override;

    Action getAction(const GameState &state) override;

    std::string getName() const override {
        return "QLearningAgent";
    }

private:
    /*
     * alpha - learning rate for Q learning
     * epsilon - exploration rate for Q learning
     */

    double m_alpha;
    double m_epsilon;
    FrozenLakeEnv &m_env;

    void update(const GameState &state, const Action &action, const GameState &nextState, double reward);

    void solve();

    void initialize() override;
};


#endif //FROZEN_LAKE_QLEARNINGAGENT_HPP
