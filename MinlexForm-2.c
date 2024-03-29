/*******************************************************************************
                           Disclaimer and License
********************************************************************************
This software is public domain.  No explicit or implicit warranty is given.
If you use this code or ideas herein in any substantial way, a credit would
be appreciated.  Suggested improvements and bug reports are also welcome.

Language:   'C'
Author:     Michael Deverin (devfam47@gmail.com)
Date:       August xx, 2011

********************************************************************************
                             Primary Routine
********************************************************************************
This software converts a standard Soduko puzzle (completed) to Minlex Form
(the smallest 81-digit number after rows are concatenated). 

         void MinlexForm_FullGrid (int puzzle[81],int stdform[81])

All 81 cells must contain a clue in the range 1-9.  As usual, there must be
no repeated clues within a row, column, or 3x3 box.

********************************************************************************
				Time Tests
********************************************************************************
Computer: Dell Studio 540, Intel Core, 2 Quad CPU, Q8200 @ 2.33 GHz.
Conversion Rate:  about 50,000 puzzles per second.

*******************************************************************************/
#include <stdio.h>
#define DEBUG	0

# if (DEBUG)
extern FILE *fp;
# endif

/*******************************************************************************
			 Structure for Two-Row Data
Example:
  Rank = 5 (0-base)	   Cycle= (1 4)(2 5 8)(3 7)(6 9)   1-base
  Minlex Form:		   Cycle= (0 3)(1 4 7)(2 6)(5 8)   0-base  
    1 2 3  4 5 6  7 8 9	   Cycle= (1 4 7)(0 3)(2 6)(5 8)   re-ordered
    4 5 7  1 8 9  3 2 6
*******************************************************************************/
typedef struct {
  int minlex[9];	// Two-row MinLex Form (given rank)
  int count;		// Number of two-row minimums
  int nc;		// Number of cycles
  int len[4];		// Cycle lengths
  int cumlen[5];	// Cumulative cycle lengths
  int pos[9];		// Cycle positions
} TWOROWDATA;

TWOROWDATA trdata[15] = {
  { {4,5,6,7,8,9,1,2,3},18, 3, {3,3,3,0}, {0,3,6,9,0}, {0,3,6,1,4,7,2,5,8} }, //  0
  { {4,5,6,7,8,9,1,3,2}, 6, 2, {6,3,0,0}, {0,6,9,0,0}, {1,4,7,2,5,8,0,3,6} }, //  1
  { {4,5,6,7,8,9,2,3,1}, 9, 1, {9,0,0,0}, {0,9,0,0,0}, {0,3,6,1,4,7,2,5,8} }, //  2
  { {4,5,7,1,8,9,2,3,6}, 1, 3, {5,2,2,0}, {0,5,7,9,0}, {1,4,7,2,6,0,3,5,8} }, //  3
  { {4,5,7,1,8,9,2,6,3}, 1, 2, {7,2,0,0}, {0,7,9,0,0}, {1,4,7,5,8,2,6,0,3} }, //  4
  { {4,5,7,1,8,9,3,2,6}, 3, 4, {3,2,2,2}, {0,3,5,7,9}, {1,4,7,0,3,2,6,5,8} }, //  5
  { {4,5,7,1,8,9,6,2,3}, 1, 3, {3,4,2,0}, {0,3,7,9,0}, {1,4,7,2,6,5,8,0,3} }, //  6
  { {4,5,7,1,8,9,6,3,2}, 1, 2, {7,2,0,0}, {0,7,9,0,0}, {1,4,7,2,6,5,8,0,3} }, //  7
  { {4,5,7,2,8,9,1,6,3}, 3, 1, {9,0,0,0}, {0,9,0,0,0}, {0,3,1,4,7,5,8,2,6} }, //  8
  { {4,5,7,2,8,9,6,1,3}, 1, 2, {5,4,0,0}, {0,5,9,0,0}, {0,3,1,4,7,2,6,5,8} }, //  9
  { {4,5,7,2,8,9,6,3,1}, 1, 1, {9,0,0,0}, {0,9,0,0,0}, {0,3,1,4,7,2,6,5,8} }, // 10
  { {4,5,7,3,8,9,6,1,2}, 1, 1, {9,0,0,0}, {0,9,0,0,0}, {0,3,2,6,5,8,1,4,7} }, // 11
  { {4,5,7,3,8,9,6,2,1}, 1, 2, {6,3,0,0}, {0,6,9,0,0}, {0,3,2,6,5,8,1,4,7} }, // 12
  { {4,5,7,8,9,3,6,1,2}, 6, 3, {3,3,3,0}, {0,3,6,9,0}, {0,3,7,1,4,8,2,6,5} }, // 13
  { {4,5,7,8,9,3,6,2,1}, 6, 2, {6,3,0,0}, {0,6,9,0,0}, {0,3,7,1,4,8,2,6,5} }  // 14
};

