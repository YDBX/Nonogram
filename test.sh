make
rm test.cnf result.log
./nonogram $1
minisat test.cnf result.log
./nonogram $1
rm test.cnf result.log