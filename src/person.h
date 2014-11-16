#pragma once
  
#include "pebble.h"
#include "blocks.h"
#include "tertiary_text.h"
#include "stored_settings.h"
  
#define PERSON_WIDTH  BLOCK_SIZE
#define PERSON_HEIGHT BLOCK_SIZE*2

signed char jump_ticks_left = 0;
#define JUMP_THRESHOLD_LIMITER -(BLOCK_SIZE + 5)

static bool is_left_button_pressed  = false;
static bool is_right_button_pressed  = false;
static bool is_middle_button_pressed = false;

static bool ready_for_doublejump = false;
static bool doublejumped = false;
  
static void show_highscores();
static void write_highscores();
static void on_name_entered( const char *name )
{
  window_stack_pop( true );
  int index = 2;
  if ( stored_highscores[0].score < score ) {
    index = 0;
  }
  else if ( stored_highscores[1].score < score ) {
    index = 1;
  }
  
  for ( int i=2; i>index; --i ) {
    strcpy( stored_highscores[i].name, stored_highscores[i-1].name );
    stored_highscores[i].score = stored_highscores[i-1].score;
  }
  strcpy( stored_highscores[index].name, name );
  stored_highscores[index].score = score;
  
  write_highscores();
  show_highscores();
}

static void on_game_over() {
  window_stack_pop( true );
  if ( stored_highscores[2].score < score )
    tertiary_enter_text( on_name_entered );
  else
    show_highscores();
}

static void left_button_pressed( ClickRecognizerRef recognizer, void *context ) {
  is_left_button_pressed = true;
}

static void left_button_released( ClickRecognizerRef recognizer, void *context ) {  
  is_left_button_pressed = false;
  
  if ( game_over )
    on_game_over();
}

static void right_button_pressed( ClickRecognizerRef recognizer, void *context ) {  
  is_right_button_pressed = true;
}

static void right_button_released( ClickRecognizerRef recognizer, void *context ) {
  is_right_button_pressed = false;
  
  if ( game_over )
    on_game_over();
}

static void middle_button_pressed( ClickRecognizerRef recognizer, void *context ) {  
  is_middle_button_pressed = true;
}

static void middle_button_released( ClickRecognizerRef recognizer, void *context ) {
  ready_for_doublejump = true;
  is_middle_button_pressed = false;
  
  if ( game_over )
    on_game_over();
}

static void proceed_bonus( int bonus_type ) {
  switch( bonus_type ) {
    case 7:  power = 1; break;
    case 8:  lives++;   break;
    case 9:  jumps += 3; if ( jumps > current_character ) jumps = current_character; break;
    case 10: if ( unlocked_characters==5 ) break;
             persist_write_int  ( UNLOCKED_CHARACTERS, ++unlocked_characters );  break;
  }
  /*static const uint32_t const segments[] = { 200 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);*/
}

static bool block_can_be_moved_right( int i, int j ) {
  if ( i == FIELD_WIDTH - 1 )
    return false;
  
  if ( i == 0 )
    return false;
  
  if ( !is_empty( &g_blocks[i][j+1] ) )
    return false;
  
  if ( !is_falling(&g_blocks[i][j]) )
    return is_empty( &g_blocks[i+1][j] ) && !is_falling(&g_blocks[i+1][j+1]);
  
  return is_empty( &g_blocks[i+1][j] ) && ( j==0 || is_empty( &g_blocks[i+1][j-1] ) || is_falling(&g_blocks[i+1][j-1]) );
}

void push_block( int x, int y, bool right ) {
  if ( is_empty(&g_blocks[x][y]) )
    return;
  
  pushing_y = y;
  if ( right ) {
    pushing_x = x+1;
    pushing_offset = BLOCK_SIZE;
  } else {
    pushing_x = x-1;
    pushing_offset = -BLOCK_SIZE;
  }
  
  g_blocks[pushing_x][y] = g_blocks[x][y];
  
  set_extra_info( pushing_x, y, get_extra_info( x, y ) );
  set_extra_info( x, y, 0 );
  
  clear_block( &g_blocks[x][y] );
}

