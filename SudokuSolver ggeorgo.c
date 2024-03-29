//http://forum.enjoysudoku.com/sudoku-solver-in-standard-c-backtracking-recursive-algorithm-t4117.html
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------

#define MAXSOLUTIONS 5

//---------------------------------------------------------------------

int grid[9][9], solutions;

//---------------------------------------------------------------------

static void reset_grid()
{
int i, j;
for (i = 0; i < 9; i++)
for (j = 0; j < 9; j++) {
grid[i][j] = 0;
}
}

//---------------------------------------------------------------------

static int CheckValueInGrid(int i,int j, int val)
{
int k, m;
//check in row
for (k=0; k<9; k++) {
if (k!=j)
if (grid[i][k]==val)
return 0;
}
// check in column
for (m=0; m<9; m++) {
if (m!=i)
if (grid[m][j]==val)
return 0;
}
//check in subgrid
for (k=(i/3)*3; k<(i/3)*3+3; k++)
for (m=(j/3)*3; m<(j/3)*3+3; m++) {
if ((k!=i) || (m!=j))
if (grid[k][m]==val)
return 0;
}
return 1;
}

//---------------------------------------------------------------------

static int read_grid()
{
int i, j, ch;

reset_grid();
for (i = 0; i < 9; i++)
for (j = 0; j < 9; j++)
for (;;) {
ch = getchar();
//printf("%c",ch);
if (ch == EOF)
return -1;
if (ch == '.')
break;
if ('1' <= ch && ch <= '9') {
if (CheckValueInGrid(i, j, 1+ch-'1')) {
grid[i][j] = 1+ch-'1';
break;
} else {
return -1;
}
}
if (!isspace(ch) && ch != '|' && ch != '-' && ch != '+')
return -1;
}

for (;;) {
int ch = getchar();
if (ch == EOF)
return 0;
if (!isspace(ch) && ch != '|' && ch != '-' && ch != '+')
return -1;
}
}

//---------------------------------------------------------------------

static void print_grid()
{
int i, j;
for (i = 0; i < 9; i++) {
if (i%3 == 0)
printf("+---+---+---+\n");
for (j = 0; j < 9; j++) {
if (j%3 == 0)
printf("|");
printf("%d", grid[i][j]);
}
printf("|\n");
}
printf("+---+---+---+\n\n");
}

//---------------------------------------------------------------------

static int SearchSolution(int i, int j)
{
int value;
if (j==9) {
j=0;
i++;
}
if (i==9) { //solution
print_grid();
solutions++;
fprintf(stderr," %d solution(s) found !\n", solutions);
return solutions >= MAXSOLUTIONS;
}

if (grid[i][j]!=0) {
if (SearchSolution(i, j+1))
return 1;
} else {
for (value = 1; value < 10; value++) /* Try possible values. */
if (CheckValueInGrid(i, j, value)) {
grid[i][j]=value; //new test value
if (SearchSolution(i, j+1))
return 1;
}
grid[i][j]=0; //unrecord try
}
return 0;
}

//---------------------------------------------------------------------

static int solution()
{
if (read_grid()==-1) {
fprintf(stderr, "Invalid input matrix\n");
return 1;
}

solutions = 0;
printf("Starting Matrix\n");
print_grid();
printf("\n\nSolving...\n\n");

SearchSolution(0,0);
if (solutions == 0) {
fprintf(stderr, "No solution found\n");
return 1;
}
return 0;
}

//---------------------------------------------------------------------

int main(int argc, char **argv)
{
char *output = 0;
if (!freopen("SUDOKU.TXT" , "r", stdin)) {
fprintf(stderr,"input file SUDOKU.TXT does not exist.\n");
return 1;
}

if (output && !freopen(output, "w", stdout)) {
perror(output);
return 1;
}

return solution();
}