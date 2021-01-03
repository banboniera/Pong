#include <sys/types.h>
#include "server.cpp"
#include "klient.cpp"

int main(int argc, char *argv[]){
    int height = 40, wigth = 20;
    std::cout << "test" << "\n";
    char X;
    std::cin >> X;
    if (X == 'c') {
        std::cout << "klient test" << "\n";
        klient(argc, argv);
    }
    if (X == 'h') {
        std::cout << "server test" << "\n";
        server(argc, argv);
    } else std::cout << "ZLE";
    /*cGameManager c(40, 20);
    c.Run();*/
    return 0;
}