static void proceed_move_right() {
  bool move_right = false;
  if ( is_right_button_pressed &&
       person_offset_x == 0 &&
       person_offset_y == 0 && 
       person_x != FIELD_WIDTH - 1
     ) {
    if ( ( is_empty(&g_blocks[person_x + 1][person_y]) || is_bonus(&g_blocks[person_x + 1][person_y]) ) && 
         ( is_empty(&g_blocks[person_x + 1][person_y + 1]) || 
           is_bonus(&g_blocks[person_x + 1][person_y + 1]) 
         ) &&
         ( !is_falling(&g_blocks[person_x + 1][person_y + 2]) 
           || is_bonus(&g_blocks[person_x + 1][person_y + 2])
         )
       ) {
      move_right = true;
    }
    
    if ( is_empty(&g_blocks[person_x+1][person_y]) && 
         is_falling(&g_blocks[person_x+1][person_y+1]) &&
         block_can_be_moved_right(person_x+1, person_y+1) 
       ) {
      if ( is_bonus( &g_blocks[person_x + 1][person_y + 1] ) ) {
        proceed_bonus( get_block_type( &g_blocks[person_x + 1][person_y + 1] ) );
        clear_block( &g_blocks[person_x + 1][person_y + 1] );
      } else {
        push_block( person_x + 1, person_y + 1, true );
      }
    } 
    else if ( block_can_be_moved_right(person_x + 1, person_y) ) {
      if ( is_bonus( &g_blocks[person_x + 1][person_y] ) ) {
        proceed_bonus( get_block_type( &g_blocks[person_x + 1][person_y] ) );
        clear_block( &g_blocks[person_x + 1][person_y] );
      } else {
        push_block( person_x + 1, person_y, true );
      }
    }
  }     
  
  if ( move_right || person_offset_x < 0 ) {
    if ( is_bonus(&g_blocks[person_x + 1][person_y]) ) {
      proceed_bonus( get_block_type( &g_blocks[person_x + 1][person_y] ) );
      clear_block( &g_blocks[person_x + 1][person_y] );
    }
    --person_offset_x;
    if ( person_offset_x == -BLOCK_SIZE ) {
      ++person_x;
      person_offset_x = 0;
    }
  }
}

static bool block_can_be_moved_left( int i, int j ) {
  if ( i == FIELD_WIDTH - 1 )
    return false;
  
  if ( i == 0 )
    return false;
  
  if ( !is_empty( &g_blocks[i][j+1] ) )
    return false;
  
  if ( !is_falling(&g_blocks[i][j]) )
    return is_empty( &g_blocks[i-1][j] ) && !is_falling(&g_blocks[i-1][j+1]);
  
  return is_empty( &g_blocks[i-1][j] ) && ( j==0 || is_empty( &g_blocks[i-1][j-1] ) || is_falling(&g_blocks[i-1][j-1]) );
}

