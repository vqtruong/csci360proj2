//
// Created by Chi Zhang on 8/26/19.
//

#include "PolicyIterationAgent.hpp"
#include "common.hpp"
#include <cmath>
#include <cstdlib>
#include <cfloat>


PolicyIterationAgent::PolicyIterationAgent(const FrozenLakeMDP &mdp, double gamma, int iterations, double threshold) :
        ValueEstimateAgent(gamma, iterations, threshold), m_mdp(mdp) {
    MSG("Training Policy Iteration Agent on " << m_mdp.getName());
    MSG("Initializing Policy Iteration Agent");
    initialize();
    MSG("Solving...");
    solve();
}

double PolicyIterationAgent::getQValue(const GameState &state, const Action &action) {
    auto newValues = evaluateCurrentPolicy();
    double tempQ = 0;
    std::map<GameState, double> probabilities = m_mdp.getTransitionStatesAndProbs(state, action);
    for (auto prob : probabilities) {
        tempQ += prob.second * 
                        (m_mdp.getReward(state, action, prob.first) + (m_gamma * newValues[prob.first]));    
    }
    return tempQ;
}

double PolicyIterationAgent::getValue(const GameState &state) {
    auto ret = evaluateCurrentPolicy();
    return ret[state];
}

Action PolicyIterationAgent::getPolicy(const GameState &state) {
    return m_policy[state];
}

/*
 * Evaluate the current policy by returning V(s), which is represented as a map,
 * where key is GameState and value is double.
 */
std::map<GameState, double> PolicyIterationAgent::evaluateCurrentPolicy() {
    std::map<GameState, std::map<Action, double>> Q;
    std::map<GameState, double> values;
    std::map<GameState, Action> policy;
    auto states = m_mdp.getStates();

    for (auto state : states) {
        values[state] = 0;
    }

    for (int i = 0; 1; ++i) {
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
            return values;
        }
    }
    return values;
}

void PolicyIterationAgent::solve() {
    auto states = m_mdp.getStates();
    for (int i = 0; i < m_iterations; ++i) {
        auto newValues = evaluateCurrentPolicy();
        bool unchanged = true;
        
        for (auto state : states) {
            if (m_mdp.isTerminal(state)) {
                continue;
            }
            double max = -1 * DBL_MAX;
            Action maxAction = LEFT;
            auto nextActions = m_mdp.getPossibleActions(state);
            for (auto action : nextActions) {
                double tempQ = 0;
                std::map<GameState, double> probabilities = m_mdp.getTransitionStatesAndProbs(state, action);
                for(auto prob : probabilities) {
                    tempQ += prob.second * 
                        (m_mdp.getReward(state, action, prob.first) + (m_gamma * newValues[prob.first]));
                }
                if (max < tempQ) {
                    max = tempQ;
                    maxAction = action;
                }

            }

            double current = 0;
            std::map<GameState, double> probabilities = m_mdp.getTransitionStatesAndProbs(state, m_policy[state]);
            for (auto prob : probabilities) {
                current += prob.second * 
                        (m_mdp.getReward(state, m_policy[state], prob.first) + (m_gamma * newValues[prob.first]));
            }

            if (max > current) {
                if (m_policy[state] != maxAction) 
                    unchanged = false;
                m_policy[state] = maxAction;
            } 
        }
        if (unchanged)
            return;
        
    }

}

void PolicyIterationAgent::initialize() {
    for (auto state : m_mdp.getStates()) {
        auto nextActions = m_mdp.getPossibleActions(state);
        if (!nextActions.empty())
            m_policy[state] = nextActions[rand() % nextActions.size()];
    }
}


