#include <iostream>

using namespace std;

class cPaddle {
private:
    int x, y;
    int originalX, originalY;
public:
    cPaddle() {
        x = y = 0;
    }

    cPaddle(int posX, int posY) : cPaddle() {
        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
    }

    inline void Reset() {
        x = originalX;
        y = originalY;
    }

    inline int getX() { return x; }

    inline int getY() { return y; }

    inline void setX(int x) { this->x = x; }

    inline void setY(int y) { this->y = y; }

    inline void moveUp() { y--; }

    inline void moveDown() { y++; }

    friend ostream &operator<<(ostream &o, cPaddle c) {
        o << "Paddle [" << c.x << "," << c.y << "]";
        return o;
    }
};