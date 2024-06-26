#include <stdio.h>
#include <time.h>

char q,                      // Number of empty Cell positions
     r[81],                  // Used for sorting and eliminating empty Cell positions
     s[81];                  // Sudoku empty 81 Cell positions Board wise

static int bd[512] = {       // Boolean to Digit
          0,        1,        2,       21,        3,       31,       32,      321,
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

static char w[81][20] = {    // Affected empty 20 Cell positions
  { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,27,36,45,54,63,72},
  { 0, 2, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,28,37,46,55,64,73},
  { 0, 1, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,29,38,47,56,65,74},
  { 0, 1, 2, 4, 5, 6, 7, 8,12,13,14,21,22,23,30,39,48,57,66,75},
  { 0, 1, 2, 3, 5, 6, 7, 8,12,13,14,21,22,23,31,40,49,58,67,76},
  { 0, 1, 2, 3, 4, 6, 7, 8,12,13,14,21,22,23,32,41,50,59,68,77},
  { 0, 1, 2, 3, 4, 5, 7, 8,15,16,17,24,25,26,33,42,51,60,69,78},
  { 0, 1, 2, 3, 4, 5, 6, 8,15,16,17,24,25,26,34,43,52,61,70,79},
  { 0, 1, 2, 3, 4, 5, 6, 7,15,16,17,24,25,26,35,44,53,62,71,80},
  { 0, 1, 2,10,11,12,13,14,15,16,17,18,19,20,27,36,45,54,63,72},
  { 0, 1, 2, 9,11,12,13,14,15,16,17,18,19,20,28,37,46,55,64,73},
  { 0, 1, 2, 9,10,12,13,14,15,16,17,18,19,20,29,38,47,56,65,74},
  { 3, 4, 5, 9,10,11,13,14,15,16,17,21,22,23,30,39,48,57,66,75},
  { 3, 4, 5, 9,10,11,12,14,15,16,17,21,22,23,31,40,49,58,67,76},
  { 3, 4, 5, 9,10,11,12,13,15,16,17,21,22,23,32,41,50,59,68,77},
  { 6, 7, 8, 9,10,11,12,13,14,16,17,24,25,26,33,42,51,60,69,78},
  { 6, 7, 8, 9,10,11,12,13,14,15,17,24,25,26,34,43,52,61,70,79},
  { 6, 7, 8, 9,10,11,12,13,14,15,16,24,25,26,35,44,53,62,71,80},
  { 0, 1, 2, 9,10,11,19,20,21,22,23,24,25,26,27,36,45,54,63,72},
  { 0, 1, 2, 9,10,11,18,20,21,22,23,24,25,26,28,37,46,55,64,73},
  { 0, 1, 2, 9,10,11,18,19,21,22,23,24,25,26,29,38,47,56,65,74},
  { 3, 4, 5,12,13,14,18,19,20,22,23,24,25,26,30,39,48,57,66,75},
  { 3, 4, 5,12,13,14,18,19,20,21,23,24,25,26,31,40,49,58,67,76},
  { 3, 4, 5,12,13,14,18,19,20,21,22,24,25,26,32,41,50,59,68,77},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,25,26,33,42,51,60,69,78},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,24,26,34,43,52,61,70,79},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,24,25,35,44,53,62,71,80},
  { 0, 9,18,28,29,30,31,32,33,34,35,36,37,38,45,46,47,54,63,72},
  { 1,10,19,27,29,30,31,32,33,34,35,36,37,38,45,46,47,55,64,73},
  { 2,11,20,27,28,30,31,32,33,34,35,36,37,38,45,46,47,56,65,74},
  { 3,12,21,27,28,29,31,32,33,34,35,39,40,41,48,49,50,57,66,75},
  { 4,13,22,27,28,29,30,32,33,34,35,39,40,41,48,49,50,58,67,76},
  { 5,14,23,27,28,29,30,31,33,34,35,39,40,41,48,49,50,59,68,77},
  { 6,15,24,27,28,29,30,31,32,34,35,42,43,44,51,52,53,60,69,78},
  { 7,16,25,27,28,29,30,31,32,33,35,42,43,44,51,52,53,61,70,79},
  { 8,17,26,27,28,29,30,31,32,33,34,42,43,44,51,52,53,62,71,80},
  { 0, 9,18,27,28,29,37,38,39,40,41,42,43,44,45,46,47,54,63,72},
  { 1,10,19,27,28,29,36,38,39,40,41,42,43,44,45,46,47,55,64,73},
  { 2,11,20,27,28,29,36,37,39,40,41,42,43,44,45,46,47,56,65,74},
  { 3,12,21,30,31,32,36,37,38,40,41,42,43,44,48,49,50,57,66,75},
  { 4,13,22,30,31,32,36,37,38,39,41,42,43,44,48,49,50,58,67,76},
  { 5,14,23,30,31,32,36,37,38,39,40,42,43,44,48,49,50,59,68,77},
  { 6,15,24,33,34,35,36,37,38,39,40,41,43,44,51,52,53,60,69,78},
  { 7,16,25,33,34,35,36,37,38,39,40,41,42,44,51,52,53,61,70,79},
  { 8,17,26,33,34,35,36,37,38,39,40,41,42,43,51,52,53,62,71,80},
  { 0, 9,18,27,28,29,36,37,38,46,47,48,49,50,51,52,53,54,63,72},
  { 1,10,19,27,28,29,36,37,38,45,47,48,49,50,51,52,53,55,64,73},
  { 2,11,20,27,28,29,36,37,38,45,46,48,49,50,51,52,53,56,65,74},
  { 3,12,21,30,31,32,39,40,41,45,46,47,49,50,51,52,53,57,66,75},
  { 4,13,22,30,31,32,39,40,41,45,46,47,48,50,51,52,53,58,67,76},
  { 5,14,23,30,31,32,39,40,41,45,46,47,48,49,51,52,53,59,68,77},
  { 6,15,24,33,34,35,42,43,44,45,46,47,48,49,50,52,53,60,69,78},
  { 7,16,25,33,34,35,42,43,44,45,46,47,48,49,50,51,53,61,70,79},
  { 8,17,26,33,34,35,42,43,44,45,46,47,48,49,50,51,52,62,71,80},
  { 0, 9,18,27,36,45,55,56,57,58,59,60,61,62,63,64,65,72,73,74},
  { 1,10,19,28,37,46,54,56,57,58,59,60,61,62,63,64,65,72,73,74},
  { 2,11,20,29,38,47,54,55,57,58,59,60,61,62,63,64,65,72,73,74},
  { 3,12,21,30,39,48,54,55,56,58,59,60,61,62,66,67,68,75,76,77},
  { 4,13,22,31,40,49,54,55,56,57,59,60,61,62,66,67,68,75,76,77},
  { 5,14,23,32,41,50,54,55,56,57,58,60,61,62,66,67,68,75,76,77},
  { 6,15,24,33,42,51,54,55,56,57,58,59,61,62,69,70,71,78,79,80},
  { 7,16,25,34,43,52,54,55,56,57,58,59,60,62,69,70,71,78,79,80},
  { 8,17,26,35,44,53,54,55,56,57,58,59,60,61,69,70,71,78,79,80},
  { 0, 9,18,27,36,45,54,55,56,64,65,66,67,68,69,70,71,72,73,74},
  { 1,10,19,28,37,46,54,55,56,63,65,66,67,68,69,70,71,72,73,74},
  { 2,11,20,29,38,47,54,55,56,63,64,66,67,68,69,70,71,72,73,74},
  { 3,12,21,30,39,48,57,58,59,63,64,65,67,68,69,70,71,75,76,77},
  { 4,13,22,31,40,49,57,58,59,63,64,65,66,68,69,70,71,75,76,77},
  { 5,14,23,32,41,50,57,58,59,63,64,65,66,67,69,70,71,75,76,77},
  { 6,15,24,33,42,51,60,61,62,63,64,65,66,67,68,70,71,78,79,80},
  { 7,16,25,34,43,52,60,61,62,63,64,65,66,67,68,69,71,78,79,80},
  { 8,17,26,35,44,53,60,61,62,63,64,65,66,67,68,69,70,78,79,80},
  { 0, 9,18,27,36,45,54,55,56,63,64,65,73,74,75,76,77,78,79,80},
  { 1,10,19,28,37,46,54,55,56,63,64,65,72,74,75,76,77,78,79,80},
  { 2,11,20,29,38,47,54,55,56,63,64,65,72,73,75,76,77,78,79,80},
  { 3,12,21,30,39,48,57,58,59,66,67,68,72,73,74,76,77,78,79,80},
  { 4,13,22,31,40,49,57,58,59,66,67,68,72,73,74,75,77,78,79,80},
  { 5,14,23,32,41,50,57,58,59,66,67,68,72,73,74,75,76,78,79,80},
  { 6,15,24,33,42,51,60,61,62,69,70,71,72,73,74,75,76,77,79,80},
  { 7,16,25,34,43,52,60,61,62,69,70,71,72,73,74,75,76,77,78,80},
  { 8,17,26,35,44,53,60,61,62,69,70,71,72,73,74,75,76,77,78,79}},
            l[27][9] = {     // Empty 9 Cell positions for Hidden Singles Row, Column and Box wise
  { 0, 1, 2, 3, 4, 5, 6, 7, 8}, { 9,10,11,12,13,14,15,16,17},
  {18,19,20,21,22,23,24,25,26}, {27,28,29,30,31,32,33,34,35},
  {36,37,38,39,40,41,42,43,44}, {45,46,47,48,49,50,51,52,53},
  {54,55,56,57,58,59,60,61,62}, {63,64,65,66,67,68,69,70,71},
  {72,73,74,75,76,77,78,79,80}, { 0, 9,18,27,36,45,54,63,72},
  { 1,10,19,28,37,46,55,64,73}, { 2,11,20,29,38,47,56,65,74},
  { 3,12,21,30,39,48,57,66,75}, { 4,13,22,31,40,49,58,67,76},
  { 5,14,23,32,41,50,59,68,77}, { 6,15,24,33,42,51,60,69,78},
  { 7,16,25,34,43,52,61,70,79}, { 8,17,26,35,44,53,62,71,80},
  { 0, 1, 2, 9,10,11,18,19,20}, { 3, 4, 5,12,13,14,21,22,23},
  { 6, 7, 8,15,16,17,24,25,26}, {27,28,29,36,37,38,45,46,47},
  {30,31,32,39,40,41,48,49,50}, {33,34,35,42,43,44,51,52,53},
  {54,55,56,63,64,65,72,73,74}, {57,58,59,66,67,68,75,76,77},
  {60,61,62,69,70,71,78,79,80}},
            j[54][15] = {    // Empty 3 Cell positions either Box-Row or Box-Column wise and affected empty 6+6 Cell positions for Intersection Removal
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20},
  { 3, 4, 5, 0, 1, 2, 6, 7, 8,12,13,14,21,22,23},
  { 6, 7, 8, 0, 1, 2, 3, 4, 5,15,16,17,24,25,26},
  { 9,10,11,12,13,14,15,16,17, 0, 1, 2,18,19,20},
  {12,13,14, 9,10,11,15,16,17, 3, 4, 5,21,22,23},
  {15,16,17, 9,10,11,12,13,14, 6, 7, 8,24,25,26},
  {18,19,20,21,22,23,24,25,26, 0, 1, 2, 9,10,11},
  {21,22,23,18,19,20,24,25,26, 3, 4, 5,12,13,14},
  {24,25,26,18,19,20,21,22,23, 6, 7, 8,15,16,17},
  {27,28,29,30,31,32,33,34,35,36,37,38,45,46,47},
  {30,31,32,27,28,29,33,34,35,39,40,41,48,49,50},
  {33,34,35,27,28,29,30,31,32,42,43,44,51,52,53},
  {36,37,38,39,40,41,42,43,44,27,28,29,45,46,47},
  {39,40,41,36,37,38,42,43,44,30,31,32,48,49,50},
  {42,43,44,36,37,38,39,40,41,33,34,35,51,52,53},
  {45,46,47,48,49,50,51,52,53,27,28,29,36,37,38},
  {48,49,50,45,46,47,51,52,53,30,31,32,39,40,41},
  {51,52,53,45,46,47,48,49,50,33,34,35,42,43,44},
  {54,55,56,57,58,59,60,61,62,63,64,65,72,73,74},
  {57,58,59,54,55,56,60,61,62,66,67,68,75,76,77},
  {60,61,62,54,55,56,57,58,59,69,70,71,78,79,80},
  {63,64,65,66,67,68,69,70,71,54,55,56,72,73,74},
  {66,67,68,63,64,65,69,70,71,57,58,59,75,76,77},
  {69,70,71,63,64,65,66,67,68,60,61,62,78,79,80},
  {72,73,74,75,76,77,78,79,80,54,55,56,63,64,65},
  {75,76,77,72,73,74,78,79,80,57,58,59,66,67,68},
  {78,79,80,72,73,74,75,76,77,60,61,62,69,70,71},
  { 0, 9,18,27,36,45,54,63,72, 1, 2,10,11,19,20},
  {27,36,45, 0, 9,18,54,63,72,28,29,37,38,46,47},
  {54,63,72, 0, 9,18,27,36,45,55,56,64,65,73,74},
  { 1,10,19,28,37,46,55,64,73, 0, 2, 9,11,18,20},
  {28,37,46, 1,10,19,55,64,73,27,29,36,38,45,47},
  {55,64,73, 1,10,19,28,37,46,54,56,63,65,72,74},
  { 2,11,20,29,38,47,56,65,74, 0, 1, 9,10,18,19},
  {29,38,47, 2,11,20,56,65,74,27,28,36,37,45,46},
  {56,65,74, 2,11,20,29,38,47,54,55,63,64,72,73},
  { 3,12,21,30,39,48,57,66,75, 4, 5,13,14,22,23},
  {30,39,48, 3,12,21,57,66,75,31,32,40,41,49,50},
  {57,66,75, 3,12,21,30,39,48,58,59,67,68,76,77},
  { 4,13,22,31,40,49,58,67,76, 3, 5,12,14,21,23},
  {31,40,49, 4,13,22,58,67,76,30,32,39,41,48,50},
  {58,67,76, 4,13,22,31,40,49,57,59,66,68,75,77},
  { 5,14,23,32,41,50,59,68,77, 3, 4,12,13,21,22},
  {32,41,50, 5,14,23,59,68,77,30,31,39,40,48,49},
  {59,68,77, 5,14,23,32,41,50,57,58,66,67,75,76},
  { 6,15,24,33,42,51,60,69,78, 7, 8,16,17,25,26},
  {33,42,51, 6,15,24,60,69,78,34,35,43,44,52,53},
  {60,69,78, 6,15,24,33,42,51,61,62,70,71,79,80},
  { 7,16,25,34,43,52,61,70,79, 6, 8,15,17,24,26},
  {34,43,52, 7,16,25,61,70,79,33,35,42,44,51,53},
  {61,70,79, 7,16,25,34,43,52,60,62,69,71,78,80},
  { 8,17,26,35,44,53,62,71,80, 6, 7,15,16,24,25},
  {35,44,53, 8,17,26,62,71,80,33,34,42,43,51,52},
  {62,71,80, 8,17,26,35,44,53,60,61,69,70,78,79}};

