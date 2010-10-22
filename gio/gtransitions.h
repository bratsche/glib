#if !defined (__GIO_GIO_H_INSIDE__) && !defined (GIO_COMPILATION)
#error "Only <gio/gio.h> can be included directly."
#endif

#ifndef __G_TRANSITIONS_H__
#define __G_TRANSITIONS_H__

#include <glib.h>

G_BEGIN_DECLS

gdouble g_transition_sinusoidal (gdouble position);
gdouble g_transition_mirror     (gdouble position);

G_END_DECLS

#endif /* __G_TRANSITIONS_H__ */
