            #define TIMING 1 

/* 
----------------------------------------------------------------------- 

* Yet Another Sudoku Grids Counter [Ed Russell 2005/11/05-v2] 
* -------------------------------- 
* 
* The method here is basically that described by "dukuso" in 
* 
* http://www.sudoku.com/forums/viewtopic.php?t=44&start=267 
* 
* using the equivalence class calculation of "kjellfp" in 
* 
* http://www.sudoku.com/forums/viewtopic.php?t=44&start=377 
* 
* plus a load of my own table lookups to shortcut some of the 
* repetitive search loops. 
* 
* Timing on a 1.4GHz Athlon: 0.046 + 0.124 = 0.170 seconds 
* 
----------------------------------------------------------------------- 

*/ 

#include <stdio.h> 
#include <stdlib.h> 
#include <mem.h> 

#if TIMING 
#include <time.h> 
#endif 

static int colBits[84]; 
static int cblookup[449]; 
static int colcompat[84][20][2]; 
static int boxcompat[280][56]; 
static int cmatrix[280][280]; 
static int nsoln[44]; 
static int nclass[44]; 
static int canon[44]; 
static int ijk[280][3]; 
static int ijklookup[1024]; 

#define CBCRUNCH(B) (B) 
#define CBLOOKUP(B) cblookup[CBCRUNCH(B)] 

#define ORDER3(A,B,C) \ 
if (A<B) { \ 
if (B<C) { int t=A; A=C; C=t; } \ 
else if (A<C) { int t=A; A=B; B=C; C=t; } \ 
else { int t=A; A=B; B=t; } \ 
} else { \ 
if (B>C) { /* do nothing */ } \ 
else if (A>C) { int t=B; B=C; C=t; } \ 
else { int t=A; A=C; C=B; B=t; } \ 
} 

#define IJKCRUNCH(I,J,K) ( ( ((I)<<14)+((J)<<5)+(K) ) & 0x3ff ) 
#define IJKLOOKUP(I,J,K) ijklookup[IJKCRUNCH(I,J,K)] 

#if 0 
/* The following alternative for IJKLOOKUP, removing the 
* need to call ORDER3(), is actually slower on my machine 
*/ 
#define IJKLOOKUP(I,J,K) \ 
ijklookup[ \ 
(I)<(J) ? (J)<(K) ? IJKCRUNCH(K,J,I) \ 
: (I)<(K) ? IJKCRUNCH(J,K,I) \ 
: IJKCRUNCH(J,I,K) \ 
: (J)>(K) ? IJKCRUNCH(I,J,K) \ 
: (I)>(K) ? IJKCRUNCH(I,K,J) \ 
: IJKCRUNCH(K,I,J) ] 
#endif 

/* 
----------------------------------------------------------------------- 

* Set up various useful lookup tables: 
* 
* colBits[0..83] = bitwise representation of each of the 84 
* possible sets of minicolumn contents 
* 
* colcompat[C][0..19][0..1] : 84 lists, one per minicolumn C. 
* Each list is 20 pairs of numbers; each pair 
* indexes the two minicolumns compatible (sharing 
* no values) with the first minicolumn 
* 
* ijk[0..279][0..2] = descriptions of the 280 miniboxes with 
* ordered columns; values are numbers 0..83, 
* ordered s.t. ijk[B][0] > ijk[B][1] > ijk[B][2] 
* 
* boxcompat[B][0..55] = 280 lists, one per minibox B. Each list 
* is a number 0..279 of a box that can go below 
* this one. The lists are ordered so that 
* boxcompat[B][i] and boxcompat[B][i^1] are mutually 
* compatible with box B (you can stack all three, 
* with suitable minicol ordering, on top of each 
* other), with even numbered indices always being 
* the lesser index of the pair 
* 
* We also set up a couple of reverse maps that let us go from one 
* representation of a column or box to another: 
* 
* cblookup[0..1023] : maps a 9-bit "colBits" value back to an 
* index 0..83 
* 
* ijklookup[0..2583] : maps an ordered triple i>j>k of column 
* indices 0..83 to a box index 0..279; but first 
* you have to use IJKCRUNCH() to reduce i,j,k 
* 
----------------------------------------------------------------------- 

*/ 

