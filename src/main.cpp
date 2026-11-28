#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int ROWS = 8;
const int COLS = 8;
const float CELL = 70.f;
const float BOARD_X = 20.f;
const float BOARD_Y = 100.f;
const int POINTS_PER_CANDY = 10;
const int MENU = 0;
const int PLAYING = 1;
const int WIN = 2;
const int GAME_OVER = 3;
const int LOAD_ERROR = 4;

int board[ROWS][COLS] = {};
bool marked[ROWS][COLS] = {};
int score = 0;
int movesLeft = 0;
int targetScore = 0;
int selectedrow = -1;
int selectedcol = -1;
int gameState = MENU;
string statusMessage;

bool loadLevel(const char* fileName);
void handleInput(sf::RenderWindow& window);
void updateGame();
void drawGame(sf::RenderWindow& window);
void findMatches(bool matches[ROWS][COLS]);
void clearMatches(const bool matches[ROWS][COLS]);
void dropCandies();
void refillBoard();
bool trySwap(int row1, int col1, int row2, int col2);
void processChains();
bool saveGame(const char* fileName);
bool loadGame(const char* fileName);
void startNewGame();
void clearSaveFile(const char* fileName);

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window);
    }

    return 0;
}

bool loadLevel(const char* fileName)
{
    // The level file contains rows, columns, moves, target, and then the board.
    ifstream file(fileName);
    if (!file)
    {
        statusMessage = "Could not open level.txt";
        return false;
    }

    int fileRows = 0;
    int fileCols = 0;
    if (!(file >> fileRows >> fileCols) || fileRows != ROWS || fileCols != COLS)
    {
        statusMessage = "level.txt must start with 8 8";
        return false;
    }

    if (!(file >> movesLeft >> targetScore) || movesLeft < 0 || targetScore < 0)
    {
        statusMessage = "Moves or target score is invalid";
        return false;
    }

    score = 0;
    selectedrow = -1;
    selectedcol = -1;

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (!(file >> board[row][col]) || board[row][col] < 0 || board[row][col] > 6)
            {
                statusMessage = "Candy values must be from 0 to 6";
                return false;
            }
            marked[row][col] = false;
        }
    }

    statusMessage.clear();
    return true;
}

void handleInput(sf::RenderWindow& window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            continue;
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            switch (gameState)
            {
            case MENU:
                if (key->code == sf::Keyboard::Key::N || key->code == sf::Keyboard::Key::Enter)
                {
                    startNewGame();
                }
                else if (key->code == sf::Keyboard::Key::C)
                {
                    if (loadGame("savegame.txt"))
                    {
                        gameState = PLAYING;
                        statusMessage.clear();
                    }
                    else
                    {
                        statusMessage = "No saved game";
                    }
                }
                else if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
                break;

            case PLAYING:
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    if (saveGame("savegame.txt"))
                    {
                        gameState = MENU;
                        selectedrow = -1;
                        selectedcol = -1;
                        statusMessage = "Game saved. Press C to continue.";
                    }
                    else
                    {
                        statusMessage = "Could not save savegame.txt";
                    }
                }
                break;

            case WIN:
            case GAME_OVER:
                if (key->code == sf::Keyboard::Key::Enter)
                {
                    gameState = MENU;
                    statusMessage.clear();
                }
                break;

            case LOAD_ERROR:
                if (key->code == sf::Keyboard::Key::Enter)
                {
                    gameState = MENU;
                }
                else if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
                break;
            }
        }

        const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>();
        if (mouse == nullptr || mouse->button != sf::Mouse::Button::Left)
        {
            continue;
        }

        if (gameState == MENU)
        {
            int x = mouse->position.x;
            int y = mouse->position.y;

            // These three clickable rows are New Game, Continue, and Exit.
            if (x >= 150 && x <= 450 && y >= 300 && y < 350)
            {
                startNewGame();
            }
            else if (x >= 150 && x <= 450 && y >= 350 && y < 400)
            {
                if (loadGame("savegame.txt"))
                {
                    gameState = PLAYING;
                    statusMessage.clear();
                }
                else
                {
                    statusMessage = "No saved game";
                }
            }
            else if (x >= 150 && x <= 450 && y >= 400 && y < 450)
            {
                window.close();
            }
            continue;
        }

        if (gameState != PLAYING)
        {
            continue;
        }

        // Check pixel limits first because dividing a small negative number by CELL
        // would round toward zero and could accidentally count as cell 0.
        if (mouse->position.x < BOARD_X || mouse->position.x >= BOARD_X + COLS * CELL ||
            mouse->position.y < BOARD_Y || mouse->position.y >= BOARD_Y + ROWS * CELL)
        {
            continue;
        }

        int col = (mouse->position.x - static_cast<int>(BOARD_X)) / static_cast<int>(CELL);
        int row = (mouse->position.y - static_cast<int>(BOARD_Y)) / static_cast<int>(CELL);

        // Ignore clicks outside the 8 by 8 board.
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        {
            continue;
        }

        if (selectedrow == -1)
        {
            selectedrow = row;
            selectedcol = col;
        }
        else if (selectedrow == row && selectedcol == col)
        {
            selectedrow = -1;
            selectedcol = -1;
        }
        else
        {
            int rowDifference = selectedrow - row;
            int colDifference = selectedcol - col;
            if (rowDifference < 0) rowDifference = -rowDifference;
            if (colDifference < 0) colDifference = -colDifference;

            if (rowDifference + colDifference != 1)
            {
                // A non-neighbour becomes the new selected candy.
                selectedrow = row;
                selectedcol = col;
            }
            else
            {
                bool madeMatch = trySwap(selectedrow, selectedcol, row, col);
                movesLeft--;

                if (madeMatch)
                {
                    processChains();
                }

                selectedrow = -1;
                selectedcol = -1;
            }
        }
    }
}

