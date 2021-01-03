#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "cGameManager.cpp"

int sockfd, newsockfd;
socklen_t cli_len;
struct sockaddr_in serv_addr, cli_addr;
int n;
char buffer[256];
char buffer2[2];

void readClient() {
    do {
        std::cout << "testserver";
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
            return;
        }
        //printf("Here is the message: %s\n", buffer);
        std::cout << buffer << "\n";

        /*//const char *msg = "I got your message";
        const char *msg = buffer;
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return;
        }*/
    } while (*buffer != 'q');

    return;
}

void start() {
    cGameManager c(40, 20);
    c.Run();
    return;
}

//TODO
void writeClient() {
    const char *msg = buffer;
    n = write(newsockfd, msg, strlen(msg) + 1);
    if (n < 0) {
        perror("Error writing to socket");
        return;
    }
}

void server(int argc, char *argv[]) {
    std::cout << "test2" << "\n";
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return;
    }
    std::cout << "test3" << "\n";
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));
    std::cout << "test4" << "\n";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return;
    }
    std::cout << "test5" << "\n";
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return;
    }
    std::cout << "test6" << "\n";
    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);
    std::cout << "test7" << "\n";
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        return;
    }


    //write informations about game
    bzero(buffer2, 2);
    std::cout << "sizeof" << sizeof(buffer2) << "\n";
    const char *msg = buffer;
    buffer2[0] = 20;
    buffer2[1] = 40;
    n = write(newsockfd, buffer2, strlen(buffer2));
    if (n < 0) {
        perror("Error writing to socket");
        return;
    }
    std::cout << "test9" << "\n";

    std::thread threadRead(&readClient);
    std::thread threadGame(&start);

    threadRead.join();
    threadGame.join();
    std::cout << "test10" << "\n";

    close(newsockfd);
    close(sockfd);
    return;
}