/*******************************************************************************
				Full-Box Answer
*******************************************************************************/
const int MAXANS=648;
typedef struct {
  int type;		// Puzzle or transpose (0 or 1)
  int symmap[10];	// Symbol map
  int colkey[ 9];	// Column key
  int rowkey[ 9];	// Row key
} FBANS;

/*******************************************************************************
		              Global Variables
*******************************************************************************/
int Lobit[1024];	// Compute_Lobit()
int *TwoPuz[2];		// MinlexForm_Fullgrid()
int Transpose[81];	// MinlexForm_Fullgrid()

/*******************************************************************************
			  MinlexForm_FullGrid()
*******************************************************************************/
void MinlexForm_FullGrid (int puzzle[81],int stdform[81])
{
  // Functions
  void Compute_Lobit(int lobit[],int nb);
  void Compute_Transpose(int puzzle[81],int transpose[81]);
  int TwoRowPrescore(int story[18][3],int *storykt);
  int FinishBand(int rank,int story[18][3],int storykt,FBANS *ans);
  void FinishGrid(FBANS *fbans,int anscnt,int stdform[81]);

  // Declarations
  FBANS fbans[MAXANS];
  int cand[18][3], candcnt, rank, anscnt;
  static int onetime = 1;

  // Perform onetime computations
  if (onetime)
  { onetime=0;
    Compute_Lobit(Lobit,10);
  }

  // Set-up pointers to puzzle & transpose. 
  TwoPuz[0] = puzzle;
  TwoPuz[1] = Transpose;
  Compute_Transpose(puzzle,Transpose);

  // Get list of candidates for row-1 and row-2.
  rank = TwoRowPrescore(cand,&candcnt);

  // Add third row & score. Make another answer list. 
  anscnt = FinishBand(rank,cand,candcnt,fbans);

  // Add remaining six rows.  Produce final answer.
  FinishGrid(fbans,anscnt,stdform);

} // End-of-routine

/*******************************************************************************
                                Compute_Lobit()
*******************************************************************************/
void Compute_Lobit (int lobit[], int nb)
{
  // Declarations & initializations
  int i, j, n=2;
  lobit[0] = -1;		// No '1' bits
  lobit[1] =  0;

  // Recursively record the bit position of the first '1'
  // (right-to-left) for each integer.
  for ( i=1; i < nb; i++ )
  {
    lobit[n++] = i;
    for ( j=1; j < (1<<i); j++ ) lobit[n++] = lobit[j];
  }

} // End-of-function

/*******************************************************************************
                             Compute_Transpose()
*******************************************************************************/
void Compute_Transpose (int puzzle[81], int transpose[81])
{
  int i, j, k;
  for ( k=i=0; i < 9; i++ )
      for ( j=0; j < 81; j += 9 )
	  { transpose[i+j] = puzzle[k++]; }
}

