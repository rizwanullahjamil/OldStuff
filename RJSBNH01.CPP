#include <stdio.h>
#include <time.h>

int q,                       // Total number of empty Cell positions
    r[81],                   // Used for sorting and eliminating empty Cell positions
    s[81];                   // Sudoku (0 to 80) Cell positions Board wise

long n,                      // Total number of guesses Board wise
     h;                      // Total number of Hidden Singles Board wise

bool g[729];                 // Possible Cell givens for each Cell position digit wise

long b2d(int p)              // Convert possible Cell givens from boolean to digits
{
  long b = 1,
       z = 0;
  for(int a = 1; a < 10; a++)
    if(g[p + a])
    {
      z += a * b;
      b *= 10;
    }
  return z;
}

bool solve(int p)
{
  int a,
      x = p,
      zr,
      zc,
      zb;
  long z;
  for(a = p + 1; a < q; a++)
    if(b2d(r[x] * 9 - 1) > b2d(r[a] * 9 - 1))
      x = a;                 // Check Cell positions for least possible Cell givens
  if((z = b2d(r[x] * 9 - 1)) > 9)
                             // Check Hidden Single if least possible Cell givens is not Naked Single
    for(int b = 0; b < 9; b++)
    {
      for(a = 1; a < 10; a++)
      {
        int rr = 0,
            cc = 0,
            bb = 0;
        for(int y = 0; y < 9; y++)
        {
          if(!s[b + y * 9] && g[(b + y * 9) * 9 + a - 1])
          {                  // Check Hidden Cell digit row wise
            rr++;
            zr = y;
          }
          if(!s[b * 9 + y] && g[(b * 9 + y) * 9 + a - 1])
          {                  // Check Hidden Cell digit column wise
            cc++;
            zc = y;
          }
          if(!s[b * 3 + (b / 3) * 18 + y + (y / 3) * 6] && g[(b * 3 + (b / 3) * 18 + y + (y / 3) * 6) * 9 + a - 1])
          {                  // Check Hidden Cell digit box wise
            bb++;
            zb = y;
          }
        }
        if(rr == 1 || cc == 1 || bb == 1)
        {                    // Found Hidden Single
          z = a;             // Assign Hidden Single Cell digit
          if(rr == 1)        // Found Hidden Single row wise
            a = b + zr * 9;
          else
            if(cc == 1)      // Found Hidden Single column wise
              a = b * 9 + zc;
            else             // Found Hidden Single box wise
              a = b * 3 + (b / 3) * 18 + zb + (zb / 3) * 6;
          for(x = p; x < q; x++)
            if(r[x] == a)
              break;
          h++;
          a = -1;
          break;
        }
      }
      if(a < 0)              // Found Hidden Single
        break;
    }
  if(x > p)
  {
    a = r[p];
    r[p] = r[x];
    r[x] = a;
  }
  for(; (s[r[p]] = z % 10) > 0; n++, z /= 10)
  {                          // Assign possible Cell givens to Cell position digit wise
    bool k[27] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (r[p] / 9) * 9] && g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit row wise
        k[a] = 1;            // Backup Cell position row wise
        g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate possible Cell digit from possible Cell givens row wise
        if(!b2d((a + (r[p] / 9) * 9) * 9 - 1))
          break;             // Break on empty possible Cell givens row wise
      }
      if(!s[a * 9 + r[p] % 9] && g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit column wise
        k[a + 9] = 1;        // Backup Cell position column wise
        g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate possible Cell digit from possible Cell givens column wise
        if(!b2d((a * 9 + r[p] % 9) * 9 - 1))
          break;             // Break on empty possible Cell givens column wise
      }
      if(!s[a + (a / 3) * 6 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] && g[(a + (a / 3) * 6 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1])
      {                      // Check empty Cell position with possible Cell digit box wise
        k[a + 18] = 1;       // Backup Cell position box wise
        g[(a + (a / 3) * 6 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1] = 0;
                             // Eliminate possible Cell digit from possible Cell givens box wise
        if(!b2d((a + (a / 3) * 6 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 - 1))
          break;             // Break on empty possible Cell givens box wise
      }
    }
    if(a > 8 && (p + 1 >= q || solve(p + 1)))
      return 1;              // If all possible Cell givens row, column and box wise found and either all Cell positions filled or recursive solve for next empty Cell position
    for(a = 0; a < 9; a++)
    {
      if(k[a])               // Restore possible Cell digit into possible Cell givens row wise
        g[(a + (r[p] / 9) * 9) * 9 + s[r[p]] - 1] = 1;
      if(k[a + 9])           // Restore possible Cell digit into possible Cell givens column wise
        g[(a * 9 + r[p] % 9) * 9 + s[r[p]] - 1] = 1;
      if(k[a + 18])          // Restore possible Cell digit into possible Cell givens box wise
        g[(a + (a / 3) * 6 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27) * 9 + s[r[p]] - 1] = 1;
    }
  }
  return 0;
}

bool check(int p)
{
  for(int a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
                             // Check duplicate Cell position Row wise
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
                             // Check duplicate Cell position Column wise
    || (a + (a / 3) * 6 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a + (a / 3) * 6 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
      return 0;              // Check duplicate Cell position Box wise
  return 1;
}

bool invalid(void)
{
  for(int p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell position
    {
      for(s[p] = 1; s[p] < 10; s[p]++)
                             // Assign Cell position from 1 to 9
        g[p * 9 + s[p] - 1] = check(p);
                             // Check Cell constraint and assign possible Cell digit to possible Cell givens
      if(!b2d(p * 9 - 1))    // No possible Cell digit assigned
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
  int a = 0,
      m;
  float c,
        d = 0,
        e = 0,
        f = 0;
  long t = 0,
       i = 0,
       u = 0,
       v = 0;
  FILE *o;
  if((o = fopen("sudoku.txt", "r")) == NULL)
    printf("Unable to open sudoku.txt file for read !!");
  else
    do
      if((m = fgetc(o)) != 10 && m != EOF && a < 81)
        s[a++] = m >= '1' && m <= '9' ? m - '0' : 0;
      else                   // Assign Cell position to pre define Cell digit
        if(m == 10 || m == EOF)
        {
          while(a < 81)
            s[a++] = 0;      // Assign remaining Cell positions to 0
          n = h = q = 0;
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
          printf(" # N%ld # H%ld # %f\n", n, h, c);
          a = 0;
        }
    while(m != EOF);
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
