#include <iostream>
#include <vector>
using namespace std;

class nonogram {

    public:
        void read(char*);
        void write();
        void show();

    private:
        int                     nRow;
        int                     nCol;
        vector<vector<int>>     rClues;
        vector<vector<int>>     cClues;
};