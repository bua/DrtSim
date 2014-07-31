#include <iostream>
#include <string>
#include <cstdlib>
#include<stdio.h>
#include <ctime>
#include <sstream>
#include "callingcpp.h"
#include <stdint.h>
using namespace std;

static const char alphanum[] = "0" "1";
int stringLength = sizeof(alphanum) - 1, ran;


int radGen()
{
     std::string Str;
    Str = alphanum[rand() % stringLength];
	//cout << Str << endl;

stringstream(Str) >> ran;
//cout << " ################ " << ran << "\n";
return ran;
}
