#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <ctime>
#include<fstream>

const int ROWS = 8;
const int COLS = 8;
const float CELL = 70;
const float BOARD_X = 20;
const float BOARD_Y = 100;
const int POINTS_PER_CANDY = 10;

// Game data. Add more variables if your team needs them.
int board[ROWS][COLS] = {};
bool marked[ROWS][COLS] = {};
int score = 0;
int movesLeft = 0;
int targetScore = 0;
int selectedrow = -1;
int selectedcol = -1;
int gamestate = 0;
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
    std::ifstream file(level.txt);
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
    (void)matches;
    // TODO: Mark every candy in a horizontal or vertical match of 3 or more.
}

void clearMatches(const bool matches[ROWS][COLS])
{
    (void)matches;
    // TODO: Clear marked candies and add points.
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
