#include <stdio.h>
#include <conio.h>
#include <time.h>

short s[81] = {
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 3, 0, 8, 5,
0, 0, 1, 0, 2, 0, 0, 0, 0,

0, 0, 0, 5, 0, 7, 0, 0, 0,
0, 0, 4, 0, 0, 0, 1, 0, 0,
0, 9, 0, 0, 0, 0, 0, 0, 0,

5, 0, 0, 0, 0, 0, 0, 7, 3,
0, 0, 2, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 4, 0, 0, 0, 9};

long i, t[81];

void print(void)
{
  printf("+-------+-------+-------+\n");
  for(short a = 0; a < 81; a++)
  {
    if(a % 9 == 0)
      printf("|");
    printf("%2ld", s[a]);
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
  i++;
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
      return 0;
  return 1;
}

bool solve(short p)
{
  long r[27] = {
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0};
  while(p < 81 && s[p])
    p++;
  if(p > 80)
    return 1;
  for(s[p] = (short) (t[p] % 10); t[p] > 0; t[p] /= 10, s[p] = (short) (t[p] % 10))
  {
//    printf("M%d %ld %ld\n", p, s[p], t[p]);
    for(short a = 0; a < 9; a++)
    {
//      printf("%d", a);
      if(!s[a + (p / 9) * 9])
      {
        long b = 1;
        r[a] = t[a + (p / 9) * 9];
        while((r[a] / b) % 10)
        {
          if((r[a] / b) % 10 == s[p])
          {
            t[a + (p / 9) * 9] = (r[a] / b / 10) * b + r[a] % b;
//            printf(" R%d %ld", a + (p / 9) * 9, t[a + (p / 9) * 9]);
            break;
          }
          b *= 10;
        }
      }
      if(!s[a * 9 + p % 9])
      {
        long b = 1;
        r[a + 9] = t[a * 9 + p % 9];
        while((r[a + 9] / b) % 10)
        {
          if((r[a + 9] / b) % 10 == s[p])
          {
            t[a * 9 + p % 9] = (r[a + 9] / b / 10) * b + r[a + 9] % b;
//            printf(" C%d %ld", a * 9 + p % 9, t[a * 9 + p % 9]);
            break;
          }
          b *= 10;
        }
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {
        long b = 1;
        r[a + 18] = t[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27];
        while((r[a + 18] / b) % 10)
        {
          if((r[a + 18] / b) % 10 == s[p])
          {
            t[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (r[a + 18] / b / 10) * b + r[a + 18] % b;
//            printf(" B%d %ld", a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27, t[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]);
            break;
          }
          b *= 10;
        }
      }
//      printf("\n");
    }
//    getche();
    if(solve(p + 1.0))
      return 1;
//    printf("U%d %d %ld\n", p, s[p], t[p]);
    for(short a = 0; a < 27; a++)
    {
      long b =1;
      while((r[a] / b) % 10)
      {
        if((r[a] / b) % 10 == s[p])
        {
          t[a] = r[a];
          r[a] = 0;
//          printf("%d %c%d %ld\n", a, a < 9 ? 'R' : a < 18 ? 'C' : 'B', a < 9 ? a + (p / 9) * 9 : a < 18 ? (a - 9) * 9 + p % 9 : (a - 18) % 3 + ((a - 18) / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27, t[a]);
        }
        b *= 10;
      }
    }
//    getche();
  }
  return s[p] = 0;
}

bool invalid(void)
{
  for(short p = 0; p < 81; p++)
  {
    if(s[p])
    {
      if(!check(p))
        return 1;
    }
    else
    {
      long b = 1;
      for(s[p] = 1; s[p] < 10; s[p]++)
        if(check(p))
        {
          t[p] += s[p] * b;
          b *= 10;
        }
//      printf("%d %ld\n", p, t[p]);
//      getche();
      s[p] = 0;
      if(!t[p])
        return 1;
    }
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
  printf("\nRun %ld iterations in %f ms.", i, (clock() - t) / CLOCKS_PER_SEC);
  getche();
}
