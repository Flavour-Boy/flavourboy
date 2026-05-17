#ifndef NUMBANK_H
#define NUMBANK_H

// ================================================================
// NUMBER QUESTION BANK
// Answers must stay between 0 and 99 because number pickups support
// one-digit and two-digit answers on the 20x4 LCD.
// Keep question text at 9 characters or fewer so it fits the HUD area.
// ================================================================

namespace NumBank {

struct Question {
    const char* text;
    int answer;
};

static const Question EASY_QUESTIONS[] = {
    {"2+3=?", 5},
    {"4+4=?", 8},
    {"7-2=?", 5},
    {"9-6=?", 3},
    {"3x2=?", 6},
    {"4x2=?", 8},
    {"8/2=?", 4},
    {"9/3=?", 3},
    {"5+6=?", 11},
    {"8+7=?", 15},
    {"12-4=?", 8},
    {"10-3=?", 7}
};

static const Question NORMAL_QUESTIONS[] = {
    {"6+8=?", 14},
    {"9+7=?", 16},
    {"12+5=?", 17},
    {"15-7=?", 8},
    {"18-9=?", 9},
    {"6x4=?", 24},
    {"7x3=?", 21},
    {"8x5=?", 40},
    {"24/6=?", 4},
    {"36/9=?", 4},
    {"45/5=?", 9},
    {"11+18=?", 29}
};

static const Question HARD_QUESTIONS[] = {
    {"13+24=?", 37},
    {"28+17=?", 45},
    {"42-19=?", 23},
    {"64-28=?", 36},
    {"9x6=?", 54},
    {"8x7=?", 56},
    {"12x8=?", 96},
    {"11x7=?", 77},
    {"72/8=?", 9},
    {"81/9=?", 9},
    {"96/12=?", 8},
    {"99-45=?", 54}
};

static const int EASY_QUESTION_COUNT = sizeof(EASY_QUESTIONS) / sizeof(EASY_QUESTIONS[0]);
static const int NORMAL_QUESTION_COUNT = sizeof(NORMAL_QUESTIONS) / sizeof(NORMAL_QUESTIONS[0]);
static const int HARD_QUESTION_COUNT = sizeof(HARD_QUESTIONS) / sizeof(HARD_QUESTIONS[0]);

}

#endif
