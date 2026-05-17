#pragma once

#include "mbed.h"
#include "tuneable.h"

class Buttons {
private:
    struct ButtonState {
        bool lastRawPressed;
        bool stablePressed;
        int lastChangeTime;
    };

    DigitalIn upButton;
    DigitalIn downButton;
    DigitalIn shootButton;
    DigitalIn abilityButton;
    DigitalIn pauseButton;

    ButtonState upState;
    ButtonState downState;
    ButtonState shootState;
    ButtonState abilityState;
    ButtonState pauseState;

    bool isPressed(DigitalIn& button) {
        return button.read() == 0;
    }

    bool pressedOnce(DigitalIn& button, ButtonState& state, int now) {
        bool rawPressed = isPressed(button);

        if (rawPressed != state.lastRawPressed) {
            state.lastRawPressed = rawPressed;
            state.lastChangeTime = now;
        }

        if (now - state.lastChangeTime < Tuneable::BUTTON_DEBOUNCE_DELAY) {
            return false;
        }

        if (rawPressed != state.stablePressed) {
            state.stablePressed = rawPressed;

            if (state.stablePressed) {
                return true;
            }
        }

        return false;
    }

    void resetState(DigitalIn& button, ButtonState& state, int now) {
        bool current = isPressed(button);

        state.lastRawPressed = current;
        state.stablePressed = current;
        state.lastChangeTime = now;
    }

public:
    Buttons(PinName upPin,
            PinName downPin,
            PinName shootPin,
            PinName abilityPin,
            PinName pausePin)
        : upButton(upPin, PullUp),
          downButton(downPin, PullUp),
          shootButton(shootPin, PullUp),
          abilityButton(abilityPin, PullUp),
          pauseButton(pausePin, PullUp) {
        upState = {false, false, 0};
        downState = {false, false, 0};
        shootState = {false, false, 0};
        abilityState = {false, false, 0};
        pauseState = {false, false, 0};
    }

    bool upPressedOnce(int now) {
        return pressedOnce(upButton, upState, now);
    }

    bool downPressedOnce(int now) {
        return pressedOnce(downButton, downState, now);
    }

    bool shootPressedOnce(int now) {
        return pressedOnce(shootButton, shootState, now);
    }

    bool abilityPressedOnce(int now) {
        return pressedOnce(abilityButton, abilityState, now);
    }

    bool pausePressedOnce(int now) {
        return pressedOnce(pauseButton, pauseState, now);
    }

    bool anyPressed() {
        return isPressed(upButton) ||
               isPressed(downButton) ||
               isPressed(shootButton) ||
               isPressed(abilityButton) ||
               isPressed(pauseButton);
    }

    void reset(int now) {
        resetState(upButton, upState, now);
        resetState(downButton, downState, now);
        resetState(shootButton, shootState, now);
        resetState(abilityButton, abilityState, now);
        resetState(pauseButton, pauseState, now);
    }

    void waitForPressAndRelease() {
        while (!anyPressed()) {
            thread_sleep_for(5);
        }

        while (anyPressed()) {
            thread_sleep_for(5);
        }
    }
};
