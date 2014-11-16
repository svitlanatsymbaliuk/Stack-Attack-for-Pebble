#pragma once
  
#include "pebble.h"
#include "images.h"
#include "person_extra.h"
#include "utils.h"
#include "rand.h"
#include "stored_settings.h"
  
#define FIELD_WIDTH  12
#define FIELD_HEIGHT 8
#define FIELD_HEIGHT_RESERVE 5
#define FIELD_HEIGHT_TOTAL ( FIELD_HEIGHT + FIELD_HEIGHT_RESERVE )
#define BLOCK_SIZE   13
  
static signed char pushing_x = 0, pushing_y = 0;
static signed char pushing_offset = 0;
static unsigned int score = 0;
static unsigned int last_shown_score = 0;

typedef unsigned char block_info;
static block_info g_extra_info[FIELD_WIDTH][FIELD_HEIGHT];
static block_info g_blocks[FIELD_WIDTH][FIELD_HEIGHT_TOTAL];

inline static void set_block_type( block_info *info, unsigned char type ) {
  *info &= 0xF0;
  *info |= type & 0x0F;
}

inline static unsigned char get_block_type( block_info *info ) {
  return *info & 0x0F;
}

inline static unsigned char get_extra_info( int i, int j ) {
  return g_extra_info[i][j];
}

inline static void set_extra_info( int i, int j, unsigned char extra_info ) {
  g_extra_info[i][j] = extra_info;
}

inline static bool is_empty( block_info *info ) {
  unsigned char type = get_block_type( info );
  return type == 0 || type == 5 || type == 6;
}

inline static bool is_bonus( block_info *info ) {
  unsigned char type = get_block_type( info );
  return type >= 7 && type <= 10;
}

inline static bool is_bomb( block_info *info ) {
  unsigned char type = get_block_type( info );
  return type == 11;
}

inline static void set_block_offset( block_info *info, unsigned char offset ) {
  *info &= 0xFF0F;
  *info |= offset << 4;
}

inline static unsigned char get_block_offset( block_info *info ) {
  return ( *info & 0x00F0 ) >> 4;
}

inline static void clear_block( block_info *info ) {
  *info = 0;
}

inline static void destroy_block( block_info *info ) {
  if ( is_empty(info) )
    return;
  
  set_block_type   ( info, 6 );
  set_block_offset ( info, 0 );
}

inline static void init_blocks() {
  pushing_x        = 0;
  pushing_y        = 0;
  pushing_offset   = 0;
  score            = 0;
  last_shown_score = 0;
  combos           = 0;
  
  for ( int i=0; i<FIELD_WIDTH; ++i ) {
    for ( int j=0; j<2; ++j ) {
      if ( _rand() % 2 ) 
        clear_block( &g_blocks[i][j] );
      else {
        set_block_type   ( &g_blocks[i][j], _rand() % 5 );
        set_block_offset ( &g_blocks[i][j], 0 );
      }
    }
    
    for ( int j=2; j<FIELD_HEIGHT_TOTAL; ++j ) {
      clear_block      ( &g_blocks[i][j] );
    }
  }
}

static bool neccessary_to_draw_bomb( int i, int j ) {
  unsigned char extra_info = get_extra_info( i, j );
  if ( extra_info < 100 )
    return true;
  
  if ( extra_info > 192 )
    return extra_info % 32 < 28;
  
  if ( extra_info > 140 )
    return extra_info % 16 < 12;
  
  return extra_info % 8 < 4;
}

static void draw_blocks(Layer *layer, GContext *ctx) {
  static unsigned char tick = 0;
  tick++;
  
  for ( int i=0; i<FIELD_WIDTH; ++i )
    for ( int j=0; j<FIELD_HEIGHT; ++j ) {
      block_info *current_block = &g_blocks[i][j];
      unsigned char block_type = get_block_type( current_block );
      if ( block_type 
           && ( !is_bonus( &g_blocks[i][j] ) || tick % 8 < 4 ) 
           && ( !is_bomb( &g_blocks[i][j] ) || neccessary_to_draw_bomb( i,j ) ) )
        graphics_draw_bitmap_in_rect( ctx, s_boxes_bitmap[ get_block_type( current_block )-1 ], 
                                      GRect( j*BLOCK_SIZE - get_block_offset( current_block ),
                                             i*BLOCK_SIZE - ( ( i == pushing_x && j == pushing_y ) ? pushing_offset : 0 ),
                                             BLOCK_SIZE, BLOCK_SIZE ) 
                                    );
    }
}

