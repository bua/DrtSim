#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#define MAX 100
#include "callingcpp.h"
#include <stdint.h>
using namespace std;

int ii,jj,ll;

//The below code will create a table to be feed by DTSU output
struct dtsu_table {
  string instructionPointer, sycCounter, flag;
  int cid, freeFrames;
};
 dtsu_table* dtsu = new dtsu_table[MAX];  


//populating DFFT table
int dffttable (char *flag)
{
srand(time(0));
cout << "Displying the STATIC DTSU \n";
dtsu[ii].freeFrames = (rand() % 10 + 1);  // randomly  generating free Frames from 1 to 1000
fflush(stdout);
dtsu[ii].cid = (rand() % 100 + 1);  // randomly core id generating 1 to 100
fflush(stdout);
dtsu[ii].flag = flag;
ii++;
return ii;
}

//populating PTQ table
int ptqTable (char *ipValue, char *scValue, char *flag)
{
dtsu[jj].cid = (1 + (rand() % 100));  // randomly core id generating 1 to 100
fflush(stdout);
dtsu[jj].instructionPointer = ipValue;
dtsu[jj].sycCounter = scValue;
dtsu[jj].flag = flag;
jj++;
return jj;
}

//One compact table to display the output
void calldtsuPrint(int counter, char *name){
cout << "\n****Displaying " << counter << " entries of "<< name << "TABLE  ****\n" ;
for(ll = 0; ll<counter; ll++){

if((strcmp(name," DFFT ")==0)){
//cout << name << "\n";
cout << dtsu[ll].cid << "\t"; 
cout << dtsu[ll].freeFrames << "\t";
}// for DFFT

if((strcmp(name, " PTQ ")==0)){
cout << dtsu[ll].cid << "\t";
cout << dtsu[ll].instructionPointer << "\t";
cout << dtsu[ll].sycCounter << "\t";
} //for PTQ only

cout << dtsu[ll].flag << "\n";
}
}



