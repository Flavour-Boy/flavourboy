#include "mbed.h"
#include "TextLCD_CC.h"

TextLCD lcd(D1, D2, D3, D4, D5, D6, TextLCD::LCD16x2);
//           RS  E   D4  D5  D6  D7

DigitalIn Lbtn(D8, PullUp);
DigitalIn Rbtn(D9, PullUp);

Timer shotTimer;
Timer ammoTimer;

constexpr auto AMMO_REGEN_DELAY = 1500ms;   // 1 second per ammo

// ---------------- CUSTOM CHARACTERS ----------------

char shipChar[] = {0x07, 0x0C, 0x1C, 0x1F, 0x1F, 0x1C, 0x0C, 0x07};
int ship_n = 0;

char heartChar[] = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
int heart_n = 1;

char astroidChar[] = {0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00};
int astroid_n = 2;

char borderChar[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
int border_n = 3;

char ammoChar[] = {0x00, 0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x0E, 0x0E};
int ammo_n = 4;

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
    int ammo = 4;

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


    void useAmmo() {
        if (ammo > 0) {
            ammo--;
            displayAmmo();
        }
    }
    void displayAmmo() {
    for (int i = 0; i < 4; i++) {
        lcd.locate(12 + i, 1);
        if (i < ammo) {
            lcd.putc(ammo_n);
        } else {
            lcd.putc(' ');
        }
    }
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

        //display ammo
        displayAmmo();


    }

    //moves shot until end of play area
    void updateShot(Entity& shot) {
        if (!shot.active) return;

        shot.clearEnt();
        shot.x++;

        if (shot.x >= 10) {
            shot.active = false;
            return;
        }

        shot.draw();
    }

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
const int MAX_SHOTS = 4;
Entity shots[MAX_SHOTS] = {
    Entity(0, 0, '-'),
    Entity(0, 0, '-'),
    Entity(0, 0, '-'),
    Entity(0, 0, '-')
};
//U for top spawning astroid, D for bottom spawning astroid
Entity Uastroid(10,0,astroid_n);
Entity Dastroid(10,1,astroid_n);

// ---------------- FUNCTIONS ----------------

void fireShot() {
    if (game.ammo <= 0) return;

    for (int i = 0; i < MAX_SHOTS; i++) {
        if (!shots[i].active) {
            game.useAmmo();
            shots[i].x = ship.x + 1;
            shots[i].y = ship.y;
            shots[i].active = true;
            shots[i].draw();
            break;  // only fire one shot per press
        }
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
    lcd.writeCustomCharacter(ammoChar, ammo_n+1);

    lcd.cls();

    for (int i = 0; i < MAX_SHOTS; i++) {
    shots[i].active = false;
}
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
    }
}

void updateGame() {
    if (shotTimer.elapsed_time() >= 150ms) {
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (shots[i].active) {
                game.updateShot(shots[i]);
            }
        }
    shotTimer.reset();

    // timed ammo regen
    if (game.ammo < 4 && ammoTimer.elapsed_time() >= AMMO_REGEN_DELAY) {
        game.ammo++;
        game.displayAmmo();
        ammoTimer.reset();
    }
}

    // TODO:
    // - move asteroids and handle astroid collisions 
    // - create a ammo regen timed function 
    // - generate questions/answers + add correct answer logic 
}



// ---------------- MAIN ----------------

int main() {
    menu();
    initialise_game();

    shotTimer.start();
    ammoTimer.start();

    while (true) {
        handleInput();
        updateGame();
        thread_sleep_for(10);
    }
}
