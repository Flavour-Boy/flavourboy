#include "mbed.h"
#include "TextLCD_CC.h"
#include "SoundEffects.h"
#include "WordBank.h"


// ================================================================
// TUNEABLE SETTINGS
// All timings are in milliseconds.
// ================================================================

namespace Tuneable {
    constexpr int GAME_COLS = 20;
    constexpr int GAME_ROWS = 4;

    constexpr int SHIP_X = 1;

    constexpr int MAX_SHOTS = 5;
    constexpr int MAX_METEORS = 5;
    constexpr int MAX_PICKUPS = 5;
    constexpr int MAX_AMMO = 3;

    constexpr int START_LIVES = 3;

    constexpr int FRAME_DELAY = 33;
    constexpr int SHOT_STEP_DELAY = 45;
    constexpr int AMMO_REGEN_DELAY = 650;
    constexpr int SHOOT_COOLDOWN = 120;
    constexpr int BUTTON_DEBOUNCE_DELAY = 35;
    constexpr int ABILITY_COOLDOWN = 25000;

    constexpr int WORD_SHOW_TIME = 3000;

    // Easy difficulty
    constexpr int EASY_METEOR_STEP_DELAY = 430;
    constexpr int EASY_METEOR_SPAWN_DELAY = 1600;
    constexpr int EASY_PICKUP_STEP_DELAY = 380;
    constexpr int EASY_PICKUP_SPAWN_DELAY = 1200;

    // Normal difficulty
    constexpr int NORMAL_METEOR_STEP_DELAY = 300;
    constexpr int NORMAL_METEOR_SPAWN_DELAY = 1000;
    constexpr int NORMAL_PICKUP_STEP_DELAY = 320;
    constexpr int NORMAL_PICKUP_SPAWN_DELAY = 1000;

    // Hard difficulty
    constexpr int HARD_METEOR_STEP_DELAY = 190;
    constexpr int HARD_METEOR_SPAWN_DELAY = 650;
    constexpr int HARD_PICKUP_STEP_DELAY = 240;
    constexpr int HARD_PICKUP_SPAWN_DELAY = 800;
}

// ================================================================
// LCD CUSTOM CHARACTER SLOTS
// ================================================================

namespace Symbol {
    const int SHIP = 0;
    const int HEART = 1;
    const int METEOR = 2;
    const int BORDER = 3;
    const int AMMO = 4;
}

// ================================================================
// ENTITY TYPES
// ================================================================

namespace EntityType {
    const int METEOR = 0;
    const int WRONG_PICKUP = 1;
    const int CORRECT_PICKUP = 2;
    const int SHOT = 3;
}

// ================================================================
// GAME MODES
// ================================================================

namespace GameMode {
    const int WORD = 0;
    const int NUMBER = 1;
}

// ================================================================
// DIFFICULTIES
// ================================================================

namespace Difficulty {
    const int EASY = 0;
    const int NORMAL = 1;
    const int HARD = 2;
}

// ================================================================
// CUSTOM CHARACTERS
// ================================================================

char shipChar[8] = {
    0x07, 0x0C, 0x1C, 0x1F,
    0x1F, 0x1C, 0x0C, 0x07
};

char heartChar[8] = {
    0x00, 0x0A, 0x1F, 0x1F,
    0x0E, 0x04, 0x00, 0x00
};

char meteorChar[8] = {
    0x00, 0x00, 0x04, 0x0E,
    0x1F, 0x0E, 0x04, 0x00
};

char borderChar[8] = {
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04
};

char ammoChar[8] = {
    0x00, 0x04, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0E, 0x0E
};

// ================================================================
// BUTTONS CLASS
// PullUp:
// not pressed = 1
// pressed     = 0
// ================================================================

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

// ================================================================
// ENTITY CLASS
// Used for shots, meteors, letters, and number answers.
// ================================================================

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

// ================================================================
// GAME CLASS
// ================================================================

class Game {
private:
    TextLCD& HUD;
    TextLCD_I2C& gamelcd;

    Buttons& buttons;
    Speaker& sound;

    Timer timer;

    Entity shots[Tuneable::MAX_SHOTS];
    Entity meteors[Tuneable::MAX_METEORS];
    Entity pickups[Tuneable::MAX_PICKUPS];

    int selectedGame;
    int selectedDifficulty;

    int shipY;

    int lives;
    int score;
    int ammo;

    bool gameOver;

    int lastLives;
    int lastScore;
    int lastAmmo;
    int lastAbilityDisplaySeconds;

    char gameBuffer[Tuneable::GAME_ROWS][Tuneable::GAME_COLS];
    char lastGameBuffer[Tuneable::GAME_ROWS][Tuneable::GAME_COLS];

    uint32_t rngState;

    int lastFrameTime;
    int lastShotStepTime;
    int lastMeteorStepTime;
    int lastMeteorSpawnTime;
    int lastPickupStepTime;
    int lastPickupSpawnTime;
    int lastAmmoRegenTime;
    int lastShootTime;
    int lastAbilityUseTime;

