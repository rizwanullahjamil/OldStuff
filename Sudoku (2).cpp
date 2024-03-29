#include <iostream>
#include <fstream>

using namespace std;

class SudokuBoard;
void printB(SudokuBoard sb);

typedef unsigned int uint;

const uint MAXVAL = 9;
const uint L = 9;
const uint C = 9;
const uint S = L * C;
const uint ZONEL = 3;
const uint ZONEC = 3;
const uint ZONES = ZONEL * ZONEC;

const uint lineElements[L][C] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8},
    { 9, 10, 11, 12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23, 24, 25, 26},
    {27, 28, 29, 30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41, 42, 43, 44},
    {45, 46, 47, 48, 49, 50, 51, 52, 53},
    {54, 55, 56, 57, 58, 59, 60, 61, 62},
    {63, 64, 65, 66, 67, 68, 68, 70, 71},
    {72, 73, 74, 75, 76, 77, 78, 79, 80}
};

const uint columnElements[C][L] = {
    { 0,  9, 18, 27, 36, 45, 54, 63, 72},
    { 1, 10, 19, 28, 37, 46, 55, 64, 73},
    { 2, 11, 20, 29, 38, 47, 56, 65, 74},
    { 3, 12, 21, 30, 39, 48, 57, 66, 75},
    { 4, 13, 22, 31, 40, 49, 58, 67, 76},
    { 5, 14, 23, 32, 41, 50, 59, 68, 77},
    { 6, 15, 24, 33, 42, 51, 60, 69, 78},
    { 7, 16, 25, 34, 43, 52, 61, 70, 79},
    { 8, 17, 26, 35, 44, 53, 62, 71, 80}
};

const uint zoneElements[S / ZONES][ZONES] = {
    { 0,  1,  2,  9, 10, 11, 18, 19, 20},
    { 3,  4,  5, 12, 13, 14, 21, 22, 23},
    { 6,  7,  8, 15, 16, 17, 24, 25, 26},
    {27, 28, 29, 36, 37, 38, 45, 46, 47},
    {30, 31, 32, 39, 40, 41, 48, 49, 50},
    {33, 34, 35, 42, 43, 44, 51, 52, 53},
    {54, 55, 56, 63, 64, 65, 72, 73, 74},
    {57, 58, 59, 66, 67, 68, 75, 76, 77},
    {60, 61, 62, 68, 70, 71, 78, 79, 80}
};

class SudokuBoard {
public:
    SudokuBoard() :
        filledIn(0)
    {
        for (uint i(0); i < S; ++i)
            table[i] = usedDigits[i] = 0;
    }
    virtual ~SudokuBoard() {
    }

    int const at(uint l, uint c) { // Returns the value at line l and row c
        if (isValidPos(l, c))
            return table[l * L + c];
        else
            return -1;
    }

    void set(uint l, uint c, uint val) { // Sets the cell at line l and row c to hold the value val
        if (isValidPos(l, c) && ((0 < val) && (val <= MAXVAL))) {
            if (table[l * C + c] == 0)
                ++filledIn;
            table[l * C + c] = val;
            for (uint i = 0; i < C; ++i) // Update lines
                usedDigits[lineElements[l][i]] |= 1<<val;
            for (uint i = 0; i < L; ++i) // Update columns
                usedDigits[columnElements[c][i]] |= 1<<val;
            int z = findZone(l * C + c);
            for (uint i = 0; i < ZONES; ++i) // Update columns
                usedDigits[zoneElements[z][i]] |= 1<<val;
        }
    }

    void solve() {
        try { // This is just a speed boost
            scanAndSet(); // Logic approach
            goBruteForce(); // Brute force approach
        } catch (int e) { // This is just a speed boost
        }
    }

    void scanAndSet() {
        int b;
        bool changed(true);
        while (changed) {
            changed = false;
            for (uint i(0); i < S; ++i)
                if (0 == table[i]) // Is there a digit already written?
                    if ((b = bitcount(usedDigits[i])) == MAXVAL - 1) { // If there's only one digit I can place in this cell, do
                        int d(1); // Find the digit
                        while ((usedDigits[i] & 1<<d) > 0)
                            ++d;
                        set(i / C, i % C, d); // Fill it in
                        changed = true; // The board has been changed so this step must be rerun
                    } else if (bitcount(usedDigits[i]) == MAXVAL)
                        throw 666; // Speed boost
        }
    }

    void goBruteForce() {
        int max(-1); // Find the cell with the _minimum_ number of posibilities (i.e. the one with the largest number of /used/ digits)
        for (uint i(0); i < S; ++i)
            if (table[i] == 0) // Is there a digit already written?
                if ((max == -1) || (bitcount(usedDigits[i]) > bitcount(usedDigits[max])))
                    max = i;
        if (max != -1) {
            for (uint i(1); i <= MAXVAL; ++i) // Go through each possible digit
                if ((usedDigits[max] & 1<<i) == 0) { // If it can be placed in this cell, do
                    SudokuBoard temp(*this); // Create a new board
                    temp.set(max / C, max % C, i); // Complete the attempt
                    temp.solve(); // Solve it
                    if (temp.getFilledIn() == S) { // If the board was completely solved (i.e. the number of filled in cells is S)
                        for (uint j(0); j < S; ++j) // Copy the board into this one
                            set(j / C, j % C, temp.at(j / C, j % C));
                        return; // Break the recursive cascade
                    }
                }
        }
    }

    uint getFilledIn() {
        return filledIn;
    }

private:
    uint table[S];
    uint usedDigits[S];
    uint filledIn;

    bool const inline isValidPos(int l, int c) {
        return ((0 <= l) && (l < (int)L) && (0 <= c) && (c < (int)C));
    }

    uint const inline findZone(uint off) {
        return ((off / C / ZONEL) * (C / ZONEC) + (off % C / ZONEC));
    }

    uint const inline bitcount(uint x) {
        uint count(0);
        for (; x; ++count, x &= (x - 1));
        return count;

    }
};

void printB(SudokuBoard sb) {
    cout << "  |  -------------------------------  |" << endl;
    for (uint i(0); i < S; ++i) {
        if (i % 3 == 0)
            cout << "  |";
        cout << "  " << sb.at(i / L, i % L);
        if (i % C == C - 1) {
            if (i / C % 3 == 2)
                cout << "  |" << endl << "  |  -------------------------------";
            cout << "  |" << endl;
        }
    }
    cout << endl;
}

int main(int argc, char *argv[]) {
    SudokuBoard sb;

    ifstream fin("sudoku4.in");
    int aux;
    for (uint i(0); i < S; ++i) {
        fin >> aux;
        sb.set(i / L, i % L, aux);
    }
    fin.close();

    printB(sb);
    sb.solve();
    printB(sb);

    return 0;
}
