#include <stdio.h>

int main(void)
{
  int x = 0;
	for(int a = 0; a < 9; ++a)
	  for(int b = a + 1; b < 9; ++b)
      {
      	++x;
        printf("  { %d, %d", a, b);
        for(int e = 0; e < 9; ++e)
          if(e != a && e != b)
            printf(", %d", e);
        printf("} // %d\n", x);
      }
	for(int a = 0; a < 9; ++a)
	  for(int b = a + 1; b < 9; ++b)
	    for(int c = b + 1; c < 9; ++c)
        {
          ++x;
          printf("  { %d, %d, %d", a, b, c);
          for(int e = 0; e < 9; ++e)
            if(e != a && e != b && e != c)
              printf(", %d", e);
          printf("}, // %d\n", x);
        }
	for(int a = 0; a < 9; ++a)
	  for(int b = a + 1; b < 9; ++b)
	    for(int c = b + 1; c < 9; ++c)
	      for(int d = c + 1; d < 9; ++d)
          {
            ++x;
            printf("  { %d, %d, %d, %d", a, b, c, d);
            for(int e = 0; e < 9; ++e)
              if(e != a && e != b && e != c && e != d)
                printf(", %d", e);
            printf("}, // %d\n", x);
          }
}