    char currentWord[10];
    int currentWordLength;
    int wordProgress;
    int lastWordIndex;

    bool showingWord;
    int wordHideTime;

    char questionText[10];
    char answerChar;
    int answerValue;

    int pickupsSinceChallengeStart;

public:
    Game(TextLCD& hudRef,
         TextLCD_I2C& gameLcdRef,
         Buttons& buttonsRef,
         Speaker& soundRef)
        : HUD(hudRef),
          gamelcd(gameLcdRef),
          buttons(buttonsRef),
          sound(soundRef) {
        selectedGame = GameMode::WORD;
        selectedDifficulty = Difficulty::NORMAL;

        shipY = 1;

        lives = Tuneable::START_LIVES;
        score = 0;
        ammo = Tuneable::MAX_AMMO;

        gameOver = false;

        lastLives = -1;
        lastScore = -1;
        lastAmmo = -1;
        lastAbilityDisplaySeconds = -999;

        rngState = 1234567;

        lastFrameTime = 0;
        lastShotStepTime = 0;
        lastMeteorStepTime = 0;
        lastMeteorSpawnTime = 0;
        lastPickupStepTime = 0;
        lastPickupSpawnTime = 0;
        lastAmmoRegenTime = 0;
        lastShootTime = 0;
        lastAbilityUseTime = -10000000;

        currentWordLength = 0;
        wordProgress = 0;
        lastWordIndex = -1;

        showingWord = false;
        wordHideTime = 0;

        questionText[0] = '\0';
        answerChar = '0';
        answerValue = 0;

        pickupsSinceChallengeStart = 0;
    }

    void begin() {
        timer.start();
        loadCustomCharacters();
    }

    void showMenu() {
        sound.themeTune();

        chooseGameMenu();
        chooseDifficultyMenu();
    }

    void reset() {
        gamelcd.cls();

        resetLastGameBuffer();
        clearGameBuffer();

        forceClearHUD();
        clearAllEntities();

        shipY = 1;

        lives = Tuneable::START_LIVES;
        score = 0;
        ammo = Tuneable::MAX_AMMO;

        gameOver = false;

        int now = timer.read_ms();

        buttons.reset(now);

        lastFrameTime = now;
        lastShotStepTime = now;
        lastMeteorStepTime = now;
        lastMeteorSpawnTime = now;
        lastPickupStepTime = now;
        lastPickupSpawnTime = now;
        lastAmmoRegenTime = now;
        lastShootTime = now;

        lastAbilityUseTime = now - Tuneable::ABILITY_COOLDOWN;
        lastAbilityDisplaySeconds = -999;

        if (selectedGame == GameMode::WORD) {
            startNewWord(now);
        } else {
            startNewQuestion();
        }

        updateHUD();
        updateAbilityHUD(now, true);
        updateChallengeHUD();

        renderGame();
    }

    void update() {
        int now = timer.read_ms();

        if (!gameOver) {
            handleInput(now);
            updateGame(now);

            if (now - lastFrameTime >= Tuneable::FRAME_DELAY) {
                renderGame();
                lastFrameTime = now;
            }
        } else {
            bool goHome = showGameOverScreen();

            if (goHome) {
                showMenu();
            }

            reset();
        }

        thread_sleep_for(2);
    }

private:
    // ============================================================
    // DIFFICULTY TIMINGS AND CHANCES
    // ============================================================

    int meteorStepDelay() {
        if (selectedDifficulty == Difficulty::EASY) {
            return Tuneable::EASY_METEOR_STEP_DELAY;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return Tuneable::HARD_METEOR_STEP_DELAY;
        }

        return Tuneable::NORMAL_METEOR_STEP_DELAY;
    }

    int meteorSpawnDelay() {
        if (selectedDifficulty == Difficulty::EASY) {
            return Tuneable::EASY_METEOR_SPAWN_DELAY;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return Tuneable::HARD_METEOR_SPAWN_DELAY;
        }

        return Tuneable::NORMAL_METEOR_SPAWN_DELAY;
    }

    int pickupStepDelay() {
        if (selectedDifficulty == Difficulty::EASY) {
            return Tuneable::EASY_PICKUP_STEP_DELAY;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return Tuneable::HARD_PICKUP_STEP_DELAY;
        }

        return Tuneable::NORMAL_PICKUP_STEP_DELAY;
    }

    int pickupSpawnDelay() {
        if (selectedDifficulty == Difficulty::EASY) {
            return Tuneable::EASY_PICKUP_SPAWN_DELAY;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return Tuneable::HARD_PICKUP_SPAWN_DELAY;
        }

        return Tuneable::NORMAL_PICKUP_SPAWN_DELAY;
    }

    int forcedWrongPickupCount() {
        if (selectedDifficulty == Difficulty::EASY) {
            return 1;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return 3;
        }

        return 2;
    }

