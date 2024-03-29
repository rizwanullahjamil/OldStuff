#include <stdio.h>
#include <conio.h>
#include <time.h>

short r[81],                 // Used for sorting and eliminating filled cells
      s[81];                 // Sudoku (0 to 80) cells Board

long g[81];                  // Possible given values of each cell

bool check(short p)
{
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
                             // Check Row wise
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
                             // Check Column wise
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
                             // Check Box wise
      return 0;
  return 1;
}

bool solve(void)
{
  short a, p = 81, x;
  for(a = 0; a < 81; a++)
  {
    if(s[a])                 // Skip already filled cell
      continue;
    if(p > 80)               // First empty cell to check
      p = a;
    else
      if(g[p] > g[a])
        p = a;
  }
  if(p > 80)                 // All cells are filled
    return 1;
  for(long r = g[p]; r > 0; r /= 10)
  {                          // Eleminate unit digit from cell givens till become zero
    s[p] = (short) (r % 10); // Assign unit digit from givens to cell
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (p / 9) * 9])
      {                      // Check empty cell row wise
        long b = 1;
        while((g[a + (p / 9) * 9] / b) % 10)
        {                    // Loop each row cell givens digit wise
          if((g[a + (p / 9) * 9] / b) % 10 == s[p])
          {                  // Found digit in row cell givens
            g[a + (p / 9) * 9] = (g[a + (p / 9) * 9] / b / 10) * b + g[a + (p / 9) * 9] % b;
            break;           // Eleminate digit from row cell givens
          }
          b *= 10;
        }
        if(!g[a + (p / 9) * 9])
          break;             // Break on empty row cell givens
      }
      if(!s[a * 9 + p % 9])  // Check empty cell column wise
      {
        long b = 1;
        while((g[a * 9 + p % 9] / b) % 10)
        {                    // Loop each column cell givens digit wise
          if((g[a * 9 + p % 9] / b) % 10 == s[p])
          {                  // Found digit in column cell givens
            g[a * 9 + p % 9] = (g[a * 9 + p % 9] / b / 10) * b + g[a * 9 + p % 9] % b;
            break;           // Eleminate digit from column cell givens
          }
          b *= 10;
        }
        if(!g[a * 9 + p % 9])// Break on empty column cell givens
          break;
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {                      // Check empty cell box wise
        long b = 1;
        while((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10)
        {                    // Loop each box cell givens digit wise
          if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 == s[p])
          {                  // Found digit in box cell givens
            g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b / 10) * b + g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] % b;
            break;           // Eleminate digit from box cell givens
          }
          b *= 10;
        }
        if(!g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
          break;             // Break on empty box cell givens
      }
    }
    if(a > 8 && solve())    // If all row, column and box cell givens found then recursive solve for next cell
      return 1;
    x = s[p];                // Assign temporary variable as cell value
    s[p] = 0;                // Clear cell value
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (p / 9) * 9])
      {                       // Check empty cell row wise
        s[a + (p / 9) * 9] = x;
                             // Assign temporary cell value
        if(check(a + (p / 9) * 9.0))
        {                    // Check all constraint
          long b = 1;
          while((g[a + (p / 9) * 9] / b) % 10)
          {                  // Loop each row cell givens digit wise
            if((g[a + (p / 9) * 9] / b) % 10 >= x)
              break;         // Found digit place in row cell givens
            b *= 10;
          }
          if((g[a + (p / 9) * 9] / b) % 10 != x)
                             // Check digit does not exist in row cell givens
            g[a + (p / 9) * 9] = (g[a + (p / 9) * 9] / b) * b * 10 + x * b + g[a + (p / 9) * 9] % b;
                             // Place digit back in row cell givens
        }
        s[a + (p / 9) * 9] = 0;
                             // Clear row cell value
      }
      if(!s[a * 9 + p % 9])  // Check empty cell column wise
      {
        s[a * 9 + p % 9] = x;// Assign temporary cell value
        if(check(a * 9.0 + p % 9))
        {                    // Check all constraint
          long b = 1;
          while((g[a * 9 + p % 9] / b) % 10)
          {                  // Loop each column cell givens digit wise
             if((g[a * 9 + p % 9] / b) % 10 >= x)
               break;        // Found digit place in column cell givens
             b *= 10;
          }
          if((g[a * 9 + p % 9] / b) % 10 != x)
                             // Check digit does not exist in column cell givens
            g[a * 9 + p % 9] = (g[a * 9 + p % 9] / b) * b * 10 + x * b + g[a * 9 + p % 9] % b;
                             // Place digit back in column cell givens
        }
        s[a * 9 + p % 9] = 0;// Clear column cell value
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {                      // Check empty cell box wise
        s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = x;
                             // Assign temporary cell value
        if(check(a % 3 + (a / 3) * 9.0 + ((p % 9) / 3) * 3.0 + (p / 27) * 27.0))
        {                    // Check all constraint
          long b = 1;
          while((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10)
          {                  // Loop each box cell givens digit wise
             if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 >= x)
               break;        // Found digit place in box cell givens
             b *= 10;
          }
          if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 != x)
                             // Check digit does not exist in box cell givens
            g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) * b * 10 + x * b + g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] % b;
                             // Place digit back in box cell givens
        }
        s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = 0;
                             // Clear box cell value
      }
    }
  }
  return 0;
}

