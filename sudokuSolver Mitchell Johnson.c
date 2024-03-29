/*
 * sudokuSolver.c
 * 
 * A simple backtracking sudoku solver.  Accepts input with cells
 * separated by spaces and rows separated by newlines.  If no input
 * file is specified as the first argument, the file is accepted
 * from stdin.
 *
 * Copyright (c) Mitchell Johnson (mitchell.johnson@atomicobject.com), 2012
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 * http://spin.atomicobject.com/2012/06/18/solving-sudoku-in-c-with-recursive-backtracking/sudokusolver/
 */

#include <stdio.h>

int solveSudoku(int puzzle[][9]);
int findAnUnassignedSpot(int puzzle[][9], int row, int column );
int isValid(int number, int puzzle[][9], int row, int column);
int sudokuHelper(int puzzle[][9], int row, int column);
void printSudoku(int puzzle[][9]);

int main(int argc, char** argv) {
    int puzzle[9][9];
    char* fileName;
    FILE* sudokuFile;
    int i = 0;

    /* Should we read from stdin or a file? */
    if (argc > 1) {
        fileName = argv[1];
        sudokuFile = fopen(fileName, "r");
    } else {
        sudokuFile = stdin;
    }

    if (sudokuFile == NULL) {
        puts("Couldn't open sudoku file for reading!");
        return 1;
    }

    /* Grab the sudoku data from the file (not very robust) */
    for ( i=0; i<9; i++) {
        fscanf(sudokuFile, "%d %d %d %d %d %d %d %d %d", &puzzle[i][0],
            &puzzle[i][1], &puzzle[i][2], &puzzle[i][3], &puzzle[i][4],
            &puzzle[i][5], &puzzle[i][6], &puzzle[i][7], &puzzle[i][8]);
    }

    /* Print out the original puzzle, then start solving. */
    puts("Original Puzzle:");

    printSudoku(puzzle);

    puts("Solving...");
    puts("");

    /* If successful, print the solution */
    if (solveSudoku(puzzle)) {
        puts("Sudoku Solved!");
        printSudoku(puzzle);
    } else {
        puts("Illegal sudoku (or a broken algorithm [not likely])");
    }
    
    return 0;
}

/*
 * A helper function to call sudokuHelper recursively
 */
int solveSudoku(int puzzle[][9]) {
    return sudokuHelper(puzzle, 0, 0);
}

/*
 * A recursive function that does all the gruntwork in solving
 * the puzzle.
 */
int sudokuHelper(int puzzle[][9], int row, int column) {
    int nextNumber = 1;
    /*
     * Have we advanced past the puzzle?  If so, hooray, all
     * previous cells have valid contents!  We're done!
     */
    if (9 == row) {
        return 1;
    }

    /*
     * Is this element already set?  If so, we don't want to 
     * change it.
     */
    if (puzzle[row][column]) {
        if (column == 8) {
            if (sudokuHelper(puzzle, row+1, 0)) return 1;
        } else {
            if (sudokuHelper(puzzle, row, column+1)) return 1;
        }
        return 0;
    }

    /*
     * Iterate through the possible numbers for this empty cell
     * and recurse for every valid one, to test if it's part
     * of the valid solution.
     */
    for (; nextNumber<10; nextNumber++) {
        if(isValid(nextNumber, puzzle, row, column)) {
            puzzle[row][column] = nextNumber;
            if (column == 8) {
                if (sudokuHelper(puzzle, row+1, 0)) return 1;
            } else {
                if (sudokuHelper(puzzle, row, column+1)) return 1;
            }
            puzzle[row][column] = 0;
        }
    }
    return 0;
}

/*
 * Checks to see if a particular value is presently valid in a
 * given position.
 */
int isValid(int number, int puzzle[][9], int row, int column) {
    int i=0;
    int modRow = 3*(row/3);
    int modCol = 3*(column/3);
    int row1 = (row+2)%3;
    int row2 = (row+4)%3;
    int col1 = (column+2)%3;
    int col2 = (column+4)%3;

    /* Check for the value in the given row and column */
    for (i=0; i<9; i++) {
        if (puzzle[i][column] == number) return 0;
        if (puzzle[row][i] == number) return 0;
    }
    
    /* Check the remaining four spaces in this sector */
    if(puzzle[row1+modRow][col1+modCol] == number) return 0;
    if(puzzle[row2+modRow][col1+modCol] == number) return 0;
    if(puzzle[row1+modRow][col2+modCol] == number) return 0;
    if(puzzle[row2+modRow][col2+modCol] == number) return 0;
    return 1;
}

/*
 * Convenience function to print out the puzzle.
 */
void printSudoku(int puzzle[][9]) {
    int i=0, j=0;
    for (i=0; i<9; i++) {
        for (j=0; j<9; j++) {
            if (2 == j || 5 == j) {
                printf("%d | ", puzzle[i][j]);
            } else if (8 == j) {
                printf("%d\n", puzzle[i][j]);
            } else {
                printf("%d ", puzzle[i][j]);
            }
        }
        if (2 == i || 5 == i) {
            puts("------+-------+------");
        }
    }
}
