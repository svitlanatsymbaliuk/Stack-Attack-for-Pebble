#pragma once
  
#include "stored_settings.h"
  
static MenuLayer *options_menu_layer;

inline static void init_options() 
{
  vibration_state = persist_read_bool( VIBRATION_KEY );
  light_state     = persist_read_bool( LIGHT_KEY );
}

static uint16_t options_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
  return 2;
}

static void options_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
  switch ( cell_index->row ) {
    case 0:
      vibration_state    = !vibration_state;
      persist_write_bool ( VIBRATION_KEY, vibration_state );
    break;
    case 1:
      light_state        = !light_state;
      persist_write_bool ( LIGHT_KEY, light_state );
    break;
  }
  layer_mark_dirty( menu_layer_get_layer(options_menu_layer) );
}

static void options_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
  switch ( cell_index->row ) {
    case 0: menu_cell_basic_draw( ctx, cell_layer, "Vibration", vibration_state ? "On" : "Off",    NULL ); break;
    case 1: menu_cell_basic_draw( ctx, cell_layer, "Light",     light_state ? "On" : "Automatic",  NULL ); break;
  }
}

static void options_menu_window_unload( Window *window )
{
  menu_layer_destroy ( options_menu_layer );
  window_destroy     ( window );
}

inline static void show_options_menu( )
{
  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .unload = options_menu_window_unload,
  });
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  options_menu_layer = menu_layer_create(bounds);

  menu_layer_set_callbacks(options_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = options_menu_get_num_rows_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = options_menu_draw_row_callback,
    .select_click = options_menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(options_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(options_menu_layer));
  
  window_stack_push( window, true );
}
