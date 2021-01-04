#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "cGameManager.cpp"

int sockfd, newsockfd;
socklen_t cli_len;
struct sockaddr_in serv_addr, cli_addr;
int n;
char buffer[256];
cGameManager c(40, 20);
bool connectionTerminated = false, gameStart = true;

void start(int height, int width) {
    c.setSize(height, width);
    c.Run();
}
void readWriteServer() {
    buffer[10] = 0;
    while (true) {
        //-------------- READ from client --------------
        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
            return;
        } else if ((int) buffer[10] == 1) {
            std::cout << "klient ukoncil hru\n";
            return;
        } else {
            c.player2SetPosition((int) buffer[0]);
        }
        bzero(buffer, 256);
        //-------------- WRITE to client --------------
        c.player1GetParams(buffer);
        if (c.getQuit()) {
            std::cout << "ukoncili ste hru\n";
            buffer[10] = 1;
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

void server(int argc, char *argv[]) {
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
    //TODO let player chose
    buffer[0] = 20;
    buffer[1] = 40;
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
        return;
    }

    std::thread threadReadWrite(&readWriteServer);
    std::thread threadGame(&start, (int) buffer[0], (int) buffer[1]);
    std::cout << "startS"  << "\n";
    threadReadWrite.join();
    threadGame.join();
    std::cout << "exitS"  << "\n";
    close(newsockfd);
    close(sockfd);
}