/*******************************************************************************
				TwoRowPrescore()
Purpose:
  Ranks all pairs of rows withing the same band in the puzzle and its
  transpose; saves information about minimum ranking pairs.
Return:
  Returns rank [0,14] of best-scoring row pairs.
*******************************************************************************/
int TwoRowPrescore (int story[18][3], int *storykt)
{
  // Functions
  int TwoRowRanker(int r1[9],int r2[9]);

  // Declarations
  int pair[4] = {0,1,2,0};
  int *pp, *bp, *rp, *r1, *r2;
  int minrank=999, rank, type, band, kt;

  // Loop over puzzle and its transpose
  for ( kt=type=0; type < 2; type++ )
  {
    // Loop over bands
    pp = TwoPuz[type];				// Puzzle ptr
    for ( band=0; band < 3; band++ )
    {
      // Loop over pairs of rows
      bp = pp + (27 * band);			// Band ptr
      for ( rp=pair; rp < pair+3; rp++ )	// Pair picker ptr
      {
	r1 = bp + (9*rp[0]);		// Row-1 ptr
	r2 = bp + (9*rp[1]);		// Row-2 ptr
	rank = TwoRowRanker(r1,r2);
	if ( rank > minrank ) continue;
	if ( rank < minrank ) { minrank=rank; kt=0; }
	story[kt][0] = type;
	story[kt][1] = band;
	story[kt][2] = rp[0] + rp[1];	// 1, 2 or 3
	kt++;
      }

    } // band
  } // type

# if (DEBUG & 1)
  int ii;
  fprintf(fp,"*** TwoRowPrescore()\n");
  for ( ii=0; ii < kt; ii++ )
      fprintf(fp," %2d :: type=%d band=%d pik=%d\n",
	     ii,story[ii][0],story[ii][1],story[ii][2]);
  fprintf(fp,"\n");
# endif

  // Return rank of lowest ranking pairs.
  (*storykt) = kt;
  return(minrank);

} // End-of-function

