#include <stdio.h>
#include <conio.h>
#include <time.h>

short m[81], s[81] = {
/*
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,

0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,

0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0};
*/
0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 3, 0, 8, 5,
0, 0, 1, 0, 2, 0, 0, 0, 0,

0, 0, 0, 5, 0, 7, 0, 0, 0,
0, 0, 4, 0, 0, 0, 1, 0, 0,
0, 9, 0, 0, 0, 0, 0, 0, 0,

5, 0, 0, 0, 0, 0, 0, 7, 3,
0, 0, 2, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 4, 0, 0, 0, 9};

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
  short a, x;
  if(p > 80)
    return 1;
  for(long r = g[m[p]]; r > 0; r /= 10)
  {
    s[m[p]] = (short) (r % 10);
    for(a = 0; a < 9; a++)
    {
      if(!s[m[a + (p / 9) * 9]])
      {
        long b = 1;
        while((g[m[a + (p / 9) * 9]] / b) % 10)
        {
          if((g[m[a + (p / 9) * 9]] / b) % 10 == s[m[p]])
          {
            g[m[a + (p / 9) * 9]] = (g[m[a + (p / 9) * 9]] / b / 10) * b + g[m[a + (p / 9) * 9]] % b;
            break;
          }
          b *= 10;
        }
        if(!g[m[a + (p / 9) * 9]])
          break;
// sorting
      }
      if(!s[m[a * 9 + p % 9]])
      {
        long b = 1;
        while((g[m[a * 9 + p % 9]] / b) % 10)
        {
          if((g[m[a * 9 + p % 9]] / b) % 10 == s[m[p]])
          {
            g[m[a * 9 + p % 9]] = (g[m[a * 9 + p % 9]] / b / 10) * b + g[m[a * 9 + p % 9]] % b;
            break;
          }
          b *= 10;
        }
        if(!g[m[a * 9 + p % 9]])
          break;
// sorting
      }
      if(!s[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]])
      {
        long b = 1;
        while((g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) % 10)
        {
          if((g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) % 10 == s[m[p]])
          {
            g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] = (g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b / 10) * b + g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] % b;
            break;
          }
          b *= 10;
        }
        if(!g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]])
          break;
// sorting
      }
    }
    print();
    if(a == 9 && solve(p + 1.0))
      return 1;
    x = s[m[p]];
    s[m[p]] = 0;
    for(a = 0; a < 9; a++)
    {
      if(!s[m[a + (p / 9) * 9]])
      {
        s[m[a + (p / 9) * 9]] = x;
        if(check(m[a + (p / 9) * 9]))
        {
          long b = 1;
          while((g[m[a + (p / 9) * 9]] / b) % 10)
          {
            if((g[m[a + (p / 9) * 9]] / b) % 10 >= x)
              break;
            b *= 10;
          }
          if((g[m[a + (p / 9) * 9]] / b) % 10 != x)
          {
            g[m[a + (p / 9) * 9]] = (g[m[a + (p / 9) * 9]] / b) * b * 10 + x * b + g[m[a + (p / 9) * 9]] % b;
// sorting
          }
        }
        s[m[a + (p / 9) * 9]] = 0;
      }
      if(!s[m[a * 9 + p % 9]])
      {
        s[m[a * 9 + p % 9]] = x;
        if(check(m[a * 9 + p % 9]))
        {
          long b = 1;
          while((g[m[a * 9 + p % 9]] / b) % 10)
          {
             if((g[m[a * 9 + p % 9]] / b) % 10 >= x)
                break;
             b *= 10;
          }
          if((g[m[a * 9 + p % 9]] / b) % 10 != x)
          {
            g[m[a * 9 + p % 9]] = (g[m[a * 9 + p % 9]] / b) * b * 10 + x * b + g[m[a * 9 + p % 9]] % b;
// sorting
          }
        }
        s[m[a * 9 + p % 9]] = 0;
      }
      if(!s[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]])
      {
        s[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] = x;
        if(check(m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]))
        {
          long b = 1;
          while((g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) % 10)
          {
             if((g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) % 10 >= x)
               break;
             b *= 10;
          }
          if((g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) % 10 != x)
          {
            g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] = (g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] / b) * b * 10 + x * b + g[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] % b;
// sorting
          }
        }
        s[m[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27]] = 0;
      }
    }
  }
  return 0;
}

bool invalid(void)
{
  short z = 0, n = 80;
  for(short p = 0; p < 81; p++)
    if(!s[p])
    {
      long b = 1;
      for(s[p] = 1; s[p] < 10; s[p]++)
        if(check(p))
        {
          g[p] += s[p] * b;
          b *= 10;
        }
      if(!g[p])
        return 1;
      s[p] = 0;
      m[n--] = p;
      for(short a = n + 1.0; a < 80; a++)
        if(g[m[a]] >= g[m[a + 1]])
        {
           short t = m[a];
           m[a] = m[a + 1];
           m[a + 1] = t;
        }
    }
    else
    {
      if(!check(p))
        return 1;
      m[z++] = p;
    }
  for(short p = 0; p < 81; p++)
    printf("%2d %2d %ld\n", p, m[p], g[m[p]]);
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
    short p = 0;
    while(p < 81 && s[m[p]])
      p++;
    if(solve(p))
      print();
    else
      printf("\nUnsolvable!\n");
  }
  printf("\nRun in %f ms.", (clock() - c) / CLOCKS_PER_SEC);
  getche();
}