void updateGame()
{
    switch (gameState)
    {
    case PLAYING:
        if (score >= targetScore)
        {
            clearSaveFile("savegame.txt");
            gameState = WIN;
        }
        else if (movesLeft <= 0)
        {
            clearSaveFile("savegame.txt");
            gameState = GAME_OVER;
        }
        break;
    default:
        break;
    }
}

void drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color(25, 25, 45));

    // This font path works on Windows; put arial.ttf beside the program if needed.
    static sf::Font font;
    static bool fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    switch (gameState)
    {
    case MENU:
        if (fontLoaded)
        {
            sf::Text title(font, "CANDY CRUSH", 40);
            title.setPosition({ 175.f, 205.f });
            window.draw(title);

            sf::Text option(font, "New Game (N/Enter)", 23);
            option.setPosition({ 185.f, 300.f });
            window.draw(option);
            option.setString("Continue (C)");
            option.setPosition({ 185.f, 350.f });
            window.draw(option);
            option.setString("Exit (Esc)");
            option.setPosition({ 185.f, 400.f });
            window.draw(option);

            if (!statusMessage.empty())
            {
                sf::Text message(font, statusMessage, 18);
                message.setPosition({ 150.f, 470.f });
                message.setFillColor(sf::Color::Yellow);
                window.draw(message);
            }
        }
        break;

    case LOAD_ERROR:
        if (fontLoaded)
        {
            sf::Text error(font, "Could not load level", 30);
            error.setPosition({ 155.f, 250.f });
            window.draw(error);

            sf::Text details(font, statusMessage + "\nPress ENTER to return", 18);
            details.setPosition({ 95.f, 315.f });
            window.draw(details);
        }
        break;

    case WIN:
    case GAME_OVER:
        if (fontLoaded)
        {
            sf::Text result(font, gameState == WIN ? "YOU WIN!" : "GAME OVER", 40);
            result.setPosition({ 190.f, 230.f });
            window.draw(result);

            sf::Text finalScore(font, "Final score: " + to_string(score) + "\nPress ENTER for menu", 22);
            finalScore.setPosition({ 170.f, 305.f });
            window.draw(finalScore);
        }
        break;

    case PLAYING:
    {
        // The HUD shows score, moves left, and target score above the board.
        sf::RectangleShape panel({ 560.f, 62.f });
        panel.setPosition({ 20.f, 20.f });
        panel.setFillColor(sf::Color(25, 25, 55));
        panel.setOutlineColor(sf::Color(145, 155, 220));
        panel.setOutlineThickness(3.f);
        window.draw(panel);

        if (fontLoaded)
        {
            sf::Text text(font, "", 16);
            text.setString("SCORE: " + to_string(score));
            text.setPosition({ 45.f, 38.f });
            window.draw(text);

            text.setString("MOVES: " + to_string(movesLeft));
            text.setPosition({ 245.f, 38.f });
            window.draw(text);

            text.setString("TARGET: " + to_string(targetScore));
            text.setPosition({ 425.f, 38.f });
            window.draw(text);
        }

        sf::Color colors[7] = {
            sf::Color(25, 25, 45), // 0: empty
            sf::Color::Red,        // 1
            sf::Color(255, 150, 0),// 2: orange
            sf::Color::Yellow,     // 3
            sf::Color::Green,      // 4
            sf::Color::Blue,       // 5
            sf::Color(170, 90, 255)// 6: purple
        };

        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                float x = BOARD_X + col * CELL;
                float y = BOARD_Y + row * CELL;

                sf::RectangleShape tile({ CELL - 2.f, CELL - 2.f });
                tile.setPosition({ x, y });
                tile.setFillColor(sf::Color(50, 55, 85));
                tile.setOutlineColor(sf::Color(130, 140, 195));
                tile.setOutlineThickness(1.f);
                window.draw(tile);

                if (board[row][col] != 0)
                {
                    sf::CircleShape candy(28.f);
                    candy.setPosition({ x + 7.f, y + 7.f });
                    candy.setFillColor(colors[board[row][col]]);

                    if (row == selectedrow && col == selectedcol)
                    {
                        candy.setOutlineColor(sf::Color::White);
                        candy.setOutlineThickness(4.f);
                    }
                    window.draw(candy);
                }
            }
        }
        break;
    }
    }

    window.display();
}

