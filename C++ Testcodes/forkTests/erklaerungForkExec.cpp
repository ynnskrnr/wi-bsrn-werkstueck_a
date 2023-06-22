#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int main(){

    // pid_t repraesentiert eine PID
    // Eine Kopie des aktuellen Processes/Codes wird erstellt und laeuft parallel
    pid_t i = fork(); // fork() gibt die PID des Kindprozesses zurueck
    // 0 = Kindprozess ; >0 = Elternprozess (i = PID vom Kinderprozess) ; <0 = Fehler

    // Bezeichnung der Hyrachie
    cout << endl << (i ? "Elternprozess:" : "Kindprozess:") << endl;

    if(!i){
        // Inhalt des Kinderprozesses wird zu einem anderen prozess
        execl("/bin/date", "date", "-u", NULL);
        // Ab hier wird nichts mehr ausgefuehrt (Im Kinderprozess), da jetzt ein anderer Code ausgefuehrt wird
    }

    // PID: Process ID
    cout << "PID: " << getpid() << endl;
    // PPID: Parent Process ID
    cout << "PPID: " << getppid() << endl;
    cout << "Child PID: " << i << endl;

	return 0;
}