#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"
#include <string.h>
#include <vector>
#include <fstream>

void showBestTime() {
    string line;
    ifstream readFile("BestTime.txt", ifstream::in);
    cout << "test" << "\n";
    if (readFile.is_open()) {
        cout << "Best time: " << "\n";
        for (int i = 0; i < 5; i++) {
            getline(readFile, line);
            cout << i << ". " << line << "\n";
        }
        readFile.close();
    } else cout << "Unable to open file" << "\n";
}

int main(int argc, char *argv[]) {
    std::string input;
    std::vector<std::string> names;
    std::fstream readFile("time.txt", fstream::in);
    getline(readFile, input);
    cout << input << "\n";
    cout << "test1" << "\n";
    while (readFile >> input) {
        cout << "test3" << "\n";
        names.push_back(input);
        cout << input << "\n";
    }
    cout << "test2" << "\n";
    return 0;


    /*
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
        cout << "2. Show best time (b) \n";
        cout << "3. Quit game (q) \n";
        cin >> input;
        if (input == 's') {
            system("clear");
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
            if (maxScore < 1) width = 11;
            cout << "Wait for client to connect\n";
            server(argc, argv, width, height, nicknameServer, maxScore);
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
     */
}