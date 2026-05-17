#pragma once


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