static void pretabulate(void) { 
int i, j, k; 
int n = 0; 

/* colBits[] 
*/ 
for (i=0; i<7; i++) 
for (j=i+1; j<8; j++) 
for (k=j+1; k<9; k++) 
colBits[n++] = (1<<i) + (1<<j) + (1<<k); 

/* colcompat[][][] 
*/ 
for (i=0; i<84; i++) { 
cblookup[CBCRUNCH(colBits[i])] = i; 
for (j=n=0; j<84; j++) { 
if (!(colBits[i]&colBits[j])) { 
for (k=0; k<84; k++) 
if (!((colBits[i]|colBits[j])&colBits[k])) 
break; 
colcompat[i][n][0] = j; 
colcompat[i][n++][1] = k; 
} 
} 
} 

/* ijk[][] 
*/ 
for (i=2,k=0; i<84; i++) { 
for (j=0; j<20; j++) { 
if (i>colcompat[i][j][0] && colcompat[i][j][0]>colcompat[i][j][1]) { 

ijk[k][0] = i; 
ijk[k][1] = colcompat[i][j][0]; 
ijk[k][2] = colcompat[i][j][1]; 
ijklookup[IJKCRUNCH(i,ijk[k][1],ijk[k][2])] = k; 
k++; 
} 
} 
} 

/* boxcompat[][] 
*/ 
for (k=0; k<280; k++) { 
int k0 = ijk[k][0]; 
int bits1 = colBits[ijk[k][1]]; 
int bits2 = colBits[ijk[k][2]]; 
int n; 
for (i=n=0; i<20; i++) { 
int colsA0, colsA[3]; 
int colsB0, colsB[3]; 
if ( (colsA0=colcompat[k0][i][0]) < (colsB0=colcompat[k0][i][1]) ) { 

for (j=0; j<20; j++) { 
if ( !(bits1&colBits[colsA[1]=colcompat[colsA0][j][0]]) \ 
&& !(bits2&colBits[colsA[2]=colcompat[colsA0][j][1]]) ) { 
colsA[0] = colsA0; 
colsB[0] = colsB0; 
colsB[1] = CBLOOKUP(0x1ff^bits1^colBits[colsA[1]]); 
colsB[2] = CBLOOKUP(0x1ff^bits2^colBits[colsA[2]]); 
ORDER3(colsA[0],colsA[1],colsA[2]); 
boxcompat[k][n++] = IJKLOOKUP(colsA[0],colsA[1],colsA[2]); 
ORDER3(colsB[0],colsB[1],colsB[2]); 
boxcompat[k][n++] = IJKLOOKUP(colsB[0],colsB[1],colsB[2]); 
} 
} 
} 
} 
} 

return; 
} 

/* 
----------------------------------------------------------------------- 

* Catalogue the 44 equivalence classes into the following structs: 
* 
* cmatrix[0..279][0..279] : cmatrix[i][j] is the gangster number, 
* 1 to 44, of the column-reduced band containing boxes 
* (X,i,j) where X has cols { {1,2,3}, {4,5,6}, {7,8,9} } 
* 
* canon[i] = 280*i + j where (X,i,j) is some column-reduced band in 
* class i-1: canon[i] is a "canonical" element 
* 
* nclass[i] = number of (X,i,j) col-reduced bands in class i-1 
* 
* nsoln[i] = number of ways of solving (setting the order of cells 
* within each minicolumn to make a valid band) the 
* canonical element of class i-1 
* 
----------------------------------------------------------------------- 

*/ 

