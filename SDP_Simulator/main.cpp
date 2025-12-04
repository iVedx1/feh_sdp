#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include <string>
#include <math.h>
using namespace std;
#define g 9.81
#define pi 3.14159265359

// -------------------------
// Button Class
// -------------------------
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
        LCD.FillRectangle(5, 26, 1, 7);   // left bar
        LCD.FillRectangle(10, 26, 1, 7);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    void draw3(){
        // Primary Button
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, 205, 70, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, 205, 70, 30);
        // Large Increment/Decrement Left
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, 205, 10, 30);
        LCD.WriteAt("<",x+1,214);
        // Small Increment/Decrement Left
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+10, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+10, 205, 10, 30);
        LCD.WriteAt("<",x+11,214);
        // Small Increment/Decrement Right
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+61, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+61, 205, 10, 30);
        LCD.WriteAt(">",x+63,214);
        // Large Increment/Decrement Right
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+71, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+71, 205, 10, 30);
        LCD.WriteAt(">",x+73,214);
        // Number Indicator

    }

    void draw4(){
        // Primary Button
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, 205, 45, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, 205, 45, 30);
        // Decrement
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, 205, 10, 30);
        LCD.WriteAt("<",x+1,214);
        // Increment 
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x+35, 205, 10, 30);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x+35, 205, 10, 30);
        LCD.WriteAt(">",x+37,214);
        // Number Indicator

    }

    void draw5() {
        LCD.SetFontColor(RED);
        LCD.FillRectangle(x, y, w, h);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
        LCD.WriteAt(label.c_str(), x + 5, y + 5);
    }

    void draw6(){
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(x, y, w, h);
    }

    bool isPressed(float tx, float ty) {
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
    }
};

// -------------------------
// Touch handling with debouncing
// -------------------------
void waitForTouch(float &x, float &y) {
    // Wait until a press is detected
    while (!LCD.Touch(&x, &y)) {}
    Sleep(0.15);  // debounce delay (150ms)
    // Wait until released to avoid double-detection
    while (LCD.Touch(&x, &y)) {}
}

void mainMenu();

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
        // Resume returns true
        if (resumeBtn.isPressed(x, y)) {
            return true;
        }
        // Return to main menu if return false so playMenu can exit
        if (menuBtn.isPressed(x, y)) {
            return false;
        }
        return true; // default to resume
    }
}
// Defines string color
string tank1Color = "";
string tank2Color = "";

