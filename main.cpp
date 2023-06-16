#include <iostream>
#include <vector>
// fork/exec
#include <unistd.h>
// Fuer abfang von ehlereingaben
#include <limits>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <fstream>
// string to array
#include <sstream>

using namespace std;
const string LOG_FOLDER = "processInfoLog";
const string OPTION_FOLDER = "options";
vector<pid_t> prozesse;

/**
 *  @brief Erzeugt parallele laufenden Kinderprozess und fügt diesen in die prozesse Liste ein
 *  @return  1 = Child, 0 = Parent.
 */
int child()
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
 */
void releaseResources()
{
    int status;
    sleep(1);
    for (pid_t pid : prozesse)
    {
        // Terminiert noch laufende Prozesse
        kill(pid, SIGTERM);
        // Prueft ob prozess bereits durchlaufen bzw. terminiert wurde
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
    DATUM_AUSGEBN = 1,
    PROCESS_INFO = 2,
    HALLO_WELT_AUSGEBEN = 3,
    ELTERN_KIND_PROZESS = 4,
    DATEI_LESEN = 5,
    CLEAR = 6,
    BEENDEN = 7,
    OPTIONEN_COUNT
};

string OptionenToString(Optionen option)
{
    switch (option)
    {
    case DATUM_AUSGEBN:
        return "Datum Ausgeben";
    case PROCESS_INFO:
        return "Prozessinformationen ausgeben";
    case HALLO_WELT_AUSGEBEN:
        return "\"Hello World!\" ausgeben";
    case ELTERN_KIND_PROZESS:
        return "Prozesshyrachie anzeigen";
    case DATEI_LESEN:
        return "Logdatei lesen";
    case CLEAR:
        return "Konsole bereinigen";
    case BEENDEN:
        return "Beenden und Systemressourcen freigeben";
    }
    return "";
}

/**
 *  @brief Cleart das Terminal und gibt das Menue aus
 */
void menu()
{
    system("clear");
    for (int i = 1; i <= OPTIONEN_COUNT - 1; ++i)
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
 *  @param path  Pfad von der zu lesenden Datei
 *  @return  Inhalt der Datei
 */
string readFile(string path)
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
 *  @return  Datum und Uhrzeit
 */
string dateTime()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string date = to_string(ltm->tm_mday) + "." + to_string(1 + ltm->tm_mon) + "." + to_string(1900 + ltm->tm_year);
    string time = to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
    return date + "_" + time;
}

/**
 *  @brief Schreibt inhalt in eine Datei
 *  @param text  Text der in die Datei geschrieben werden soll
 *  @param path  Pfad von der zu schreibenden Datei. Leer lassen um eine log Datei zu erstellen
 *  @param mode  a = ios::app = append, else = write
 *  @return  1 = erfolgreich, 0 = fehler.
 */
int writeFile(string text = "", char mode = 'a', string path = "")
{
    if (path == "")
    {
        path = LOG_FOLDER + "/" + dateTime() + ".txt";
    }
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
 *  @brief Vereinfachte /proc/[pid]/.. Abfrage
 *  @param pid  /proc/#pid#/..
 *  @param info  /proc/../#info#
 *  @return  Ergebnis der Anfrage.
 */
string procReq(pid_t pid, string info)
{
    string path = "/proc/" + to_string(pid) + "/" + info;
    return readFile(path);
}

/**
 *  @brief Liest die Informationen aller Prozesse aus
 *  @return  Liste (44 Spalten) der Prozessinformationen von allen Prozessen.
 *  @returns 0 = PID, 1 = filename, 2 = state, 3 = PPID, 4 = GID, 5 = UID, ...
 *  @details https://linux.die.net/man/5/proc (/proc/[pid]/stat)
 */
vector<vector<string>> getStatData()
{
    vector<vector<string>> stats;
    string procPIDstat;
    for (pid_t pid : prozesse)
    {
        procPIDstat = procReq(pid, "stat"); // info von /proc/[pid]/stat

        // words = 44, da die request proc/pid/stat infos enthält
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
 *  @return  Liste (7 Spalten) der RAM-Nutzung von allen Prozessen.
 *  @returns 0 = size, 1 = resident, 2 = share, 3 = text, 4 = lib, 5 = data, 6 = dt
 */
vector<vector<string>> getStatmData()
{
    vector<vector<string>> statms;
    string procPIDstatm;
    for (pid_t pid : prozesse)
    {
        procPIDstatm = procReq(pid, "statm");

        int i = 0, words = 7;
        vector<string> statmArray(words);
        stringstream ssin(procPIDstatm);
        while (ssin.good() && i < words)
        {
            ssin >> statmArray[i];
            i++;
        }
        statms.push_back(statmArray);
    }
    return statms;
}

/**
 *  @brief Liest die Informationen ueber Speicheradresse und Rechte aller Prozesse aus
 *  @return  Liste (6 Spalten) der Speicheradresse und Rechte allen Prozessen.
 *  @returns 0 = Speicheradresse, 1 = Rechte, 2 = Offset, 3 = Device, 4 = Inode, 5 = Dateiname
 *  @details Rechte: r = read, w = write, x = execute, s = shared, p = private (copy on write)
 */
vector<vector<string>> getMapsData()
{
    vector<vector<string>> mapss;
    string procPIDmaps;
    for (pid_t pid : prozesse)
    {
        procPIDmaps = procReq(pid, "maps");

        int i = 0, words = 6;
        vector<string> mapsArray(words);
        stringstream ssin(procPIDmaps);
        while (ssin.good() && i < words)
        {
            ssin >> mapsArray[i];
            i++;
        }
        mapss.push_back(mapsArray);
    }
    return mapss;
}

/**
 *  @return  Alle Prozessinformationen von allen Prozessen als Tabelle.
 */
string processInfoToString()
{
    // Header
    string output = "----- Process info -----\t\t\t----- Memory Usage -----\nPID\tRechte\tUID\tGID\tName\t\tsize\tresident\tshare\ttext\tdata\n";
    vector<vector<string>> stats = getStatData();
    vector<vector<string>> mems = getStatmData();
    vector<vector<string>> maps = getMapsData();

    for (size_t i = 0; i < stats.size(); i++)
    {
        // Bei kurzem Namen Tab hinzufügen
        stats[i][1].length() < 8 ? stats[i][1] += "\t" : "";

        output += stats[i][0] + "\t" + maps[i][1] + "\t" + stats[i][5] + "\t" + stats[i][4] + "\t" + stats[i][1] + "\t" +
                  mems[i][0] + "\t" + mems[i][1] + "\t\t" + mems[i][2] + "\t" + mems[i][3] + "\t" + mems[i][5] + "\t";
        output += "\n";
    }
    return output;
}

/**
 *  @brief Zeig die Beziehung zwischen Vater- und Kindprozessen an
 */
void visualizeRelationship()
{
    pid_t pidVater = getpid();
    for (auto pid = prozesse.begin(); pid != prozesse.end(); ++pid)
    {
        if (pidVater == *pid)
        {
            std::cout << "Vater: " << pidVater << std::endl;
        }
        else
        {
            std::cout << "Kind: " << *pid << std::endl;
        }
    }
}

// main
int main()
{
    prozesse.push_back(getpid());

    bool running = true;
    int option, request = 0;
    string output, path;

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
        case DATUM_AUSGEBN:
            if (child())
            {
                date();
            }
            break;

        // Prozess infos ausgeben
        case PROCESS_INFO:
            output = processInfoToString();
            writeFile(output);
            cout << output << endl;
            break;

        // Hello World! mit fork() und exec()
        // So werden die einzelnen Funktionen des Programms aufgerufen (Dateinen sind in options/)
        case HALLO_WELT_AUSGEBEN:
            if (child())
            {
                exec("./" + OPTION_FOLDER + "/helloWorld");
            }
            break;

        case ELTERN_KIND_PROZESS:
            visualizeRelationship();
            break;

        // Lesen einer log Datei
        case DATEI_LESEN:
            cout << "Welche der folgenden Dateien soll eingelesen werden?\n";
            if (child())
            {
                execlp("ls", "ls", LOG_FOLDER.c_str(), NULL);
            }
            cin >> path;
            cout << readFile(LOG_FOLDER + "/" + path);
            path = "";
            break;

        // Menue Clearen
        case CLEAR:
            menu();
            break;
        // Prgramm beenden
        case BEENDEN:
            cout << "Beenden" << endl;
            running = false;
            break;

        // Falsche eingabe
        default:
            cout << "Falsche Eingabe" << endl;
            break;
        }
    }

    releaseResources();

    return 0;
}
