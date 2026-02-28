#include "mbed.h"
#include "TextLCD_CC.h"

TextLCD lcd(D1, D2, D3, D4, D5, D6, TextLCD::LCD16x2);
//           RS  E   D4  D5  D6  D7

DigitalIn Lbtn(D8, PullUp);
DigitalIn Rbtn(D9, PullUp);

Timer shotTimer;

// ---------------- CUSTOM CHARACTERS ----------------

char shipChar[] = {0x07, 0x0C, 0x1C, 0x1F, 0x1F, 0x1C, 0x0C, 0x07};
int ship_n = 0;

char heartChar[] = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
int heart_n = 1;

char astroidChar[] = {0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00};
int astroid_n = 2;

char borderChar[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
int border_n = 3;

// ---------------- CLASSES ----------------
class Entity {
public:
    int x;
    int y;
    char symbol;
    bool active;

    Entity(int startX, int startY, char startSymbol) {
        x = startX;
        y = startY;
        symbol = startSymbol;
        active = true;
    }

    void moveTo(int newX, int newY) {
        // erase old position
        lcd.locate(x, y);
        lcd.putc(' ');

        // update stored position
        x = newX;
        y = newY;

        // draw at new position
        lcd.locate(x, y);
        lcd.putc(symbol);
    }

    void draw() {
        if (active) {
            lcd.locate(x, y);
            lcd.putc(symbol);
        }
    }

    //moves ship row
    void toggleRow() {
        moveTo(x, 1 - y);
    }

    //removes entity for when astroid or number is shot
    void clearEnt(){
        lcd.locate(x,y);
        lcd.printf(" ");

    }

};

class Game {
public:
    int lives = 3;
    int score = 0;
    int answer = 0;

    void HealthLoss() {
        lives--;
        if (lives < 0) {
            lives = 0;
        }
    }

    void AddScore() {
        score++;
    }

    void displayHealth() {
        lcd.locate(0, 0);
        lcd.putc(heart_n);          // custom heart character
        lcd.locate(1, 0);
        lcd.printf("%d ", lives);   // extra space clears old digits
    }

    void displayScore() {
        lcd.locate(0, 1);
        lcd.printf("S%d ", score);  
    }

    void drawHUD(){
        //display score
        lcd.locate(0, 1);
        lcd.printf("S%d ", score);  

        //display health
        lcd.locate(0, 0);
        lcd.putc(heart_n);          // custom heart character
        lcd.locate(1, 0);
        lcd.printf("%d ", lives);   

        //display borders
        lcd.locate(11,0); lcd.putc(border_n);
        lcd.locate(11,1); lcd.putc(border_n);
        lcd.locate(2,0); lcd.putc(border_n);;
        lcd.locate(2,1); lcd.putc(border_n);


    }

    //moves shot until end of play area
    void updateShot(Entity& shot){
        if (!shot.active) return;

        shot.clearEnt();
        shot.x++;

        if (shot.x >= 10) {
            shot.active = false;
            return;
        }

        shot.draw();
    };

    //takes two entities and returns if they have collided 
    bool checkCollision(const Entity& a, const Entity& b) {
        return a.active && b.active && a.x == b.x && a.y == b.y;
    }

    void generate_question(){

    };
};



// ---------------- GLOBAL GAME OBJECTS ----------------

Game game;

Entity ship(3, 0, ship_n);
Entity shot(0, 0, '-');

//U for top spawning astroid, D for bottom spawning astroid
Entity Uastroid(10,0,astroid_n);
Entity Dastroid(10,1,astroid_n);

// ---------------- FUNCTIONS ----------------

void fireShot() {
    if (!shot.active) {
        shot.x = ship.x + 1;
        shot.y = ship.y;
        shot.active = true;
        shot.draw();
    }
}

void WaitForPress() {
    while (Lbtn.read() != 0 && Rbtn.read() != 0) {
        ThisThread::sleep_for(5ms);
    }
}

void WaitForRelease() {
    while (Lbtn.read() == 0 || Rbtn.read() == 0) {
        ThisThread::sleep_for(5ms);
    }
}

void menu() {
    lcd.cls();
    lcd.locate(2, 0);
    lcd.printf("FLAVOUR BOYS");
    lcd.locate(1, 1);
    lcd.printf("PRESS TO START");
    WaitForPress();
    WaitForRelease();

    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("L=Shoot R=Move");
    lcd.locate(1, 1);
    lcd.printf("PRESS TO START");
    WaitForPress();
    WaitForRelease();
}

void initialise_game() {
    //loading custom characters onto LCD memory
    lcd.writeCustomCharacter(shipChar, ship_n + 1);
    lcd.writeCustomCharacter(heartChar, heart_n + 1);
    lcd.writeCustomCharacter(astroidChar, astroid_n + 1);
    lcd.writeCustomCharacter(borderChar, border_n + 1);

    lcd.cls();
    shot.active = false;
    ship.draw();
    //draws borders, score, health
    game.drawHUD();


}

void handleInput() {
    // Right button = move ship up/down
    if (Rbtn.read() == 0) {
        ship.toggleRow();
        WaitForRelease();
    }

    // Left button = shoot hook for later
    if (Lbtn.read() == 0) {
        fireShot();
        WaitForRelease();
        // TODO: add shot logic here later
 

        WaitForRelease();
    }
}

void updateGame() {
    if (shot.active && shotTimer.elapsed_time() >= 150ms) {
        game.updateShot(shot);
        shotTimer.reset();}
        
    // TODO:
    // - move asteroids
    // - generate questions/answers
}



// ---------------- MAIN ----------------

int main() {
    menu();
    initialise_game();
    shotTimer.start();

    while (true) {
        handleInput();
        updateGame();

        thread_sleep_for(10);
    }
}
