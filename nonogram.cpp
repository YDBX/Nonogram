#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "nonogram.h"
using namespace std;

vector<int> split(string s) {
    vector<int> nums;

    size_t pos = 0;
    string token;
    while ((pos = s.find(" ")) != string::npos) {
        token = s.substr(0, pos);
        nums.push_back(stoi(token));
        s.erase(0, pos + 1);
    }
    nums.push_back(stoi(s));
    return nums;
}

void
nonogram::read(char* fn) {
    ifstream inFile;
    inFile.open(fn);

    string str;
    vector<int> nums;
    if (inFile.is_open()) {
        for (int i = 0; getline(inFile, str); ++i) {
            nums = split(str);
            if (i == 0) {
                nRow = nums[0];
                nCol = nums[1];
            }
            else if (i <= nRow)
                rClues.push_back(nums);
            else
                cClues.push_back(nums);
        }
    }
}

void
nonogram::write() {
    vector<int> preSumR;
    vector<int> preSumC;
    preSumR.reserve(nRow);
    int sum = nRow * nCol + 1;
    for (int i = 0; i < nRow; ++i) {
        preSumR[i] = sum;
        sum += rClues[i].size() * (nCol - rClues[i][rClues[i].size() - 1] + 1);
        cout << preSumR[i] << " ";
    }
    cout << endl;
    preSumC.reserve(nCol);
    for (int j = 0; j < nCol; ++j) {
        preSumC[j] = sum;
        sum += cClues[j].size() * (nRow - cClues[j][cClues[j].size() - 1] + 1);
        cout << preSumC[j] << " ";
    }
    cout << endl;
    
    ofstream cnf;
    cnf.open("test.cnf");

    // calculate the number of variable and clauses
    int nVar = sum - 1, nClauses = 0;
    // nClauses = (a_0+...a_{m-1})+(b_0+...+b_{n-1}) + 

    cnf << "p cnf " << nVar << " " << nClauses << "\n";

    // range of left-most position (number the clauses: a_0+a_1+...a_{m-1})
    for (int i = 0; i < nRow; ++i) {
        for (size_t k = 0; k < rClues[i].size(); ++k) {
            for (int l = 0; l <= nCol - rClues[i][rClues[i].size() - 1]; ++l)
                cnf << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
            cnf << "0\n";
        }
    }

    // unique (a_0 * (n-r_{0, a_0-1})(n-r_{0, a_0-1} + 1) / 2 + ...)
    for (int i = 0; i < nRow; ++i)
        for (size_t k = 0; k < rClues[i].size(); ++k)
            for (size_t l = 0; l < size_t(nCol) - rClues[i][rClues[i].size() - 1]; ++l)
                for (size_t m = l + 1; m <= size_t(nCol) - rClues[i][rClues[i].size() - 1]; ++m)
                    cnf << "-" << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " -" << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + m << " 0\n";

    // range of top-most position (number the clauses: b_0+b_1+...b_{n-1})
    for (int j = 0; j < nCol; ++j) {
        for (size_t k = 0; k < cClues[j].size(); ++k) {
            for (size_t l = 0; l <= size_t(nRow) - cClues[j][cClues[j].size() - 1]; ++l)
                cnf << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
            cnf << "0\n";
        }
    }

    // unique (b_0 * (m-c_{0, b_0-1})(m-c_{0, b_0-1} + 1) / 2 + ...)
    for (int j = 0; j < nCol; ++j)
        for (size_t k = 0; k < cClues[j].size(); ++k)
            for (size_t l = 0; l < size_t(nRow) - cClues[j][cClues[j].size() - 1]; ++l)
                for (size_t m = l + 1; m <= size_t(nRow) - cClues[j][cClues[j].size() - 1]; ++m)
                    cnf << "-" << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " -" << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + m << " 0\n";

    // overlap
    for (int i = 0; i < nRow; ++i) {
        for (size_t k = 0; k < rClues[i].size() - 1; ++k) {
            // r_{i, k} + l + 1 <= n - r_{i, a_i-1}
            // r_{i, k} + l < n - r_{i, a_i - 1}
            // l < n - r_{i, a_i - 1} - r_{i, k}
            for (int l = 0; l < nCol - rClues[i][rClues[i].size() - 1] - rClues[i][k]; ++l) {
                cnf << "-" << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
                for (int m = l + rClues[i][k] + 1; m <= nCol - rClues[i][rClues[i].size() - 1]; ++m)
                    cnf << preSumR[i] + (k + 1) * (nCol - rClues[i][rClues[i].size() - 1] + 1) + m << " ";
                cnf << "0\n";
            }
        }
        
        for (size_t k = 1; k < rClues[i].size(); ++k) {
            for (int l = 0; l < nCol - rClues[i][rClues[i].size() - 1] + 1; ++l) {
                cnf << "-" << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
                for (int m = 0; m < l - rClues[i][k - 1]; ++m) {
                    cnf << preSumR[i] + (k - 1) * (nCol - rClues[i][rClues[i].size() - 1] + 1) + m << " ";
                }
                cnf << "0\n";
            }
        }
    }

    for (int j = 0; j < nCol; ++j) {
        for (size_t k = 0; k < cClues[j].size() - 1; ++k) {
            // r_{i, k} + l + 1 <= r_{i, a_i-1}
            for (int l = 0; l < nRow - cClues[j][cClues[j].size() - 1] - cClues[j][k]; ++l) {
                cnf << "-" << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
                for (int m = l + cClues[j][k] + 1; m <= nRow - cClues[j][cClues[j].size() - 1]; ++m) {
                    cnf << preSumC[j] + (k + 1) * (nRow - cClues[j][cClues[j].size() - 1] + 1) + m << " ";
                }
                cnf << "0\n";
            }
        }

        for (size_t k = 1; k < cClues[j].size(); ++k) {
            for (int l = 0; l < nCol - cClues[j][cClues[j].size() - 1] + 1; ++l) {
                cnf << "-" << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
                for (int m = 0; m < l - cClues[j][k - 1]; ++m) {
                    cnf << preSumC[j] + (k - 1) * (nRow - cClues[j][cClues[j].size() - 1] + 1) + m << " ";
                }
                cnf << "0\n";
            }
        }
    }

    // colored (Rightarrow) 2mn
    for (int i = 0; i < nRow; ++i) {
        for (int j = 0; j < nCol; ++j) {
            cnf << "-" << i * nCol + j + 1 << " ";
            for (size_t k = 0; k < rClues[i].size(); ++k) {
                for (int l = max(0, j - rClues[i][k] + 1); l <= min(j, nCol - rClues[i][rClues[i].size() - 1]); ++l) {
                    cnf << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
                }
            }
            cnf << "0\n";
        }
    }

    for (int i = 0; i < nRow; ++i) {
        for (int j = 0; j < nCol; ++j) {
            cnf << "-" << i * nCol + j + 1 << " ";
            for (size_t k = 0; k < cClues[j].size(); ++k) {
                for (int l = max(0, i - cClues[j][k] + 1); l <= min(i, nRow - cClues[j][cClues[j].size() - 1]); ++l) {
                    cnf << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
                }
            }
            cnf << "0\n";
        }
    }

    // colored (Leftarrow)
    for (int i = 0; i < nRow; ++i) {
        for (int j = 0; j < nCol; ++j) {
            for (size_t k = 0; k < rClues[i].size(); ++k) {
                cnf << i * nCol + j + 1 << " ";
                for (int l = 0; l <= j - rClues[i][k]; ++l)
                    cnf << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
                for (int l = j + 1; l <= nCol - rClues[i][rClues[i].size() - 1]; ++l)
                    cnf << preSumR[i] + k * (nCol - rClues[i][rClues[i].size() - 1] + 1) + l << " ";
                cnf << "0\n";
            }
        }
    }

    for (int i = 0; i < nRow; ++i) {
        for (int j = 0; j < nCol; ++j) {
            for (size_t k = 0; k < cClues[j].size(); ++k) {
                cnf << i * nCol + j + 1 << " ";
                for (int l = 0; l <= i - cClues[j][k]; ++l)
                    cnf << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
                for (int l = i + 1; l <= nRow - cClues[j][cClues[j].size() - 1]; ++l)
                    cnf << preSumC[j] + k * (nRow - cClues[j][cClues[j].size() - 1] + 1) + l << " ";
                cnf << "0\n";
            }
        }
    }

    cnf.close();
}

void
nonogram::show() {
    ifstream file;
    file.open("result.log");
    string str;
    if (file.is_open()) {
        for (int i = 0; getline(file, str); ++i) {
            if (i == 0 && str == "SAT"){
                continue;
            }
            else if (i == 0 && str == "UNSAT") {
                break;
            }
            else if (i == 1) {
                vector<int> nums = split(str);
                for (int j = 0; j < nRow; ++j) {
                    for (int k = 0; k < nCol; ++k) {
                        if (nums[j * nCol + k] > 0)
                            cout << "■ ";
                        else
                            cout << "□ ";
                        // cout << setw(5) << nums[j * nCol + k] << " ";
                    }
                    cout << "\n";
                }
            }
        }
    }
}