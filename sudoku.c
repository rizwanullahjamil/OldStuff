// Copyright 2005 Thomas R. Davis
//
// This file is part of Sudoku.
//
// Sudoku is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Sudoku is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sudoku; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// This code solves sudoku puzzles.
//
// Usage: sudoku [-shj] [-f filter] [-J seed] <filename>
//
// where <filename> contains the puzzle encoded as follows:

// *** See below: the format is now much more flexible.

/*
..48.....
.9.46..7.
.5....614
21.6..5..
58.7.9.41
..7..8.69
345....9.
.6..37.2.
.....41..
*/

// If the -s option is used, the program will print the board
// after each number or test number is filled in and you
// need to type <return> to go to the next step; otherwise
// it just prints the answer (or indicates failure). If -s
// is in effect, after every step (and at the beginning)
// the program dumps an Encapsulated PostScript file showing
// the current board position, but only if SIZE == 3.
//
// The -j option causes the board to be
// totally shuffled before displaying it: there are random
// transpositions, swaps within and among rows and columns,
// as well as a random permutation of the entry values.  The
// random number seed is generated from the time of day so this
// will yield a different jumbling with each use.
//
// The -J option is similar, but it jumbles based on the seed
// given by the parameter for -J
//
// The -h option prints the help message
//
// The -f option filters the "possible" candidates shown in
// each square that is not yet assigned.  If this option is
// not included, all candidates are listed.  If the filter
// is the single digit '0', none are printed.  If the filter
// is, say, '157' only those values are printed.  This does
// not work when compiled for SIZE == 4.
//
// SIZE is 3 or 4, depending on whether the puzzle is
// 9x9 (with numbers 1 through 9) or 16x16
// (with numbers 0, 1, 2, ..., 9, a, b, c, d, e, f
// (or A, B, C, D, E, F)).
//
// The "no entry" character is '.', 'x', or 'X'.
//
// Here's a 16 x 16 puzzle you can try if you compile sudoku with
// SIZE = 4:

/*
8xxxxxxxxebxxxxx
xex4xxx8af1xxxx6
xx3xx729xd6xxe1x
xxb6xefd8xxx2xxx
x6xdxfxxb8xx1x4x
3xxxeb5xxxfxa8xd
x8fbxxx4xx7xcxxx
1xa2xx3xxxc4xxxx
xxxx41xxxaxxbdxc
xxx1xdxx3xxx790x
2xe9x8xxxc0bxxxf
xbxfxxa0xxex5x2x
xxx0xxxb965xe7xx
xf6xx39x07dxx2xx
bxxxx48afxxxdxcx
xxxxx57xxxxxxxx0
*/

// The code is now improved to take almost anything, as long as
// there are 81 values for a 9x9 or 256 values for a 16x16,
// but now the "no entry" character must be a '.', an 'X', an
// 'x' or a space (' '), so files like this or similar are valid:

/*
..7|...|.9.
.6.|.8.|3..
39.|2.7|..5
-----------
...|4..|..2
.1.|.9.|.8.
5..|..3|...
-----------
6..|9.2|.71
..3|.4.|.5.
.2.|...|6..
*/

// You can obtain as many test puzzles as you want from:
//
//    http://www.websudoku.com/

// If you run the program with the -s (slow) option, after each
// time you hit the <return> key, an EPS (Encapsulated PostScript)
// file called "puz.eps" is written in the current directory that
// displays the current state of the puzzle.  If you were, for
// example, documenting a solution, you could rename the file after
// each press of the <return> key.

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// only define one of the following:

#define SIZE 3
//#define SIZE 4


#define LEN SIZE*SIZE
#define UNKNOWN 1000

#define absval(x) ((x) > 0 ? (x) : (-(x)))

#define NOSHARE 0
#define ROW 1
#define COLUMN 2
#define BLOCK 4

struct square {
    int value;
    int possible[LEN];
};

int find1, find2, find3[LEN], find4, find5, find6, find7, find8, find9, push[100];
int find10, find11, find12, find13, find14, find15;
int slow;
int jum, randseed = -12345;
char *sudokufile;

char digitfilter[LEN+1];

int scount; // solution count

void printstats()
{
    int i, j;

    if (find2) printf("single possibility: %d\n", find2);
    if (find1) printf("one in row/column/block: %d\n", find1);
    if (find3[2]) printf("naked pairs: %d\n", find3[2]);
    if (find3[3]) printf("naked triplets: %d\n", find3[3]);
    if (find3[4]) printf("naked quads: %d\n", find3[4]);
    if (find3[5]) printf("naked quintuplets: %d\n", find3[5]);
    if (find4) printf("locked candidates: %d\n", find4);
    if (find5) printf("hidden pair: %d\n", find5);
    if (find6) printf("hidden triple: %d\n", find6);
    if (find7) printf("hidden quad: %d\n", find7);
    if (find8) printf("x-wing: %d\n", find8);
    if (find9) printf("xy-wing: %d\n", find9);
    if (find10) printf("swordfish: %d\n", find10);
    if (find13) printf("jellyfish: %d\n", find13);
    if (find11) printf("coloring: %d\n", find11);
    if (find12) printf("multicoloring: %d\n", find12);
    if (find14) printf("unique constraint: %d\n", find14);
    if (find15) printf("forcing chain: %d\n", find15);
    for (j = 99; j >=0; j--) if (push[j] != 0) break;
    if (j == 0 && push[0])
        printf("push[0] = %d\n", push[0]);
    else
        for (i = 0; i <= j; i++)
            printf("push[%d] = %d\n", i, push[i]);
}

struct square puzzle[LEN][LEN];

void initsearchvalues()
{
    int i;

    // initialiaze difficulty counters
    find1 = find2 = find4 = find5 = find6 = find7 = find8 = 0;
    find9 = find10 = find11 = find12 = find13 = find14 = find15 = 0;
    scount = 0;
    for (i = 0; i < 100; i++) push[i] = 0;
    for (i = 0; i < LEN; i++) find3[i] = 0;
}

void initpuzzle()
{
    int i, j, k;
    
    initsearchvalues();

    // initialize puzzle
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            puzzle[i][j].value = UNKNOWN;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            for (k = 0; k < LEN; k++)
                puzzle[i][j].possible[k] = 1;
                
}

void resetpossible()
{
    int i, j, k, l, v;
    int iblock, jblock;

    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            if ((v=puzzle[i][j].value) != UNKNOWN) {
                for (k = 0; k < LEN; k++) {
                    puzzle[i][k].possible[v]=0;
                    puzzle[k][j].possible[v]=0;
                }
                iblock = (i / SIZE) * SIZE;
                jblock = (j / SIZE) * SIZE;
                for (k = 0; k < SIZE; k++)
                    for (l = 0; l < SIZE; l++)
                        puzzle[iblock+k][jblock+l].possible[v] = 0;
            }
        }
}

#include <time.h>

void rotatepuz() // rotate 90 degrees
{
    struct square tmp[LEN][LEN];
    int i, j;
    
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            tmp[i][j] = puzzle[8-j][i];
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            puzzle[i][j] = tmp[i][j];
}

void transposepuz()
{
    int i, j;
    struct square tmp;
    for (i = 0; i < LEN-1; i++)
        for (j = i+1; j < LEN; j++) {
           tmp = puzzle[i][j];
           puzzle[i][j] = puzzle[j][i];
           puzzle[j][i] = tmp;
        }
}

void permvalues(int perm[LEN])
{
    int i, j, v;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            if ((v = puzzle[i][j].value) != UNKNOWN)
                puzzle[i][j].value = perm[v];
}

void permrows(int block, int perm[SIZE])
{
    struct square tmp[SIZE];
    int i, j, k;
    
    for (i = 0; i < LEN; i++) {
        for (j = 0; j < SIZE; j++)
            tmp[j] = puzzle[SIZE*block + perm[j]][i];
        for (j = 0; j < SIZE; j++)
            puzzle[SIZE*block+j][i] = tmp[j];
    }
}

void permcols(int block, int perm[SIZE])
{
    struct square tmp[SIZE];
    int i, j, k;
    
    for (i = 0; i < LEN; i++) {
        for (j = 0; j < SIZE; j++)
            tmp[j] = puzzle[i][SIZE*block + perm[j]];
        for (j = 0; j < SIZE; j++)
            puzzle[i][SIZE*block+j] = tmp[j];
    }
}

void permrowblocks(int perm[SIZE])
{
    struct square tmp[SIZE][SIZE];
    int i, j, k;
    for (j = 0; j < LEN; j++) {
        for (i = 0; i < SIZE; i++)
            for (k = 0; k < SIZE; k++)
                tmp[i][k] = puzzle[3*perm[i]+k][j];
        for (i = 0; i < SIZE; i++)
            for (k = 0; k < SIZE; k++)
                puzzle[3*i+k][j] = tmp[i][k];
    }
}

void permcolblocks(int perm[SIZE])
{
    struct square tmp[SIZE][SIZE];
    int i, j, k;
    for (j = 0; j < LEN; j++) {
        for (i = 0; i < SIZE; i++)
            for (k = 0; k < SIZE; k++)
                tmp[i][k] = puzzle[j][3*perm[i]+k];
        for (i = 0; i < SIZE; i++)
            for (k = 0; k < SIZE; k++)
                puzzle[j][3*i+k] = tmp[i][k];
    }
}

void shuffle(int count, int array[])
{
    int i, k, tmp;

    for (i = 0; i < count-1; i++) {
        k = rand() % (count-i);
        tmp = array[k]; array[k] = array[i]; array[i] = tmp;
    }
}

void symjumble() // symmetric jumble
{
    int i, j, perm[LEN];
    j = (rand()%4);
    for (i = 0; i < j; i++)
        rotatepuz();
    if (rand()%2) transposepuz();
    for (i = 0; i < LEN; i++) perm[i] = i;
    for (i = 0; i < SIZE; i++) perm[i] = SIZE - i -1;
    i = rand()%2;
    if (i) { // works, but not quite right for SIZE = 4;
        permrowblocks(perm);
        permrows(1, perm);
    }
    i = rand()%2;
    if (i) { // works, but not quite right for SIZE = 4;
        permcolblocks(perm);
        permcols(1, perm);
    }
    // other shuffling here ... XXX
    
    
    shuffle(LEN, perm);
    permvalues(perm);
}

void jumble(int symmetric)
{
    int i, j, perm[LEN];
    struct tm *newtime;
    time_t ltime;
    
    if (randseed == -12345) {
        time(&ltime);
        newtime = localtime(&ltime);
        srand(newtime->tm_sec);
    } else
        srand(randseed);
    if (symmetric) {
        symjumble();
        return;
    }
    for (i = 0; i < LEN; i++) perm[i] = i;
    j = (rand()%4);
    for (i = 0; i < j; i++)
        rotatepuz();
    if (rand()%2) transposepuz();
    shuffle(SIZE, perm);
    permrowblocks(perm);
    shuffle(SIZE, perm);
    permcolblocks(perm);
    for (i = 0; i < SIZE; i++) {
        shuffle(SIZE, perm);
        permrows(i, perm);
        shuffle(SIZE, perm);
        permcols(i, perm);
    }
    shuffle(LEN, perm);
    permvalues(perm);
}

int solvable()
{
    int i, j, k, count;
    
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            count = 0;
            if (puzzle[i][j].value == UNKNOWN) {
                for (k = 0; k < LEN; k++)
                   if (puzzle[i][j].possible[k]) { count++; break; }
                if (count == 0) return 0;
            }
        }
    return 1;
}

int solved()
{
    int i, j;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            if (puzzle[i][j].value == UNKNOWN) return 0;
    return 1;
}

int readpuzzle(FILE *f)
{
    int i, j;
    int ch, gotentry;
    
    initpuzzle();
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            gotentry = 0;
            do {
                ch = fgetc(f);
                if (ch == EOF) return 0;
                if (SIZE==3 && isdigit(ch)) {
                    if (ch == '9') ch = '0';
                    puzzle[i][j].value = ch - '0';
                    gotentry = 1;
                } else if (SIZE == 4 && isxdigit(ch)) {
                    if ('0' <= ch && ch <= '9')
                        puzzle[i][j].value = ch - '0';
                    if ('a' <= ch && ch <= 'f')
                        puzzle[i][j].value = ch - 'a' + 10;
                    if ('A' <= ch && ch <= 'F')
                        puzzle[i][j].value = ch - 'A' + 10;
                    gotentry = 1;
                } else if (ch == '.' || ch == 'x' || ch == 'X') {
                    puzzle[i][j].value = UNKNOWN;
                    gotentry = 1;
                }
            } while (gotentry == 0);
        }
    return 1;
}

// The following is because the 9x9 sudoku uses 1-9 and internally
// the program uses 0-8 or 0-15, depending on the puzzle size.

int convert(int i)
{
    if (i == 0 && SIZE == 3) return 9;
    return i;
}

