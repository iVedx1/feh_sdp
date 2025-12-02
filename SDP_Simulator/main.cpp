#include "FEHLCD.h"
#include "FEHUtility.h"
#include <string>
using namespace std;

// Button class
class Button {
public:
    string label;
    int x, y, w, h;

    Button(string text, int xPos, int yPos, int width, int height)
        : label(text), x(xPos), y(yPos), w(width), h(height) {}

    void draw() {
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, y, w, h);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    bool isPressed(float tx, float ty) {
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
    }
};

// touch handling function
void waitForTouch(float &x, float &y) {
    // Wait until a press is detected
    while (!LCD.Touch(&x, &y)) {}
    Sleep(0.15);  // debounce delay (150ms)

    // Wait until released to avoid double-detection
    while (LCD.Touch(&x, &y)) {}
}


void mainMenu();

// Play Screen
void playMenu() {
    LCD.Clear();
    LCD.WriteAt("Play game here", 10, 10);

    Button back("Return to Menu", 10, 40, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// Stats Screen
void statsMenu() {
    LCD.Clear();
    LCD.WriteAt("Stats", 10, 10);
    LCD.WriteAt("Games Played: 0", 10, 30);
    LCD.WriteAt("High Score: 0", 10, 50);
    LCD.WriteAt("Average Score: 0", 10, 70);
    LCD.WriteAt("Total Points: 0", 10, 90);

    Button back("Return to Menu", 10, 120, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// Instructions Screen
void instructionsMenu() {
    LCD.Clear();
    LCD.WriteAt("Instructions", 10, 10);
    LCD.WriteAt("1. Select Angle & Power", 10, 30);
    LCD.WriteAt("2. Press Fire", 10, 50);
    LCD.WriteAt("3. Take turns!", 10, 70);

    Button back("Return to Menu", 10, 110, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// Credits Screen
void creditsMenu() {
    LCD.Clear();
    LCD.WriteAt("Credits", 10, 10);
    LCD.WriteAt("Made by Vedant and Lucas", 10, 30);

    Button back("Return to Menu", 10, 70, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// main menu using buttons
void mainMenu() {
    LCD.Clear();

    LCD.WriteAt("Tank Game!", 50, 10);

    Button play("Play", 50, 50, 100, 30);
    Button stats("Stats", 50, 90, 100, 30);
    Button inst("Instructions", 50, 130, 160, 30);
    Button cred("Credits", 50, 170, 100, 30);
    Button exit("Exit", 50, 210, 100, 30);

    play.draw();
    stats.draw();
    inst.draw();
    cred.draw();
    exit.draw();

    float x, y;
    waitForTouch(x, y);

    if (play.isPressed(x, y)) playMenu();
    else if (stats.isPressed(x, y)) statsMenu();
    else if (inst.isPressed(x, y)) instructionsMenu();
    else if (cred.isPressed(x, y)) creditsMenu();
    else if (exit.isPressed(x, y)) {
        LCD.Clear();
        LCD.WriteAt("Exiting...", 50, 100);
        Sleep(1.0);
        return;
    }
}

// execution block
int main() {
    LCD.Clear();
    mainMenu();  // start program
    return 0;
}