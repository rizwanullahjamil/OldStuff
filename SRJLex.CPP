#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

unsigned short s[81] = {
1, 2, 3, 4, 5, 6, 7, 8, 9,
4, 5, 6, 7, 8, 9, 1, 2, 3,
7, 8, 9, 1, 2, 3, 4, 5, 6,

2, 1, 4, 3, 6, 5, 8, 9, 7,
3, 6, 5, 8, 9, 7, 2, 1, 4,
8, 9, 7, 2, 1, 4, 5, 6, 3};

unsigned long i;
unsigned short d = 1;
FILE *f;

void print(void)
{
  for(unsigned short a = 0; a < 81; a++)
    fprintf(f, "%1u", s[a]);
  fprintf(f, " // %lu\n", i);
}

bool check(unsigned short q)
{
  i++;
  for(unsigned short a = 0; a < 9; a++)
    if((a + (q / 9) * 9 != q && s[a + (q / 9) * 9] == s[q])
    || (a * 9 + q % 9 != q && s[a * 9 + q % 9] == s[q])
    || (a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27 != q
        && s[a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27] == s[q]))
      return 0;
  return 1;
}

bool solve(unsigned short p)
{
  while(p < 81 && s[p])
    p++;
  if(p > 80)
  {
    print();
//    printf("\nRun %lu iterations in %f ms.", i, double(clock() - t) / double(CLOCKS_PER_SEC) * 1000.0);
    i = 0;
//    printf("\nContinue searching [N]ext or [P]revious valid 9x9 Sudoku solution grid N/P ? ");
//    switch (abs(105 - getche()) + 73)
//    {
//    case 'N':
//      d = 1;
      return 0;
//    case 'P':
//      d = -1;
//      return 0;
//    default:
//      return 1;
//    }
  }
  for(s[p] = 5 - 4 * d; s[p] > 0 && s[p] < 10; s[p] += d)
    if(check(p) && solve(p + 1))
      return 1;
  return s[p] = 0;
}

bool invalid(void)
{
  for(unsigned short p = 0; p < 81; p++)
    if(s[p] && !check(p))
      return 1;
  return 0;
}

void main(void)
{
  clock_t t = clock();
  clrscr();
  printf("Start searching [N]ext or [P]revious valid 9x9 Sudoku solution grid N/P ? ");
  if(abs(105 - getche()) + 73 == 'P')
    d = -1;
  if((f = fopen("sudokus.txt", "w")) == NULL)
  {
    printf("\nError: Unable to open sudokus.txt file for writing mode.");
    return;
  }
  if(invalid())
    printf("\nError: Invalid Sudoku given.");
  else
    if(!solve(0))
        fprintf(f, "Unsolvable %s !\nRun %lu iterations in %f ms.", (d == 1) ? "Next" : "Previous",i , double(clock() - t) / double(CLOCKS_PER_SEC) * 1000.0);
  if(fclose(f) == EOF)
    printf("\nError: Unable to close sudokus.txt file.");
  getche();
}
