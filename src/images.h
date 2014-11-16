#pragma once
  
#include "pebble.h"

static GBitmap *s_boxes_bitmap[11];
static GBitmap *s_person_images[7];
static GBitmap *s_crane_bitmap;
static GBitmap *s_wall_bitmap;
static GBitmap *s_ceiling_bitmap;
static GBitmap *s_ceiling_background_bitmap;
static GBitmap *s_floor_bitmap;
static GBitmap *s_level_bitmap;
static GBitmap *s_power_bitmap;
static GBitmap *s_jump_bitmap;
static GBitmap *s_score_bitmap;
static GBitmap *s_colon_bitmap;

static GBitmap *s_numbers[11];

typedef struct person_bitmaps_pair_
{
  GBitmap *w;
  GBitmap *b;
} person_bitmaps_pair;

static person_bitmaps_pair person_bitmaps[26];

inline static void init_game_images( int person )
{
  unsigned char RESOURCE_ID_PERSONS_WHITE[6] = { RESOURCE_ID_PETE_WHITE, RESOURCE_ID_LIZZIE_WHITE, RESOURCE_ID_FRANK_WHITE, RESOURCE_ID_WILL_WHITE, RESOURCE_ID_CATH_WHITE, RESOURCE_ID_SAM_WHITE };
  unsigned char RESOURCE_ID_PERSONS_BLACK[6] = { RESOURCE_ID_PETE_BLACK, RESOURCE_ID_LIZZIE_BLACK, RESOURCE_ID_FRANK_BLACK, RESOURCE_ID_WILL_BLACK, RESOURCE_ID_CATH_BLACK, RESOURCE_ID_SAM_BLACK };
  
  GBitmap *person_full_w = gbitmap_create_with_resource( RESOURCE_ID_PERSONS_WHITE[ person ] );
  GBitmap *person_full_b = gbitmap_create_with_resource( RESOURCE_ID_PERSONS_BLACK[ person ] );
  
  for ( int i=0; i<26; ++i ) {
    person_bitmaps[i].w = gbitmap_create_as_sub_bitmap( person_full_w, GRect( 0, i*13, 26, 13 ) );
    person_bitmaps[i].b = gbitmap_create_as_sub_bitmap( person_full_b, GRect( 0, i*13, 26, 13 ) ); 
  }
  
  gbitmap_destroy                                     ( person_full_w       );
  gbitmap_destroy                                     ( person_full_b       );

  s_crane_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_CRANE   );
  s_wall_bitmap         = gbitmap_create_with_resource( RESOURCE_ID_WALL    );
  s_ceiling_bitmap      = gbitmap_create_with_resource( RESOURCE_ID_CEILING );
  s_ceiling_background_bitmap
                        = gbitmap_create_with_resource( RESOURCE_ID_CEILING_BACKGROUND );
  s_floor_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_FLOOR   );
  s_level_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_LEVEL   );
  s_power_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_POWER   );
  s_jump_bitmap         = gbitmap_create_with_resource( RESOURCE_ID_JUMP    );
  s_score_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_SCORE   );
  s_colon_bitmap        = gbitmap_create_with_resource( RESOURCE_ID_COLON   );

  unsigned char RESOURCE_ID_BOXES[11] = { RESOURCE_ID_BOX1, RESOURCE_ID_BOX2, RESOURCE_ID_BOX3, RESOURCE_ID_BOX4, RESOURCE_ID_BOX5, 
                                          RESOURCE_ID_BOX6, RESOURCE_ID_BOX7, RESOURCE_ID_BOX8, RESOURCE_ID_BOX9, RESOURCE_ID_BOX10, 
                                          RESOURCE_ID_BOX11 };
  
  unsigned char RESOURCE_ID_NUMBERS[11] = { RESOURCE_ID_0, RESOURCE_ID_1, RESOURCE_ID_2, RESOURCE_ID_3, RESOURCE_ID_4, 
                                            RESOURCE_ID_5, RESOURCE_ID_6, RESOURCE_ID_7, RESOURCE_ID_8, RESOURCE_ID_9, 
                                            RESOURCE_ID_7P };
  
  for (int i=0; i<11; ++i) {
    s_numbers[i]      = gbitmap_create_with_resource( RESOURCE_ID_NUMBERS[i] );
    s_boxes_bitmap[i] = gbitmap_create_with_resource( RESOURCE_ID_BOXES[i] );
  }
}

inline static void deinit_game_images()
{
  for ( int i=0; i<11; ++i ) {
    gbitmap_destroy( s_boxes_bitmap[i] );
    gbitmap_destroy( s_numbers[i] );
  }
  
  gbitmap_destroy( s_crane_bitmap   );
  gbitmap_destroy( s_wall_bitmap    );
  gbitmap_destroy( s_ceiling_bitmap );
  gbitmap_destroy( s_ceiling_background_bitmap );
  gbitmap_destroy( s_floor_bitmap   );
  gbitmap_destroy( s_level_bitmap   );
  gbitmap_destroy( s_power_bitmap   );
  gbitmap_destroy( s_jump_bitmap    );
  gbitmap_destroy( s_score_bitmap   );
  gbitmap_destroy( s_colon_bitmap   );
  
  for ( int i=0; i<26; ++i ) {
    gbitmap_destroy( person_bitmaps[i].w );
    gbitmap_destroy( person_bitmaps[i].b );
  }
}

inline static void init_menu_images()
{
  unsigned char RESOURCE_ID_PERSONS[7] = { RESOURCE_ID_PERSON0, RESOURCE_ID_PERSON1, RESOURCE_ID_PERSON2, RESOURCE_ID_PERSON3, RESOURCE_ID_PERSON4, RESOURCE_ID_PERSON5, RESOURCE_ID_PERSON_LOCKED };
  
  for (int i=0; i<7; ++i) {
    s_person_images[i]  = gbitmap_create_with_resource( RESOURCE_ID_PERSONS[i] );
  }
}

inline static void deinit_menu_images()
{
  for ( int i=0; i<7; ++i ) {
    gbitmap_destroy( s_person_images[i] );
  }
}