static void get44(void) { 
int perm[18] = { 0,1,2, 0,2,1, 1,0,2, 1,2,0, 2,0,1, 2,1,0 }; 
int i, j, k; 
int classnum = 0; 

memset(cmatrix,0,280*280*sizeof(int)); 
memset(nclass,0,44*sizeof(int)); 

/* loop over pairs of 3x3 box reps 
*/ 
for (i=0; i<280; i++) { 
for (j=i; j<280; j++) { 
if (!cmatrix[i][j]) { 

/* not a rep we've seen before: so construct it 
*/ 
int tband[9][3]; 
for (k=0; k<9; k++) tband[k/3][k%3] = k; 
for (k=3; k<9; k++) { 
int b = colBits[ijk[k<6?i:j][k%3]]; 
int c, d; 
for (c=d=0; d<3; c++) if (b&(1<<c)) tband[k][d++] = c; 
} 

/* store the (i,j) index of this canonical element 
*/ 
canon[classnum] = 280*i + j; 

/* count the number of solutions from permuting within minicols 
*/ 
{ 
int pcol[9][6]; /* bitmasks of permuted minicols */ 
int a, b, c, d, e, f, g, m; 
for (k=0; k<9; k++) 
for (a=0; a<6; a++) 
pcol[k][a] = (1<<( 0+tband[k][perm[3*a+0]])) \ 
+ (1<<( 9+tband[k][perm[3*a+1]])) \ 
+ (1<<(18+tband[k][perm[3*a+2]])) ; 
m = pcol[0][0]; 
for (a=k=0; a<6; a++) { 
if (pcol[3][a]&m) continue; else m^=pcol[3][a]; 
for (b=0; b<6; b++) { 
if (pcol[6][b]&m) continue; else m^=pcol[6][b]; 
for (c=0; c<6; c++) { 
if (pcol[1][c]&m) continue; else m^=pcol[1][c]; 
for (d=0; d<6; d++) { 
if (pcol[4][d]&m) continue; else m^=pcol[4][d]; 
for (e=0; e<6; e++) { 
if (pcol[7][e]&m) continue; else m^=pcol[7][e]; 
for (f=0; f<6; f++) { 
if (pcol[2][f]&m) continue; else m^=pcol[2][f]; 
for (g=0; g<6; g++) if (!(pcol[5][g]&m)) k+=6; 
m^=pcol[2][f]; } 
m^=pcol[7][e]; } 
m^=pcol[4][d]; } 
m^=pcol[1][c]; } 
m^=pcol[6][b]; } 
m^=pcol[3][a]; } 
} 
nsoln[classnum] = k; 

/* now loop over all 3x6^4 relabellings of this (slowww...) 
*/ 
{ 
int firstbox; 
int *colperm; 
int *cellperm[3]; 
int map[9]; 

#define SETMAP(C,X,Y,Z) { \ 
int *colptr = &tband[firstbox+colperm[C]][0]; \ 
map[ colptr[ cellperm[C][0] ] ] = 1<<(X); \ 
map[ colptr[ cellperm[C][1] ] ] = 1<<(Y); \ 
map[ colptr[ cellperm[C][2] ] ] = 1<<(Z); \ 
} 

for (firstbox=0; firstbox<9; firstbox+=3) { 
for (colperm=perm; colperm<&perm[18]; colperm+=3) { 
for (cellperm[0]=perm; cellperm[0]<&perm[18]; cellperm[0]+=3) { 
SETMAP(0,0,1,2) 
for (cellperm[1]=perm; cellperm[1]<&perm[18]; cellperm[1]+=3) { 
SETMAP(1,3,4,5) 
for (cellperm[2]=perm; cellperm[2]<&perm[18]; cellperm[2]+=3) { 
SETMAP(2,6,7,8) 
int otherbox, col0, col1, col2; 
int coord1, coord2; 

/* extract otherbox #1 
*/ 
otherbox = (firstbox+3) % 9; 
col0 = CBLOOKUP(map[tband[otherbox+0][0]] + 
map[tband[otherbox+0][1]] + map[tband[otherbox+0][2]]); 
col1 = CBLOOKUP(map[tband[otherbox+1][0]] + 
map[tband[otherbox+1][1]] + map[tband[otherbox+1][2]]); 
col2 = CBLOOKUP(map[tband[otherbox+2][0]] + 
map[tband[otherbox+2][1]] + map[tband[otherbox+2][2]]); 
ORDER3(col0,col1,col2); 
coord1 = IJKLOOKUP(col0,col1,col2); 

/* extract otherbox #2 
*/ 
otherbox = (firstbox+6) % 9; 
col0 = CBLOOKUP(map[tband[otherbox+0][0]] + 
map[tband[otherbox+0][1]] + map[tband[otherbox+0][2]]); 
col1 = CBLOOKUP(map[tband[otherbox+1][0]] + 
map[tband[otherbox+1][1]] + map[tband[otherbox+1][2]]); 
col2 = CBLOOKUP(map[tband[otherbox+2][0]] + 
map[tband[otherbox+2][1]] + map[tband[otherbox+2][2]]); 
ORDER3(col0,col1,col2); 
coord2 = IJKLOOKUP(col0,col1,col2); 

/* store the class number 
*/ 
cmatrix[coord1][coord2] = cmatrix[coord2][coord1] = 1+classnum; 

}}}}} 
} 
classnum++; 
} 
} 
} 

