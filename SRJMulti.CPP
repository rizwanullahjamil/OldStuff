#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

short s[81];
long i;
clock_t t;

void print(void)
{
  for(short a = 0; a < 81; a++)
    printf("%1d", s[a]);
}

bool check(short q)
{
  i++;
  for(short a = 0; a < 9; a++)
    if((a + (q / 9) * 9 != q && s[a + (q / 9) * 9] == s[q])
    || (a * 9 + q % 9 != q && s[a * 9 + q % 9] == s[q])
    || (a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27 != q && s[a % 3 + (a / 3) * 9 + ((q % 9) / 3) * 3 + (q / 27) * 27] == s[q]))
      return 0;
  return 1;
}

bool solve(short p)
{
  while(p < 81 && s[p])
    p++;
  if(p > 80)
  {
    print();
    printf("\n");
//    printf(" - %u\n", i /*, double(clock() - t) / (double(CLOCKS_PER_SEC) / 1000.0) */);
    i = 0;
/*
    printf("Would you like to continue searching another solution Y/N ? ");
    if(abs(105 - getche()) + 73 != 'Y')
      return 1;
    printf("\n");
*/
  }
  for(s[p] = 1; s[p] < 10; s[p]++)
    if(check(p) && solve(p + 1.0))
      return 1;
  return s[p] = 0;
}

void main(void)
{
  clrscr();
  t = clock();
  if(!solve(0))
    printf("Unsolvable!\nRun %u iterations in %u ms.\n", i, double(clock() - t) / (double(CLOCKS_PER_SEC) / 1000.0));
}
