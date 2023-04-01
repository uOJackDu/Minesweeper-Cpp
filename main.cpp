#include <SFML/Graphics.hpp>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class GameMode {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
    CUSTOM
};

const int tileWidth = 32;

int numRows = 0;
int numCols = 0;

int numMines = 0;

int numTiles = 0;
int numOpened = 0;

bool win = false;

std::vector<std::vector<int>> board;
std::vector<std::vector<bool>> openedTiles;
std::vector<std::vector<bool>> flags;

std::ofstream os;

bool checkPosValid(int row, int col) {
    return (0 <= row && row < numRows) && (0 <= col && col < numCols);
}

char tileValToChar(int val) {
    char ch = '0';

    if (val == -1) {
        ch = '*';

    } else if (val == 0) {
        ch = '.';

    } else {
        ch = val + '0';
    }
    return ch;
}

int getNumber(int limit) {
    int num = 0;

    for (;;) {
        std::cin >> num;

        if (std::cin.fail() || (num < 0 || num >= limit)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid input, please enter a number." << std::endl;
            continue;
        }
        break;
    }
    return num;
}

void resetBoard() {
    board = std::vector<std::vector<int>> (numRows, std::vector<int> (numCols, 0));
    openedTiles = std::vector<std::vector<bool>> (numRows, std::vector<bool> (numCols, false));
    flags = std::vector<std::vector<bool>> (numRows, std::vector<bool> (numCols, false));

    numTiles = numRows * numCols;
    if (numMines > numTiles) {
        numMines = numTiles;
    }
    numOpened = 0;
    win = false;
}

void createRamdomBoard() {
    std::srand(std::time(nullptr));

    for (int n = numMines; n > 0;) {
        int row = rand() % numRows;
        int col = rand() % numCols;

        if (board[row][col] == -1) {
            continue;
        }
        board[row][col] = -1;

        for (int i = row - 1; i < row + 1 + 1; i++) {
            for (int j = col - 1; j < col + 1 + 1; j++) {
                if (checkPosValid(i, j) && board[i][j] != -1) {
                    board[i][j]++;
                }
            }
        }
        n--;
    }
}

void showAllMines() {
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            if (board[row][col] == -1) {
                openedTiles[row][col] = true;
            }
        }
    }
}

void flagAllMines() {
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            if (board[row][col] == -1) {
                flags[row][col] = true;
            }
        }
    }
}

void displayBoard(sf::RenderWindow& app, sf::Sprite& s) {
    app.clear(sf::Color::White);

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            int idx = board[row][col];

            if (idx == -1) {
                idx = 9;
            }
            if (!openedTiles[row][col]) {
                idx = 10;
            }
            if (flags[row][col]) {
                idx = 11;
            }
            s.setTextureRect(sf::IntRect(idx * tileWidth, 0, tileWidth, tileWidth));
            s.setPosition(col * tileWidth, row * tileWidth);
            app.draw(s);
        }
    }
    app.display();
}

void printBoard() {
    os.open("./board.txt");

    if (os.is_open()) {
        os << "    ";
        for (int col = 0; col < numCols; col++) {
            os << col;
            if (col < 10) {
                os << ' ';
            }
        }
        os << '\n';

        os << "  __";
        for (int col = 0; col < numCols; col++) {
            os << "__";
        }
        os << '\n';

        for (int row = 0; row < numRows; row++) {
            os << row;
            if (row < 10) {
                os << ' ';
            }
            os << "| ";

            for (int col = 0; col < numCols; col++) {
                int tileVal = board[row][col];
                char tileChar;

                if (!openedTiles[row][col]) {
                    tileChar = '#';

                } else {
                    tileChar = tileValToChar(tileVal);
                }
                os << tileChar << " ";
            }
            os << '\n';
        }
        os.close();
    }
}

bool openTile(int row, int col) {
    if (!checkPosValid(row, col) || openedTiles[row][col] || flags[row][col]) {
        return true;
    }
    openedTiles[row][col] = true;
    numOpened++;

    if (board[row][col] == -1) {
        return false;
    }
    if (numTiles - numOpened == numMines) {
        win = true;
        return false;
    }
    return true;
}

