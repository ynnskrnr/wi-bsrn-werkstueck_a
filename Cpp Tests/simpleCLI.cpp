// include io tools
#include <iostream>

using namespace std;

// func
int fehlversucheBeiEingabe(int &counter, int max = 5){

    cout << "Bitte geben Sie 'true' ein: " << endl;
    string eingabe;
    cin >> eingabe;

    if(eingabe == "true"){
        return 1;
    }
    else{    
        if(counter == max){
            return 0;
        }

        counter++;
        system("clear");
        return fehlversucheBeiEingabe(counter);
    }
}

int main(){
    int counter = 0;
    system("clear");

    if (fehlversucheBeiEingabe(counter)){
        cout << "Fehlversuche: " << counter << endl;
    } else {
        cout << "Zu viele Fehlversuche!" << endl;
    }
    return 0;
}