/* accumulate class membership counts 
*/ 
for (i=0; i<280; i++) 
for (j=0; j<280; j++) 
nclass[cmatrix[i][j]-1]++; 

#if !(TIMING) 
for (i=0; i<classnum; i++) 
printf("Class %2d : nsoln %4d, nclass 
%4d\n",i+1,nsoln[i],nclass[i]); 
#endif 

return; 
} 

/* 
----------------------------------------------------------------------- 

* count solutions, given some canonical top band and boxes B4,B7 
* 
----------------------------------------------------------------------- 

*/ 

static int count(int *B2compat, int *B3compat, int *B4colbits, int 
*B7colbits) { 
int dmap[9]; 
int cmap456[84], cmap789[84]; 
int B5[56], B6[56]; 
int B8[56], B9[56]; 
int i, j, k, n; 

/* column map for B4/5/6 
*/ 
for (j=k=0; j<3; k++) if (B4colbits[0] & (1<<k)) dmap[k] = 0x01 << 
j++; 
for (j=k=0; j<3; k++) if (B4colbits[1] & (1<<k)) dmap[k] = 0x08 << 
j++; 
for (j=k=0; j<3; k++) if (B4colbits[2] & (1<<k)) dmap[k] = 0x40 << 
j++; 
for (i=n=0; i<7; i++) 
for (j=i+1; j<8; j++) 
for (k=j+1; k<9; k++) 
cmap456[n++] = CBLOOKUP(dmap[i]+dmap[j]+dmap[k]); 

/* column map for B7/8/9 
*/ 
for (j=k=0; j<3; k++) if (B7colbits[0] & (1<<k)) dmap[k] = 0x01 << 
j++; 
for (j=k=0; j<3; k++) if (B7colbits[1] & (1<<k)) dmap[k] = 0x08 << 
j++; 
for (j=k=0; j<3; k++) if (B7colbits[2] & (1<<k)) dmap[k] = 0x40 << 
j++; 
for (i=n=0; i<7; i++) 
for (j=i+1; j<8; j++) 
for (k=j+1; k<9; k++) 
cmap789[n++] = CBLOOKUP(dmap[i]+dmap[j]+dmap[k]); 

/* find all mapped B5/6/8/9 
*/ 
for (i=0; i<56; i++) { 
int newc[3]; 

j = B2compat[i]; 
newc[0] = cmap456[ijk[j][0]]; 
newc[1] = cmap456[ijk[j][1]]; 
newc[2] = cmap456[ijk[j][2]]; 
ORDER3(newc[0],newc[1],newc[2]); 
B5[i] = IJKLOOKUP(newc[0],newc[1],newc[2]); 

j = B3compat[i]; 
newc[0] = cmap456[ijk[j][0]]; 
newc[1] = cmap456[ijk[j][1]]; 
newc[2] = cmap456[ijk[j][2]]; 
ORDER3(newc[0],newc[1],newc[2]); 
B6[i] = IJKLOOKUP(newc[0],newc[1],newc[2]); 

j = B2compat[i^1]; 
newc[0] = cmap789[ijk[j][0]]; 
newc[1] = cmap789[ijk[j][1]]; 
newc[2] = cmap789[ijk[j][2]]; 
ORDER3(newc[0],newc[1],newc[2]); 
B8[i] = IJKLOOKUP(newc[0],newc[1],newc[2]); 

j = B3compat[i^1]; 
newc[0] = cmap789[ijk[j][0]]; 
newc[1] = cmap789[ijk[j][1]]; 
newc[2] = cmap789[ijk[j][2]]; 
ORDER3(newc[0],newc[1],newc[2]); 
B9[i] = IJKLOOKUP(newc[0],newc[1],newc[2]); 
} 

/* finally: count 
*/ 
for (i=k=0; i<56; i++) 
for (j=0; j<56; j++) 
k += nsoln[cmatrix[B5[i]][B6[j]]-1] 
* nsoln[cmatrix[B8[i]][B9[j]]-1]; 

return k; 
} 