bool openDfs(int row, int col) {
    std::vector<std::vector<int>> st;

    st.push_back({ row, col });

    while (!st.empty()) {
        std::vector<int> pos = st.back();
        st.pop_back();

        row = pos[0];
        col = pos[1];

        for (int i = row - 1; i < row + 1 + 1; i++) {
            for (int j = col - 1; j < col + 1 + 1; j++) {
                if (checkPosValid(i, j) && !openedTiles[i][j]) {
                    if (!openTile(i, j)) {
                        return false;
                    };
                    if (board[i][j] == 0) {
                        st.push_back({ i, j });
                    }
                }
            }
        }
    }
    return true;
}

bool open(int row, int col) {
    if (!checkPosValid(row, col) || openedTiles[row][col]) {
        return true;
    }
    if (!openTile(row, col)) {
        return false;
    };

    if (board[row][col] == 0) {
        if (!openDfs(row, col)) {
            return false;
        };
    }
    return true;
}

bool menu() {
    numRows = 20;
    numCols = 10;
    numMines = 0;

    resetBoard();

    sf::Texture menuTexture;
    menuTexture.loadFromFile("./images/tiles.jpg");
    sf::Sprite menuS(menuTexture);

    sf::RenderWindow menuWindow(sf::VideoMode(numCols * tileWidth, numRows * tileWidth), "Minesweeper!");

    GameMode gameMode = GameMode::BEGINNER;

    for (; menuWindow.isOpen();) {
        sf::Event e;

        while (menuWindow.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                menuWindow.close();
                return false;
            }
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Num1) {
                    gameMode = GameMode::BEGINNER;
                    menuWindow.close();

                } else if (e.key.code == sf::Keyboard::Num2) {
                    gameMode = GameMode::INTERMEDIATE;
                    menuWindow.close();

                } else if (e.key.code == sf::Keyboard::Num3) {
                    gameMode = GameMode::EXPERT;
                    menuWindow.close();

                } else if (e.key.code == sf::Keyboard::Num4) {
                    gameMode = GameMode::CUSTOM;
                    menuWindow.close();
                }
            }
        }
        displayBoard(menuWindow, menuS);
    }
    if (gameMode == GameMode::BEGINNER) {
        numRows = 9;
        numCols = 9;
        numMines = 10;

    } else if (gameMode == GameMode::INTERMEDIATE) {
        numRows = 16;
        numCols = 16;
        numMines = 40;

    } else if (gameMode == GameMode::EXPERT) {
        numRows = 16;
        numCols = 30;
        numMines = 99;

    } else if (gameMode == GameMode::CUSTOM) {
        numRows = 20;
        numCols = 20;
        numMines = 1;
    }
    resetBoard();
    return true;
}

bool gameLoop(sf::RenderWindow& app, sf::Sprite s) {
    for (bool gameover = false; app.isOpen();) {
        sf::Vector2i pos = sf::Mouse::getPosition(app);

        int row = pos.y / tileWidth;
        int col = pos.x / tileWidth;

        sf::Event e;

        while (app.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                app.close();
            }
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    app.close();
                    if (!menu()) {
                        return false;
                    }
                    return true;
                }
                if (e.key.code == sf::Keyboard::Space) {
                    return true;
                }
            }
            if (!gameover && e.type == sf::Event::MouseButtonPressed) {
                if (e.mouseButton.button == sf::Mouse::Left) {
                    if (!open(row, col)) {
                        gameover = true;

                        if (win) {
                            flagAllMines();
                        } else {
                            showAllMines();
                        }
                    }
                }
            }
        }
        displayBoard(app, s);
    }
    return false;
}

int main() {
    if (!menu()) {
        return 0;
    }
    sf::Texture texture;
    texture.loadFromFile("./images/tiles.jpg");
    sf::Sprite s(texture);

    for (bool running = true; running;) {
        sf::RenderWindow app(sf::VideoMode(numCols * tileWidth, numRows * tileWidth), "Minesweeper!");

        resetBoard();
        createRamdomBoard();

        if (!gameLoop(app, s)) {
            running = false;
        };
    }
    return 0;
}