/*******************************************************************************
			      TwoRowRanker()
Purpose:
  Given two full rows from the same band, this routine returns a number [0,14]
  which ranks how well the rows would be as the first two rows (either order)
  in a Row Minlex Form matrix.  Rank=0 is best.
*******************************************************************************/
int TwoRowRanker (int r1[9], int r2[9])
{
  // Use a binary search of this pre-computed table to find the output of
  // TwoRowChainer().  The "rank" is in the corresponing spot in array Rank[].
  static const int Table[224] = {
      71835,   72075,   74139,   74343,   74907,   75291,   75303,   75339,
      99483,   99867,   99879,   99915,  100455,  100635,  100647,  100743,
     102795,  102939,  102951,  102987,  104523,  104583,  135579,  135783,
     137319,  137499,  137511,  137607,  140391,  140871,  148635,  149019,
     149031,  149067,  149607,  149787,  149799,  149895,  155931,  155943,
     156039,  156231,  156747,  156807,  287451,  288459,  289947,  296667,
     297435,  299739,  301515,  301851,  301863,  302235,  307611,  307815,
     398043,  399819,  400155,  400167,  400539,  401883,  403143,  403227,
     403239,  404583,  411339,  412107,  412443,  412455,  418251,  418503,
     420939,  420999,  422283,  422427,  422439,  422475,  443547,  443931,
     443943,  443979,  444519,  444699,  444711,  444807,  465051,  465291,
     467355,  467559,  468123,  468507,  468519,  468555,  542427,  543195,
     549339,  550599,  550683,  550695,  552039,  553371,  553575,  561627,
     563655,  564327,  594651,  596427,  596763,  596775,  597147,  598491,
     599751,  599835,  599847,  601191,  624327,  624411,  624423,  625095,
     627147,  627399,  629835,  629895,  635163,  635175,  635271,  635463,
     640155,  640539,  640551,  640587,  641127,  641307,  641319,  641415,
     725403,  725607,  727143,  727323,  727335,  727431,  730215,  730695,
    1159899, 1209051, 1230555, 1231323, 1602267, 1618395, 1683915, 1684167,
    1689291, 1690059, 1690395, 1690407, 1774299, 1776075, 1776411, 1776423,
    1776795, 1778139, 1779399, 1779483, 1779495, 1780839, 1860315, 1861323,
    1862811, 1869531, 1870299, 1872603, 1874379, 1874715, 1874727, 1875099,
    1880475, 1880679, 2208219, 2213595, 2214363, 2257371, 2388699, 2404827,
    2519499, 2519751, 2541255, 2541339, 2541351, 2542023, 2560731, 2562507,
    2562843, 2562855, 2563227, 2564571, 2565831, 2565915, 2565927, 2567271,
    2901723, 2902491, 2908635, 2909895, 2909979, 2909991, 2911335, 2912667,
    2912871, 2920923, 2922951, 2923623, 7107291, 7123419, 7451355, 7500507,
    7522011, 7522779,10253019,10269147,11645403,11650779,11651547,11694555
  };

  static const int Rank[224] = {
    10, 8,10, 8,11,11,10,10,11, 2,11,11,10,11,11,10,10,11,10,11,10, 8, 8,10,11,
    10,11,10,10, 8,10,11,11,10,11,11, 2,11,10,11,11,10, 8,10, 4, 4, 9, 7, 4, 7,
     4,12,12, 9, 9, 9, 7, 7, 1,14, 9, 4, 7,12,12, 9, 7, 7,12,12, 4, 4, 9, 9, 9,
     9, 9, 9,12, 1,12,12,12,12,14,12, 7, 4, 4, 4, 7, 7, 7, 4, 4, 7, 7, 4,12,12,
     9, 9, 9, 4, 4, 9, 4, 7,12,12, 9, 7, 7,14, 1, 9, 7,12,12, 7, 4, 4, 9, 9, 9,
     9, 9, 9,12,14,12,12,12,12, 1,12, 4, 4, 7, 7, 7, 4, 7, 4, 3, 3, 6, 6, 3, 3,
     3, 3, 6, 6, 6, 6, 6, 6, 0,13, 6, 6, 6,13,13, 6, 3, 3, 6, 3, 3, 3, 3, 6, 6,
     6, 3, 3, 3, 6, 6, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6,13,13, 6, 6, 6,13, 0, 6,
     3, 3, 3, 3, 6, 6, 6, 3, 3, 3, 3, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
  };

  // Functions
  int TwoRowChainer(int r1[9],int r2[9]);
  void FatalError(char *routine);

  // Compute two-row cycle structure representative
  int i, j, n;
  int x = TwoRowChainer(r1,r2);

  // Find representative in table (binary search).
  for ( i=0, j=224; i < j; )
  {
    n = (i+j)>>1;
    if ( Table[n] < x ) { i = n+1; continue; }
    if ( Table[n] > x ) { j = n;   continue; }
    return(Rank[n]);
  }

  // Fatal error.
  FatalError("TwoRowRanker()");

} // End-of-function

