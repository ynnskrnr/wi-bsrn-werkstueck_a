#include <iostream>
#include <vector>
// fork/exec
#include <unistd.h>
// Fuer abfang von fehlereingaben
#include <limits>

using namespace std;

/*
Erzeugt Prozess
Fuegt Kinderprozess in den Prozesse vector ein
Gibt 1 zuerueck wenn der laufende Prozess der Kinderprozess ist
*/
int addChild(vector<pid_t> &prozesse)
{
    prozesse.push_back(fork());
    if (!prozesse.back())
    {
        return 1;
    }
    return 0;
}

/*
Aktueller Prozess wird durch date ersetzt
*/
void date()
{
    execl("/bin/date", "date", "-u", NULL);
}

/*
Vereinfachte Version der exec Familie
Startet Programm mit angegebenen Pfad
*/
void exec(const char *path)
{
    const char *args = NULL;
    execl(path, args, (char *)NULL);
}

// main
int main()
{
    string optionen[] = {"Datum Ausgeben",
                         "PIDs Ausgeben",
                         "Beenden",
                         "Hello World! mit fork() und exec()"};
    vector<pid_t> prozesse;
    // Fuegt Vaterprozess hinzu (sinnvoll, oder nicht? Zum bereinigen der ressourcen, nicht. Ansonsten schon.)
    //prozesse.push_back(getpid());

    bool running = true;
    int option,
        optionenSize = sizeof(optionen) / sizeof(string);

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

        // Ausfuehrung der gewaehlten Option
        switch (option)
        {
        // Datum ausgeben
        case 1:
            if (addChild(prozesse))
            {
                date();
            }
            usleep(1500);
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
        // Hello World! mit fork() und exec()
        // So werden die einzelnen Funktionen des Programms aufgerufen (Dateinen sind in options/)
        case 4:
            if (addChild(prozesse))
            {
                exec("./options/helloWorld");
            }
            usleep(1500);
            break;
        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    return 0;
}
