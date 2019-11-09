#include <iostream>
#include <fstream>
#include "MAPFInstance.h"
#include "AStarPlanner.h"
#include <tuple>

int main(int argc, char *argv[]) {
    MAPFInstance ins;
    string input_file = argv[1];
    string output_file = argv[2];
    if (ins.load_instance(input_file)) {
        ins.print_instance();
    } else {
        cout << "Fail to load the instance " << input_file << endl;
        exit(-1);
    }

    AStarPlanner a_star(ins);
    vector<Path> paths(ins.num_of_agents);

    // assign priority ordering to agents
    // By default, we use the index ordering of the agents where
    // the first always has the highest priority.
    list<int> priorities;
    for (int i = 0; i < ins.num_of_agents; i++) {
        priorities.push_back(ins.num_of_agents - i - 1);
        // priorities.push_back(i);
    }

    // plan paths
    int count = 1;
    for (int i : priorities) {
        // TODO: Transform already planned paths into constraints
        //  Replace the following line with something like paths[i] = a_star.find_path(i, constraints);
        paths[i] = a_star.find_path(i, a_star.mConstraints, count++);
        if (paths[i].empty()) {
            cout << "Fail to find any solutions for agent " << i << endl;
            return 0;
        }

        // Add constraints for future agents
        for (int k : priorities) {
            if (k != i) {
                for(int j = 0; j < paths[i].size(); ++j) {
                    // Vertex Constraints
                    cout << "Agent " << k << " cannot be at " << paths[i][j] << " at time " << j << endl;
                    a_star.mConstraints.push_back(make_tuple(k, paths[i][j], -1, j));

                    // Add edge constraints from a->b and b->a
                    if (j != 0) {
                        cout << "Agent " << k << " cannot go to " << paths[i][j-1] << " from " << paths[i][j] << " at time " << j << endl;
                        a_star.mConstraints.push_back(make_tuple(k, paths[i][j], paths[i][j-1], j));
                        a_star.mConstraints.push_back(make_tuple(k, paths[i][j-1], paths[i][j], j));
                    }
                }

                // Block agents indefinitely for the future
                a_star.mConstraints.push_back(make_tuple(k, paths[i][paths[i].size()-1], -1, ((int)paths[i].size()-1) * -1));
                cout << "Agent " << k << " cannot go to " << paths[i][paths[i].size()-1] << " after time " << ((int)paths[i].size()-1)*-1 << endl;
            }
        }
    }

    // print paths
    cout << "Paths:" << endl;
    int sum = 0;
    for (int i = 0; i < ins.num_of_agents; i++) {
        cout << "a" << i << ": " << paths[i] << endl;
        sum += (int)paths[i].size() - 1;
    }
    cout << "Sum of cost: " << sum << endl;

    // save paths
    ofstream myfile (output_file.c_str(), ios_base::out);
    if (myfile.is_open()) {
        for (int i = 0; i < ins.num_of_agents; i++) {
            myfile << paths[i] << endl;
        }
        myfile.close();
    } else {
        cout << "Fail to save the paths to " << output_file << endl;
        exit(-1);
    }
    return 0;
}