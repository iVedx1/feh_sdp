#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include <string>
#include <math.h>
#include <cmath>
#include <stdlib.h>
using namespace std;
#define g 9.81
#define pi 3.14159265359

// A class that represents a clickable button on the screen
// Has a string label, position (x, y), width, and height
// Can draw itself and check if it is pressed based on touch coordinates
// Multiple draw styles are provided for different button appearances
// Methods: draw(), draw2(), draw3(), draw4(), draw5(), draw6(), isPressed(tx, ty)
// Written by Vedant Rakhonde
class Button {
public:
    string label; // button label
    int x, y, w, h; // position and size (x, y, width, height)

    // Simple constructor
    Button(string text, int xPos, int yPos, int width, int height)
        : label(text), x(xPos), y(yPos), w(width), h(height) {}

    // Draw button with black fill and white border
    void draw() {
        LCD.SetFontColor(BLACK); 
        LCD.FillRectangle(x, y, w, h);
        LCD.SetFontColor(WHITE); 
        LCD.DrawRectangle(x, y, w, h);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    // Draw button as a simple pause icon
    void draw2() {
        LCD.SetFontColor(WHITE);
        LCD.FillRectangle(x + 3, y, 2, h);
        LCD.FillRectangle(x + 8, y, 2, h);
    }

    // Draw button with arrows on sides
    void draw3(){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, y, 70, 30); 
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, 70, 30);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, 10, 30);
        LCD.WriteAt("<", x+1, y+9);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+10, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+10, y, 10, 30);
        LCD.WriteAt("<", x+11, y+9);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+61, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+61, y, 10, 30);
        LCD.WriteAt(">", x+63, y+9);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+71, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+71, y, 10, 30);
        LCD.WriteAt(">", x+73, y+9);
    }

    // Draw button with single arrows on sides
    void draw4(){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, y, 45, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, 45, 30);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, 10, 30);
        LCD.WriteAt("<", x+1, y+9);
        
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+35, y, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+35, y, 10, 30);
        LCD.WriteAt(">", x+37, y+9);
    }

    // Draw a button with a red filled background
    void draw5() {
        LCD.SetFontColor(RED);
        LCD.FillRectangle(x, y, w,h);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w,h);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    // Generic button with a white border
    void draw6() {
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
    }

    // Check if the button is pressed based on touch coordinates
    // Input: tx, ty - touch coordinates
    // Output: true if pressed, false otherwise
    bool isPressed(float tx, float ty) {
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
    }
};


// Touch handling helper to maintain consistent touch input behavior
// Inputs: references to float variables x and y to store touch coordinates
// Written by Vedant Rakhonde
void waitForTouch(float &x, float &y) {
    while (!LCD.Touch(&x, &y)) {}
    Sleep(0.15); // sleep to prevent multiple detections of the same real-world touch
    while (LCD.Touch(&x, &y)) {}
}

// Function prototypes
void mainMenu(); // Written by Vedant Rakhonde
void tankSelectMenu(); // Written by Lucas Tinter
void playMenu(); // Written by Vedant Rakhonde and Lucas Tinter
void statsMenu(); // Written by Vedant Rakhonde
void instructionsMenu(); // Written by Vedant Rakhonde
void creditsMenu(); // Written by Vedant Rakhonde
bool pauseMenu(); // Written by Lucas Tinter
void generateTerrain(); // Written by Lucas Tinter
void drawTerrain(); // Written by Lucas Tinter
void spawnTanks(int &tank1X, int &tank1Y, int &tank2X, int &tank2Y); // Written by Lucas Tinter
void endScreen(int p1Score, int p2Score); // Written by Vedant Rakhonde and Lucas Tinter
bool renderProjectileAndCheckHit(class Tank &shooter, class Tank &target); // Written by Vedant Rakhonde
void drawTerrainWithDelay(); // Written by Vedant Rakhonde