/*******************************************************************************
			     TwoRowChainer()
Purpose:
  Using chaining, compute the "abc" cycle structure of two rows from the
  same band.
Return value:
  After the cycle structure is computed, all cycles are stuffed into a single
  word.  The "abc" values are coded as 0,1,2 and cycle separators coded as 3.
  Each coded value occupies 2-bits.  The stuffed word is returned and is
  meant to used as input to TwoRowRanker().
*******************************************************************************/
int TwoRowChainer (int r1[9], int r2[9])
{
  // Declarations
  int map[10], cycle[4], len, abc, i, j, n, x;

  // Compute inverse r1[]
  for ( i=0; i < 9; i++ ) map[r1[i]] = i;

  // Loop over possible start-of-cycle positions
  for ( n=i=0; i < 9; i++ )
  {
    // Check for start-of-cycle
    if ( map[ x=r1[i] ] < 0 ) continue;

    // Initialize cycle
    abc = i/3;				// ABC value
    len = 0;				// Cycle length
    j = i;				// Chain index

    // Chain values to find next cycle entry
    newentry:
    map[x] = -1;
    len++;
    if ( map[ x=r2[j] ] >= 0 )
       { abc = (abc<<2) | (j=map[x])/3; goto newentry; }

    // End-of-cycle
    cycle[n++] = abc | (len<<18);

  } // i

  // Pack cycles into one word (use binary '11' as separator)
  for ( x=i=0; i < n; i++ )
  {
    len = cycle[i]>>18;
    x = (x<<(2*len)) | (cycle[i] & 0777777);
    x = (x<<2) | 03;
  }

  // Return packed cycle structure
  return(x);

} // End-of-function

/*******************************************************************************
				  KeySwapper()
*******************************************************************************/
void KeySwapper (int rank, int key[18][9], int keykt)
{
  // Swap permutations
  static int swapa[9] = {8,7,6,5,4,3,2,1,0};
  static int swapb[9] = {0,1,2,6,8,7,3,5,4};
  static int swapc[9] = {3,4,5,0,1,2,7,6,8};
  static int swapd[9] = {3,4,5,0,1,2,6,7,8};
  static int swape[9] = {3,4,5,0,1,2,6,8,7};
  static int swapf[9] = {4,3,5,1,0,2,6,7,8};
  static int swapg[9] = {4,3,5,1,0,2,6,8,7};
  static int swaph[9] = {0,1,2,7,8,6,5,3,4};
  static int swapi[9] = {0,1,2,8,7,6,5,4,3};

  static int *swapit[15] = { swapa, swapb, swapa, swapa, swapc,
			     swapa, swapd, swape, swapa, swapf,
			     swapg, swapa, swapa, swaph, swapi };
  // Declarations
  int *perm = swapit[rank], *kee, t[9], ii, i;

  // Permute keys
  for ( ii=0; ii < keykt; ii++ )
  { kee = key[ii];
    for ( i=0;  i < 9; i++ ) t[i] = kee[i]; 
    for ( i=0;  i < 9; i++ ) kee[i] = t[perm[i]]; 
  }

} // End-of-function

/*******************************************************************************
				  FinishBand()
*******************************************************************************/
int FinishBand (int rank, int cand[18][3], int candkt, FBANS *fbans)
{
  // Functions
  int Eureka(int rank,int r1[9],int r2[9],int key[18][9]);
  void KeySwapper(int rank,int key[18][9],int keykt);

  // Declarations
  FBANS *ans;
  int pair[18] = { 0,1,2, 1,0,2,  0,2,1, 2,0,1,  1,2,0, 2,1,0 };
  int key[18][9], keykt, symmap[10];
  int *cp, *bp, *f1, *f2, *r1, *r2, *r3, *kee, *p1, *p2;
  int anskt, ii, jj, i, j, x;

  // Initialize minimum score
  int score, minscore = 1<<30;
  symmap[0] = 0;

  // Loop over candidate row pairs
  for ( ii=0; ii < candkt; ii++ )
  {
    // Break-out story info
    cp = cand[ii];			// Candidate ptr (type,band,spin)
    bp = TwoPuz[cp[0]]+(27*cp[1]);	// Band ptr

    // Loop over flip-flop order for first two rows
    for ( f1=pair+(6*(cp[2]-1)), f2=f1+6; f1 < f2; f1 += 3 ) 
    {
      r1 = bp + (9*f1[0]);
      r2 = bp + (9*f1[1]);
      r3 = bp + (9*f1[2]);

      // Compute keys, given first two rows.
      if ( (f2-f1)==6 ) { keykt = Eureka(rank,r1,r2,key); } else
			{ KeySwapper(rank,key,keykt); }

      // Loop over Eureka keys
      for ( jj=0; jj < keykt; jj++ )
      {
	// Compute symbol map for this key
	kee = key[jj];
	for ( i=0; i < 9; i++ ) symmap[ r1[ kee[i] ] ] = i+1;

	// Compute score
	for ( score=i=0; i < 9; i++ )
	    score = (10*score) + symmap[r3[kee[i]]];
	if (score > minscore) continue;
	if (score < minscore) { minscore=score; anskt=0; }

	// Save story for subsequent extension
	if ( anskt == MAXANS ) exit(987);
	ans = fbans + anskt;
	ans->type = cp[0];
	for ( i=0; i < 10; i++ ) ans->symmap[i] = symmap[i];
	for ( i=0; i <  9; i++ ) ans->colkey[i] = kee[i];
	for ( i=0; i <  3; i++ ) ans->rowkey[i] = (3*cp[1])+f1[i];
	anskt++;

      } // jj

    } // f1

  } // ii

  // All is well!
  return(anskt);

} // End-of-routine

