#pragma once
  
#include "pebble.h"
#include "texts.h"
  
static MenuLayer *menu_layer;

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
  return 5;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data)
{
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
  menu_cell_basic_draw( ctx, cell_layer, menu_texts[cell_index->row], NULL, NULL );
}

static void menu_window_unload( Window *window )
{
  menu_layer_destroy(menu_layer);
  unload_menu_texts();
}

// This initializes the menu upon window load
inline static void init_menu( Window *window, MenuLayerSelectCallback select_callback )
{
  load_menu_texts();
  
  window_set_window_handlers(window, (WindowHandlers) {
    .unload = menu_window_unload,
  });
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = menu_get_num_rows_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}
