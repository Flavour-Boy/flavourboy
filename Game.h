#ifndef GAME_H
#define GAME_H

#include "mbed.h"
#include "TextLCD_CC.h"
#include "SoundEffects.h"
#include "WordBank.h"
#include "NumBank.h"
#include "tuneable.h"
#include "buttons.h"
#include "entity.h"

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
    int answerValue;
    int lastQuestionIndex;

    int pickupsSinceChallengeStart;

    int meteorStepDelay();
    int meteorSpawnDelay();
    int pickupStepDelay();
    int pickupSpawnDelay();

    int forcedWrongPickupCount();
    int wordCorrectChance();
    int numberCorrectChance();
    const char* difficultyName();

    void chooseGameMenu();
    void chooseDifficultyMenu();
    void drawGameMenu(int option);
    void drawDifficultyMenu(int option);

    void loadCustomCharacters();
    void clearAllEntities();
    void clearPickupsOnly();

    int randomNumber(int maxValue);

    void startNewWord(int now);
    bool wordComplete();
    char neededLetter();
    char randomWrongLetter();
    void collectCorrectWordLetter(Entity& pickup, int now);

    const NumBank::Question* currentNumberBank();
    int currentNumberBankCount();
    void copyQuestionText(const char* text);
    void startNewQuestion();
    int numberPickupWidth(int value);
    char numberPickupFirstChar(int value);
    char numberPickupSecondChar(int value);
    int randomWrongNumberAnswer();
    void collectCorrectNumberAnswer(Entity& pickup);

    void printHudMiddle(int row, const char* text);
    void updateChallengeHUD();

    bool abilityReady(int now);
    int abilityRemainingSeconds(int now);
    void useAbility(int now);
    void pauseGame();

    void handleInput(int now);
    void updateGame(int now);

    void spawnShot();
    void spawnMeteor();
    void spawnPickup();
    void spawnWordPickup();
    void spawnNumberPickup();

    void moveShots();
    void moveMeteors();
    void movePickups();
    void moveShipUp();
    void moveShipDown();

    void damagePlayer();

    void handleCollisions(int now);
    void handleShotMeteorCollisions();
    bool shotHitsPickup(Entity& shot, Entity& pickup);
    void handleShotPickupCollisions();
    void handleMeteorShipCollisions();
    void handlePickupShipCollisions(int now);

    void clearGameBuffer();
    void resetLastGameBuffer();
    void putGameChar(int x, int y, char c);
    void putNumberPickup(Entity& pickup);
    void buildGameFrame();
    void flushGameFrame();
    void renderGame();

    void forceClearHUD();
    void updateHUD();
    void updateAbilityHUD(int now, bool force = false);

    bool showGameOverScreen();

public:
    Game(TextLCD& hudRef,
         TextLCD_I2C& gameLcdRef,
         Buttons& buttonsRef,
         Speaker& soundRef);

    void begin();
    void showMenu();
    void reset();
    void update();
};

#endif
