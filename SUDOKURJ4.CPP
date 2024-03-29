#include <stdio.h>
#include <conio.h>
#include <time.h>

unsigned short s[81] = {
4, 0, 0, 0, 3, 0, 0, 0, 0,
0, 0, 0, 6, 0, 0, 8, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 1,

0, 0, 0, 0, 5, 0, 0, 9, 0,
0, 8, 0, 0, 0, 0, 6, 0, 0,
0, 7, 0, 2, 0, 0, 0, 0, 0,

0, 0, 0, 1, 0, 2, 7, 0, 0,
5, 0, 3, 0, 0, 0, 0, 4, 0,
9, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned long i, t[81];

void print(void)
{
  printf("+-------+-------+-------+\n");
  for(unsigned short a = 0; a < 81; a++)
  {
    if(a % 9 == 0)
      printf("|");
    printf("%2u", s[a]);
    if(a % 3 == 2)
      printf(" |");
    if(a % 9 == 8)
    {
      printf("\n");
      if(a % 27 == 26)
        printf("+-------+-------+-------+\n");
    }
  }
}

bool check(unsigned short q)
{
  i++;
  for(unsigned short a = 0; a < 9; a++)
    if((a + (q / 9) * 9 != q && s[a + (q / 9) * 9] == s[q])
    || (a * 9 + q % 9 != q && s[a * 9 + q % 9] == s[q])
    || (a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27 != q && s[a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27] == s[q]))
      return 0;
  return 1;
}

bool solve(unsigned short p)
{
  unsigned long a = t[p];
  while(p < 81 && s[p])
    p++;
  if(p > 80)
    return 1;
  printf("%u %lu\n", p, t[p]);
  getche();
  for(s[p] = t[p] % 10; t[p] > 0; t[p] /= 10, s[p] = t[p] % 10)
    if(check(p) && solve(p + 1))
      return 1;
  t[p] = a;
  return s[p] = 0;
}

bool invalid(void)
{
  for(unsigned short p = 0; p < 81; p++)
    if(s[p])
    {
      if(!check(p))
        return 1;
    }
    else
    {
      unsigned long a = 1;
      for(s[p] = 1; s[p] < 10; s[p]++)
        if(check(p))
        {
          t[p] += s[p] * a;
          a *= 10;
        }
      s[p] = 0;
      if(!t[p])
        return 1;
    }
  return 0;
}

void main(void)
{
  clock_t t = clock();
  print();
  if(invalid())
    printf("\nError: Invalid Sudoku given.");
  else
  {
    if(solve(0))
      print();
    else
      printf("\nUnsolvable!\n");
  }
  printf("\nRun %lu iterations in %f ms.", i, (clock() - t) / CLK_TCK);
  getche();
}
