#include <iostream>
#include <cstdlib>
#include <vector>
#include "snake.h"
#include <conio.h>
#include <print>
#include <filesystem>
#include <fstream>
#include <string>
#include <ctime>

#ifndef WIN_32
#include <windows.h>
#endif

using namespace std;

const int sizeX = 40;
const int sizeY = 20;

int score = 0;
int highscore;
string playerName = "";
bool running;

// Encryption key for XOR cipher
const int ENCRYPTION_KEY = 0xABCD1234;

int foodX, foodY;
char world[sizeY][sizeX];

// predefiniton functions
void setHighscore();

void initStart()
{
    running = true;
    std::println("Welcome to Snake Game!");
    std::println("Current highscore: {} - {}", playerName.empty() ? "No record" : playerName, highscore);
    std::println("Use WASD keys to move the snake. Press any key to start...");
    std::cin.get(); // Wait for user input to start the game

    // create saves directory if it doesn't exist
    std::filesystem::path savesDir("saves");
    if (!std::filesystem::exists(savesDir))
    {
        std::filesystem::create_directory(savesDir);
    }

    // Initialize random seed for different food positions each run
    srand(time(nullptr));
}

// Initialize game world - fill with walls at borders, space insides
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

// The record holder and encrypted score are stored as one record.
int getHighscore()
{
    ifstream file("saves/save.dat");
    int encryptedScore = 0;
    if (!file || !std::getline(file, playerName) || !(file >> encryptedScore))
    {
        playerName.clear();
        return 0;
    }
    return encryptedScore ^ ENCRYPTION_KEY;
}

void setHighscore()
{
    ofstream file("saves/save.dat");
    if (file)
    {
        file << playerName << '\n'
             << (highscore ^ ENCRYPTION_KEY) << '\n';
    }
}

// Move console cursor back to top-left without clearing the whole screen
void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPosition);
    cout << std::flush; // Ensure the output buffer is flushed
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

void showGameOver()
{
    running = false;
    clearScreen();
    std::println("Game Over!");
    std::println("Score: {}", score);

    if (score > highscore)
    {
        std::print("New highscore! Enter your name: ");
        std::getline(std::cin >> std::ws, playerName);
        highscore = score;
        setHighscore();
    }

    std::println("Highscore: {} - {}", playerName.empty() ? "No record" : playerName, highscore);
    Sleep(2000);
}

void restartGame()
{
    std::println("Press any R for restart or Q to quit...");
    char choice = std::cin.get();
    if (choice == 'r' || choice == 'R')
    {
        score = 0;
        running = true;
    }
    else if (choice == 'q' || choice == 'Q')
    {
        running = false;
    }
    else
    {
        std::println("Invalid input. Exiting game.");
        running = false;
    }
}

int main()
{
    highscore = getHighscore();
    initStart();
    clearScreen();
    // Main game loop
    do
    {
        hideCursor();
        initWorld();
        Snake snake(sizeX / 2, sizeY / 2);

        // Initialize food position
        foodX = (sizeX - 2) / 2;
        foodY = (sizeY - 2) / 2 + 2;
        world[foodY][foodX] = '°'; // Draw food at initial position

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
            std::println("Score: {}", score);
            // Calculate new position
            int newX = snake.getX() + dx;
            int newY = snake.getY() + dy;

            // Wall collision
            if (newX <= 0 || newX >= sizeX - 1 || newY <= 0 || newY >= sizeY - 1)
            {
                showGameOver();
                restartGame();
                return 0;
            }

            // Self collision
            for (auto &p : snake.getBody())
            {
                if (p.first == newX && p.second == newY)
                {
                    showGameOver();
                    restartGame();
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
                showGameOver();
                restartGame();
                return 0;
            }
            // Draw head and food
            world[snake.getY()][snake.getX()] = 'O';
            world[foodY][foodX] = '*';

            clearScreen();
            drawWorld();
            Sleep(150);
        } // Game loop continues until collision
    } while (running);
    return 0;
}