/// global variables for game state
string tank1Color = "";
string tank2Color = "";
int terrainHeight[320];

// Stats tracking global variables
int gamesPlayed = 0;
int totalP1Score = 0;
int totalP2Score = 0;
int highestScore = 0;

// A class that represents a player tank
// Attributes: color, position (x, y), size (width, height), angle, power, movesLeft, score, sprite image, isPlayer1
// Written by Vedant Rakhonde
class Tank {
public:
    string color; // color of the tank
    int x, y; // position of the tank
    int width; // width of the tank's hitbox
    int height; // height of the tank's hitbox
    int angle; // angle of the tank's turret
    int power; // power of the tank's shot
    int movesLeft; // remaining moves this round for the tank
    int score; // score of the tank
    FEHImage sprite; // sprite image of the tank
    bool isPlayer1; // true if player 1, false if player 2

    // Default constructor without any values in case something went wrong
    Tank() {
        color = "Blue";
        x = 0; y = 0;
        width = 12; height = 8;
        angle = 45;
        power = 50;
        movesLeft = 3;
        score = 0;
        isPlayer1 = true;
    }

    // Constructor with color and player number
    Tank(string c, bool p1) {
        color = c;
        width = 12; height = 8;
        angle = 45;
        power = 50;
        movesLeft = 3;
        score = 0;
        isPlayer1 = p1;
        x = 0; y = 0;
    }

    // Load the appropriate sprite based on color and player number
    void loadSprite() {
        if (isPlayer1) {
            sprite.Open((color + "TankP1.png").c_str());
        } else {
            sprite.Open((color + "TankP2.png").c_str());
        }
    }

    // Draw the tank at its current position
    void draw() {
        sprite.Draw(x, y);
    }

    // Respawn the tank at a given x position, adjusting y based on terrain height to place it on the ground
    void respawnAtX(int spawnX) {
        x = spawnX;
        if (x < 0) x = 0;
        if (x > 319) x = 319;
        y = terrainHeight[x] - height;
        if (y < 0) y = 0;
    }

    // Move the tank by a given delta x, adjusting y based on terrain height
    void moveBy(int dx) {
        if (movesLeft <= 0) return;
        x += dx;
        if (x < 0) x = 0;
        if (x > 320 - width) x = 320 - width;
        y = terrainHeight[x] - height;
        movesLeft--;
    }

    // Reset the number of moves left for the tank
    void resetMoves(int m = 3) {
        movesLeft = m;
    }

    // Check if a given point (px, py) hits the tank's hitbox
    bool isHit(int px, int py) {
        bool withinX = (px >= x) && (px <= x + width);
        bool withinY = (py >= y) && (py <= y + height);
        return (withinX && withinY);
    }
};

// Generates terrain heights for the game by creating a random walk and running 4 smoothing passes
void generateTerrain()
{
    int h = 150;

    for (int x = 0; x < 320; x++) {
        int change = (rand() % 7) - 3;
        h += change;
        if (h < 100) h = 100;
        if (h > 200) h = 200;
        terrainHeight[x] = h;
    }

    for (int pass = 0; pass < 4; pass++) {
        int temp[320];
        for (int x = 0; x < 320; x++) {
            int sum = terrainHeight[x] * 4;
            int count = 4;
            if (x > 0) { sum += terrainHeight[x-1] * 2; count += 2; }
            if (x < 319) { sum += terrainHeight[x+1] * 2; count += 2; }
            temp[x] = sum / count;
        }
        for (int x = 0; x < 320; x++) terrainHeight[x] = temp[x];
    }
}

// draw the terrain on the screen, add a delay for visual effect
void drawTerrainWithDelay() {
    LCD.SetFontColor(GREEN);
    for (int i = 0; i < 320; i++) {
        LCD.DrawLine(i, terrainHeight[i], i, 240);
        Sleep(0.001);
    }
}

