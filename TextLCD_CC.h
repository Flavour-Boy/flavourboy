#ifndef MBED_TEXTLCD_CC_H
#define MBED_TEXTLCD_CC_H

#include "mbed.h"

//////////////////////////////////////////////////////////////////
// PARENT LCD CLASS
//////////////////////////////////////////////////////////////////

class TextLCD_Base : public Stream {
public:
    enum LCDType {
        LCD16x2,
        LCD16x2B,
        LCD20x2,
        LCD20x4
    };

    TextLCD_Base(LCDType type = LCD16x2);

    void locate(int column, int row);
    void writeCustomCharacter(char custom[8], int index);
    void putCustomCharacter(int index);
    void cls();

    int rows();
    int columns();

protected:
    virtual int _putc(int value);
    virtual int _getc();

    int address(int column, int row);
    void character(int column, int row, int c);

    virtual void writeCommand(int command) = 0;
    virtual void writeData(int data) = 0;

    LCDType _type;

    int _column;
    int _row;
};

//////////////////////////////////////////////////////////////////
// PARALLEL LCD CHILD CLASS
//////////////////////////////////////////////////////////////////

class TextLCD : public TextLCD_Base {
public:
    TextLCD(PinName rs,
            PinName e,
            PinName d4,
            PinName d5,
            PinName d6,
            PinName d7,
            LCDType type = LCD16x2);

protected:
    void writeByte(int value);
    virtual void writeCommand(int command);
    virtual void writeData(int data);

    DigitalOut _rs;
    DigitalOut _e;
    BusOut _d;
};

//////////////////////////////////////////////////////////////////
// I2C LCD CHILD CLASS — PCF8574 BACKPACK SUPPORT
//////////////////////////////////////////////////////////////////

class TextLCD_I2C : public TextLCD_Base {
public:
    TextLCD_I2C(I2C *i2c, int address, LCDType type = LCD16x2);

protected:
    virtual void writeCommand(int command);
    virtual void writeData(int data);

    void write4bits(int value);
    void expanderWrite(int data);
    void pulseEnable(int data);

    I2C *_i2c;
    int _address;
    int _backlight;
};

#endif
