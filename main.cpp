#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {
    int a = 1;
    char input, height, width;
    do {
        if (a == 1) {
            cout << "\n";
            a = 2;
        } else {
            system("clear");
        }
        cout << "1. Choose server (s) or client (c) \n";
        cout << "2. Show best scores (b) \n";
        cout << "3. Quit game (q) \n";
        cin >> input;
        if (input == 's') {
            system("clear");
            cout << "Enter height: \n";
            cin >> height;
            cout << "Enter width: \n";
            cin >> width;
            //argv add(height, width)
            server(argc, argv);
            a = 0;
        }
        if (input == 'c') {
            client(argc, argv);
            a = 0;
        }
        if (input == 'b') {
            //showdcores();
        }
        if (input == 'q') {
            a = 0;
        }
    } while (a > 0);
}
