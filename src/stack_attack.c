#include "images.h"
#include "blocks.h"
#include "cranes.h"
#include "person.h"
#include "menu.h"
#include "options_menu.h"
#include "info_window.h"
#include "texts.h"
  
#define FPS 60

static BitmapLayer *s_bottom_layer;
static BitmapLayer *s_background_layer;
static BitmapLayer *s_blocks_layer;
static BitmapLayer *s_cranes_layer;
static BitmapLayer *s_person_layer;
static BitmapLayer *s_texts_layer;

static AppTimer *timer;

void draw_background( Layer *layer, GContext *ctx )
{
  graphics_context_set_fill_color( ctx, GColorWhite );
  graphics_fill_rect( ctx, GRect( 0, 0, 117, 168 ), 0, GCornerNone );
  
  graphics_draw_bitmap_in_rect( ctx, s_wall_bitmap,    GRect( 0,    0, 120,   4 ) );
  graphics_draw_bitmap_in_rect( ctx, s_floor_bitmap,   GRect( 13,   4,   4, 164 ) );
  graphics_draw_bitmap_in_rect( ctx, s_ceiling_bitmap, GRect( 117,  4,   4, 164 ) );
  graphics_draw_bitmap_in_rect( ctx, s_ceiling_background_bitmap, 
                                                       GRect( 98,   4,  19, 164 ) );
  graphics_draw_bitmap_in_rect( ctx, s_level_bitmap,   GRect( 3,   12,   7,   9 ) );
  
  if ( power )
    graphics_draw_bitmap_in_rect( ctx, s_power_bitmap, GRect( 3,   74,  7, 7   ) );
  
  if ( jumps ) 
    graphics_draw_bitmap_in_rect( ctx, s_jump_bitmap,  GRect( 3,   43,  7, 11  ) );
  
  graphics_context_set_fill_color( ctx, GColorBlack );
  graphics_fill_rect( ctx, GRect( 123, 0,  13, 168 ), 0, GCornerNone );
  
  graphics_draw_bitmap_in_rect( ctx, s_score_bitmap,   GRect( 126,  12,   7, 39  ) );
  graphics_draw_bitmap_in_rect( ctx, s_colon_bitmap,   GRect( 127, 143,   5, 2   ) );
}

void draw_texts( Layer *layer, GContext *ctx )
{
  if ( lives > 7 )
    graphics_draw_bitmap_in_rect( ctx, s_numbers[ 10 ], GRect( 0, 18, 7, 12 ) );
  else
    graphics_draw_bitmap_in_rect( ctx, s_numbers[ lives ], GRect( 0, 18, 7, 7 ) );
  
  if ( jumps ) {
    graphics_draw_bitmap_in_rect( ctx, s_numbers[ jumps ], GRect( 0, 51, 7, 7 ) );
  } else {
    graphics_context_set_fill_color( ctx, GColorWhite  );
    graphics_fill_rect(ctx, GRect( 0, 35, 7, 15 ), 0, GCornerNone );
  }
  
  if ( !power ) {
    graphics_context_set_fill_color( ctx, GColorWhite  );
    graphics_fill_rect(ctx, GRect( 0, 70, 7, 7 ), 0, GCornerNone );
  }
  
  graphics_context_set_compositing_mode( ctx, GCompOpAssignInverted );
  
  if ( last_shown_score < score ) 
    last_shown_score++;
  
  int digits_count = 0;
  unsigned char digits[10];
  for ( unsigned int score_copy = last_shown_score; score_copy != 0; ++digits_count ) {
    digits[digits_count] = score_copy % 10;
    score_copy /= 10;
  }
  
  if ( digits_count == 0 ) {
    graphics_draw_bitmap_in_rect( ctx, s_numbers[ 0 ], GRect( 123, 51, 7, 7 ) );
  } else {
    --digits_count;
    for ( int i=0; digits_count >= 0; --digits_count, ++i ) {
      graphics_draw_bitmap_in_rect( ctx, s_numbers[ digits[digits_count] ], GRect( 123, 51 + i*8, 7, 7 ) );
    }
  }
  
  time_t temp = time(NULL); 
  struct tm tick_time = *localtime(&temp);
  if ( tick_time.tm_hour / 10 )
    graphics_draw_bitmap_in_rect( ctx, s_numbers[ tick_time.tm_hour / 10 ], GRect( 123, 123, 7, 7 ) );
  graphics_draw_bitmap_in_rect( ctx, s_numbers[ tick_time.tm_hour % 10 ], GRect( 123, 131, 7, 7 ) );
  
  graphics_draw_bitmap_in_rect( ctx, s_numbers[ tick_time.tm_min / 10 ], GRect( 123, 142, 7, 7 ) );
  graphics_draw_bitmap_in_rect( ctx, s_numbers[ tick_time.tm_min % 10 ], GRect( 123, 150, 7, 7 ) );

  graphics_context_set_compositing_mode( ctx, GCompOpClear );
}

inline static void tick()
{
  tick_blocks();
  tick_cranes();
  tick_person();
  /*static int tick_n = 0;
  ++tick_n;
  if ( tick_n%10 == 1 )
    log_free_ram( 0 );*/
}

