#include <iostream>
#include <cstdlib>
#include <vector>
#include <windows.h>
#include "snake.h"
#include <conio.h>
#include <print>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

const int sizeX = 40;
const int sizeY = 20;

int score = 0;
int highscore;
string playerName = "";

// Encryption key for XOR cipher
const int ENCRYPTION_KEY = 0xABCD1234;

int foodX, foodY;
char world[sizeY][sizeX];

// Initialize game world - fill with walls at borders, space inside
void initWorld()
{
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            if (x == 0 || x == sizeX - 1 || y == 0 || y == sizeY - 1)
                world[y][x] = '#';
            else
                world[y][x] = ' ';
        }
    }
}

// Display the game world to console
void drawWorld()
{
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            cout << world[y][x];
        }
        cout << '\n';
    }
}

// Save highscore to file (encrypted)
void save()
{
    try
    {
        ofstream file("save.save");
        if (file.is_open())
        {
            file << playerName << '\n' << highscore << '\n';
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// Load highscore from file (encrypted)
int load()
{
    try
    {
        ifstream file("save.save");
        int score = 0;
        if (file.is_open())
        {
            getline(file, playerName);
            file >> highscore;
        }
        return score;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// Move console cursor back to top-left without clearing the whole screen
void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPosition);
}

// Hide the blinking cursor while the game runs
void hideCursor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(hConsole, &cursorInfo))
    {
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
}

int main()
{
    // Initialize random seed for different food positions each run
    srand(time(nullptr));

    // Load highscore from file at start
    highscore = getHighscore();

    hideCursor();
    initWorld();
    Snake snake(sizeX / 2, sizeY / 2);

    // Initialize food position
    foodX = (sizeX - 2) / 2;
    foodY = (sizeY - 2) / 2 + 2;
    world[foodY][foodX] = '°'; // Zeichne Essen beim Start

    // Direction vector
    int dx = 1, dy = 0;

    world[snake.getY()][snake.getX()] = 'O';

    // Game loop
    while (true)
    {
        // Handle keyboard input
        if (_kbhit())
        {
            char key = _getch();
            if (key == 'w')
            {
                dx = 0;
                dy = -1;
            }
            if (key == 's')
            {
                dx = 0;
                dy = 1;
            }
            if (key == 'a')
            {
                dx = -1;
                dy = 0;
            }
            if (key == 'd')
            {
                dx = 1;
                dy = 0;
            }
        }
        println("Score: {}", score);
        // Calculate new position
        int newX = snake.getX() + dx;
        int newY = snake.getY() + dy;

        // Wall collision
        if (newX <= 0 || newX >= sizeX - 1 || newY <= 0 || newY >= sizeY - 1)
        {
            // Save highscore if current score is higher
            if (score > highscore)
            {
                highscore = score;
                setHighscore();
            }
            clearScreen();
            println("Game Over!");
            println("Score: {}", score);
            println("Highscore: {}", highscore);
            println("Geben Sie ihren Namen ein!: ");
            cin >> playerName;
            save();
            Sleep(2000);
            return 0;
        }

        // Self collision
        for (auto &p : snake.getBody())
        {
            if (p.first == newX && p.second == newY)
            {
                // Save highscore if current score is higher
                if (score > highscore)
                {
                    highscore = score;
                    setHighscore();
                }
                clearScreen();
                println("Game Over!");
                println("Score: {}", score);
                println("Highscore: {}", highscore);
                Sleep(2000);
                return 0;
            }
        }

        // Food eaten
        if (newX == foodX && newY == foodY)
        {
            snake.grow();
            score += 10;
            world[foodY][foodX] = ' '; // Alte Essen-Position löschen
            foodX = rand() % (sizeX - 2) + 1;
            foodY = rand() % (sizeY - 2) + 1;
            world[foodY][foodX] = '*'; // Neues Essen zeichnen
        }

        // Update world and render frame
        initWorld();
        snake.move(dx, dy);

        // Draw snake
        for (auto &p : snake.getBody())
        {
            world[p.second][p.first] = 'o';
        }
        if (snake.getX() <= 0 || snake.getX() >= sizeX - 1 || snake.getY() <= 0 || snake.getY() >= sizeY - 1)
        {
            // Save highscore if current score is higher
            if (score > highscore)
            {
                highscore = score;
                setHighscore();
            }
            clearScreen();
            println("Game Over!");
            println("Score: {}", score);
            println("Highscore: {}", highscore);
            Sleep(2000);
            return 0;
        }
        // Draw head and food
        world[snake.getY()][snake.getX()] = 'O';
        world[foodY][foodX] = '*';

        clearScreen();
        drawWorld();
        Sleep(150);
    }
    return 0;
}

