#include <iostream>
#include <chrono>
#include <list>
#include <random>
#include <Windows.h>
using namespace std;

int ScreenWidth = 120;
int ScreenHeight = 50;

struct SSnakeSegment {
    int x;
    int y;
};

struct Food {
    int x;
    int y;
};

[[noreturn]] int main() {
    auto *screen = new wchar_t[ScreenWidth * ScreenHeight];
    const HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    while (true) {
        list<SSnakeSegment> snake;
        for (int i = 60; i <= 69; i++) {
            snake.push_back({i, 15});
        }

        Food food = {30, 15}; // Initialize food position
        int Score = 0;
        int SnakeDirection = 3;
        bool Dead = false;
        bool Key[4] = {false};

        while (!Dead) {
            std::random_device rd; // Seed
            std::mt19937 gen(rd()); // Mersenne Twister generator

            // Define distributions
            std::uniform_int_distribution<> distX(0, ScreenWidth - 1);
            std::uniform_int_distribution<> distY(3, ScreenHeight - 1);

            // timing & input
            // get input
            auto t1 = chrono::system_clock::now();
            while (chrono::system_clock::now() - t1 < ((SnakeDirection % 2) ? 150ms : 200ms)) {
                for (int k = 0; k < 4; k++)
                    Key[k] = (0x8000 & GetAsyncKeyState(static_cast<unsigned char>(R"('&%()"[k]))) != 0;
                if (Key[0] && SnakeDirection != 3) SnakeDirection = 1;
                if (Key[1] && SnakeDirection != 2) SnakeDirection = 0;
                if (Key[2] && SnakeDirection != 1) SnakeDirection = 3;
                if (Key[3] && SnakeDirection != 0) SnakeDirection = 2;
            }

            // game Logic
            // move snake
            switch (SnakeDirection) {
                case 0: snake.push_front({snake.front().x, snake.front().y - 1}); break; // Up
                case 1: snake.push_front({snake.front().x + 1, snake.front().y}); break; // Right
                case 2: snake.push_front({snake.front().x, snake.front().y + 1}); break; // Down
                case 3: snake.push_front({snake.front().x - 1, snake.front().y}); break; // Left
                default: break;
            }

            // collision with border
            if (snake.front().x < 0 || snake.front().x >= ScreenWidth || snake.front().y < 3 || snake.front().y >= ScreenHeight) Dead = true;

            // collision with food
            if (snake.front().x == food.x && snake.front().y == food.y) {
                Score++;
                do {
                    food.x = distX(gen);
                    food.y = distY(gen);
                } while (screen[food.y * ScreenWidth + food.x] != L' ');

                for (int i = 0; i < 5; i++) snake.push_back({snake.back().x, snake.back().y});
            }

            // collision with self
            for (auto it = next(snake.begin()); it != snake.end(); ++it) {
                if (it->x == snake.front().x && it->y == snake.front().y) Dead = true;
            }

            // remove tail
            snake.pop_back();

            // display to player
            // clear screen
            for (int i = 0; i < ScreenWidth * ScreenHeight; i++) screen[i] = L' ';
            // draw stats and borders
            for (int i = 0; i < ScreenWidth; i++) {
                screen[i] = L'-';
                screen[2 * ScreenWidth + i] = L'-';
                screen[i * ScreenWidth] = L'|';
                screen[i * ScreenWidth + ScreenWidth - 1] = L'|';
                screen[(ScreenHeight - 1) * ScreenWidth + i] = L'-';
            }
            swprintf(&screen[ScreenWidth + 5], L"Snake_Game ");
            swprintf(&screen[ScreenWidth + 15], L" Score: %d", Score);

            // draw snake body
            for (auto s : snake) screen[s.y * ScreenWidth + s.x] = Dead ? L'+' : L'O';
            // draw snake head
            screen[snake.front().y * ScreenWidth + snake.front().x] = Dead ? L'X' : L'@';

            // draw food
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            screen[food.y * ScreenWidth + food.x] = L'%';
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            if (Dead) swprintf(&screen[ScreenWidth * 15 + 40], L"Press 'R' to restart");

            // display frame
            WriteConsoleOutputCharacterW(hConsole, screen, ScreenWidth * ScreenHeight, {0, 0}, &dwBytesWritten);
        }
        while ((0x8000 & GetAsyncKeyState((unsigned char)'R')) == 0);
    }
}
