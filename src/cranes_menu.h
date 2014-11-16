#pragma once
  
#include "cranes.h"
  
static MenuLayer *cranes_menu_layer;

static uint16_t cranes_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
  return 5;
}

static void cranes_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
  window_stack_pop( false );
  start_game( current_character, cell_index->row + 1 );
}

static void cranes_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
  int const index     = cell_index->row;
  menu_cell_basic_draw( ctx, cell_layer, texts[ index * 2 ], texts[ index * 2 + 1 ], NULL );
}

static void cranes_menu_window_unload( Window *window )
{
  menu_layer_destroy ( cranes_menu_layer );
  window_destroy     ( window );
  unload_texts       ( );
}

inline static void show_cranes_menu( int character )
{
  load_cranes_textss( );
  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .unload = cranes_menu_window_unload,
  });
  
  current_character = character;
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  cranes_menu_layer = menu_layer_create(bounds);

  menu_layer_set_callbacks(cranes_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = cranes_menu_get_num_rows_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = cranes_menu_draw_row_callback,
    .select_click = cranes_menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(cranes_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(cranes_menu_layer));
  
  window_stack_push( window, true );
}