// draw the terrain on the screen without delay
void drawTerrain() {
    LCD.SetFontColor(GREEN);
    for (int i = 0; i < 320; i++) {
        LCD.DrawLine(i, terrainHeight[i], i, 240);
    }
}

// Spawn tanks at random x positions on the terrain
void spawnTanks(int &tank1X, int &tank1Y, int &tank2X, int &tank2Y)
{
    tank1X = rand() % 80 + 20;
    tank2X = rand() % 80 + 220;

    if (tank1X < 0) tank1X = 0;
    if (tank1X > 319) tank1X = 319;
    if (tank2X < 0) tank2X = 0;
    if (tank2X > 319) tank2X = 319;

    tank1Y = terrainHeight[tank1X] - 8;
    tank2Y = terrainHeight[tank2X] - 8;
}

// pause menu that allows the player to resume or return to main menu
// Returns true to resume, false to return to main menu
bool pauseMenu()
{
    LCD.Clear();
    LCD.WriteAt("Game Paused", 100, 40);

    Button resumeBtn("Resume", 80, 100, 160, 40);
    Button menuBtn("Main Menu", 80, 160, 160, 40);

    resumeBtn.draw();
    menuBtn.draw();

    float x, y;
    while (true){
        waitForTouch(x, y);
        if (resumeBtn.isPressed(x, y)) {
            return true;
        }
        if (menuBtn.isPressed(x, y)) {
            return false;
        }
    }
}

// end screen showing final scores and options to play again or return to main menu
// Updates global stats variables
// Inputs: p1Score, p2Score - final scores of player 1 and player 2
void endScreen(int p1Score, int p2Score) {
    
    // Update stats
    gamesPlayed++;
    totalP1Score += p1Score;
    totalP2Score += p2Score;
    if (p1Score > highestScore) highestScore = p1Score;
    if (p2Score > highestScore) highestScore = p2Score;
    
    LCD.Clear();

    LCD.SetFontColor(WHITE);
    LCD.SetFontScale(2);
    LCD.WriteAt("GAME OVER", 45, 20);

    LCD.SetFontScale(1);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Player 1 Score:", 35, 80);
    LCD.WriteAt(to_string(p1Score).c_str(), 220, 80);

    LCD.WriteAt("Player 2 Score:", 35, 120);
    LCD.WriteAt(to_string(p2Score).c_str(), 220, 120);

    if (p1Score > p2Score) {
        LCD.SetFontColor(GREEN);
        LCD.WriteAt("Winner: PLAYER 1!", 35, 160);
    } else if (p2Score > p1Score) {
        LCD.SetFontColor(GREEN);
        LCD.WriteAt("Winner: PLAYER 2!", 35, 160);
    } else {
        LCD.SetFontColor(YELLOW);
        LCD.WriteAt("TIE GAME!", 70, 160);
    }

    LCD.SetFontColor(WHITE);
    Button playAgainBtn("Play Again", 30, 200, 130, 30);
    Button exitBtn("Main Menu", 166, 200, 125, 30);
    
    playAgainBtn.draw();
    exitBtn.draw();

    float x, y;
    while (true) {
        waitForTouch(x, y);
        if (playAgainBtn.isPressed(x, y)) {
            playMenu();
            return;
        }
        if (exitBtn.isPressed(x, y)) {
            mainMenu();
            return;
        }
    }
}

