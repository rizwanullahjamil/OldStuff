/* SudokuEZ.c - a concise sudoku brute force program, Author unknown.

status: na

This program was written using Turbo C.

Users of newer compilers should:
1) include the windows.h file line, if you don't have conio.h
2) include the define gotoxy file line
3) change CLK_TCK to TICKS_PER_SEC

All users should know that this program has not been tested,
beyond a very low level. We're just experimenting with it,
at this time.

*/

//#include <windows.h>     //for older compilers, don't include this line
//#define gotoxy Gotoxy    // "    "     "         "       "     "    "

#include <stdio.h>
#include <conio.h>
#include <time.h>

int grid[9][9];

int benchmark(FILE *);
void Gotoxy(int x, int y);
void load(void);
void printIt(void);
void print_solution(void);
int safe(int row, int col, int n);
void solve(int row, int col, int format);

/* format == 0, show solutions, format == 1, don't show solutions */

int benchmark(FILE* fp)
{
  clock_t start, stop;
  int c, r, temp;
  unsigned long puzzNum = 0;
  if((fp = fopen("Top150.txt", "rt")) == NULL)
  {
    perror("\n Error opening Top150.txt file - terminating program ");
    return 1;
  }
  start = clock();
  temp = 0;
  while(temp != EOF)
  {
    for(r = 0; r < 9; r++)
    {
      for(c = 0; c < 9; c++)
      {
        temp = fgetc(fp);
        if(temp == '.')
          temp = 0;
        else
          temp = temp - '0';
        grid[r][c] = temp;
      }
    }
    temp = fgetc(fp); //pulls off the newline
    if(temp == EOF)
      continue;
    printIt();
    gotoxy(1,16);
    printf("   Working on puzzle number: %ld", ++puzzNum);
    //if(puzzNum > 100) break;
    //getch(); getch();
    solve(0,0,1);
  }
  stop = clock();
  gotoxy(1, 18);
  //newer compiler? replace CLK_TCK with TICKS_PER_SEC
  printf("   Elapsed time was: %.2f seconds", (stop-start)/CLK_TCK);
  printf("\n\n\t\t    Benchmark complete - press enter when ready");
  while((temp = getchar()) != '\n');
  temp = getchar();
  fclose(fp);
  return 0;
}

/* Include this function, for newer compilers
void Gotoxy(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
*/

int main()
{
  FILE *fp;
  int i, j;
  char ch;
  for(i=0; i< 10; i++)
    printf("\n\n\n\n\n");
  do
  {
    for(i=3; i< 21; i++)
    {
      gotoxy(1,i);
      printf("                                                                    ");
    }
    gotoxy(1,3);
    printf("\t\t        Welcome to Sudoku Solver's Main Menu\n\n");
    printf("    *press 'b' to benchmark this program\n\n");
    printf("    *press 'l' to load a puzzle from this program\n\n");
    printf("    *press 'e' to enter your own puzzle, or 'q' to quit [b/l/e/q]: ");
    for(i=0; i < 9; i++)
    {
      for(j = 0; j < 9; j++)
      {
        grid[i][j] = 0;
      }
    }
    ch=getchar();

    if(ch == 'q')
      break;
    for(i=3; i< 10; i++)
    {
      gotoxy(1,i);
      printf("                                                                    ");
    }
    if(ch == 'b')
    {
      gotoxy(1,2);
      printf("\t\t      Sudoku is Running a Benchmark Suite of Puzzles");
      j = benchmark(fp);
      if(j)
        return 1;
    }
    else if(ch == 'l')
      load();
    else if(ch == 'e')
    {
      printf("enter your puzzle: \n");
      for(i=0;i<9;i++)
      {
        for(j=0;j<9;j++)
        {
          printf("(%d, %d): ", i+1, j+1);
          scanf("%d", &grid[i][j]);
        }
      }
    }
    if(ch != 'b')
    {
      printIt();
      solve(0,0,0);
      gotoxy(1, 22);
      printf("\t\t\t         Press enter when ready");
      i = getchar();
      gotoxy(1,22);
      printf("\t\t\t                                    ");
    }
  } while(ch != 'q');

  gotoxy(1, 22);
  printf("\t\t    Program complete, press enter when ready");
  while((i = getchar()) != '\n');
  i = getchar();
  return 0;
}

