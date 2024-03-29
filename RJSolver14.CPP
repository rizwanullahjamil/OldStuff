#include <stdio.h>
#include <time.h>

short q,                     // Total number of pre define Cell values
      r[81],                 // Used for sorting and eliminating filled Cell values
      s[81];                 // Sudoku (0 to 80) Cell values Board

long g[81];                  // Possible givens of each Cell value

bool check(short p)
{
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
                             // Check duplicate Cell value in Row wise
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
                             // Check duplicate Cell value in Column wise
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
                             // Check duplicate Cell value in Box wise
      return 0;
  return 1;
}

bool solve(short p)
{
  short a, x = p;
  long k[27];                // Backup Cell givens as 0-8 row, 9-17 column and 18-26 box wise
  if(p >= q)                 // All Cell values are filled
    return 1;
  for(a = p + 1.0; a < q; a++)
    if(g[r[x]] > g[r[a]])
      x = a;
  a = r[p];
  r[p] = r[x];
  r[x] = a;
  for(long z = g[r[p]]; z > 0; z /= 10)
  {                          // Eleminate unit digit from Cell givens till become zero
    s[r[p]] = (short) (z % 10);
                             // Assign unit digit from Cell givens to Cell value
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (r[p] / 9) * 9])
      {                      // Check empty Cell value row wise
        long b = 1;
        while((g[a + (r[p] / 9) * 9] / b) % 10)
        {                    // Loop each row Cell givens digit wise
          if((g[a + (r[p] / 9) * 9] / b) % 10 == s[r[p]])
          {                  // Found digit in row Cell givens
            k[a] = g[a + (r[p] / 9) * 9];
                             // Backup Cell givens row wise
            g[a + (r[p] / 9) * 9] = (g[a + (r[p] / 9) * 9] / b / 10) * b + g[a + (r[p] / 9) * 9] % b;
            break;           // Eleminate digit from row Cell givens
          }
          b *= 10;
        }
        if(!g[a + (r[p] / 9) * 9])
          break;             // Break on empty row Cell givens
      }
      if(!s[a * 9 + r[p] % 9])
      {                      // Check empty Cell value column wise
        long b = 1;
        while((g[a * 9 + r[p] % 9] / b) % 10)
        {                    // Loop each column Cell givens digit wise
          if((g[a * 9 + r[p] % 9] / b) % 10 == s[r[p]])
          {                  // Found digit in column Cell givens
            k[a + 9] = g[a * 9 + r[p] % 9];
                             // Backup Cell givens column wise
            g[a * 9 + r[p] % 9] = (g[a * 9 + r[p] % 9] / b / 10) * b + g[a * 9 + r[p] % 9] % b;
            break;           // Eleminate digit from column Cell givens
          }
          b *= 10;
        }
        if(!g[a * 9 + r[p] % 9])
          break;             // Break on empty column Cell givens
      }
      if(!s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27])
      {                      // Check empty Cell value box wise
        long b = 1;
        while((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10)
        {                    // Loop each box Cell givens digit wise
          if((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10 == s[r[p]])
          {                  // Found digit in box Cell givens
            k[a + 18] = g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27];
                             // Backup Cell givens box wise
            g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b / 10) * b + g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] % b;
            break;           // Eleminate digit from box Cell givens
          }
          b *= 10;
        }
        if(!g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27])
          break;             // Break on empty box Cell givens
      }
    }
    if(a > 8 && solve(p + 1.0))
      return 1;              // If all row, column and box Cell givens found then recursive solve for next Cell value