/*******************************************************************************
				FinishGrid()
*******************************************************************************/
void FinishGrid (FBANS *fbans, int anscnt, int stdform[81])
{
  // Declarations
  FBANS *a[MAXANS], *ans;
  int t[10], rownum, colnum, score, minscore;
  int *puz, *rp, band, ii, i, j, n;

  // Loop over full box answers
  for ( ii=0; ii < anscnt; ii++ )
  {
    // Break-out answer info
    a[ii] = ans = fbans + ii;
    puz = TwoPuz[ans->type];

    // Get "translated" digits for first column
    j = ans->colkey[0];
    for ( i=0; i < 9; i++ )
    { rp = puz + (9*i);
      t[ ans->symmap[ rp[j] ] ] = i;
    }

    // Compute row key. 
    for ( n=0, i=1; i < 10; i++ )
    { if ( t[i] >= 0 )
      { band = t[i]/3;
        ans->rowkey[n++] = t[i];
        t[i] = -3;
        for ( j=i+1; j < 10; j++ )
        { if ( t[j]/3 == band )
	     { ans->rowkey[n++] = t[j]; t[j] = -3; }
        }
      }
    }

  } // ii

  // Loop over remaining grid positions, 
  // removing bad scoring answers on the fly.
  for ( rownum=3; rownum < 9; rownum++ )
  {
    for ( colnum=0; colnum < 9; colnum++ )
    {
      minscore = 999;
      for ( ii=0; ii < anscnt; ii++ )
      {
	// Compute 1-digit score for this answer 
	ans = a[ii];
	rp = TwoPuz[ans->type] + (9*ans->rowkey[rownum]);
        score = ans->symmap[ rp[ans->colkey[colnum]] ];

	// Delete answer if score exceeds minimum
	if (score > minscore)
	   { a[ii] = a[anscnt-1]; anscnt--; ii--; }

	// Check for new minimum score
	if (score < minscore)
	{
	  // Delete prior answers, if any.
	  minscore = score;
	  if (ii)
	  { for ( j=0, i=ii; i < anscnt; i++ ) a[j++] = a[i];
	    ii = 0; anscnt = j;
	  }
	}

	// The only answer must be the best answer!
	if (anscnt==1) goto jackpot;

      } // ii

    } // colnum

  } // rownum

  // Finally.....
  jackpot:
  ans = a[0];
  for ( n=i=0; i < 9; i++ )
  { rp = TwoPuz[ans->type] + (9*ans->rowkey[i]);
    for ( j=0; j < 9; j++ )
	stdform[n++] = ans->symmap[ rp[ans->colkey[j]] ];
  }

} // End-of-routine

