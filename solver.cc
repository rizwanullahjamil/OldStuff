//
// The following code is public domain (written in 2005 by Nick Smallbone)
// It can be found at http://www.8325.org/sudoku/
//

// It works by keeping track of which numbers can be used to fill the remaining
// cells in each row, column and block. It then will only fill a cell with a
// number that is valid for that row, column and block.

// On every recursion, it chooses the cell with the least possible number
// available to choose from. This has the rather nice effect that impossible
// cells are noticed straight away without any special cases, and cells with
// only one possibility are filled in straight away.

#include <iostream>
#include <cassert>

using namespace std;

// The size of the Sudoku game
static const unsigned blockSize = 3;
static const unsigned gridSize = blockSize * blockSize;

//
// Some functions for representing sets as bitfields
// (counting from 0)
//

typedef unsigned long bitfield;

// No bit we'll be using is above this
static const bitfield maskMax = 1 << gridSize;

// A bitfield with all bits set
static const bitfield allSet = maskMax - 1;

// Returns the size of the set
static unsigned bitCount(bitfield bits) {
	unsigned result = 0;
	bitfield mask = 1;

	while(mask != maskMax) {
		if (bits & mask)
			result++;
		mask *= 2;
	}

	return result;
}

// Returns a bitfield representing {num}
static inline bitfield bitFor(unsigned num) {
	return 1 << (num - 1);
}

class Puzzle {
	// The value of any empty cell (i, j) comes from
	// rows[i] & cols[j] & blocks[i/3][j/3]
	bitfield rows[gridSize];
	bitfield cols[gridSize];
	bitfield blocks[blockSize][blockSize];

	// This contains invalid if the cell is empty,
	// its value otherwise.
	unsigned cells[gridSize][gridSize];

public:
	// This number is invalid, and represents an empty cell
	static const unsigned invalid = gridSize + 1;

	Puzzle() {
		// Better initialise the arrays
		unsigned i, j;

		for (i = 0; i < gridSize; i++) rows[i] = allSet;
		for (i = 0; i < gridSize; i++) cols[i] = allSet;

		for (i = 0; i < blockSize; i++)
			for (j = 0; j < blockSize; j++)
				blocks[i][j] = allSet;

		for (i = 0; i < gridSize; i++)
			for (j = 0; j < gridSize; j++)
				cells[i][j] = invalid;
	}

	// Return the set of values which could be at (i, j)
	inline bitfield candidates(unsigned i, unsigned j) {
		assert(cells[i][j] == invalid);

		return rows[i] & cols[j] & 
			blocks[i/blockSize][j/blockSize];
	}

	// Fill in the cell at (i, j)
	inline void set(unsigned i, unsigned j, unsigned n) {
		bitfield bit = bitFor(n);

		// Check this is a valid value for the cell
		assert((candidates(i, j) & bit) != 0);

		// No other cells in the row, column or block
		// can now have this value
		rows[i] &= ~bit;
		cols[j] &= ~bit;
		blocks[i/blockSize][j/blockSize] &= ~bit;
		cells[i][j] = n;
	}

	// Erase the cell at (i, j)
	inline void unset(unsigned i, unsigned j, unsigned n) {
		bitfield bit = bitFor(n);

		// Make sure it was already set
		assert(cells[i][j] == n);

		rows[i] |= bit;
		cols[j] |= bit;
		blocks[i/blockSize][j/blockSize] |= bit;
		cells[i][j] = invalid;
	}

	// Find the cell with the lowest number of candidates
	// in the specified rectangular area.
	// It searches in iMin <= i < iMax
	// and jMin <= j < jMax
	// Return false if the grid is full.
	bool findMin(unsigned iMin, unsigned iMax, unsigned jMin, unsigned jMax,
		unsigned &outI, unsigned &outJ)
	{
		bool found = false;
		unsigned count = 0;

		for (unsigned i = iMin; i < iMax; i++)
			for (unsigned j = jMin; j < jMax; j++)
				if (cells[i][j] == invalid &&	
					(!found || bitCount(candidates(i, j)) < count))
				{
					count = bitCount(candidates(i, j));
					outI = i;
					outJ = j;
					found = true;
				}

		return found;
	}

	// Returns the value of a certain cell, or invalid
	inline unsigned getCell(unsigned i, unsigned j) { return cells[i][j]; }
};

// Tries to solve the puzzle in-place. It returns true if it could fill in
// all the squares between iMin and iMax and jMin and jMax, false otherwise.
// If destructive is false it will reverse all its changes once it
// finds a solution or gives up.
// If checkBlocks is true it will try to fill in each 3x3 block separately
// before it starts - if it can't do that then it can't solve the puzzle

bool solve(Puzzle &puzzle, unsigned iMin, unsigned iMax,
	unsigned jMin, unsigned jMax, bool destructive, bool checkBlocks)
{
	// Check that each block can be filled if we've been asked to
	if (checkBlocks) {
		for (unsigned i = 0; i < blockSize; i++)
			for (unsigned j = 0; j < blockSize; j++)
				if (!solve(puzzle,
					i*blockSize, i*blockSize + blockSize,
					j*blockSize, j*blockSize + blockSize, 
					false, false))
						return false;
	}

	// Guess a good cell to brute-force with
	unsigned i;
	unsigned j;

	if (!puzzle.findMin(iMin, iMax, jMin, jMax, i, j))
		// We must have finished
		return true;

	// Iterate through the possible values this cell could have
	unsigned num = 1;
	unsigned mask = bitFor(num);

	while(mask != maskMax) {
		if (puzzle.candidates(i, j) & mask) {

			// Try this number
			puzzle.set(i, j, num);

			bool solved = (solve(puzzle, iMin, iMax, jMin, jMax, 
				destructive, checkBlocks));

			// Reverse the changes if needed
			if (!solved || !destructive)
				puzzle.unset(i, j, num);

			if (solved)
				return true;

		}
			

		// Advance to the next number
		mask *= 2;
		num++;
	}

	// None of the possibilities for cell (i,j) work
	return false;
}

// Reads in a puzzle from standard input
void readPuzzle(Puzzle &p) {
	for (unsigned i = 0; i < gridSize; i++) {
		for (unsigned j = 0; j < gridSize; j++) {
			char c;
			cin >> c;
			if (c >= '1' && c <= '9') {
				c = c - '0';
				// Refuse to input an invalid puzzle

				if ((p.candidates(i, j) & bitFor(c)) == 0)
					exit(1);
				else
					p.set(i, j, c);
			} else if (c == '.') {
			} else exit(1);
		}

		//cin >> endl;
	}
}

// Prints out a (hopefully) completed puzzle
void printPuzzle(Puzzle &p) {
	for (unsigned i = 0; i < gridSize; i++) {
		for (unsigned j = 0; j < gridSize; j++) {
			if (p.getCell(i, j) == p.invalid)
				cout << " ";
			else
				cout << p.getCell(i, j);
		}

		cout << endl;
	}
}

// Solve and print out a puzzle
void outputSolution(Puzzle &p) {
	if (solve(p, 0, gridSize, 0, gridSize, true, true))
		printPuzzle(p);
}

int main() {
	Puzzle p;

	readPuzzle(p);
	outputSolution(p);
	
	return 0;
}	
