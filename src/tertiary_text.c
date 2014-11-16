#include <pebble.h>
#include "tertiary_text.h"

static text_entered_callback s_callback;

typedef struct Layers_{
TextLayer* header_layer;
TextLayer* text_layer;
TextLayer* hint_layer;
TextLayer* bbuttons[3][3];
} Layers;
static Layers layers;

static bool s_is_curson_shown;
static AppTimer *s_timer;

static char btexts[3][3][4]; // warning! assume it would be filled with zeroes
static char setlist[3][4] = { "  a\0","  b\0","  c\0" };

static void draw_sides();

static const char rotate_text[3][27] = { "ABCDEFGHIJKLM NOPQRSTUVWXYZ" , "abcdefghijklm nopqrstuvwxyz", "1234567890!?-'\"$()&*+#:@/,."};
static const char* master;

static char text_buffer[20];
static unsigned char pos = 0;
static unsigned char top = 0, end = 26, size = 27;

static void draw_notepad_text( )
{
  text_layer_set_text(layers.text_layer, text_buffer);
}

static void change_set( int s )
{
  int count = 0;
  master = rotate_text[s];
  for (int i=0; i<9; ++i)
  {
      for (int k=0; k<3; ++k)
      {
        btexts[i/3][i%3][k] = master[count];
        count++;
      }
  }

  draw_sides();
}

static inline void next( )
{
  top = 0;
  end = 26;
  size = 27;
}

static void click_button( int b )
{
  if (size > 3)
  {
    size /= 3;
    top += b*size;
    end -= (2-b)*size;
  }
  else
  {
    text_buffer[pos++] = master[top+b];
    draw_notepad_text();
    change_set(1);
    next();
  }

  draw_sides();
}

static void on_timeout( void *data )
{
  s_timer = app_timer_register	(	500, on_timeout, data );
  s_is_curson_shown = !s_is_curson_shown;
  text_buffer[pos] = s_is_curson_shown ? '_' : ' ';
  draw_notepad_text();
}

static void up_single_click_handler( ClickRecognizerRef recognizer, void* context )
{
  click_button(0);
}

static void select_single_click_handler( ClickRecognizerRef recognizer, void* context ) 
{
  click_button(1);
}

static void down_single_click_handler( ClickRecognizerRef recognizer, void* context )
{
  click_button(2);
}

static void up_long_click_handler( ClickRecognizerRef recognizer, void* context )
{
  change_set(0);
}

static void select_long_click_handler( ClickRecognizerRef recognizer, void* context )
{
  text_buffer[pos] = 0;
  s_callback( text_buffer );
}

static void back_click_handler( ClickRecognizerRef recognizer, void* context )
{
  // delete or cancel when back is pressed
  if ( size == 27 ) {
    if ( pos>0 ) {
      text_buffer[pos] = 0;
      text_buffer[--pos] = 0;
      draw_notepad_text();
    }
    else {
      //window_stack_pop(true);
    }
  }
  else {
    next();
    draw_sides();
  }
}

static void down_long_click_handler( ClickRecognizerRef recognizer, void* context )
{
  change_set(2);
}

static void click_config_provider( void* context )
{
  window_single_click_subscribe  ( BUTTON_ID_BACK, back_click_handler );

  window_single_click_subscribe  ( BUTTON_ID_UP, up_single_click_handler );
  window_long_click_subscribe    ( BUTTON_ID_UP, 1000, up_long_click_handler, NULL );

  window_single_click_subscribe  ( BUTTON_ID_SELECT, select_single_click_handler );
  window_long_click_subscribe    ( BUTTON_ID_SELECT, 1000, select_long_click_handler, NULL );

  window_single_click_subscribe  ( BUTTON_ID_DOWN, down_single_click_handler );
  window_long_click_subscribe    ( BUTTON_ID_DOWN, 1000, down_long_click_handler, NULL );
}


// This method draws the characters on the right side near the buttons
static void draw_sides( )
{
  if ( size==27 ) {// first click (full size)
    // update all 9 labels to their proper values
    for (int h=0; h<9; h++) {
      text_layer_set_text(layers.bbuttons[h/3][h%3], btexts[h/3][h%3]);
      text_layer_set_background_color(layers.bbuttons[h/3][h%3], GColorClear);
      text_layer_set_font(layers.bbuttons[h/3][h%3], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    }
  }
  else {
    for (int i=0; i<3; i++) {
      if (size==9) {  // second click
        text_layer_set_text(layers.bbuttons[i][i!=2], " ");
        text_layer_set_text(layers.bbuttons[i][2], " ");
  
        text_layer_set_text(layers.bbuttons[i][i==2], btexts[top/9][i]);
        text_layer_set_font(layers.bbuttons[i][i==2], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
      }
      else if (size == 3) {
        setlist[i][2] = master[top+i];
        text_layer_set_text(layers.bbuttons[i][i==2], setlist[i]);
      }
    }
  }
}

static void window_unload( Window *window )
{
  for (int i = 0; i<12; i++) {
    // hack to save 32 bytes
    text_layer_destroy( (&layers.header_layer)[i] );
  }

  app_timer_cancel(	s_timer );
  window_destroy( window );
}

static void window_load( Window* window )
{
  Layer *window_layer = window_get_root_layer(window);

  layers.header_layer = text_layer_create((GRect) { .origin = { 5, -4 }, .size = { 124, 20 } });
  text_layer_set_font(layers.header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text(layers.header_layer, "Enter your name:");
  layer_add_child(window_layer, text_layer_get_layer(layers.header_layer));

  layers.text_layer = text_layer_create((GRect) { .origin = { 5, 17 }, .size = { 100, 55 } });
  text_layer_set_font(layers.text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(layers.text_layer));

  layers.hint_layer = text_layer_create((GRect) { .origin = { 5, 72 }, .size = { 100, 90 } });
  text_layer_set_font(layers.hint_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text(layers.hint_layer, "Hold top button for CAPS, down button for symbols, middle button to confirm");
  layer_add_child(window_layer, text_layer_get_layer(layers.hint_layer));
  
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      layers.bbuttons[j][i] = text_layer_create((GRect) { .origin = { 112, 10+12*i+50*j }, .size = { 100, 100 } });
      layer_add_child(window_layer, text_layer_get_layer(layers.bbuttons[j][i]));
    }
  }
  
  draw_sides();
  
  on_timeout( NULL );
}

static inline void tertiary_init( )
{
  Window* window = window_create();
  window_set_fullscreen(window, true);

  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push( window, true );
}

void tertiary_enter_text( text_entered_callback callback )
{
  s_callback = callback;
  tertiary_init();
  change_set(0);
  draw_notepad_text();
}