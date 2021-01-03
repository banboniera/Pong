#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <termios.h>

class klient {
private:
    int sockfd, n, width, height;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char buffer2[2];
public:

    /*
     * Funkcia prevzata z webu:
     * https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
     */
    char mygetch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
            perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
        return (buf);
    }

    void writeServer() {
        do {
            //printf("Please enter a message: ");
            bzero(buffer, 256);
            //fgets(buffer, 255, stdin);
            buffer[0] = mygetch();
            n = write(sockfd, buffer, strlen(buffer));

            if (n < 0) {
                perror("Error writing to socket");
                return;
            }

            /*bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return;
            }*/
        } while (*buffer != 'q');
    }

    void start(int height, int width){
        //cGameManager c(40, 20);
        //c.Run();
        std::cout << height << " " << width;
        return;
    }

    klient(int argc, char *argv[]) {

        std::cout << "1" << "\n";
        if (argc < 3) {
            fprintf(stderr, "usage %s hostname port\n", argv[0]);
            return;
        }
        std::cout << "2" << "\n";
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            fprintf(stderr, "Error, no such host\n");
            return;
        }
        std::cout << "3" << "\n";
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(
                (char *) server->h_addr,
                (char *) &serv_addr.sin_addr.s_addr,
                server->h_length
        );
        serv_addr.sin_port = htons(atoi(argv[2]));
        std::cout << "4" << "\n";
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Error creating socket");
            return;
        }
        std::cout << "5" << "\n";
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Error connecting to socket");
            return;
        }
        std::cout << "6" << "\n";
        std::cout << "7" << "\n";
        bzero(buffer2, 2);
        n = read(sockfd, buffer2, 2);
        if (n < 0) {
            perror("Error reading from socket");
            return;
        }
        std::cout << "8" << "\n";
        //std::thread threadRead(&klient::writeServer, this);
        std::thread threadGame(&klient::start, this, (int)buffer2[0], (int)buffer2[1]);

        threadGame.join();
        //printf("%s\n",buffer);
        close(sockfd);

        return;
    }
};