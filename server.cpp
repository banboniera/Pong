#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "cGameManager.cpp"

using namespace std;
//trieda pre serverovskeho hraca
class server {
private:
    int sockfd, newsockfd, n, maxScore;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];
    string nicknameClient, nicknameServer;
    cGameManager *c;
public:
    ~server() {
        delete c;
    }
    //zavola funkciu z hry
    void start() {
        c->Run();
    }
    //vymienanie informaci medzi hracmi
    void readWriteServer() {
        while (true) {
            //-------------- READ from client --------------
          
            //nacita informacie od klienta
            n = read(newsockfd, buffer, 255);
            //skontroluje ci klient ukoncil hru alebo nastala chyba pri citani
            if ((int) buffer[1] == 1) {
                cout << "\n";
                cout << "client ended the game\n";
                c->setQuit(true);
                return;
            } else if (n < 0) {
                c->setQuit(true);
                perror("Error reading from socket");
                return;
            } else {
                //odstrani upravu dat
                buffer[0] = (int) buffer[0] - 1;
                //nastavy data do hry
                c->player2SetPosition((int) buffer[0]);
            }
            bzero(buffer, 256);
            //-------------- WRITE to client --------------
          
            this_thread::sleep_for(0.03s);
            //ziska data zo svojej hry
            c->player1GetParams(buffer);
            //upravy data pre buffer
            for (int i = 0; i < 5; i++) {
                buffer[i] = (int) buffer[i] + 1;
            }
            //skontroluje, ci hrac ukoncil hru, a posle data klientovy
            if (c->getQuit()) {
                cout << "\n";
                cout << "you closed the game\n";
                buffer[5] = 1;
                n = write(newsockfd, buffer, strlen(buffer));
                return;
            } else {
                n = write(newsockfd, buffer, strlen(buffer));
            }
            //skontroluje ci nastal problem pri zapise
            if (n < 0) {
                perror("Error writing to socket");
                return;
            }
            //skontroluje, ci nejaky hrac vyhral, ak ano, vypise vyherne info a skonci
            if ((int) buffer[3] - 1 == this->maxScore || (int) buffer[4] - 1 == this->maxScore) {
                cout << "\n";
                cout << "game finished, ";
                if ((int) buffer[3] == maxScore) cout << nicknameServer << " won with score " << (int) buffer[3] << "." <<  nicknameClient  <<" lost with score " << (int) buffer[4] << "\n";
                else cout << nicknameClient << " won with score " << (int) buffer[4] << "." <<  nicknameServer  <<" lost with score " << (int) buffer[3] << "\n";
                c->setQuit(true);
                return;
            }
            bzero(buffer, 256);
        }
    }
    /*
     * vytvorenie socketu je skopirovane od vyučujuceho
     * konstruktor s funkciou servera
     */
    server(int argc, char *argv[], int width, int height, string nicknameServer, int maxScore) {
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
        //da zakladne informacie do bufferu
        this->maxScore = maxScore;
        bzero(buffer, 256);
        buffer[0] = width;
        buffer[1] = height;
        buffer[2] = maxScore;
        buffer[3] = nicknameServer.length();
      
        for (int i = 0; i < nicknameServer.length(); i++) {
            buffer[4 + i] = nicknameServer[i];
        }
        //posle zakladne informacie klientovy
        n = write(newsockfd, buffer, strlen(buffer));
        //skontroluje, ci bol problem pri posielani
        if (n < 0) {
            perror("Error writing to socket");
            return;
        }
        //nacita nick klienta
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        //skontroluje, ci bol problem pri citani
        if (n < 0) {
            perror("Error reading from socket");
            return;
        }
        //nastavy mena
        this->nicknameServer = nicknameServer;
        nicknameClient = buffer;
        nicknameClient = nicknameClient.substr(1, (int)buffer[0]);
        //zapne hru
        c = new cGameManager(width, height);
        c->setServerNickname(nicknameServer);
        c->setClientNickname(nicknameClient);
        //spusti vlakna
        thread threadReadWrite(&server::readWriteServer, this);
        thread threadGame(&server::start, this);
        threadReadWrite.join();
        threadGame.join();
        //ukonci sockety
        close(newsockfd);
        close(sockfd);
    }
};