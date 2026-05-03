#pragma once

#include "mbed.h"
#include "events/EventQueue.h"

class Speaker {
private:
    PwmOut speaker;

    Thread soundThread;
    EventQueue soundQueue;

    bool started;

private:
    void menuMoveBlocking() {
        speaker.period(1.0f / 650.0f);
        speaker.write(0.18f);
        thread_sleep_for(35);

        speaker.period(1.0f / 850.0f);
        speaker.write(0.16f);
        thread_sleep_for(35);

        speaker.write(0.0f);
    }

    void menuSelectBlocking() {
        speaker.period(1.0f / 900.0f);
        speaker.write(0.24f);
        thread_sleep_for(60);

        speaker.period(1.0f / 1250.0f);
        speaker.write(0.24f);
        thread_sleep_for(70);

        speaker.period(1.0f / 1600.0f);
        speaker.write(0.18f);
        thread_sleep_for(80);

        speaker.write(0.0f);
    }
    void laserBlocking() {
        for (float freq = 2500.0f; freq > 200.0f; freq -= 20.0f) {
            speaker.period(1.0f / freq);

            float volume = freq / 2500.0f;
            speaker.write(0.5f * volume);

            thread_sleep_for(2);
        }

        speaker.write(0.0f);
    }

    void collectBlocking() {
        speaker.period(1.0f / 900.0f);
        speaker.write(0.35f);
        thread_sleep_for(55);

        speaker.period(1.0f / 1300.0f);
        speaker.write(0.32f);
        thread_sleep_for(55);

        speaker.period(1.0f / 1700.0f);
        speaker.write(0.28f);
        thread_sleep_for(70);

        speaker.write(0.0f);
    }

    void hitBlocking() {
        for (float freq = 1800.0f; freq > 500.0f; freq -= 80.0f) {
            speaker.period(1.0f / freq);
            speaker.write(0.45f);
            thread_sleep_for(2);
        }

        for (float freq = 500.0f; freq > 90.0f; freq -= 25.0f) {
            speaker.period(1.0f / freq);

            float volume = freq / 500.0f;
            speaker.write(0.35f * volume);

            thread_sleep_for(6);
        }

        speaker.period(1.0f / 80.0f);
        speaker.write(0.25f);
        thread_sleep_for(40);

        speaker.write(0.0f);
    }

    void damageBlocking() {
        for (float freq = 1400.0f; freq > 600.0f; freq -= 60.0f) {
            speaker.period(1.0f / freq);
            speaker.write(0.5f);
            thread_sleep_for(2);
        }

        for (int i = 0; i < 25; i++) {
            speaker.period(1.0f / 180.0f);

            if (i % 2) {
                speaker.write(0.35f);
            } else {
                speaker.write(0.0f);
            }

            thread_sleep_for(6);
        }

        speaker.write(0.0f);
    }

    void themeTuneBlocking() {
        struct Note {
            float freq;
            int durationMs;
        };

        const float REST = 0.0f;

        const int T = 105;
        const int S = 150;
        const int E = 230;
        const int Q = 420;

        const float C4 = 261.63f;
        const float D4 = 293.66f;
        const float E4 = 329.63f;
        const float F4 = 349.23f;
        const float G4 = 392.00f;
        const float A4 = 440.00f;
        const float B4 = 493.88f;

        const float C5 = 523.25f;
        const float D5 = 587.33f;
        const float E5 = 659.25f;
        const float F5 = 698.46f;
        const float G5 = 783.99f;
        const float A5 = 880.00f;
        const float B5 = 987.77f;
        const float C6 = 1046.50f;

        tinyLaser();

        const Note tune[] = {
            {C5, S}, {E5, S}, {G5, S}, {C6, E},
            {G5, T}, {C6, T}, {G5, T}, {E5, E},

            {A4, S}, {C5, S}, {E5, S}, {A5, E},
            {E5, T}, {A5, T}, {E5, T}, {C5, E},

            {F4, S}, {A4, S}, {C5, S}, {F5, E},
            {C5, T}, {F5, T}, {C5, T}, {A4, E},

            {G4, S}, {B4, S}, {D5, S}, {G5, E},

            {REST, T},

            {E5, S}, {G5, S}, {C6, Q}
        };

        const int noteCount = sizeof(tune) / sizeof(tune[0]);

        for (int i = 0; i < noteCount; i++) {
            playNote(tune[i].freq, tune[i].durationMs, 0.24f);
        }

        speaker.write(0.0f);
    }

    void gameOverTuneBlocking() {
        struct Note {
            float freq;
            int durationMs;
        };

        const float REST = 0.0f;

        const float C4 = 261.63f;
        const float D4 = 293.66f;
        const float E4 = 329.63f;
        const float G4 = 392.00f;

        const float C5 = 523.25f;
        const float D5 = 587.33f;
        const float E5 = 659.25f;
        const float G5 = 783.99f;

        const Note tune[] = {
            {G5, 120},
            {E5, 120},
            {D5, 120},
            {C5, 220},

            {REST, 80},

            {G4, 140},
            {E4, 140},
            {D4, 140},
            {C4, 420}
        };

        const int noteCount = sizeof(tune) / sizeof(tune[0]);

        for (int i = 0; i < noteCount; i++) {
            playNote(tune[i].freq, tune[i].durationMs, 0.30f);
        }

        for (float freq = 260.0f; freq > 60.0f; freq -= 8.0f) {
            speaker.period(1.0f / freq);

            float volume = freq / 260.0f;
            speaker.write(0.25f * volume);

            thread_sleep_for(8);
        }

        speaker.write(0.0f);
    }

    void playNote(float freq, int durationMs, float volume = 0.28f) {
        if (freq <= 0.0f) {
            speaker.write(0.0f);
            thread_sleep_for(durationMs);
            return;
        }

        speaker.period(1.0f / freq);

        int gapMs = 18;
        int playMs = durationMs - gapMs;

        if (playMs < 10) {
            playMs = durationMs;
            gapMs = 0;
        }

        speaker.write(volume * 0.45f);
        thread_sleep_for(5);

        speaker.write(volume);
        thread_sleep_for(playMs - 5);

        speaker.write(0.0f);
        thread_sleep_for(gapMs);
    }

    void tinyLaser() {
        for (float freq = 520.0f; freq < 1200.0f; freq += 45.0f) {
            speaker.period(1.0f / freq);
            speaker.write(0.16f);
            thread_sleep_for(4);
        }

        speaker.write(0.0f);
        thread_sleep_for(40);
    }

public:
    Speaker(PinName pin)
        : speaker(pin),
          soundThread(osPriorityLow, 4096),
          soundQueue(4096),
          started(false) {}

    void start() {
        if (started) {
            return;
        }

        started = true;
        soundThread.start(callback(&soundQueue, &EventQueue::dispatch_forever));
    }

    void laser() {
        soundQueue.call(callback(this, &Speaker::laserBlocking));
    }

    void collect() {
        soundQueue.call(callback(this, &Speaker::collectBlocking));
    }

    void hit() {
        soundQueue.call(callback(this, &Speaker::hitBlocking));
    }

    void damage() {
        soundQueue.call(callback(this, &Speaker::damageBlocking));
    }

    void themeTune() {
        soundQueue.call(callback(this, &Speaker::themeTuneBlocking));
    }

    void gameOverTune() {
        soundQueue.call(callback(this, &Speaker::gameOverTuneBlocking));
    }

    void menuMove() {
        soundQueue.call(callback(this, &Speaker::menuMoveBlocking));
    }

    void menuSelect() {
        soundQueue.call(callback(this, &Speaker::menuSelectBlocking));
    }
};
