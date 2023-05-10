// include
#include <iostream>
#include <fstream>

using namespace std;

int write(string path){
    ofstream f;
    try
    {       
        f.open (path);
        f << "Writing this to a file.\n";
        f.close();
        return 1;
    }
    catch(const std::exception& e)
    {
        return 0;
    }
}

int main()
{
    string path = "test.txt";

    return 0;
}