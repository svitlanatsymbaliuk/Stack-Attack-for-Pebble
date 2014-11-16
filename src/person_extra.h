#pragma once
  
static unsigned char person_x = 3, person_y = 10;
static signed char person_offset_x = 0;
static unsigned char person_offset_y = 0;
static unsigned char dead_ticks_left = 0;
static unsigned char current_character;
static bool game_over = false;
static int combos = 0;;

#define PETE   0
#define LIZZIE 1
#define FRANK  2
#define WILL   3
#define CATH   4
#define SAM    5

static unsigned char lives = 3, jumps = 0, power = 0;

static void proceed_death() {
  if ( power ) {
    power = 0;
    return;
  }
  
  if ( !dead_ticks_left ) {
    --lives;
    dead_ticks_left = 120;
    if ( !lives )
      game_over = true;
  }
}

static void check_person_on_exploding( int i, int j ) {
  int dx    = abs( person_x - i );
  int dx_ex = abs( person_x + (person_offset_x ? ( person_offset_x > 0 ? -1 : 1 )  : 0) - i );
  
  if ( dx <= 1 || dx_ex <= 1 ) {
    int dy    = abs( person_y - j );
    if ( dy <= 1 || ( person_offset_y > 0 && person_y - j == 2 ) ) {
      proceed_death();
    }
  }
}