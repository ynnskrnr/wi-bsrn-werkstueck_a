// include <iostream>
#include <iostream>
// include vector
#include <vector>
// include fork
#include <unistd.h>
// include streamsize
#include <limits>
// iclude find
#include <algorithm>

using namespace std;

// main
int main()
{
    string optionen[] = {"Datum Ausgeben",
                         "PIDs Ausgeben",
                         "Beenden"};
    vector<pid_t> prozesse;

    bool running = true;
    int option, optionenSize = sizeof(optionen) / sizeof(string);

    system("clear");

    while (running)
    {
        // Menü
        for (int i = 0; i < optionenSize; i++)
        {
            cout << i + 1 << ". " << optionen[i] << endl;
        }

        // Eingabe
        cin >> option;
        // Fehlereingabe abfangen
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            option = 0;
        }

        // Sauberes Terminal
        system("clear");
        cout << endl
             << "Gewaehlte Option: " << optionen[option - 1] << endl;

        /*
        wenn man die reihenfolge aendern will, kann das switch-case statement gleich bleiben
        !unnoetig eigentlich!
        index von bestimmter option
        funktioniert aktuell nicht da case eine konstante braucht
        // int(distance(optionen, find(optionen, optionen + optionenSize, "Datum Ausgeben"))+1);
        */

        switch (option)
        {
        // Datum ausgeben
        case 1:
            // tochterprozess noch in array speichern
            prozesse.push_back(fork());
            if (!prozesse.back())
            {
                execl("/bin/date", "date", "-u", NULL);
            }
            sleep(1);
            break;
        // PIDs ausgeben
        case 2:
            for (pid_t i : prozesse)
            {
                cout << i << ",\t";
            }
            cout << endl;
            break;
        // Beenden
        case 3:
            cout << "Beenden" << endl;
            running = false;
            break;
        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    return 0;
}
