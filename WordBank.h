#pragma once



// Keep words 9 letters or less.
// The HUD middle space is only 9 characters wide.

namespace WordBank {
    static const char* const WORDS[] = {
        // Space / game theme
        "SPACE",
        "LASER",
        "ROCKET",
        "ALIEN",
        "SHIP",
        "ORBIT",
        "COMET",
        "PLANET",
        "GALAXY",
        "ASTRO",
        "POWER",
        "BOOST",
        "RADAR",
        "STORM",
        "FLAME",
        "LIGHT",

        // Electronics / project theme
        "MBED",
        "CODE",
        "DEBUG",
        "BUTTON",
        "SENSOR",
        "SCREEN",
        "CIRCUIT",
        "WIRE",
        "BREAD",
        "BOARD",
        "SPEAKER",
        "DISPLAY",
        "INPUT",
        "OUTPUT",
        "SIGNAL",
        "VOLTAGE",

        // General easy words
        "APPLE",
        "BRAIN",
        "CLOUD",
        "DREAM",
        "EARTH",
        "FROST",
        "GHOST",
        "HONEY",
        "IGLOO",
        "JELLY",
        "KNIFE",
        "LEMON",
        "MAGIC",
        "NIGHT",
        "OCEAN",
        "PIZZA",
        "QUEEN",
        "RIVER",
        "SNAKE",
        "TIGER",
        "UNITY",
        "VIRUS",
        "WATER",
        "YOUTH",
        "ZEBRA",

        // Slightly harder
        "SYSTEM",
        "PLAYER",
        "MEMORY",
        "TARGET",
        "DANGER",
        "SHIELD",
        "ENERGY",
        "PUZZLE",
        "RANDOM",
        "VECTOR",
        "BINARY",
        "PIXEL"
    };

    static const int WORD_COUNT = sizeof(WORDS) / sizeof(WORDS[0]);
}