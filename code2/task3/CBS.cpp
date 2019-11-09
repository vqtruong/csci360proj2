#include "CBS.h"
#include <iostream>
#include <queue>
#include <tuple>
#include <utility>
#include <unordered_map>
using namespace std;

vector<Path> CBS::find_solution() {
    priority_queue<CBSNode*, vector<CBSNode*>, CompareCBSNode> open; // open list

    /* generate the root CBS node */
    auto root = new CBSNode();
    all_nodes.push_back(root);  // whenever generating a new node, we need to
                                // put it into all_nodes
                                // so that we can release the memory properly later in ~CBS()

    // find paths for the root node
    root->paths.resize(a_star.ins.num_of_agents);
    for (int i = 0; i < a_star.ins.num_of_agents; i++) {
        // TODO: if you change the input format of function find_path()
        //  you also need to change the following line to something like
        root->paths[i] = a_star.find_path(i, list<Constraint>(), 1);
        if (root->paths[i].empty()) {
            cout << "Fail to find a path for agent " << i << endl;
            return vector<Path>(); // return "No solution"
        }
    }
    // compute the cost of the root node
    for (const auto& path : root->paths)
        root->cost += (int)path.size() - 1;

    // put the root node into open list
    open.push(root);


    while (!open.empty()) {
        // TODO: implement the high-level of CBS

        // P = node from OPEN with smallest cost
        CBSNode* curr = open.top();
        open.pop();
        curr->print();

        // Find collisions from the paths of P
        Collision collision = findCollision(curr->paths);
        // a1, a2, from, to, time
        
        // If there are no collisions, return P's paths

        if (collision == make_tuple(-1,-1,-1,-1,-1)) {
            cout << "NO COLLISIONS" << endl;
            return curr->paths;
        }

        // constraints = all constraints on P
        vector<Constraint> constraints;

        // Vertex Constraint
        if (get<3>(collision) == -1) {
            constraints.emplace_back(make_tuple(get<1>(collision), get<2>(collision),-1, get<4>(collision)));
            constraints.emplace_back(make_tuple(get<0>(collision), get<2>(collision),-1, get<4>(collision)));
        }

        // Edge Constraints
        else {
            constraints.emplace_back(make_tuple(get<0>(collision), get<2>(collision), get<3>(collision), get<4>(collision)));
            constraints.emplace_back(make_tuple(get<1>(collision), get<3>(collision), get<2>(collision), get<4>(collision)));
        }


        // For each constraint, c
        for (Constraint c: constraints) {
            // Child node
            CBSNode* Q = new CBSNode();
            all_nodes.push_back(Q);

            // The agent being constrained
            int agentA = get<0>(c);
            cout << "\nConstraining agent " << agentA << endl;
            if (get<2>(c) == -1) {
                cout << "\tConsidering New Vertex Constraint: Agent " << get<0>(c)  << " at " << get<1>(c) << " at time " << get<3>(c) << endl;
            }
            else {
                cout << "\tConsidering New Edge Constraint: Agent " << get<0>(c) << " from " << get<1>(c) << " to " << get<2>(c) << " at time " << get<3>(c) << endl;
            }

            // Q's paths will be almost the same as P's except
            // agentA's path will change with the new constraints
            Q->paths = curr->paths;

            // Q's constraints is P's constraint, plus c
            list<Constraint> newConstraints = curr->constraints;
            newConstraints.push_back(c);


            // Find a new path for this agent
            Path path = a_star.find_path(agentA, newConstraints, a_star.ins.num_of_agents); // NOT SURE: if the priority is right
            cout << "\tResulting path: " << path << endl;
            // If there is a path with the new constraints, make a new node with the new constraints
            if (!path.empty()) {
                cout << "\tFound a path with the new constraints!" << endl;
                Q->paths[agentA] = path;
                Q->cost = getCost(Q->paths, a_star.ins);
                cout << "\t\tCost: " << Q->cost << endl;

                int z = 0;
                for (auto p : Q->paths) {
                    cout << "\t\tAgent " << z++ << ": " << p << endl;
                }
                Q->constraints = newConstraints;
                cout << "\t\tChild List of Constraints: " << endl;
                for (auto cons : Q->constraints) {
                    if (get<2>(c) == -1) 
                        cout << "\t\t\tVertex - Agent " << get<0>(cons)  << " at " << get<1>(cons) << " at time " << get<3>(cons) << endl;
                    else 
                        cout << "\t\t\tEdge - Agent " << get<0>(cons) << " from " << get<1>(cons) << " to " 
                        << get<2>(cons) << " at time " << get<3>(cons) << endl;
                }
                
                open.push(Q);
            }            
        }   
    }
    return vector<Path>(); // return "No solution"
}

// Collisions: agentA, agentB, from, to, time
Collision CBS::findCollision(vector<Path> paths) {
    // Key: location, time
    // Value: permanent
    unordered_map<pair<int, int>, int, hash_pair> positions;

    int maxTime = -1;
    for (int i = 0; i < paths.size(); ++i) {
        if (maxTime < (int)paths[i].size()) {
            maxTime = paths[i].size();
        }
        for (int j = 0; j < paths[i].size(); ++j) {
            // Vertex Collision: If there is any agent 
            if (positions.find(make_pair(paths[i][j], j)) != positions.end()) { 
                return make_tuple(i, positions[make_pair(paths[i][j], j)], paths[i][j], -1, j);

            }
            // Edge Collision
            if (j < paths[i].size()-1) {
                auto b = positions.find(make_pair(paths[i][j], j+1)); // The agent at location(i,j) at the next step
                auto a = positions.find(make_pair(paths[i][j+1], j)); //
                if (a != positions.end() && b != positions.end() && a->second == b->second) {
                    return make_tuple(i, positions[make_pair(paths[i][j], j+1)], paths[i][j], paths[i][j+1], j+1);
                }
            }
            // Permanent Vertex Constraints
            else  {
                for (int k = j; k < maxTime; ++k) {
                    if (positions.find(make_pair(paths[i][j], k)) != positions.end()) {
                        return make_tuple(i, positions[make_pair(paths[i][j], k)], paths[i][j], -1, k);
                    }
                }
            }
            positions[make_pair(paths[i][j], j)] = i;


            // cout << "\nCURRENT MAP: " << endl;
            // for (auto it: positions) {
            //     auto p = it.first;
            //     cout << "\tAgent: " << it.second << " | " << p.first << "\t | \t" << p.second << endl;
            // }
        }
    }
    return make_tuple(-1, -1, -1, -1, -1);
}

int CBS::getCost(vector<Path> paths, MAPFInstance ins) {
    int sum = 0;
    for (int i = 0; i < ins.num_of_agents; i++) {
        sum += (int)paths[i].size() - 1;
    }
    return sum;
}

CBS::~CBS() {
    // release the memory
    for (auto n : all_nodes)
        delete n;
}

void CBSNode::print() {
    cout << "\n\n\n___________________________________________________\nNEW NODE" << endl;
    cout << "CONSTRAINTS: " << endl;
    if (!constraints.empty()) {
        for (auto constraint : constraints) {
            if (get<2>(constraint) == -1) {
                cout << "\tVertex for " << get<0>(constraint)  << " at " << get<1>(constraint) << " at time " << get<3>(constraint) << endl;
            }
            else {
                cout << "\tEdge for " << get<0>(constraint) << " from " << get<1>(constraint) << " to " << get<2>(constraint) << " at time " << get<3>(constraint) << endl;
            }
        }
    }
    else {
        cout << "\tEMPTY" << endl;
    }
}
