#pragma once
  
#include "pebble.h"
  
void log_free_ram( int p )
{
  char a[32];
  snprintf( a,sizeof(a),"p%i. free ram (b): %i", p, heap_bytes_free() );
  APP_LOG( APP_LOG_LEVEL_DEBUG, a );
}