int findobvious()
{
    int i, j, k, count, last, num;
    int iblock, jblock, lasti, lastj;
    
    last = lastj = lasti = 0;
    // see if a square can only be filled in one way:
    
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            if (puzzle[i][j].value == UNKNOWN) {
                count = 0;
                for (k = 0; k < LEN; k++)
                    if (puzzle[i][j].possible[k])
                        {last = k; count++;}
                if (count == 1) {
                    puzzle[i][j].value = last;
                    find2++;
                    if (slow == 1 && scount == 0)
                        printf("no other possibility: row: %d column: %d value: %d\n",
                            i+1, j+1, convert(last));
                    return 1;
                }
            }

    // now go through each row, column, and block to see if
    // only one of the entries can contain a missing number:

    for (num = 0; num < LEN; num++) {
        // check rows:
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (j = 0; j < LEN; j++) {
                if (puzzle[i][j].value == UNKNOWN && 
                            puzzle[i][j].possible[num])
                    { count++; last = j; }
            }
            if (count == 1) {
                puzzle[i][last].value = num;
                find1++;
                if (slow == 1 && scount == 0)
                    printf("row unique missing: row: %d column: %d value: %d\n",
                        i+1, last+1, convert(num));
                return 1;
            }
        }
        // check columns:
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (j = 0; j < LEN; j++) {
                if (puzzle[j][i].value == UNKNOWN && 
                            puzzle[j][i].possible[num])
                    { count++; last = j; }
            }
            if (count == 1) {
                puzzle[last][i].value = num;
                find1++;
                if (slow == 1 && scount == 0)
                    printf("column unique missing: row: %d column: %d value: %d\n",
                        last+1, i+1, convert(num));
                return 1;
            }
        }
        // check blocks:
        for (iblock = 0; iblock < LEN; iblock += SIZE)
          for (jblock = 0; jblock < LEN; jblock += SIZE) {
            count = 0;
            for (i = 0; i < SIZE; i++)
              for (j = 0; j < SIZE; j++) {
                if (puzzle[iblock+i][jblock+j].value == UNKNOWN && 
                            puzzle[iblock+i][jblock+j].possible[num])
                    { count++; lasti = iblock+i; lastj = jblock+j; }
            }
            if (count == 1) {
                puzzle[lasti][lastj].value = num;
                find1++;
                if (slow == 1 && scount == 0)
                    printf("block unique missing: row: %d column: %d value: %d\n",
                        lasti+1, lastj+1, convert(num));
                return 1;
            }
        }

    }


    return 0;
}

// findnakedhelp looks on a virtual "line" of LEN squares that need one
// of each number in each slot.  If there are, for example, exactly
// two slots that have to contain exactly the same two numbers, those
// numbers can't go in other slots.  Same with exactly the same three
// numbers in three slots, et cetera.  This routine will return 1 if
// it finds such a situation and is able to eliminate a possibility from
// an additional slot.  It returns zero on failure.  This routine does
// not fill in any slots as solved: it just cuts down the number of
// possibilities that can go in other slots.
//
// The "size" parameter tells how big a set to look for (2 of 2, 3 of
// 3, et cetera).

int findnakedhelp(struct square line[LEN], int size)
{
    int count;
    int i, j, k, l, fail, match;
    int vals[100], vals1[100], squares[100];
    int i1, i2, i3, i4, i5;
    int values[LEN][LEN], counts[LEN];
    int naked;
    
    count = naked = 0;
    for (i = 0; i < LEN; i++)
        if (line[i].value == UNKNOWN) count++;
    if (count <= size) return 0;
    
    for (i = 0; i < LEN; i++) {
        counts[i] = 0;
        for (j = 0; j < LEN; j++)
            if (line[i].value == UNKNOWN && line[i].possible[j])
                { values[i][j] = 1; counts[i]++; }
            else
                values[i][j] = 0;
    }
    
    if (size == 2) {
        for (i1 = 0; i1 < LEN-1; i1++)
            for (i2 = i1+1; i2 < LEN; i2++) {
                count = 0;
                for (k = 0; k < LEN; k++) {
                    if (counts[k] == 0 || counts[k] > size) continue;
                    fail = 0;
                    for (i = 0; i < LEN; i++)
                        if (i != i1 && i != i2 &&
                            values[k][i]) fail = 1;
                    if (fail) continue;
                    squares[count++] = k; // one of the possible set
                }
                if (count != size) continue;
                for (i = 0; i < LEN; i++) {
                    fail = 0;
                    for (k = 0; k < count; k++)
                        if (i == squares[k]) fail = 1;
                    if (fail) continue;
                    for (k = 0; k < LEN; k++)
                        if ((k == i1 || k == i2) &&
                            line[i].value == UNKNOWN && line[i].possible[k]) {
                                line[i].possible[k] = 0;
                                if (slow) printf("naked pair: square %d value %d\n",
                                    i+1, convert(k));
                                naked++;
                                find3[2]++;
                        }
                }
            }
    }
    
    if (size == 3) {
        for (i1 = 0; i1 < LEN-2; i1++)
            for (i2 = i1+1; i2 < LEN-1; i2++) 
                for (i3 = i2+1; i3 < LEN; i3++) {
                    count = 0;
                    for (k = 0; k < LEN; k++) {
                        if (counts[k] == 0 || counts[k] > size) continue;
                        fail = 0;
                        for (i = 0; i < LEN; i++)
                            if (i != i1 && i != i2 && i != i3 &&
                                values[k][i]) fail = 1;
                        if (fail) continue;
                        squares[count++] = k; // one of the possible set
                    }
                    if (count != size) continue;
                    for (i = 0; i < LEN; i++) {
                        fail = 0;
                        for (k = 0; k < count; k++)
                            if (i == squares[k]) fail = 1;
                        if (fail) continue;
                        for (k = 0; k < LEN; k++)
                            if ((k == i1 || k == i2 || k == i3) &&
                                line[i].value == UNKNOWN && line[i].possible[k]) {
                                    line[i].possible[k] = 0;
                                    if (slow) printf("naked triple: square %d value %d\n",
                                        i+1, convert(k));
                                    naked++;
                                    find3[3]++;
                            }
                    }
                }
    }
    
    if (size == 4) {
        for (i1 = 0; i1 < LEN-3; i1++)
            for (i2 = i1+1; i2 < LEN-2; i2++) 
                for (i3 = i2+1; i3 < LEN-1; i3++) 
                    for (i4 = i3+1; i4 < LEN; i4++) {
                        count = 0;
                        for (k = 0; k < LEN; k++) {
                            if (counts[k] == 0 || counts[k] > size) continue;
                            fail = 0;
                            for (i = 0; i < LEN; i++)
                                if (i != i1 && i != i2 && i != i3 && i != i4 &&
                                    values[k][i]) fail = 1;
                            if (fail) continue;
                            squares[count++] = k; // one of the possible set
                        }
                        if (count != size) continue;
                        for (i = 0; i < LEN; i++) {
                            fail = 0;
                            for (k = 0; k < count; k++)
                                if (i == squares[k]) fail = 1;
                            if (fail) continue;
                            for (k = 0; k < LEN; k++)
                                if ((k == i1 || k == i2 || k == i3 || k == i4) &&
                                    line[i].value == UNKNOWN && line[i].possible[k]) {
                                        line[i].possible[k] = 0;
                                        if (slow) printf("naked quad: square %d value %d\n",
                                            i+1, convert(k));
                                        naked++;
                                        find3[4]++;
                                }
                        }
                    }
    }
    
    if (size == 5) {
        for (i1 = 0; i1 < LEN-4; i1++)
            for (i2 = i1+1; i2 < LEN-3; i2++) 
                for (i3 = i2+1; i3 < LEN-2; i3++) 
                    for (i4 = i3+1; i4 < LEN-1; i4++)
                        for (i5 = i4+1; i5 < LEN; i5++) {
                            count = 0;
                            for (k = 0; k < LEN; k++) {
                                if (counts[k] == 0 || counts[k] > size) continue;
                                fail = 0;
                                for (i = 0; i < LEN; i++)
                                    if (i != i1 && i != i2 && i != i3 && i != i4 && i != i5 &&
                                        values[k][i]) fail = 1;
                                if (fail) continue;
                                squares[count++] = k; // one of the possible set
                            }
                            if (count != size) continue;
                            for (i = 0; i < LEN; i++) {
                                fail = 0;
                                for (k = 0; k < count; k++)
                                    if (i == squares[k]) fail = 1;
                                if (fail) continue;
                                for (k = 0; k < LEN; k++)
                                    if ((k == i1 || k == i2 || k == i3 || k == i4 || k == i5) &&
                                        line[i].value == UNKNOWN && line[i].possible[k]) {
                                            line[i].possible[k] = 0;
                                            if (slow) printf("naked quintuplet: square %d value %d\n",
                                                i+1, convert(k));
                                            naked++;
                                            find3[5]++;
                                    }
                            }
                        }
    }
    
    if (naked) return 1;
    return 0;
}

int findnaked()
{
    int i, j, k;
    int i1, j1;
    struct square line[LEN];
    
    for (k = 2; k < 5; k++) {
        for (i = 0; i < LEN; i++) {
            for (j = 0; j < LEN; j++)
                line[j] = puzzle[i][j];
            if (findnakedhelp(line, k)) {
                for (j = 0; j < LEN; j++)
                    puzzle[i][j] = line[j];
                if (slow) printf("from row: %d\n", i+1); 
                return 1;
            }
        }
        for (i = 0; i < LEN; i++) {
            for (j = 0; j < LEN; j++)
                line[j] = puzzle[j][i];
            if (findnakedhelp(line, k)) {
                for (j = 0; j < LEN; j++)
                    puzzle[j][i] = line[j];
                if (slow) printf("from column: %d\n", i+1); 
                return 1;
            }
        }
        for (i1 = 0; i1 < LEN; i1 += SIZE)
            for (j1 = 0; j1 < LEN; j1 += SIZE) {
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        line[i*SIZE + j] = puzzle[i1+i][j1+j];
                if (findnakedhelp(line, k)) {
                    for (i = 0; i < SIZE; i++)
                        for (j = 0; j < SIZE; j++)
                            puzzle[i1+i][j1+j] = line[i*SIZE + j];
                    if (slow) printf("from block: %d %d\n", i1+1, j1+1);
                    return 1;
                }
            }
    }
    return 0;
}

int lockedcandidates()
{
    int i1, j1, i, j, k, rows;
    int found = 0;
    
    // Check for type 1: within a box, all possibilities in a row
    // if so, nuke the rest in the row outside the box...
    for (i1 = 0; i1 < SIZE; i1++)
        for (j1 = 0; j1 < SIZE; j1++) {
            for (k = 0; k < LEN; k++) { // try each value
                rows = 0;
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        if (puzzle[i1*SIZE+i][j1*SIZE+j].value == UNKNOWN &&
                            puzzle[i1*SIZE+i][j1*SIZE+j].possible[k])
                                rows |= (1 << i);
                if (rows == 1 || rows == 2 || rows == 4 || rows == 8){
                    for (i = 0; i < SIZE; i++) if (rows == (1<<i)) {
                        for (j = 0; j < LEN; j++)
                            if (puzzle[i1*SIZE+i][j].value == UNKNOWN &&
                                (j < j1*SIZE || j >= (j1+1)*SIZE) &&
                                puzzle[i1*SIZE+i][j].possible[k]) {
                                    puzzle[i1*SIZE+i][j].possible[k] = 0;
                                    if (slow) printf("row-locked candidate1: "
                                        "%d invalid in row: %d, column %d\n",
                                            convert(k), i1*SIZE+i+1, j+1);
                                    found++;
                            }
                    }
                }
                rows = 0;
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        if (puzzle[j1*SIZE+j][i1*SIZE+i].value == UNKNOWN &&
                            puzzle[j1*SIZE+j][i1*SIZE+i].possible[k])
                                rows |= (1 << i);
                if (rows == 1 || rows == 2 || rows == 4 || rows == 8){
                    for (j = 0; j < SIZE; j++) if (rows == (1<<j)) {
                        for (i = 0; i < LEN; i++)
                            if (puzzle[i][i1*SIZE+j].value == UNKNOWN &&
                                (i < j1*SIZE || i >= (j1+1)*SIZE) &&
                                puzzle[i][i1*SIZE+j].possible[k]) {
                                    puzzle[i][i1*SIZE+j].possible[k] = 0;
                                    if (slow) printf("column-locked candidate1: "
                                        "%d invalid in row: %d, column %d\n",
                                            convert(k), i+1, i1*SIZE+j+1);
                                    found++;
                            }
                    }
                }
            }
    }
    
    // Now check for type 2: a candidate within a row or column
    // is restricted to be within a box.  Nuke others in the box.
    
    int boxval = 0, ibox;
    
    for (k = 0; k < LEN; k++) { // check each candidate
        for (i = 0; i < LEN; i++) { // check each row
            rows = 0;
            ibox = i/SIZE;
            for (j = 0; j < LEN; j++)
                if (puzzle[i][j].value == UNKNOWN &&
                    puzzle[i][j].possible[k]) {
                        boxval = j/SIZE; rows |= (1<<(j/SIZE));
                }
            if (rows == 1 || rows == 2 || rows == 4 || rows == 8) {
                for (i1 = 0; i1 < SIZE; i1++)
                    if (SIZE*ibox + i1 != i) for (j1 = 0; j1 < SIZE; j1++) {
                        if (puzzle[SIZE*ibox+i1][SIZE*boxval+j1].value == UNKNOWN &&
                            puzzle[SIZE*ibox+i1][SIZE*boxval+j1].possible[k]) {
                            puzzle[SIZE*ibox+i1][SIZE*boxval+j1].possible[k] = 0;
                            if (slow) printf("row-locked candidate2: "
                                   "%d invalid in row %d, column %d\n",
                                   convert(k), SIZE*ibox+i1+1, SIZE*boxval+j1);
                            found++;
                        }
                    }
            }
        }
        for (i = 0; i < LEN; i++) { // check each column
            rows = 0;
            ibox = i/SIZE;
            for (j = 0; j < LEN; j++)
                if (puzzle[j][i].value == UNKNOWN &&
                    puzzle[j][i].possible[k]) {
                        boxval = j/SIZE; rows |= (1<<(j/SIZE));
                }
            if (rows == 1 || rows == 2 || rows == 4 || rows == 8) {
                for (i1 = 0; i1 < SIZE; i1++)
                    if (SIZE*ibox + i1 != i) for (j1 = 0; j1 < SIZE; j1++) {
                        if (puzzle[SIZE*boxval+j1][SIZE*ibox+i1].value == UNKNOWN &&
                            puzzle[SIZE*boxval+j1][SIZE*ibox+i1].possible[k]) {
                            puzzle[SIZE*boxval+j1][SIZE*ibox+i1].possible[k] = 0;
                            if (slow) printf("column-locked candidate2: "
                                   "%d invalid in row %d, column %d\n",
                                   convert(k), SIZE*boxval+j1+1, SIZE*ibox+i1+1);
                            found++;
                        }
                    }
            }
        }
    }
    
    find4 += found;
    if (found) return 1;
    return 0;
}

