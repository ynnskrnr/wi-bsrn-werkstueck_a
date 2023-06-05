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

/*
Terminiert und loescht alle laufenden Prozesse
*/
void releaseResources(vector<pid_t> &prozesse)
{
    int status;
    sleep(1);
    for (pid_t pid : prozesse)
    {
        // Terminiert noch laufende Prozesse
        kill(pid, SIGTERM);
        // Prueft ob prozess bereits durchlaufen, oder terminiert wurde
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid fehlgeschlagen");
        }
        else
        {
            // Prozess ist durchlaufen
            if (WIFEXITED(status))
            {
                cout << "Kindprozess " << pid << " wurde beendet. Exit-Status: " << WEXITSTATUS(status) << endl;
            }
            // Prozess wurde terminiert 
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

void readFile()
{   // Eingabe des Dateinamen aus dem man auslesen möchte
    string filename;
    cout << "Bitte geben Sie den Dateinamen ein: ";
    cin >> filename;
    // Fehler abfangen wenn die Datei nicht existiert(oder andere Fehler)
    ifstream file(filename);
    if (!file)
    {
        cout << "Fehler beim Öffnen der Datei!" << endl;
        return;
    }
    // Auslesen der Datei Zeile für Zeile
    string line;
    while (getline(file, line))
    {
        cout << line << endl;
    }

    file.close();
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

    // Erstellen einer ergebnisse Textdatei und umlenken der Standardausgabe in die Textdatei
    FILE *outputFile = //fopen("ergebnisse.txt", "w");
    freopen("ergebnisse.txt", "w", stdout);

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
            readFile();
            break;
        case 6:
            menu();
            break;
        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    releaseResources(prozesse);

    // Schliessen der ergebnisse Textdatei
    fclose(outputFile);

    return 0;
}
