//
// Created by Chi Zhang on 8/24/19.
//

#include "common.hpp"
#include "ValueIterationAgent.hpp"
#include <limits>
#include <cmath>

ValueIterationAgent::ValueIterationAgent(FrozenLakeMDP const &mdp, double gamma, int iterations, double threshold) :
        ValueEstimateAgent(gamma, iterations, threshold), m_mdp(mdp) {
    MSG("Training Value Iteration Agent on " << m_mdp.getName());
    MSG("Initializing Value Iteration Agent");
    initialize();
    MSG("Solving...");
    solve();
}

double ValueIterationAgent::getValue(const GameState &state) {
    return values[state];
}

double ValueIterationAgent::getQValue(const GameState &state, const Action &action) {
    return Q[state][action];
}



Action ValueIterationAgent::getPolicy(const GameState &state) {
    return policy[state];
}


void ValueIterationAgent::solve() {
    std::map<GameState, std::map<Action, double>> Q;
    std::unordered_map<GameState, double> values;
    std::map<GameState, Action> policy;


    for (int i = 0; i < m_iterations; ++i) {
        auto states = m_mdp.getStates();

        // Update Q
        for (auto state : states) {
            auto nextActions = m_mdp.getPossibleActions(state);
            for (auto action : nextActions) {
                double tempQ = 0;
                std::map<GameState, double> probabilities = m_mdp.getTransitionStatesAndProbs(state, action);
                for(auto prob : probabilities) {
                    tempQ += prob.second * 
                        (m_mdp.getReward(state, action, prob.first) + (m_gamma * values[prob.first]));
                }
                Q[state][action] = tempQ;
            }
        } 

        // Update Value
        int lessThanError = 0;
        for (auto state : states) {
            Action bestAction = LEFT;
            if (m_mdp.isTerminal(state)) {
                values[state] = 0;
                continue;
            }
            auto nextActions = m_mdp.getPossibleActions(state);
            double best = std::numeric_limits<double>::max() *-1;
            for (auto action : nextActions) {
                if (best < Q[state][action]) {
                    best = Q[state][action];
                    bestAction = action;
                }
            }
            if (abs(values[state] - best) < m_threshold) {
                lessThanError++;
            }
            values[state] = best;
            policy[state] = bestAction;
        }

        if (lessThanError == states.size()) {
            return;
        }
    }
}

void ValueIterationAgent::initialize() {
    auto states = m_mdp.getStates();
    for (auto it = states.begin(); it != states.end(); ++it) 
        values[*it] = 0;
}