dumpline(struct square line[LEN]) // for debugging
{
    int i, j;

    printf("dumpline:\n");
    for (i = 0; i < LEN; i++) {
        printf("val: %d\n", line[i].value);
        for (j = 0; j < LEN; j++)
            printf("%d ", line[i].possible[j]);
        printf("\n");
    }
}

int hiddenpairhelp(struct square line[LEN], int type, int val)
{
    int i, j, k, l;
    int count, fail;
    int klist[LEN];
    
    for (i = 0; i < LEN-1; i++)
        for (j = i+1; j < LEN; j++) { // count squares that contain i or j
            fail = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == i || line[k].value == j) fail = 1;
            if (fail) continue;
            count = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == UNKNOWN &&
                    (line[k].possible[i] || line[k].possible[j]))
                        klist[count++] = k;
            if (count == 2) { // found a pair!
                //printf("%d %d %d %d\n", i, j, klist[0], klist[1]);
                //dumpline(line);
                for (k = 0; k < 2; k++)
                    for (l = 0; l < LEN; l++) {
                        if ((l != i) && (l != j) && line[klist[k]].possible[l]) {
                            line[klist[k]].possible[l] = 0;
                            if (slow) {
                                switch(type) {
                                    case ROW: printf("row "); break;
                                    case COLUMN: printf("column "); break;
                                    case BLOCK: printf("block "); break;
                                }
                                printf("hiddenpair: (%d %d); %d impossible in ",
                                convert(i), convert(j), convert(l), klist[k]+1);
                                switch(type) {
                                    case ROW:
                                        printf("row %d, column %d\n",
                                            val+1, klist[k]+1);
                                        break;
                                    case COLUMN:
                                        printf("row %d, column %d\n",
                                            klist[k]+1, val+1);
                                        break;
                                    case BLOCK:
                                        printf("row %d, column %d\n",
                                            SIZE*(val/SIZE) + (klist[k]/SIZE)+1,
                                            SIZE*(val%SIZE) + (klist[k]%SIZE) +1);
                                        break;
                                }
                            }
                            find5++;
                            return 1;
                        }
                    }
            }
        }
    return 0;
}

int hiddentriplehelp(struct square line[LEN], int type, int val)
{
    int i, j, j1, k, l;
    int count, fail;
    int klist[LEN];
    
    for (i = 0; i < LEN-2; i++)
        for (j = i+1; j < LEN-1; j++)
            for (j1 = j+1; j1 < LEN; j1++) { // count squares with i,j,j1
            fail = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == i ||
                    line[k].value == j || line[k].value == j1) fail = 1;
            if (fail) continue;
            count = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == UNKNOWN &&
                    (line[k].possible[i] || line[k].possible[j] || 
                        line[k].possible[j1]))
                            klist[count++] = k;
            if (count == 3) { // found a triplet!
                //printf("%d %d %d %d\n", i, j, klist[0], klist[1]);
                //dumpline(line);
                for (k = 0; k < count; k++)
                    for (l = 0; l < LEN; l++) {
                        if ((l != i) && (l != j) && (l != j1) &&
                          line[klist[k]].possible[l]) {
                            line[klist[k]].possible[l] = 0;
                            if (slow) {
                                switch(type) {
                                    case ROW: printf("row "); break;
                                    case COLUMN: printf("column "); break;
                                    case BLOCK: printf("block "); break;
                                }
                                printf("hiddentriple: (%d %d %d); %d impossible in ",
                                convert(i), convert(j), convert(j1), convert(l));
                                switch(type) {
                                    case ROW:
                                        printf("row %d, column %d\n",
                                            val+1, klist[k]+1);
                                        break;
                                    case COLUMN:
                                        printf("row %d, column %d\n",
                                            klist[k]+1, val+1);
                                        break;
                                    case BLOCK:
                                        printf("row %d, column %d\n",
                                            SIZE*(val/SIZE) + (klist[k]/SIZE)+1,
                                            SIZE*(val%SIZE) + (klist[k]%SIZE) +1);
                                        break;
                                }
                            }
                            find6++;
                            return 1;
                        }
                    }
            }
        }
    return 0;
}

int hiddenquadhelp(struct square line[LEN], int type, int val)
{
    int i, j, j1, j2, k, l;
    int count, fail;
    int klist[LEN];
    
    for (i = 0; i < LEN-3; i++)
        for (j = i+1; j < LEN-2; j++)
            for (j1 = j+1; j1 < LEN-1; j1++)
                for (j2 = j1+1; j2 < LEN; j2++) { // count squares with i,j,j1,j2
            fail = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == i ||
                    line[k].value == j || line[k].value == j1 ||
                    line[k].value == j2) fail = 1;
            if (fail) continue;
            count = 0;
            for (k = 0; k < LEN; k++)
                if (line[k].value == UNKNOWN &&
                    (line[k].possible[i] || line[k].possible[j] || 
                        line[k].possible[j1] || line[k].possible[j2]))
                            klist[count++] = k;
            if (count == 4) { // found a quad!
                //printf("%d %d %d %d\n", i, j, klist[0], klist[1]);
                //dumpline(line);
                for (k = 0; k < count; k++)
                    for (l = 0; l < LEN; l++) {
                        if ((l != i) && (l != j) && (l != j1) && (l != j2) &&
                          line[klist[k]].possible[l]) {
                            line[klist[k]].possible[l] = 0;
                            if (slow) {
                                switch(type) {
                                    case ROW: printf("row "); break;
                                    case COLUMN: printf("column "); break;
                                    case BLOCK: printf("block "); break;
                                }
                                printf("hiddenquad: (%d %d %d %d); %d impossible in ",
                                convert(i), convert(j), convert(j1), 
                                convert(j2), convert(l));
                                switch(type) {
                                    case ROW:
                                        printf("row %d, column %d\n",
                                            val+1, klist[k]+1);
                                        break;
                                    case COLUMN:
                                        printf("row %d, column %d\n",
                                            klist[k]+1, val+1);
                                        break;
                                    case BLOCK:
                                        printf("row %d, column %d\n",
                                            SIZE*(val/SIZE) + (klist[k]/SIZE)+1,
                                            SIZE*(val%SIZE) + (klist[k]%SIZE) +1);
                                        break;
                                }
                            }
                            find7++;
                            return 1;
                        }
                    }
            }
        }
    return 0;
}