static bool is_falling( block_info *block ) {
  // bottom blocks never fall
  if ( ( block - &g_blocks[0][0] ) % FIELD_HEIGHT_TOTAL == 0 )
    return false;
  
  block_info *bottom_block = block - 1;
  return !is_empty(block) && 
         ( is_empty( bottom_block ) || get_block_offset( bottom_block ) );
}

static bool is_bottom_row_full() {
  for ( int i=0; i<FIELD_WIDTH; ++i ) {
    if ( i == pushing_x && 0 == pushing_y )
        continue;
    
    if ( is_empty( &g_blocks[i][0] ) )
      return false;
  }
  
  return true;
}

static void explode_bomb( int i, int j ) {
  check_person_on_exploding( i,j );
  
  destroy_block ( &g_blocks[i][j] );
  if ( i != 0 ) destroy_block ( &g_blocks[i-1][j] );
  if ( i != FIELD_WIDTH-1 ) destroy_block ( &g_blocks[i+1][j] );
  
  if ( j != 0 ) {
    destroy_block ( &g_blocks[i][j-1] );
    if ( i != 0 ) destroy_block ( &g_blocks[i-1][j-1] );
    if ( i != FIELD_WIDTH-1 ) destroy_block ( &g_blocks[i+1][j-1] );
  }
  
  destroy_block ( &g_blocks[i][j+1] );
  if ( i != 0 ) destroy_block ( &g_blocks[i-1][j+1] );
  if ( i != FIELD_WIDTH-1 ) destroy_block ( &g_blocks[i+1][j+1] );

  if ( is_falling( &g_blocks[i][j+2] ) ) destroy_block ( &g_blocks[i][j+2] );
  if ( i != 0 && is_falling( &g_blocks[i-1][j+2] ) ) destroy_block ( &g_blocks[i-1][j+2] );
  if ( i != FIELD_WIDTH-1 && is_falling( &g_blocks[i+1][j+2] ) ) destroy_block ( &g_blocks[i+1][j+2] );
}

static int same_in_row( int i, int j ) {
  if ( i == pushing_x && j == pushing_y )
    return 1;
  
  if ( i+1 < FIELD_WIDTH && !is_falling(&g_blocks[i+1][j]) && get_block_type( &g_blocks[i][j] ) == get_block_type( &g_blocks[i+1][j] ) )
    return 1 + same_in_row( i+1, j );
  
  return 1;
}

static int same_in_column( int i, int j ) {
  if ( i == pushing_x && j == pushing_y )
    return 1;
  
  if ( j > 0 && !is_falling(&g_blocks[i][j-1]) && get_block_type( &g_blocks[i][j] ) == get_block_type( &g_blocks[i][j-1] ) )
    return 1 + same_in_column( i, j-1 );
  
  return 1;
}

