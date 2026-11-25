#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>   // <-- THIS LINE IS MISSING
using namespace std;

const int ROWS = 8;
const int COLS = 8;
const float CELL = 70.f;
const float BOARD_X = 20.f;
const float BOARD_Y = 100.f;
const int POINTS_PER_CANDY = 10;

// Game data. Add more variables if your team needs them.
int board[ROWS][COLS] = {};
bool marked[ROWS][COLS] = {};
int score = 0;
int movesLeft = 0;
int targetScore = 0;
int selectedrow = -1;
int selectedcol = -1;
int gameState = 0;
int clickX = -1;
int clickY = -1;
bool clicked = false;

// Function declarations. The game logic is intentionally left for your team.
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
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        clicked = false;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    clickX = mouse->position.x;
                    clickY = mouse->position.y;
                    clicked = true;
                }
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                // TODO: Store and handle the keys your game needs.
                (void)key;
            }
        }

        handleInput(window);
        updateGame();
        drawGame(window);
    }

    return 0;
}

// TODO: Implement the function bodies below with your partner.
// The declarations are provided to help divide the project into features.

bool loadLevel(const char* fileName)
{//umama
    std::ifstream file(fileName);
    if (!file)
    {
        return false;
    }

    file >> movesLeft;
    file >> targetScore;
    selectedrow = -1;
    selectedcol = -1;
    score = 0;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            file >> board[r][c];
            marked[r][c] = false;
        }
    }

    file.close();
    return true;


}

void handleInput(sf::RenderWindow& window)//maira
{
    while (auto event = window.pollEvent())
    {
        // close button
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        // key press
        if (auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            // Escape - back to menu or close
            if (key->code == sf::Keyboard::Key::Escape)
            {
                if (gameState == 1)
                {
                    gameState = 0;
                    selectedrow = -1;
                    selectedcol = -1;
                }
                else
                {
                    window.close();
                }
            }

            // Enter - start game from menu
            if (key->code == sf::Keyboard::Key::Enter)
            {
                if (gameState == 0)
                {
                    gameState = 1;
                    score = 0;
                    movesLeft = 20;
                    selectedrow = -1;
                    selectedcol = -1;
                }
            }
        }

        // mouse click
        if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (gameState != 1)
            {
                continue;
            }

            if (mouse->button == sf::Mouse::Button::Left)
            {
                int mouseX = mouse->position.x;
                int mouseY = mouse->position.y;

                // mouse to board r and c - using YOUR CELL, BOARD_X, BOARD_Y
                int c = (mouseX - (int)BOARD_X) / (int)CELL;
                int r = (mouseY - (int)BOARD_Y) / (int)CELL;

                // if outside board, ignore
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS)
                {
                    continue;
                }

                // save click for updateGame
                clickX = c;
                clickY = r;
                clicked = true;

                // first click - select
                if (selectedrow == -1)
                {
                    selectedrow = r;
                    selectedcol = c;
                }
                // second click - clear selection, updateGame will do swap
                else
                {
                    selectedrow = -1;
                    selectedcol = -1;
                }
            }
        }
    }
}

void updateGame()//maira
{
    // TODO: Add game-state changes, match processing, scoring, and win/lose checks.
    if (gameState != 1)
    {
        return; // only work when playing
    }

    bool matches[ROWS][COLS];
    findMatches(matches);

    // check if any match is there
    bool any = false;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (matches[r][c] == true)
            {
                any = true;
            }
        }
    }

    // if match found, clear and refill
    if (any == true)
    {
        clearMatches(matches);
        dropCandies();
        refillBoard();
    }

    // win check
    if (score >= 500)
    {
        gameState = 2; // win screen
        return;
    }

    // lose check
    if (movesLeft <= 0)
    {
        // if still match left, don't lose yet
        if (any == true)
        {
            return;
        }
        gameState = 3; // lose screen
        return;
    }
}

void drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color(25, 25, 45));
    // TODO: Draw the HUD, menu, board, candies, selection, and end screen.
    window.display();
}

void findMatches(bool matches[ROWS][COLS])
{
    // First make all false, no match in start
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            matches[r][c] = false;
        }
    }

    // Check side by side - 3 same in a row
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c <= COLS - 3; c++)
        {
            // If not empty and all 3 are same
            if (board[r][c] != 0 &&
                board[r][c] == board[r][c + 1] &&
                board[r][c] == board[r][c + 2])
            {
                matches[r][c] = true;
                matches[r][c + 1] = true;
                matches[r][c + 2] = true;
            }
        }
    }

    // Check up to down - 3 same in a column
    for (int c = 0; c < COLS; c++)
    {
        for (int r = 0; r <= ROWS - 3; r++)
        {
            // If not empty and all 3 are same
            if (board[r][c] != 0 &&
                board[r][c] == board[r + 1][c] &&
                board[r][c] == board[r + 2][c])
            {
                matches[r][c] = true;
                matches[r + 1][c] = true;
                matches[r + 2][c] = true;
            }
        }
    }
}

void clearMatches(const bool matches[ROWS][COLS])
{
    // Go through whole board
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            // If this cell was matched
            if (matches[r][c] == true)
            {
                board[r][c] = 0; // make it empty
                score = score + 10; // 10 points for 1 candy
            }
        }
    }
}

void dropCandies()
{
    // Check every column
    for (int col = 0; col < COLS; col++)
    {
        // We do this many times so candies fall all the way down
        for (int times = 0; times < ROWS; times++)
        {
            // Check from bottom to top
            for (int row = ROWS - 1; row > 0; row--)
            {
                // If this cell is empty, pull the candy from above down
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
                // 1 to 6, new candy
                int newCandy = rand() % 6 + 1;
                board[row][col] = newCandy;
            }
        }
    }
}

bool trySwap(int row1, int col1, int row2, int col2)//maira
{
    (void)row1;
    (void)col1;
    (void)row2;
    (void)col2;
    // TODO: Swap neighboring candies only when the swap makes a match.
    return false;
}

void processChains()//umama 

{
    bool keepChecking = true;

    while (keepChecking == true)
    {
        bool matches[ROWS][COLS];
        findMatches(matches);

        bool hasMatch = false;

        // Check if any candy is part of a match
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

        // Stop when there are no matches
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

{//umAMA
    ofstream saveFile;
    saveFile.open(fileName);

    if (saveFile.is_open() == false)
    {
        return false;
    }

    // Save the game information
    saveFile << score << endl;
    saveFile << movesLeft << endl;
    saveFile << targetScore << endl;

    // Save the candies on the board
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            saveFile << board[row][col];
        }

        saveFile << endl;
    }

    saveFile.close();
    return true;
}

bool loadGame(const char* fileName)
{
    (void)fileName;
    // TODO: Load a saved game and reset the selected cell.
    return false;
}

void startNewGame()//maira
{
    // TODO: Reset score and load level.txt.
}

void clearSaveFile(const char* fileName)
{
    (void)fileName;
    // TODO: Empty the save file when the game ends.
}