findhiddenpair()
{
    int i, j, i1, j1;
    struct square line[LEN];

    for (i = 0; i < LEN; i++) {
        for (j = 0; j < LEN; j++)
            line[j] = puzzle[i][j];
        if (hiddenpairhelp(line, ROW, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[i][j] = line[j];
            //if (slow) printf("from row: %d\n", i+1); 
            return 1;
        }
        if (hiddentriplehelp(line, ROW, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[i][j] = line[j];
            if (slow) printf("from row: %d\n", i+1); 
            return 1;
        }
        if (hiddenquadhelp(line, ROW, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[i][j] = line[j];
            if (slow) printf("from row: %d\n", i+1); 
            return 1;
        }
    }
    for (i = 0; i < LEN; i++) {
        for (j = 0; j < LEN; j++)
            line[j] = puzzle[j][i];
        if (hiddenpairhelp(line, COLUMN, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[j][i] = line[j];
            //if (slow) printf("from column: %d\n", i+1); 
            return 1;
        }
        if (hiddentriplehelp(line, COLUMN, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[j][i] = line[j];
            if (slow) printf("from column: %d\n", i+1); 
            return 1;
        }
        if (hiddenquadhelp(line, COLUMN, i)) {
            for (j = 0; j < LEN; j++)
                puzzle[j][i] = line[j];
            if (slow) printf("from column: %d\n", i+1); 
            return 1;
        }
    }
    for (i1 = 0; i1 < LEN; i1 += SIZE)
        for (j1 = 0; j1 < LEN; j1 += SIZE) {
            for (i = 0; i < SIZE; i++)
                for (j = 0; j < SIZE; j++)
                    line[i*SIZE + j] = puzzle[i1+i][j1+j];
            if (hiddenpairhelp(line, BLOCK, SIZE*(i1/SIZE)+j1/SIZE)) {
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        puzzle[i1+i][j1+j] = line[i*SIZE + j];
                //if (slow) printf("from block: %d %d\n", i1+1, j1+1);
                return 1;
            }
            if (hiddentriplehelp(line, BLOCK, SIZE*(i1/SIZE)+j1/SIZE)) {
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        puzzle[i1+i][j1+j] = line[i*SIZE + j];
                //if (slow) printf("from block: %d %d\n", i1+1, j1+1);
                return 1;
            }
            if (hiddenquadhelp(line, BLOCK, SIZE*(i1/SIZE)+j1/SIZE)) {
                for (i = 0; i < SIZE; i++)
                    for (j = 0; j < SIZE; j++)
                        puzzle[i1+i][j1+j] = line[i*SIZE + j];
                //if (slow) printf("from block: %d %d\n", i1+1, j1+1);
                return 1;
            }
        }
    return 0;
}

int xwing()
{
    int i, j, j1, k, j1save = 0, rcount, i2, j2, printed, count;
    int val[LEN], val1[LEN];

    // first check rows
    for (k = 0; k < LEN; k++) // k = candidate value
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[i][j].value == UNKNOWN &&
                    puzzle[i][j].possible[k])
                        val[count++] = j;
            if (count == 2) {
                rcount = 0;
                for (j1 = 0; j1 < LEN; j1++) {
                    count = 0;
                    for (j = 0; j < LEN; j++)
                        if (puzzle[j1][j].value == UNKNOWN &&
                            puzzle[j1][j].possible[k])
                                val1[count++] = j;
                    if (count == 2 &&
                        val[0] == val1[0] &&
                        val[1] == val1[1]) {
                            if (j1 != i) j1save = j1;
                            rcount++;
                    }
                }
                printed = 0;
                if (rcount == 2) { // got a possible x-wing!
                    for (i2 = 0; i2 < LEN; i2++)
                        for (j2 = 0; j2 < LEN; j2++) {
                            if (i2 != i && i2 != j1save &&
                                (j2 == val[0] || j2 == val[1]) &&
                                puzzle[i2][j2].value == UNKNOWN &&
                                puzzle[i2][j2].possible[k]) {
                                    puzzle[i2][j2].possible[k] = 0;
                                    if (slow)
                                        printf("x-wing candidate %d in rows: %d %d: "
                                               "value %d impossible at (%d %d)\n",
                                                    convert(k), i+1,
                                                    j1save+1, convert(k), i2+1, j2+1);
                                    find8++;
                                    printed = 1;
                                }
                        }
                }
                if (printed == 1) return 1;
            }
        }
    // next check columns
    for (k = 0; k < LEN; k++) // k = candidate value
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[j][i].value == UNKNOWN &&
                    puzzle[j][i].possible[k])
                        val[count++] = j;
            if (count == 2) {
                rcount = 0;
                for (j1 = 0; j1 < LEN; j1++) {
                    count = 0;
                    for (j = 0; j < LEN; j++)
                        if (puzzle[j][j1].value == UNKNOWN &&
                            puzzle[j][j1].possible[k])
                                val1[count++] = j;
                    if (count == 2 &&
                        val[0] == val1[0] &&
                        val[1] == val1[1]) {
                            if (j1 != i) j1save = j1;
                            rcount++;
                    }
                }
                printed = 0;
                if (rcount == 2) { // got a possible x-wing!
                    for (i2 = 0; i2 < LEN; i2++)
                        for (j2 = 0; j2 < LEN; j2++) {
                            if (i2 != i && i2 != j1save &&
                                (j2 == val[0] || j2 == val[1]) &&
                                puzzle[j2][i2].value == UNKNOWN &&
                                puzzle[j2][i2].possible[k]) {
                                    puzzle[j2][i2].possible[k] = 0;
                                    if (slow)
                                        printf("x-wing columns: %d %d: "
                                               "value %d impossible at (%d %d)\n",
                                                    i+1, j1save+1, convert(k), j2+1, i2+1);
                                    find8++;
                                    printed = 1;
                                }
                        }
                }
                if (printed == 1) return 1;
            }
        }
    return 0;
}

// share compares two pairs of coordinates.  It returns:
// 1 if in the same row
// 2 if in the same column
// 4 if in the same block
// 5: row+block
// 6: column+block
// 0 otherwise

int share(int x1, int y1, int x2, int y2)
{
    int retval = 0;
    
    if (x1 == x2) retval = ROW;
    if (y1 == y2) retval = COLUMN;
    if ((x1 / SIZE == x2 / SIZE) &&
        (y1 / SIZE == y2 / SIZE)) retval |= BLOCK;
    return retval;
}

debugxywing(int i, int j, int k, int pairlist[LEN*LEN][4])
{
    printf("xywing:\n");
    printf("(%d %d): (%d < %d)\n", pairlist[i][0]+1,
        pairlist[i][1]+1, convert(pairlist[i][2]),
        convert(pairlist[i][3]));
    printf("(%d %d): (%d < %d)\n", pairlist[j][0]+1,
        pairlist[j][1]+1, convert(pairlist[j][2]),
        convert(pairlist[j][3]));
    printf("(%d %d): (%d < %d)\n", pairlist[k][0]+1,
        pairlist[k][1]+1, convert(pairlist[k][2]),
        convert(pairlist[k][3]));
}

int xywing()
{
    int i, j, k, l;
    int p1, q1, p2, q2, p3, q3;
    int x1, y1, x2, y2, x3, y3;
    int s12, s23, s13;
    int paircount, count;
    int pairlist[LEN*LEN][4]; // totally paranoid size
    int save[LEN]; // only need first 2, but can easily overflow
    int test[LEN];
    int fail;

    // the four entries are: (x, y), (p < q), where (x, y)
    // are the coordinates of the square and p and q are the values
    
    // first, search for squares that have only two candidates:
    paircount = 0;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            count = 0;
            for (k = 0; k < LEN; k++)
                if (puzzle[i][j].value == UNKNOWN &&
                    puzzle[i][j].possible[k]) save[count++] = k;
            if (count == 2) {
                pairlist[paircount][0] = i;
                pairlist[paircount][1] = j;
                pairlist[paircount][2] = save[0];
                pairlist[paircount++][3] = save[1];
            }
        }
    // OK, the pairs are all found
    if (paircount < 3) return 0;
    // now look at all sets of three squares:
    for (i = 0; i < paircount-2; i++)
        for (j = i+1; j < paircount-1; j++)
            for (k = j+1; k < paircount; k++) {
                p1 = pairlist[i][2];
                q1 = pairlist[i][3];
                p2 = pairlist[j][2];
                q2 = pairlist[j][3];
                p3 = pairlist[k][2];
                q3 = pairlist[k][3];
                for (l = 0; l < LEN; l++) test[l] = 0;
                test[p1]++; test[p2]++; test[p3]++;
                test[q1]++; test[q2]++; test[q3]++;
                fail = 0;
                for (l = 0; l < LEN; l++)
                    if (test[l] != 0 && test[l] != 2) fail=1;
                if (fail) continue;
                // possible set!
                x1 = pairlist[i][0];
                y1 = pairlist[i][1];
                x2 = pairlist[j][0];
                y2 = pairlist[j][1];
                x3 = pairlist[k][0];
                y3 = pairlist[k][1];
                
                s12 = share(x1, y1, x2, y2);
                s23 = share(x2, y2, x3, y3);
                s13 = share(x1, y1, x3, y3);
                
                if (s12*s13 + s12*s23 + s13*s23 == 0)
                    continue;
                // at this point, assume one of s12, s23, s23 is
                // zero, or all are in the same row, column, or
                // block and this case is handled elsewhere
                
                fail = 1;
                if (s13 == 0) {
                    int tmp;
                   
                    tmp = x1; x1 = x2; x2 = tmp;
                    tmp = y1; y1 = y2; y2 = tmp;
                    tmp = p1; p1 = p2; p2 = tmp;
                    tmp = q1; q1 = q2; q2 = tmp;
                    tmp = s23; s23 = s13; s13 = tmp;
                }
                if (s12 == 0) {
                    int tmp;
                   
                    tmp = x1; x1 = x3; x3 = tmp;
                    tmp = y1; y1 = y3; y3 = tmp;
                    tmp = p1; p1 = p3; p3 = tmp;
                    tmp = q1; q1 = q3; q3 = tmp;
                    tmp = s23; s23 = s12; s12 = tmp;
                }
                if (s13 & BLOCK) {
                    int tmp;
                    
                    tmp = x2; x2 = x3; x3 = tmp;
                    tmp = y2; y2 = y3; y3 = tmp;
                    tmp = p2; p2 = p3; p3 = tmp;
                    tmp = q2; q2 = q3; q3 = tmp;
                    tmp = s12; s12 = s13; s13 = tmp;
                }
                if (s23 == 0) {
                    int val = 0;
                    // find excluded value:
                    for (l = 0; l < LEN; l++)
                        if (test[l] && l != p1 && l != q1) val = l;
                    if ((s12 & ROW) && (s13 & COLUMN)) {
                        if (puzzle[x3][y2].value == UNKNOWN &&
                                    puzzle[x3][y2].possible[val]) {
                            puzzle[x3][y2].possible[val] = 0;
                            fail = 0;
                            find9++;
                            if (slow) printf("xy-wing: %d impossible in row: %d col %d\n",
                                convert(val), x3+1, y2+1);
                        }
                    }
                    if ((s12 & COLUMN) && (s13 & ROW)) {
                        if (puzzle[x2][y3].value == UNKNOWN &&
                                    puzzle[x2][y3].possible[val]) {
                            puzzle[x2][y3].possible[val] = 0;
                            fail = 0;
                            find9++;
                            if (slow) printf("xy-wing: %d impossible in row: %d col %d\n",
                                convert(val), x2+1, y3+1);
                        }
                    }
                    if ((s12 & BLOCK) && (s13 & ROW)) {
                        int xb;
                        xb = (y1/SIZE)*SIZE;
                        for (l = xb; l < xb+SIZE; l++)
                            if (l != y1 && puzzle[x1][l].value == UNKNOWN &&
                                puzzle[x1][l].possible[val]) {
                                    puzzle[x1][l].possible[val] = 0;
                                    fail = 0;
                                    find9++;
                                    if (slow) printf("xy-wing1: %d impossible in row: %d col %d\n",
                                        convert(val), x1+1, l+1);
                                }
                        xb = (y3/SIZE)*SIZE;
                        for (l = xb; l < xb+SIZE; l++)
                            if (puzzle[x2][l].value == UNKNOWN &&
                                puzzle[x2][l].possible[val]) {
                                    puzzle[x2][l].possible[val] = 0;
                                    fail = 0;
                                    find9++;
                                    if (slow) printf("xy-wing2: %d impossible in row: %d col %d\n",
                                        convert(val), x2+1, l+1);
                                }
                    }
                    if ((s12 & BLOCK) && (s13 & COLUMN)) {
                       int xb;
                       xb = (x1/SIZE)*SIZE;
                        for (l = xb; l < xb+SIZE; l++)
                            if (l != x1 && puzzle[l][y1].value == UNKNOWN &&
                                puzzle[l][y1].possible[val]) {
                                    puzzle[l][y1].possible[val] = 0;
                                    fail = 0;
                                    find9++;
                                    if (slow) printf("xy-wing3: %d impossible in row: %d col %d\n",
                                        convert(val), l+1, y1+1);
                                }
                        xb = (x3/SIZE)*SIZE;
                        for (l = xb; l < xb+SIZE; l++)
                            if (puzzle[l][y2].value == UNKNOWN &&
                                puzzle[l][y2].possible[val]) {
                                    puzzle[l][y2].possible[val] = 0;
                                    fail = 0;
                                    find9++;
                                    if (slow) printf("xy-wing4: %d impossible in row: %d col %d\n",
                                        convert(val), l+1, y2+1);
                                }
                    }
                }
                if (fail == 0) return 1;
                    
            }


    return 0;
}

int bitcount[65536];
static int bitinited = 0;

int sw4() // jellyfish
{
    int i, j, count;
    int candidate, value;
    int bitmap[LEN];
    int j1, j2, j3, j4;
    int found = 0;

    if (bitinited == 0) {
        bitinited = 1;
        for (i = 0; i < 65536; i++) {
            j = i;
            count = 0;
            while (j) {
                if (j&1) count++;
                j>>= 1;
            }
            bitcount[i] = count;
        }
    }
    for (candidate = 0; candidate < LEN; candidate++) {
        // fill in bitmap array for rows:
        for (i = 0; i < LEN; i++) {
            value = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[i][j].value == UNKNOWN &&
                    puzzle[i][j].possible[candidate]) value |= (1 << j);
            bitmap[i] = value;
        }
        for (j1 = 0; j1 < LEN - 3; j1++) {
            if (bitcount[bitmap[j1]] < 2 || bitcount[bitmap[j1]] > 4) continue;
            for (j2 = j1+1; j2 < LEN - 2; j2++) {
                if (bitcount[bitmap[j2]] < 2 || bitcount[bitmap[j2]] > 4) continue;
                for (j3 = j2+1; j3 < LEN - 1; j3++) {
                    if (bitcount[bitmap[j3]] < 2 || bitcount[bitmap[j3]] > 4) continue;
                    for (j4 = j3+1; j4 < LEN; j4++) {
                        if (bitcount[bitmap[j4]] < 2 || bitcount[bitmap[j4]] > 4) continue;
                        value = bitmap[j1] | bitmap[j2] | bitmap[j3] | bitmap[j4];
                        if (bitcount[value] > 4) continue;
                        // possible hit
                        for (i = 0; i < LEN; i++) {
                            if (i == j1 || i == j2 || i == j3 || i == j4) continue;
                            for (j = 0; j < LEN; j++) {
                                if (((1 << j) & value) &&
                                    puzzle[i][j].value == UNKNOWN &&
                                    puzzle[i][j].possible[candidate]) {
                                        if (slow) printf("jellyfish rows: %d %d %d %d\n"
                                               "eliminate: candidate: %d from row: %d col: %d\n",
                                               j1+1, j2+1, j3+1, j4+1,
                                               convert(candidate), i+1, j+1);
                                        puzzle[i][j].possible[candidate] = 0;
                                        find13++;
                                        found = 1;
                                    }
                            }
                        }
                    }
                }
            }
        }
        // now check columns:
        for (i = 0; i < LEN; i++) {
            value = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[j][i].value == UNKNOWN &&
                    puzzle[j][i].possible[candidate]) value |= (1 << j);
            bitmap[i] = value;
        }
        for (j1 = 0; j1 < LEN - 3; j1++) {
            if (bitcount[bitmap[j1]] < 2 || bitcount[bitmap[j1]] > 4) continue;
            for (j2 = j1+1; j2 < LEN - 2; j2++) {
                if (bitcount[bitmap[j2]] < 2 || bitcount[bitmap[j2]] > 4) continue;
                for (j3 = j2+1; j3 < LEN - 1; j3++) {
                    if (bitcount[bitmap[j3]] < 2 || bitcount[bitmap[j3]] > 4) continue;
                    for (j4 = j3+1; j4 < LEN; j4++) {
                        if (bitcount[bitmap[j4]] < 2 || bitcount[bitmap[j4]] > 4) continue;
                        value = bitmap[j1] | bitmap[j2] | bitmap[j3] | bitmap[j4];
                        if (bitcount[value] > 4) continue;
                        // possible hit
                        for (i = 0; i < LEN; i++) {
                            if (i == j1 || i == j2 || i == j3 || i == j4) continue;
                            for (j = 0; j < LEN; j++) {
                                if (((1 << j) & value) &&
                                    puzzle[j][i].value == UNKNOWN &&
                                    puzzle[j][i].possible[candidate]) {
                                        if (slow) printf("jellyfish columns: %d %d %d %d\n"
                                               "eliminate: candidate: %d from row: %d col: %d\n",
                                               j1+1, j2+1, j3+1, j4+1,
                                               convert(candidate), j+1, i+1);
                                        puzzle[j][i].possible[candidate] = 0;
                                        find13++;
                                        found = 1;
                                    }
                            }
                        }
                    }
                }
            }
        }
    }
    return found;
}