bool invalid(void)
{
  short a = 80, b = 0;
  for(short p = 0; p < 81; p++)
    if(!s[p])                // Found empty cell value
    {
      long b = 1;
      for(s[p] = 1; s[p] < 10; s[p]++)
                             // Loop cell value from 1 to 9 digit
        if(check(p))         // Check 1-9 digit constraint
        {
          g[p] += s[p] * b;  // Assign cell givens to digit
          b *= 10;
        }
      if(!g[p])              // No digit assigned
        return 1;
      r[a--] = p;            // Fill empty Cell position from 80 to 0
      s[p] = 0;              // Clear cell value
    }
    else
    {
      if(!check(p))          // Check all constraint for pre givens
        return 1;
      r[b++] = p;            // Fill given Cell position from 0 to 80
    }
  return 0;
}

void main(void)
{
  clock_t c, d = 0, e = 0, f = 0;
  short a = 0;
  long t = 0;
  double i = 0, u = 0, v = 0;
  int h;
  FILE *r;
  if((r = fopen("sudoku.txt", "r")) == NULL)
    printf("Unable to open sudoku.txt file for read !!");
  else
    do
    {
      h = fgetc(r);
      if(h != 10 && h != EOF && a < 81)
        s[a++] = h >= '1' && h <= '9' ? h - '0' : 0.0;
      else
        if(h == 10 || h == EOF)
        {
          for(; a < 81; a++)
            s[a] = 0;
          c = clock();
          if(invalid())
          {
            c = (clock() - c) / CLOCKS_PER_SEC * 1000;
            d += c;
            printf("%ld) Error: Invalid Sudoku given. # %ld", ++t, ++i);
          }
          else
          {
            for(short p = 0; p < 81; p++)
              if(s[r[p]])
                break;
            if(solve(p + 1))
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              e += c;
              printf("%ld) ", ++t);
              for(a = 0; a < 80; a++)
                printf("%c", s[a] ? s[a] + '0' : '.');
              printf(" # %ld", ++v);
            }
            else
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              f += c;
              printf("%ld) Unsolvable! # %ld", ++t, ++u);
            }
            printf(" # %f\n", c);
          }
          a = 0;
        }
    }
  while(h != EOF);
  printf("=======================================");
  printf("Total Sudoku puzzle read   : %ld\n", t);
  printf("Total time for all puzzles : %f\n", d + e + f);
  printf("Average time per puzzle    : %f\n", (d + e + f) / t);
  printf("Number of invalid puzzle   : %ld\n", i);
  printf("Time for invalid puzzle    : %f\n", d);
  printf("Average time per invalid   : %f\n", d / i);
  printf("Number of puzzle solved    : %ld\n", v);
  printf("Time to solve puzzles      : %f\n", e);
  printf("Average time per solve     : %f\n", e / v);
  printf("Number of unsolvables      : %ld\n", u);
  printf("Time of unsolvable puzzles : %f\n", f);
  printf("Average time per unsolvable: %f\n", f / u);
  if(fclose(r) == EOF)
    printf("Unable to close sudoku.txt file");
}
