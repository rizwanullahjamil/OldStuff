#include <stdio.h>
#include <conio.h>
#include <time.h>

short s[81] = {

0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,

0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,

0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0};
/*
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 3, 0, 8, 5,
0, 0, 1, 0, 2, 0, 0, 0, 0,

0, 0, 0, 5, 0, 7, 0, 0, 0,
0, 0, 4, 0, 0, 0, 1, 0, 0,
0, 9, 0, 0, 0, 0, 0, 0, 0,

5, 0, 0, 0, 0, 0, 0, 7, 3,
0, 0, 2, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 4, 0, 0, 0, 9};
*/
long g[81];

void print(void)
{
  printf("+-------+-------+-------+\n");
  for(short a = 0; a < 81; a++)
  {
    if(a % 9 == 0)
      printf("|");
    printf("%2d", s[a]);
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

bool check(short p)
{
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
      return 0;
  return 1;
}

bool solve(short p)
{
  short a;
  long r[27];
  while(p < 81 && s[p])
    p++;
  if(p > 80)
    return 1;
  for(long x = g[p]; x > 0; x /= 10)
  {
    s[p] = (short) (x % 10);
    for(a = 0; a < 27; a++)
      r[a] = 0;
//    printf("M%d %d %ld %ld\n", p, s[p], x, g[p]);
    for(a = 0; a < 9; a++)
    {
//      printf("%d", a);
      if(!s[a + (p / 9) * 9])
      {
        long b = 1;
        while((g[a + (p / 9) * 9] / b) % 10)
        {
          if((g[a + (p / 9) * 9] / b) % 10 == s[p])
          {
            r[a] = g[a + (p / 9) * 9];
            g[a + (p / 9) * 9] = (r[a] / b / 10) * b + r[a] % b;
//            printf(" R%d %ld", a + (p / 9) * 9, g[a + (p / 9) * 9]);
            break;
          }
          b *= 10;
        }
        if(!g[a + (p / 9) * 9])
          break;
      }
      if(!s[a * 9 + p % 9])
      {
        long b = 1;
        while((g[a * 9 + p % 9] / b) % 10)
        {
          if((g[a * 9 + p % 9] / b) % 10 == s[p])
          {
            r[a + 9] = g[a * 9 + p % 9];
            g[a * 9 + p % 9] = (r[a + 9] / b / 10) * b + r[a + 9] % b;
//            printf(" C%d %ld", a * 9 + p % 9, g[a * 9 + p % 9]);
            break;
          }
          b *= 10;
        }
        if(!g[a * 9 + p % 9])
          break;
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {
        long b = 1;
        while((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10)
        {
          if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 == s[p])
          {
            r[a + 18] = g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27];
            g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (r[a + 18] / b / 10) * b + r[a + 18] % b;
//            printf(" B%d %ld", a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27, g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]);
            break;
          }
          b *= 10;
        }
        if(!g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
          break;
      }
//      printf("\n");
    }
//    getche();
//    if(a < 9)
//      printf("\n");
//    print();
    if(a == 9 && solve(p + 1.0))
      return 1;
//    printf("U%d %d %ld %ld\n", p, s[p], x, g[p]);
    s[p] = 0;
    for(a = 0; a < 9; a++)
    {
//      printf("%d", a);
      if(!s[a + (p / 9) * 9])
//      {
        g[a + (p / 9) * 9] = r[a];
//        printf(" R%d %ld", a + (p / 9) * 9, g[a + (p / 9) * 9]);
//      }
      if(!s[a * 9 + p % 9])
//      {
        g[a * 9 + p % 9] = r[a + 9];
//        printf(" C%d %ld", a * 9 + p % 9, g[a * 9 + p % 9]);
//      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
//      {
        g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = r[a + 18];
//        printf(" B%d %ld", a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27, g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]);
//      }
//      printf("\n");
    }
//    getche();
  }
//  printf("E%d - %d - %ld\n", p, s[p], g[p]);
  return 0;
}

bool invalid(void)
{
  for(short p = 0; p < 81; p++)
    if(!s[p])
    {
      for(s[p] = 9; s[p] > 0; s[p]--)
        if(check(p))
          g[p] = g[p] * 10 + s[p];
//      printf("%ld) %d %ld\n", i, p, g[p]);
//      getche();
      s[p] = 0;
      if(!g[p])
        return 1;
    }
    else
      if(!check(p))
        return 1;
  return 0;
}

void main(void)
{
  clock_t c = clock();
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
//  for(short p = 0; p < 81; p++)
//    if(!s[p])
//      printf("%d %ld\n", p, g[p]);
  printf("\nRun in %f ms.", (clock() - c) / CLOCKS_PER_SEC);
  getche();
}
