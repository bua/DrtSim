#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#define MAX 100
#include "callingcpp.h"
#include <stdint.h>
using namespace std;

int i,j=0;
int k,l,m;

//The below code will create a table to be feed by LTSU output
struct ltsu_table {
  string framePointer, instructionPointer, sycCounter,flag;
  int cid;
};
 ltsu_table* ltsu = new ltsu_table[MAX];  


//populating LFFT table
int lffttable (char *fpValue, char *flag)
{
ltsu[i].framePointer = fpValue;
ltsu[i].flag = flag;
i++;
return i;
}

//populating WT table
int wtTable (char *fpValue, char *ipValue, char *scValue, char *flag)
{
ltsu[j].framePointer = fpValue;
ltsu[j].instructionPointer = ipValue;
ltsu[j].sycCounter = scValue;
ltsu[j].flag = flag;
j++;
return j;
}

//populating PLQ table
int plqTable (char *fpValue, char *ipValue, char *flag)
{
ltsu[k].framePointer = fpValue;
ltsu[k].instructionPointer = ipValue;

ltsu[k].cid = (rand() % 100 + 1);  // randomly core id generating 1 to 100
fflush(stdout);
ltsu[k].flag = flag;
k++;
return k;
}

//One compact table to display the output
void callPrint(int counter, char *name){
cout << "\n****Displaying " << counter << " entries of "<< name << "TABLE  ****\n" ;
for(l = 0; l<counter; l++){
if((strcmp(name," WT ")==0)|| (strcmp(name," LFFT ")==0)){
//cout << name << "\n";
cout << ltsu[l].framePointer << "\t";
}// for LEFFT AND WT


if((strcmp(name, " WT ")==0)){
//cout << "\n****Displaying WT \n";
cout << ltsu[l].instructionPointer << "\t";
cout << ltsu[l].sycCounter << "\t";
} // FOR WT ONLY



if((strcmp(name, " PLQ ")==0)){
cout << ltsu[l].framePointer << "\t";
cout << ltsu[l].instructionPointer << "\t";
cout << ltsu[l].cid << "\t";
} //for PLQ only
cout << ltsu[l].flag << "\n";
}
}