void load(void)
{
  int c, gar;

  gotoxy(1,6);
    printf("   There are four very hard puzzles. Choose 1, 2, 3 or 4: ");
    scanf("%d", &c);
    gar = getchar();
  gotoxy(1,6);
  printf("                                                           ");
    if(c == 1)
    {
            /*sudoku17.txt Game 9 Tough!.*/
            /*000000012 400090000 000000050 070200000 600000400 000108000
            018000000 000030700 502000000*/
        grid[0][7] = 1; grid[0][8] = 2; grid[1][0] = 4; grid[1][4] = 9; grid[2][7] = 5;
        grid[3][1] = 7; grid[3][3] = 2; grid[4][0] = 6; grid[4][6] = 4; grid[5][3] = 1;
        grid[5][5] = 8; grid[6][1] = 1; grid[6][2] = 8; grid[7][4] = 3; grid[7][6] = 7;
        grid[8][0] = 5; grid[8][2] = 2;
    } /*This game ^^ is a great testbed - Very hard. Solution starts with 367 485 912*/
    else if(c == 2)
    {
            /*VH 2: "near worst case for brute force sudoku solver" - Wikipedia
            My solve time: 0.22 seconds */
      grid[1][5] = 3; grid[1][7] = 8; grid[1][8] = 5;
        grid[2][2] = 1; grid[2][4] = 2;
        grid[3][3] = 5; grid[3][5] = 7;
        grid[4][2] = 4; grid[4][6] = 1;
        grid[5][1] = 9;
        grid[6][0] = 5; grid[6][7] = 7; grid[6][8] = 3;
        grid[7][2] = 2; grid[7][4] = 1;
        grid[8][4] = 4; grid[8][8] = 9;
    }
    else if(c == 3)
    {
            /*VH 3: Sudoku17.txt #11 - one of my hardest to solve, Time: 3 seconds
            000000012 700060000 000000050   080200000 600000400 000109000
            019000000 000030800 502000000*/
        grid[0][7] = 1; grid[0][8] = 2;
        grid[1][0] = 7; grid[1][4] = 6;
        grid[2][7] = 5;
        grid[3][1] = 8; grid[3][3] = 2;
        grid[4][0] = 6; grid[4][6] = 4;
        grid[5][3] = 1; grid[5][5] = 9;
        grid[6][1] = 1; grid[6][2] = 9;
        grid[7][4] = 3; grid[7][6] = 8;
        grid[8][0] = 5; grid[8][2] = 2;
    }
    else if(c == 4)
    {
     /*VH 4: Sudoku17.txt #619 -
     000000071 900000060 020000000 004070000 030000400
     000910000 700600008 000300200 100000000 */
      grid[0][7] = 7; grid[0][8] = 1;
    grid[1][0] = 9; grid[1][7] = 6;
    grid[2][1] = 2;
    grid[3][2] = 4; grid[3][4] = 7;
    grid[4][1] = 3; grid[4][6] = 4;
    grid[5][3] = 9; grid[5][4] = 1;
    grid[6][0] = 7; grid[6][3] = 6; grid[6][8] = 8;
    grid[7][3] = 3; grid[7][6] = 2;
    grid[8][0] = 1;
    }
    gar++;
}

void printIt(void)
{
    int r, c;
    gotoxy(2, 3);
    for(r = 0; r < 9; r++)
    {
        printf("\n   ");
        if(r % 3 == 0 && r > 1)
            printf("=======================\n   ");
        for(c = 0; c < 9; c++)
        {
            if(c % 3 == 0 && c > 1)
                printf("|| ");
            if(grid[r][c])
            {
                //textcolor(14);
                printf("%d ", grid[r][c]);  //was cprintf()
                //textcolor(15);
            }
            else
                printf("%d ", grid[r][c]);   //also cprintf()
        }
    }
  //textcolor(15);
    //gar = getchar(); gar++;
}

void print_solution(void)
{
  static int nsol = 0;
  int r, c;

  printf("----- solution %d -----\n", ++nsol);

  for (r = 0; r < 9; r++)
  {
    for (c = 0; c < 9; c++)
    {
      printf("%d", grid[r][c]);
      if (c % 3 == 2)
        printf("  ");
    }
    printf("\n");
    if (r % 3 == 2) printf("\n");
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

void solve(int row, int col, int format)
{
  int n, t;
  unsigned long solNum = 0;

  if (row == 9 && !format )
  {
    printIt();          //static grid printout
    //print_solution(); //enables the rolling screen of solutions mode
    solNum++;
    printf("\n\n   Found %ld Solution%c", solNum, solNum > 1 ? 's': ' ');
  }
  else
  {
    for (n = 1; n <= 9; n++)
    {
      if (safe(row, col, n))
      {
        t = grid[row][col];
        grid[row][col] = n;
        if (col == 8)
          solve(row + 1, 0, format);
        else
          solve(row, col + 1, format);

        grid[row][col] = t;
      }
    }
  }
}