int sw3() // improved swordfish
{
    int i, j, count;
    int candidate, value;
    int bitmap[LEN];
    int j1, j2, j3;
    int found = 0;

    if (bitinited == 0) {
        bitinited = 1;
        for (i = 0; i < 65536; i++) {
            j = i;
            count = 0;
            while (j) {
                if (j&1) count++;
                j>>= 1;
            }
            bitcount[i] = count;
        }
    }
    for (candidate = 0; candidate < LEN; candidate++) {
        // fill in bitmap array for rows:
        for (i = 0; i < LEN; i++) {
            value = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[i][j].value == UNKNOWN &&
                    puzzle[i][j].possible[candidate]) value |= (1 << j);
            bitmap[i] = value;
        }
        for (j1 = 0; j1 < LEN - 2; j1++) {
            if (bitcount[bitmap[j1]] < 2 || bitcount[bitmap[j1]] > 3) continue;
            for (j2 = j1+1; j2 < LEN - 1; j2++) {
                if (bitcount[bitmap[j2]] < 2 || bitcount[bitmap[j2]] > 3) continue;
                for (j3 = j2+1; j3 < LEN; j3++) {
                    if (bitcount[bitmap[j3]] < 2 || bitcount[bitmap[j3]] > 3) continue;
                        value = bitmap[j1] | bitmap[j2] | bitmap[j3];
                        if (bitcount[value] > 3) continue;
                        // possible hit
                        for (i = 0; i < LEN; i++) {
                            if (i == j1 || i == j2 || i == j3) continue;
                            for (j = 0; j < LEN; j++) {
                                if (((1 << j) & value) &&
                                    puzzle[i][j].value == UNKNOWN &&
                                    puzzle[i][j].possible[candidate]) {
                                        if (slow) printf("swordfish rows: %d %d %d\n"
                                               "eliminate: candidate: %d from row: %d col: %d\n",
                                               j1+1, j2+1, j3+1,
                                               convert(candidate), i+1, j+1);
                                        puzzle[i][j].possible[candidate] = 0;
                                        find10++;
                                        found = 1;
                                    }
                        }
                    }
                }
            }
        }
        // now check columns:
        for (i = 0; i < LEN; i++) {
            value = 0;
            for (j = 0; j < LEN; j++)
                if (puzzle[j][i].value == UNKNOWN &&
                    puzzle[j][i].possible[candidate]) value |= (1 << j);
            bitmap[i] = value;
        }
        for (j1 = 0; j1 < LEN - 2; j1++) {
            if (bitcount[bitmap[j1]] < 2 || bitcount[bitmap[j1]] > 3) continue;
            for (j2 = j1+1; j2 < LEN - 1; j2++) {
                if (bitcount[bitmap[j2]] < 2 || bitcount[bitmap[j2]] > 3) continue;
                for (j3 = j2+1; j3 < LEN; j3++) {
                    if (bitcount[bitmap[j3]] < 2 || bitcount[bitmap[j3]] > 3) continue;
                        value = bitmap[j1] | bitmap[j2] | bitmap[j3];
                        if (bitcount[value] > 3) continue;
                        // possible hit
                        for (i = 0; i < LEN; i++) {
                            if (i == j1 || i == j2 || i == j3) continue;
                            for (j = 0; j < LEN; j++) {
                                if (((1 << j) & value) &&
                                    puzzle[j][i].value == UNKNOWN &&
                                    puzzle[j][i].possible[candidate]) {
                                        if (slow) printf("swordfish columns: %d %d %d\n"
                                               "eliminate: candidate: %d from row: %d col: %d\n",
                                               j1+1, j2+1, j3+1,
                                               convert(candidate), j+1, i+1);
                                        puzzle[j][i].possible[candidate] = 0;
                                        find10++;
                                        found = 1;
                                    }
                            }
                    }
                }
            }
        }
    }
    return found;
}

int swordfish() // fucked: need to look at 3 individual columns.
{
    int k, can;
    int j1;
    int jj1, jj2, jj3;
    int count, findfish;
    int tmpsave[LEN], tmpsave1[LEN], tmpsave2[LEN], tmpsave3[LEN];
    
    findfish = 0;
    for (can = 0; can < LEN; can++) { // can = candidate
        for (jj1 = 0; jj1 < LEN-2; jj1++) {
            count = 0;
            for (k = 0; k < LEN; k++) {
                tmpsave1[k] = 0;
                if (puzzle[jj1][k].value == UNKNOWN &&
                    puzzle[jj1][k].possible[can]) {
                        tmpsave1[k] = 1;
                        count++;
                }
            }
            if (count > 3 || count < 2) continue;
            for (jj2 = jj1+1; jj2 < LEN-1; jj2++) {
                count = 0;
                for (k = 0; k < LEN; k++) {
                    tmpsave2[k] = 0;
                    if (puzzle[jj2][k].value == UNKNOWN &&
                        puzzle[jj2][k].possible[can]) {
                            tmpsave2[k] = 1;
                            count++;
                    }
                }
                if (count > 3 || count < 2) continue;
                for (jj3 = jj2+1; jj3 < LEN; jj3++) {
                    count = 0;
                    for (k = 0; k < LEN; k++) {
                        tmpsave3[k] = 0;
                        if (puzzle[jj3][k].value == UNKNOWN &&
                            puzzle[jj3][k].possible[can]) {
                                tmpsave3[k] = 1;
                                count++;
                        }
                    }
                    if (count > 3 || count < 2) continue;
                    // we've got a possible triple that works
                    count = 0;
                    for (k = 0; k < LEN; k++)
                        if (tmpsave[k] = tmpsave1[k]|tmpsave2[k]|tmpsave3[k]) 
                            count++;
                    if (count != 3) continue;
                    // Got one!  Let's see if we can nuke some possibilities ...
                    for (k= 0; k < LEN; k++) {
                        if (k != jj1 && k != jj2 && k != jj3)
                            for (j1 = 0; j1 < LEN; j1++)
                                if (tmpsave[j1] == 1 &&
                                    puzzle[k][j1].value == UNKNOWN &&
                                    puzzle[k][j1].possible[can]) {
                                        puzzle[k][j1].possible[can] = 0;
                                        if (slow) {
                                            printf("swordfish rows: %d %d %d:\n",
                                               jj1+1, jj2+1, jj3+1);
                                            printf("eliminate candidate %d "
                                                   "from row %d column %d\n",
                                                        convert(can), k+1, j1+1);
                                        }
                                        findfish++; find10++;
                                }
                    }
                }
            }
        }
        for (jj1 = 0; jj1 < LEN-2; jj1++) {
            count = 0;
            for (k = 0; k < LEN; k++) {
                tmpsave1[k] = 0;
                if (puzzle[k][jj1].value == UNKNOWN &&
                    puzzle[k][jj1].possible[can]) {
                        tmpsave1[k] = 1;
                        count++;
                }
            }
            if (count > 3 || count < 2) continue;
            for (jj2 = jj1+1; jj2 < LEN-1; jj2++) {
                count = 0;
                for (k = 0; k < LEN; k++) {
                    tmpsave2[k] = 0;
                    if (puzzle[k][jj2].value == UNKNOWN &&
                        puzzle[k][jj2].possible[can]) {
                            tmpsave2[k] = 1;
                            count++;
                    }
                }
                if (count > 3 || count < 2) continue;
                for (jj3 = jj2+1; jj3 < LEN; jj3++) {
                    count = 0;
                    for (k = 0; k < LEN; k++) {
                        tmpsave3[k] = 0;
                        if (puzzle[k][jj3].value == UNKNOWN &&
                            puzzle[k][jj3].possible[can]) {
                                tmpsave3[k] = 1;
                                count++;
                        }
                    }
                    if (count > 3 || count < 2) continue;
                    // we've got a possible triple that works
                    count = 0;
                    for (k = 0; k < LEN; k++)
                        if (tmpsave[k] = tmpsave1[k]|tmpsave2[k]|tmpsave3[k]) 
                            count++;
                    if (count != 3) continue;
                    // Got one!  Let's see if we can nuke some possibilities ...
                    for (k= 0; k < LEN; k++) {
                        if (k != jj1 && k != jj2 && k != jj3)
                            for (j1 = 0; j1 < LEN; j1++)
                                if (tmpsave[j1] == 1 &&
                                    puzzle[j1][k].value == UNKNOWN &&
                                    puzzle[j1][k].possible[can]) {
                                        puzzle[j1][k].possible[can] = 0;
                                        if (slow) {
                                            printf("swordfish columns: %d %d %d:\n",
                                               jj1+1, jj2+1, jj3+1);
                                            printf("eliminate candidate %d "
                                                   "from row %d column %d\n",
                                                        convert(can), j1+1, k+1);
                                        }
                                        findfish++; find10++;
                                }
                    }
                }
            }
        }
    }
    if (findfish) return 1;
    return 0;
}

int doublepair()  // bad code!
{
    int i, j, k, l, m, v, c, count, n, o, p, q;
    int r, c1, c2;
    int R;
    int can[LEN][LEN];
    
    if (bitinited == 0) {
        bitinited = 1;
        for (i = 0; i < 65536; i++) {
            j = i;
            count = 0;
            while (j) {
                if (j&1) count++;
                j>>= 1;
            }
            bitcount[i] = count;
        }
    }
    
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            can[i][j] = 0;
            for (k = 0; k < LEN; k++)
                if (puzzle[i][j].value == UNKNOWN && puzzle[i][j].possible[k])
                    can[i][j] |= (1 << k);
        }

    count = 0;
    // check rows first:
    for (i = 0; i < SIZE; i++)
      for (j = 0; j < SIZE; j++)
        for (k = 0; k < SIZE; k++)
          for (l = 0; l < SIZE-1; l++)
            for (m = l+1; m < SIZE; m++) {
                r = i*SIZE+k;
                c1 = j*SIZE+l;
                c2 = j*SIZE+m;
                if (can[r][c1] == can[r][c2] &&
                    bitcount[can[r][c1]] == 2) {
                    for (n = 0; n < SIZE; n++) {
                      if (n == i) continue;
                      for (o = 0; o < SIZE; o++) {
                        R = n*SIZE + o;
                        if (can[R][c1] == can[R][c2] && ((can[R][c1] & can[r][c1]) == can[r][c1]) &&
                            bitcount[can[R][c1]] == 3) {
                            for (c = 0; c < LEN; c++)
                                if ((can[r][c1] ^ can[R][c1]) == (1 << c)) v = c;
                            for (p = 0; p < SIZE; p++)
                              for (q = 0; q < SIZE; q++)
                                if (p != o || (q != l && q != m))
                                  if (puzzle[n*SIZE+p][j*SIZE+q].value == UNKNOWN &&
                                      puzzle[n*SIZE+p][j*SIZE+q].possible[v]) {
                                        printf("row unique constraint: row: %d col: %d, value: %d\n",
                                          n*SIZE+p+1, j*SIZE+q+1, convert(v));
                                        puzzle[n*SIZE+p][j*SIZE+q].possible[v] = 0;
                                        find14++;
                                        count = 1;
                                  }
                        }
                    }
                  }
                }
            }

    // now check columns:
    for (i = 0; i < SIZE; i++)
      for (j = 0; j < SIZE; j++)
        for (k = 0; k < SIZE; k++)
          for (l = 0; l < SIZE-1; l++)
            for (m = l+1; m < SIZE; m++) {
                r = i*SIZE+k;
                c1 = j*SIZE+l;
                c2 = j*SIZE+m;
                if (can[c1][r] == can[c2][r] &&
                    bitcount[can[c1][r]] == 2) {
                    for (n = 0; n < SIZE; n++) {
                      if (n == i) continue;
                      for (o = 0; o < SIZE; o++) {
                        R = n*SIZE + o;
                        if (can[c1][R] == can[c2][R] && ((can[c1][R] & can[c1][r]) == can[c1][r]) &&
                            bitcount[can[c1][R]] == 3) {
                            for (c = 0; c < LEN; c++)
                                if ((can[c1][r] ^ can[c1][R]) == (1 << c)) v = c;
                            for (p = 0; p < SIZE; p++)
                              for (q = 0; q < SIZE; q++)
                                if (p != o || (q != l && q != m))
                                  if (puzzle[j*SIZE+q][n*SIZE+p].value == UNKNOWN &&
                                      puzzle[j*SIZE+q][n*SIZE+p].possible[v]) {
                                        printf("column unique constraint: row: %d col: %d, value: %d\n",
                                          j*SIZE+q+1, n*SIZE+p+1, convert(v));
                                        puzzle[j*SIZE+q][n*SIZE+p].possible[v] = 0;
                                        find14++;
                                        count = 1;
                                  }
                        }
                    }
                  }
                }
            }

    return count;
}

struct pairset {
    int v1, v2;
    int val;
    int p1, p2; // pointer to previous
} puz[LEN][LEN], b1[LEN][LEN], b2[LEN][LEN];

