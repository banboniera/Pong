#include "cBall.cpp"
#include "cPaddle.cpp"
#include <sys/ioctl.h>
#include <time.h>
#include <curses.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <termios.h>
#include <unistd.h>

class cGameManager {
private:
    int width, height;
    int score1, score2;
    char up1, down1, up2, down2;
    bool quit;
    cBall *ball;
    cPaddle *player1;
    cPaddle *player2;
    std::mutex mut;
    std::condition_variable cvBall;
    std::condition_variable cvPlayer;

public:
    cGameManager(int w, int h) {
        srand(time(NULL));
        quit = false;
        up1 = 'w';
        up2 = 'i';
        down1 = 's';
        down2 = 'k';
        score1 = score2 = 0;
        width = w;
        height = h;
        ball = new cBall(w / 2, h / 2);
        player1 = new cPaddle(1, h / 2 - 3);
        player2 = new cPaddle(w - 2, h / 2 - 3);
    }

    ~cGameManager() {
        delete ball, player1, player2;
    }

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
            perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror("tcsetattr ~ICANON");
        return (buf);
    }

    /*
     * Funkcia prevzata z webu:
     * https://stackoverflow.com/questions/29335758/using-kbhit-and-getch-on-linux
     */
    bool kbhit()
    {
        termios term;
        tcgetattr(0, &term);

        termios term2 = term;
        term2.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &term2);

        int byteswaiting;
        ioctl(0, FIONREAD, &byteswaiting);

        tcsetattr(0, TCSANOW, &term);

        return byteswaiting > 0;
    }

    void ScoreUp(cPaddle *player) {
        if (player == player1)
            score1++;
        else if (player == player2)
            score2++;

        ball->Reset();
        player1->Reset();
    }

    void Draw() {
        system("clear");

        for (int i = 0; i < width + 2; i++)
            cout << "\xB2";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int ballx = ball->getX();
                int bally = ball->getY();
                int player1x = player1->getX();
                int player2x = player2->getX();
                int player1y = player1->getY();
                int player2y = player2->getY();

                if (j == 0)
                    cout << "\xB2";
                if (ballx == j && bally == i)
                    cout << "O"; //ball
                else if (player1x == j && player1y == i)
                    cout << "\xDB"; //player1
                else if (player2x == j && player2y == i)
                    cout << "\xDB"; //player2
                else if (player1x == j && player1y + 1 == i)
                    cout << "\xDB"; //player1
                else if (player1x == j && player1y + 2 == i)
                    cout << "\xDB"; //player1
                else if (player1x == j && player1y + 3 == i)
                    cout << "\xDB"; //player1
                else if (player2x == j && player2y + 1 == i)
                    cout << "\xDB"; //player1
                else if (player2x == j && player2y + 2 == i)
                    cout << "\xDB"; //player1
                else if (player2x == j && player2y + 3 == i)
                    cout << "\xDB"; //player1
                else
                    cout << " ";
                if (j == width - 1)
                    cout << "\xB2";
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++)
            cout << "\xB2";
        cout << endl;

        cout << "Score 1: " << score1 << endl << "Score 2: " << score2 << endl;
    }

    void Logic() {
        int ballx = ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player2x = player2->getX();
        int player1y = player1->getY();
        int player2y = player2->getY();

        //left paddle
        for (int i = 0; i < 4; i++)
            if (ballx == player1x + 1)
                if (bally == player1y + i)
                    ball->changeDirection((eDir) ((rand() % 3) + 4));

        //right paddle
        for (int i = 0; i < 4; i++)
            if (ballx == player2x - 1)
                if (bally == player2y + i)
                    ball->changeDirection((eDir) ((rand() % 3) + 1));

        //bottom wall
        if (bally == height - 1)
            ball->changeDirection(ball->getDirection() == DOWNRIGHT ? UPRIGHT : UPLEFT);
        //top wall
        if (bally == 0)
            ball->changeDirection(ball->getDirection() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);
        //right wall
        if (ballx == width - 1)
            ScoreUp(player1);
        //left wall
        if (ballx == 0)
            ScoreUp(player2);
    }

    void player1SetPosition(int player1Y, int ballX, int ballY, int score1, int score2) {
        this->player1->setY(player1Y);
        this->ball->setX(ballX);
        this->ball->setY(ballY);
        this->score1 = score1;
        this->score2 = score2;
        Draw();
    }

    void player2SetPosition(int posY) {
        this->player2->setY(posY);
        Draw();
    }

    void player1Function(std::mutex *mut, std::condition_variable *cvPlayer, std::condition_variable *cvBall) {
        //Draw();
        while (quit == false) {
            //(*cvBall).notify_one();
            //(*cvPlayer).wait(lock);
            //mut->lock();
            if (kbhit()) {
                char current = mygetch();
                if (current == up1)
                    if (player1->getY() > 0) {
                        player1->moveUp();
                        //Draw();
                    }
                if (current == down1)
                    if (player1->getY() + 4 < height) {
                        player1->moveDown();
                        //Draw();
                    }
                if (current == 'q')
                    quit = true;

                //mut->unlock();
            }
        }
        return;
    }

    void player1GetParams(char *buffer) {
        buffer[0] = player1->getY();
        buffer[1] = ball->getX();
        buffer[2] = ball->getY();
        buffer[3] = score1;
        buffer[4] = score2;
    }

    void player2GetParams(char *buffer) {
        buffer[0] = player2->getY();
    }

    void player2Function(/*std::mutex *mut, std::condition_variable *cvPlayer, std::condition_variable *cvBall*/) {
        //Draw();
        //std::unique_lock<std::mutex> lock(*mut);
        while (quit == false) {
            //(*cvBall).notify_one();
            //(*cvPlayer).wait(lock);
            if (kbhit()) {
                char current = mygetch();
                if (current == up2)
                    if (player2->getY() > 0) {
                        player2->moveUp();
                        //Draw();
                    }
                if (current == down2)
                    if (player2->getY() + 4 < height) {
                        player2->moveDown();
                        //Draw();
                    }
                if (current == 'q')
                    quit = true;
            }
        }
        return;
    }

    bool getQuit() {
        return this->quit;
    }

    void setQuit(bool quit) {
        this->quit = quit;
    }

    void ballFunction(std::mutex *mut, std::condition_variable *cvPlayer, std::condition_variable *cvBall) {
        std::unique_lock<std::mutex> lock(*mut);
        while (quit == false) {
            //(*cvPlayer).notify_all();
            //mut->lock();
            ball->Move();
            //Draw();
            //mut->unlock();
            //(*cvBall).wait(lock);
            if (ball->getDirection() == STOP)
                ball->randomDirection();
            Logic();
            std::this_thread::sleep_for(0.01s);
        }
        return;
    }

    void setSize(int height, int width) {
        this->height = height;
        this->width = width;
    }

    int getScore1() {
        return this->score1;
    }

    int getScore2() {
        return this->score2;
    }

    void resetPlayer2() {
        return player2->Reset();
    }

    void Run() {
        std::thread threadPlayer1(&cGameManager::player1Function, this, &mut, &cvPlayer, &cvBall);
        std::thread threadBall(&cGameManager::ballFunction, this, &mut, &cvPlayer, &cvBall);

        threadPlayer1.join();
        threadBall.join();
    }
};