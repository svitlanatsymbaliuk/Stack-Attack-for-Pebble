#pragma once
  
#include "pebble.h"
  
static char *texts[20];
static char *menu_texts[5];
static unsigned char language = 0;

static void load_texts_into_array( char **output_texts, int output_texts_size, int resource_id )
{
  int length = resource_size( resource_get_handle(resource_id) );
  output_texts[0] = malloc( length );
  resource_load( resource_get_handle(resource_id), (unsigned char *)output_texts[0], length );
  int pos = 0;
  for ( int i=0; i<output_texts_size; ++i ) {
    output_texts[i] = output_texts[0] + pos;
    while ( output_texts[0][pos] != '\n' && pos < length )
      ++pos;
    
    if ( pos != length ) {
      output_texts[0][pos] = 0;
      ++pos;
    }
  }
}

inline static void load_menu_texts()
{
  load_texts_into_array( menu_texts, 5, RESOURCE_ID_MENU_EN );
}

inline static void unload_menu_texts()
{
  free( menu_texts[0] );
}

inline static void load_characters_texts()
{
  load_texts_into_array( texts, 13, RESOURCE_ID_CHARACTERS_EN );
}

inline static void load_cranes_textss()
{
  load_texts_into_array( texts, 10, RESOURCE_ID_CRANES_EN );
}

static void load_one_text_resource( int resource_id )
{
  int length = resource_size( resource_get_handle(resource_id) );
  texts[0] = malloc( length+1 );
  resource_load( resource_get_handle(resource_id), (unsigned char *)texts[0], length );
  texts[0][length] = 0;
}

inline static void load_instructions()
{
  load_one_text_resource( RESOURCE_ID_INSTRUCTIONS_EN );
}

inline static void load_about()
{
  load_one_text_resource( RESOURCE_ID_ABOUT_EN );
}

static void unload_texts()
{
  free( texts[0] );
}
