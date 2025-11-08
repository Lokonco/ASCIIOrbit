#include <iostream>
#include <cmath>
#include <vector>

// ANSI escape codes for colors
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define GRAY "\033[90m"
#define ORANGE "\033[38;5;208m"
#define CYAN "\033[36m"

// Clear screen
void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

// Hide cursor
void hideCursor() {
    std::cout << "\033[?25l" << std::flush;
}

// Show cursor
void showCursor() {
    std::cout << "\033[?25h" << std::flush;
}

// Get terminal size
void getTerminalSize(int& width, int& height) {
    width = 80;
    height = 24;

    #ifndef _WIN32
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
    double orbitRadius;
};

// Draw a point if in bounds
void drawPoint(std::vector<std::vector<char>>& buffer,
               std::vector<std::vector<std::string>>& colorBuffer,
               int x, int y, char c, const std::string& color,
               int width, int height) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        buffer[y][x] = c;
        colorBuffer[y][x] = color;
    }
}

int main() {
    // Initialize planets with wider spacing for visibility
    std::vector<Planet> planets = {
        {"Mercury", 'M', GRAY, 4.0},
        {"Venus", 'V', YELLOW, 7.0},
        {"Earth", 'E', BLUE, 10.0},
        {"Mars", 'm', RED, 13.0},
        {"Jupiter", 'J', ORANGE, 17.0},
        {"Saturn", 'S', YELLOW, 21.0},
        {"Uranus", 'U', CYAN, 25.0},
        {"Neptune", 'N', BLUE, 29.0}
    };

    // Get terminal size
    int termWidth, termHeight;
    getTerminalSize(termWidth, termHeight);

    // Calculate center and scale
    int centerX = termWidth / 2;
    int centerY = termHeight / 2;

    // Scale to fit terminal
    double maxRadius = 21.0;
    int availableWidth = termWidth / 2 - 2;
    int availableHeight = termHeight / 2 - 2;
    double scale = std::min(availableWidth / (maxRadius * 2.0),
                           static_cast<double>(availableHeight) / maxRadius);

    // Clear screen and hide cursor
    clearScreen();
    hideCursor();

    // Create frame buffers
    std::vector<std::vector<char>> buffer(termHeight, std::vector<char>(termWidth, ' '));
    std::vector<std::vector<std::string>> colorBuffer(termHeight, std::vector<std::string>(termWidth, RESET));

    // Draw Sun at center
    drawPoint(buffer, colorBuffer, centerX, centerY, 'O', YELLOW, termWidth, termHeight);

    // Draw orbits and planets
    const double ASPECT_RATIO = 2.0;

    for (const auto& planet : planets) {
        int radius = static_cast<int>(planet.orbitRadius * scale);

        // Draw orbit circle - 360 points
        for (int deg = 0; deg < 360; deg++) {
            double angle = deg * M_PI / 180.0;
            int x = centerX + static_cast<int>(round(radius * ASPECT_RATIO * cos(angle)));
            int y = centerY + static_cast<int>(round(radius * sin(angle)));
            drawPoint(buffer, colorBuffer, x, y, '.', GRAY, termWidth, termHeight);
        }

        // Place planet at angle 0 (right side)
        int planetX = centerX + static_cast<int>(radius * ASPECT_RATIO);
        int planetY = centerY;
        drawPoint(buffer, colorBuffer, planetX, planetY, planet.symbol, planet.color, termWidth, termHeight);
    }

    // Render the buffer
    for (int y = 0; y < termHeight; y++) {
        for (int x = 0; x < termWidth; x++) {
            std::cout << colorBuffer[y][x] << buffer[y][x] << RESET;
        }
        std::cout << "\n";
    }

    // Wait for user input
    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    // Restore cursor
    showCursor();
    clearScreen();

    return 0;
}