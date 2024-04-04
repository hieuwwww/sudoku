#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <chrono> 
#include <thread> 

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 60;
const int GRID_WIDTH = CELL_SIZE * 9;
const int GRID_HEIGHT = CELL_SIZE * 9;
const sf::Color LINE_COLOR = sf::Color::Black;
const sf::Color TEXT_COLOR = sf::Color::Black;
const sf::Color HIGHLIGHT_COLOR = sf::Color(150, 150, 150);
std::vector<std::vector<int>> sudokuGrid;

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
bool solveSudoku(sf::RenderWindow& window, sf::Font& font, int speed) {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (sudokuGrid[row][col] == 0) {
                for (int num = 1; num <= 9; ++num) {
                    if (isValid(row, col, num)) {
                        sudokuGrid[row][col] = num;
                        drawGrid(window, font); 
                        window.display(); 
                        std::this_thread::sleep_for(std::chrono::milliseconds(speed)); 
                        if (solveSudoku(window, font, speed)) {
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
int getInputSpeed(sf::RenderWindow& window, sf::Font& font) {
    sf::RectangleShape inputBox(sf::Vector2f(200, 50));
    inputBox.setPosition(350, 250);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);

    sf::Text text("Enter display speed (5-100 ms): ", font, 20);
    text.setPosition(250, 200);
    text.setFillColor(sf::Color::Black);

    std::string inputString;
    sf::Text inputText("", font, 20);
    inputText.setPosition(360, 260);

    bool isInputActive = false; // Biến để kiểm tra xem hộp nhập có được tương tác không

    bool showCursor = true;
    sf::Clock cursorTimer;

    while (true) {
        window.clear(sf::Color::White);

        // Hiển thị khung nhập và chú thích
        window.draw(inputBox);
        window.draw(text);

        // Hiển thị số vừa nhập
        if (isInputActive || !inputString.empty()) {
            inputText.setString(inputString + (showCursor ? "|" : ""));
            window.draw(inputText);
        }

        window.display();

        // Xử lý sự kiện
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (inputBox.getGlobalBounds().contains(mousePos)) {
                    isInputActive = true; // Kích hoạt hộp nhập khi người dùng nhấn vào
                } else {
                    isInputActive = false; // Vô hiệu hóa hộp nhập nếu người dùng nhấn ra ngoài
                }
            }
            if (event.type == sf::Event::TextEntered && isInputActive) {
                if (event.text.unicode == 13) { // Enter key
                    int speed;
                    try {
                        speed = std::stoi(inputString);
                        if (speed >= 5 && speed <= 100) {
                            return speed;
                        } else {
                            // Hiển thị thông báo khi nhập không hợp lệ
                            inputString.clear();
                        }
                    } catch (...) {
                        // Hiển thị thông báo khi nhập không hợp lệ
                        inputString.clear();
                    }
                } else if (event.text.unicode == 8 && !inputString.empty()) { // Backspace key
                    inputString.pop_back();
                } else {
                    if (event.text.unicode >= 48 && event.text.unicode <= 57) {
                        inputString += static_cast<char>(event.text.unicode);
                    }
                }
            }
        }

        // Kiểm tra và cập nhật đồng hồ đếm cho việc hiển thị con trỏ
        if (cursorTimer.getElapsedTime().asSeconds() >= 0.5) {
            showCursor = !showCursor;
            cursorTimer.restart();
        }
    }
}


void generateRandomSudoku() {
    std::ifstream file("sudoku_puzzles.txt");
    std::vector<std::string> puzzles;
    std::string line;
    while (std::getline(file, line)) {
        puzzles.push_back(line);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, puzzles.size() - 1);
    std::string puzzle = puzzles[dis(gen)];

    sudokuGrid.clear();
    for (int i = 0; i < 9; ++i) {
        std::vector<int> row;
        for (int j = 0; j < 9; ++j) {
            row.push_back(puzzle[i * 9 + j] - '0');
        }
        sudokuGrid.push_back(row);
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

    sf::RectangleShape buttonNew(sf::Vector2f(100, 50));
    buttonNew.setPosition(50, 120);
    buttonNew.setOutlineThickness(1);
    buttonNew.setOutlineColor(LINE_COLOR);
    sf::Text textNew("New", font, 20);
    textNew.setPosition(80, 130);
    textNew.setFillColor(TEXT_COLOR);

    generateRandomSudoku();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonReleased) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (buttonSolve.getGlobalBounds().contains(mousePos)) {
                    int speed = getInputSpeed(window, font);
                    solveSudoku(window, font, speed);
                }
                if (buttonNew.getGlobalBounds().contains(mousePos)) {
                    generateRandomSudoku();
                }
            }
        }

        window.clear(sf::Color::White);
        drawGrid(window, font);
        window.draw(buttonSolve);
        window.draw(textSolve);
        window.draw(buttonNew);
        window.draw(textNew);
        window.display();
    }

    return 0;
}
