#include <sys/types.h>
#include "server.cpp"
#include "client.cpp"

int main(int argc, char *argv[]) {
    int a = 1;
    char input;
    do {
        cin >> input;
        if (input == 'c') {
            client(argc, argv);
            a = 0;
        } else if (input == 's') {
            server(argc, argv);
            a = 0;
        }
    } while (a > 0);
}