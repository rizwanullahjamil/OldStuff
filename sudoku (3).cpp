//
// sudoku.cpp - This code uses logic to solve a sudoku puzzle of arbitrary
//              size. Placed in the public domain by Ed Kaiser.
//
// The puzzle is specified in a file specified at the command line (default
// is "input"). The solution is printed to stderr or a file as specified.
// The input file is composed of integers. The first defines the sudoku
// dimension. The remaining integers specify the values provided by the
// puzzle. 0 corresponds to unspecified. An example input file:
//
//                                4
//                                1 2 3 0
//                                0 3 0 2
//                                0 1 4 0
//                                0 0 0 0
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

class sudoku {
private:
   int  n, b, digits, remaining;
   char *solution, *possible;
   int  mrow, mcol, changed;

   void set(int row, int col, int value);
   void eliminate(int row, int col, int value);
   void isolation();
   void reduction();
public:
   sudoku();
   ~sudoku();
   void solve(FILE* input);
   void print(FILE* output);
};


sudoku::sudoku() {
   n = 0;
   solution = possible = (char*)NULL;
}

sudoku::~sudoku() {
   if(solution)      free(solution);
   if(possible)      free(possible);
}


void sudoku::print(FILE* output = stderr) {
   if(!n) return;

   // Print how many cells are empty if there are any.
   if(remaining) fprintf(output, "Cells still empty: %d\n", remaining);

   // Print what the current solution looks like.
   for(int row = 0; row < n; row++) {
      if(row % b == 0) {
         for(int i = 0; i < (((digits + 1) * n) - b); i++) {
            if(i % (((digits + 1) * b) - 1) == 0) fprintf(output, "+");
            fprintf(output, "-");
         }
         fprintf(output, "+\n");
      }
      for(int col = 0; col < n; col++) {
         if(col % b == 0) fprintf(output, "|");
         else             fprintf(output, " ");

         if(solution[row*n + col]) fprintf(output, "%*d", digits, solution[row*n + col]);
         else                      fprintf(output, "%*s", digits, " ");
      }
      fprintf(output, "|\n");
   }
   for(int i = 0; i < (((digits + 1) * n) - b); i++) {
      if(i % (((digits + 1) * b) - 1) == 0) fprintf(output, "+");
      fprintf(output, "-");
   }
   fprintf(output, "+\n");
}


void sudoku::set(int row, int col, int value) {
   if(!possible[row*mrow + col*mcol + value]) return;

   // Set the cell (row,col) to value.
   solution[row*n + col] = (char)value;
   remaining--;
   changed = 1;
   // Eliminate all other possibilities for this cell.
   for(int i = 0; i < n + 1; i++)
      possible[row*mrow + col*mcol + i] = 0;
   // Eliminate the value from the row and column.
   for(int i = 0; i < n; i++) {
      eliminate(i, col, value);
      eliminate(row, i, value);
   }
   // Eliminate the value from the block. 
   int blockrow = (row/b)*b, blockcol = (col/b)*b;
   for(int i = 0; i < b; i++)
      for(int j = 0; j < b; j++)
         eliminate(blockrow + i, blockcol + j, value);
}


void sudoku::isolation() {
   if(!remaining) return;
   // Find a single cell in a row, column or block that can be a value. 
   for(int value = 1; value < n + 1; value++) {
      // Search rows and columns for single possibilities.
      for(int i = 0; i < n; i++) {
         int countrow = 0, row, countcol = 0, col;
         for(int j = 0; j < n; j++) {
            if(possible[i*mrow + j*mcol + value]) {
               countrow++; col = j; }
            if(possible[j*mrow + i*mcol + value]) {
               countcol++; row = j; }
         }
         if(countrow == 1)
            set(i, col, value);
         if(countcol == 1 && !(row == i && col == i))
            set(row, i, value);
      }
      // Search blocks for single possibilites.
      for(int blockrow = 0; blockrow < n; blockrow += b)
         for(int blockcol = 0; blockcol < n; blockcol += b) {
            int count = 0, x, y;
            for(int row = blockrow; row < blockrow + b; row++)
               for(int col = blockcol; col < blockcol + b; col++)
                  if(possible[row*mrow + col*mcol + value]) {
                     count++; y = row; x = col; }
            if(count == 1) set(y, x, value);
         } 
   }
}


