#pragma once

static bool vibration_state = false;
static bool light_state     = false;

typedef struct highscore_ {
  char           name[20];
  unsigned int   score;
} highscore;

static highscore stored_highscores[3];
static unsigned char unlocked_characters;

#define VIBRATION_KEY       0
#define LIGHT_KEY           1
#define HIGHSCORES_KEY      2
#define UNLOCKED_CHARACTERS 3
