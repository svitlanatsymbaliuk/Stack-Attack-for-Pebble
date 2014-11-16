#pragma once
  
#include "person_extra.h"
#include "cranes_menu.h"
  
static MenuLayer *character_menu_layer;

static uint16_t character_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
  return 6;
}

static void character_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
  if ( cell_index->row > unlocked_characters ) {
    int           text_size = strlen( texts[12] ) + 1;
    char *text    = (char *)malloc( text_size );
    memcpy        ( text, texts[12], text_size );
    
    show_info     ( text );
    return;
  }
  
  window_stack_pop( false );
  show_cranes_menu( cell_index->row );
}

static void character_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
  int const index     = cell_index->row;
  menu_cell_basic_draw( ctx, cell_layer, texts[ index * 2 ], texts[ index * 2 + 1 ], s_person_images[  index <= unlocked_characters ? index : 6 ] );
}

static void character_menu_window_unload( Window *window )
{
  menu_layer_destroy ( character_menu_layer );
  window_destroy     ( window );
  deinit_menu_images ( );
  unload_texts       ( );
}

inline static void show_character_menu( )
{
  unlocked_characters = persist_read_int( UNLOCKED_CHARACTERS );
  init_menu_images();
  load_characters_texts();
  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .unload = character_menu_window_unload,
  });
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  character_menu_layer = menu_layer_create(bounds);

  menu_layer_set_callbacks(character_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = character_menu_get_num_rows_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = character_menu_draw_row_callback,
    .select_click = character_menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(character_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(character_menu_layer));
  
  window_stack_push( window, true );
}