void copypairset(struct pairset from[LEN][LEN], struct pairset to[LEN][LEN])
{
    int i, j;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            to[i][j] = from[i][j];
    }
}

void initpairset(struct pairset b[LEN][LEN])
{
    int i, j, k, count;

    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            count = 0;
            b[i][j].p1 = b[i][j].p2 = UNKNOWN;
            b[i][j].val = UNKNOWN;
            if (puzzle[i][j].value == UNKNOWN) {
                for (k = 0; k < LEN; k++) {
                  if (puzzle[i][j].possible[k])
                      if (count == 0) { b[i][j].v1 = k; count++; }
                  else
                      if (count == 1) { b[i][j].v2 = k; count++; }
                  else
                      count++;
                }
            }
            if (count != 2)
                b[i][j].v1 = b[i][j].v2 = UNKNOWN;
        }
}

void printback(struct pairset b[LEN][LEN], int i, int j)
{
    int i1, j1;
    int stack[100][3];
    int ptr = 0;

    stack[ptr][0] = i+1;
    stack[ptr][1] = j+1;
    stack[ptr++][2] = convert(b[i][j].val);
    while (1) {
        i1 = b[i][j].p1; j1 = b[i][j].p2;
        i = i1; j = j1;
        if (i == UNKNOWN) break;
        stack[ptr][0] = i+1;
        stack[ptr][1] = j+1;
        stack[ptr++][2] = convert(b[i][j].val);
    }
    ptr--;
    while (ptr >= 0) {
        printf("(%d %d)=%d", stack[ptr][0], stack[ptr][1], stack[ptr][2]);
        if (ptr != 0) printf(" =>\n"); else printf("\n");
        ptr--;
    }
}

int iterateforce(struct pairset b[LEN][LEN])
{
    int i, j, k, l, v, c1, c2;
    int found = 0;

    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            if ((v = b[i][j].val) == UNKNOWN) continue;
            for (k = 0; k < LEN; k++) {
                if (b[k][j].val == UNKNOWN && b[k][j].v1 == v) {
                    b[k][j].val = b[k][j].v2;
                    b[k][j].p1 = i; b[k][j].p2 = j;
                    found = 1;
                }
                if (b[k][j].val == UNKNOWN && b[k][j].v2 == v) {
                    b[k][j].val = b[k][j].v1;
                    b[k][j].p1 = i; b[k][j].p2 = j;
                    found = 1;
                }
                if (b[i][k].val == UNKNOWN && b[i][k].v1 == v) {
                    b[i][k].val = b[i][k].v2;
                    b[i][k].p1 = i; b[i][k].p2 = j;
                    found = 1;
                }
                if (b[i][k].val == UNKNOWN && b[i][k].v2 == v) {
                    b[i][k].val = b[i][k].v1;
                    b[i][k].p1 = i; b[i][k].p2 = j;
                    found = 1;
                }
                if (b[c1 = SIZE*(i/SIZE) + k/SIZE][c2 = SIZE*(j/SIZE)+(k%SIZE)].val == UNKNOWN &&
                    b[c1][c2].v1 == v) {
                    b[c1][c2].val = b[c1][c2].v2;
                    b[c1][c2].p1 = i;
                    b[c1][c2].p2 = j;
                    found = 1;
                }
                if (b[c1 = SIZE*(i/SIZE) + k/SIZE][c2 = SIZE*(j/SIZE)+(k%SIZE)].val == UNKNOWN &&
                    b[c1][c2].v2 == v) {
                    b[c1][c2].val = b[c1][c2].v1;
                    b[c1][c2].p1 = i;
                    b[c1][c2].p2 = j;
                    found = 1;
                }
            }
        }
    return found;
}

int forcingchains()
{
    int i, j, k, l, v;
    
    initpairset(puz);
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            if (puz[i][j].v1 == UNKNOWN) continue;
            // got a pair: make copies
            copypairset(puz, b1);
            b1[i][j].val = puz[i][j].v1;
            copypairset(puz, b2);
            b2[i][j].val = puz[i][j].v2;
            while (iterateforce(b1) || iterateforce(b2)) {
                // check for forced value
                for (k = 0; k < LEN; k++)
                    for (l = 0; l < LEN; l++)
                        if ((b1[k][l].val != UNKNOWN) && (b1[k][l].val == b2[k][l].val)) {
                            v = puzzle[k][l].value = b1[k][l].val;
                            if (slow) {
                                printf("forcing chain: row %d column %d takes %d\n",
                                    k+1, l+1, convert(v));
                                printback(b1, k, l);
                                printback(b2, k, l);
                            }
                            find15++;
                            return 1;
                        }
            }
        }
    // apparently we didn't find anything ...
    return 0;
}

void debugcolorarray(int can, int colors[LEN][LEN])
{
    int i, j;
    printf("Candidate: %d\n----------\n", can);
    for (i = 0; i < LEN; i++) {
        for (j = 0; j < LEN; j++)
            printf("%3d ", colors[i][j]);
        printf("\n");
    }
    printf("---------\n");
}

int fillexclusion(int line[LEN], int exclude[200][2], int excludecount)
{
    int i, j, k;
    int a, b, tmp, fail;
    
    for (i = 0; i < LEN-1; i++) {
        if (line[i] == 0) continue;
        for (j = i+1; j < LEN; j++) {
            if (line[j] == 0) continue;
            if ((line[i] == -line[j]) || (line[i] == line[j])) continue;
            a = line[i];
            b = line[j];
            if (a > b) { tmp = a; a = b; b = tmp; }
            fail = 0;
            for (k = 0; k < excludecount; k++)
                if (exclude[k][0] == a && exclude[k][1] == b) fail = 1;
            if (fail) continue;
            exclude[excludecount][0] = a;
            exclude[excludecount++][1] = b;
        }
    }
    return excludecount;
}

int addexclude(int exclude[200][2], int a, int b, int excludecount)
{
    int tmp, i, fail;
    
    if (a > b) { tmp = a; a = b; b = tmp; }
    fail = 0;
    for (i = 0; i < excludecount; i++)
        if (exclude[i][0] == a && exclude[i][1] == b) { fail = 1; break; }
    if (fail == 0) { exclude[excludecount][0] = a; exclude[excludecount++][1] = b; }
    return excludecount;
}