long n,                      // Number of guesses Board wise
     h;                      // Number of Hidden Singles Board wise

bool g[81][9];               // Candidates empty Cell position wise

long b2d(char p)             // Convert Candidates from boolean to digits
{
  int z = (g[p][0] ? 1 : 0) + (g[p][1] ? 2 : 0) + (g[p][2] ? 4 : 0)
        + (g[p][3] ? 8 : 0) + (g[p][4] ? 16 : 0) + (g[p][5] ? 32 : 0)
        + (g[p][6] ? 64 : 0) + (g[p][7] ? 128 : 0) + (g[p][8] ? 256 : 0);

  return bd[z];
}

bool solve(char p)
{
  char a,
       x = p,
       y;

  long z = 987654322;

  for(a = p; a < q; a++)
    if(z > b2d(r[a]))        // Check least Candidates empty Cell position wise
    {
      x = a;
      z = b2d(r[x]);
      if (z < 10)            // Found Naked Single Candidate
        goto NHSCF;
    }
    for(char zr, rr, b = 0; b < 27; b++)
    {
      for(a = 0; a < 9; a++)
      {
        for(rr = 0, y = 0; y < 9; y++)
          if(g[l[b][y]][a])  // Check Hidden Candidate
          {
            rr++;
            zr = y;
          }
        if(rr == 1)          // Found Hidden Single Candidate
        {
          for(h++, z = a + 1, a = l[b][zr], x = p; x < q; x++)
            if(r[x] == a)
              goto NHSCF;
        }
      }
    }
    for(char b = 0; b < 54; b++)
      for(a = 0; a < 9; a++)
        if(g[j[b][0]][a] || g[j[b][1]][a] || g[j[b][2]][a])
        {                    // Check Candidate within empty 3 Cell positions either Box-Row or Box-Column wise
          if(g[j[b][3]][a] || g[j[b][4]][a] || g[j[b][5]][a] || g[j[b][6]][a] || g[j[b][7]][a] || g[j[b][8]][a])
          {                  // Check Candidate within affected empty 6 Cell positions either Row or Column wise
            if(!(g[j[b][9]][a] || g[j[b][10]][a] || g[j[b][11]][a] || g[j[b][12]][a] || g[j[b][13]][a] || g[j[b][14]][a]))
            {                // Check Candidate within affected empty 6 Cells positions Box wise
              bool k[6] = { 0, 0, 0, 0, 0, 0};

              for(y = 3; y < 9; y++)
                if(g[j[b][y]][a])
                {            // Backup Candidate within affected empty 6 Cell positions either Row or Column wise
                  k[y - 3] = 1;
                  g[j[b][y]][a] = 0;
                }
              if(solve(p))
                return 1;
              for(y = 3; y < 9; y++)
                if(k[y - 3]) // Restore Candidate within affected empty 6 Cell positions either Row or Column wise
                  g[j[b][y]][a] = 1;
            }
          }
          else if(g[j[b][9]][a] || g[j[b][10]][a] || g[j[b][11]][a] || g[j[b][12]][a] || g[j[b][13]][a] || g[j[b][14]][a])
          {                  // Check Candidate within affected empty 6 Cell positions Box wise
            bool k[6] = { 0, 0, 0, 0, 0, 0};

            for(y = 9; y < 15; y++)
              if(g[j[b][y]][a])
              {              // Backup Candidate within affected empty 6 Cell positions Box wise
                k[y - 9] = 1;
                g[j[b][y]][a] = 0;
              }
            if(solve(p))
              return 1;
            for(y = 9; y < 15; y++)
              if(k[y - 9])   // Restore Candidate within affected empty 6 Cell positions Box wise
                g[j[b][y]][a] = 1;
          }
        }
NHSCF:
  if(x > p)                  // Check define Cell position for sorting and eliminating
  {
    a = r[p];
    r[p] = r[x];
    r[x] = a;
  }
  for(; (s[r[p]] = z % 10) > 0; n++, z /= 10)
  {
    bool k[29] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for(a = 0; a < 9; a++)   // Backup define Cell position Candidates wise
      if(g[r[p]][a])
      {
        k[a + 20] = 1;
        g[r[p]][a] = 0;
      }
    for(a = 0; a < 20; a++)  // Backup Candidate within affected empty 20 Cell positions wise
      if(g[w[r[p]][a]][s[r[p]] - 1])
      {
        if(b2d(w[r[p]][a]) < 10)
          break;
        k[a] = 1;
        g[w[r[p]][a]][s[r[p]] - 1] = 0;
      }
    if(a > 19 && (p + 1 >= q || solve(p + 1)))
      return 1;              // If either all empty Cell positions defined or recursive solve for next empty Cell position
    for(a = 0; a < 9; a++)   // Restore Candidates define Cell position wise
      if(k[a + 20])
        g[r[p]][a] = 1;
    for(a = 0; a < 20; a++)  // Restore Candidate within affected empty 20 Cell positions wise
      if(k[a])
        g[w[r[p]][a]][s[r[p]] - 1] = 1;
  }
  return 0;
}