// Render the projectile's trajectory and check for hits on the target tank
// Inputs: shooter - the tank that is shooting
//         target - the tank that is being targeted
// Returns: true if the target tank is hit, false otherwise
// Displays hit or miss and updates shooter's score if hit
// Uses physics equations for projectile motion and simple animation
bool renderProjectileAndCheckHit(Tank &shooter, Tank &target)
{
    LCD.SetFontColor(WHITE);
    float angleRad = shooter.angle * (pi / 180.0f);
    float dir = (shooter.isPlayer1 ? 1.0f : -1.0f);
    float vx = shooter.power * cos(angleRad) * dir;
    float vy = shooter.power * sin(angleRad);

    float x0 = shooter.x + shooter.width / 2;
    float y0 = shooter.y;
    
    float animationSpeed = 0.9f;
    float baseTimeStep = 0.015f + (shooter.power / 100.0f) * 0.03f;
    float sleepDuration = 0.005f;
    
    const int trailLength = 8;
    int trailX[trailLength];
    int trailY[trailLength];
    int trailPos = 0;
    
    for (int i = 0; i < trailLength; i++) {
        trailX[i] = -1;
    }

    for (float t = 0.0f; t < 100.0f; ) {
        float currentVy = vy - g * t;
        float speed = sqrt(vx * vx + currentVy * currentVy);
        
        float speedFactor = speed / (shooter.power + 1.0f);
        float dt = baseTimeStep * (1.0f + speedFactor) * animationSpeed;
        t += dt;
        
        int x = (int)round(x0 + vx * t);
        int y = (int)round(y0 - (vy * t - 0.5f * g * t * t));

        if (x < 0 || x >= 320 || y < 0 || y >= 240) break;
        
        if (y >= terrainHeight[x]) {
            LCD.SetFontColor(WHITE);
            LCD.WriteAt("Miss!", 140, 120);
            Sleep(0.5);
            break;
        }
        
        if (target.isHit(x, y)) {
            LCD.SetFontColor(RED);
            LCD.WriteAt("HIT!", 140, 120);
            Sleep(0.6);
            shooter.score++;
            return true;
        }

        if (trailX[trailPos] >= 0) {
            LCD.SetFontColor(GRAY);
            LCD.DrawPixel(trailX[trailPos], trailY[trailPos]);
        }
        
        trailX[trailPos] = x;
        trailY[trailPos] = y;
        trailPos = (trailPos + 1) % trailLength;
        
        LCD.SetFontColor(WHITE);
        LCD.DrawPixel(x, y);
        
        Sleep(sleepDuration);
        LCD.Update();
    }

    return false;
}

// tank selection menu allowing players to choose their tank colors
// Updates global variables tank1Color and tank2Color
void tankSelectMenu() {
    while (true) {
        LCD.Clear();
        LCD.SetFontColor(WHITE);

        FEHImage tankselectbackground;
        tankselectbackground.Open("TankSelectBack2.png");
        tankselectbackground.Draw(0,0);

        LCD.WriteAt("Select Tank Colors", 60, 10);

        Button p1Blue("Blue", 15, 40, 47, 47);
        Button p1Green("Green", 15, 97, 47, 47);
        Button p1Red("Red", 15, 156, 47, 47);
        Button p1Pink("Pink",67,40,47,47);
        Button p1Yellow("Yellow",67,97,47,47);
        Button p1Purple("Purple",67,156,47,47);

        Button p2Blue("Blue", 256, 40, 47, 47);
        Button p2Green("Green", 256, 97, 47, 47);
        Button p2Red("Red", 256, 156, 47, 47);
        Button p2Pink("Pink",204,40,47,47);
        Button p2Yellow("Yellow",204,97,47,47);
        Button p2Purple("Purple",204,156,47,47);
        Button confirm("Start", 125, 40, 70, 30);

        p1Blue.draw6(); 
        p1Green.draw6(); 
        p1Red.draw6();
        p1Yellow.draw6();
        p1Purple.draw6();
        p1Pink.draw6();
        p2Blue.draw6(); 
        p2Green.draw6(); 
        p2Red.draw6();
        p2Yellow.draw6();
        p2Purple.draw6();
        p2Pink.draw6();
        confirm.draw();

        LCD.WriteAt(("P1: " + tank1Color).c_str(), 15, 210);
        LCD.WriteAt(("P2: " + tank2Color).c_str(), 200, 210);

        float x, y;
        waitForTouch(x, y);

        if (p1Blue.isPressed(x, y))   tank1Color = "Blue";
        else if (p1Green.isPressed(x, y)) tank1Color = "Green";
        else if (p1Red.isPressed(x, y))   tank1Color = "Red";
        else if (p1Pink.isPressed(x, y))   tank1Color = "Pink";
        else if (p1Yellow.isPressed(x, y)) tank1Color = "Yellow";
        else if (p1Purple.isPressed(x, y))   tank1Color = "Purple";

        else if (p2Blue.isPressed(x, y))   tank2Color = "Blue";
        else if (p2Green.isPressed(x, y)) tank2Color = "Green";
        else if (p2Red.isPressed(x, y))   tank2Color = "Red";
        else if (p2Pink.isPressed(x, y))   tank2Color = "Pink";
        else if (p2Yellow.isPressed(x, y)) tank2Color = "Yellow";
        else if (p2Purple.isPressed(x, y))   tank2Color = "Purple";

        else if (confirm.isPressed(x, y)) {
            if (tank1Color != "" && tank2Color != "") {
                return;
            } 
            else {
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(50, 100, 214, 20);
                LCD.SetFontColor(WHITE);
                LCD.DrawRectangle(50, 100, 214, 20);
                LCD.SetFontColor(RED);
                LCD.WriteAt("Select both tanks!", 50, 100);
                LCD.SetFontColor(BLACK);
                Sleep(1.0);
            }
        }
    }
}

