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
        "ReadFile",
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
        data += line+"\n";
    }
    file.close();
    return data;
}

string getProcessInfo(pid_t pid, string info){
    string path = "/proc/" + to_string(pid) + "/" + info;
    return readFile(path);
}

int writeFile(string text, string path = "ergebnisse.txt", char mode = 'a'){
    ofstream f;
    try{       
        if (mode == 'a'){
            f.open(path, ios::app);
        }else{
            f.open(path);
        }
        f << text;
        f.close();
        return 1;
    }
    catch(exception e){
        return 0;
    }
}

string processData(vector<pid_t> *prozesse){
    string data, path;
    // Heaer                              mem: size, resident, share, text, lib, data, dt : in pages(4kb)
    cout <<  "PID\t" /*<< "Rechte\t" << "UID\t"*/ << "MEM\t" << endl;
    for(pid_t pid : *prozesse){
        data += to_string(pid) + "\t";
        // Rechte
        //data += getProcessInfo(pid, "maps");
        // UID
        //data += getProcessInfo(pid, "status");
        // MEM
        data += getProcessInfo(pid, "statm");
        data += "\n";
    }
    

    return data;
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
    //FILE *outputFile = fopen("ergebnisse.txt", "w");
    //freopen("ergebnisse.txt", "w", stdout);

    menu();

    while (running)
    {
        request++;
        output = "";
        // Eingabe
        option = input();

        cout << request << ". " << "Request: " << endl;
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
            cout << output;
            cout << processData(&prozesse);
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
            //cout << "Bitte geben Sie den Dateinamen ein: ";
            //cin >> path;
            //cout << readFile(path);
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
    //fclose(outputFile);

    return 0;
}
