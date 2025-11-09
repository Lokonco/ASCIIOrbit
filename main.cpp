//-------Imports-------//
#include <iostream>
#include <cmath>
#include <vector>
//--------------------//

//------ANSI escape codes for colors------//
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define GRAY "\033[90m"
#define ORANGE "\033[38;5;208m"
#define CYAN "\033[36m"
//---------------------------------------//


//-------------Helper Functions-----------//
//Clear screen
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
//---------------------------------------//


class Canvas {
private:
    int width;
    int height;
    double span;
    double span_x;
    double span_y;
    std::vector<char> data;
    std::vector<std::string> colors;

public:
    Canvas(int w, int h, double font_aspect = 0.5, double span_val = 30.0)
        : width(w), height(h), span(span_val) {

        data.resize(width * height, ' ');
        colors.resize(width * height, RESET);

        // Calculate apparent width based on font aspect ratio
        double apparent_width = width * font_aspect;

        // Adjust spans based on aspect ratio
        if (apparent_width > height) {
            span_x = span / height * apparent_width;
            span_y = span;
        } else {
            span_x = span;
            span_y = span / apparent_width * height;
        }
    }

    // Set a character at real-world coordinates (x, y in AU)
    void setPosition(char c, const std::string& color, double x, double y) {
        // Map from real coordinates to screen coordinate
        int sx = static_cast<int>(0.5 * (x / span_x + 1) * width);
        int sy = static_cast<int>(0.5 * (-y / span_y + 1) * height);

        // Check bounds
        if (sx < 0 || sx >= width || sy < 0 || sy >= height) {
            return;
        }

        int index = sy * width + sx;
        data[index] = c;
        colors[index] = color;
    }

    void show() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int index = y * width + x;
                std::cout << colors[index] << data[index] << RESET;
            }
            std::cout << "\n";
        }
    }
};

// Structure to represent a planet
struct Planet {
    std::string name;
    char symbol;
    std::string color;
    double orbitRadius;  // Use AU for this
};

int main() {
    // Initialize planets with spacing in "AU" (arbitrary units for now)
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

    // Clear screen and hide cursor
    clearScreen();
    hideCursor();

    // Create canvas with font aspect ratio of 0.5 (typical terminal font)
    // Span of 30.0 means we show from -30 to +30 in each direction
    Canvas canvas(termWidth, termHeight, 0.5, 30.0);

    // Draw Sun at center (0, 0)
    canvas.setPosition('O', YELLOW, 0.0, 0.0);

    // Draw orbits
    const int numPoints = 30; //Used for amount of tiny circles
    // Draw orbits with density based on circumference
    for (const auto& planet : planets) {
        double radius = planet.orbitRadius;

        // Calculate circumference and number of points needed
        double circumference = 2 * M_PI * radius;
        int pointsNeeded = static_cast<int>(circumference * .8); //Number here is the radius, so distance from each circle or how far they are from each other

        // Draw complete orbit
        for (int i = 0; i < pointsNeeded; i++) {
            double angle = (2.0 * M_PI * i) / pointsNeeded;
            double x = radius * cos(angle);
            double y = radius * sin(angle);
            canvas.setPosition('.', GRAY, x, y);
        }

        // Place planet at angle 0 (right side of orbit)
        canvas.setPosition(planet.symbol, planet.color, radius, 0.0);
    }

    // Render the canvas
    canvas.show();

    // Wait for user input
    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    // Restore cursor
    showCursor();
    clearScreen();

    return 0;
}