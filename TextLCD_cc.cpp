#include "TextLCD_CC.h"
#include "mbed.h"

//////////////////////////////////////////////////////////////////
// PARENT LCD CLASS
//////////////////////////////////////////////////////////////////

TextLCD_Base::TextLCD_Base(LCDType type)
    : _type(type),
      _column(0),
      _row(0) {
}

void TextLCD_Base::cls() {
    writeCommand(0x01);
    thread_sleep_for(2);
    locate(0, 0);
}

void TextLCD_Base::locate(int column, int row) {
    if (column < 0) {
        column = 0;
    }

    if (row < 0) {
        row = 0;
    }

    if (column >= columns()) {
        column = columns() - 1;
    }

    if (row >= rows()) {
        row = rows() - 1;
    }

    _column = column;
    _row = row;

    writeCommand(0x80 | address(column, row));
}

int TextLCD_Base::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row++;

        if (_row >= rows()) {
            _row = 0;
        }

        locate(_column, _row);
    } else {
        writeData(value);

        _column++;

        if (_column >= columns()) {
            _column = 0;
            _row++;

            if (_row >= rows()) {
                _row = 0;
            }

            locate(_column, _row);
        }
    }

    return value;
}

int TextLCD_Base::_getc() {
    return -1;
}

void TextLCD_Base::character(int column, int row, int c) {
    locate(column, row);
    writeData(c);
}

int TextLCD_Base::address(int column, int row) {
    switch (_type) {
        case LCD20x4: {
            int offsets[] = {0x00, 0x40, 0x14, 0x54};
            return offsets[row] + column;
        }

        case LCD16x2B:
            return (row * 40) + column;

        default:
            return (row * 0x40) + column;
    }
}

int TextLCD_Base::columns() {
    return (_type == LCD20x4 || _type == LCD20x2) ? 20 : 16;
}

int TextLCD_Base::rows() {
    return (_type == LCD20x4) ? 4 : 2;
}

void TextLCD_Base::writeCustomCharacter(char custom[8], int index) {
    // Your old convention:
    // index 1-8 maps to LCD CGRAM slots 0-7.
    // index 0 also maps to slot 0.
    index = index < 1 ? 0 : (index > 8 ? 7 : index - 1);

    // Set CGRAM address.
    // Each custom character takes 8 bytes.
    writeCommand(0x40 | (index << 3));

    for (int i = 0; i < 8; i++) {
        writeData(custom[i]);
    }

    // Return to normal display memory.
    locate(_column, _row);
}

void TextLCD_Base::putCustomCharacter(int index) {
    // Same index convention as writeCustomCharacter().
    index = index < 1 ? 0 : (index > 8 ? 7 : index - 1);

    writeData(index);

    _column++;

    if (_column >= columns()) {
        _column = 0;
        _row++;

        if (_row >= rows()) {
            _row = 0;
        }

        locate(_column, _row);
    }
}

//////////////////////////////////////////////////////////////////
// PARALLEL LCD CHILD CLASS
//////////////////////////////////////////////////////////////////

TextLCD::TextLCD(PinName rs,
                 PinName e,
                 PinName d4,
                 PinName d5,
                 PinName d6,
                 PinName d7,
                 LCDType type)
    : TextLCD_Base(type),
      _rs(rs),
      _e(e),
      _d(d4, d5, d6, d7) {

    _e  = 1;
    _rs = 0;

    thread_sleep_for(15);

    for (int i = 0; i < 3; i++) {
        writeByte(0x3);
        thread_sleep_for(2);
    }

    writeByte(0x2);
    thread_sleep_for(1);

    writeCommand(0x28);   // 4-bit mode, 2 lines, 5x8 font
    writeCommand(0x0C);   // display on, cursor off
    writeCommand(0x06);   // entry mode
    cls();
}

void TextLCD::writeByte(int value) {
    _d = value >> 4;
    thread_sleep_for(1);
    _e = 0;
    thread_sleep_for(1);
    _e = 1;

    _d = value;
    thread_sleep_for(1);
    _e = 0;
    thread_sleep_for(1);
    _e = 1;
}

void TextLCD::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
}

void TextLCD::writeData(int data) {
    _rs = 1;
    writeByte(data);
}

//////////////////////////////////////////////////////////////////
// I2C LCD CHILD CLASS
//////////////////////////////////////////////////////////////////

#define PIN_RS 0x01
#define PIN_EN 0x04
#define PIN_BL 0x08

TextLCD_I2C::TextLCD_I2C(I2C *i2c, int address, LCDType type)
    : TextLCD_Base(type),
      _i2c(i2c),
      _address(address),
      _backlight(PIN_BL) {

    thread_sleep_for(50);

    expanderWrite(0x00);
    thread_sleep_for(100);

    // LCD init sequence for 4-bit mode over I2C backpack
    write4bits(0x30);
    thread_sleep_for(5);

    write4bits(0x30);
    thread_sleep_for(5);

    write4bits(0x30);
    thread_sleep_for(1);

    write4bits(0x20);

    writeCommand(0x28);   // 4-bit mode, 2 lines, 5x8 font
    writeCommand(0x0C);   // display on, cursor off
    writeCommand(0x06);   // entry mode
    cls();
}

void TextLCD_I2C::writeCommand(int command) {
    write4bits(command & 0xF0);
    write4bits((command << 4) & 0xF0);
}

void TextLCD_I2C::writeData(int data) {
    write4bits((data & 0xF0) | PIN_RS);
    write4bits(((data << 4) & 0xF0) | PIN_RS);
}

void TextLCD_I2C::write4bits(int value) {
    expanderWrite(value);
    pulseEnable(value);
}

void TextLCD_I2C::pulseEnable(int data) {
    expanderWrite(data | PIN_EN);
    wait_us(1);

    expanderWrite(data & ~PIN_EN);
    wait_us(50);
}

void TextLCD_I2C::expanderWrite(int data) {
    char d = data | _backlight;
    _i2c->write(_address, &d, 1);
}