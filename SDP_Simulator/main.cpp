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

// ---------- Button Class ----------
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

    void draw2() {
        LCD.SetFontColor(WHITE);
        LCD.FillRectangle(x + 3, y, 2, h);
        LCD.FillRectangle(x + 8, y, 2, h);
    }

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

    void draw5() {
        LCD.SetFontColor(RED);
        LCD.FillRectangle(x, y, w, h);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    void draw6() {
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
    }

    bool isPressed(float tx, float ty) {
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
    }
};


// ---------- Touch handling ----------
void waitForTouch(float &x, float &y) {
    while (!LCD.Touch(&x, &y)) {}
    Sleep(0.15);
    while (LCD.Touch(&x, &y)) {}
}

// prototypes
void mainMenu();
void tankSelectMenu();
void playMenu();
void statsMenu();
void instructionsMenu();
void creditsMenu();
bool pauseMenu();
void generateTerrain();
void drawTerrain();
void spawnTanks(int &tank1X, int &tank1Y, int &tank2X, int &tank2Y);
void endScreen(int p1Score, int p2Score);

// ---------- Globals ----------
string tank1Color = "";
string tank2Color = "";
int terrainHeight[320];

// ---------- Tank class ----------
class Tank {
public:
    string color;
    int x, y;
    int width;
    int height;
    int angle;
    int power;
    int movesLeft;
    int score;
    FEHImage sprite;
    bool isPlayer1;

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

    void loadSprite() {
        if (isPlayer1) {
            sprite.Open((color + "TankP1.png").c_str());
        } else {
            sprite.Open((color + "TankP2.png").c_str());
        }
    }

    void draw() {
        sprite.Draw(x, y);
    }

    void respawnAtX(int spawnX) {
        x = spawnX;
        if (x < 0) x = 0;
        if (x > 319) x = 319;
        y = terrainHeight[x] - height;
        if (y < 0) y = 0;
    }

    void moveBy(int dx) {
        if (movesLeft <= 0) return;
        x += dx;
        if (x < 0) x = 0;
        if (x > 320 - width) x = 320 - width;
        y = terrainHeight[x] - height;
        movesLeft--;
    }

    void resetMoves(int m = 3) {
        movesLeft = m;
    }

    bool isHit(int px, int py) {
        bool withinX = (px >= x) && (px <= x + width);
        bool withinY = (py >= y) && (py <= y + height);
        return (withinX && withinY);
    }
};

// ---------- Terrain ----------
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

void drawTerrain()
{
    LCD.SetFontColor(GREEN);
    for (int i = 0; i < 320; i++) {
        LCD.DrawLine(i, terrainHeight[i], i, 240);
    }
}

// ---------- Spawn helper ----------
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

// ---------- Pause Menu ----------
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

// ---------- End screen ----------
void endScreen(int p1Score, int p2Score) {
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
        LCD.WriteAt("Winner: PLAYER 1", 35, 170);
    } else if (p2Score > p1Score) {
        LCD.WriteAt("Winner: PLAYER 2", 35, 170);
    } else {
        LCD.WriteAt("TIE GAME!", 35, 170);
    }

    LCD.WriteAt("Tap to return to menu", 35, 210);

    float x, y;
    waitForTouch(x, y);
    mainMenu();
}

