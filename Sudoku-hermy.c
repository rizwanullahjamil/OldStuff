#include <iostream.h>
#include <ctype.h>
#include <algorithm.h>
#include <vector.h>
#include <cmath.h>

using namespace std;

#define PUZZLE_SIZE 16

// Rrint the puzzle out
// Returns: None
void print_puzzle(int **puzzle, int size) {
for (int j = 0; j < size; j++) {
for (int k = 0; k < size - 1; k++) {
printf("%i ", puzzle[j][k]);
}
printf("%i\n", puzzle[j][size-1]);
}
}

// Find valid entries for x,y in puzzle
// Returns: A vector<int> with valid numbers to put in x,y in puzzle[][]
vector<int> find_available(int **puzzle, int size, int x, int y) {
vector<int> ret;
vector<bool> used(size+1, false);
for (int i = 0; i < size; i++)
used[puzzle[x][i]] = true;
for (int i = 0; i < size; i++)
used[puzzle[i][y]] = true;
int rsize = (int) sqrt(size);
for (int i = 0; i < rsize; i++) {
for (int j = 0; j < rsize; j++) {
used[puzzle[rsize*(x/rsize)+j][rsize*(y/rsize)+i]] = true;
}
}

for (unsigned int i = 1; i < used.size(); i++) {
if (!used[i]) {
ret.push_back(i);
}
}
return ret;
}

// Find the empty square with the least amount of available entries.
// Returns: A pair<int, int> representing the x,y coordinates of the entry in
// the puzzle - If the current puzzle has a conflic, returns -2,-2 If there are
// no more empty squares in the puzzle, returns -1,-1
pair<int, int> find_next_pos(int **puzzle, int size) {
unsigned int best = size+1;
vector<int> r;
pair<int, int> ret(-1, -1);
for (int i = 0; i < size; i++) {
for (int j = 0; j < size; j++) {
if (puzzle[i][j] != 0)
continue;
r = find_available(puzzle, size, i, j);
if (r.size() == 0) {
return pair<int, int>(-2, -2);
}
if (r.size() < best) {
best = r.size();
ret = pair<int, int>(i, j);
}
}
}
return ret;
}

// recursive function to solve the puzzle, it works by finding the empty square
// with the least amount of possible solutions and then tries placing the
// possible solutions in that entry one at a time and trying to solve the
// resulting puzzle.
// Returns: True if the puzzle has been solved, else returns false
bool solve_puzzle(int **puzzle, int size) {
pair<int, int> r;
r = find_next_pos(puzzle, size);
// Is this a valid puzzle?
if (r.first == -2) {
return false;
}
// Was the puzzle solved?
if (r.first == -1) {
print_puzzle(puzzle, size);
return true;
}

// Place entries
vector<int> available = find_available(puzzle, size, r.first, r.second);

for (unsigned int i = 0; i < available.size(); i++) {
puzzle[r.first][r.second] = available[i];
if (solve_puzzle(puzzle, size))
return true;
}
puzzle[r.first][r.second] = 0;
return false;
}



int main(int argc, char **argv) {
int puzzle_size = PUZZLE_SIZE;

// create a puzzle matrix
int *puzzle[puzzle_size];
for (int i = 0; i < puzzle_size; i++) {
puzzle[i] = (int *) malloc(sizeof(int) * puzzle_size);
}


// read the input into our puzzle matrix
int value;
for (int i = 0; i < puzzle_size; i++) {
for (int j = 0; j < puzzle_size; j++) {
cin >> value;
puzzle[i][j] = value;
}
}

// print the puzzle matrix and its solution
print_puzzle(puzzle, puzzle_size);
printf("Solution:\n---\n");
if (!solve_puzzle(puzzle, puzzle_size))
printf("impossible\n");

return 0;
}
