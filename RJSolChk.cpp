#include <stdio.h>
#include <time.h>

short q,                     // Total number of pre define Cell values
      r[81],                 // Used for sorting and eliminating filled Cell values
      s[81];                 // Sudoku (0 to 80) Cell values Board

long n,                      // Total number of guesses
     g[81];                  // Possible givens for each Cell value

static int b[512] = {        // Binary to Digit
  987654322,        1,        2,       21,        3,       31,       32,      321,
          4,       41,       42,      421,       43,      431,      432,     4321,
          5,       51,       52,      521,       53,      531,      532,     5321,
         54,      541,      542,     5421,      543,     5431,     5432,    54321,
          6,       61,       62,      621,       63,      631,      632,     6321,
         64,      641,      642,     6421,      643,     6431,     6432,    64321,
         65,      651,      652,     6521,      653,     6531,     6532,    65321,
        654,     6541,     6542,    65421,     6543,    65431,    65432,   654321,
          7,       71,       72,      721,       73,      731,      732,     7321,
         74,      741,      742,     7421,      743,     7431,     7432,    74321,
         75,      751,      752,     7521,      753,     7531,     7532,    75321,
        754,     7541,     7542,    75421,     7543,    75431,    75432,   754321,
         76,      761,      762,     7621,      763,     7631,     7632,    76321,
        764,     7641,     7642,    76421,     7643,    76431,    76432,   764321,
        765,     7651,     7652,    76521,     7653,    76531,    76532,   765321,
       7654,    76541,    76542,   765421,    76543,   765431,   765432,  7654321,
          8,       81,       82,      821,       83,      831,      832,     8321,
         84,      841,      842,     8421,      843,     8431,     8432,    84321,
         85,      851,      852,     8521,      853,     8531,     8532,    85321,
        854,     8541,     8542,     5421,     8543,    85431,    85432,   854321,
         86,      861,      862,     8621,      863,     8631,     8632,    86321,
        864,     8641,     8642,    86421,     8643,    86431,    86432,   864321,
        865,     8651,     8652,    86521,     8653,    86531,    86532,   865321,
       8654,    86541,    86542,   865421,    86543,   865431,   865432,  8654321,
         87,      871,      872,     8721,      873,     8731,     8732,    87321,
        874,     8741,     8742,    87421,     8743,    87431,    87432,   874321,
        875,     8751,     8752,    87521,     8753,    87531,    87532,   875321,
       8754,    87541,    87542,   875421,    87543,   875431,   875432,  8754321,
        876,     8761,     8762,    87621,     8763,    87631,    87632,   876321,
       8764,    87641,    87642,   876421,    87643,   876431,   876432,  8764321,
       8765,    87651,    87652,   876521,    87653,   876531,   876532,  8765321,
      87654,   876541,   876542,  8765421,   876543,  8765431,  8765432, 87654321,
          9,       91,       92,      921,       93,      931,      932,     9321,
         94,      941,      942,     9421,      943,     9431,     9432,    94321,
         95,      951,      952,     9521,      953,     9531,     9532,    95321,
        954,     9541,     9542,    95421,     9543,    95431,    95432,   954321,
         96,      961,      962,     9621,      963,     9631,     9632,    96321,
        964,     9641,     9642,    96421,     9643,    96431,    96432,   964321,
        965,     9651,     9652,    96521,     9653,    96531,    96532,   965321,
       9654,    96541,    96542,   965421,    96543,   965431,   965432,  9654321,
         97,      971,      972,     9721,      973,     9731,     9732,    97321,
        974,     9741,     9742,    97421,     9743,    97431,    97432,   974321,
        975,     9751,     9752,    97521,     9753,    97531,    97532,   975321,
       9754,    97541,    97542,   975421,    97543,   975431,   975432,  9754321,
        976,     9761,     9762,    97621,     9763,    97631,    97632,   976321,
       9764,    97641,    97642,   976421,    97643,   976431,   976432,  9764321,
       9765,    97651,    97652,   976521,    97653,   976531,   976532,  9765321,
      97654,   976541,   976542,  9765421,   976543,  9765431,  9765432, 97654321,
         98,      981,      982,     9821,      983,     9831,     9832,    98321,
        984,     9841,     9842,    98421,     9843,    98431,    98432,   984321,
        985,     9851,     9852,    98521,     9853,    98531,    98532,   985321,
       9854,    98541,    98542,    95421,    98543,   985431,   985432,  9854321,
        986,     9861,     9862,    98621,     9863,    98631,    98632,   986321,
       9864,    98641,    98642,   986421,    98643,   986431,   986432,  9864321,
       9865,    98651,    98652,   986521,    98653,   986531,   986532,  9865321,
      98654,   986541,   986542,  9865421,   986543,  9865431,  9865432, 98654321,
        987,     9871,     9872,    98721,     9873,    98731,    98732,   987321,
       9874,    98741,    98742,   987421,    98743,   987431,   987432,  9874321,
       9875,    98751,    98752,   987521,    98753,   987531,   987532,  9875321,
      98754,   987541,   987542,  9875421,   987543,  9875431,  9875432, 98754321,
       9876,    98761,    98762,   987621,    98763,   987631,   987632,  9876321,
      98764,   987641,   987642,  9876421,   987643,  9876431,  9876432, 98764321,
      98765,   987651,   987652,  9876521,   987653,  9876531,  9876532, 98765321,
     987654,  9876541,  9876542, 98765421,  9876543, 98765431, 98765432,987654321};