// ---------- Projectile & firing ----------
bool renderProjectileAndCheckHit(Tank &shooter, Tank &target)
{
    LCD.SetFontColor(WHITE);
    float angleRad = shooter.angle * (pi / 180.0f);
    float dir = (shooter.isPlayer1 ? 1.0f : -1.0f);
    float vx = shooter.power * cos(angleRad) * dir;
    float vy = shooter.power * sin(angleRad);

    float x0 = shooter.x + shooter.width / 2;
    float y0 = shooter.y;
    
    // TUNING PARAMETERS - adjust these to control speed
    float animationSpeed = 0.9f;  // Overall speed multiplier (lower = slower)
    float baseTimeStep = 0.015f + (shooter.power / 100.0f) * 0.03f;
    float sleepDuration = 0.005f;  // Time between frames
    
    // Trail storage
    const int trailLength = 8;  // Shorter trail = faster fade
    int trailX[trailLength];
    int trailY[trailLength];
    int trailPos = 0;
    
    // Initialize trail
    for (int i = 0; i < trailLength; i++) {
        trailX[i] = -1;
    }

    for (float t = 0.0f; t < 100.0f; ) {
        // Adaptive time step based on current velocity and power
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

        // Erase the oldest trail point
        if (trailX[trailPos] >= 0) {
            LCD.SetFontColor(BLACK);  // Background color
            LCD.DrawPixel(trailX[trailPos], trailY[trailPos]);
        }
        
        // Store current position
        trailX[trailPos] = x;
        trailY[trailPos] = y;
        trailPos = (trailPos + 1) % trailLength;
        
        // Draw current projectile
        LCD.SetFontColor(WHITE);
        LCD.DrawPixel(x, y);
        
        Sleep(sleepDuration);
        LCD.Update();
    }

    return false;
}


// ---------- Menus: Tank select ----------
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

// ---------- Play Menu ----------
enum Turn { PLAYER1, PLAYER2 };

Turn nextTurn(Turn cur) {
    return (cur == PLAYER1 ? PLAYER2 : PLAYER1);
}

// Main gameplay loop
void playMenu() {
    generateTerrain();
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

    while (true) {
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

        LCD.WriteAt("Turn:", 138, 5);
        LCD.WriteAt((turn == PLAYER1 ? "P1" : "P2"), 170, 5);

        LCD.WriteAt(to_string(P2.score).c_str(), 270, 5);
        LCD.WriteAt(":P2", 285, 5);
        FEHImage tankmockP2;
        tankmockP2.Open((P2.color + "TankP2.png").c_str());
        tankmockP2.Draw(302, 7);

        LCD.SetFontScale(1);

        LCD.SetFontColor(GRAY);
        LCD.FillRectangle(0, 200, 320, 40);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(0, 200, 320, 40);

        LCD.SetFontScale(1);

        // Movement controls - spans approximately 100 pixels
        Button moveLeft("<", 10, 206, 22, 28);
        Button moveRight(">", 65, 206, 22, 28);

        moveLeft.draw();
        moveRight.draw();

        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.movesLeft).c_str(), 41, 210);
        } else {
            LCD.WriteAt(to_string(P2.movesLeft).c_str(), 41, 210);
        }

        // Angle controls - spans approximately 100 pixels
        Button angDecBig("<", 102, 206, 22, 28);
        Button angDec("<", 126, 206, 22, 28);
        Button angInc(">", 178, 206, 22, 28);
        Button angIncBig(">", 202, 206, 22, 28);

        angDecBig.draw();
        angDec.draw();
        angInc.draw();
        angIncBig.draw();

        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.angle).c_str(), 150, 210);
        } else {
            LCD.WriteAt(to_string(P2.angle).c_str(), 150, 210);
        }

        // Power controls - spans approximately 90 pixels
        Button powDec("<", 234, 206, 22, 28);
        Button powInc(">", 289, 206, 22, 28);

        powDec.draw();
        powInc.draw();

        LCD.SetFontColor(WHITE);
        if (turn == PLAYER1) {
            LCD.WriteAt(to_string(P1.power).c_str(), 260, 210);
        } else {
            LCD.WriteAt(to_string(P2.power).c_str(), 260, 210);
        }

        LCD.SetFontScale(1);

        // FIRE button - centered above the control bar
        Button fireBtn("FIRE", 130, 173, 60, 28); 
        fireBtn.draw5();

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

            turn = nextTurn(turn);
            Sleep(0.2);
            continue;
        }
    }
}

// ---------- Other menus ----------
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

// ---------- Main Menu ----------
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

// ---------- Program entry ----------
int main() {
    LCD.Clear();
    mainMenu();
    return 0;
}
