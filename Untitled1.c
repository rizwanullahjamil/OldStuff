#include <stdio.h>

static const int b[512] = {  // Bitwise to digit Cell values
          0,        1,        2,       12,        3,       13,       23,      123,
          4,       14,       24,      124,       34,      134,      234,     1234,
          5,       15,       25,      125,       35,      135,      235,     1235,
         45,      145,      245,     1245,      345,     1345,     2345,    12345,
          6,       16,       26,      126,       36,      136,      236,     1236,
         46,      146,      246,     1246,      346,     1346,     2346,    12346,
         56,      156,      256,     1256,      356,     1356,     2356,    12356,
        456,     1456,     2456,    12456,     3456,    13456,    23456,   123456,
          7,       17,       27,      127,       37,      137,      237,     1237,
         47,      147,      247,     1247,      347,     1347,     2347,    12347,
         57,      157,      257,     1257,      357,     1357,     2357,    12357,
        457,     1457,     2457,    12457,     3457,    13457,    23457,   123457,
         67,      167,      267,     1267,      367,     1367,     2367,    12367,
        467,     1467,     2467,    12467,     3467,    13467,    23467,   123467,
        567,     1567,     2567,    12567,     3567,    13567,    23567,   123567,
       4567,    14567,    24567,   124567,    34567,   134567,   234567,  1234567,
          8,       18,       28,      128,       38,      138,      238,     1238,
         48,      148,      248,     1248,      348,     1348,     2348,    12348,
         58,      158,      258,     1258,      358,     1358,     2358,    12358,
        458,     1458,     2458,    12458,     3458,    13458,    23458,   123458,
         68,      168,      268,     1268,      368,     1368,     2368,    12368,
        468,     1468,     2468,    12468,     3468,    13468,    23468,   123468,
        568,     1568,     2568,    12568,     3568,    13568,    23568,   123568,
       4568,    14568,    24568,   124568,    34568,   134568,   234568,  1234568,
         78,      178,      278,     1278,      378,     1378,     2378,    12378,
        478,     1478,     2478,    12478,     3478,    13478,    23478,   123478,
        578,     1578,     2578,    12578,     3578,    13578,    23578,   123578,
       4578,    14578,    24578,   124578,    34578,   134578,   234578,  1234578,
        678,     1678,     2678,    12678,     3678,    13678,    23678,   123678,
       4678,    14678,    24678,   124678,    34678,   134678,   234678,  1234678,
       5678,    15678,    25678,   125678,    35678,   135678,   235678,  1235678,
      45678,   145678,   245678,  1245678,   345678,  1345678,  2345678, 12345678,
          9,       19,       29,      129,       39,      139,      239,     1239,
         49,      149,      249,     1249,      349,     1349,     2349,    12349,
         59,      159,      259,     1259,      359,     1359,     2359,    12359,
        459,     1459,     2459,    12459,     3459,    13459,    23459,   123459,
         69,      169,      269,     1269,      369,     1369,     2369,    12369,
        469,     1469,     2469,    12469,     3469,    13469,    23469,   123469,
        569,     1569,     2569,    12569,     3569,    13569,    23569,   123569,
       4569,    14569,    24569,   124569,    34569,   134569,   234569,  1234569,
         79,      179,      279,     1279,      379,     1379,     2379,    12379,
        479,     1479,     2479,    12479,     3479,    13479,    23479,   123479,
        579,     1579,     2579,    12579,     3579,    13579,    23579,   123579,
       4579,    14579,    24579,   124579,    34579,   134579,   234579,  1234579,
        679,     1679,     2679,    12679,     3679,    13679,    23679,   123679,
       4679,    14679,    24679,   124679,    34679,   134679,   234679,  1234679,
       5679,    15679,    25679,   125679,    35679,   135679,   235679,  1235679,
      45679,   145679,   245679,  1245679,   345679,  1345679,  2345679, 12345679,
         89,      189,      289,     1289,      389,     1389,     2389,    12389,
        489,     1489,     2489,    12489,     3489,    13489,    23489,   123489,
        589,     1589,     2589,    12589,     3589,    13589,    23589,   123589,
       4589,    14589,    24589,   124589,    34589,   134589,   234589,  1234589,
        689,     1689,     2689,    12689,     3689,    13689,    23689,   123689,
       4689,    14689,    24689,   124689,    34689,   134689,   234689,  1234689,
       5689,    15689,    25689,   125689,    35689,   135689,   235689,  1235689,
      45689,   145689,   245689,  1245689,   345689,  1345689,  2345689, 12345689,
        789,     1789,     2789,    12789,     3789,    13789,    23789,   123789,
       4789,    14789,    24789,   124789,    34789,   134789,   234789,  1234789,
       5789,    15789,    25789,   125789,    35789,   135789,   235789,  1235789,
      45789,   145789,   245789,  1245789,   345789,  1345789,  2345789, 12345789,
       6789,    16789,    26789,   126789,    36789,   136789,   236789,  1236789,
      46789,   146789,   246789,  1246789,   346789,  1346789,  2346789, 12346789,
      56789,   156789,   256789,  1256789,   356789,  1356789,  2356789, 12356789,
     456789,  1456789,  2456789, 12456789,  3456789, 13456789, 23456789,123456789};
  char s[20];

char *S (int a)
{
  sprintf (s, "r%dc%d", b[(a >> 18) & 511], b[a & 511]);
  return s;
}

int main (void)
{
  int x = 66775433, y = 456935;

  printf ("%s %s", S (x), S (y));
  printf (" %s\n", S (y));
  return 0;
}
