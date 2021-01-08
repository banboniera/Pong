#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace std;

class client {
private:
    int sockfd, n, maxScore;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    string nicknameServer, nicknameClient;
    cGameManager *c;
public:
    ~client() {
        delete c;
    }
    //zavola funkciu z hry
    void start() {
        c->player2Function();
    }
    //vymienanie informaci medzi hracmi
    void readWriteServer() {
        while (true) {
            //-------------- WRITE to server --------------
            //nacita svoje informacie do bufferu
            bzero(buffer, 256);
            this_thread::sleep_for(0.03s);
            c->player2GetParams(buffer);
            //upravy data, aby v array neboly nuly
            buffer[0] = (int) buffer[0] + 1;
            //skontroluje ci klient ukoncil hru a posle informacie serveru
            if (c->getQuit()) {
                cout << "\n";
                cout << "you closed the game\n";
                buffer[1] = 1;
                n = write(sockfd, buffer, strlen(buffer));
                return;
            } else {
                n = write(sockfd, buffer, strlen(buffer));
            }
            //kontrola problemu pri zapise
            if (n < 0) {
                perror("Error writing to socket");
                return;
            }
            //-------------- READ from server --------------
            //nacita informacie od servera
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            //odstrani upravu dat
            for (int i = 0; i < 5; i++) {
                buffer[i] = (int) buffer[i] - 1;
            }
            //skontroluje, ci server ukoncil hru alebo bol problem pri nacitani
            if ((int) buffer[5] == 1) {
                cout << "\n";
                cout << "server closed the game\n";
                c->setQuit(true);
                return;
            } else if (n < 0) {
                perror("Error reading from socket");
                return;
            } else {
                //skontroluje, ci nejaky hrac nedal bod, ak ano, resetuje svoje herne elementy
                if ((((int) buffer[3]) > c->getScore1()) || (((int) buffer[4]) > c->getScore2())) {
                    c->resetPlayer2();
                }
                //nastavy poziciu serverovych hernich elementov
                c->player1SetPosition((int) buffer[0], (int) buffer[1], (int) buffer[2], (int) buffer[3],
                                      (int) buffer[4]);
                //skontroluje, ci nejaky hrac nevyhral, ak ano, vypise kto a ukonci sa
                if ((int) buffer[3] == maxScore || (int) buffer[4] == maxScore) {
                    cout << "\n";
                    cout << "game finished, ";
                    if ((int) buffer[3] == maxScore) cout << nicknameServer << " won with score " << (int) buffer[3] << "." <<  nicknameClient  <<" lost with score " << (int) buffer[4] << "\n";
                    else cout << nicknameClient << " won with score " << (int) buffer[4] << "." <<  nicknameServer  <<" lost with score " << (int) buffer[3] << "\n";
                    c->setQuit(true);
                    return;
                }
            }
        }
    }
    /*
     * pripojenie na socket je skopirovane od vyučujuceho
     * konstruktor s funkciou klienta
     */
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
        //od servera ziska, do akeho skore sa hra
        this->maxScore = (int) buffer[2];
        //vytvory hru
        c = new cGameManager((int) buffer[0], (int) buffer[1]);
        //zada nicknames
        nicknameServer = buffer;
        nicknameServer = nicknameServer.substr(4, (int)buffer[3]);
        c->setServerNickname(nicknameServer);
        cout << "Enter nickname: \n";
        cin >> nicknameClient;
        if (nicknameClient == "") nicknameClient = "Player2";
        c->setClientNickname(nicknameClient);
        //posle svoj nickname serveru
        buffer[0] = nicknameClient.length();
        for (int i = 0; i < nicknameClient.length(); i++) {
            buffer[1 + i] = nicknameClient[i];
        }
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return;
        }
        //spusti vlakna
        thread threadGame(&client::start, this);
        thread threadReadWrite(&client::readWriteServer, this);
        threadGame.join();
        threadReadWrite.join();
        //ukonci socket
        close(sockfd);
    }
};