int colorcheck()
{
    int can, count;
    int i, j, k;
    int i1, j1;
    int freecolor;
    int colors[LEN][LEN];
    int save[LEN], save1[LEN], save2[LEN];
    int colorchange = 0;
    int multichange = 0;
    
    for (can = 0; can < LEN; can++) {
        freecolor = 1;
        for (i = 0; i < LEN; i++)
            for (j = 0; j < LEN; j++)
                colors[i][j] = 0;
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (k = 0; k < LEN; k++)
                if (puzzle[i][k].value == UNKNOWN &&
                    puzzle[i][k].possible[can]) save[count++] = k;
            if (count == 2) { // found a row-conjugate
                colors[i][save[0]] = freecolor;
                colors[i][save[1]] = -freecolor;
                freecolor++;
            }
        }
        for (i = 0; i < LEN; i++) {
            count = 0;
            for (k = 0; k < LEN; k++)
                if (puzzle[k][i].value == UNKNOWN &&
                    puzzle[k][i].possible[can]) save[count++] = k;
            if (count == 2) { // found a column-conjugate
                int a1, a2;
                a1 = colors[save[0]][i];
                a2 = colors[save[1]][i];
                if ( a1 && a2 ) {
                    for (i1 = 0; i1 < LEN; i1++)
                        for (j1 = 0; j1 < LEN; j1++) {
                            if (colors[i1][j1] == a1)
                                colors[i1][j1] = -a2;
                            if (colors[i1][j1] == -a1)
                                colors[i1][j1] = a2;
                        }
                } else if (colors[save[0]][i]== 0 && colors[save[1]][i] == 0) {
                    colors[save[0]][i] = freecolor;
                    colors[save[1]][i] = -freecolor;
                    freecolor++;
                } else if (colors[save[0]][i]) {
                    colors[save[1]][i] = -colors[save[0]][i];
                } else { // colors[i][save[1]] != 0
                    colors[save[0]][i] = - colors[save[1]][i];
                }
            }
        }
        for (i = 0; i < LEN; i += SIZE)
            for (j = 0; j < LEN; j += SIZE) {
                count = 0;
                for (i1 = 0; i1 < SIZE; i1++)
                    for (j1 = 0; j1 < SIZE; j1++)
                        if (puzzle[i+i1][j+j1].value == UNKNOWN &&
                            puzzle[i+i1][j+j1].possible[can]) {
                            save1[count] = i1;
                            save2[count++] = j1;
                        }
                if (count == 2) { // found a box-conjugate
                    int a1, a2;
                    a1 = colors[i+save1[0]][j+save2[0]];
                    a2 = colors[i+save1[1]][j+save2[1]];
                    if ( a1 && a2 ) {
                        for (i1 = 0; i1 < LEN; i1++)
                            for (j1 = 0; j1 < LEN; j1++) {
                                if (colors[i1][j1] == a1)
                                    colors[i1][j1] = -a2;
                                if (colors[i1][j1] == -a1)
                                    colors[i1][j1] = a2;
                            }
                   } else if (a1 == 0 && a2 == 0) {
                        colors[i+save1[0]][j+save2[0]] = freecolor;
                        colors[i+save1[1]][j+save2[1]] = -freecolor;
                        freecolor++;
                    } else if (colors[i+save1[0]][j+save2[0]]) {
                        colors[i+save1[1]][j+save2[1]] = -colors[i+save1[0]][j+save2[0]];
                    } else { // colors[i+save1[1]][j+save2[1]] != 0
                        colors[i+save1[0]][j+save2[0]] = -colors[i+save1[1]][j+save2[1]];
                    }
                }
            }
        // OK, now it's completely colored; let's look for stuff...
        freecolor--;
        // colors run from -freecolor to freecolor
        for (k = -freecolor; k <= freecolor; k++) {
            if (k == 0) continue;
            for (i = 0; i < LEN; i++) { // look in each row
                count = 0;
                for (j = 0; j < LEN; j++)
                    if (colors[i][j] == k) count++;
                if (count > 1) {
                    for (i1 = 0; i1 < LEN; i1++)
                      for (j = 0; j < LEN; j++)
                        if (colors[i1][j] == k) {
                            puzzle[i1][j].possible[can] = 0;
                            if (slow) printf("coloring: candidate %d"
                                " impossible at row %d, column %d\n",
                                convert(can), i1+1, j+1);
                            colorchange++;
                        }
                 }
            }
            for (i = 0; i < LEN; i++) { // look in each column
                count = 0;
                for (j = 0; j < LEN; j++)
                    if (colors[j][i] == k) count++;
                if (count > 1)
                    for (i1 = 0; i1 < LEN; i1++)
                      for (j = 0; j < LEN; j++)
                        if (colors[j][i1] == k) {
                            puzzle[j][i1].possible[can] = 0;
                            if (slow) printf("coloring: candidate %d"
                                " impossible at row %d, column %d\n",
                                convert(can), j+1, i1+1);
                            colorchange++;
                        }
            }
            for (i = 0; i < LEN; i += SIZE)
                for (j = 0; j < LEN; j += SIZE) {
                    count = 0;
                    for (i1 = 0; i1 < SIZE; i1++)
                        for (j1 = 0; j1 < SIZE; j1++)
                            if (colors[i+i1][j+j1] == k) count++;
                    if (count > 1)
                        for (i1 = 0; i1 < LEN; i1++)
                            for (j1 = 0; j1 < LEN; j1++)
                                if (colors[i1][j1] == k) {
                                    puzzle[i1][j1].possible[can] = 0;
                                    if (slow) printf("coloring: candidate %d"
                                        " impossible at row %d, column %d\n",
                                        convert(can), i1+1, j1+1);
                                    colorchange++;
                                }
                }
                
        }
        for (k = 1; k <= freecolor; k++) {
            int plus, minus;
            // check for k and -k in same row, column, block
            for (i = 0; i < LEN; i++) {
                plus = minus = 0;
                for (j = 0; j < LEN; j++) {
                    if (colors[i][j] == k) plus = j;
                    if (colors[i][j] == -k) minus = j;
                }
                if (plus && minus)
                    for (j = 0; j < LEN; j++) {
                        if (j != plus && j != minus)
                          if (puzzle[i][j].value == UNKNOWN &&
                              puzzle[i][j].possible[can]) {
                            puzzle[i][j].possible[can] = 0;
                            if (slow) printf("coloringx: candidate %d"
                                " impossible at row %d, column %d\n",
                                convert(can), i+1, j+1);
                            colorchange++;
                            }
                    }
             }
            for (i = 0; i < LEN; i++) {
                plus = minus = 0;
                for (j = 0; j < LEN; j++) {
                    if (colors[j][i] == k) plus = j;
                    if (colors[j][i] == -k) minus = j;
                }
                if (plus && minus)
                    for (j = 0; j < LEN; j++) {
                        if (j != plus && j != minus)
                          if (puzzle[j][i].value == UNKNOWN &&
                              puzzle[j][i].possible[can]) {
                            puzzle[j][i].possible[can] = 0;
                            if (slow) printf("coloringy: candidate %d"
                                " impossible at row %d, column %d\n",
                                convert(can), j+1, i+1);
                            colorchange++;
                            }
                    }
             }
             for (i = 0; i < LEN; i += SIZE)
               for (j = 0; j < LEN; j += SIZE) {
                 plus = minus = 0;
                 for (i1 = 0; i1 < SIZE; i1++)
                   for (j1 = 0; j1 < SIZE; j1++) {
                       if (colors[i+i1][j+j1] == k) plus = 3*i1 + j1;
                       if (colors[i+i1][j+j1] == -k) minus = 3*i1 + j1;
                   }
                   if (plus && minus)
                       for (i1 = 0; i1 < LEN; i1++) {
                         if (i1 != plus && i1 != minus)
                           if (puzzle[i + i1/3][j + (i1%3)].value == UNKNOWN &&
                               puzzle[i + i1/3][j + (i1%3)].possible[can]) {
                                 puzzle[i + i1/3][j + (i1%3)].possible[can] = 0;
                                    if (slow) printf("coloringz: candidate %d"
                                        " impossible at row %d, column %d\n",
                                        convert(can), i + i1/3 + 1, j + (i1%3) +1);
                                    colorchange++;
                            }
                       }
               }
               for (i = 0; i < LEN; i++)
                 for (j = 0; j < LEN; j++) {
                     if (colors[i][j] != k) continue;
                     for (i1 = 0; i1 < LEN; i1++)
                       for (j1 = 0; j1 < LEN; j1++) {
                         if (colors[i1][j1] != -k) continue;
                         if (i == i1 || j == j1) continue;
                         if (puzzle[i][j1].value == UNKNOWN &&
                             puzzle[i][j1].possible[can]) {
                                puzzle[i][j1].possible[can] = 0;
                                if (slow) printf("coloringq: candidate %d"
                                    " impossible at row %d, column %d\n",
                                    convert(can), i + 1, j1 + 1);
                                colorchange++;
                         }
                         if (puzzle[i1][j].value == UNKNOWN &&
                             puzzle[i1][j].possible[can]) {
                                puzzle[i1][j].possible[can] = 0;
                                if (slow) printf("coloringq: candidate %d"
                                    " impossible at row %d, column %d\n",
                                    convert(can), i1 + 1, j + 1);
                                colorchange++;
                         }
                       }
                 }
        }
    
        // We can use the same color data to check on the multicolor situation.
        
        // First, we need to make an exclusion table:
        
        int exclude[200][2];
        int excludecount, excludesave;
        int colorlist[200][2]; // 0 => pos; 1 => neg
        int colorlistcount;
        int line[LEN];
        int a, b, newcount, i2, j2, i3, j3;
        
        // Normalize colors
        
        excludecount = colorlistcount = 0;
    
        // Fill in exclusions:
        
        for (i = 0; i < LEN; i++) {
            for (j = 0; j < LEN; j++)
                line[j] = colors[i][j];
            excludecount = fillexclusion(line, exclude, excludecount);
        }
        for (i = 0; i < LEN; i++) {
            for (j = 0; j < LEN; j++)
                line[j] = colors[j][i];
            excludecount = fillexclusion(line, exclude, excludecount);
        }
        for (i = 0; i < LEN; i += SIZE)
            for (j = 0; j < LEN; j += SIZE) {
                for (k = 0; k < LEN; k++)
                    line[k] = colors[i + k/3][j + (k%3)];
                excludecount = fillexclusion(line, exclude, excludecount);
            }
        
        // Now complete the transitive closure of the exclusion list:
        
        do {
            excludesave = excludecount;
            for (i = 0; i < excludecount; i++) {
                a = exclude[i][0];
                b = exclude[i][1];
                for (j = 0; j < excludecount; j++) {
                    if (a == -exclude[j][0])
                        excludecount = addexclude(exclude, b, exclude[j][1], excludecount);
                    if (a == -exclude[j][1])
                        excludecount = addexclude(exclude, b, exclude[j][0], excludecount);
                    if (b == -exclude[j][0])
                        excludecount = addexclude(exclude, a, exclude[j][1], excludecount);
                    if (b == -exclude[j][1])
                        excludecount = addexclude(exclude, a, exclude[j][0], excludecount);
                }
            }
        } while (excludesave != excludecount);
        
        //if (excludecount > 6) printf("Exclude count: %d\n", excludecount);
        
        // Now, for every pair a!b in the exclude list, we need to find
        // instances at the intersection of A and B in the table and eliminate them
        if (0 && excludecount >= 2) {
            debugcolorarray(can, colors);
            for (i = 0; i < excludecount; i++)
                printf("%d excludes %d\n", exclude[i][0], exclude[i][1]);
            printf("excludecount: %d\n", excludecount);
        }
        
        // Look for implications of the exclude list.
        
        for (i = 0; i < excludecount; i++) {
            if (exclude[i][0] == -exclude[i][1]) continue;
            if (exclude[i][0] == exclude[i][1]) { // number excludes itself!
                for (i1 = 0; i1 < LEN; i1++)
                    for (j1 = 0; j1 < LEN; j1++) {
                        if (colors[i1][j1] == -exclude[i][0]) {
                            puzzle[i1][j1].value = can;
                            if (slow) printf("multicolor: %d at row %d column %d\n",
                                convert(can), i1+1, j1+1);
                            multichange++;
                        }
                        if (colors[i1][j1] == exclude[i][0])
                            puzzle[i1][j1].possible[can] = 0; // for good measure...
                    }
            } else { // different exclude values
                for (i1 = 0; i1 < LEN; i1++)
                    for (j1 = 0; j1 < LEN; j1++) {
                        if (colors[i1][j1] != -exclude[i][0]) continue;
                        for (i2 = 0; i2 < LEN; i2++)
                            for (j2 = 0; j2 < LEN; j2++) {
                                if (i1 == i2 || j1 == j2) continue;
                                if (colors[i2][j2] != -exclude[i][1]) continue;
                                if (puzzle[i1][j2].value == UNKNOWN &&
                                    puzzle[i1][j2].possible[can]) {
                                        puzzle[i1][j2].possible[can] = 0;
                                        if (slow) printf("multicolor: %d impossible "
                                        "in row %d column %d\n", convert(can),
                                        i1+1, j2+1);
                                        multichange++;
                                }
                                if (puzzle[i2][j1].value == UNKNOWN &&
                                    puzzle[i2][j1].possible[can]) {
                                        puzzle[i2][j1].possible[can] = 0;
                                        if (slow) printf("multicolor: %d impossible "
                                        "in row %d column %d\n", convert(can),
                                        i2+1, j1+1);
                                        multichange++;
                                }
                                if ((i1/3 == i2/3) && (j1/3 == j2/3)) { // in same block
                                    for (i3 = 0; i3 < SIZE; i3++)
                                        for (j3 = 0; j3 < SIZE; j3++) {
                                             a = 3*(i1/3) + i3;
                                             b = 3*(j1/3) + j3;
                                             if (((a == i1) &&
                                                 (b == j1)) ||
                                                 ((a == i2) && (b == j2))) continue;
                                             if (puzzle[a][b].value == UNKNOWN &&
                                                 puzzle[a][b].possible[can]) {
                                                     puzzle[a][b].possible[can] = 0;
                                                     if (slow) printf("multicolor: %d impossible "
                                                     "in row %d column %d\n", convert(can),
                                                     a+1, b+1);
                                                     multichange++;
                                             }                                                 
                                         }
                                 }
                                 if ((i1/3) == (i2/3)) {
                                     for (j3 = 0; j3 < SIZE; j3++) {
                                         a = i1; b = 3*(j2/3) + j3;
                                         if (puzzle[a][b].value == UNKNOWN &&
                                             puzzle[a][b].possible[can] &&
                                             colors[a][b] == 0) {
                                                 puzzle[a][b].possible[can] = 0;
                                                 if (slow) printf("multicolora: %d impossible "
                                                 "in row %d column %d\n", convert(can),
                                                 a+1, b+1);
                                                 multichange++;
                                         }                                                 
                                     }
                                 }
                                 if ((j1/3) == (j2/3)) {
                                     for (i3 = 0; i3 < SIZE; i3++) {
                                         a = 3*(i2/3) + i3; b = j1;
                                         if (puzzle[a][b].value == UNKNOWN &&
                                             puzzle[a][b].possible[can] &&
                                             colors[a][b] == 0) {
                                                 puzzle[a][b].possible[can] = 0;
                                                 if (slow) printf("multicolorb: %d impossible "
                                                 "in row %d column %d\n", convert(can),
                                                 a+1, b+1);
                                                 multichange++;
                                         }                                                 
                                     }
                                 }
                                 if ((i1/3) == (i2/3)) {
                                     for (j3 = 0; j3 < SIZE; j3++) {
                                         a = i2; b = 3*(j1/3) + j3;
                                         if (puzzle[a][b].value == UNKNOWN &&
                                             puzzle[a][b].possible[can] &&
                                             colors[a][b] == 0) {
                                                 puzzle[a][b].possible[can] = 0;
                                                 if (slow) printf("multicolorc: %d impossible "
                                                 "in row %d column %d\n", convert(can),
                                                 a+1, b+1);
                                                 multichange++;
                                         }                                                 
                                     }
                                 }
                                 if ((j1/3) == (j2/3)) {
                                     for (i3 = 0; i3 < SIZE; i3++) {
                                         a = 3*(i1/3) + i3; b = j2;
                                         if (puzzle[a][b].value == UNKNOWN &&
                                             puzzle[a][b].possible[can] &&
                                             colors[a][b] == 0) {
                                                 puzzle[a][b].possible[can] = 0;
                                                 if (slow) printf("multicolord: %d impossible "
                                                 "in row %d column %d\n", convert(can),
                                                 a+1, b+1);
                                                 multichange++;
                                         }                                                 
                                     }
                                 }
                            }
                    }
            }
        }
        
    }
    find11 += colorchange;
    find12 += multichange;
    if (colorchange || multichange) return 1;
    return 0;
}

void printpuzzle()
{
    int i, j, v;
    
    if (SIZE == 3) {
        printf("+---+---+---+\n");
        for (i = 0; i < LEN; i++) {
        printf("|");
            for (j = 0; j < LEN; j++) {
                v = puzzle[i][j].value;
                if (v == 0) v = 9;
                if (v == UNKNOWN) printf(" ");
                else printf("%c", '0' + v);
                if(j % SIZE == 2) printf("|");
            }
            if (i % SIZE == 2)
                printf("\n+---+---+---+\n");
            else
                printf("\n");
        }
    }
    if (SIZE == 4) {
        printf("+----+----+----+----+\n");
        for (i = 0; i < LEN; i++) {
        printf("|");
            for (j = 0; j < LEN; j++) {
                v = puzzle[i][j].value;
                if (v == UNKNOWN) printf(" ");
                else {
                    if (v <= 9)
                        printf("%c", '0' + v);
                    else
                        printf("%c", 'A' + v - 10);
                }
                if(j % SIZE == 3) printf("|");
            }
            if (i % SIZE == 3)
                printf("\n+----+----+----+----+\n");
            else
                printf("\n");
        }
    }
}

int countpossibles();

struct square puzzlestack[100][LEN][LEN];
int guessstack[100][3];
int stackpointer = 0;

void pushpuzzle(int ival, int jval, int guess)
{
    int i, j;
    
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            puzzlestack[stackpointer][i][j] = puzzle[i][j];
    guessstack[stackpointer][0] = ival;
    guessstack[stackpointer][1] = jval;
    guessstack[stackpointer][2] = guess;
    push[stackpointer]++;
    //printf("push: %d: %d\n", stackpointer, countpossibles());
    stackpointer++;
}

int poppuzzle()
{
    int i, j, ival, jval, guess;
    
    stackpointer--;
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            puzzle[i][j] = puzzlestack[stackpointer][i][j];
    ival = guessstack[stackpointer][0];
    jval = guessstack[stackpointer][1];
    guess = guessstack[stackpointer][2];
    //if (puzzle[ival][jval].possible[guess] == 0)
    //    printf("poppuzzle: fuckup!\n");
    puzzle[ival][jval].possible[guess] = 0;
    //printf("pop: %d: %d\n", stackpointer, countpossibles());
}

int makeguess()
{
    int i, j, k;
    int ibest, jbest, count = 0, countbest, ksave, kbest = 0;

    ibest = jbest = ksave = -1;
    countbest = 1000;
    resetpossible();
    // first, find the best slot:
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            if (puzzle[i][j].value == UNKNOWN) {
                count = 0;
                for (k = 0; k < LEN; k++) {
                    if (puzzle[i][j].possible[k]) {
                        count++;
                        ksave = k;
                    }
                }
                if (count == 1) {
                    printf("makeguess: gross fuckup!!!\n");
                    exit(1);
                }
                if (count > 0 && count < countbest) {
                    countbest = count;
                    ibest = i;
                    jbest = j;
                    kbest = ksave;
                }
            }
    if ((countbest == 1000 || count == 0) && stackpointer <= 0) // impossible to solve
        return 0;
    
    pushpuzzle(ibest, jbest, kbest);
    puzzle[ibest][jbest].value = kbest;
    return 1;
#ifdef CRUDEGUESS
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            if (puzzle[i][j].value == UNKNOWN)
                for (k = 0; k < LEN; k++)
                    if (puzzle[i][j].possible[k]) {
                        pushpuzzle(i, j, k);
                        puzzle[i][j].value = k;
                        return 1;
                    }
     return 0; // puzzle impossible to solve
 #endif
}

