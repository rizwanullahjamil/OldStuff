// SudokuZen.cpp : Defines the entry point for the console application.
//

#include "..\Common\Board.h"
#include <iostream>
#include <tchar.h>
#include <time.h>


void PrintBoard(CBoard& b)
{
	for (int u=0; u<2+9*4-1; u++) printf("-");
	printf("\n");

	for (int v=0; v<BOARD_SIZE; v++)
	{
		printf("|");

		for (int u=0; u<BOARD_SIZE; u++)
		{
			uint16	x = b.GetItem(u,v);

			if (NO_FIXED(x))	printf("   %c", u==2||u==5||u==8?'|':' ');
			else				printf(" %d %c", GET_FIXED(x), u==2||u==5||u==8?'|':' ');
		}

		printf("\n");
		for (int u=0; u<2+9*4-1; u++) printf((v==2||v==5||v==8)?"-":" ");
		printf("\n");
	}
	printf("\n");

    printf("Board Status is : ");

	BoardStatusType bst = b.GetBoardStatus();
	if (bst == BS_INCOMPLETE)		printf("INCOMPLETE");
	else if (bst == BS_ERROR)		printf("ERROR");
	else if (bst == BS_COMPLETE)	printf("COMPLETE");
	printf("\n\n");	
}

extern "C++" int counter;

int _tmain(int argc, _TCHAR* argv[])
{
/*	solved
	// http://image.guardian.co.uk/sys-files/Guardian/documents/2005/06/23/GdnSudoku044_050628.pdf
	int board[] = {
		0,3,7,	0,0,9,	0,0,1,
		6,0,0,	4,0,1,	0,0,0,
		4,0,0,	8,0,0,	5,0,0,

		0,2,8,	0,0,0,	0,3,7,
		0,0,0,	0,0,0,	0,0,0,
		7,1,0,	0,0,0,	2,4,0,

		0,0,9,	0,0,7,	0,0,5,
		0,0,0,	6,0,3,	0,0,2,
		2,0,0,	9,0,0,	3,6,0
	};*/

/*	solved
	// http://image.guardian.co.uk/sys-files/Guardian/documents/2005/06/23/GdnSudoku045_050629.pdf
	int board[] = {
		0,0,0,	7,0,8,	0,0,0,
		4,0,5,	0,6,0,	7,0,8,
		0,3,0,	4,0,5,	0,6,0,

		1,0,2,	0,3,0,	4,0,0,
		0,0,0,	0,0,0,	0,0,0,
		0,0,6,	0,7,0,	8,0,9,

		0,4,0,	5,0,6,	0,7,0,
		2,0,3,	0,4,0,	5,0,6,
		0,0,0,	2,0,3,	0,0,0
	};*/

	// http://image.guardian.co.uk/sys-files/Guardian/documents/2005/06/23/GdnSudoku041_050624.pdf
/*	int board[] = {
		0,0,1,	9,0,0,	3,0,0,
		0,0,0,	0,0,0,	2,0,0,
		7,6,0,	0,2,0,	0,0,9,

		3,0,0,	0,6,0,	9,0,5,
		0,0,2,	1,0,3,	4,0,0,
		4,0,0,	0,9,0,	7,0,3,

		1,0,0,	0,3,0,	0,9,7,
		0,0,4,	0,0,9,	0,0,0,
		0,0,5,	0,0,8,	6,0,0
	};*/

/*	// http://image.guardian.co.uk/sys-files/Guardian/documents/2005/06/23/GdnSudoku041_050624.pdf
	int board[] = {
		0,0,0,	0,0,0,	3,0,0,
		0,0,0,	0,0,0,	2,0,0,
		7,6,0,	0,2,0,	0,0,9,

		3,0,0,	0,6,0,	9,0,5,
		0,0,2,	1,0,3,	4,0,0,
		4,0,0,	0,9,0,	7,0,3,

		0,0,0,	0,3,0,	0,9,7,
		0,0,0,	0,0,9,	0,0,0,
		0,0,0,	0,0,8,	6,0,0
	};*/

/*	solved
	int board[] = {
		0,7,0,	0,2,0,	5,8,0,
		0,1,0,	0,0,0,	0,0,0,
		0,0,0,	0,6,0,	2,0,9,

		2,8,7,	4,0,3,	0,0,0,
		0,0,0,	0,7,0,	0,0,0,
		0,0,0,	0,0,6,	8,7,3,

		5,0,1,	0,3,0,	0,0,0,
		0,0,0,	0,0,0,	0,4,0,
		0,4,3,	0,1,0,	0,2,0
	};*/

/*	// solved
	int board[] = {
		7,0,5,	2,1,0,	0,0,0,
		2,0,3,	0,0,9,	0,0,5,
		0,9,1,	0,0,5,	0,6,2,

		0,0,0,	1,0,0,	0,0,7,
		1,2,4,	9,0,8,	5,0,6,
		0,7,0,	3,5,0,	0,0,0,

		0,3,0,	6,0,1,	2,0,0,
		9,0,0,	0,4,0,	6,0,0,
		6,0,7,	0,0,3,	0,9,8
	};*/

/*	// 17 visibiles, 1 solution
	int board[] = {
		0,9,8,	0,0,0,	0,0,0,
		0,0,0,	0,7,0,	0,0,0,
		0,0,0,	0,1,5,	0,0,0,

		1,0,0,	0,0,0,	0,0,0,
		0,0,0,	2,0,0,	0,0,9,
		0,0,0,	9,0,6,	0,8,2,

		0,0,0,	0,0,0,	0,3,0,
		5,0,1,	0,0,0,	0,0,0,
		0,0,0,	4,0,0,	0,2,0
	};*/

	int board[] = {
		3,0,6,	0,0,0,	0,0,0,
		0,0,0,	0,7,0,	0,0,0,
		4,0,5,	0,0,0,	0,0,0,

		0,0,0,	0,0,0,	0,0,0,
		0,0,0,	0,0,0,	0,0,0,
		0,0,0,	0,0,0,	0,0,0,

		0,0,0,	0,0,0,	0,0,0,
		0,0,0,	0,0,0,	0,0,0,
		0,0,0,	0,0,0,	0,0,0,
	};

	CBoard::InitMem();
	
	CBoard	b; 

	for (int v=0; v<9; v++)
		for (int u=0; u<9; u++)
			b.SetItem(u,v,board[u+v*9]);

	//b.Generate(rand(), 40);	// generate a new board

	SolutionType r =  b.Solve(true);

	PrintBoard(b);

	CBoard::KillMem();

	return 0;
}
