#include <iostream>
#include <unistd.h>

using namespace std;

int main(/*int argc, char* argv[]*/)
{
    
    cout << endl << endl /*<< argv[1]*/ << "\tEnd of Child Process with PID: " << getpid() << endl << endl;
     
    return 0;
}