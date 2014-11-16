#pragma once
  
#include "pebble.h"
#include "images.h"
#include "blocks.h"
#include "rand.h"
  
typedef struct crane_ {
  unsigned char block_type;
  unsigned char position;
  unsigned char destination_column;
  bool direction_right;
  bool available;
} crane;

inline static bool is_crane_empty( const crane * c) {
  return c->block_type == 0;
}

#define MAX_CRANES 5

static crane cranes[ MAX_CRANES ];

#define CRANE_WIDTH 17
#define CRANE_HEIGHT 9
#define CRANE_WITH_BLOCK_HEIGHT 15
#define BLOCK_CRANE_OFFSET 2
  
static int g_cranes_count;

inline static void init_cranes( int cranes_count ) {
  g_cranes_count = cranes_count;
  for ( int i=0; i<g_cranes_count; ++i ) {
    cranes[i].available = true;
  }
}

#define LEFT_OFFSET 4
  
static bool is_crane_for_column( int column ) {
  for ( int i=0; i<g_cranes_count; ++i ) {
    if ( cranes[i].destination_column == column )
      return true;
  }
  
  return false;
}
  
inline static void tick_cranes( ) {
  static signed char tick = 0;
  tick++;
  
  for ( int i=0; i<g_cranes_count; ++i ) {
    if ( !cranes[i].available ) {
      cranes[i].position += cranes[i].direction_right ? 1 : -1;
      
      if ( !game_over &&
           cranes[i].position - LEFT_OFFSET - CRANE_WIDTH + BLOCK_CRANE_OFFSET == BLOCK_SIZE * cranes[i].destination_column ) {
        if ( is_empty( &g_blocks[ cranes[i].destination_column ][ FIELD_HEIGHT - 2 ] ) ) {
          score += 2;
          block_info *block = &g_blocks[ cranes[i].destination_column ][ FIELD_HEIGHT - 1 ];
          set_block_type( block, cranes[i].block_type );
          set_block_offset( block, 0 );
          cranes[i].block_type = 0;
          
          if ( is_bonus( block ) ) {
            set_extra_info( cranes[i].destination_column, FIELD_HEIGHT - 1, BLOCK_SIZE + 1 );
          }
        }
        cranes[i].destination_column = FIELD_WIDTH;
      } else if ( !cranes[i].direction_right && cranes[i].position == 0 ) {
        cranes[i].available = true;
      } else if ( cranes[i].direction_right && cranes[i].position == 168 + CRANE_WIDTH*2 ) {
        cranes[i].available = true;
      }
    }
    else {
      static signed char last_block_created_tick = 0;
      
      if ( tick - last_block_created_tick > 31 || ( tick < 0 && last_block_created_tick > 0 ) ) {
        last_block_created_tick = tick;
        
        cranes[i].available = false;
        cranes[i].block_type = _rand() % 4 + 1;
        cranes[i].direction_right = _rand() % 2;
        cranes[i].position = cranes[i].direction_right ? 0 : 168 + CRANE_WIDTH*2;
        
        cranes[i].destination_column = FIELD_WIDTH;
        
        int preffered_column = _rand() % FIELD_WIDTH;
        while ( is_crane_for_column( preffered_column ) )
          preffered_column = _rand() % FIELD_WIDTH;
        
        cranes[i].destination_column = preffered_column;
        
        // do not offer extra jumps for first character
        if ( _rand() % 40 == 0 )
          cranes[i].block_type = _rand() % ( current_character == 0 ? 2 : 4) + 7;
        
        if ( _rand() % 10000 != 1023 && cranes[i].block_type == 10 )
          cranes[i].block_type = 11;
      }
    }
  }
}

static void draw_cranes(Layer *layer, GContext *ctx) {
  for ( int i=0; i<g_cranes_count; ++i ) {
    if ( !cranes[i].available ) {
      graphics_context_set_compositing_mode( ctx, GCompOpAnd );
      graphics_draw_bitmap_in_rect( ctx, s_crane_bitmap, 
                                    GRect( CRANE_WITH_BLOCK_HEIGHT - CRANE_HEIGHT, cranes[i].position, CRANE_HEIGHT, CRANE_WIDTH ) );
      graphics_context_set_compositing_mode( ctx, GCompOpAssign  );
      if ( !is_crane_empty( &cranes[i] ) )
        graphics_draw_bitmap_in_rect( ctx, s_boxes_bitmap[ cranes[i].block_type - 1 ], 
                                      GRect( 0, cranes[i].position + BLOCK_CRANE_OFFSET, BLOCK_SIZE, BLOCK_SIZE ) );
    }
  }
}