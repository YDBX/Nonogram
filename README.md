# Nonogram
This is a SAT-based Nonogram Solver.

## How to use
 - **Install MiniSAT**
 ```
    sudo apt install minisat
 ```
 - **Solve Nonogram**
 ```
    bash test.sh <input_file>
 ```
 The input file records the number of rows and columns and clues of a Nonogram.
 The following is an example of the input file (./inputs/5x5.in). The first line is
 the number of rows (n) and columns (m), and the next n (n=5) lines are the clues of each row, and the last m (m=5) lines are the clues of each column.
 ```
 5 5
 3
 3
 1 1
 3
 2
 1
 1 3
 2 1
 4
 1
 ```
