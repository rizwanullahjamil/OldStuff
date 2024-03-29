// pj downey algorithm
// written by Paul J. Downey in C++

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

int main (void)
{
  short a[82] = { 0,
  0, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 1, 0, 0, 5, 6, 0, 0,
  0, 9, 0, 0, 4, 0, 0, 7, 0,
  0, 0, 0, 0, 0, 9, 0, 5, 0,
  7, 0, 0, 0, 0, 0, 0, 0, 8,
  0, 5, 0, 4, 0, 2, 0, 0, 0,
  0, 8, 0, 0, 2, 0, 0, 9, 0,
  0, 0, 3, 5, 0, 0, 1, 0, 0,
  6, 0, 0, 0, 0, 0, 0, 0, 0},
/*  0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 3, 0, 2, 0, 0, 0, 0, 5,
  7, 0, 6, 0, 0, 8, 2, 1, 9,
  0, 0, 0, 0, 0, 7, 3, 0, 0,
  0, 0, 9, 0, 3, 0, 8, 0, 0,
  0, 0, 2, 1, 0, 0, 0, 0, 0,
  2, 8, 1, 4, 0, 0, 6, 0, 0,
  6, 0, 0, 0, 0, 2, 0, 7, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 0},*/
  g, i, j, k, l, m, n, p, s, t,
  b[82][3], c[82][25], e[82];
  int v = 0;
// setting up the cross checking legality array. Each box has 20 constraints
  for (i = 1; i < 82; ++i)
    for (t = 0,
         j = 1; j < 10; ++j)
    {
      s = ((i - 1) / 9) * 9 + j;
      if (s != i)
        c[i][++t] = s;
    }
  for (i = 1; i < 82; ++i)
    for (t = 8,
         j = 1; j < 10; ++j)
    {
      s = i - ((i - 1) / 9) * 9 + (j - 1) * 9;
      if (s != i)
        c[i][++t] = s;
    }
  for (i = 1; i < 82; ++i)
    for (t = 16,
         k = (((i - 1) / 9) / 3) * 27,
         l = ((i - ((i - 1) / 9) * 9 - 1) / 3) * 3,
         m = 0; m < 19; m += 9)
      for (n = 1; n < 4; ++n)
      {
        s = k + l + m + n;
        if (s != i)
          c[i][++t] = s;
      }
  for (i = 1; i < 82; ++i)
    for (t = 17,
         j = 17; j < 25; ++j)
    {
      for (l = 1,
           k = 1; k < 17; ++k)
        if (c[i][j] == c[i][k])
          l = 0;
      if (l)
        c[i][t++] = c[i][j];
    }
  for (i = 1; i < 82; ++i)
    if (a[i])
    {
      b[i][1] = a[i];
      b[i][2] = a[i];
      e[i] = 1;
    }
    else
    {
      b[i][1] = 1;
      b[i][2] = 9;
      e[i] = 0;
    }
// this is the actual beginning of the program. It is effectively 81 nested "for" loops.
  p = 1;
  a[1] = b[1][1];
//  label1:
  while(1)
  {
  ++v;
  l = 1;
  i = 0;
  while (++i < 21 and l)
    if(a[p] == a[c[p][i]])
      l = 0;
  if(l)
  {
    if (++p == 82)
    {
// prints the a array which holds the solution
      for (i = 1; i < 10; ++i)
      {
        for (j = 1; j < 10; ++j)
          cout << a[i * 9 + j - 9] << " ";
        cout << "\n";
      }
      cout << v << "\n";
//end of print
      return (0);
    }
    a[p] = b[p][1];
  }
  else
    while (1)
      if (++a[p] <= b[p][2])
        break;
      else
      {
        if (e[p])
          a[p] = b[p][1];
        else
          a[p] = 0;
        --p;
      }
//  goto label1;
  }
}
