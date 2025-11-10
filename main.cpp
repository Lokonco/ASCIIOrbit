//-------Imports-------//
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <sys/select.h>
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
void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void hideCursor() {
    std::cout << "\033[?25l" << std::flush;
}

void showCursor() {
    std::cout << "\033[?25h" << std::flush;
}

void getTerminalSize(int& width, int& height) {
    width = 80;
    height = 24;

#ifndef _WIN32
    FILE* fp = popen("tput cols", "r");
    if (fp) { fscanf(fp, "%d", &width); pclose(fp); }
    fp = popen("tput lines", "r");
    if (fp) { fscanf(fp, "%d", &height); pclose(fp); }
#endif
}

// Check if user pressed Enter (non-blocking)
bool checkForExit() {
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
        char c;
        read(STDIN_FILENO, &c, 1);
        return c == '\n' || c == 'q';
    }
    return false;
}
//---------------------------------------//

class Canvas {
public:
    int width, height;
    double span, span_x, span_y;
    std::vector<char> data;
    std::vector<std::string> colors;

    Canvas(int w, int h, double font_aspect = 0.5, double span_val = 30.0)
        : width(w), height(h), span(span_val) {
        data.resize(width * height, ' ');
        colors.resize(width * height, RESET);

        double apparent_width = width * font_aspect;
        if (apparent_width > height) {
            span_x = span / height * apparent_width;
            span_y = span;
        } else {
            span_x = span;
            span_y = span / apparent_width * height;
        }
    }

    void clear() {
        std::fill(data.begin(), data.end(), ' ');
        std::fill(colors.begin(), colors.end(), RESET);
    }

    void setPosition(char c, const std::string& color, double x, double y) {
        int sx = static_cast<int>(0.5 * (x / span_x + 1) * width);
        int sy = static_cast<int>(0.5 * (-y / span_y + 1) * height);

        if (sx < 0 || sx >= width || sy < 0 || sy >= height) return;

        int index = sy * width + sx;
        data[index] = c;
        colors[index] = color;
    }
};

// Structure to represent a planet
struct Planet {
    std::string name;
    char symbol;
    std::string color;
    double orbitRadius;
    double orbitalPeriod;
    double currentAngle;

    void getPosition(double& x, double& y) const {
        x = orbitRadius * cos(currentAngle);
        y = orbitRadius * sin(currentAngle);
    }

    void updatePosition(double deltaTime, double speedMultiplier = 1.0) {
        double angularVelocity = (2.0 * M_PI) / orbitalPeriod;
        currentAngle += angularVelocity * deltaTime * speedMultiplier;
        if (currentAngle > 2.0 * M_PI) currentAngle -= 2.0 * M_PI;
    }
};

//---------------------------------------//
int main() {
    // Planet setup
    std::vector<Planet> planets = {
        {"Mercury", 'M', GRAY, 4.0, std::pow(4.0, 1.5), 0.0},
        {"Venus", 'V', YELLOW, 7.0, std::pow(7.0, 1.5), 0.5},
        {"Earth", 'E', BLUE, 10.0, std::pow(10.0, 1.5), 1.0},
        {"Mars", 'm', RED, 13.0, std::pow(13.0, 1.5), 1.5},
        {"Jupiter", 'J', ORANGE, 17.0, std::pow(17.0, 1.5), 2.0},
        {"Saturn", 'S', YELLOW, 21.0, std::pow(21.0, 1.5), 2.5},
        {"Uranus", 'U', CYAN, 25.0, std::pow(25.0, 1.5), 3.0},
        {"Neptune", 'N', BLUE, 29.0, std::pow(29.0, 1.5), 3.5}
    };

    // Terminal setup
    int termWidth, termHeight;
    getTerminalSize(termWidth, termHeight);
    clearScreen();
    hideCursor();

    const double FPS = 30.0;
    const double frameTime = 1.0 / FPS;
    const double speedMultiplier = 0.5;

    // Base canvas: orbits only (Sun added each frame to ensure it's visible)
    Canvas baseCanvas(termWidth, termHeight, 0.5, 30.0);


    // Draw stars for background
    // Bug here only happening in orbit lines
    const std::string starColors[] = {GRAY, RESET, YELLOW, CYAN};
    int numStars = (termWidth * termHeight) / 50;  // density factor

    for (int i = 0; i < numStars; i++) {
        double x = (rand() % 200 - 100) / 100.0 * 30.0;  // random X in range -30..30
        double y = (rand() % 200 - 100) / 100.0 * 30.0;  // random Y in range -30..30
        baseCanvas.setPosition('.', starColors[rand() % 4], x, y);
    }



    for (const auto& planet : planets) {
        double radius = planet.orbitRadius;
        double circumference = 2 * M_PI * radius;
        int pointsNeeded = static_cast<int>(circumference * 0.8);

        for (int i = 0; i < pointsNeeded; i++) {
            double angle = (2.0 * M_PI * i) / pointsNeeded;
            double x = radius * cos(angle);
            double y = radius * sin(angle);
            baseCanvas.setPosition('.', GRAY, x, y);
        }
    }

    Canvas lastFrame = baseCanvas;
    Canvas frameCanvas = baseCanvas;

    auto lastFrameTime = std::chrono::steady_clock::now();

    std::cout << "Press Enter or 'q' to exit...\n" << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    clearScreen();

    while (true) {
        // Check for exit
        if (checkForExit()) {
            break;
        }

        auto currentTime = std::chrono::steady_clock::now();
        double deltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();

        if (deltaTime < frameTime)
            continue;

        lastFrameTime = currentTime;

        // Prepare frame
        frameCanvas = baseCanvas;

        // Draw Sun AFTER orbits to ensure it's on top
        frameCanvas.setPosition('O', YELLOW, 0.0, 0.0);

        // Update planet positions
        for (auto& planet : planets) {
            planet.updatePosition(deltaTime, speedMultiplier);
            double px, py;
            planet.getPosition(px, py);
            frameCanvas.setPosition(planet.symbol, planet.color, px, py);
        }

        // only draw changed cells
        std::cout << "\033[H";
        for (size_t i = 0; i < frameCanvas.data.size(); i++) {
            if (frameCanvas.data[i] != lastFrame.data[i] ||
                frameCanvas.colors[i] != lastFrame.colors[i]) {
                int y = i / frameCanvas.width;
                int x = i % frameCanvas.width;
                std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H"
                          << frameCanvas.colors[i] << frameCanvas.data[i] << RESET;
            }
        }

        std::cout.flush();
        lastFrame = frameCanvas;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    showCursor();
    clearScreen();
    return 0;
}
