#include <iostream>
#include <vector>
// fork/exec
#include <unistd.h>
// Fuer abfang von fehlereingaben
#include <limits>
#include <sys/wait.h>
// include syscalls
#include <sys/syscall.h>

using namespace std;

/*
Erzeugt Prozess
Fuegt Kinderprozess in den Prozesse vector ein
Gibt 1 zuerueck wenn der laufende Prozess der Kinderprozess ist
*/
int addChild(vector<pid_t> &prozesse)
{
    pid_t pid = fork();
    if (pid)
    {
        prozesse.push_back(pid);
        return 0;
    }
    return 1;
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

void exec(string path)
{
    const char *pathC = path.c_str();
    const char *args = NULL;
    execl(pathC, args, (char *)NULL);
}

void releaseResources(vector<pid_t>& prozesse) {
    int status;
    for (pid_t pid : prozesse) {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid fehlgeschlagen");
        } else {
            if (WIFEXITED(status)) {
                cout << "Kindprozess " << pid << " wurde beendet. Exit-Status: " << WEXITSTATUS(status) << endl;
            } else if (WIFSIGNALED(status)) {
                cout << "Kindprozess " << pid << " wurde durch Signal beendet: " << WTERMSIG(status) << endl;
            }
        }
    }
}

// main
int main()
{
    string optionen[] = {"Datum Ausgeben",
                         "PIDs Ausgeben",
                         "Beenden",
                         "Hello World! mit fork() und exec()"/*,
                         "Freigabe test"*/};
    vector<pid_t> prozesse;
    // Fuegt Vaterprozess hinzu (sinnvoll, oder nicht? Zum bereinigen der ressourcen, nicht. Ansonsten schon.)
    // getpid() = syscall(SYS_getpid)
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
            usleep(100000);
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
            usleep(100000);
            break;
        /*case 5:
            system("ps");
            releaseResources(prozesse);
            system("ps");
            break;*/
        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    releaseResources(prozesse);

    return 0;
}