// an enum that represents which player's turn it is
enum Turn { PLAYER1, PLAYER2 };

Turn nextTurn(Turn cur) {
    return (cur == PLAYER1 ? PLAYER2 : PLAYER1);
}

// Main gameplay loop handling turns, drawing UI, and managing game state
void playMenu() {
    generateTerrain();
    drawTerrainWithDelay();
    srand(TimeNow());

    int t1x, t1y, t2x, t2y;
    spawnTanks(t1x, t1y, t2x, t2y);

    Tank P1(tank1Color, true);
    Tank P2(tank2Color, false);
    P1.respawnAtX(t1x);
    P2.respawnAtX(t2x);
    P1.loadSprite();
    P2.loadSprite();
    P1.score = 0;
    P2.score = 0;
    P1.resetMoves(3);
    P2.resetMoves(3);

    Turn turn = PLAYER1;
    int roundsPlayed = 0;
    const int MAX_ROUNDS = 10;

    while (roundsPlayed < MAX_ROUNDS) {
        LCD.Clear();

        FEHImage background;
        background.Open("NightSky1.png");
        background.Draw(0,0);

        drawTerrain();
        P1.draw();
        P2.draw();

        LCD.SetFontColor(GRAY);
        LCD.FillRectangle(0, 0, 320, 20);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(0, 0, 320, 20);
        LCD.SetFontScale(0.5);
        
        FEHImage tankmockP1;
        tankmockP1.Open((P1.color + "TankP1.png").c_str());
        tankmockP1.Draw(5, 7);
        LCD.WriteAt("P1:", 20, 5);
        LCD.WriteAt(to_string(P1.score).c_str(), 40, 5);

        // Display round counter
        LCD.WriteAt("Round:", 118, 5);
        LCD.WriteAt(to_string((roundsPlayed / 2) + 1).c_str(), 154, 5);
        LCD.WriteAt("/5", 159, 5);

        LCD.WriteAt(to_string(P2.score).c_str(), 270, 5);
        LCD.WriteAt(":P2", 280, 5);
        FEHImage tankmockP2;
        tankmockP2.Open((P2.color + "TankP2.png").c_str());
        tankmockP2.Draw(302, 7);

        LCD.SetFontScale(1);

        LCD.SetFontColor(GRAY);
        LCD.FillRectangle(0, 200, 320, 40);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(0, 200, 320, 40);

        LCD.SetFontScale(1);

        // Moves label and controls
        Button moveLeft("<", 10, 206, 22, 28);
        Button moveRight(">", 46, 206, 22, 28);

        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(10,206,50,28);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(10,206,50,28);
        LCD.SetFontColor(WHITE);
        LCD.SetFontScale(0.5);
        LCD.WriteAt("M", 36, 205);
        LCD.SetFontScale(1);
        moveLeft.draw();
        moveRight.draw();
        LCD.SetFontScale(.5);
        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.movesLeft).c_str(), 36, 215);
        } else {
            LCD.WriteAt(to_string(P2.movesLeft).c_str(), 36, 215);
        }
        LCD.SetFontScale(1);
        // Angle label and controls
        Button angDecBig("<", 75, 206, 22, 28);
        Button angDec("<", 98, 206, 22, 28);
        Button angInc(">", 146, 206, 22, 28);
        Button angIncBig(">", 169, 206, 22, 28);

        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(98,206,50,28);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(98,206,50,28);
        LCD.SetFontColor(WHITE);
        LCD.SetFontScale(0.5);
        LCD.WriteAt("ANG", 124, 205);
        LCD.SetFontScale(1);
        angDecBig.draw();
        angDec.draw();
        angInc.draw();
        angIncBig.draw();
        LCD.SetFontScale(.5);
        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.angle).c_str(), 126, 215);
        } else {
            LCD.WriteAt(to_string(P2.angle).c_str(), 126, 215);
        }
        LCD.SetFontScale(1);
        // Power label and controls
        Button powDec("<", 198, 206, 22, 28);
        Button powInc(">", 246, 206, 22, 28);

        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(198,206,60,28);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(198,206,60,28);
        LCD.SetFontColor(WHITE);
        LCD.SetFontScale(0.5);
        LCD.WriteAt("POW", 223, 205);
        LCD.SetFontScale(1);
        powDec.draw();
        powInc.draw();
        LCD.SetFontScale(.5);
        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.power).c_str(), 226, 215);
        } else {
            LCD.WriteAt(to_string(P2.power).c_str(), 226, 215);
        }

        LCD.SetFontScale(1);

        Button fireBtn("", 278, 206, 26,26); 
        LCD.SetFontColor(RED);
        LCD.FillCircle(290, 218, 13);
        LCD.SetFontColor(WHITE);
        LCD.DrawCircle(290, 218, 13);

        Button pauseBtn("", 5, 26, 12, 12);
        pauseBtn.draw2();

        float tx, ty;
        waitForTouch(tx, ty);

        if (pauseBtn.isPressed(tx, ty)) {
            bool resume = pauseMenu();
            if (!resume) { mainMenu(); return; }
            else continue;
        }

        if (moveLeft.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.moveBy(-5);
            } else {
                P2.moveBy(-5);
            }
            continue;
        }
        if (moveRight.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.moveBy(+5);
            } else {
                P2.moveBy(+5);
            }
            continue;
        }

        if (angDecBig.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.angle -= 5;
                if (P1.angle < 0) P1.angle = 0;
            } else {
                P2.angle -= 5;
                if (P2.angle < 0) P2.angle = 0;
            }
            continue;
        }
        if (angDec.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.angle -= 1;
                if (P1.angle < 0) P1.angle = 0;
            } else {
                P2.angle -= 1;
                if (P2.angle < 0) P2.angle = 0;
            }
            continue;
        }
        if (angInc.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.angle += 1;
                if (P1.angle > 90) P1.angle = 90;
            } else {
                P2.angle += 1;
                if (P2.angle > 90) P2.angle = 90;
            }
            continue;
        }
        if (angIncBig.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.angle += 5;
                if (P1.angle > 90) P1.angle = 90;
            } else {
                P2.angle += 5;
                if (P2.angle > 90) P2.angle = 90;
            }
            continue;
        }

        if (powDec.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.power -= 1;
                if (P1.power < 0) P1.power = 0;
            } else {
                P2.power -= 1;
                if (P2.power < 0) P2.power = 0;
            }
            continue;
        }
        if (powInc.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.power += 1;
                if (P1.power > 100) P1.power = 100;
            } else {
                P2.power += 1;
                if (P2.power > 100) P2.power = 100;
            }
            continue;
        }

        if (fireBtn.isPressed(tx, ty)) {
            if (turn == PLAYER1) {
                P1.resetMoves(3);
                renderProjectileAndCheckHit(P1, P2);
            } else {
                P2.resetMoves(3);
                renderProjectileAndCheckHit(P2, P1);
            }

            roundsPlayed++;
            turn = nextTurn(turn);
            Sleep(0.2);
            
            // Check if we've completed 10 rounds
            if (roundsPlayed >= MAX_ROUNDS) {
                endScreen(P1.score, P2.score);
                return;
            }
            continue;
        }
    }
}

