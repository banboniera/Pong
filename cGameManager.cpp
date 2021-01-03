#include "cBall.cpp"
#include "cPaddle.cpp"
#include <time.h>
#include <curses.h>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <termios.h>

class cGameManager
{
private:
    int width, height;
    int score1, score2;
    char up1, down1, up2, down2;
    bool quit;
    cBall* ball;
    cPaddle* player1;
    cPaddle* player2;
    std::mutex mut;
    std::condition_variable cvBall;
    std::condition_variable cvPlayer;

    /*typedef struct playerData {
        pthread_mutex_t * mut;
        pthread_cond_t * condBall;
        pthread_cond_t * condPlayer;
    }PLAYER;

    typedef struct ballData{
        pthread_mutex_t * mut;
        pthread_cond_t * condBall;
        pthread_cond_t * condPlayer;
    }BALL;*/

public:
    cGameManager(int w, int h)
    {
        srand(time(NULL));
        quit = false;
        up1 = 'w'; up2 = 'i';
        down1 = 's'; down2 = 'k';
        score1 = score2 = 0;
        width = w; height = h;
        ball = new cBall(w / 2, h / 2);
        player1 = new cPaddle(1, h / 2 - 3);
        player2 = new cPaddle(w - 2, h / 2 - 3);
    }
    ~cGameManager()
    {
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
            perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
        return (buf);
    }

    void ScoreUp(cPaddle* player)
    {
        if (player == player1)
            score1++;
        else if (player == player2)
            score2++;

        ball->Reset();
        player1->Reset();
        player2->Reset();
    }
    void Draw()
    {
        system("clear");
        for (int i = 0; i < width + 2; i++)
            cout << "\xB2";
        cout << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
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
    void Input()
    {
        int ballx = ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player2x = player2->getX();
        int player1y = player1->getY();
        int player2y = player2->getY();

            char current = 'a';
            current = mygetch();
            if (current == up1)
                if (player1y > 0) {
                    player1->moveUp();
                    Draw();
                }
            if (current == up2)
                if (player2y > 0) {
                    player2->moveUp();
                    Draw();
                }
            if (current == down1)
                if (player1y + 4 < height) {
                    player1->moveDown();
                    Draw();
                }
            if (current == down2)
                if (player2y + 4 < height){
                    player2->moveDown();
                    Draw();
                }

            if (current == 'q')
                quit = true;
    }
    void Logic()
    {
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
                    ball->changeDirection((eDir)((rand() % 3) + 4));

        //right paddle
        for (int i = 0; i < 4; i++)
            if (ballx == player2x - 1)
                if (bally == player2y + i)
                    ball->changeDirection((eDir)((rand() % 3) + 1));

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

     void player1Function(std::mutex* mut, std::condition_variable* cvPlayer, std::condition_variable* cvBall) {
         std::cout << "test3";
        while (quit == false) {
            //(*cvBall).notify_one();
            //(*cvPlayer).wait(lock);
            //mut->lock();
            char current = mygetch();
            std::cout << "test4";
                if (current == up1)
                    if (player1->getY() > 0) {
                        player1->moveUp();
                        Draw();
                    }
                if (current == down1)
                    if (player1->getY() + 4 < height) {
                        player1->moveDown();
                        Draw();
                    }

                if (current == 'q')
                    quit = true;
                //mut->unlock();
            std::cout << "test5";
            /*
            if (serverChar == up1)
                if (player1->getY() > 0) {
                    player1->moveUp();
                    Draw();
                }
            if (serverChar == down1)
                if (player1->getY() + 4 < height) {
                    player1->moveDown();
                    Draw();
                }

            if (serverChar == 'q')
                quit = true;*/


        }
    }

    /*void player2Function(std::mutex* mut, std::condition_variable* cvPlayer, std::condition_variable* cvBall) {
        std::unique_lock<std::mutex> lock(*mut);
        while (quit == false) {
            (*cvBall).notify_one();
            (*cvPlayer).wait(lock);
            char current = 'a';
            cin >> current;
                if (current == up2)
                    if (player2->getY() > 0) {
                        player2->moveUp();
                        Draw();
                    }
                if (current == down2)
                    if (player2->getY() + 4 < height) {
                        player2->moveDown();
                        Draw();
                    }

                if (current == 'q')
                    quit = true;



        }
    }*/

    void ballFunction(std::mutex* mut, std::condition_variable* cvPlayer, std::condition_variable* cvBall) {
        std::cout << "test7";
        std::unique_lock<std::mutex> lock(*mut);
        while (quit == false) {
            //(*cvPlayer).notify_all();
            //mut->lock();
            std::cout << "test8";
            ball->Move();
            Draw();
            std::cout << "test9";
            //mut->unlock();
            //(*cvBall).wait(lock);
            if (ball->getDirection() == STOP)
                ball->randomDirection();
            Logic();
            std::cout << "test10";
            //std::this_thread::sleep_for(0.05s);
        }
    }

    void Run()
    {
        Draw();

        /*pthread_t threadPlayer1;
        pthread_t threadPlayer2;
        pthread_t threadBall;

        pthread_mutex_t  mut;
        pthread_cond_t condBall, condPlayer;

        pthread_mutex_init(&mut, NULL);
        pthread_cond_init(&condBall, NULL);
        pthread_cond_init(&condPlayer, NULL);

        PLAYER dataPlayer1 = {&mut, &condBall, &condPlayer};
        PLAYER dataPlayer2 = {&mut, &condBall, &condPlayer};
        BALL dataBall = {&mut, &condBall, &condPlayer};

        pthread_create(&threadPlayer1,NULL,&cGameManager::player1Function,&dataPlayer1);*/

        std::cout << "test1";
        std::thread threadPlayer1(&cGameManager::player1Function, this, &mut, &cvPlayer, &cvBall);
        std::cout << "test2";
        //std::thread threadPlayer2(&cGameManager::player2Function, this, &mut, &cvPlayer, &cvBall);
        std::thread threadBall(&cGameManager::ballFunction, this, &mut, &cvPlayer, &cvBall);



        threadBall.join();
        threadPlayer1.join();
        //threadPlayer2.join();

    }
};