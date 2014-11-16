#pragma once
  
#include "stored_settings.h"
#include "info_window.h"

static inline void init_highscores() 
{
  if ( persist_exists( HIGHSCORES_KEY ) ) {
    persist_read_data( HIGHSCORES_KEY, stored_highscores, sizeof( stored_highscores ) );
  } else {
    for ( int i=0; i<3; ++i ) {
      stored_highscores[i].score = 0;
      strcpy( stored_highscores[i].name, "" );
    }
  }
}

static void write_highscores() 
{
  persist_write_data( HIGHSCORES_KEY, stored_highscores, sizeof( stored_highscores ) );
}

static void show_highscores()
{
  init_highscores();
  char *highscores_string = (char *) malloc( 128 );

  strcpy( highscores_string, stored_highscores[0].score == 0 ? "No highscores\n " : "" );
  for ( int i=0; i<3; ++i ) {
    if ( stored_highscores[i].score == 0 )
      break;
    
    snprintf       ( highscores_string, 128,
                     "%s%i. %s\n   %i\n", highscores_string, i+1, stored_highscores[i].name, stored_highscores[i].score );
  }
  
  show_info( highscores_string );
}