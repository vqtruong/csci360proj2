//
// Created by Chi Zhang on 8/27/19.
//

#include "common.hpp"
#include "third_party/random.hpp"
#include "QLearningAgent.hpp"
#include <iostream>
#include <fstream>

QLearningAgent::QLearningAgent(FrozenLakeEnv &env, double gamma, int iterations, double alpha, double epsilon) :
        ValueEstimateAgent(gamma, iterations, 0.0), m_alpha(alpha), m_epsilon(epsilon), m_env(env) {
    MSG("Training Q Learning Agent on " << m_env.getName());
    MSG("Initializing Q Learning Agent");
    initialize();
    MSG("Solving...");
    solve();
}

double QLearningAgent::getValue(const GameState &state) {
    auto actions = m_env.getPossibleActions(state);
    if (actions.empty()) {
        return 0;
    }

    double max = std::numeric_limits<double>::max() * -1;
    for (auto action : actions) {
        double tempQ = getQValue(state, action);
        if (tempQ > max) {
            max = tempQ;
        }
    }
    return max;
}

double QLearningAgent::getQValue(const GameState &state, const Action &action) {
    if (Q.find(state) == Q.end()) {
        frequency[state][action] = 1;
        Q[state][action] = 0;
    }
    else if (Q[state].find(action) == Q[state].end()){
        frequency[state][action] = 1;
        Q[state][action] = 0;
    }

    // Return for epsilon greedy
    // return Q[state][action]


    // Return for counting based
    return Q[state][action] + m_epsilon * 1 / sqrt(frequency[state][action]++);


}

// The final policy without exploration. Used for evaluation.
Action QLearningAgent::getPolicy(const GameState &state) {
    auto actions = m_env.getPossibleActions(state);
    double max = std::numeric_limits<double>::max() * -1;
    Action maxAction;

    for (auto action : actions) {
        double tempQ = getQValue(state, action);
        if (tempQ > max) {
            max = tempQ;
            maxAction = action;
        }
    }
    return maxAction;
}

// you should use getAction in solve instead of getPolicy and implement your exploration strategy here.
Action QLearningAgent::getAction(const GameState &state) {
    // COMMENT OUT BELOW TO GET EPSILON GREEDY ALGORITHM
    /* 
    std::random_device rando;
    std::default_random_engine generator{rando()};
    auto actions = m_env.getPossibleActions(state);

    std::uniform_real_distribution<double> distribution{0.0,1.0};
    std::uniform_int_distribution<int> d2{0, static_cast<int>(actions.size())-1};

    auto rng = distribution(generator);
    if (rng < m_epsilon) {
        return actions[d2(generator)];
    }
    else {
        return getPolicy(state);
    }
    */

    return getPolicy(state);

}
// Concerns: 
void QLearningAgent::update(const GameState &state, const Action &action, const GameState &nextState, double reward) {
    // FOR COUNTING BASED, UNCOMMENT CODE BELOW
    getQValue(state, action);
    
    Q[state][action] += m_alpha*
        (reward + (m_gamma  * getValue(nextState)) - getQValue(state, action));
}

void QLearningAgent::solve() {
    // output a file for plotting
    std::ofstream outFile;
    outFile.open("result.csv");
    outFile << "Episode,Reward" << std::endl;

    int maxEpisodeSteps = 100;
    // collect m_iterations trajectories for update
    for (int i = 0; i < m_iterations; i++) {
        int numSteps = 0;
        GameState state = m_env.reset();
        while (!m_env.isTerminal(state)) {
            Action action = getAction(state);
            GameState nextState = m_env.getNextState(state, action);
            double reward = m_env.getReward(state, action, nextState);

            update(state, action, nextState, reward);
            state = nextState;
            numSteps += 1;
            if (numSteps >= maxEpisodeSteps) break;  // avoid infinite loop in some cases.
        }
        // evaluate for 100 episodes using the current optimal policy. You can't change this line.
        double episodeReward = m_env.runGame(*this, 100, m_gamma, false).first;
        std::cout << "Evaluating episode reward at learning iteration " << i << " is " << episodeReward << std::endl;
        outFile << i << "," << episodeReward << std::endl;
    }
    outFile.close();
}

void QLearningAgent::initialize() {
    // TODO
    return;
}
