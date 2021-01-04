#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {
    int a = 1;
    char input;
    do {
        std::cout << "Choose server (s) or client (c) or quit game (q)" << "\n";
        std::cin >> input;
        if (input == 'c') {
            client(argc, argv);
            a = 0;
        } else if (input == 's') {
            server(argc, argv);
            a = 0;
        }else if (input == 'q')
            a = 0;
    } while (a > 0);
}
