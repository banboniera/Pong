#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include "cGameManager.cpp"

using namespace std;

void writeTime() {
    string timeArray[10];
    ifstream readFile("BestTime.txt");
    if (readFile.is_open()) {
        for (int i = 0; i < 10; i++) {
            readFile >> timeArray[i];
        }
        readFile.close();
    } else cout << "Unable to open file";
    auto finish = chrono::high_resolution_clock::now();
    /* ------------------------------------------------
    chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time: " << elapsed.count() << " s\n";

    for(int i = 0; i < 5; i++){
        if(elapsed.count() < bestTime[i]){
            for(int j = i; j < 4; j++){
                bestTime[i  + 1] = bestTime[i];
            }
            bestTime[i] = elapsed.count();
            return;
        }
    }
    ------------------------------------------------ */
    ofstream writeFile("BestTime.txt");
    if (writeFile.is_open()) {
        int k = 0;
        for (int i = 0; i < 5; i++) {
            writeFile << timeArray[k] << " " << timeArray[k + 1] << "\n";
            k += 2;
        }
        writeFile.close();
    } else cout << "Unable to open file";
}

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

    void start() {
        c->Run();
    }

    void readWriteServer() {
        buffer[10] = 0;

        while (true) {
            //-------------- READ from client --------------
            n = read(newsockfd, buffer, 255);

            if ((int) buffer[1] == 1) {
                cout << "klient ukoncil hru\n";
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
            this_thread::sleep_for(0.03s);
            c->player1GetParams(buffer);

            for (int i = 0; i < 5; i++) {
                buffer[i] = (int) buffer[i] + 1;
            }
            if (c->getQuit()) {
                cout << "ukoncili ste hru\n";
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

    server(int argc, char *argv[], int width, int height, string nicknameServer) {
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
        buffer[2] = nicknameServer.length();
        for (int i = 0; i < nicknameServer.length(); i++) {
            buffer[3 + i] = nicknameServer[i];
        }
        n = write(newsockfd, buffer, strlen(buffer));

        if (n < 0) {
            perror("Error writing to socket");
            return;
        }
        c = new cGameManager(width, height);
        c->setServerNickname(nicknameServer);
        auto start = chrono::high_resolution_clock::now();
        thread threadReadWrite(&server::readWriteServer, this);
        thread threadGame(&server::start, this);
        threadReadWrite.join();
        threadGame.join();
        close(newsockfd);
        close(sockfd);
    }
};