// stats menu displaying game statistics including games played, highest score, total points, and average score per player
void statsMenu() {
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    
    LCD.SetFontScale(2);
    LCD.WriteAt("Statistics", 70, 20);
    LCD.SetFontScale(1);
    
    LCD.WriteAt("Games Played:", 40, 70);
    LCD.WriteAt(to_string(gamesPlayed).c_str(), 220, 70);
    
    LCD.WriteAt("Highest Score:", 40, 100);
    LCD.WriteAt(to_string(highestScore).c_str(), 220, 100);
    
    int totalPoints = totalP1Score + totalP2Score;
    LCD.WriteAt("Total Points:", 40, 130);
    LCD.WriteAt(to_string(totalPoints).c_str(), 220, 130);
    
    if (gamesPlayed > 0) {
        float avgScore = (float)totalPoints / (float)(gamesPlayed * 2);
        LCD.WriteAt("Avg Score:", 40, 160);
        char avgStr[10];
        sprintf(avgStr, "%.1f", avgScore);
        LCD.WriteAt(avgStr, 220, 160);
    } else {
        LCD.WriteAt("Avg Score/Player:", 40, 160);
        LCD.WriteAt("N/A", 220, 160);
    }

    Button back("Return to Menu", 70, 200, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// instructions menu displaying game instructions
void instructionsMenu() {
    LCD.Clear();
    LCD.WriteAt("Instructions", 10, 10);
    LCD.WriteAt("1. Select Angle & Power", 10, 30);
    LCD.WriteAt("2. Press Fire", 10, 50);
    LCD.WriteAt("3. Take turns!", 10, 70);
    LCD.WriteAt("4. First to hit most in", 10, 90);
    LCD.WriteAt("   5 rounds wins!", 10, 110);

    Button back("Return to Menu", 10, 140, 180, 30);
    back.draw();

    float x, y;
    waitForTouch(x, y);

    if (back.isPressed(x, y)) {
        mainMenu();
    }
}

// credits menu displaying game credits
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

// main menu displaying options to play, view stats, instructions, credits, or exit
// Navigates to the appropriate menu based on user selection
void mainMenu() {
    LCD.Clear();

    FEHImage backgroundmain;
    backgroundmain.Open("MAINBACK.png");
    backgroundmain.Draw(0,0);

    Button play("Play", 130, 55, 60, 30);
    Button stats("Stats", 125, 90, 75, 30);
    Button inst("Instructions", 85, 125, 160, 30);
    Button cred("Credits", 114, 160, 100, 30);
    Button exit("Exit", 130, 195, 60, 30);

    play.draw();
    stats.draw();
    inst.draw();
    cred.draw();
    exit.draw();

    float x, y;
    waitForTouch(x, y);
    if (play.isPressed(x, y)) {
        tankSelectMenu();
        playMenu();
    }
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

// main function initializing the program
int main() {
    LCD.Clear();
    mainMenu();
    return 0;
}