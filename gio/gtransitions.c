#include <math.h>

#include "gtransitions.h"

gdouble
g_transition_sinusoidal (gdouble position)
{
  return sinf ((position * G_PI) / 2);
}

gdouble
g_transition_mirror (gdouble position)
{
  if (position < 0.5)
    return position * 2;
  else
    return 1 - (position - 0.5) * 2;
}
