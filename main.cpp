#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {
    int height, width, a = 1;
    char input, nicknameServer;
    do {
        if (a == 1) {
            cout << "\n";
            a = 2;
        } else {
            system("clear");
        }
        cout << "1. Choose server (s) or client (c) \n";
        cout << "2. Show best time (b) \n";
        cout << "3. Quit game (q) \n";
        cin >> input;
        if (input == 's') {
            system("clear");
            cout << "Enter nickname: \n";
            cin >> nicknameServer;
            if(nicknameServer = '\0'){
                nicknameServer = 'P';
            }
            cout << "Enter height (min 10): \n";
            cin >> height;
            if (height < 10) height = 20;
            cout << "Enter width (min 10): \n";
            cin >> width;
            if (width < 10) width = 40;
            server(argc, argv, width, height, nicknameServer);
            a = 0;
        }
        if (input == 'c') {
            client(argc, argv);
            a = 0;
        }
        if (input == 'b') {
            //showBestTime();
        }
        if (input == 'q') {
            a = 0;
        }
    } while (a > 0);
}

static void showBestTime(){
    string line;
    ifstream readFile ("BestTime.txt");
    if (readFile.is_open())
    {
        for(int i = 0; i < 5; i++){
            getline (readFile,line);
            cout << "Best time: " <<  "\n";
            cout << i << ". " <<stoi(line) <<  "\n";
        }
        readFile.close();
    }
}