http://www.cplusplus.com/forum/lounge/29126/
----------
#include <iostream>
#include <algorithm>
#include <ctime>

const unsigned SIZE=9;

typedef unsigned char sudoku[SIZE][SIZE];

bool check(sudoku matrix,unsigned x,unsigned y){
	unsigned char val=matrix[x][y];
	for (unsigned a=0;a<x;a++)
		if (matrix[a][y]==val)
			return 0;
	for (unsigned a=x+1;a<SIZE;a++)
		if (matrix[a][y]==val)
			return 0;
	for (unsigned a=0;a<y;a++)
		if (matrix[x][a]==val)
			return 0;
	for (unsigned a=y+1;a<SIZE;a++)
		if (matrix[x][a]==val)
			return 0;
	unsigned startx=x/(SIZE/3)*3,
		starty=y/(SIZE/3)*3,
		endx=startx+SIZE/3,
		endy=starty+SIZE/3;
	for (unsigned a=startx;a<endx;a++){
		for (unsigned b=starty;b<endy;b++){
			if (a!=x && b!=y && matrix[a][b]==val)
				return 0;
		}
	}
	return 1;
}

void printMatrix(sudoku matrix){
	for (unsigned b=0;b<SIZE;b++){
		for (unsigned a=0;a<SIZE;a++){
			if (matrix[a][b])
				std::cout <<(int)matrix[a][b];
			else
				std::cout <<'_';
			if (a && a%(SIZE/3)==SIZE/3-1)
				std::cout <<' ';
		}
		std::cout <<std::endl;
		if (b && b%(SIZE/3)==SIZE/3-1)
			std::cout <<std::endl;
	}
}

void transpose(sudoku matrix){
	for (unsigned y=0;y<SIZE-1;y++){
		for (unsigned x=y+1;x<SIZE;x++){
			std::swap(matrix[x][y],matrix[y][x]);
		}
	}
}

unsigned solution_time=0;

bool solve(sudoku matrix,unsigned pos=0){
	unsigned x=pos%SIZE,
		y=pos/SIZE;
	if (y>=SIZE)
		return 1;
	if (matrix[x][y])
		return solve(matrix,pos+1);
	for (unsigned a=1;a<=SIZE;a++){
		solution_time++;
		matrix[x][y]=a;
		if (check(matrix,x,y) && solve(matrix,pos+1))
			return 1;
	}
	matrix[x][y]=0;
	return 0;
}

int main(){
	sudoku matrix=
	{
		1,0,0,	0,0,7,	0,9,0,
		0,3,0,	0,2,0,	0,0,8,
		0,0,9,	6,0,0,	5,0,0,

		0,0,5,	3,0,0,	9,0,0,
		0,1,0,	0,8,0,	0,0,2,
		6,0,0,	0,0,4,	0,0,0,

		3,0,0,	0,0,0,	0,1,0,
		0,4,0,	0,0,0,	0,0,7,
		0,0,7,	0,0,0,	3,0,0,
	};
	clock_t t0,t1;
	t0=clock();
	transpose(matrix);
	if (!solve(matrix))
		std::cout <<"Unsolvable!"<<std::endl;
	else
		printMatrix(matrix);
	t1=clock();
	std::cout <<"Solved in "<<solution_time<<" iterations."<<std::endl;
	std::cout <<double(t1-t0)/(double(CLOCKS_PER_SEC)/1000.0)<<" ms"<<std::endl;
	return 0;
}
