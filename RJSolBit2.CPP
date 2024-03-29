#include <stdio.h>
#include <time.h>

short q,                     // Total number of empty Cell positions
      r[81],                 // Used for sorting and eliminating empty Cell positions
      s[81];                 // Sudoku (0 to 80) Cell positions Board wise
long n;                      // Total number of guesses Board wise
bool g[729];                 // Possible Cell givens for each Cell position digit wise

long b2d(short p)
{
  long b = 1, z = 0;
  for(short a = 1; a < 10; a++)
    if(g[p * 9 + a - 1])
    {
      z += a * b;
      b *= 10;
    }
  return z;
}

bool solve(short p)
{
  short a, x = p;
  bool k[27] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0};
  for(a = p + 1.0; a < q; a++)
    if(b2d(r[x]) > b2d(r[a]))
      x = a;                 // Check Cell positions for least Cell givens
  if(x > p)
  {
    a = r[p];
    r[p] = r[x];
    r[x] = a;
  }
//  for(long z = 1; (s[r[p]] = short ((b2d(r[p]) / z) % 10)) > 0; n++, z *= 10)
  for(long z = b2d(r[p]); (s[r[p]] = short (z % 10)) > 0; n++, z /= 10)
  {                          // Assign Cell givens to Cell position digit wise
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (r[p] / 9) * 9] && g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit row wise
        k[a] = 1;            // Backup Cell position row wise
        g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate Cell digit from Cell givens row wise
        if(!b2d(short (a + (r[p] / 9) * 9)))
          break;             // Break on empty Cell givens row wise
      }
      if(!s[a * 9 + r[p] % 9] && g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit column wise
        k[a + 9] = 1;        // Backup Cell position column wise
        g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate Cell digit from Cell givens column wise
        if(!b2d(short (a * 9 + r[p] % 9)))
          break;             // Break on empty Cell givens column wise
      }
      if(!s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] && g[(a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit box wise
        k[a + 18] = 1;       // Backup Cell position box wise
        g[(a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate Cell digit from Cell givens box wise
        if(!b2d(short (a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27)))
          break;             // Break on empty Cell givens box wise
      }
    }
    if(a > 8 && (p + 1.0 >= q || solve(p + 1.0)))
      return 1;              // If all Cell givens row, column and box wise found and either all Cell positions filled or recursive solve for next empty Cell position
    for(a = 0; a < 9; a++)
    {
      if(k[a])               // Restore Cell digit into Cell givens row wise
      {
        k[a] = 0;
        g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1] = 1;
      }
      if(k[a + 9])           // Restore Cell digit into Cell givens column wise
      {
        k[a + 9] = 0;
        g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1] = 1;
      }
      if(k[a + 18])          // Restore Cell digit into Cell givens box wise
      {
        k[a + 18] = 0;
        g[(a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1] = 1;
      }
    }
  }
  return 0;
}

bool check(short p)
{
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
                             // Check duplicate Cell position Row wise
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
                             // Check duplicate Cell position Column wise
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
      return 0;              // Check duplicate Cell position Box wise
  return 1;
}

bool invalid(void)
{
  for(short p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell position
    {
      for(s[p] = 1; s[p] < 10; s[p]++)
                             // Assign Cell position from 1 to 9
        g[p * 9 + s[p] - 1] = check(p);
                             // Assign Cell givens to possible Cell digit
      if(!b2d(p))            // No Cell digit assigned
        return 1;
      r[q++] = p;            // Fill empty Cell position from 0 to 80 Sudoku Board
      s[p] = 0;              // Clear Cell position
    }
    else
      if(!check(p))          // Check all Cell constraint for pre define Cell position
        return 1;
  return 0;
}

void main(void)
{
  float c, d = 0, e = 0, f = 0;
  short a = 0;
  long t = 0, i = 0, u = 0, v = 0;
  int h;
  FILE *o;
  if((o = fopen("sudoku.txt", "r")) == NULL)
    printf("Unable to open sudoku.txt file for read !!");
  else
    do
      if((h = fgetc(o)) != 10 && h != EOF && a < 81)
        s[a++] = h >= '1' && h <= '9' ? h - '0' : 0.0;
      else                   // Assign Cell position to pre define Cell digit
        if(h == 10 || h == EOF)
        {
          while(a < 81)
            s[a++] = 0;      // Assign remaining Cell positions to 0
          n = q = 0;
          c = clock();
          if(invalid())
          {
            c = (clock() - c) / CLOCKS_PER_SEC * 1000;
            d += c;
            printf("%ld) Error: Invalid Sudoku! # I%ld", ++t, ++i);
          }
          else
            if(solve(0))
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              e += c;
              printf("%ld) ", ++t);
              for(a = 0; a < 81; a++)
                printf("%c", s[a] ? s[a] + '0' : '.');
              printf(" # S%ld", ++v);
            }
            else
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              f += c;
              printf("%ld) Error: Unsolvable Sudoku! # U%ld", ++t, ++u);
            }
          printf(" # N%ld # %f\n", n, c);
          a = 0;
        }
    while(h != EOF);
  printf("=======================================\n");
  printf("Total Sudoku puzzle read   : %ld\n", t);
  printf("Total time for all puzzles : %f\n", d + e + f);
  if(t)
    printf("Average time per puzzle    : %f\n", (d + e + f) / t);
  printf("Number of invalid puzzles  : %ld\n", i);
  printf("Time for invalid puzzles   : %f\n", d);
  if(i)
    printf("Average time per invalid   : %f\n", d / i);
  printf("Number of solved puzzles   : %ld\n", v);
  printf("Time for solved puzzles    : %f\n", e);
  if(v)
    printf("Average time per solved    : %f\n", e / v);
  printf("Number of unsolved puzzle  : %ld\n", u);
  printf("Time for unsolved puzzles  : %f\n", f);
  if(u)
    printf("Average time per unsolved  : %f\n", f / u);
  if(fclose(o) == EOF)
    printf("Unable to close sudoku.txt file !!");
}
