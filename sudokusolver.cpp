/*

Panagopoulos George - Dec 2007 - "Sudoku" Project for The lesson C++ 

*/



#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <math.h>


// This function Checks Weather I have a number 2 times in a row
bool row(int a[],int n,int r){
for(int i=1;i<=9;i++){
  if(n==a[r+9*i]){return false;}
  }
return true;
}

// This function Checks Weather I have a number 2 times in a colume
bool col(int a[],int n,int c){
for(int i=1;i<=9;i++){
  if(n==a[i+9*c]){return false;}
  }
return true;
}

// This function Checks Weather I have a number 2 times in a box
bool box(int a[],int n,int r,int c){

  int s1,s2,f1,f2;
  if(r>=1 && r<=3 && c>=1 && c<=3){s1=1;f1=3;s2=1;f2=3;}
  if(r>=1 && r<=3 && c>=4 && c<=6){s1=1;f1=3;s2=4;f2=6;}
  if(r>=1 && r<=3 && c>=7 && c<=9){s1=1;f1=3;s2=7;f2=9;}

  if(r>=4 && r<=6 && c>=1 && c<=3){s1=4;f1=6;s2=1;f2=3;}
  if(r>=4 && r<=6 && c>=4 && c<=6){s1=4;f1=6;s2=4;f2=6;}
  if(r>=4 && r<=6 && c>=7 && c<=9){s1=4;f1=6;s2=7;f2=9;}

  if(r>=7 && r<=9 && c>=1 && c<=3){s1=7;f1=9;s2=1;f2=3;}
  if(r>=7 && r<=9 && c>=4 && c<=6){s1=7;f1=9;s2=4;f2=6;}
  if(r>=7 && r<=9 && c>=7 && c<=9){s1=7;f1=9;s2=7;f2=9;}

  

    for(int i=s1;i<=f1;i++){
     for(int j=s2;j<=f2;j++){
      if(n==a[i+9*j]){return false;}
     }
    }
return true;
}

// This function Checks Weather it is possible to put a a number n in a position  
bool check(int a[],int n,int r,int c){
 if(box(a,n,r,c) && row(a,n,r) && col(a,n,c)){return true;}else{return false;}
}

// This function prints the grid
void print(int a[]){

for(int i=1;i<=9;i++){
    for(int j=1;j<=9;j++){
	if(j==4 || j==7){std::cout<<" | ";}      
	if(a[i+9*j]==0){std::cout<<" ";}else{std::cout<<a[i+9*j];}
    }                           
if(i==3 || i==6){std::cout<<" \n--------------- ";} 
 std::cout<<"\n";
}
std::cout<<"\n\n";
}


// This function checks if there is an empty position in the grid
bool all(int a[]){
bool hak=true;
for(int i=1;i<=9;i++){
for(int j=1;j<=9;j++){
if(a[i+9*j]==0){hak=false;}
}
}
return hak;
}


/*This function Solves th sudoku
With the vector gap I insert the positions which are initially  blank.
For each one of them I check which  is the capable  number. If  I  can
not find it I move to the previous position and I search for  the next
correct number. If I can not find it in the first prospective position
I terminate the program. If the Grid is correct I  print the solved puzzle.  
*/

void solve( std::vector<int>gap , std::vector<int>gap2 , int a[] , int e ){
bool ha;

if(e<0){std::cout<<"The Grid does not have solution !\n\n";abort();}
int gsize=gap.size();
for (int i=e;i<gsize;i++){
 ha=false;
 for(int j=a[gap[i]+9*gap2[i]]+1;j<=9;j++){
  if (check(a,j,gap[i],gap2[i])){a[gap[i]+9*gap2[i]]=j;ha=true;break;}
 }
if(all(a)){break;}
if(!ha){a[gap[i]+9*gap2[i]]=0;solve(gap,gap2,a,i-1);}
}

}


int main(int argc , const char* argv[]){
int a[100];
int b[11];


// Read from file
std::ifstream instr(argv[1]);
std::vector<int> f;
int q;
while(instr>>q) {
 f.push_back(q);
}


for(int i=0;i<9;i++){
 b[i+1]=f[i];
}


int t;
for(int i=1;i<=9;i++){
 t=0;
 for(int j=1;j<=9;j++){
  a[i+9*(10-j)]=((b[i]%int(pow(10,j)))-t)/(int(pow(10,j))/10);
  t=a[i+9*(10-j)];
 }
}



// Inert the blank positions in a vector
std::vector<int> gap;
std::vector<int> gap2;

for(int i=1;i<=9;i++){
 for(int j=1;j<=9;j++){
  if(a[i+9*j]==0){
   gap.push_back(i);
   gap2.push_back(j);
  }
 }
}

//Print the puzzle Solved
solve(gap,gap2,a,0);
std::cout<<"\nThe Sudoku Solved is :\n\n";
print(a);
}
