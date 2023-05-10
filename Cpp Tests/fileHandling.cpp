// include
#include <iostream>
#include <fstream>

using namespace std;

// write text to file of given path
int writeFile(string path, string text){
    ofstream f;
    try{       
        f.open(path);
        f << text;
        f.close();
        return 1;
    }
    catch(exception e){
        return 0;
    }
}

// read text from file of given path
string readFile(string path){
    string line, s;
    ifstream f(path);

    if (f.is_open()){
      while (getline(f,line)){
        s += line + "\n";
      }
      f.close();
      return s;
    }
    else return "Unable to open file"; 
}

int main(){

    string  path = "test.txt",
            text = "Hallo Welt!",
            pathProc = "/proc/&pid/statm";

    //writeFile(path, text);
    //cout << readFile(path);
    
    cout << system("ps -eFw");
    
    return 0;
}