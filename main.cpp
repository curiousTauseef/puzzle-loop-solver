#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>

#include "puzzle.h"
#include "puzzle_solver.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "No puzzle file\n";
        return -1;
    }
    string output_file_name = "solution.txt";
    if (argc == 3)
    {
        output_file_name = argv[2];
    }
    ifstream puzzle_file(argv[1]);
    ofstream of(output_file_name);

    puzzle_solver ps;
    ps.read_puzzle(puzzle_file);
    ps.set_output(of);
    int n = ps.solve();
    if (n <= 0)
    {
        cerr << "Invalid puzzle\n";
        return -1;
    }
    cout << "Solutions: " << n << endl;
    return 0;
}
