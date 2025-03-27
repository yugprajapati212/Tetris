#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;
void red () {
    printf("\033[1;31m");
  }
  
  void yellow() {
    printf("\033[1;33m");
  }
  void green(){
      printf("\033[0;32m");
      }
  void cyan() {
    printf("\033[0;36m");
  }
  void reset () {
    printf("\033[0m");
  }
  void brightBlue() {
      printf("\033[1;94m");
  }
  void brightGreen() {
      printf("\033[1;92m");
  }
  void darkBlue() {
      printf("\033[0;34m");
  }
  void blink() {
      printf("\033[5m");
  }
  
  void bold() {
      printf("\033[1m");
  }
void printGameOver() {
    red();blink();
    cout << "  * * *     *     *     *   * * *      * * *  *   *  * * *   * * * " << endl;
    yellow();blink();
    cout << "  *        * *    *     *   *          *   *  *   *  *       *   *" << endl;
    green();blink();
    cout << "  *  *    * * *   * * * *   * * *      *   *  *   *  * * *   * *" << endl;
    cyan();blink();
    cout << "  *   *   *   *   *  *  *   *          *   *   * *   *       * *" << endl;
    darkBlue();blink();
    cout << "  * * *   *   *   *     *   * * *      * * *    *    * * *   *   *" << endl<<endl;
    reset();
}

void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPosition);
}

class Tetromino {
    public:
        vector<vector<int>> shape;
        char symbol;
        int color;
    
        // Default constructor
        Tetromino() : shape({{0}}), symbol(' '), color(15) {}
    
        // Parameterized constructor
        Tetromino(vector<vector<int>> s, char sym, int col) : shape(s), symbol(sym), color(col) {}
    };
    

class Game {
private:
    vector<vector<int>> board;
    Tetromino currentPiece;
    int pieceX, pieceY;
    int score;
    bool gameOver;
    int sleepDuration;
    HANDLE hConsole;

    void spawnPiece() {
        static vector<Tetromino> pieces = {
            Tetromino({{1}, {1}, {1}, {1}}, 'I', 11),
            Tetromino({{1,1,1}, {0,1,0}}, 'T', 13),
            Tetromino({{1,0,0}, {1,0,0}, {1,1,0}}, 'L', 9),
            Tetromino({{0,0,1}, {0,0,1}, {0,1,1}}, 'J', 12),
            Tetromino({{1,1}, {1,1}}, 'O', 14),
            Tetromino({{1,1,0}, {0,1,1}}, 'S', 10),
            Tetromino({{0,1,1}, {1,1,0}}, 'Z', 13)
        };

        currentPiece = pieces[rand() % pieces.size()];
        pieceX = WIDTH / 2 - currentPiece.shape[0].size() / 2;
        pieceY = 0;

        if (!isValidMove(0, 0)) {
            gameOver = true;
            Beep(500,400);
        }
    }

    bool isValidMove(int offsetX, int offsetY) {
        for (int y = 0; y < currentPiece.shape.size(); y++) {
            for (int x = 0; x < currentPiece.shape[y].size(); x++) {
                if (currentPiece.shape[y][x]) {
                    int newX = pieceX + x + offsetX;
                    int newY = pieceY + y + offsetY;
                    if (newX < 0 || newX >= WIDTH || newY >= HEIGHT) return false;
                    if (newY >= 0 && board[newY][newX]) return false;
                }
            }
        }
        return true;
    }

    void mergePiece() {
        for (int y = 0; y < currentPiece.shape.size(); y++) {
            for (int x = 0; x < currentPiece.shape[y].size(); x++) {
                if (currentPiece.shape[y][x]) {
                    board[pieceY + y][pieceX + x] = currentPiece.color;
                }
            }
        }
    }

    void clearLines() {
        for (int y = 0; y < HEIGHT; y++) {
            bool full = true;
            for (int x = 0; x < WIDTH; x++) {
                if (!board[y][x]) full = false;
            }
            if (full) {
                board.erase(board.begin() + y);
                board.insert(board.begin(), vector<int>(WIDTH, 0));
                Beep(500,400);
                score++;
                adjustSleepDuration();
            }
        }
    }

