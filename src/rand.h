#pragma once

int _rand(void) /* RAND_MAX assumed to be 32767. */
{
  static unsigned long next = 1;
  next = next * 1103515245 + 12345;
  return next >> 16;
}