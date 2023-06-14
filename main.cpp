#include <iostream>
#include <vector>
// fork/exec
#include <unistd.h>
// Fuer abfang von fehlereingaben
#include <limits>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <fstream>
// string to array
#include <sstream>

using namespace std;

/**
 *  @brief Erzeugt parallele laufenden Kinderprozess
 *  @param prozesse  Prozesse vector in den der Kinderprozess eingefuegt wird
 *  @return  1 = Child, 0 = Parent.
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
 */
void date()
{
    execl("/bin/date", "date", "-u", NULL);
}

/**
 *  @brief Vereinfachte Version der exec Familie. Startet Datei
 *  @param path  Pfad von der zu startenden Datei
 */
void exec(const char *path)
{
    const char *args = NULL;
    execl(path, args, (char *)NULL);
}

/**
 *  @brief Vereinfachte Version der exec Familie. Startet Datei
 *  @param path  Pfad von der zu startenden Datei
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

enum Optionen
{
    Datum_Ausgeben = 1,
    Process_infos = 2,
    Hallo_Welt_Ausgeben = 3,
    Readfile = 4,
    Clear = 5,
    Beenden = 6,
    Optionen_count
};
const char *OptionenToString(Optionen option)
{
    switch (option)
    {
    case Datum_Ausgeben:
        return "Datum Ausgeben";
    case Process_infos:
        return "Prozessinformationen_Ausgeben";
    case Hallo_Welt_Ausgeben:
        return "Hallo_Welt Ausgeben";
    case Readfile:
        return "Readfile";
    case Clear:
        return "Clear";
    case Beenden:
        return "Beenden und Systemressourcen Freigeben";
    }
    return "";
}
/**
 *  @brief Cleart das Terminal und gibt das Menue aus
 */
void menu()
{
    system("clear");
    for (int i = 1; i <= Optionen_count - 1; ++i)
    {
        cout << i << ". " << OptionenToString(static_cast<Optionen>(i)) << endl;
    }
}
/**
 *  @brief Fragt an welches Funktion des Programms gestartet werden soll
 *  @return  Eingegebene Option.
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
 */
string procReq(pid_t pid, string info)
{
    string path = "/proc/" + to_string(pid) + "/" + info;
    return readFile(path);
}

/**
 *  @brief Liest die Informationen aller Prozesse aus
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Liste (44 Spalten) der Prozessinformationen von allen Prozessen.
 *  @returns 0 = PID, 1 = filename, 2 = state, 3 = PPID, 4 = GID, 5 = UID, ...
 *  @details https://linux.die.net/man/5/proc (/proc/[pid]/stat)
 */
vector<vector<string> > getStatData(vector<pid_t> *prozesse)
{
    vector<vector<string> > stats;
    string procPIDstat;
    for (pid_t pid : *prozesse)
    {
        procPIDstat = procReq(pid, "stat"); // info von /proc/[pid]/stat

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
 *  @brief Liest die Informationen ueber RAM-Nutzung aller Prozesse aus
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Liste (7 Spalten) der RAM-Nutzung von allen Prozessen.
 *  @returns 0 = size, 1 = resident, 2 = share, 3 = text, 4 = lib, 5 = data, 6 = dt
 */
vector<vector<string> > getStatmData(vector<pid_t> *prozesse)
{
    vector<vector<string> > stats;
    string procPIDstat;
    for (pid_t pid : *prozesse)
    {
        procPIDstat = procReq(pid, "statm");

        int i = 0, words = 7;
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
 *  @brief Liest die Informationen ueber Speicheradresse und Rechte aller Prozesse aus
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Liste (6 Spalten) der Speicheradresse und Rechte allen Prozessen.
 *  @returns 0 = Speicheradresse, 1 = Rechte, 2 = Offset, 3 = Device, 4 = Inode, 5 = Dateiname
 *  @details Rechte: r = read, w = write, x = execute, s = shared, p = private (copy on write)
 */
vector<vector<string> > getMapsData(vector<pid_t> *prozesse)
{
    vector<vector<string> > stats;
    string procPIDstat;
    for (pid_t pid : *prozesse)
    {
        procPIDstat = procReq(pid, "maps");

        int i = 0, words = 6;
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
 *  @brief Alle Prozessinformationen als Tabelle
 *  @param prozesse  Liste der PIDs der Prozesse
 *  @return  Alle Prozessinformationen von allen Prozessen als Tabelle.
 */
string processInfoToString(vector<pid_t> *prozesse)
{
    // Header
    string output = "----- Process info -----\t\t\t----- Memory Usgae -----\nPID\tRechte\tUID\tGID\tName\t\tsize\tresident\tshare\ttext\tdata\n";
    vector<vector<string> > stats = getStatData(prozesse);
    vector<vector<string> > mems = getStatmData(prozesse);
    vector<vector<string> > maps = getMapsData(prozesse);

    for (size_t i = 0; i < stats.size(); i++)
    {
        stats[i][1].length() < 8 ? stats[i][1] += "\t" : "";
        output += stats[i][0] + "\t" + maps[i][1] + "\t" + stats[i][5] + "\t" + stats[i][4] + "\t" + stats[i][1] + "\t" +
                  mems[i][0] + "\t" + mems[i][1] + "\t\t" + mems[i][2] + "\t" + mems[i][3] + "\t" + mems[i][5] + "\t";
        output += "\n";
    }
    return output;
}

// main
int main()
{
    vector<pid_t> prozesse;
    prozesse.push_back(getpid());

    bool running = true;
    int option, request = 0;
    string output, path;

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
        case Datum_Ausgeben:
            if (addChild(prozesse))
            {
                date();
            }
            break;

        // Prozess infos ausgeben
        case Process_infos:
            output = processInfoToString(&prozesse);
            writeFile("Prozess Infos von Abfrage " + to_string(request) + ":\n" + output + "\n");
            cout << output << endl;
            break;

        // Hello World! mit fork() und exec()
        // So werden die einzelnen Funktionen des Programms aufgerufen (Dateinen sind in options/)
        case Hallo_Welt_Ausgeben:
            if (addChild(prozesse))
            {
                exec("./options/helloWorld");
            }
            break;

        // Eingabe des Dateinamen aus dem man auslesen möchte
        // cout << "Bitte geben Sie den Dateinamen ein: ";
        case Readfile:
            cout << readFile();
            break;

        // Menue Clearen
        case Clear:
            menu();
            break;
        // Prgramm beenden
        case Beenden:
            cout << "Beenden" << endl;
            running = false;
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
