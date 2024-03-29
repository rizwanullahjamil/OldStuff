/**********************************­ **************************
*** Dipl.- Inf. Detlef Hafer
*** D-91058 Erlangen (Germany)
**********************************­ ***************************/

#include <stdafx.h>
#include <stdlib.h>

#define BACK -1
#define FORWARD 1
#define NSIZE 9

int F[NSIZE][NSIZE];

#if _DEBUG
static int M[NSIZE][NSIZE] = { {1, 6, 4, 0, 0, 0, 0, 0, 2},
                        {2, 0, 0, 4, 0, 3, 9, 1, 0},
                        {0, 0, 5, 0, 8, 0, 4, 0, 7},
                        {0, 9, 0, 0, 0, 6, 5, 0, 0},
                        {5, 0, 0, 1, 0, 2, 0, 0, 8},
                        {0, 0, 8, 9, 0, 0, 0, 3, 0},
                        {8, 0, 9, 0, 4, 0, 2, 0, 0},
                        {0, 7, 3, 5, 0, 9, 0, 0, 1},
                        {4, 0, 0, 0, 0, 0, 6, 7, 9} };

#endif

static BOOL findnumber (int z, int s)
{
    int u = (z/3)*3;
    int v = (s/3)*3;
    int m = F[z][s];
    F[z][s] = 0;

    for (int t = m+1; t <= NSIZE; t++)
    {
        for (int i = 0; i < NSIZE; i++)
        {
            div_t q = div(i,3);
            if ((F[i][s] == t) || (F[z][i] == t) || F[u+q.quot][v+q.rem] == t)
                break;
        }
        if (i == NSIZE )
        {
            F[z][s] = t;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL soduko (void)
{
    BOOL bFix[NSIZE][NSIZE];
    int r = FORWARD;

#if _DEBUG
    memcpy(F, M, sizeof(M));
#endif

    for (int i = 0; i < NSIZE; i++)
        for (int j = 0; j < NSIZE; j++)
            bFix[i][j] = (F[i][j] != 0);

    i = 0;
    while ( (i >= 0) && (i < NSIZE*NSIZE) )
    {
        div_t q = div(i,NSIZE);
        int z = q.quot;
        int s = q.rem;

        if ( ! bFix[z][s])
        {
            if ( ! findnumber(z,s))
                r = BACK;
            else
                r = FORWARD;
        }
        i += r;
    }
    if ( i < 0)
    {
        // no solution
        return FALSE;
    }
    return TRUE;
}
