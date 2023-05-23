// include <iostream>
#include <iostream>
// include vector
#include <vector>
// include fork
#include <unistd.h>

using namespace std;

// main
int main()
{

    vector<pid_t> prozesse;
    string optionen = {"1. Datum Ausgeben\n"};

    cout << optionen;
    int option;
    cin >> option;

    switch (option)
    {
    case 1:
        // tochterprozess noch in array speichern
        if (!fork())
        {
            execl("/bin/date", "date", "-u", NULL);
        }
    case 2:
        // code block
        break;
    default:
        break;
    }

    return 0;
}
