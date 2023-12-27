#include <iostream>
#include <vector>
#include <fstream>
#include "nonogram.h"

using namespace std;


int main(int argc, char** argv) {
    nonogram nonoSolver;
    char* fn = argv[1];
    ifstream file;
    file.open("result.log");
    nonoSolver.read(fn);
    if (file.is_open()) {
        nonoSolver.show();
    } else {
        nonoSolver.write();
    }
    return 0;
}