static void proceed_move_left() {
  bool move_left = false;
  if ( is_left_button_pressed &&
       person_offset_x == 0 &&
       person_offset_y == 0 && 
       person_x != 0
     ) {
    if ( ( is_empty(&g_blocks[person_x - 1][person_y]) || is_bonus(&g_blocks[person_x - 1][person_y]) ) && 
         ( is_empty(&g_blocks[person_x - 1][person_y + 1]) || is_bonus(&g_blocks[person_x - 1][person_y + 1]) ) &&
         ( !is_falling(&g_blocks[person_x - 1][person_y + 2]) || is_bonus(&g_blocks[person_x - 1][person_y + 2])
         )
       ) {
      move_left = true;
    }
    
    if ( is_empty(&g_blocks[person_x-1][person_y]) && 
         is_falling(&g_blocks[person_x-1][person_y+1]) &&
         block_can_be_moved_left(person_x-1, person_y+1) 
       ) {
      if ( is_bonus( &g_blocks[person_x - 1][person_y + 1] ) ) {
        proceed_bonus( get_block_type( &g_blocks[person_x - 1][person_y + 1] ) );
        clear_block( &g_blocks[person_x - 1][person_y + 1] );
      } else {
        push_block( person_x - 1, person_y + 1, false );
      }
    } 
    else if ( block_can_be_moved_left(person_x-1, person_y) ) {
      if ( is_bonus( &g_blocks[person_x - 1][person_y] ) ) {
        proceed_bonus( get_block_type( &g_blocks[person_x - 1][person_y] ) );
        clear_block( &g_blocks[person_x - 1][person_y] );
      } else {
        push_block( person_x - 1, person_y, false );
      }
    }
  }     
  
  if ( move_left || person_offset_x > 0 ) {
    if ( is_bonus(&g_blocks[person_x - 1][person_y]) ) {
      proceed_bonus( get_block_type( &g_blocks[person_x - 1][person_y] ) );
      clear_block( &g_blocks[person_x - 1][person_y] );
    }
    ++person_offset_x;
    if ( person_offset_x == BLOCK_SIZE ) {
      --person_x;
      person_offset_x = 0;
    }
  }
}

static void proceed_falling_block( int x, int y, bool allow_to_break ) {
  if ( !is_empty(&g_blocks[x][y]) ) {
    if ( is_bonus(&g_blocks[x][y]) ) {
      proceed_bonus( get_block_type( &g_blocks[x][y] ) );
    } else {
      if ( allow_to_break && jump_ticks_left > 0 ) {
        set_block_type   ( &g_blocks[x][y], 6 );
        return;
      }
      proceed_death();
    }
    clear_block( &g_blocks[x][y] );
  }
}

static void proceed_falling_blocks_impl( int p_x ) {
  proceed_falling_block( p_x, person_y, false );
  
  proceed_falling_block( p_x, person_y + 1, false );
  
  if ( is_falling(&g_blocks[person_x][person_y + 2]) &&
       person_offset_y < get_block_offset( &g_blocks[person_x][person_y + 2] ) )
      proceed_falling_block( p_x, person_y + 2, true );
}

static void proceed_falling_blocks( ) {
  proceed_falling_blocks_impl ( person_x );
  
  if ( person_offset_x )
    proceed_falling_blocks_impl ( person_x + ( person_offset_x > 0 ? -1 : 1 ) );
}

static bool can_fall() {
  return ( person_y != 0 && 
           ( !person_offset_x ||
             ( is_empty(&g_blocks[person_x + ( person_offset_x > 0 ? -1 : 1 )][person_y - 1]) ||
               is_bonus(&g_blocks[person_x + ( person_offset_x > 0 ? -1 : 1 )][person_y - 1]) ||
               is_falling(&g_blocks[person_x + ( person_offset_x > 0 ? -1 : 1 )][person_y - 1]) )
           ) &&
           ( is_empty(&g_blocks[person_x][person_y - 1]) ||
             is_bonus(&g_blocks[person_x][person_y - 1]) ||
             is_falling(&g_blocks[person_x][person_y - 1])
           )
         );
}

static bool is_person_falling() {
  return person_offset_y || can_fall();
}

static void proceed_jumps() {
  if ( !is_middle_button_pressed ) 
    return;
  
  if ( jump_ticks_left == JUMP_THRESHOLD_LIMITER && // do not allow to jump always
       person_offset_x == 0 && person_offset_y == 0 && 
       ( person_y == 0 || 
         ( !is_empty(&g_blocks[person_x][person_y - 1]) && 
          !is_bonus(&g_blocks[person_x][person_y - 1]) && 
          !is_falling(&g_blocks[person_x][person_y - 1]) 
         ) 
       )
     ) {
    ready_for_doublejump = false;
    jump_ticks_left = BLOCK_SIZE + 2;
    doublejumped = false;
    return;
  }
      
  if ( jumps && ready_for_doublejump && !doublejumped && jump_ticks_left > 0 ) {
    ready_for_doublejump = false;
    doublejumped = true;
    --jumps;
    jump_ticks_left += BLOCK_SIZE + 2;
  }
}
  
