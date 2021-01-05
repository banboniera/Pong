#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "cGameManager.cpp"

class server {
private:
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];
    cGameManager *c;
public:

    ~server() {
        delete c;
    }

    void start(int width, int height) {
        c->setSize(height, width);
        c->Run();
    }

    void readWriteServer() {
        buffer[10] = 0;
        while (true) {
            //-------------- READ from client --------------
            n = read(newsockfd, buffer, 255);
            if ((int) buffer[1] == 1) {
                std::cout << "klient ukoncil hru\n";
                c->setQuit(true);
                return;
            } else if (n < 0) {
                c->setQuit(true);
                perror("Error reading from socket");
                return;
            } else {
                buffer[0] = (int) buffer[0] - 1;
                c->player2SetPosition((int) buffer[0]);
                if ((int) buffer[3] == 11 || (int) buffer[4] == 11) {
                    cout << "hra skoncila, ";
                    if ((int) buffer[3] == 11) cout << "hrac 1 vyhral so skore " << (int) buffer[3] << "\n";
                    else cout << "hrac 2 vyhral so skore " << (int) buffer[4] << "\n";
                    c->setQuit(true);
                    return;
                }
            }
            bzero(buffer, 256);
            //-------------- WRITE to client --------------
            std::this_thread::sleep_for(0.03s);
            c->player1GetParams(buffer);
            for (int i = 0; i < 5; i++) {
                buffer[i] = (int) buffer[i] + 1;
            }
            if (c->getQuit()) {
                std::cout << "ukoncili ste hru\n";
                buffer[5] = 1;
                n = write(newsockfd, buffer, strlen(buffer));
                return;
            } else {
                n = write(newsockfd, buffer, strlen(buffer));
            }
            if (n < 0) {
                perror("Error writing to socket");
                return;
            }
            bzero(buffer, 256);
        }
    }

    server(int argc, char *argv[], int height, int width) {
        if (argc < 2) {
            fprintf(stderr, "usage %s port\n", argv[0]);
            return;
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(atoi(argv[1]));
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            perror("Error creating socket");
            return;
        }

        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Error binding socket address");
            return;
        }
        listen(sockfd, 5);
        cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            return;
        }
        //write information about game
        bzero(buffer, 256);
        buffer[0] = width;
        buffer[1] = height;
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return;
        }

        c = new cGameManager(width, height);

        std::thread threadReadWrite(&server::readWriteServer, this);
        std::thread threadGame(&server::start, this, (int) buffer[0], (int) buffer[1]);
        std::cout << "startS" << "\n";
        threadReadWrite.join();
        threadGame.join();
        std::cout << "exitS" << "\n";
        close(newsockfd);
        close(sockfd);


    }
};