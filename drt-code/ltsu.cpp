//This program is to cretae a LTSU structure with defined tables

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

struct ltsu_table {
  string framePointer;
  int  flag;
};

ltsu_table lfft;   //LEFT table creation with only two data members 1. FP 2. status(1/0)

int main ()
{
  string mystr;
  cout << "Enter FP: ";
  getline (cin,lfft.framePointer);
  cout << "Enter Flag: ";
  getline (cin,mystr);
  stringstream(mystr) >> lfft.flag;
  cout << "My FP is:\n ";
  cout<< lfft.framePointer;
  cout << "And Flag is:\n ";
  cout<< lfft.flag;
  return 0;
}
