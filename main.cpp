#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {

    int height, width, maxScore, a = 1;
    char input;
    string nicknameServer;
    do {
        if (a == 1) {
            cout << "\n";
            a = 2;
        } else {
            system("clear");
        }
        cout << "1. Choose server (s) or client (c) \n";
        cout << "3. Quit game (q) \n";
        cin >> input;
        if (input == 's') {
            system("clear");
            if (argc < 2) {
                cout << "not enough arguments\n";
                return 1;
            }
            cout << "Enter nickname: \n";
            cin >> nicknameServer;
            if (nicknameServer == "") {
                nicknameServer = "Player1";
            }
            cout << "Enter height (min 10, default 20): \n";
            cin >> height;
            if (height < 10) height = 20;
            cout << "Enter width (min 10, default 40): \n";
            cin >> width;
            if (width < 10) width = 40;
            cout << "Enter max score (min 1, default 11): \n";
            cin >> maxScore;
          
            if (maxScore < 1) maxScore = 11;
            cout << "Wait for client to connect\n";
            server(argc, argv, width, height, nicknameServer, maxScore);
            a = 0;
        }
      
        if (input == 'c') {
            if (argc < 3) {
                cout << "not enough arguments\n";
                return 1;
            }
            client(argc, argv);
            a = 0;
        }
      
        if (input == 'q') {
            a = 0;
        }
    } while (a > 0);
}