void sudoku::eliminate(int row, int col, int value) {
   // Eliminate the value as a possibility for the cell at (row,col).
   if(possible[row*mrow + col*mcol + value]) {
      possible[row*mrow + col*mcol + value] = 0;
      changed = 1;
      if(--possible[row*mrow + col*mcol] == 1)
         for(int i = 1; i < n + 1; i++)
            if(possible[row*mrow + col*mcol + i]) {
               set(row, col, i);
               return;
            }
   }
}


void sudoku::reduction() {
   if(!remaining) return;
   // Find a row or a column in a block that must be a value and remove
   // that possibility from the other cells in that row or column.
   for(int value = 1; value < n + 1; value++) 
      for(int blockrow = 0; blockrow < n; blockrow += b)
         for(int blockcol = 0; blockcol < n; blockcol += b) {
            int countrow = 0, countcol = 0;
            int lastrow = -1, lastcol = -1;
            for(int row = blockrow; row < blockrow + b; row++)
               for(int col = blockcol; col < blockcol + b; col++)
                  if(possible[row*mrow + col*mcol + value]) {
                     if(lastrow != row) {
                        countrow++; lastrow = row;
                     }
                     if(lastcol != col) {
                        countcol++; lastcol = col;
                     }
                  }
            if(countrow == 1) 
               // Remove value as a possibility from this row in other blocks.
               for(int col = 0; col < n; col++)
                  if(!(col >= blockcol && col < blockcol + b))
                     eliminate(lastrow, col, value);

            if(countcol == 1) 
               // Remove value as a possibility from this column in other blocks.
               for(int row = 0; row < n; row++)
                  if(!(row >= blockrow && row < blockrow + b))
                     eliminate(row, lastcol, value);
         }
}


void sudoku::solve(FILE* input) {
   if(!input) return;

   // Read the grid dimension.
   fscanf(input, "%d", &n);
   b = (int)sqrt(n);
   if(b * b != n) {
      fprintf(stderr, "Specify a dimension with an integer square root.\n");
      return;
   }
   remaining = n * n;
   mcol = n + 1; mrow = n * mcol; 
   int value = n;
   digits = 0;
   while(value > 0) { digits++; value /= 10; }

   // Initialize the grid with all possible numbers.   
   possible = (char*)malloc(n * mrow);
   for(int row = 0; row < n; row++)
      for(int col = 0; col < n; col++) {
         possible[row*mrow + col*mcol] = n;
         for(int i = 1; i < n + 1; i++)
            possible[row*mrow + col*mcol + i] = 1;
      }

   // Process the input file.
   solution = (char*)malloc(n * n);
   for(int row = 0; row < n; row++)
      for(int col = 0; col < n; col++) {
         fscanf(input, "%d", &value);
         if(value && remaining) set(row, col, value);
      }

   // While the puzzle has not been solved, yet has incremental changes,
   // continue to isolate numbers and reduce possibilities.
   while(changed) {
      changed = 0;
      isolation();
      reduction();
   }
}


int main(int argc, char* argv[]) {
   FILE *input = NULL, *output = NULL;
   sudoku S;

   if(argc > 1) input = fopen(argv[1], "rt");
   if(!input)   input = fopen("input", "rt");
   if(!input)   return 0;

   if(argc > 2) output = fopen(argv[2], "wt");
   if(!output)  output = stderr;
   
   S.solve(input);
   S.print(output);

   if(input)                      fclose(input);
   if(output && output != stderr) fclose(output);
   return 0;
}
