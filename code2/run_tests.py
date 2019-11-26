"""
This script is meant to simplify the process of running the input files for project 2.

To run the script use:
python3 run_tests.py <command> <input_directory>

command - The command that will be used to process the input files.
input_directory - The directory containing the input files.

Examples:

python3 run_tests.py ./task3/task3 task3 
 - This runs CBS on all of the input files in test3.

python3 run_tests.py ./task2/task2 task3
 - This runs prioritized planning on all of the input files in test3.

"""


from visualize import import_paths, import_mapf_instance
from conflict_test import check_conflicts
import sys, time, subprocess, os


# This should match the begining of the names of the input files you are running the algorithm against.
FILE_NAME_BASE = "exp3_{}"
#FILE_NAME_BASE = "test_{}"

def main(base_directory, executable_file):

    path_lengths = []
    run_times = []
    conflicts = []

    for i in range(1, 51):

        base_file_name = FILE_NAME_BASE.format(i)
        input_file_path = os.path.join(base_directory, "{}.txt".format(base_file_name))
        output_directory = os.path.join(base_directory, "output")

        output_directory_path = os.path.abspath(output_directory)
        if not os.path.exists(output_directory_path):
            try:
                os.makedirs(output_directory_path)
            except OSError:
                print ("Creation of the directory {} failed".format(output_directory_path))

        output_file = os.path.join(output_directory, "{}_out.txt".format(base_file_name))

        if os.path.exists(output_file):
            os.remove(output_file)

        start_time = time.time()

        # If you want to view the output of the executable, comment the line below and add:
        # p = subprocess.Popen([executable_file, input_file_path, output_file])
        p = subprocess.Popen([executable_file, input_file_path, output_file], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
        
        p.wait()

        end_time = time.time()
        milliseconds = int(round((end_time - start_time) * 1000))

        if os.path.exists(output_file):

            current_map, starts, goals = import_mapf_instance(input_file_path)
            paths = import_paths(output_file, len(current_map[0]))

            result = check_conflicts(paths, current_map)
            if result:
                print("Conflict found in input {}".format(i))
                conflicts.append(i)

            path_length = 0
            for path in paths:
                path_length += len(path)-1
        else:
            path_length = -1

        path_lengths.append(path_length)
        run_times.append(milliseconds)


    if len(conflicts) == 0:
        print("No conflicts found!")
    else:
        print("Conflicts found in tests {}".format(conflicts))

    print("Path lengths:")
    lengths = ""
    for x in path_lengths:
        lengths += "{}, ".format(x)
    print(lengths[:-1])

    print("Times:")
    times = ""
    for x in run_times:
        times += "{}, ".format(x)
    print(times[:-1])


if __name__ == "__main__":
    main(sys.argv[2], sys.argv[1])