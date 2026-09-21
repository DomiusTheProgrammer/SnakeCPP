#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <utility>

// Snake class - represents the snake entity in the game
class Snake
{
public:
     // Constructor - initialize snake at given position
     Snake(int startX, int startY);

     // Move snake in specified direction
     void move(int dx, int dy);
     // Grow snake by one segment
     void grow();

     // Getters for head position
     int getX() const;
     int getY() const;

     // Get all body segments
     const std::vector<std::pair<int,int>>& getBody() const;

private:
     int x, y;  // Head position
     std::vector<std::pair<int, int>> body;  // Body segments
     bool growing = false;  // Whether the snake should grow on the next move
};
#endif