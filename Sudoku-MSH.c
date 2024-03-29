#include <stdio.h>

int grid[9][9];

void print_solution(void)
{
 static int nsol = 0;
 int r, c;
 printf("----- solution %d -----\n", ++nsol);
 for (r = 0; r < 9; r++)
 {
   for (c = 0; c < 9; c++)\
   {
     printf("%d", grid[r][c]);
     if (c % 3 == 2)
       printf("  ");
   }
   printf("\n");
   if (r % 3 == 2)
    printf("\n");
 }
}

int safe(int row, int col, int n)
{
 int r, c, br, bc;
 if (grid[row][col] == n)
  return 1;
 if (grid[row][col] != 0)
  return 0;
 for (c = 0; c < 9; c++)
  if (grid[row][c] == n)
   return 0;
 for (r = 0; r < 9; r++)
  if (grid[r][col] == n)
   return 0;
 br = row / 3;
 bc = col / 3;
 for (r = br * 3; r < (br + 1) * 3; r++)
  for (c = bc * 3; c < (bc + 1) * 3; c++)
   if (grid[r][c] == n)
     return 0;
 return 1;
}

void solve(int row, int col)
{
 int n, t;
 if (row == 9)
  print_solution();
 else
  for (n = 1; n <= 9; n++)
   if (safe(row, col, n))
   {
     t = grid[row][col];
     grid[row][col] = n;
     if (col == 8)
      solve(row + 1, 0);
     else
      solve(row, col + 1);
     grid[row][col] = t;
   }
}

int main()
{
 int i, j;
 printf("enter the sudoku: \n");
 for(i=0;i<9;i++)
  for(j=0;j<9;j++)
  {
    printf("(%d, %d): ", i+1, j+1);
    scanf("%d", &grid[i][j]);
  }
  solve(0,0);
  return 0;
} 