    void adjustSleepDuration() {
        if (sleepDuration > 100) sleepDuration -= 50;
    }
    void rotatePiece() {
        vector<vector<int>> rotatedPiece(currentPiece.shape[0].size(), vector<int>(currentPiece.shape.size()));
    
        for (int y = 0; y < currentPiece.shape.size(); y++) {
            for (int x = 0; x < currentPiece.shape[y].size(); x++) {
                rotatedPiece[x][currentPiece.shape.size() - 1 - y] = currentPiece.shape[y][x];
            }
        }
    
        // Store original piece in case rotation is invalid
        vector<vector<int>> originalShape = currentPiece.shape;
    
        // Apply rotation
        currentPiece.shape = rotatedPiece;
    
        // If rotation is invalid, revert to original shape
        if (!isValidMove(0, 0)) {
            currentPiece.shape = originalShape;
        }
    }

public:
    Game(int diff) : board(HEIGHT, vector<int>(WIDTH, 0)), score(0), gameOver(false), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {
        srand(time(0));
        spawnPiece();
        switch (diff) {
            case 1: sleepDuration = 500; break;
            case 2: sleepDuration = 300; break;
            case 3: sleepDuration = 100; break;
            default: sleepDuration = 500; break;
        }
    }

    void draw() {
        clearScreen();
        for (int y = 0; y < HEIGHT; y++) {
            cout << "|";
            for (int x = 0; x < WIDTH; x++) {
                bool isPiece = false;
                for (int py = 0; py < currentPiece.shape.size(); py++) {
                    for (int px = 0; px < currentPiece.shape[py].size(); px++) {
                        if (currentPiece.shape[py][px] && y == pieceY + py && x == pieceX + px) {
                            isPiece = true;
                        }
                    }
                }
                if (isPiece) {
                    SetConsoleTextAttribute(hConsole, currentPiece.color);
                    cout << "[]";
                } else if (board[y][x]) {
                    SetConsoleTextAttribute(hConsole, board[y][x]);
                    cout << "[]";
                } else {
                    cout << "  ";
                }
                SetConsoleTextAttribute(hConsole, 15);
            }
            cout << "|" << endl;
        }
        int level = (500 - sleepDuration) / 50 + 1;
        cout << "Score: " << score << endl;
        cout << "Level: " << level << endl;
        if (gameOver) printGameOver();
    }

    void update() {
        if (gameOver) return;
        if (!isValidMove(0, 1)) {
            mergePiece();
            clearLines();
            spawnPiece();
            return;
        }
        pieceY++;
    }

    void handleInput() {
        if (gameOver) return;
        //char input = _getch();
        switch (_getch()) {
            case 'a' : 
                if (isValidMove(-1, 0)) pieceX--;
                break;
            case 'd' : 
                if (isValidMove(1, 0)) pieceX++;
                break;
            case 's' : 
                if (isValidMove(0, 1)) pieceY++;
                break;
            case 'r':
                rotatePiece(); // Rotate the piece 90 degrees clockwise
                if (!isValidMove(0, 0)) {
                    // If the rotation is invalid, revert it
                    rotatePiece(); // Rotate back
                    rotatePiece(); // Rotate back
                    rotatePiece(); // Rotate back
                }
                break;
            case 75 : 
                if (isValidMove(-1, 0)) pieceX--;
                break;
            case 77 : 
                if (isValidMove(1, 0)) pieceX++;
                break;
            case 80 : 
                if (isValidMove(0, 1)) pieceY++;
                break;
            case 72:
                rotatePiece(); // Rotate the piece 90 degrees clockwise
                if (!isValidMove(0, 0)) {
                    // If the rotation is invalid, revert it
                    rotatePiece(); // Rotate back
                    rotatePiece(); // Rotate back
                    rotatePiece(); // Rotate back
                }
                break;
            case ' ':
                while (isValidMove(0, 1)) pieceY++;
                break;
            case 27:
                gameOver = true;
                break;
        }
    }

    bool isGameOver() { return gameOver; }
    int getSleepDuration() { return sleepDuration; }
};

int main() {
    int difficulty;
    cout << "Select difficulty level (1: Easy, 2: Medium, 3: Hard): ";
    cin >> difficulty;
    if (difficulty < 1 || difficulty > 3) {
        cout << "Invalid difficulty level. Defaulting to Easy." << endl;
        difficulty = 1;
        
    }
    system("cls");
    Game game(difficulty);

    while (true) {
        game.draw();
        if (game.isGameOver()) break;
        if (_kbhit()) game.handleInput();
        game.update();
        Sleep(game.getSleepDuration());
    }
    return 0;
}