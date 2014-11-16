#pragma once

// text might be NULL if user canceled entering
typedef void(* text_entered_callback)(const char *text);

void tertiary_enter_text( text_entered_callback callback );