/* 
----------------------------------------------------------------------- 

* loop over boxes B4,B7 for a given class, calling the count() 
* function for each ordered pair 
* 
----------------------------------------------------------------------- 

*/ 

static void loop28(int cl, int *ct) { 
int myct[4] = {0}; 
int *B2compat, *B3compat; 
int i, j; 

/* get lists of boxes compatible with B2,B3 
*/ 
B2compat = boxcompat[canon[cl]/280]; 
B3compat = boxcompat[canon[cl]%280]; 

/* loop over possible B4,B7 (given B1) 
* - no point using boxcompat[][] for this 
*/ 
for (i=0; i<20; i++) { 
int B4col0; 
int B4colbits[3]; 
int B7colbits[3]; 
if ( (B4col0=colcompat[0][i][0]) < colcompat[0][i][1] ) { 
B4colbits[0] = colBits[B4col0]; 
B7colbits[0] = colBits[B4col0] ^ 0x1f8; 
for (j=0; j<20; j++) { 
if ( !(0x038&(B4colbits[1]=colBits[colcompat[B4col0][j][0]])) \ 
&& !(0x1c0&(B4colbits[2]=colBits[colcompat[B4col0][j][1]])) ) { 
B7colbits[1] = B4colbits[1] ^ 0x1c7; 
B7colbits[2] = B4colbits[2] ^ 0x03f; 

/* count grid completions given B1,B2,B3,B4,B7 
*/ 
myct[0] += count(B2compat,B3compat,B4colbits,B7colbits); 
myct[1] += myct[0] >> 28; 
myct[0] &= 0x0fffffff; 
} 
} 
} 
} 

/* split into two loops or suffer the consequences! 
*/ 
myct[2] = myct[3] = 0; 
for (i=0; i<nclass[cl]; i++) { 
myct[2] += myct[0]; 
myct[3] += myct[1] + (myct[2]>>28); 
myct[2] &= 0x0fffffff; 
} 
for (i=0; i<nsoln[cl]/6; i++) { 
ct[0] += myct[2]; 
ct[1] += myct[3] + (ct[0]>>28); 
ct[0] &= 0x0fffffff; 
} 

#if !(TIMING) 
printf("Class %2d : subtotal %07x%07x * 9! * 2\n",cl+1,ct[1],ct[0]); 

#endif 

return; 
} 

/* 
----------------------------------------------------------------------- 

* PROGRAM START 
* 
----------------------------------------------------------------------- 

*/ 

int main(void) { 

#if TIMING 

/* ------------------------------------------------ */ 
/* version of the code used for performance testing */ 
/* ------------------------------------------------ */ 

int iter, t; 

printf("Timing 100 iterations ...\n"); 

t = -clock(); 
for (iter=0; iter<100; iter++) { 
pretabulate(); 
get44(); 
} 
t += clock(); 
printf("Step 0: average %f seconds\n",t/(iter*1.0*CLK_TCK)); 

t = -clock(); 
for (iter=0; iter<100; iter++) { 
int ct[2] = {0,0}; 
int i; 
for (i=0; i<44; i++) loop28(i,ct); 
} 
t += clock(); 
printf("Step 1: average %f seconds\n",t/(iter*1.0*CLK_TCK)); 

#else 

/* --------------------------------------------- */ 
/* version of the code used for normal operation */ 
/* --------------------------------------------- */ 

int ct[2] = {0}; 
int i; 

pretabulate(); 
get44(); 
for (i=0; i<44; i++) loop28(i,ct); 

#endif 

return 0; 
}
  