void dumpepshead(FILE *f)
{
    fprintf(f, "%%!PS-Adobe-2.0 EPSF-1.2\n");
    fprintf(f, "%%%%Creator: Sudoku\n");
    fprintf(f, "%%%%BoundingBox: 0 0 1000 1000\n");
    fprintf(f, "%%%%EndComments\n");
    fprintf(f, "\n");
    fprintf(f, "%% (5) 4 5 shownum shows a '5' in row 4, column 5\n");
    fprintf(f, "\n");
    fprintf(f, "/shownum {\n");
    fprintf(f, "1 sub exch 1 sub exch\n");
    fprintf(f, "100 mul 30 add exch\n");
    fprintf(f, "8 exch sub 100 mul 23 add\n");
    fprintf(f, "moveto\n");
    fprintf(f, "/Times-Roman findfont 80 scalefont setfont\n");
    fprintf(f, "show\n");
    fprintf(f, "} bind def\n");
    fprintf(f, "\n");
    fprintf(f, "%% 3 5 showring emphasizes (3, 5)\n");
    fprintf(f, "/showring {\n");
    fprintf(f, "newpath\n");
    fprintf(f, "100 mul 50 sub exch\n");
    fprintf(f, "8 exch sub 100 mul 150 add\n");
    fprintf(f, "60 0 360 arc stroke\n");
    fprintf(f, "} bind def\n");
    fprintf(f, "\n");
    fprintf(f, "%% (1278) 3 6 shows a possible grid at (3, 6)\n");
    fprintf(f, "\n");
    fprintf(f, "/showgrid {\n");
    fprintf(f, "/Times-Roman findfont 30 scalefont setfont\n");
    fprintf(f, "1 sub exch 1 sub exch\n");
    fprintf(f, "100 mul exch\n");
    fprintf(f, "8 exch sub 100 mul\n");
    fprintf(f, "gsave\n");
    fprintf(f, "translate\n");
    fprintf(f, "-3 -5 translate\n");
    fprintf(f, "dup (1) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "15 75 moveto (1) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (2) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "45 75 moveto (2) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (3) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "75 75 moveto (3) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (4) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "15 45 moveto (4) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (5) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "45 45 moveto (5) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (6) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "75 45 moveto (6) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (7) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "15 15 moveto (7) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (8) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "45 15 moveto (8) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "dup (9) search\n");
    fprintf(f, "{pop pop pop\n");
    fprintf(f, "75 15 moveto (9) show}\n");
    fprintf(f, "{pop} ifelse\n");
    fprintf(f, "pop\n");
    fprintf(f, "grestore\n");
    fprintf(f, "} bind def\n");
    fprintf(f, "\n");
    fprintf(f, "75 25 translate\n");
    fprintf(f, "1 setlinecap\n");
    fprintf(f, "1 setlinejoin\n");
    fprintf(f, "\n");
    fprintf(f, "%% draw the grid:\n");
    fprintf(f, "\n");
    fprintf(f, "0 100 900 {\n");
    fprintf(f, "dup dup dup\n");
    fprintf(f, "dup 100 idiv 3 mod 0 eq\n");
    fprintf(f, "newpath\n");
    fprintf(f, "{ 3 setlinewidth} {1 setlinewidth} ifelse\n");
    fprintf(f, "0 moveto 900 lineto stroke\n");
    fprintf(f, "newpath\n");
    fprintf(f, "0 exch moveto 900 exch lineto stroke\n");
    fprintf(f, "} for\n");
    fprintf(f, "\n");
    fprintf(f, "%% label the grid\n");
    fprintf(f, "\n");
    fprintf(f, "/Times-Italic findfont 50 scalefont setfont\n");
    fprintf(f, "-50 35 moveto (i) show\n");
    fprintf(f, "-50 135 moveto (h) show\n");
    fprintf(f, "-50 235 moveto (g) show\n");
    fprintf(f, "-50 335 moveto (f) show\n");
    fprintf(f, "-50 435 moveto (e) show\n");
    fprintf(f, "-50 535 moveto (d) show\n");
    fprintf(f, "-50 635 moveto (c) show\n");
    fprintf(f, "-50 735 moveto (b) show\n");
    fprintf(f, "-50 835 moveto (a) show\n");
    fprintf(f, "35 925 moveto (1) show\n");
    fprintf(f, "135 925 moveto (2) show\n");
    fprintf(f, "235 925 moveto (3) show\n");
    fprintf(f, "335 925 moveto (4) show\n");
    fprintf(f, "435 925 moveto (5) show\n");
    fprintf(f, "535 925 moveto (6) show\n");
    fprintf(f, "635 925 moveto (7) show\n");
    fprintf(f, "735 925 moveto (8) show\n");
    fprintf(f, "835 925 moveto (9) show\n");
    fprintf(f, "\n");
}

void dumpepsvalue(FILE *f, int row, int col, int value)
{
    fprintf(f, "(%d) %d %d shownum\n", convert(value), row, col);
}

void dumpepspossible(FILE *f, int row, int col, int *possible)
{
    int i;
    fprintf(f, "(");
    for (i = 0; i < 9; i++)
        if (possible[i] && digitfilter[i]) fprintf(f, "%d", convert(i));
    fprintf(f, ") %d %d showgrid\n", row, col);
}

void dumpepstail(FILE *f)
{
    fprintf(f, "showpage\n");
    fprintf(f, "\n");
}

void dumpepspuzzle()
{
    FILE *f;
    int i, j;

    if (slow == 0) return;
    if (SIZE == 4) return; // Later... much later...
    f = fopen("puz.eps", "w");
    dumpepshead(f);
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++) {
            if (puzzle[i][j].value != UNKNOWN)
                dumpepsvalue(f, i+1, j+1, puzzle[i][j].value);
            else
                dumpepspossible(f, i+1, j+1, &(puzzle[i][j].possible[0]));
        }
    dumpepstail(f);
    fclose(f);
}

void debugdump()
{
    int i, j, k;
    for (i = 0; i < LEN; i++) {
        for (j = 0; j < LEN; j++)
            if (puzzle[i][j].value != UNKNOWN)
                printf("[[[[%d]]]] ", puzzle[i][j].value);
            else {
                for (k=0; k < LEN; k++)
                    if (puzzle[i][j].possible[k]) printf("%d", k);
                    else printf(".");
                printf(" ");
            }
        printf("\n");
    }
}

int countpossibles()
{
    int i, j, k;
    int count = 0;

    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            for (k = 0; k < LEN; k++)
                if (puzzle[i][j].possible[k]) count++;
    return count;
}

struct square savepuzzle[LEN][LEN];

// checkpuzzle returns 0 if the puzzle's OK; 1 if at
// least one solution exists

int checkpuzzle()
{
    int i, j, saveslow, issolved=0;
    
    // save the original puzzle
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            savepuzzle[i][j] = puzzle[i][j];
    saveslow = slow;
    slow = 0;

    resetpossible();
    while (issolved == 0) {
        if (solved())
            issolved = 1;
        else {
            if (0 == solvable() && stackpointer) {
                poppuzzle();
            }
            resetpossible();
            if (findobvious()) {
            } else { // need to use recursion
                if (0 == makeguess()) {
                for (i = 0; i < LEN; i++)
                    for (j = 0; j < LEN; j++)
                        puzzle[i][j] = savepuzzle[i][j];
                    slow = saveslow;
                    return 1;
                }
            }
        }
    }    

    // restore the original puzzle
    for (i = 0; i < LEN; i++)
        for (j = 0; j < LEN; j++)
            puzzle[i][j] = savepuzzle[i][j];
    slow = saveslow;
    stackpointer = 0;
    resetpossible();
    initsearchvalues();
    return 0;
}

void printhelp()
{
    printf("-f: filter: filter lists the series of digits\n"
           "    to be printed in the dumped eps files.  For\n"
           "    now they are 1-9.  A 0 (zero) prints none.\n");
    printf("-h: help: print this message\n");
    printf("-j: jumble: jumble the given puzzle to look different\n");
    printf("-J: jumble, but using the seed given as a parameter\n");
    printf("-x: jumble, but retain symmetry\n");
    printf("-s: step mode; type <return> for each step\n");
}

int symmetric = 0;

int parseoptions(int argc, char **argv)
{
    int i = 1, j;

    if (argc == 1) {
        fprintf(stderr, "Usage: %s [-hjsx] [-f filter] [-J value] <filename> "
            "(%s -h for help)\n", argv[0], argv[0]);
        exit(1);
    }
    do {
        if (argv[i][0] == '-') { // read option(s)
            char *p = &(argv[i][1]), *q;
            while (*p) {
                switch (*p) {
                    case 's': slow = 1; break;
                    case 'h': printhelp(); exit(0); break;
                    case 'j': jum = 1; break;
                    case 'x': symmetric = jum = 1; break;
                    case 'f':
                        i++;
                        q = &(argv[i][0]);
                        for (j = 0; j < LEN; j++)
                            digitfilter[j] = 0;
                        while (*q) {
                            if ('1' <= *q && *q <= '9') {
                                if (*q == '9') *q = '0';
                                digitfilter[*q - '0'] = 1;
                            }
                            q++;
                        }
                        break;
                    case 'J':
                        i++;
                        jum = 1;
                        randseed = atoi(argv[i]);
                        break;
                }
                p++;
            }
        } else {
            sudokufile = argv[i];
        }
        i++;
    } while (i < argc);
}

int main(int argc, char **argv)
{
    FILE *f;
    int i;
    char cmd[1000];

    slow = jum = 0;
    for (i = 0; i < LEN; i++) digitfilter[i] = 1;
    parseoptions(argc, argv);
    if (sudokufile == 0) {
        fprintf(stderr, "Missing file name");
        exit(1);
    }
    f = fopen(sudokufile, "r");
    if (f == 0) {
        fprintf(stderr, "Couldn't open %s\n", sudokufile);
        exit(1);
    }
    if (slow)
        printf("Type <return> to see each step\nin the solution.\n\n");
    if (0 == readpuzzle(f)) {
        fprintf(stderr, "Invalid puzzle: no solution\n");
        exit(1);
    }
    fclose(f);
    if (jum) jumble(symmetric);
    printf("Initial puzzle (%s):\n\n", sudokufile);
    if (slow == 0) printpuzzle();
    if (checkpuzzle() == 1) {
        printpuzzle();
        if (slow) dumpepspuzzle();
        printf("no solution\n");
        exit(1);
    }
    resetpossible();
    if (slow) dumpepspuzzle();
    while (1) {
        if (solved()) {
            if (scount == 1) {
                printf("There are multiple solutions.  Here's another:\n");
                printpuzzle();
                return;
            }
            printf("Solved!\n");
            if (scount == 0) printpuzzle();
            //printf("slow: %d\n", slow);
            if (slow) dumpepspuzzle();
            scount++;
            printstats();
            printf("--------------------------------\n");
            if (stackpointer) {
                poppuzzle();
            } else
                return;
        }
        if (0 == solvable()) {
            if (slow == 1 && scount == 0) printf("Backtrack!\n");
            if (stackpointer)
                poppuzzle();
        }
        resetpossible();
        if (slow) dumpepspuzzle();
        if (slow) {
            if (scount == 0) printpuzzle();
            if (scount == 0) {
                char *p;
                fgets(cmd, 999, stdin);
                p = cmd;
                if (*p != '\n') {
                    int row, col, val;
                    row = *p++ - '0' - 1;
                    col = *p++ - '0' - 1;
                    val = *p - '0';
                    if (val == 9) val = 0;
                    puzzle[row][col].value = val;
                    continue;
                }
            }
        }
        if (findobvious()) {
        } else if (lockedcandidates()) {
        } else if (findnaked()) {
        } else if (findhiddenpair()) {
        } else if (forcingchains()) {
        } else if (xwing()) {
        } else if (sw3()) {
        } else if (sw4()) {
        } else if (xywing()) {
        } else if (colorcheck()) {
        } else if (doublepair()) {
        } else { // need to use recursion
            if (0 == makeguess()) {
                printstats();
                if (scount == 0) printf("Puzzle can't be solved\n");
                exit(0);
            }
            if (slow == 1) {
                if (scount == 0)
                    printf("Make a guess: row: %d column: %d value: %d\n",
                        guessstack[stackpointer-1][0]+1,
                        guessstack[stackpointer-1][1]+1,
                        convert(guessstack[stackpointer-1][2]));
            }
        }
    }
}
