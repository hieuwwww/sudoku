#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 540;
const int CELL_SIZE = 60;
const int GRID_WIDTH = CELL_SIZE * 9;
const int GRID_HEIGHT = CELL_SIZE * 9;
const sf::Color LINE_COLOR = sf::Color::Black;
const sf::Color TEXT_COLOR = sf::Color::Black;
const sf::Color HIGHLIGHT_COLOR = sf::Color(150, 150, 150);
std::vector<std::vector<int>> sudokuGrid = {
    {5, 3, 0, 0, 7, 0, 0, 0, 0},
    {6, 0, 0, 1, 9, 5, 0, 0, 0},
    {0, 9, 8, 0, 0, 0, 0, 6, 0},
    {8, 0, 0, 0, 6, 0, 0, 0, 3},
    {4, 0, 0, 8, 0, 3, 0, 0, 1},
    {7, 0, 0, 0, 2, 0, 0, 0, 6},
    {0, 6, 0, 0, 0, 0, 2, 8, 0},
    {0, 0, 0, 4, 1, 9, 0, 0, 5},
    {0, 0, 0, 0, 8, 0, 0, 7, 9}
};

bool isValid(int row, int col, int num) {
    // Kiểm tra xem giá trị num có hợp lệ trong ô (row, col) không
    for (int x = 0; x < 9; ++x) {
        if (sudokuGrid[row][x] == num || sudokuGrid[x][col] == num) {
            return false;
        }
    }
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sudokuGrid[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }
    return true;
}

bool solveSudoku() {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (sudokuGrid[row][col] == 0) {
                for (int num = 1; num <= 9; ++num) {
                    if (isValid(row, col, num)) {
                        sudokuGrid[row][col] = num;
                        if (solveSudoku()) {
                            return true;
                        } else {
                            sudokuGrid[row][col] = 0;
                        }
                    }
                }
                return false;
            }
        }
    }
    return true;
}

void drawGrid(sf::RenderWindow& window, sf::Font& font) {
    int startX = (WINDOW_WIDTH - GRID_WIDTH) / 2;
    int startY = (WINDOW_HEIGHT - GRID_HEIGHT) / 2;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(startX + j * CELL_SIZE, startY + i * CELL_SIZE);
            cell.setOutlineThickness(1);
            cell.setOutlineColor(LINE_COLOR);
            window.draw(cell);
            
            if (sudokuGrid[i][j] != 0) {
                sf::Text text(std::to_string(sudokuGrid[i][j]), font, 30);
                text.setPosition(startX + j * CELL_SIZE + 20, startY + i * CELL_SIZE + 10);
                text.setFillColor(TEXT_COLOR);
                window.draw(text);
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Sudoku");
    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::RectangleShape buttonSolve(sf::Vector2f(100, 50));
    buttonSolve.setPosition(50, 50);
    buttonSolve.setOutlineThickness(1);
    buttonSolve.setOutlineColor(LINE_COLOR);
    sf::Text textSolve("Solve", font, 20);
    textSolve.setPosition(75, 60);
    textSolve.setFillColor(TEXT_COLOR);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonReleased) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (buttonSolve.getGlobalBounds().contains(mousePos)) {
                    solveSudoku();
                }
            }
        }

        window.clear(sf::Color::White);
        drawGrid(window, font);
        window.draw(buttonSolve);
        window.draw(textSolve);
        window.display();
    }

    return 0;
}
