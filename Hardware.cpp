#include "Hardware.h"

TextLCD HUD(D1, D2, D4, D5, D6, D7, TextLCD::LCD16x2);

I2C i2c(D14, D15);
TextLCD_I2C gamelcd(&i2c, 0x4E, TextLCD_I2C::LCD20x4);

Buttons buttons(D8, D9, D10, D11, D12);

Speaker sound(D3);