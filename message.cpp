#include "message.h"
#include <iostream>
using namespace std;
void welcomeMessage(){
    cout << "***************************************\n"
            "** Welcome to the information server **\n"
            "***************************************" << endl;
}
void loginMessage(const char IP[], int port){
    cout << "*** User '(no name)' entered from " << IP << ":" << port << ". ***" << endl;
}
void logoutMessage(string name){
    cout << "*** User '" << name <<"' left. ***" << endl;
}
void yellMessage(const char name[], const char message[]){
    cout << "*** " << name << " yelled ***: " << message << endl;
}
void toldMessage(const char name[], const char message[]){
    cout << "*** " << name << " told you ***: " << message << endl;
}
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID){
    cout << "*** " << sendername << " " << "(#" << senderID <<  ") just piped '" << message <<" ' to " << receivername <<" (#" << receiverID << ") ***" << endl;
}
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID){
    cout << "*** " << receivername << " " << "(#" << receiverID <<  ") just received from '" << receivername <<" (#" << receiverID << ") " << "by '" << message << "' ***" << endl;
}
void nouserMessage(int ID){
    cout << "*** Error: user #" << ID << " does not exist yet. ***";
}
void nomessageMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " does not exist yet. ***" << endl;
}
void occuipiedMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " already exists. ***" << endl;
}