    int wordCorrectChance() {
        if (selectedDifficulty == Difficulty::EASY) {
            return 28;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return 14;
        }

        return 20;
    }

    int numberCorrectChance() {
        if (selectedDifficulty == Difficulty::EASY) {
            return 30;
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return 15;
        }

        return 22;
    }

    const char* difficultyName() {
        if (selectedDifficulty == Difficulty::EASY) {
            return "EASY";
        }

        if (selectedDifficulty == Difficulty::HARD) {
            return "HARD";
        }

        return "NORMAL";
    }

    // ============================================================
    // MENUS
    // ============================================================

    void chooseGameMenu() {
        int option = selectedGame;

        drawGameMenu(option);

        int now = timer.read_ms();
        buttons.reset(now);

        while (true) {
            now = timer.read_ms();

            if (buttons.upPressedOnce(now)) {
                if (option == GameMode::WORD) {
                    option = GameMode::NUMBER;
                } else {
                    option = GameMode::WORD;
                }

                sound.menuMove();
                drawGameMenu(option);
            }

            if (buttons.downPressedOnce(now)) {
                if (option == GameMode::WORD) {
                    option = GameMode::NUMBER;
                } else {
                    option = GameMode::WORD;
                }

                sound.menuMove();
                drawGameMenu(option);
            }

            if (buttons.shootPressedOnce(now)) {
                selectedGame = option;
                rngState += now;

                sound.menuSelect();
                thread_sleep_for(120);

                buttons.waitForPressAndRelease();
                return;
            }

            thread_sleep_for(10);
        }
    }

    void chooseDifficultyMenu() {
        int option = selectedDifficulty;

        drawDifficultyMenu(option);

        int now = timer.read_ms();
        buttons.reset(now);

        while (true) {
            now = timer.read_ms();

            if (buttons.upPressedOnce(now)) {
                option--;

                if (option < Difficulty::EASY) {
                    option = Difficulty::HARD;
                }

                drawDifficultyMenu(option);
                sound.menuMove();
            }

            if (buttons.downPressedOnce(now)) {
                option++;

                if (option > Difficulty::HARD) {
                    option = Difficulty::EASY;
                }

                drawDifficultyMenu(option);
                sound.menuMove();
            }

            if (buttons.shootPressedOnce(now)) {
                selectedDifficulty = option;
                rngState += now;

                sound.menuSelect();

                buttons.waitForPressAndRelease();
                return;
            }

            thread_sleep_for(10);
        }
    }

    void drawGameMenu(int option) {
        HUD.cls();
        gamelcd.cls();

        HUD.locate(2, 0);
        HUD.printf("PICK GAME");

        HUD.locate(1, 1);
        HUD.printf("RED START");

        gamelcd.locate(0, 0);
        gamelcd.printf("GREEN/BLUE SELECT");

        gamelcd.locate(0, 1);

        if (option == GameMode::WORD) {
            gamelcd.printf("> WORD GAME");
        } else {
            gamelcd.printf("  WORD GAME");
        }

        gamelcd.locate(0, 2);

        if (option == GameMode::NUMBER) {
            gamelcd.printf("> NUMBER GAME");
        } else {
            gamelcd.printf("  NUMBER GAME");
        }

        gamelcd.locate(0, 3);
        gamelcd.printf("RED = NEXT");
    }

    void drawDifficultyMenu(int option) {
        HUD.cls();
        gamelcd.cls();

        HUD.locate(1, 0);
        HUD.printf("DIFFICULTY");

        HUD.locate(1, 1);
        HUD.printf("RED START");

        gamelcd.locate(0, 0);
        gamelcd.printf("GREEN/BLUE SELECT");

        gamelcd.locate(0, 1);
        if (option == Difficulty::EASY) {
            gamelcd.printf("> EASY");
        } else {
            gamelcd.printf("  EASY");
        }

        gamelcd.locate(0, 2);
        if (option == Difficulty::NORMAL) {
            gamelcd.printf("> NORMAL");
        } else {
            gamelcd.printf("  NORMAL");
        }

        gamelcd.locate(0, 3);
        if (option == Difficulty::HARD) {
            gamelcd.printf("> HARD");
        } else {
            gamelcd.printf("  HARD");
        }
    }

    // ============================================================
    // SETUP
    // ============================================================

    void loadCustomCharacters() {
        gamelcd.writeCustomCharacter(shipChar, Symbol::SHIP + 1);
        gamelcd.writeCustomCharacter(meteorChar, Symbol::METEOR + 1);

        HUD.writeCustomCharacter(heartChar, Symbol::HEART + 1);
        HUD.writeCustomCharacter(ammoChar, Symbol::AMMO + 1);
        HUD.writeCustomCharacter(borderChar, Symbol::BORDER + 1);
    }

