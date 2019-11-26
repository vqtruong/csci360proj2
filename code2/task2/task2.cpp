#include <iostream>
#include <fstream>
#include "MAPFInstance.h"
#include "AStarPlanner.h"
#include <tuple>
#include <chrono>
using namespace std;
using namespace std::chrono;

int testOnce(int argc, char* argv[]);

void testAll() {
    std::string total, timeString;
    for (int i = 1; i <= 50; ++i) {
        auto start = high_resolution_clock::now();
        clock_t c_start = clock();
        MAPFInstance ins;
        string input_file = "../task3/exp3_" + to_string(i) + ".txt";
        string output_file = "exp2_" + to_string(i) + "_paths.txt";
        // string hwDataFile = 
        if (ins.load_instance(input_file)) {
            // ins.print_instance();
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
            // priorities.push_back(ins.num_of_agents - i - 1);
            priorities.push_back(i);
        }

        // plan paths
        list<Constraint> constraints;
        int count = 1;
        bool noBreak = true;
        for (int i : priorities) {
            // TODO: Transform already planned paths into constraints
            //  Replace the following line with something like paths[i] = a_star.find_path(i, constraints);
            paths[i] = a_star.find_path(i, constraints, count++);
            if (paths[i].empty()) {
                // cout << "Fail to find any solutions for agent " << i << endl;
                noBreak = false;
                break;

                // return 0;
            }

            // Add constraints for future agents
            for (int k : priorities) {
                if (k != i) {
                    for(int j = 0; j < paths[i].size(); ++j) {
                        // Vertex Constraints
                        // cout << "Agent " << k << " cannot be at " << paths[i][j] << " at time " << j << endl;
                        constraints.push_back(make_tuple(k, paths[i][j], -1, j));

                        // Add edge constraints from a->b and b->a
                        if (j != 0) {
                            // cout << "Agent " << k << " cannot go to " << paths[i][j-1] << " from " << paths[i][j] << " at time " << j << endl;
                            constraints.push_back(make_tuple(k, paths[i][j], paths[i][j-1], j));
                            constraints.push_back(make_tuple(k, paths[i][j-1], paths[i][j], j));
                        }
                    }

                    // Block agents indefinitely for the future
                    constraints.push_back(make_tuple(k, paths[i][paths[i].size()-1], -1, ((int)paths[i].size()-1) * -1));
                    // cout << "Agent " << k << " cannot go to " << paths[i][paths[i].size()-1] << " after time " << ((int)paths[i].size()-1)*-1 << endl;
                }
            }
        }

        // print paths
        // cout << "Paths:" << endl;
        int sum = 0;
        if (noBreak) {
            for (int i = 0; i < ins.num_of_agents; i++) {
                // cout << "a" << i << ": " << paths[i] << endl;
                sum += (int)paths[i].size() - 1;
            }
        }
        total += to_string(sum);
        total += ", ";
        // cout << "Sum of cost: " << sum << endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop-start);
        cout << duration.count() << ", ";
        // save paths
        ofstream myfile (output_file.c_str(), ios_base::out);
        if (myfile.is_open()) {
            for (int i = 0; i < ins.num_of_agents; i++) {
                myfile << paths[i] << endl;
            }
            myfile.close();
        } 
        else {
            cout << "Fail to save the paths to " << output_file << endl;
            exit(-1);
        }
    }
    cout << "\nTotal : " << total << endl;
}


int main(int argc, char *argv[]) {
    // testAll(); // comment out for normal functionality
    return testOnce(argc, argv); 
}


int testOnce(int argc, char* argv[]) {

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
        // priorities.push_back(ins.num_of_agents - i - 1);
        priorities.push_back(i);
    }

    // plan paths
    list<Constraint> constraints;
    int count = 1;
    for (int i : priorities) {
        // TODO: Transform already planned paths into constraints
        //  Replace the following line with something like paths[i] = a_star.find_path(i, constraints);
        paths[i] = a_star.find_path(i, constraints, count++);
        if (paths[i].empty()) {
            cout << "Fail to find any solutions for agent " << i << endl;
            return 0;
        }

        // Add constraints for future agents
        for (int k : priorities) {
            if (k != i) {
                for(int j = 0; j < paths[i].size(); ++j) {
                    // Vertex Constraints
                    // cout << "Agent " << k << " cannot be at " << paths[i][j] << " at time " << j << endl;
                    constraints.push_back(make_tuple(k, paths[i][j], -1, j));

                    // Add edge constraints from a->b and b->a
                    if (j != 0) {
                        // cout << "Agent " << k << " cannot go to " << paths[i][j-1] << " from " << paths[i][j] << " at time " << j << endl;
                        constraints.push_back(make_tuple(k, paths[i][j], paths[i][j-1], j));
                        constraints.push_back(make_tuple(k, paths[i][j-1], paths[i][j], j));
                    }
                }

                // Block agents indefinitely for the future
                constraints.push_back(make_tuple(k, paths[i][paths[i].size()-1], -1, ((int)paths[i].size()-1) * -1));
                // cout << "Agent " << k << " cannot go to " << paths[i][paths[i].size()-1] << " after time " << ((int)paths[i].size()-1)*-1 << endl;
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
