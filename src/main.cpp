//------Imports------//
#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>
//-----------------//

//-----ANSI codes for colors-----//
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define GRAY "\033[90m"
#define ORANGE "\033[38;5;208m"
#define CYAN "\033[36m"
//-----------------------------//

// Clear screen and move cursor to top-left
void clearScreen() {
    std::cout << "\033[2J\033[H";
}

// Hide cursor
void hideCursor() {
    std::cout << "\033[?25l";
}

// Show cursor
void showCursor() {
    std::cout << "\033[?25h";
}

// Get terminal size (cross-platform approximation)
void getTerminalSize(int& width, int& height) {
    // Default values if we can't detect
    width = 80;
    height = 24;

    #ifdef _WIN32
        // Windows implementation would go here
        // For now using defaults
    #else
        // Unix/Linux - use environment or ioctl
        FILE* fp = popen("tput cols", "r");
        if (fp) {
            fscanf(fp, "%d", &width);
            pclose(fp);
        }
        fp = popen("tput lines", "r");
        if (fp) {
            fscanf(fp, "%d", &height);
            pclose(fp);
        }
    #endif
}

// Structure to represent a planet
struct Planet {
    std::string name;
    char symbol;
    std::string color;
    double orbitRadius;  // Relative radius for display
};

// Draw a point at (x, y) with a character and color
void drawPoint(std::vector<std::vector<char>>& buffer,
               std::vector<std::vector<std::string>>& colorBuffer,
               int x, int y, char c, const std::string& color,
               int width, int height) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        buffer[y][x] = c;
        colorBuffer[y][x] = color;
    }
}

// Draw a circle using midpoint circle algorithm
void drawCircle(std::vector<std::vector<char>>& buffer,
                std::vector<std::vector<std::string>>& colorBuffer,
                int centerX, int centerY, int radius,
                int width, int height) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    auto plot8Points = [&](int xc, int yc, int x, int y) {
        drawPoint(buffer, colorBuffer, xc + x, yc + y, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc - x, yc + y, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc + x, yc - y, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc - x, yc - y, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc + y, yc + x, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc - y, yc + x, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc + y, yc - x, '.', GRAY, width, height);
        drawPoint(buffer, colorBuffer, xc - y, yc - x, '.', GRAY, width, height);
    };

    plot8Points(centerX, centerY, x, y);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot8Points(centerX, centerY, x, y);
    }
}

int main() {
    // Initialize planets
    std::vector<Planet> planets = {
        {"Mercury", 'M', GRAY, 1.0},
        {"Venus", 'V', YELLOW, 1.5},
        {"Earth", 'E', BLUE, 2.0},
        {"Mars", 'M', RED, 2.5},
        {"Jupiter", 'J', ORANGE, 3.5},
        {"Saturn", 'S', YELLOW, 4.5},
        {"Uranus", 'U', CYAN, 5.5},
        {"Neptune", 'N', BLUE, 6.5}
    };

    // Get terminal size
    int termWidth, termHeight;
    getTerminalSize(termWidth, termHeight);

    // Calculate center and scale
    int centerX = termWidth / 2;
    int centerY = termHeight / 2;
    int scale = std::min(termWidth, termHeight * 2) / 16;  // Adjust for aspect ratio

    // Hide cursor and clear screen
    hideCursor();
    clearScreen();

    // Create frame buffer
    std::vector<std::vector<char>> buffer(termHeight, std::vector<char>(termWidth, ' '));
    std::vector<std::vector<std::string>> colorBuffer(termHeight, std::vector<std::string>(termWidth, RESET));

    // Draw Sun at center
    drawPoint(buffer, colorBuffer, centerX, centerY, 'O', YELLOW, termWidth, termHeight);

    // Draw orbits and place planets
    for (size_t i = 0; i < planets.size(); i++) {
        int radius = static_cast<int>(planets[i].orbitRadius * scale);

        // Draw orbit circle
        drawCircle(buffer, colorBuffer, centerX, centerY, radius, termWidth, termHeight);

        // Place planet on orbit (at angle 0 for now - right side)
        int planetX = centerX + radius;
        int planetY = centerY;
        drawPoint(buffer, colorBuffer, planetX, planetY, planets[i].symbol,
                 planets[i].color, termWidth, termHeight);
    }

    // Render the buffer
    for (int y = 0; y < termHeight; y++) {
        for (int x = 0; x < termWidth; x++) {
            std::cout << colorBuffer[y][x] << buffer[y][x] << RESET;
        }
        std::cout << "\n";
    }

    // Wait for user input before exiting
    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    // Restore cursor
    showCursor();
    clearScreen();

    return 0;
}