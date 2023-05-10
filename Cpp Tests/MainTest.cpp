#include <iostream>
#include <unistd.h>
#include <stdlib.h>


using namespace std;

int main()
{
	cout << "HalloWelt!" << endl;
	return 0;
}


// Die Methode createProcess() erzeugt ein Prozess.
void createProcess() {
	int rueckgabewert = fork();
	
	if (rueckgabewert == 0) {
		cout << "Kindprozess" << endl;
	}
	else if (rueckgabewert > 0) {
		cout << "Elternprozess" << endl;
	}
	else {
		cout << "Fehler" << endl;
	}


}

// Die Methode forkProcess() erstellt ein Duplikat von dem erstellten Prozess aus createProcess().
void forkProcess() {

}

// Die Methode execProcess() ersetzt den Prozess aus createProcess(). 
void execProcess() {

}

// Die Methode showProcessInformation() gibt die zentralen Informationen der erzeugten Prozesse aus.
void showProcessInformation() {

}

/*
Die Methode visualize() erstellt textuell eine Baumstruktur in der Konsole
und zeigt die Beziehung zwischen exec() und fork().
*/ 
void visualize() {

}

/*
Die Methode writeFile() erzeugt eine Datei
und speichert die zentralen Informationen der erzeugten Informationen aus der Methode showProcessInformation()
in einer maschinenlesbaren Sprache.
*/
void readFile() {

}


// Die Methode readFile() liest eine Datei ein und zeigt die geschriebenen Informationen in der Konsole aus.
void writeFile() {

}


/*
Die Methode collectGarbage() gibt die Betriebssystemressourcen frei vor beenden des Programms.
Sprich beendet die erstellten Prozesse und gibt somit die Ressourcen frei.
*/
void collectGarbage() {

}