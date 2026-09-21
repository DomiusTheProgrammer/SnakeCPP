#include "snake.h"
#include <conio.h>

// Constructor - add initial head segment
Snake::Snake(int startX, int startY) : x{startX}, y{startY} {
     body.push_back({startX, startY});
}

// Move snake: add new head, optionally remove tail
void Snake::move(int dx, int dy)
{
     int newX = body.back().first + dx;
     int newY = body.back().second + dy;

     body.push_back({newX, newY});
     if (!growing)
     {
         body.erase(body.begin());
     }
     else
     {
         growing = false;
     }
     x = newX;
     y = newY;
}

// Mark snake to grow on the next move
void Snake::grow()
{
     growing = true;
}

// Getter methods
const std::vector<std::pair<int, int>>& Snake::getBody() const {return body;}
int Snake::getX() const { return x; }
int Snake::getY() const { return y; }