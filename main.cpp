#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {
    int height, width, a = 1;
    char input;
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
            cout << "Enter height (min 10): \n";
            cin >> height;
            if (height < 10) height = 20;
            cout << "Enter width (min 10): \n";
            cin >> width;
            if (width < 10) width = 40;
            //argv add(height, width)
            server(argc, argv, height, width);
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