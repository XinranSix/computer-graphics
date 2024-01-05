#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <thread>

const int rows = 100;
const int cols = 100;
const float cellSize = 20.0f;
const float borderSize = 1.0f;
std::vector<std::vector<bool>> grid(rows, std::vector<bool>(cols, false));

void initializeGrid() {
    // 使用当前时间作为随机数种子
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 随机初始化细胞状态
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = std::rand() % 2 == 1;
        }
    }
}

int countNeighbors(int x, int y) {
    // 计算细胞周围活细胞的数量
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0)
                continue;

            int newX = x + i;
            int newY = y + j;

            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols &&
                grid[newX][newY])
                count++;
        }
    }
    return count;
}

void updateGrid() {
    // 根据规则更新细胞状态
    std::vector<std::vector<bool>> newGrid(rows,
                                           std::vector<bool>(cols, false));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int neighbors = countNeighbors(i, j);

            if (grid[i][j]) {
                // 活细胞规则
                newGrid[i][j] = (neighbors == 2 || neighbors == 3);
            } else {
                // 死细胞规则
                newGrid[i][j] = (neighbors == 3);
            }
        }
    }

    grid = newGrid;
}

void drawGrid(sf::RenderWindow &window) {
    // 绘制当前细胞状态
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape cell(sf::Vector2f(cellSize - 2 * borderSize,
                                                 cellSize - 2 * borderSize));
            cell.setPosition(j * cellSize + borderSize,
                             i * cellSize + borderSize);
            cell.setFillColor(grid[i][j] ? sf::Color::White : sf::Color::Black);
            cell.setOutlineThickness(borderSize);
            cell.setOutlineColor(sf::Color::Green); // 设置边框颜色
            window.draw(cell);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(cols * static_cast<int>(cellSize),
                                          rows * static_cast<int>(cellSize)),
                            "Conway's Game of Life");

    initializeGrid();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        drawGrid(window);

        window.display();

        updateGrid();

        // 休眠一段时间，以便观察演变过程
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
