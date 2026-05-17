#include "mbed.h"
#include "Hardware.h"
#include "Game.h"

int main() {
    i2c.frequency(400000);

    sound.start();

    Game game(HUD, gamelcd, buttons, sound);

    game.begin();
    game.showMenu();
    game.reset();

    while (true) {
        game.update();
    }
}