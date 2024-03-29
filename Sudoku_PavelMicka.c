/**
 * Sudoku solver using backtracking
 * @author Pavel Micka
 */
class SudokuSolver {
public:
    /**
     * Size of the sudoku, classical Sudoku 9x9
     */
    static const int SUDOKU_SIZE = 9;
    /**
     * Size of the inner subgrids
     */
    static const int SQUARE_SIZE = 3;
    /**
     * Empty cell value
     */
    static const int EMPTY = 0;

    /**
     * Solves the given Sudoku using backtracking (finds all possible solutions)
     * @param array innital setting of the puzzle
     */
    static void solveSudoku(int ** array) {
        bool ** fixed = new bool *[SUDOKU_SIZE];
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            fixed[i] = new bool[SUDOKU_SIZE];
        }
        
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (array[i][j] != 0) fixed[i][j] = true;
                else fixed[i][j] = false;
            }
        }

        int x = -1;
        int y = 0;
        do {
            x = x + 1; //shift to the next cell
            bool overflow = x >= SUDOKU_SIZE; //row overflow?
            if (overflow){
                x = 0;
                y += 1;
            }
        } while (fixed[y][x]); //while the cell is fixed (part of the initial setting)

        for (int i = 1; i <= SUDOKU_SIZE; i++) {
            solve(array, fixed, x, y, i);
        }

        //deallocate
        for (int i = 0; i < SUDOKU_SIZE; i++) delete fixed[i];
        delete[] fixed;

    }
private:
    /**
     * Solves the Sudoku
     * @param array array of the solution
     * @param fixed array, where true denotes that the value is in the initial setting, false
     * that its a member of the partial solution
     * @param x x coordinate, where the next decision will be made
     * @param y y coordinate, where the next decision will be made
     * @param value decision
     */
    static void solve(int ** array, bool ** fixed, int x, int y, int value) {
        if (!checkConsistency(array, x, y, value)) return; //the solution is not consistent
        array[y][x] = value; //set
        do {
            x = x + 1; //shift to the next cell
            bool overflow = x >= SUDOKU_SIZE; //row overflow?
            if (overflow){
                x = 0;
                y += 1;
                if (y == SUDOKU_SIZE) { //column overflow?...solution is complete
                    printArray(array);
                    return;
                }
            }
        } while (fixed[y][x]); //while the field is fixed (part of the initial setting)
        for (int i = 1; i <= SUDOKU_SIZE; i++) { //backtrack
            solve(array, fixed, x, y, i);
        }
        array[y][x] = EMPTY; //reset the cell (otherwise it would infere with the backtracking algorithm)
    }
    /**
     * Checks, if the partial solution is consistent
     * @param array partial solution
     * @param x x coordinate, where the decision was made
     * @param y y coordinate, where the decision was made
     * @param value decision
     * @return true - if the partial solution is consistent, false - if it is not consistent
     */
    static bool checkConsistency(int ** array, int x, int y, int value) {
        //column
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            if (i != y && array[i][x] == value) return false;
        }
        //row
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            if (i != x && array[y][i] == value) return false;
        }
        //group
        int vertical = y/SQUARE_SIZE; //vertical row index
        int horizontal = x/SQUARE_SIZE; //horizontal row index

        for (int i = vertical*SQUARE_SIZE; i < vertical*SQUARE_SIZE + 3; i++) {
            for (int j = horizontal*SQUARE_SIZE; j < horizontal*SQUARE_SIZE + 3; j++) {
                if (array[i][j] == value) return false;
            }
        }
        return true;
    }
    /**
     * Prints out the array (solution)
     * @param array array of the solution
     */
    static void printArray(int ** array) {
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                cout << array[i][j] << "|";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};
