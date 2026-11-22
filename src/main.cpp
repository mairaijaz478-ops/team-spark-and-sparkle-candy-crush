#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <ctime>

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
int selectedRow = -1;
int selectedCol = -1;
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

    sf::RenderWindow window(sf::VideoMode({600, 680}), "Candy Crush");
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
{
    (void)fileName;
    return false;
}

void handleInput(sf::RenderWindow& window)
{
    (void)window;
    // TODO: Add menu, board selection, swapping, and Escape-key handling.
}

void updateGame()
{
    // TODO: Add game-state changes, match processing, scoring, and win/lose checks.
}

void drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color(25, 25, 45));
    // TODO: Draw the HUD, menu, board, candies, selection, and end screen.
    window.display();
}

void findMatches(bool matches[ROWS][COLS])
{
  
    
    
        // Step 1: Start fresh - nothing is matched yet
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                matches[row][col] = false;
            }
        }

        // Step 2: Look for 3 in a row HORIZONTALLY
        // (col only goes up to COLS - 3 so col + 2 never goes off the board)
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col <= COLS - 3; col++)
            {
                int candy = board[row][col];

                if (candy != 0 &&
                    candy == board[row][col + 1] &&
                    candy == board[row][col + 2])
                {
                    matches[row][col] = true;
                    matches[row][col + 1] = true;
                    matches[row][col + 2] = true;
                }
            }
        }

        // Step 3: Look for 3 in a column VERTICALLY
        // (row only goes up to ROWS - 3 so row + 2 never goes off the board)
        for (int col = 0; col < COLS; col++)
        {
            for (int row = 0; row <= ROWS - 3; row++)
            {
                int candy = board[row][col];

                if (candy != 0 &&
                    candy == board[row + 1][col] &&
                    candy == board[row + 2][col])
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
        // Go through every cell on the board
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                // If findMatches marked this cell, remove the candy
                if (matches[row][col] == true)
                {
                    board[row][col] = 0;           // 0 means the cell is now empty
                    score = score + POINTS_PER_CANDY;  // 10 points for each candy cleared
                }
            }
        }
}

void dropCandies()
{
    // TODO: Move candies down within each column.
}

void refillBoard()
{
    // TODO: Fill empty cells with random candy values from 1 to 6.
}

bool trySwap(int row1, int col1, int row2, int col2)
{
    (void)row1;
    (void)col1;
    (void)row2;
    (void)col2;
    // TODO: Swap neighboring candies only when the swap makes a match.
    return false;
}

void processChains()
{
    // TODO: Repeat clear, drop, refill, and match detection until no matches remain.
}

bool saveGame(const char* fileName)
{
    (void)fileName;
    // TODO: Save score, moves, target, and all 8 board rows.
    return false;
}

bool loadGame(const char* fileName)
{
    (void)fileName;
    // TODO: Load a saved game and reset the selected cell.
    return false;
}

void startNewGame()
{
    // TODO: Reset score and load level.txt.
}

void clearSaveFile(const char* fileName)
{
    (void)fileName;
    // TODO: Empty the save file when the game ends.
}
