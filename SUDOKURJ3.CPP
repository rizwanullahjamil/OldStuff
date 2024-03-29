#include <stdio.h>
#include <math.h>
#include <time.h>

unsigned short r[81], s[81] = {
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

  if(p > 80)
    return 1;
  for(s[p] = t[p] % 10; s[p] > 0; t[p] /= 10)
    if(solve(p + 1))
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
  char c;
  clock_t t = clock();
  print();
  if(invalid())
    printf("\nErrot: Invalid Sudoku given.");
  else
  {
    unsigned short q = 1;
    for(unsigned short p = 0; p < 81; p++)
      if(!s[p])
      {
        for(s[p] = 1; s[p] < 10; s[p]++)
          if(check(p))
          {
            t[p] += s[p] * q;
            q *= 10.0;
          }
        for(q = 0; q < p; q++)
          if(t[r[q]] > t[p])
          {
            for(s[p] = p; s[p] > q; s[p]--)
              r[s[p]+1] = r[s[p]];
            r[q] = p;
          }
        s[p] = q = 0;
      }
    if(solve(0))
      print();
    else
      printf("\nUnsolvable!\n");
  }
  printf("\nRun %lu iterations in %f ms.", i, (clock() - t) / CLK_TCK);
  scanf("%c", &c);
}
