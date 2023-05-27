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

/*
Vereinfachte Version der exec Familie
Startet Programm mit angegebenen Pfad als String
*/
void exec(string path)
{
    const char *pathC = path.c_str();
    const char *args = NULL;
    execl(pathC, args, (char *)NULL);
}

void releaseResources(vector<pid_t> &prozesse)
{
    int status;
    for (pid_t pid : prozesse)
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid fehlgeschlagen");
        }
        else
        {
            if (WIFEXITED(status))
            {
                cout << "Kindprozess " << pid << " wurde beendet. Exit-Status: " << WEXITSTATUS(status) << endl;
            }
            else if (WIFSIGNALED(status))
            {
                cout << "Kindprozess " << pid << " wurde durch Signal beendet: " << WTERMSIG(status) << endl;
            }
        }
    }
}

/*
Clears the terminal and prints the menu
*/
void menu()
{
    system("clear");
    string optionen[] = {
        "Datum Ausgeben",
        "PIDs Ausgeben",
        "Beenden",
        "Hello World! mit fork() und exec()",
        "Clear"};
    int optionenSize = sizeof(optionen) / sizeof(string);

    for (int i = 0; i < optionenSize; i++)
    {
        cout << i + 1 << ". " << optionen[i] << endl;
    }
}

int input()
{
    int option;

    // Eingabe
    cin >> option;
    // Fehlereingabe abfangen
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        option = 0;
    }
    
    return option;
}

// main
int main()
{
    vector<pid_t> prozesse;
    // Fuegt Vaterprozess hinzu (sinnvoll, oder nicht? Zum bereinigen der ressourcen, nicht. Ansonsten schon.)
    // getpid() = syscall(SYS_getpid)
    // prozesse.push_back(getpid());

    bool running = true;
    int option;

    menu();

    while (running)
    {
        // Eingabe
        option = input();

        // Ausfuehrung der gewaehlten Option
        switch (option)
        {
        // Datum ausgeben
        case 1:
            if (addChild(prozesse))
            {
                date();
            }
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
            break;
        case 5:
            menu();
            break;
        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    releaseResources(prozesse);

    return 0;
}
