#include <stdio.h>
#include <stdlib.h>
#define SIZE 9

int isValid(int (*sudoku)[SIZE],int row,int col,int weight)
{
  int i,j,start_row,start_col;

  for(i=0;i<SIZE;i++)
    if(sudoku[row][i]==weight)
      return 0;

  for(i=0;i<SIZE;i++)
    if(sudoku[i][col]==weight)
      return 0;

  start_row=(row/3)*3;
  start_col=(col/3)*3;
  for(i=start_row;i<start_row+3;i++)
    for(j=start_col;j<start_col+3;j++)
      if(sudoku[i][j]==weight)
        return 0;

  return 1;
}

void printSolution(int (*sudoku)[SIZE])
{
  int i,j;

  printf("....................................\n");

  for(i=0;i<SIZE;i++)
  {
    for(j=0;j<SIZE;j++)
      printf("%d ",sudoku[i][j]);
    printf("\n");
  }
  printf("....................................\n");
}

void solveSudoku(int (*sudoku)[SIZE],int row,int col)
{
  int i;
  if(row==SIZE && col==0)
    printSolution(sudoku);
  else
  {
    if(sudoku[row][col])    //encounters a pre-filled cell.
    {
      solveSudoku(sudoku,row,col+1);
      if(col==SIZE-1)
        solveSudoku(sudoku,row+1,0);
    }
    else
       for(i=1;i<=SIZE;i++)
         if( isValid(sudoku,row,col,i) )
         {
            sudoku[row][col]=i;
            solveSudoku(sudoku,row,col+1);
            if(col==SIZE-1)
              solveSudoku(sudoku,row+1,0);
            sudoku[row][col]=0;
          }
  }
}

int main()
{
  int sudoku[SIZE][SIZE]={{3, 0, 6, 5, 0, 8, 4, 0, 0},
                          {5, 2, 0, 0, 0, 0, 0, 0, 0},
                          {0, 8, 7, 0, 0, 0, 0, 3, 1},
                          {0, 0, 3, 0, 1, 0, 0, 8, 0},
                          {9, 0, 0, 8, 6, 3, 0, 0, 5},
                          {0, 5, 0, 0, 9, 0, 6, 0, 0},
                          {1, 3, 0, 0, 0, 0, 2, 5, 0},
                          {0, 0, 0, 0, 0, 0, 0, 7, 4},
                          {0, 0, 5, 2, 0, 6, 3, 0, 0}};
  int i,j;

  printSolution(sudoku);  //Display inputted sudoku from the file
  solveSudoku(sudoku,0,0);
  return 0;
}