bool check(char p, char q)
{
  for(char a = 0; a < 20; a++)
    if(s[w[p][a]] == q)      // Check duplicate Candidate within affected empty 20 Cell positions wise
      return 0;
  return 1;
}

bool invalid(void)
{
  for(char p = 0; p < 81; p++)
    if(!s[p])                // Found empty Cell position
    {
      for(char a = 0; a < 9; a++)
        g[p][a] = check(p, a + 1);
                             // Check and assign Candidate for empty Cell positions
      if(!b2d(p))            // Check no Candidate assigned
        return 1;
      r[q++] = p;            // Assign empty Cell position for sorting and eliminating
    }
    else if(!check(p, s[p])) // Check all pre define Cell positions constraint
      return 1;
  return 0;
}

int main(void)
{
  char a = 0,
       m;

  float c,
        d = 0,
        e = 0,
        f = 0;

  long i = 0,
       t = 0,
       u = 0,
       v = 0;

  FILE *o = fopen("sudoku.txt", "r");

  if(o == NULL)
    printf("Unable to open sudoku.txt file for read !!");
  else do
    if((m = fgetc(o)) != 10 && m != EOF && a < 81)
      s[a++] = m >= '1' && m <= '9' ? m - '0' : 0;
                             // Assign digit to pre define Cell position
    else if(m == 10 || m == EOF)
      {
        while(a < 81)
          s[a++] = 0;        // Clear remaining empty Cell positions
        n = h = q = 0;
        c = clock();
        if(invalid())
        {
          c = (clock() - c) / CLOCKS_PER_SEC * 1000;
          d += c;
          printf("%ld) Error: Invalid Sudoku! # I%ld", ++t, ++i);
        }
        else if(solve(0))
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
        printf(" # N%ld # H%ld # %f\n", n, h, c);
        a = 0;
      }
  while(m != EOF);
  printf("=======================================\n");
  printf("Total Sudoku puzzle read   : %ld\n", t);
  printf("Total time for all puzzles : %f\n", d + e + f);
  printf("Average time per puzzle    : %f\n", t ? (d + e + f) / t : 0);
  printf("Number of invalid puzzles  : %ld\n", i);
  printf("Time for invalid puzzles   : %f\n", d);
  printf("Average time per invalid   : %f\n", i ? d / i : 0);
  printf("Number of solved puzzles   : %ld\n", v);
  printf("Time for solved puzzles    : %f\n", e);
  printf("Average time per solved    : %f\n", v ? e / v : 0);
  printf("Number of unsolved puzzle  : %ld\n", u);
  printf("Time for unsolved puzzles  : %f\n", f);
  printf("Average time per unsolved  : %f\n", u ? f / u : 0);
  if(fclose(o) == EOF)
    printf("Unable to close sudoku.txt file !!");
}
