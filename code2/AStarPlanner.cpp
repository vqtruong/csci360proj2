#include "AStarPlanner.h"
#include <queue>
#include <unordered_map>
#include <algorithm> 
#include <iostream>
using namespace std;

ostream& operator<<(ostream& os, const Path& path)
{
    for (auto loc : path) {
        os << loc << " ";
    }
    return os;
}

Path AStarPlanner::make_path(const AStarNode* goal_node) const {
    Path path;
    const AStarNode* curr = goal_node;
    while (curr != nullptr) {
        path.push_back(curr->location);
        curr = curr->parent;
    }
    std::reverse(path.begin(),path.end());
    return path;
}

// Returns true if moving from curr to next is constrained
bool AStarPlanner::isConstrained(list<Constraint> constraints, AStarNode* curr, AStarNode* next, int agent_id) const {
    for (auto i = constraints.begin(); i != constraints.end(); ++i) {
        if (get<0>(*i) == agent_id ) {
            // Permanent constraints
            if (get<3>(*i) < 0 && get<3>(*i)*-1 <= curr->timestep && get<1>(*i) == curr->location) {
                // cout << "\tCannot enter because permanent block" << endl;
                return true;
            }
            // Vertex contraints
            if (get<2>(*i) == -1 && get<1>(*i) == next->location && get<3>(*i) == curr->timestep+1)
                return true;
            // Edge constraints
            if (get<1>(*i) == curr->location && get<2>(*i) == next->location && get<3>(*i) == next->timestep)
                return true;
        }
    }
    return false;
}

bool AStarPlanner::isGoalConstrained(list<Constraint> constraints, AStarNode* curr, int agent_id) const {
    for (auto i = constraints.begin(); i != constraints.end(); ++i) {
        if (get<0>(*i) == agent_id ) {
            if (get<2>(*i) == -1 && get<1>(*i) == curr->location && get<3>(*i) >= curr->timestep+1)
                return true;
        }
    }
    return false;
}


Path AStarPlanner::find_path(int agent_id, const list<Constraint>& constraints, int agentPriority) {
    int start_location = ins.start_locations[agent_id];
    int goal_location = ins.goal_locations[agent_id];

    // Open list
    priority_queue<AStarNode*, vector<AStarNode*>, CompareAStarNode> open;

    // Unordered map is an associative container that contains key-value pairs with unique keys.
    // The following unordered map is used for duplicate detection, where the key is the location of the node.
    // unordered_map<int, AStarNode*> all_nodes;

    // Contains all discovered nodes
    unordered_map<pair<int, int>, AStarNode*, hash_pair> all_nodes;

    int h = ins.get_Manhattan_distance(start_location, goal_location); // h value for the root node
    AStarNode* root = new AStarNode(start_location, 0, h, nullptr, 0);
    open.push(root);

    Path path;

    // While there are still nodes to expand
    int max = agentPriority * ins.map_size();
    while (!open.empty()) {
        AStarNode* curr = open.top();
        open.pop();
        cout << curr->location << " for  agent " << agent_id << " at time " << curr->timestep << std::endl;

        // Arrived at goal state
        if (curr->location == goal_location && !isGoalConstrained(constraints, curr, agent_id)) {
            path = make_path(curr);
            break;
        }

        if (curr->timestep > max) {
            cout << "RETUUUURN" << endl;
            return Path();
        }
        // For all children of the current location
        for (int next_location : ins.get_adjacent_locations(curr->location)) {
            // Get a child instance
            auto it = all_nodes.find(make_pair(next_location, curr->timestep+1)); 

            
            // If the child has not been discovered and moving from curr to child is not constrained
            // Push it on open (list of unexpanded nodes)
            if (it == all_nodes.end()) {

                int next_g = curr->g + 1;
                int next_h = ins.get_Manhattan_distance(next_location, goal_location);
                AStarNode* next = new AStarNode(next_location, next_g, next_h, curr, curr->timestep+1);
                bool constrained = isConstrained(mConstraints, curr, next, agent_id);

                if (!constrained) {
                    open.push(next);
                    all_nodes[make_pair(next_location, curr->timestep+1)] = next; // not sure if should be in if
                }
                else
                    delete next;
            }

            // If the node has existed before, we do not need to update the value
            // Because we look by the smallest f values first (f = g + h) 
            // And because g = 1 for all nodes, 
            // f(child) >= f(parent)

            // Note that if the location has been visited before,
            // next_g + next_h must be greater than or equal to the f value of the existing node,
            // because we are searching on a 4-neighbor grid with uniform-cost edges.
            // So we don't need to update the existing node.
        }

        // If staying still is not constrained, add it to the list of to found but unexpanded nodes
        // bool constrained = isConstrained(mConstraints, curr, curr, agent_id);
        // if (!constrained) 
        //     open.push(new AStarNode(curr->location, curr->g+1, curr->h, curr, curr->timestep+1));
    }

    // release memory
    for (auto n : all_nodes)
        delete n.second;

    return path;
}