void on_timeout(void *data)
{
  timer = app_timer_register	(	1000 / FPS, on_timeout, NULL );
  
  tick();
  layer_mark_dirty( bitmap_layer_get_layer(s_blocks_layer) );
}


static void click_config_provider(Window *window)
{
  window_raw_click_subscribe( BUTTON_ID_UP,     left_button_pressed,   left_button_released,   NULL );
  window_raw_click_subscribe( BUTTON_ID_DOWN,   right_button_pressed,  right_button_released,  NULL );
  window_raw_click_subscribe( BUTTON_ID_SELECT, middle_button_pressed, middle_button_released, NULL );
}

static void game_window_load(Window *me)
{  
  Layer *layer = window_get_root_layer(me);
  
  layer_set_frame(layer, GRect(0, 0, 144, 167)); // Why 168 doesn't work?!
  
  s_bottom_layer = bitmap_layer_create( GRect(0, 0, 144, 168) );
  bitmap_layer_set_background_color	(	s_bottom_layer, GColorBlack );
  layer_add_child( layer, bitmap_layer_get_layer( s_bottom_layer ) );

  s_background_layer = bitmap_layer_create( GRect(4, 0, 136, 168) );
  layer_add_child(layer, bitmap_layer_get_layer(s_background_layer));
    
  s_blocks_layer = bitmap_layer_create( GRect( 21, LEFT_OFFSET, BLOCK_SIZE * FIELD_HEIGHT, BLOCK_SIZE * FIELD_WIDTH ) );
  layer_add_child(layer, bitmap_layer_get_layer(s_blocks_layer));
  
  s_cranes_layer = bitmap_layer_create( GRect( 127-CRANE_WITH_BLOCK_HEIGHT, - CRANE_WIDTH, CRANE_WITH_BLOCK_HEIGHT, 168 + CRANE_WIDTH*2 ) );
  layer_add_child(layer, bitmap_layer_get_layer(s_cranes_layer));
  
  s_texts_layer = bitmap_layer_create( GRect( 7, 4, 130, 164 ) );
  layer_add_child(layer, bitmap_layer_get_layer(s_texts_layer));
  
  s_person_layer = bitmap_layer_create( GRect( 21, LEFT_OFFSET, BLOCK_SIZE * ( FIELD_HEIGHT + 2 ), BLOCK_SIZE * FIELD_WIDTH ) );
  layer_add_child(layer, bitmap_layer_get_layer(s_person_layer));
  
  layer_set_update_proc( bitmap_layer_get_layer(s_background_layer), draw_background );
  layer_set_update_proc( bitmap_layer_get_layer(s_blocks_layer),     draw_blocks     );
  layer_set_update_proc( bitmap_layer_get_layer(s_cranes_layer),     draw_cranes     );
  layer_set_update_proc( bitmap_layer_get_layer(s_texts_layer ),     draw_texts      );
  layer_set_update_proc( bitmap_layer_get_layer(s_person_layer),     draw_person     );
  
  if ( light_state )
    light_enable(true);
}

static void game_window_unload(Window *window)
{
  app_timer_cancel(	timer );

  bitmap_layer_destroy( s_bottom_layer );
  bitmap_layer_destroy( s_background_layer );
  bitmap_layer_destroy( s_blocks_layer );
  bitmap_layer_destroy( s_cranes_layer );
  bitmap_layer_destroy( s_texts_layer );
  bitmap_layer_destroy( s_person_layer );
  
  if ( light_state )
    light_enable( false );
  
  window_destroy( window );
  
  log_free_ram( 6 );
  deinit_game_images();
  log_free_ram( 7 );
}

static void game_window_appear()
{
  timer = app_timer_register	(	1000 / FPS, on_timeout, NULL );
}

static void game_window_disappear()
{
  app_timer_cancel(	timer );
}

static void start_game( int person, int cranes_count )
{
  log_free_ram( 1 );
  init_game_images( person );
  log_free_ram( 2 );
    
  init_person();
  init_cranes( cranes_count );
  init_blocks();
  
  Window *game_window = window_create();
  window_set_fullscreen(game_window, true);
  window_set_window_handlers( game_window, (WindowHandlers) {
    .load      = game_window_load,
    .appear    = game_window_appear,
    .disappear = game_window_disappear,
    .unload    = game_window_unload,
  } );
  
  window_set_click_config_provider( game_window, (ClickConfigProvider) click_config_provider );

  window_stack_push( game_window, true );
}

#include "character_menu.h"
#include "highscores_menu.h"

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
  switch ( cell_index->row ) {
    case 0:
      show_character_menu();
    break;
    case 1: {
      show_highscores();
    }
    break;
    case 2:
      load_instructions();
      show_info( texts[0] );
    break;
    case 3:
      load_about();
      show_info( texts[0] );
    break;
    case 4:
      show_options_menu( );
    break;
  }
}

int main(void)
{
  init_options();
  log_free_ram( 2 );
  Window *window = window_create();
  log_free_ram( 3 );
  init_menu( window, menu_select_callback );
  log_free_ram( 4 );
  window_stack_push( window, true );
  log_free_ram( 5 );
  app_event_loop();
  log_free_ram( 8 );
  window_destroy(window);
  log_free_ram( 9 );
}
