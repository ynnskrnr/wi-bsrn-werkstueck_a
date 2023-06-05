#include <iostream>
#include <vector>
// fork/exec
#include <unistd.h>
// Fuer abfang von fehlereingaben
#include <limits>
#include <sys/wait.h>
// include syscalls
#include <sys/syscall.h>
#include <fstream>
// string to array
#include <sstream>

using namespace std;

/**
 *  @brief Erzeugt Prozess
 *  @param prozesse  Fuegt Kinderprozess in den Prozesse vector ein
 *  @return  Gibt 1 zuerueck wenn der laufende Prozess der Kinderprozess ist.
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

/**
 *  @brief Aktueller Prozess wird durch date ersetzt
 *
 */
void date()
{
    execl("/bin/date", "date", "-u", NULL);
}

/**
 *  @brief Vereinfachte Version der exec Familie. Startet Datei
 *  @param path  Pfad von der zu startenden Datei
 *
 */
void exec(const char *path)
{
    const char *args = NULL;
    execl(path, args, (char *)NULL);
}

/**
 *  @brief Vereinfachte Version der exec Familie. Startet Datei
 *  @param path  Pfad von der zu startenden Datei
 *
 */
void exec(string path)
{
    const char *pathC = path.c_str();
    const char *args = NULL;
    execl(pathC, args, (char *)NULL);
}

/**
 *  @brief Terminiert und loescht alle laufenden Prozesse
 *  @param prozesse  Vector mit allen Prozessen
 *
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

/**
 *  @brief Cleart das Terminal und gibt das Menue aus
 *
 */
void menu()
{
    system("clear");
    string optionen[] = {
        "Datum Ausgeben",
        "PIDs Ausgeben",
        "Beenden",
        "Hello World! mit fork() und exec()",
        "ReadFile",
        "Clear"};
    int optionenSize = sizeof(optionen) / sizeof(string);

    for (int i = 0; i < optionenSize; i++)
    {
        cout << i + 1 << ". " << optionen[i] << endl;
    }
}

/**
 *  @brief Fragt an welches Funktion des Programms gestartet werden soll
 *  @return  Eingegebene Option.
 *
 */
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

/**
 *  @brief Liest inhalt einer Datei aus
 *  @param path  Pfad von der zu lesenden Datei
 *  @return  Inhalt der Datei
 *
 */
string readFile(string path = "ergebnisse.txt")
{
    ifstream file(path);
    // Fehler abfangen wenn die Datei nicht existiert(oder andere Fehler)
    if (!file)
    {
        return "Fehler beim Öffnen der Datei!\n";
    }
    // Auslesen der Datei Zeile für Zeile
    string line, data = "";
    while (getline(file, line))
    {
        data += line + "\n";
    }
    file.close();
    return data;
}

/**
 *  @brief Schreibt inhalt in eine Datei
 *  @param text  Text der in die Datei geschrieben werden soll
 *  @param path  Pfad von der zu schreibenden Datei
 *  @param mode  a = ios::app = append, else = write
 *  @return  1 = erfolgreich, 0 = fehler.
 *
 */
int writeFile(string text = "", char mode = 'a', string path = "ergebnisse.txt")
{
    ofstream f;
    try
    {
        if (mode == 'a')
        {
            f.open(path, ios::app);
        }
        else if (mode == 'w')
        {
            f.open(path);
        }
        else
        {
            return 0;
        }
        f << text;
        f.close();
        return 1;
    }
    catch (exception e)
    {
        return 0;
    }
}

/**
 *  @brief Vereinfachte /proc/[pid]/ Abfrage
 *  @param pid  PID des Prozesses
 *  @param info  /proc/[pid]/...info
 *  @return  Ergebnis der Anfrage.
 *
 */
string procReq(pid_t pid, string info)
{
    string path = "/proc/" + to_string(pid) + "/" + info;
    return readFile(path);
}

/**
 *  @brief Liest die Informationen aller Prozesse aus
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Liste der Prozessinformationen von allen Prozessen.
 *
 */
vector<vector<string>> processData(vector<pid_t> *prozesse)
{
    vector<vector<string>> stats;
    string procPIDstat;
    for (pid_t pid : *prozesse)
    {
        procPIDstat = procReq(pid, "stat");

        int i = 0, words = 44;
        vector<string> statArray(words);
        stringstream ssin(procPIDstat);
        while (ssin.good() && i < words)
        {
            ssin >> statArray[i];
            i++;
        }
        stats.push_back(statArray);
    }
    return stats;
}

/**
 *  @brief Prozessinformationen als Tabelle
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Prozessinformationen von allen Prozessen als Tabelle.
 *
 */
string processInfo(vector<pid_t> *prozesse)
{
    // Header
    string output = "PID ; Rechte ; UID ; GID ; RAM ; Name\n";
    vector<vector<string>> stats = processData(prozesse);

    for (vector<string> stat : stats)
    {
        // TODO ergaenzen/erweitern
        output += stat[0] + " ; " + "Rechte" + " ; " + " UID" + " ; " + " GID" + " ; " + stat[22] + " ; " + stat[1];
        output += "\n";
    }
    return output;
}

// main
int main()
{
    vector<pid_t> prozesse;
    // Fuegt Vaterprozess hinzu (sinnvoll, oder nicht? Zum bereinigen der ressourcen, nicht. Ansonsten schon.)
    // getpid() = syscall(SYS_getpid)
    // prozesse.push_back(getpid());

    bool running = true;
    int option, request = 0;
    string output, path;

    // Erstellen einer ergebnisse Textdatei und umlenken der Standardausgabe in die Textdatei
    // FILE *outputFile = fopen("ergebnisse.txt", "w");
    // freopen("ergebnisse.txt", "w", stdout);

    // Clear ergebnisse.txt
    writeFile("", 'w');

    menu();

    while (running)
    {
        request++;
        output = "";
        // Eingabe
        option = input();

        cout << request << ". "
             << "Request: " << endl;
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
                output += to_string(i) + ",\t";
            }
            output += "\n";
            writeFile("PIDs from request " + to_string(request) + ": " + output);
            cout << output << endl;
            cout << processInfo(&prozesse) << endl;

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
            // Eingabe des Dateinamen aus dem man auslesen möchte
            // cout << "Bitte geben Sie den Dateinamen ein: ";
            // cin >> path;
            // cout << readFile(path);
            cout << readFile();
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
    // fclose(outputFile);

    return 0;
}
