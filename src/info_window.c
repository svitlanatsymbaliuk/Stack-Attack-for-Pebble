#include "pebble.h"

// This is a scroll layer
static ScrollLayer *scroll_layer;

// We also use a text layer to scroll in the scroll layer
static TextLayer *text_layer;

static const int vert_scroll_text_padding = 4;

// Setup the scroll layer on window load
// We do this here in order to be able to get the max used text size
static void info_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(2, 0, bounds.size.w-4, 2000);

  // Initialize the scroll layer
  scroll_layer = scroll_layer_create(bounds);

  // This binds the scroll layer to the window so that up and down map to scrolling
  // You may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(scroll_layer, window);

  // Initialize the text layer
  text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(text_layer, ( char * ) window_get_user_data(window)	);

  // Change the font to a nice readable one
  // This is system font; you can inspect pebble_fonts.h for all system fonts
  // or you can take a look at feature_custom_font to add your own font
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(text_layer);
  text_layer_set_size(text_layer, max_size);
  scroll_layer_set_content_size(scroll_layer, GSize(bounds.size.w, max_size.h + vert_scroll_text_padding ));

  // Add the layers for display
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(text_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
}

static void info_window_unload(Window *window)
{
  text_layer_destroy(text_layer);
  scroll_layer_destroy(scroll_layer);
  free( window_get_user_data(window) );
  window_destroy(window);
}

void show_info( const char * text )
{
  Window *info_window = window_create();
  window_set_window_handlers(info_window, (WindowHandlers) {
    .load = info_window_load,
    .unload = info_window_unload,
  });
  window_set_user_data( info_window, (void *) text );
  window_stack_push(info_window, true );
}