inline static void tick_blocks() {
  // proceed pushing block
  if ( pushing_offset ) {
    if ( pushing_offset > 0 ) {
      --pushing_offset;
    } else {
      ++pushing_offset;
    }
    
    if ( pushing_offset == 0 ) {
        pushing_x = -1;
      }
  }
  
  // proceed destruction
  for ( int i=0; i<FIELD_WIDTH; ++i )
    for ( int j=0; j<FIELD_HEIGHT; ++j ) {
      unsigned char type = get_block_type( &g_blocks[i][j] );
      if ( type == 6 )
        set_block_type( &g_blocks[i][j], 5 );
      else if ( type == 5 )
        clear_block( &g_blocks[i][j] );
    }
  
  // proceed bottom row
  if ( is_bottom_row_full() ) {
    for ( int i=0; i<FIELD_WIDTH; ++i )
      destroy_block( &g_blocks[i][0] );
    
    score += 20;
  }
  
  // proceed 3 and more in row
  for ( int j=0; j<FIELD_HEIGHT; ++j ) {
    for( int i=0; i<FIELD_WIDTH-2;  ) {
      if ( is_empty(&g_blocks[i][j]) || is_falling(&g_blocks[i][j]) ) {
        ++i;
        continue;
      }
      
      int same_blocks = same_in_row( i, j );
      if ( same_blocks >= 3 ) {
        int destroyed_blocks = 0;
        for ( int k=0; k<same_blocks; ++k ) {
          int same_blocks_in_column = same_in_column( i+k, j );
          if ( same_blocks_in_column == 3) {
            // 4 blocks in column fix
            if ( !is_empty(&g_blocks[i+k][j+1]) && !is_falling(&g_blocks[i+k][j+1]) && same_in_column( i+k,j+1 ) == 4 ) {
              destroyed_blocks++;
              destroy_block( &g_blocks[i + k][j + 1] );
            }
            
            for ( int l=0; l<same_blocks_in_column; ++l ) {
              destroyed_blocks++;
              destroy_block( &g_blocks[i + k][j - l] );
            }
          }
          
          destroyed_blocks++;
          destroy_block( &g_blocks[i + k][j] );
        }
        
        static unsigned short scores[] = { 0, 0, 0, 50, 80, 130, 210, 340, 550, 890, 1440, 2330, 3770 };
        score += scores[ destroyed_blocks ];
        
        combos += destroyed_blocks;
      }
      i += same_blocks;
    }
  }
  
  // proceed 3 or more in column
  for( int i=0; i<FIELD_WIDTH; ++i ) {
    for ( int j=FIELD_HEIGHT-1; j > 0; ) {
      if ( is_empty(&g_blocks[i][j]) || is_falling(&g_blocks[i][j]) ) {
        --j;
        continue;
      }
      
      int same_blocks = same_in_column( i, j );
      if ( same_blocks >= 3 ) {
        for ( int k=0; k<same_blocks; ++k ) {
          destroy_block( &g_blocks[i][j - k] );
        }
        combos += same_blocks;
      }
      j -= same_blocks;
    }
  }
  
  static unsigned short combos_for_unlock[] = { 10, 100, 200, 500, 1000 };
  if ( unlocked_characters < 5 && combos > combos_for_unlock[unlocked_characters] ) {
    persist_write_int( UNLOCKED_CHARACTERS, ++unlocked_characters );
  }
  
  // process blocks fall
  for ( int i=0; i<FIELD_WIDTH; ++i ) {
    for ( int j=1; j<FIELD_HEIGHT; ++j ) {
      if ( i == pushing_x && j == pushing_y )
        continue;
      
      block_info *current_block = &g_blocks[i][j];
      block_info *bottom_block  = &g_blocks[i][j-1];
    
      if ( is_falling( current_block ) ) {
        unsigned char current_offset = get_block_offset( current_block );
        if ( current_offset + 1 == BLOCK_SIZE ) {
          *bottom_block = *current_block;
          set_extra_info( i, j-1, get_extra_info( i, j ) );
          set_extra_info( i, j, 0 );
          
          set_block_offset ( bottom_block, 0 );
          clear_block( current_block );
        } else {
          set_block_offset( current_block, current_offset + 1 );
        }
      }
    }
  }
  
  // proceed bonuses and bombs
  for ( int i=0; i<FIELD_WIDTH; ++i ) {
    for ( int j=0; j<FIELD_HEIGHT; ++j ) {
      if ( !is_bonus( &g_blocks[i][j] ) && !is_bomb( &g_blocks[i][j] ) )
        continue;
      
      if ( get_extra_info( i, j ) < 100 ) {
        if ( get_extra_info( i, j ) == get_block_offset( &g_blocks[i][j] ) ) {
          // block stopped
          if ( is_bomb( &g_blocks[i][j] ) ) 
            set_extra_info( i, j, 255 ); 
          else
            set_extra_info( i, j, 220 );
        } else {
          set_extra_info( i, j, get_block_offset( &g_blocks[i][j] ) );
        }
      } else if ( get_extra_info( i, j ) == 100 ) {
        if ( is_bomb( &g_blocks[i][j] ) ) {
          explode_bomb( i, j );
        } else {
          clear_block( &g_blocks[i][j] );
        }
      } else {
        set_extra_info( i, j, get_extra_info( i, j ) - 1 );
      }
    }
  }
}