bool solve(short p)
{
  short a, x = p;
  if(p >= q)                 // All empty Cell values are filled
    return 1;
  for(a = p + 1; a < q; a++)
    if(b[g[r[x]]] > b[g[r[a]]])    // Check for least Cell givens
      x = a;
  if(x > p)
  {
    a = r[p];
    r[p] = r[x];
    r[x] = a;
  }
  for(int z = g[r[p]]; s[r[p]] = b[z & -z]; ++n, z &= (z - 1))
  {                          // Assign unit digit from Cell givens to Cell value
    long k[27] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(a = 0; a < 9; a++)
    {
      if(!s[a + (r[p] / 9) * 9])
      {                      // Check empty Cell value row wise
        for(long b = 1; (g[a + (r[p] / 9) * 9] / b) % 10; b *= 10)
                             // Loop each row Cell givens digit wise
          if((g[a + (r[p] / 9) * 9] / b) % 10 == s[r[p]])
          {                  // Found digit in row Cell givens
            k[a] = g[a + (r[p] / 9) * 9];
                             // Backup Cell givens row wise
            g[a + (r[p] / 9) * 9] = (g[a + (r[p] / 9) * 9] / b / 10) * b + g[a + (r[p] / 9) * 9] % b;
            break;           // Eleminate digit from row Cell givens
          }
        if(!g[a + (r[p] / 9) * 9])
          break;             // Break on empty row Cell givens
      }
      if(!s[a * 9 + r[p] % 9])
      {                      // Check empty Cell value column wise
        for(long b = 1; (g[a * 9 + r[p] % 9] / b) % 10; b *= 10)
                             // Loop each column Cell givens digit wise
          if((g[a * 9 + r[p] % 9] / b) % 10 == s[r[p]])
          {                  // Found digit in column Cell givens
            k[a + 9] = g[a * 9 + r[p] % 9];
                             // Backup Cell givens column wise
            g[a * 9 + r[p] % 9] = (g[a * 9 + r[p] % 9] / b / 10) * b + g[a * 9 + r[p] % 9] % b;
            break;           // Eleminate digit from column Cell givens
          }
        if(!g[a * 9 + r[p] % 9])
          break;             // Break on empty column Cell givens
      }
      if(!s[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27])
      {                      // Check empty Cell value box wise
        for(long b = 1; (g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10; b *= 10)
                             // Loop each box Cell givens digit wise
          if((g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b) % 10 == s[r[p]])
          {                  // Found digit in box Cell givens
            k[a + 18] = g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27];
                             // Backup Cell givens box wise
            g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = (g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] / b / 10) * b + g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] % b;
            break;           // Eleminate digit from box Cell givens
          }
        if(!g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27])
          break;             // Break on empty box Cell givens
      }
    }
    if(a > 8 && solve(p + 1))
      return 1;              // If all row, column and box Cell givens found then recursive solve for next Cell value
    for(a = 0; a < 9; a++)
    {
      if(k[a])               // Restore Cell givens row wise
        g[a + (r[p] / 9) * 9] = k[a];
      if(k[a + 9])           // Restore Cell givens column wise
        g[a * 9 + r[p] % 9] = k[a + 9];
      if(k[a + 18])          // Restore Cell givens box wise
        g[a % 3 + (a / 3) * 9 + ((r[p] % 9) / 3) * 3 + (r[p] / 27) * 27] = k[a + 18];
    }
  }
  return 0;
}

bool check(short p)
{
  for(short a = 0; a < 9; a++)
    if((a + (p / 9) * 9 != p && s[a + (p / 9) * 9] == s[p])
                             // Check duplicate Cell value in Row wise
    || (a * 9 + p % 9 != p && s[a * 9 + p % 9] == s[p])
                             // Check duplicate Cell value in Column wise
    || (a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27 != p && s[a % 3 + (a / 3) * 9 + ((p % 9) / 3) * 3 + (p / 27) * 27] == s[p]))
      return 0;              // Check duplicate Cell value in Box wise
  return 1;
}

bool invalid(void)
{
  q = 0;
  for(short p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell value
    {
//      long b = 1;
      for(g[p] = 0, s[p] = 1; s[p] < 10; s[p]++)
                             // Loop Cell value from 1 to 9 digit
        if(check(p))         // Check all Cell constraint
          g[p] |= 1 << (s[p] - 1);
	//        {
//          g[p] += s[p] * b;  // Assign Cell givens to digit
//          b *= 10;
//        }
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

int main(void)
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
        s[a++] = h >= '1' && h <= '9' ? h - '0' : 0;
      else
        if(h == 10 || h == EOF)
        {
          while(a < 81)
            s[a++] = 0;
          n = 0;
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
  printf("Time for unsoled puzzles   : %f\n", f);
  if(u)
    printf("Average time per unsolved  : %f\n", f / u);
  if(fclose(o) == EOF)
    printf("Unable to close sudoku.txt file !!");
}