// Tank Color Selection
void tankSelectMenu() {
    while (true) {
        LCD.Clear();
        LCD.SetFontColor(WHITE);

        FEHImage tankselectbackground;
        tankselectbackground.Open("TankSelectBack2.png");
        tankselectbackground.Draw(0,0);

        LCD.WriteAt("Select Tank Colors", 60, 10);

        // --- Player 1 options ---
        Button p1Blue("Blue", 15, 40, 47, 47);
        Button p1Green("Green", 15, 97, 47, 47);
        Button p1Red("Red", 15, 156, 47, 47);
        Button p1Pink("Pink",67,40,47,47);
        Button p1Yellow("Yellow",67,97,47,47);
        Button p1Purple("Purple",67,156,47,47);

        // --- Player 2 options ---
        Button p2Blue("Blue", 256, 40, 47, 47); // Top Left
        Button p2Green("Green", 256, 97, 47, 47); // Middle Left
        Button p2Red("Red", 256, 156, 47, 47); // Bottom Left
        Button p2Pink("Pink",204,40,47,47); // Top Riught
        Button p2Yellow("Yellow",204,97,47,47); // Middle Right
        Button p2Purple("Purple",204,156,47,47); // Bottom Right
        Button confirm("Start", 125, 40, 70, 30);

        // Draw
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

        // Show current selections
        LCD.WriteAt(("P1: " + tank1Color).c_str(), 15, 210);
        LCD.WriteAt(("P2: " + tank2Color).c_str(), 200, 210);

        float x, y;
        waitForTouch(x, y);

        // Player 1
        if (p1Blue.isPressed(x, y))   tank1Color = "Blue";
        else if (p1Green.isPressed(x, y)) tank1Color = "Green";
        else if (p1Red.isPressed(x, y))   tank1Color = "Red";
        else if (p1Pink.isPressed(x, y))   tank1Color = "Pink";
        else if (p1Yellow.isPressed(x, y)) tank1Color = "Yellow";
        else if (p1Purple.isPressed(x, y))   tank1Color = "Purple";

        // Player 2
        else if (p2Blue.isPressed(x, y))   tank2Color = "Blue";
        else if (p2Green.isPressed(x, y)) tank2Color = "Green";
        else if (p2Red.isPressed(x, y))   tank2Color = "Red";
        else if (p2Pink.isPressed(x, y))   tank2Color = "Pink";
        else if (p2Yellow.isPressed(x, y)) tank2Color = "Yellow";
        else if (p2Purple.isPressed(x, y))   tank2Color = "Purple";


        // Only allow leaving if both selected
        else if (confirm.isPressed(x, y)) {
            if (tank1Color != "" && tank2Color != "") {
                return; // leave menu → start game
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


// Play Screen
void playMenu() {
    int powerValue = 50;   // default
    int angleValue = 45;   // default
    int moveperturn = 3;
    int tank1X = 30;      // starting x-position
    int tank1Y = 172;
    int tank2X = 150;      // starting x-position
    int tank2Y = 172;     
    int tankWidth = 12;
    int tankHeight = 8;


    while(true){
        LCD.Clear();
        // Reference Lines
        LCD.DrawVerticalLine(160, 0, 240 );
        LCD.DrawHorizontalLine(120,0,320);

        int groundY = 180;
        LCD.SetFontColor(GREEN);
        LCD.FillRectangle(0, groundY, 320, 60);

        FEHImage tank1;
        tank1.Open((tank1Color + "TankP1.png").c_str());
        tank1.Draw(tank1X, tank1Y);

        FEHImage tank2;
        tank2.Open((tank2Color + "TankP2.png").c_str());
        tank2.Draw(tank2X, tank2Y);


        // top part of UI
        LCD.SetFontColor(GRAY);
        LCD.FillRectangle(0, 0, 320, 20);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(0, 0, 320, 20);
        LCD.SetFontScale(0.5);
        FEHImage tankmockP1;
        tankmockP1.Open((tank1Color + "TankP1.png").c_str());
        tankmockP1.Draw(5,7);
        LCD.WriteAt("P1:", 20, 4);
        FEHImage tankmockP2;
        tankmockP2.Open((tank2Color + "TankP2.png").c_str());
        tankmockP2.Draw(302,7);
        LCD.WriteAt(":P2", 280, 5);
        LCD.WriteAt("Volleys:", 138, 5);
        LCD.SetFontScale(1);
        
        // Botton part of hud
        LCD.SetFontColor(GRAY);
        LCD.FillRectangle(0, 200, 320, 40);
        LCD.SetFontColor(WHITE);
        LCD.DrawRectangle(0, 200, 320, 40);

        LCD.SetFontScale(.5);
        Button fireBtn("FIRE!",278,210,37,20);
        fireBtn.draw5();

        LCD.SetFontColor(BLACK);
        // Power Select Button
        Button Pow("",5,0,0,0);
        Pow.draw3();
        LCD.WriteAt("POW",35,205);
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(powerValue, 40, 217);
        LCD.SetFontColor(BLACK);
        // Pow Large Decrement Button
        Button PowLD("",5,205,10,30);
        // Pow Small Decrement Button
        Button PowSD("",15,205,10,30);
        // Pow Small Increment Button
        Button PowSI("",66,205,10,30);
        // Pow Large Increment Button
        Button PowLI("",76,205,10,30);
        
        // Angle Select Button
        Button Ang("",95,0,0,0);
        Ang.draw3();
        LCD.WriteAt("ANG",125,205);
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(angleValue, 125, 217);
        LCD.SetFontColor(BLACK);
        // Ang Large Decrement Button
        Button AngLD("",95,205,10,30);
        // Ang Small Decrement Button
        Button AngSD("",105,205,10,30);
        // Ang Small Increment Button
        Button AngSI("",156,205,10,30);
        // Ang Large Increment Button
        Button AngLI("",166,205,10,30);


        // Move Button
        Button Move("",200,0,0,0);
        Move.draw4();
        LCD.WriteAt("MOVE",210,205);
        // Move Decrement Button
        Button MoveD("",200,205,10,30);
        // Move Increment Button
        Button MoveI("",235,205,10,30);
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(moveperturn, 215, 217);
        LCD.SetFontColor(BLACK);

        LCD.SetFontScale(1);
        Button pauseBtn("", 2, 26, 10, 10);
        pauseBtn.draw2();
        float x, y;
        waitForTouch(x, y);
        if (pauseBtn.isPressed(x, y)) {
        if (!pauseMenu()) {  
        // pauseMenu returns false to return to main menu
            mainMenu();
            return;
            }
        // pauseMenu returns true to resume gameplay
            }
        // power logic
        if (PowLD.isPressed(x, y)) powerValue -= 5;
        else if (PowSD.isPressed(x, y)) powerValue -= 1;
        else if (PowSI.isPressed(x, y)) powerValue += 1;
        else if (PowLI.isPressed(x, y)) powerValue += 5;
        if (powerValue < 0) powerValue = 0;
        if (powerValue > 100) powerValue = 100;

        // angle logic
        if (AngLD.isPressed(x, y)) angleValue -= 5;
        else if (AngSD.isPressed(x, y)) angleValue -= 1;
        else if (AngSI.isPressed(x, y)) angleValue += 1;
        else if (AngLI.isPressed(x, y)) angleValue += 5;

        if (angleValue < 0) angleValue = 0;
        if (angleValue > 90) angleValue = 90;

        if (moveperturn > 0){
            if (MoveD.isPressed(x, y)){
                moveperturn -=1;
                tank1X -= 5;  
            }
            if (MoveI.isPressed(x, y)) {
                tank1X += 5;
                moveperturn -=1;
            }
            // keep tank within screen bounds
            if (tank1X < 0) tank1X = 0;
            if (tank1X > 320 - tankWidth) tank1X = 320 - tankWidth;

        }
        if (fireBtn.isPressed(x, y)) moveperturn =3;
        continue;

        LCD.DrawHorizontalLine(210,0,320);
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

// background image
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
    else (true);
}

// reset volleys
// Initializes a variable to track the volley number and sets it to 5
void resetVolleys(int &volleyCount) {
    volleyCount=5;
}

// renders the arc of a projectile given the power and angle of a shot
void renderProjectileArc(float x0, float y0, float power, float angleDeg) {
    float angleRad=angleDeg*(pi/180.0);
    float vx = power * cos(angleRad);
    float vy = power * sin(angleRad);
    for (float t = 0; t < 7.0; t += 0.05)
    {
        float x = x0 + vx * t;
        float y = y0 - (vy * t - 0.5 * g * t * t); // minus because LCD y increases downward
        if (x < 0 || x > 320 || y < 0 || y > 240) // Stop if arc goes offscreen
            break;
        LCD.DrawPixel(x, y);
    }
}

bool wasHitOnSelf(float projX, float projY, float tankX, float tankY)
{
    const int TANK_width=12;
    const int TANK_height=8;
    bool withinX = (projX >= tankX) && (projX <= tankX + TANK_width);
    bool withinY = (projY >= tankY) && (projY <= tankY + TANK_height);
    return (withinX && withinY);
}

// execution block
int main() {
    LCD.Clear();
    mainMenu();  // start program
    return 0;
}