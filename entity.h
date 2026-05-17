#pragma once

#include "mbed.h"
#include "tuneable.h"
// ================================================================
// ENTITY TYPES
// ================================================================

namespace EntityType {
    const int METEOR = 0;
    const int WRONG_PICKUP = 1;
    const int CORRECT_PICKUP = 2;
    const int SHOT = 3;
}
class Entity {
public:
    int x;
    int y;
    char symbol;
    bool active;

    int type;
    int data;

    Entity() {
        x = -1;
        y = -1;
        symbol = ' ';
        active = false;

        type = EntityType::METEOR;
        data = -1;
    }

    void spawn(int startX, int startY, char newSymbol, int newType, int newData = -1) {
        x = startX;
        y = startY;
        symbol = newSymbol;
        active = true;

        type = newType;
        data = newData;
    }

    void remove() {
        x = -1;
        y = -1;
        active = false;

        type = EntityType::METEOR;
        data = -1;
    }

    void moveLeft() {
        x--;
    }

    void moveRight() {
        x++;
    }

    bool offLeft() {
        return x < 0;
    }

    bool offRight() {
        return x >= Tuneable::GAME_COLS;
    }
};