/*******************************************************************************
				  Eureka()
Purpose:
  Use two-row cycle info to generate keys which will transform input
  rows back to minimal form.  (Yes, this is complicated!)
*******************************************************************************/
int Eureka (int rank, int r1[9], int r2[9], int key[18][9])
{
  // Functions
  int GetCycleInfo(int r1[9],int r2[9],int cyclen[4],int cycpos[4][18]);
  void FatalError(char *routine);

  // Declarations
  int abc[9] = { 0,0,0, 1,1,1, 2,2,2 }, ABC[3];
  TWOROWDATA *trd = trdata+rank;
  int cyclen[4], cycpos[4][18];
  int cyc[4], pic[4], a[4], avail, keykt, nc;
  int ii, jj, kk, tem, i, j, k, n, *p1, *p2, *kee;

  // Compute cycle info
  GetCycleInfo(r1,r2,cyclen,cycpos);

	    // Loop over MinLex Form cycles
	    keykt = 0;				// Number of keys found
	    nc = trd->nc;			// Number of cycles
	    avail = (1<<nc)-1;			// Available cycles
	    ii = -1;
  newlev:   if ( ++ii == nc )			// New MinLex Cycle
	    {
	      // Jackpot: construct key.
	      kee = key[keykt];
	      for ( i=0; i < nc; i++ )
	      { p1 = trd->pos + trd->cumlen[i];		// MinLex positions
		p2 = cycpos[cyc[i]] + pic[i];		// Data positions
		for ( j=0; j < trd->len[i]; j++ )
		    //kee[ p2[j] ] = p1[j];
		    kee[ p1[j] ] = p2[j];
	      }

	      if ( ++keykt == trd->count ) return(keykt);
	      goto backup;
	    }

	    // Get next available 'data' cycle
	    a[ii] = avail;
  newcyc:   jj = Lobit[ a[ii] ];
	    if ( jj < 0 ) goto backup;

	    // Try next cycle if cycle lengths do not match
	    a[ii] ^= (1<<jj);
	    if ( trd->len[ii] != cyclen[jj] ) goto newcyc;
	    cyc[ii] = jj;			// Record cycle number
	    avail ^= (1<<jj);			// Make cycle unavailable

	    // Get next position-in-cycle
	    kk = -1;
  newpic:   if ( ++kk == trd->len[ii] )
	    {
	      avail |= (1<<cyc[ii]);		// Restore availability    	      
	      goto newcyc;
	    }

	    // Compute pointers to cycle positions
	    p1 = trd->pos + trd->cumlen[ii];	// MinLex positions
	    p2 = cycpos[jj] + kk;		// Data positions

	    // Use first cycle to define 'ABC'
	    if (ii==0)
	    { ABC[ i = p2[0]/3 ] = 0;
	      ABC[ j = p2[1]/3 ] = 1;
	      ABC[ Lobit[ 7^(1<<i)^(1<<j) ] ] = 2;
	    }

	    // Check for 'abc' vs. 'ABC' match
	    for ( i=0; i < trd->len[ii]; i++ )
		if ( abc[ p1[i] ] != ABC[ p2[i]/3 ] )
		   goto newpic;

	    // Record position-in-cycle.
	    pic[ii] = kk;
	    goto newlev;

  backup:   if ( --ii >= 0 )
	    { jj = cyc[ii];		// Restore cycle number
	      kk = pic[ii];		// Restore position-in-cycle
	      goto newpic;
	    }

  // Fatal error.
  FatalError("Eureka()");

} // End-of-routine

