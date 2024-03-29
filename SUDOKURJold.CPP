#include <conio.h>
#include <time.h>
#include <stdlib.h>

unsigned short s[81] = {
8, 1, 2, 0, 0, 0, 0, 0, 0,
0, 0, 3, 6, 0, 0, 0, 0, 0,
0, 7, 0, 4, 9, 0, 2, 0, 0,

0, 5, 0, 0, 0, 7, 0, 0, 0,
0, 0, 0, 0, 4, 5, 7, 0, 0,
0, 0, 0, 1, 0, 0, 0, 3, 0,

0, 0, 1, 0, 0, 0, 0, 6, 8,
0, 0, 8, 5, 0, 0, 0, 1, 0,
0, 9, 0, 0, 0, 0, 4, 0, 0};

unsigned long i = 0;

void print(void)
{
  cprintf("+-------+-------+-------+\r\n");
  for(unsigned short a = 0; a < 81; a++)
  {
    if(a % 9 == 0)
      cprintf("|");
    cprintf("%2u", s[a]);
    if(a % 3 == 2)
      cprintf(" |");
    if(a % 9 == 8)
    {
      cprintf("\r\n");
      if(a % 27 == 26)
	cprintf("+-------+-------+-------+\r\n");
    }
  }
}

unsigned short check(unsigned short q)
{
  for(unsigned short a = 0; a < 9; a++)
  {
    cprintf("0 - %u - %u - %u - %u - %u - %u\r\n", a, q,
    s[a + (q / 9) * 9],
    s[a * 9 + (q % 9)],

    s[a % 3 + (a / 3) * 9 + (q / 3) * 3 + (q / 9) * 9],

    s[q]);

    if(a != q % 9 && s[a + (q / 9) * 9] == s[q])
    {
      cprintf("1 - %u - %u - %u - %u - %u\r\n", a, q, (q / 9) * 9, s[a + (q / 9) * 9], s[q]);
      return 0;
    }
    if(a != q / 9 && s[a * 9 + (q % 9)] == s[q])
    {
      cprintf("2 - %u - %u - %u - %u - %u\r\n", a, q, q / 9, s[a + (q % 9) * 9], s[q]);
      return 0;
    }
    if(a % 3 + (a / 3) * 9 != (q % 3) * 3 + (q % 9) * 9 && a / 3 + q / 27 != q / 9 && s[a % 3 + (a / 3) * 9 + (q / 3) * 3 + (q / 9) * 9] == s[q])
    {
      cprintf("3 - %u - %u - %u - %u - %u - %u - %u - %u\r\n", a, q, a % 3 + (q % 9) / 3, q % 9, a / 3 + q / 27, q / 9, s[(a % 3 + (q % 9) / 3) * 9 + a / 3 + q / 27], s[q]);
      return 0;
    }
  }
  return 1;
}

unsigned short solve(unsigned short p)
{
  while(p < 81 && s[p] > 0)
    p++;
  if(p > 80)
    return 1;
  for(s[p] = 1; s[p] < 10; s[p]++)
  {
    i++;
    if(check(p) == 1 && solve(p + 1) == 1)
      return 1;
  }
  s[p] = 0;
  return 0;
}

void main(void)
{
  clock_t t = clock();
  clrscr();
  if(solve(0) == 1)
    print();
  else
    cprintf("Unsolvable!\r\n");
  cprintf("Run %u iterations.\r\n", i);
  cprintf("%u ms.\r\n", double(clock() - t) / (double(CLOCKS_PER_SEC) / 1000.0));
}
