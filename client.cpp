#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <termios.h>

using namespace std;

class client {
private:
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    string nicknameServer, nicknameClient;
    cGameManager *c;
public:
    ~client() {
        delete c;
    }

    void start() {
        c->player2Function();
    }

    void readWriteServer() {
        buffer[10] = 0;
        while (true) {
            //-------------- WRITE to server --------------
            bzero(buffer, 256);
            this_thread::sleep_for(0.03s);
            c->player2GetParams(buffer);
            buffer[0] = (int) buffer[0] + 1;

            if (c->getQuit()) {
                cout << "ukoncili ste hru\n";
                buffer[1] = 1;
                n = write(sockfd, buffer, strlen(buffer));
                return;
            } else {
                n = write(sockfd, buffer, strlen(buffer));
            }
            if (n < 0) {
                perror("Error writing to socket");
                return;
            }
            //-------------- READ from server --------------
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);

            for (int i = 0; i < 5; i++) {
                buffer[i] = (int) buffer[i] - 1;
            }
            if ((int) buffer[5] == 1) {
                cout << "server ukoncil hru\n";
                c->setQuit(true);
                return;
            } else if (n < 0) {
                perror("Error reading from socket");
                return;
            } else {
                if ((((int) buffer[3]) > c->getScore1()) || (((int) buffer[4]) > c->getScore2())) {
                    c->resetPlayer2();
                }
                c->player1SetPosition((int) buffer[0], (int) buffer[1], (int) buffer[2], (int) buffer[3],
                                      (int) buffer[4]);
                if ((int) buffer[3] == 11 || (int) buffer[4] == 11) {
                    cout << "hra skoncila, ";
                    if ((int) buffer[3] == 11) cout << "hrac 1 vyhral so skore " << (int) buffer[3] << "\n";
                    else cout << "hrac 2 vyhral so skore " << (int) buffer[4] << "\n";
                    c->setQuit(true);
                    return;
                }
            }
        }
    }

    client(int argc, char *argv[]) {
        if (argc < 3) {
            fprintf(stderr, "usage %s hostname port\n", argv[0]);
            return;
        }
        server = gethostbyname(argv[1]);

        if (server == NULL) {
            fprintf(stderr, "Error, no such host\n");
            return;
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(
                (char *) server->h_addr,
                (char *) &serv_addr.sin_addr.s_addr,
                server->h_length
        );
        serv_addr.sin_port = htons(atoi(argv[2]));
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            perror("Error creating socket");
            return;
        }
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Error connecting to socket");
            return;
        }
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
            return;
        }

        c = new cGameManager((int) buffer[0], (int) buffer[1]);
        nicknameServer = buffer;
        nicknameServer = nicknameServer.substr(3, (int)buffer[2]);
        c->setServerNickname(nicknameServer);
        cout << "Enter nickname: \n";
        cin >> nicknameClient;
        if (nicknameClient == "") nicknameClient = "Player2";
        c->setClientNickname(nicknameClient);

        buffer[0] = nicknameClient.length();
        for (int i = 0; i < nicknameClient.length(); i++) {
            buffer[1 + i] = nicknameClient[i];
        }
        n = write(sockfd, buffer, strlen(buffer));

        thread threadGame(&client::start, this);
        thread threadReadWrite(&client::readWriteServer, this);
        threadGame.join();
        threadReadWrite.join();
        close(sockfd);
    }
};