    void clearAllEntities() {
        for (int i = 0; i < Tuneable::MAX_SHOTS; i++) {
            shots[i].remove();
        }

        for (int i = 0; i < Tuneable::MAX_METEORS; i++) {
            meteors[i].remove();
        }

        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            pickups[i].remove();
        }
    }

    void clearPickupsOnly() {
        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            pickups[i].remove();
        }
    }

    // ============================================================
    // RANDOM
    // ============================================================

    int randomNumber(int maxValue) {
        rngState = rngState * 1103515245 + 12345;
        return (rngState >> 16) % maxValue;
    }

    // ============================================================
    // WORD GAME
    // ============================================================

    void startNewWord(int now) {
        int randomWordIndex = randomNumber(WordBank::WORD_COUNT);

        if (WordBank::WORD_COUNT > 1) {
            while (randomWordIndex == lastWordIndex) {
                randomWordIndex = randomNumber(WordBank::WORD_COUNT);
            }
        }

        lastWordIndex = randomWordIndex;

        const char* nextWord = WordBank::WORDS[randomWordIndex];

        for (int i = 0; i < 10; i++) {
            currentWord[i] = '\0';
        }

        currentWordLength = 0;

        for (int i = 0; i < 9; i++) {
            if (nextWord[i] == '\0') {
                break;
            }

            currentWord[i] = nextWord[i];
            currentWordLength++;
        }

        wordProgress = 0;
        showingWord = true;
        wordHideTime = now + Tuneable::WORD_SHOW_TIME;
        pickupsSinceChallengeStart = 0;

        clearPickupsOnly();
        updateChallengeHUD();
    }

    bool wordComplete() {
        return wordProgress >= currentWordLength;
    }

    char neededLetter() {
        if (wordComplete()) {
            return ' ';
        }

        return currentWord[wordProgress];
    }

    char randomWrongLetter() {
        char letter = 'A';

        // Sometimes use a future letter from the same word as a decoy.
        // Example: if the word is SPACE and the player needs S,
        // P, A, C, or E can appear early but count as wrong for now.
        if (currentWordLength > wordProgress + 1 && randomNumber(100) < 45) {
            int futureIndex = wordProgress + 1 + randomNumber(currentWordLength - wordProgress - 1);
            return currentWord[futureIndex];
        }

        for (int tries = 0; tries < 30; tries++) {
            letter = 'A' + randomNumber(26);

            if (letter != neededLetter()) {
                return letter;
            }
        }

        return letter;
    }

    void collectCorrectWordLetter(Entity& pickup, int now) {
        if (pickup.data != wordProgress) {
            pickup.remove();
            damagePlayer();
            return;
        }

        if (pickup.symbol != neededLetter()) {
            pickup.remove();
            damagePlayer();
            return;
        }

        pickup.remove();

        wordProgress++;
        sound.collect();

        clearPickupsOnly();
        updateChallengeHUD();

        if (wordComplete()) {
            score += 50;
            updateHUD();

            startNewWord(now);
        }
    }

    // ============================================================
    // NUMBER GAME
    // Random maths generator.
    // Answer is always one digit because the pickup is one LCD char.
    // ============================================================

    void startNewQuestion() {
        int a = 0;
        int b = 0;
        int answer = 0;
        int questionType = 0;

        if (selectedDifficulty == Difficulty::EASY) {
            questionType = randomNumber(2);
        } else if (selectedDifficulty == Difficulty::NORMAL) {
            questionType = randomNumber(3);
        } else {
            questionType = randomNumber(4);
        }

        if (questionType == 0) {
            do {
                a = randomNumber(10);
                b = randomNumber(10);
                answer = a + b;
            } while (answer > 9);

            snprintf(questionText, sizeof(questionText), "%d+%d=?", a, b);
        } else if (questionType == 1) {
            a = randomNumber(10);
            b = randomNumber(a + 1);
            answer = a - b;

            snprintf(questionText, sizeof(questionText), "%d-%d=?", a, b);
        } else if (questionType == 2) {
            do {
                if (selectedDifficulty == Difficulty::NORMAL) {
                    a = 1 + randomNumber(5);
                    b = 1 + randomNumber(5);
                } else {
                    a = 1 + randomNumber(9);
                    b = 1 + randomNumber(9);
                }

                answer = a * b;
            } while (answer > 9);

            snprintf(questionText, sizeof(questionText), "%dx%d=?", a, b);
        } else {
            do {
                answer = 1 + randomNumber(9);
                b = 1 + randomNumber(9);
                a = answer * b;
            } while (a > 9);

            snprintf(questionText, sizeof(questionText), "%d/%d=?", a, b);
        }

        answerValue = answer;
        answerChar = '0' + answerValue;

        pickupsSinceChallengeStart = 0;

        clearPickupsOnly();
        updateChallengeHUD();
    }

    char randomWrongDigit() {
        int value = answerValue;

        // Most wrong answers are near the real answer.
        // Example: if answer is 6, decoys like 4, 5, 7, 8 are likely.
        for (int tries = 0; tries < 30; tries++) {
            if (randomNumber(100) < 65) {
                int offset = 1 + randomNumber(3);

                if (randomNumber(2) == 0) {
                    offset = -offset;
                }

                value = answerValue + offset;
            } else {
                value = randomNumber(10);
            }

            if (value >= 0 && value <= 9 && value != answerValue) {
                return '0' + value;
            }
        }

        return '0' + ((answerValue + 1) % 10);
    }

    void collectCorrectNumberAnswer(Entity& pickup) {
        pickup.remove();

        score += 50;
        updateHUD();

        sound.collect();

        startNewQuestion();
    }

    // ============================================================
    // HUD CHALLENGE DISPLAY
    // ============================================================

    void printHudMiddle(int row, const char* text) {
        HUD.locate(3, row);

        bool finishedText = false;

        for (int i = 0; i < 9; i++) {
            if (!finishedText && text[i] != '\0') {
                HUD.putc(text[i]);
            } else {
                finishedText = true;
                HUD.putc(' ');
            }
        }
    }

    void updateChallengeHUD() {
        char display[10];

        for (int i = 0; i < 9; i++) {
            display[i] = ' ';
        }

        display[9] = '\0';

        if (selectedGame == GameMode::WORD) {
            for (int i = 0; i < currentWordLength; i++) {
                if (showingWord) {
                    display[i] = currentWord[i];
                } else {
                    if (i < wordProgress) {
                        display[i] = currentWord[i];
                    } else {
                        display[i] = '_';
                    }
                }
            }

            printHudMiddle(0, display);
            printHudMiddle(1, difficultyName());
        } else {
            printHudMiddle(0, questionText);
            printHudMiddle(1, difficultyName());
        }
    }

    // ============================================================
    // ABILITY
    // ============================================================

    bool abilityReady(int now) {
        return now - lastAbilityUseTime >= Tuneable::ABILITY_COOLDOWN;
    }

    int abilityRemainingSeconds(int now) {
        int elapsed = now - lastAbilityUseTime;

        if (elapsed >= Tuneable::ABILITY_COOLDOWN) {
            return 0;
        }

        int remaining = Tuneable::ABILITY_COOLDOWN - elapsed;

        return (remaining + 999) / 1000;
    }

    void useAbility(int now) {
        if (!abilityReady(now)) {
            return;
        }

        bool clearedSomething = false;

        for (int i = 0; i < Tuneable::MAX_METEORS; i++) {
            if (meteors[i].active) {
                meteors[i].remove();
                clearedSomething = true;
            }
        }

        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            if (pickups[i].active && pickups[i].type == EntityType::WRONG_PICKUP) {
                pickups[i].remove();
                clearedSomething = true;
            }
        }

        if (clearedSomething) {
            lastAbilityUseTime = now;
            updateAbilityHUD(now, true);
            sound.collect();
        }
    }

    // ============================================================
    // PAUSE
    // ============================================================

    void pauseGame() {
        int pauseStart = timer.read_ms();

        HUD.cls();
        gamelcd.cls();

        HUD.locate(4, 0);
        HUD.printf("PAUSED");

        HUD.locate(4, 1);
        HUD.printf("RESUME");

        gamelcd.locate(2, 1);
        gamelcd.printf("GAME PAUSED");

        gamelcd.locate(1, 2);
        gamelcd.printf("PRESS PAUSE AGAIN");

        buttons.reset(pauseStart);

        while (true) {
            int now = timer.read_ms();

            if (buttons.pausePressedOnce(now)) {
                buttons.waitForPressAndRelease();
                break;
            }

            thread_sleep_for(10);
        }

        int pauseEnd = timer.read_ms();
        int pauseLength = pauseEnd - pauseStart;

        lastFrameTime += pauseLength;
        lastShotStepTime += pauseLength;
        lastMeteorStepTime += pauseLength;
        lastMeteorSpawnTime += pauseLength;
        lastPickupStepTime += pauseLength;
        lastPickupSpawnTime += pauseLength;
        lastAmmoRegenTime += pauseLength;
        lastShootTime += pauseLength;
        lastAbilityUseTime += pauseLength;

        if (showingWord) {
            wordHideTime += pauseLength;
        }

        buttons.reset(pauseEnd);

        gamelcd.cls();
        resetLastGameBuffer();

        forceClearHUD();
        updateHUD();
        updateChallengeHUD();
        updateAbilityHUD(pauseEnd, true);

        renderGame();
    }

    // ============================================================
    // INPUT
    // ============================================================

    void handleInput(int now) {
        if (buttons.pausePressedOnce(now)) {
            pauseGame();
            return;
        }

        if (buttons.upPressedOnce(now)) {
            moveShipUp();
        }

        if (buttons.downPressedOnce(now)) {
            moveShipDown();
        }

        if (buttons.shootPressedOnce(now)) {
            if (now - lastShootTime >= Tuneable::SHOOT_COOLDOWN) {
                spawnShot();
                lastShootTime = now;
            }
        }

        if (buttons.abilityPressedOnce(now)) {
            useAbility(now);
        }
    }

    // ============================================================
    // GAME UPDATE
    // ============================================================

    void updateGame(int now) {
        if (selectedGame == GameMode::WORD && showingWord) {
            if (now >= wordHideTime) {
                showingWord = false;
                updateChallengeHUD();

                lastMeteorSpawnTime = now;
                lastPickupSpawnTime = now;
            }

            updateAbilityHUD(now);
            return;
        }

        if (now - lastShotStepTime >= Tuneable::SHOT_STEP_DELAY) {
            moveShots();
            lastShotStepTime = now;
        }

        if (now - lastMeteorSpawnTime >= meteorSpawnDelay()) {
            spawnMeteor();
            lastMeteorSpawnTime = now;
        }

        if (now - lastPickupSpawnTime >= pickupSpawnDelay()) {
            spawnPickup();
            lastPickupSpawnTime = now;
        }

        if (now - lastMeteorStepTime >= meteorStepDelay()) {
            moveMeteors();
            lastMeteorStepTime = now;
        }

        if (now - lastPickupStepTime >= pickupStepDelay()) {
            movePickups();
            lastPickupStepTime = now;
        }

        if (ammo < Tuneable::MAX_AMMO &&
            now - lastAmmoRegenTime >= Tuneable::AMMO_REGEN_DELAY) {
            ammo++;
            updateHUD();
            lastAmmoRegenTime = now;
        }

        updateAbilityHUD(now);

        handleCollisions(now);
    }

    // ============================================================
    // SPAWNING
    // ============================================================

    void spawnShot() {
        if (ammo <= 0) {
            return;
        }

        for (int i = 0; i < Tuneable::MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].spawn(
                    Tuneable::SHIP_X + 1,
                    shipY,
                    '-',
                    EntityType::SHOT
                );

                ammo--;
                updateHUD();

                sound.laser();

                return;
            }
        }
    }

    void spawnMeteor() {
        for (int i = 0; i < Tuneable::MAX_METEORS; i++) {
            if (!meteors[i].active) {
                meteors[i].spawn(
                    Tuneable::GAME_COLS - 1,
                    randomNumber(Tuneable::GAME_ROWS),
                    Symbol::METEOR,
                    EntityType::METEOR
                );

                return;
            }
        }
    }

    void spawnPickup() {
        if (selectedGame == GameMode::WORD) {
            spawnWordPickup();
        } else {
            spawnNumberPickup();
        }
    }

    void spawnWordPickup() {
        if (wordComplete()) {
            return;
        }

        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            if (!pickups[i].active) {
                bool forceWrong = pickupsSinceChallengeStart < forcedWrongPickupCount();
                bool spawnCorrect = false;

                if (!forceWrong) {
                    spawnCorrect = randomNumber(100) < wordCorrectChance();
                }

                if (spawnCorrect) {
                    pickups[i].spawn(
                        Tuneable::GAME_COLS - 1,
                        randomNumber(Tuneable::GAME_ROWS),
                        neededLetter(),
                        EntityType::CORRECT_PICKUP,
                        wordProgress
                    );
                } else {
                    pickups[i].spawn(
                        Tuneable::GAME_COLS - 1,
                        randomNumber(Tuneable::GAME_ROWS),
                        randomWrongLetter(),
                        EntityType::WRONG_PICKUP
                    );
                }

                pickupsSinceChallengeStart++;

                return;
            }
        }
    }

    void spawnNumberPickup() {
        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            if (!pickups[i].active) {
                bool forceWrong = pickupsSinceChallengeStart < forcedWrongPickupCount();
                bool spawnCorrect = false;

                if (!forceWrong) {
                    spawnCorrect = randomNumber(100) < numberCorrectChance();
                }

                if (spawnCorrect) {
                    pickups[i].spawn(
                        Tuneable::GAME_COLS - 1,
                        randomNumber(Tuneable::GAME_ROWS),
                        answerChar,
                        EntityType::CORRECT_PICKUP
                    );
                } else {
                    pickups[i].spawn(
                        Tuneable::GAME_COLS - 1,
                        randomNumber(Tuneable::GAME_ROWS),
                        randomWrongDigit(),
                        EntityType::WRONG_PICKUP
                    );
                }

                pickupsSinceChallengeStart++;

                return;
            }
        }
    }

    // ============================================================
    // MOVEMENT
    // ============================================================

    void moveShots() {
        for (int i = 0; i < Tuneable::MAX_SHOTS; i++) {
            if (!shots[i].active) {
                continue;
            }

            shots[i].moveRight();

            if (shots[i].offRight()) {
                shots[i].remove();
            }
        }
    }

    void moveMeteors() {
        for (int i = 0; i < Tuneable::MAX_METEORS; i++) {
            if (!meteors[i].active) {
                continue;
            }

            meteors[i].moveLeft();

            if (meteors[i].offLeft()) {
                meteors[i].remove();
            }
        }
    }

    void movePickups() {
        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            if (!pickups[i].active) {
                continue;
            }

            pickups[i].moveLeft();

            if (pickups[i].offLeft()) {
                pickups[i].remove();
            }
        }
    }

    void moveShipUp() {
        if (shipY > 0) {
            shipY--;
        }
    }

    void moveShipDown() {
        if (shipY < Tuneable::GAME_ROWS - 1) {
            shipY++;
        }
    }

    // ============================================================
    // COLLISIONS
    // ============================================================

    void damagePlayer() {
        if (lives > 0) {
            lives--;
        }

        if (lives <= 0) {
            lives = 0;
            gameOver = true;
        }

        updateHUD();
        sound.damage();
    }

    void handleCollisions(int now) {
        handleShotMeteorCollisions();
        handleShotPickupCollisions();
        handleMeteorShipCollisions();
        handlePickupShipCollisions(now);
    }

    void handleShotMeteorCollisions() {
        for (int s = 0; s < Tuneable::MAX_SHOTS; s++) {
            if (!shots[s].active) {
                continue;
            }

            for (int m = 0; m < Tuneable::MAX_METEORS; m++) {
                if (!meteors[m].active) {
                    continue;
                }

                bool sameRow = shots[s].y == meteors[m].y;
                bool closeEnough = abs(shots[s].x - meteors[m].x) <= 1;

                if (sameRow && closeEnough) {
                    shots[s].remove();
                    meteors[m].remove();

                    score++;
                    updateHUD();

                    sound.hit();

                    break;
                }
            }
        }
    }

    void handleShotPickupCollisions() {
        for (int s = 0; s < Tuneable::MAX_SHOTS; s++) {
            if (!shots[s].active) {
                continue;
            }

            for (int p = 0; p < Tuneable::MAX_PICKUPS; p++) {
                if (!pickups[p].active) {
                    continue;
                }

                // In word mode, correct letters cannot be shot.
                // The player has to collect them in the right order.
                if (selectedGame == GameMode::WORD &&
                    pickups[p].type == EntityType::CORRECT_PICKUP) {
                    continue;
                }

                bool sameRow = shots[s].y == pickups[p].y;
                bool closeEnough = abs(shots[s].x - pickups[p].x) <= 1;

                if (sameRow && closeEnough) {
                    shots[s].remove();
                    pickups[p].remove();

                    score++;
                    updateHUD();

                    sound.hit();

                    break;
                }
            }
        }
    }

    void handleMeteorShipCollisions() {
        for (int m = 0; m < Tuneable::MAX_METEORS; m++) {
            if (!meteors[m].active) {
                continue;
            }

            bool sameRow = meteors[m].y == shipY;
            bool hitShip = meteors[m].x <= Tuneable::SHIP_X;

            if (sameRow && hitShip) {
                meteors[m].remove();
                damagePlayer();
            }
        }
    }

    void handlePickupShipCollisions(int now) {
        for (int p = 0; p < Tuneable::MAX_PICKUPS; p++) {
            if (!pickups[p].active) {
                continue;
            }

            bool sameRow = pickups[p].y == shipY;
            bool reachedShip = pickups[p].x <= Tuneable::SHIP_X;

            if (sameRow && reachedShip) {
                if (pickups[p].type == EntityType::CORRECT_PICKUP) {
                    if (selectedGame == GameMode::WORD) {
                        collectCorrectWordLetter(pickups[p], now);
                    } else {
                        collectCorrectNumberAnswer(pickups[p]);
                    }
                } else {
                    pickups[p].remove();
                    damagePlayer();
                }
            }
        }
    }

    // ============================================================
    // GAME LCD BUFFER
    // ============================================================

    void clearGameBuffer() {
        for (int y = 0; y < Tuneable::GAME_ROWS; y++) {
            for (int x = 0; x < Tuneable::GAME_COLS; x++) {
                gameBuffer[y][x] = ' ';
            }
        }
    }

    void resetLastGameBuffer() {
        for (int y = 0; y < Tuneable::GAME_ROWS; y++) {
            for (int x = 0; x < Tuneable::GAME_COLS; x++) {
                lastGameBuffer[y][x] = 0x7F;
            }
        }
    }

    void putGameChar(int x, int y, char c) {
        if (x < 0 || x >= Tuneable::GAME_COLS) {
            return;
        }

        if (y < 0 || y >= Tuneable::GAME_ROWS) {
            return;
        }

        gameBuffer[y][x] = c;
    }

    void buildGameFrame() {
        clearGameBuffer();

        for (int i = 0; i < Tuneable::MAX_METEORS; i++) {
            if (meteors[i].active) {
                putGameChar(meteors[i].x, meteors[i].y, meteors[i].symbol);
            }
        }

        for (int i = 0; i < Tuneable::MAX_PICKUPS; i++) {
            if (pickups[i].active) {
                putGameChar(pickups[i].x, pickups[i].y, pickups[i].symbol);
            }
        }

        for (int i = 0; i < Tuneable::MAX_SHOTS; i++) {
            if (shots[i].active) {
                putGameChar(shots[i].x, shots[i].y, shots[i].symbol);
            }
        }

        putGameChar(Tuneable::SHIP_X, shipY, Symbol::SHIP);
    }

    void flushGameFrame() {
        for (int y = 0; y < Tuneable::GAME_ROWS; y++) {
            int x = 0;

            while (x < Tuneable::GAME_COLS) {
                if (gameBuffer[y][x] == lastGameBuffer[y][x]) {
                    x++;
                    continue;
                }

                gamelcd.locate(x, y);

                while (x < Tuneable::GAME_COLS &&
                       gameBuffer[y][x] != lastGameBuffer[y][x]) {
                    gamelcd.putc(gameBuffer[y][x]);
                    lastGameBuffer[y][x] = gameBuffer[y][x];
                    x++;
                }
            }
        }
    }

    void renderGame() {
        buildGameFrame();
        flushGameFrame();
    }

    // ============================================================
    // HUD
    // ============================================================

    void forceClearHUD() {
        HUD.cls();

        HUD.locate(0, 0);
        HUD.putc(Symbol::HEART);

        HUD.locate(13, 0);
        HUD.printf("000");

        HUD.locate(0, 1);
        HUD.printf("Y ");

        HUD.locate(2, 0);
        HUD.putc(Symbol::BORDER);

        HUD.locate(2, 1);
        HUD.putc(Symbol::BORDER);

        HUD.locate(12, 0);
        HUD.putc(Symbol::BORDER);

        HUD.locate(12, 1);
        HUD.putc(Symbol::BORDER);

        printHudMiddle(0, "");
        printHudMiddle(1, "");

        lastLives = -1;
        lastScore = -1;
        lastAmmo = -1;
        lastAbilityDisplaySeconds = -999;
    }

    void updateHUD() {
        if (lives != lastLives) {
            HUD.locate(0, 0);
            HUD.putc(Symbol::HEART);

            HUD.locate(1, 0);
            HUD.printf("%d", lives);

            lastLives = lives;
        }

        if (score != lastScore) {
            HUD.locate(13, 0);
            HUD.printf("%03d", score % 1000);

            lastScore = score;
        }

        if (ammo != lastAmmo) {
            for (int i = 0; i < Tuneable::MAX_AMMO; i++) {
                HUD.locate(13 + i, 1);

                if (i < ammo) {
                    HUD.putc(Symbol::AMMO);
                } else {
                    HUD.putc(' ');
                }
            }

            lastAmmo = ammo;
        }
    }

    void updateAbilityHUD(int now, bool force = false) {
        int remaining = abilityRemainingSeconds(now);

        if (!force && remaining == lastAbilityDisplaySeconds) {
            return;
        }

        HUD.locate(0, 1);

        if (remaining <= 0) {
            HUD.printf("Y ");
        } else {
            HUD.printf("%-2d", remaining);
        }

        lastAbilityDisplaySeconds = remaining;
    }

    // ============================================================
    // GAME OVER
    // ============================================================

    bool showGameOverScreen() {
        HUD.cls();
        gamelcd.cls();

        sound.gameOverTune();

        HUD.locate(3, 0);
        HUD.printf("GAME OVER");

        HUD.locate(3, 1);
        HUD.printf("Score:%03d", score % 1000);

        gamelcd.locate(0, 0);
        gamelcd.printf("RED    = RESTART");

        gamelcd.locate(0, 1);
        gamelcd.printf("YELLOW = MENU");

        int now = timer.read_ms();
        buttons.reset(now);

        while (true) {
            now = timer.read_ms();

            if (buttons.shootPressedOnce(now)) {
                buttons.waitForPressAndRelease();
                return false;
            }

            if (buttons.abilityPressedOnce(now)) {
                buttons.waitForPressAndRelease();
                return true;
            }

            thread_sleep_for(10);
        }
    }
};

// ================================================================
// HARDWARE
// ================================================================

TextLCD HUD(D1, D2, D4, D5, D6, D7, TextLCD::LCD16x2);
//           RS  E   D4  D5  D6  D7

I2C i2c(D14, D15);
TextLCD_I2C gamelcd(&i2c, 0x4E, TextLCD_I2C::LCD20x4);

Buttons buttons(D8, D9, D10, D11, D12);
//              up  down shoot ability pause

Speaker sound(D3);

// ================================================================
// GAME OBJECT
// ================================================================

Game game(HUD, gamelcd, buttons, sound);

// ================================================================
// MAIN
// ================================================================

int main() {
    i2c.frequency(400000);

    sound.start();

    game.begin();

    game.showMenu();
    game.reset();

    while (true) {
        game.update();
    }
}