/*******************************************************************************
				GetCycleInfo()
*******************************************************************************/
int GetCycleInfo (int r1[9], int r2[9], int cyclen[4], int cycpos[4][18])
{
  // Declarations
  long long cycle[4], tem;
  int map[10], len, i, j, k, n, x, *p;

  // Initialize used-clue map
  for ( map[0]=0, i=0; i < 9; i++ ) map[r1[i]] = i;

  // Loop over possible start-of-cycle positions
  for ( n=i=0; i < 9; i++ )
  {
    // Check for start-of-cycle
    if ( map[ x=r1[i] ] < 0 ) continue;

    // Initialize cycle
    cycle[n] = i;		// Initial value	
    len = 0;			// Cycle length
    j = i;			// Chain index

    // Chain values to find next cycle entry
    newentry:
    map[x] = -1;
    len++;
    if ( map[ x=r2[j] ] >= 0 )
       { cycle[n] = (cycle[n]<<4) ^ (j = map[x]); goto newentry; }

    // End-of-cycle
    cycle[n] |= ((long long)len)<<36;
    n++;

  } // i

  // Sort cycle lengths big to small.
  for ( i=0; i < n; i++ )
      for ( j=i+1; j < n; j++ )
          if ( cycle[i] < cycle[j] )
	     { tem=cycle[i]; cycle[i]=cycle[j]; cycle[j]=tem; }

  // Unpack cycle positions
  for ( i=0; i < n; i++ )
  {
    tem = cycle[i];
    cyclen[i] = len = tem>>36LL;
    k = 4*len;
    p = cycpos[i];
    for ( j=0; j < len; j++ )
        { k -= 4; p[j] = p[len+j] = (int)((tem>>k)&0xf); }
  }

} // End-of-function

/*******************************************************************************
				Fatal Error()
*******************************************************************************/
void FatalError (char *routine)
{
  // Declarations
  int corner[9] = { 0,3,6,27,30,33,54,57,60 };
  int offset[9] = { 0,1,2,9,10,11,18,19,20 };
  int *puz, *p1, *p, ii, i, j, n, k, x, kt=0;

  fprintf(stderr,"*** Sorry, but the program has died!\n");
  fprintf(stderr,"Error detected by routine \"%s\".\n\n",routine);

  // Output the puzzle
  puz = TwoPuz[0];
  for ( n=i=0; i < 9; i++ )
  { for ( j=0; j < 3; j++ )
    { for ( k=0; k < 3; k++ )
      { x = puz[n++];
	if ( (x<0) || (x>9) )
	   { fprintf(stderr," x"); kt++; } else
	if ( x==0 )
	   fprintf(stderr," ."); else
	   fprintf(stderr," %d",x);
      }
      fprintf(stderr," ");
    }
    if ( (i%3)==2 ) fprintf(stderr,"\n");
    fprintf(stderr,"\n");
  }

  // Out-of-range clue found.
  if (kt)
  { fprintf(stderr,"*** %d out-of-range clue(s) ('x') found!\n",kt);
    exit(666);
  }

  // Check for duplicates
  for ( ii=0; ii < 9; ii++ )
  {
    // Check row for duplicates
    p = puz + (9*ii);
    for ( x=i=0; i < 9; i++ )
    { if ( p[i] && ((x>>p[i])&1) )
         { fprintf(stderr,"Duplicate clue(s) found in row %d.\n",ii+1);
           exit(666); }
      x |= (1<<p[i]);
    }

    // Check column for duplicates
    p = puz+ii;
    for ( x=i=0; i < 81; i += 9 )
    { if ( p[i] && ((x>>p[i])&1) )
         { fprintf(stderr,"Duplicate clue(s) found in row %d.\n",ii+1);
           exit(666); }
      x |= (1<<p[i]);
    }

    // Check box for duplicates
    p1 = puz + corner[ii];
    for ( x=i=0; i < 9; i++ )
    { p = p1 + offset[i];
      if ( (*p) && ((x>>(*p))&1) )
         { fprintf(stderr,"Duplicate clue(s) found in box %d.\n",ii+1);
           exit(666); }
      x |= (1<<(*p));
    }

  } // ii

  fprintf(stderr,"*** How strange, the puzzle seems to be error free!\n");
  fprintf(stderr,"    Contact: Mike Deverin at devfam47@gmail.com\n");
  exit(0);
}


