/*
 * Copyright © 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * Author: Cody Russell <bratsche@gnome.org>
 */

#if !defined (__GIO_GIO_H_INSIDE__) && !defined (GIO_COMPILATION)
#error "Only <gio/gio.h> can be included directly."
#endif

#ifndef __G_TIMELINE_H__
#define __G_TIMELINE_H__

#include <glib-object.h>
#include "gioenums.h"

G_BEGIN_DECLS

#define G_TYPE_TIMELINE         (g_timeline_get_type ())
#define G_TIMELINE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_TIMELINE, GTimeline))
#define G_TIMELINE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), G_TYPE_TIMELINE, GTimelineClass))
#define G_IS_TIMELINE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_TIMELINE))
#define G_IS_TIMELINE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), G_TYPE_TIMELINE))
#define G_TIMELINE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), G_TYPE_TIMELINE, GTimelineClass))

typedef struct _GTimeline        GTimeline;
typedef struct _GTimelinePrivate GTimelinePrivate;
typedef struct _GTimelineClass   GTimelineClass;

struct _GTimeline
{
  GObject parent_instance;

  GTimelinePrivate *priv;
};

struct _GTimelineClass
{
  GObjectClass parent_class;
};

GType        g_timeline_get_type      (void) G_GNUC_CONST;

GTimeline   *g_timeline_new           (GPeriodic  *periodic,
				       guint       length);

guint        g_timeline_get_length    (GTimeline  *timeline);
void         g_timeline_set_length    (GTimeline  *timeline,
				       guint       length);
GDirection   g_timeline_get_direction (GTimeline  *timeline);
void         g_timeline_set_direction (GTimeline  *timeline,
				       GDirection  direction);
void         g_timeline_set_repeat    (GTimeline  *timeline,
				       gboolean    repeat);
gboolean     g_timeline_get_repeat    (GTimeline  *timeline);

void         g_timeline_start         (GTimeline  *timeline);
void         g_timeline_stop          (GTimeline  *timeline);
void         g_timeline_reset         (GTimeline  *timeline);

gboolean     g_timeline_is_running    (GTimeline  *timeline);

G_END_DECLS

#endif /* __G_TIMELINE_H__ */