void findMatches(bool matches[ROWS][COLS])
{
    // Start by saying no cell is part of a match.
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            matches[row][col] = false;
        }
    }

    // Check every group of three side-by-side candies.
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col <= COLS - 3; col++)
        {
            if (board[row][col] != 0 &&
                board[row][col] == board[row][col + 1] &&
                board[row][col] == board[row][col + 2])
            {
                matches[row][col] = true;
                matches[row][col + 1] = true;
                matches[row][col + 2] = true;
            }
        }
    }

    // Check every group of three candies going down a column.
    for (int col = 0; col < COLS; col++)
    {
        for (int row = 0; row <= ROWS - 3; row++)
        {
            if (board[row][col] != 0 &&
                board[row][col] == board[row + 1][col] &&
                board[row][col] == board[row + 2][col])
            {
                matches[row][col] = true;
                matches[row + 1][col] = true;
                matches[row + 2][col] = true;
            }
        }
    }
}

void clearMatches(const bool matches[ROWS][COLS])
{
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (matches[row][col] == true)
            {
                board[row][col] = 0;
                score = score + POINTS_PER_CANDY;
            }
        }
    }
}

void dropCandies()
{
    // Repeat the bottom-up pass so candies move through several empty cells.
    for (int col = 0; col < COLS; col++)
    {
        for (int times = 0; times < ROWS; times++)
        {
            for (int row = ROWS - 1; row > 0; row--)
            {
                if (board[row][col] == 0)
                {
                    board[row][col] = board[row - 1][col];
                    board[row - 1][col] = 0;
                }
            }
        }
    }
}

void refillBoard()
{
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (board[row][col] == 0)
            {
                int newCandy = rand() % 6 + 1;
                board[row][col] = newCandy;
            }
        }
    }
}

bool trySwap(int r1, int c1, int r2, int c2)
{
    bool neighbour = false;

    if (r1 == r2 && (c1 - c2 == 1 || c2 - c1 == 1))
    {
        neighbour = true;
    }

    if (c1 == c2 && (r1 - r2 == 1 || r2 - r1 == 1))
    {
        neighbour = true;
    }

    if (neighbour == false)
    {
        return false;
    }

    int temp = board[r1][c1];
    board[r1][c1] = board[r2][c2];
    board[r2][c2] = temp;

    bool matches[ROWS][COLS];
    findMatches(matches);

    bool anyMatch = false;
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (matches[row][col] == true)
            {
                anyMatch = true;
            }
        }
    }

    if (anyMatch == false)
    {
        // No match was made, so put the candies back.
        temp = board[r1][c1];
        board[r1][c1] = board[r2][c2];
        board[r2][c2] = temp;
        return false;
    }

    return true;
}

void processChains()
{
    bool keepChecking = true;

    while (keepChecking == true)
    {
        bool matches[ROWS][COLS];
        findMatches(matches);
        bool hasMatch = false;

        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (matches[row][col] == true)
                {
                    hasMatch = true;
                }
            }
        }

        if (hasMatch == false)
        {
            keepChecking = false;
        }
        else
        {
            clearMatches(matches);
            dropCandies();
            refillBoard();
        }
    }
}

bool saveGame(const char* fileName)
{
    ofstream saveFile(fileName);
    if (!saveFile)
    {
        return false;
    }

    // The first line stores score, moves left, and target score.
    saveFile << score << ' ' << movesLeft << ' ' << targetScore << endl;
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            saveFile << board[row][col] << ' ';
        }
        saveFile << endl;
    }

    return true;
}

bool loadGame(const char* fileName)
{
    ifstream saveFile(fileName);
    if (!saveFile)
    {
        return false;
    }

    if (!(saveFile >> score >> movesLeft >> targetScore) || score < 0 || movesLeft < 0 || targetScore < 0)
    {
        return false;
    }

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (!(saveFile >> board[row][col]) || board[row][col] < 0 || board[row][col] > 6)
            {
                return false;
            }
            marked[row][col] = false;
        }
    }

    selectedrow = -1;
    selectedcol = -1;
    return true;
}

void startNewGame()
{
    score = 0;
    movesLeft = 0;
    targetScore = 0;
    selectedrow = -1;
    selectedcol = -1;

    if (loadLevel("level.txt"))
    {
        gameState = PLAYING;
    }
    else
    {
        gameState = LOAD_ERROR;
    }
}

void clearSaveFile(const char* fileName)
{
    // Opening with truncation empties a finished game's save file.
    ofstream saveFile(fileName, ios::trunc);
}