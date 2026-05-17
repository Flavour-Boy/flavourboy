#ifndef HARDWARE_H
#define HARDWARE_H

#include "mbed.h"
#include "TextLCD_CC.h"
#include "SoundEffects.h"
#include "buttons.h"

extern TextLCD HUD;
extern I2C i2c;
extern TextLCD_I2C gamelcd;
extern Buttons buttons;
extern Speaker sound;

#endif