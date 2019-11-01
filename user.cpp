#include "user.h"
int User::n = 0;
void User::Init(string IP1, int ID1, int port1) {
    ID = ID1;
    port = port1;
    IP = move(IP1);
    name = "(no name)";
    ++n;
}
void User::Delete() {
    ID = -1;
    --n;
}