inline static void tick_person( ) {
  if ( dead_ticks_left ) {
    --dead_ticks_left;
  } 
  
  if ( dead_ticks_left <= 60 ) {
    if ( game_over ) {
      dead_ticks_left = 108;
    } else {
      proceed_jumps();
      proceed_jumps();
      proceed_move_right();
      proceed_move_left();
    }
  }
  
  proceed_falling_blocks();
  
  if ( jump_ticks_left > JUMP_THRESHOLD_LIMITER )
    --jump_ticks_left;

  if ( jump_ticks_left >= 0 ) {
    // proceed jumps 
    if ( person_offset_y == 0 ) {
      ++person_y;
      person_offset_y = BLOCK_SIZE - 1;
    } else {
      --person_offset_y;
    }
  } else {
    // process falling down
    if ( person_offset_y ) {
      ++person_offset_y;
    
      if ( person_offset_y == BLOCK_SIZE ) {
        --person_y;
        if ( is_bonus(&g_blocks[person_x][person_y]) ) {
          proceed_bonus( get_block_type( &g_blocks[person_x][person_y] ) );
          clear_block( &g_blocks[person_x][person_y ] );
        }
        person_offset_y = 0;
      }
      return;
    }
      
    if ( can_fall() ) {
      person_offset_y = 1;
    }
  }
}

static void draw_person(Layer *layer, GContext *ctx) {
  static int tick = 0;
  tick++;
  
  if( dead_ticks_left < 114 && dead_ticks_left % 8 >= 4 && !game_over )
    return;
    
  GBitmap *w, *b;
  
  int index;
  if ( dead_ticks_left > 60 ) {
    if ( dead_ticks_left >= 117 )
      index = 20;
    else if ( dead_ticks_left >= 114 )
      index = 21;
    else {
      index = 22 + dead_ticks_left / 3 % 4;
    }
  } else if ( pushing_offset > 0 ) {
    index = 12 + tick / 2 % 3;
  } else if ( pushing_offset < 0 ) {
    index = 15 + tick / 2 % 3;
  } else if ( person_offset_x == 0 && !is_left_button_pressed && !is_right_button_pressed ) {
    if ( is_person_falling() )
      index = 0;
    else
      index =  tick % 54 > 36 ? tick / 3 % 6 : 0;
  } else if ( is_left_button_pressed || person_offset_x > 0 ) {
    if ( is_person_falling() )
      index = 19;
    else
      index = 9 + tick / 2 % 3;
  } else {
    if ( is_person_falling() )
      index = 18;
    else
      index = 6 + tick / 2 % 3;
  }
  
  w = person_bitmaps[index].w;
  b = person_bitmaps[index].b;
  
  GRect person_rect = GRect( person_y * BLOCK_SIZE - person_offset_y, person_x * BLOCK_SIZE - person_offset_x, 
                             w->bounds.size.w, w->bounds.size.h );

  graphics_context_set_compositing_mode( ctx, GCompOpOr );
  graphics_draw_bitmap_in_rect( ctx, w, person_rect );
  
  graphics_context_set_compositing_mode( ctx, GCompOpClear );
  graphics_draw_bitmap_in_rect( ctx, b, person_rect );
}

inline static void init_person() {
  jump_ticks_left = 0;
  is_left_button_pressed   = false;
  is_right_button_pressed  = false;
  is_middle_button_pressed = false;
  ready_for_doublejump     = false;
  doublejumped             = false;
  
  person_x        = 3;
  person_y        = 3;
  person_offset_x = 0;
  person_offset_y = 0;
  dead_ticks_left = 0;
  
  game_over = false;
  lives  = 3;
  jumps  = 0;
  power  = 0;
}