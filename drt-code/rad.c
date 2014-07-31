#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char genRandom()
{

    return alphanum[rand() % stringLength];
}

int main()
{
    srand(time(0));
    std::string Str;
    for(unsigned int i = 0; i < 20; ++i)
    {
    Str += genRandom();

    }
    cout << Str << endl;
    


}


    
    // THANKS FOR THE HELP EssGeEich!

    return 0;

}

