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

void start(int height, int width) {
    c.setSize(height, width);
    c.Run();
}

void readWriteServer() {
    //-------------- READ from client --------------
    n = read(newsockfd, buffer, 255);

    if (n < 0) {
        perror("Error reading from socket");
        return;
    } else {
        c.player1Function(buffer[0]);
    }
    char buffer4[] = c.player1GetParams();
    bzero(buffer, 256);

    for (int i = 0; i < 4; i++) {
        buffer[i] = buffer4[i];
    }

    //-------------- WRITE to client --------------
    n = write(newsockfd, buffer, strlen(buffer));
    const char *msg = buffer;
    n = write(newsockfd, msg, strlen(msg) + 1);
    if (n < 0) {
        perror("Error writing to socket");
        return;
    }
    bzero(buffer, 256);
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
    buffer[0] = 20;
    buffer[1] = 40;
    n = write(newsockfd, buffer, strlen(buffer));

    if (n < 0) {
        perror("Error writing to socket");
        return;
    }

    std::thread threadRead(&readWriteServer);
    std::thread threadGame(&start, (int) buffer[0], (int) buffer[1]);

    threadRead.join();
    threadGame.join();
    close(newsockfd);
    close(sockfd);
}