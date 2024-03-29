#include <stdio.h>
#include <time.h>

short s[81];                 // Sudoku (0 to 80) Cells Board

long g[81];                  // Possible each Cell givens

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
    if(s[a])                 // Skip already filled Cell values
      continue;
    if(p > 80)               // First empty Cell value to check
      p = a;
    else
      if(g[p] > g[a])
        p = a;
  }
  if(p > 80)                 // All Cell values are filled
    return 1;
  for(long r = g[p]; r > 0; r /= 10)
  {                          // Eleminate unit digit from Cell givens till become zero
    s[p] = (short) (r % 10); // Assign unit digit from Cell givens to Cell value
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (p / 9) * 9])
      {                      // Check empty Cell value row wise
        long b = 1;
        while((g[a + (p / 9) * 9] / b) % 10)
        {                    // Loop each row Cell givens digit wise
          if((g[a + (p / 9) * 9] / b) % 10 == s[p])
          {                  // Found digit in row Cell givens
            g[a + (p / 9) * 9] = (g[a + (p / 9) * 9] / b / 10) * b + g[a + (p / 9) * 9] % b;
            break;           // Eleminate digit from row Cell givens
          }
          b *= 10;
        }
        if(!g[a + (p / 9) * 9])
          break;             // Break on empty row Cell givens
      }
      if(!s[a * 9 + p % 9])  // Check empty Cell value column wise
      {
        long b = 1;
        while((g[a * 9 + p % 9] / b) % 10)
        {                    // Loop each column Cell givens digit wise
          if((g[a * 9 + p % 9] / b) % 10 == s[p])
          {                  // Found digit in column Cell givens
            g[a * 9 + p % 9] = (g[a * 9 + p % 9] / b / 10) * b + g[a * 9 + p % 9] % b;
            break;           // Eleminate digit from column Cell givens
          }
          b *= 10;
        }
        if(!g[a * 9 + p % 9])// Break on empty column Cell givens
          break;
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {                      // Check empty Cell value box wise
        long b = 1;
        while((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10)
        {                    // Loop each box Cell givens digit wise
          if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 == s[p])
          {                  // Found digit in box Cell givens
            g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b / 10) * b + g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] % b;
            break;           // Eleminate digit from box Cell givens
          }
          b *= 10;
        }
        if(!g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
          break;             // Break on empty box Cell givens
      }
    }
    if(a > 8 && solve())    // If all row, column and box Cell givens found then recursive solve for next cell value
      return 1;
    x = s[p];                // Assign temporary variable as Cell value
    s[p] = 0;                // Clear Cell value
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (p / 9) * 9])
      {                       // Check empty Cell value row wise
        s[a + (p / 9) * 9] = x;
                             // Assign temporary Cell value
        if(check(a + (p / 9) * 9.0))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a + (p / 9) * 9] / b) % 10)
          {                  // Loop each row Cell givens digit wise
            if((g[a + (p / 9) * 9] / b) % 10 >= x)
              break;         // Found digit place in row Cell givens
            b *= 10;
          }
          if((g[a + (p / 9) * 9] / b) % 10 != x)
                             // Check digit does not exist in row Cell givens
            g[a + (p / 9) * 9] = (g[a + (p / 9) * 9] / b) * b * 10 + x * b + g[a + (p / 9) * 9] % b;
                             // Place digit back in row Cell givens
        }
        s[a + (p / 9) * 9] = 0;
                             // Clear row Cell value
      }
      if(!s[a * 9 + p % 9])  // Check empty Cell value column wise
      {
        s[a * 9 + p % 9] = x;// Assign temporary Cell value
        if(check(a * 9.0 + p % 9))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a * 9 + p % 9] / b) % 10)
          {                  // Loop each column Cell givens digit wise
             if((g[a * 9 + p % 9] / b) % 10 >= x)
               break;        // Found digit place in column Cell givens
             b *= 10;
          }
          if((g[a * 9 + p % 9] / b) % 10 != x)
                             // Check digit does not exist in column Cell givens
            g[a * 9 + p % 9] = (g[a * 9 + p % 9] / b) * b * 10 + x * b + g[a * 9 + p % 9] % b;
                             // Place digit back in column Cell givens
        }
        s[a * 9 + p % 9] = 0;// Clear column Cell value
      }
      if(!s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27])
      {                      // Check empty Cell value box wise
        s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = x;
                             // Assign temporary Cell value
        if(check(a % 3 + (a / 3) * 9.0 + ((p % 9) / 3) * 3.0 + (p / 27) * 27.0))
        {                    // Check all Cell constraint
          long b = 1;
          while((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10)
          {                  // Loop each box Cell givens digit wise
             if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 >= x)
               break;        // Found digit place in box Cell givens
             b *= 10;
          }
          if((g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) % 10 != x)
                             // Check digit does not exist in box Cell givens
            g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] / b) * b * 10 + x * b + g[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] % b;
                             // Place digit back in box Cell givens
        }
        s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] = 0;
                             // Clear box Cell value
      }
    }
  }
  return 0;
}

bool invalid(void)
{
//  short a = 80, b = 0;
  for(short p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell value
    {
      long b = 1;
      for(s[p] = 1; s[p] < 10; s[p]++)
                             // Loop Cell value from 1 to 9 digit
        if(check(p))         // Check all Cell constraint
        {
          g[p] += s[p] * b;  // Assign Cell givens to digit
          b *= 10;
        }
      if(!g[p])              // No digit assigned to Cell value
        return 1;
//      r[a--] = p;            // Fill empty Cell position from 80 to 0
      s[p] = 0;              // Clear Cell value
    }
    else
    {
      if(!check(p))          // Check all constraint for Cell pre givens
        return 1;
//      r[b++] = p;            // Fill given Cell position from 0 to 80
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
            printf("%ld) Error: Invalid Sudoku given. # %.0f", ++t, ++i);
          }
          else
          {
            if(solve())
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              e += c;
              printf("%ld) ", ++t);
              for(a = 0; a < 80; a++)
                printf("%c", s[a] ? s[a] + '0' : '.');
              printf(" # %.0f", ++v);
            }
            else
            {
              c = (clock() - c) / CLOCKS_PER_SEC * 1000;
              f += c;
              printf("%ld) Unsolvable! # %.0f", ++t, ++u);
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