//    x = s[r[p]];             // Assign temporary variable as Cell value
//    s[r[p]] = 0;             // Clear Cell value
    for(a = 0; a < 9; a++)
    {
      if(!k[a])
      {
        g[a + (r[p] / 9) * 9] = k[a];
        k[a] = 0;
      }
/*
      if(!s[a + (r[p] / 9) * 9])
      {                      // Check empty Cell value row wise
        s[a + (r[p] / 9) * 9] = x;
                             // Assign temporary Cell value
        if(check(a + (r[p] / 9) * 9.0))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a + (r[p] / 9) * 9] / b) % 10)
          {                  // Loop each row Cell givens digit wise
            if((g[a + (r[p] / 9) * 9] / b) % 10 >= x)
              break;         // Found digit place in row Cell givens
            b *= 10;
          }
          if((g[a + (r[p] / 9) * 9] / b) % 10 != x)
                             // Check digit does not exist in row Cell givens
            g[a + (r[p] / 9) * 9] = (g[a + (r[p] / 9) * 9] / b) * b * 10 + x * b + g[a + (r[p] / 9) * 9] % b;
        }                    // Place digit back in row Cell givens
        s[a + (r[p] / 9) * 9] = 0;
      }                      // Clear row Cell value
*/
      if(!k[a + 9])
      {
        g[a * 9 + r[p] % 9] = k[a + 9];
        k[a + 9] = 0;
      }
/*
      if(!s[a * 9 + r[p] % 9])
      {                      // Check empty Cell value column wise
        s[a * 9 + r[p] % 9] = x;
                             // Assign temporary Cell value
        if(check(a * 9.0 + r[p] % 9))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a * 9 + r[p] % 9] / b) % 10)
          {                  // Loop each column Cell givens digit wise
             if((g[a * 9 + r[p] % 9] / b) % 10 >= x)
               break;        // Found digit place in column Cell givens
             b *= 10;
          }
          if((g[a * 9 + r[p] % 9] / b) % 10 != x)
                             // Check digit does not exist in column Cell givens
            g[a * 9 + r[p] % 9] = (g[a * 9 + r[p] % 9] / b) * b * 10 + x * b + g[a * 9 + r[p] % 9] % b;
        }                    // Place digit back in column Cell givens
        s[a * 9 + r[p] % 9] = 0;
      }                      // Clear column Cell value
*/
      if(!k[a + 18])
      {
        g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = k[a + 18];
        k[a + 18] = 0;
      }
/*
      if(!s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27])
      {                      // Check empty Cell value box wise
        s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = x;
                             // Assign temporary Cell value
        if(check(a % 3 + (a / 3) * 9.0 + ((r[p] % 9) / 3) * 3.0 + (r[p] / 27) * 27.0))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10)
          {                  // Loop each box Cell givens digit wise
             if((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10 >= x)
               break;        // Found digit place in box Cell givens
             b *= 10;
          }
          if((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10 != x)
                             // Check digit does not exist in box Cell givens
            g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) * b * 10 + x * b + g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] % b;
        }                    // Place digit back in box Cell givens
        s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = 0;
      }                      // Clear box Cell value
*/
    }
  }
  return 0;
}

bool invalid(void)
{
  q = 0;
  for(short p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell value
    {
      long b = 1;
      for(g[p] = 0, s[p] = 1; s[p] < 10; s[p]++)
                             // Loop Cell value from 1 to 9 digit
        if(check(p))         // Check all Cell constraint
        {
          g[p] += s[p] * b;  // Assign Cell givens to digit
          b *= 10;
        }
      if(!g[p])              // No digit assigned
        return 1;
      r[q++] = p;            // Fill empty Cell position from 0 to 80
      s[p] = 0;              // Clear Cell value
    }
    else
      if(!check(p))          // Check all Cell constraint for pre define Cell value
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
      else
        if(h == 10 || h == EOF)
        {
          while(a < 81)
            s[a++] = 0;
          c = clock();
          if(invalid())
          {
            c = clock() - c;
            d += c;
            printf("%ld) Error: Invalid Sudoku given! # I%ld", ++t, ++i);
          }
          else
            if(solve(0))
            {
              c = clock() - c;
              e += c;
              printf("%ld) ", ++t);
              for(a = 0; a < 81; a++)
                printf("%c", s[a] ? s[a] + '0' : '.');
              printf(" # S%ld", ++v);
            }
            else
            {
              c = clock() - c;
              f += c;
              printf("%ld) Error: Unsolvable! # U%ld", ++t, ++u);
            }
          printf(" # %f\n", c / CLOCKS_PER_SEC * 1000);
          a = 0;
        }
    while(h != EOF);
  printf("=======================================\n");
  printf("Total Sudoku puzzle read   : %ld\n", t);
  printf("Total time for all puzzles : %f\n", (d + e + f) / CLOCKS_PER_SEC * 1000);
  if(t)
    printf("Average time per puzzle    : %f\n", ((d + e + f) / CLOCKS_PER_SEC * 1000) / t);
  printf("Number of invalid puzzles  : %ld\n", i);
  printf("Time for invalid puzzles   : %f\n", d / CLOCKS_PER_SEC * 1000);
  if(i)
    printf("Average time per invalid   : %f\n", (d / CLOCKS_PER_SEC * 1000) / i);
  printf("Number of solved puzzles   : %ld\n", v);
  printf("Time for solved puzzles    : %f\n", e / CLOCKS_PER_SEC * 1000);
  if(v)
    printf("Average time per solved    : %f\n", (e / CLOCKS_PER_SEC * 1000) / v);
  printf("Number of unsolved puzzle  : %ld\n", u);
  printf("Time for unsoled puzzles   : %f\n", f / CLOCKS_PER_SEC * 1000);
  if(u)
    printf("Average time per unsolved  : %f\n", (f / CLOCKS_PER_SEC * 1000) / u);
  if(fclose(o) == EOF)
    printf("Unable to close sudoku.txt file");
}
