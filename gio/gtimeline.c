#include "config.h"
#include "gperiodic.h"
#include "gtimeline.h"
#include "glibintl.h"

struct _GTimelinePrivate
{
  GPeriodic    *periodic;
  guint         id;
  guint         length;
  guint64       start_time;
  guint64       last_tick;

  gboolean      repeat;
  GDirection    direction;
  gfloat        progress;
};

enum {
  PROP_0,
  PROP_PERIODIC,
  PROP_LENGTH
};

enum {
  START,
  FINISH,
  FRAME,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

#define G_TIMELINE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_TIMELINE, GTimelinePrivate))

G_DEFINE_TYPE (GTimeline, g_timeline, G_TYPE_OBJECT)

static void
g_timeline_dispose (GObject *object)
{
  GTimelinePrivate *priv = G_TIMELINE (object)->priv;

  if (priv->periodic != NULL)
    {
      g_object_unref (priv->periodic);
      priv->periodic = NULL;
    }
}

static void
g_timeline_set_property (GObject      *object,
			 guint         prop_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
  GTimeline *timeline = G_TIMELINE (object);

  switch (prop_id)
    {
    case PROP_PERIODIC:
      timeline->priv->periodic = g_value_get_object (value);
      break;

    case PROP_LENGTH:
      g_timeline_set_length (timeline, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
g_timeline_get_property (GObject    *object,
			 guint       prop_id,
			 GValue     *value,
			 GParamSpec *pspec)
{
  GTimelinePrivate *priv = G_TIMELINE (object)->priv;

  switch (prop_id)
    {
    case PROP_PERIODIC:
      g_value_set_object (value, priv->periodic);
      break;

    case PROP_LENGTH:
      g_value_set_uint (value, priv->length);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
g_timeline_class_init (GTimelineClass *c)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (c);

  gobject_class->get_property = g_timeline_get_property;
  gobject_class->set_property = g_timeline_set_property;
  gobject_class->dispose      = g_timeline_dispose;

  g_object_class_install_property (gobject_class,
				   PROP_PERIODIC,
				   g_param_spec_object ("periodic",
							P_("Periodic"),
							P_("Periodic"),
							G_TYPE_PERIODIC,
							G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_LENGTH,
				   g_param_spec_uint ("length",
						      "Length",
						      "Length",
						      0, G_MAXUINT,
						      0,
						      G_PARAM_READWRITE));

  signals[FRAME] = g_signal_new ("frame",
				 G_TYPE_FROM_CLASS (gobject_class),
				 G_SIGNAL_RUN_LAST,
				 0,
				 NULL, NULL,
				 g_cclosure_marshal_VOID__DOUBLE,
				 G_TYPE_NONE, 1,
				 G_TYPE_DOUBLE);

  signals[START] = g_signal_new ("start",
				 G_TYPE_FROM_CLASS (gobject_class),
				 G_SIGNAL_RUN_LAST,
				 0,
				 NULL, NULL,
				 g_cclosure_marshal_VOID__VOID,
				 G_TYPE_NONE, 0);

  signals[FINISH] = g_signal_new ("finish",
				  G_TYPE_FROM_CLASS (gobject_class),
				  G_SIGNAL_RUN_LAST,
				  0,
				  NULL, NULL,
				  g_cclosure_marshal_VOID__VOID,
				  G_TYPE_NONE, 0);

  g_type_class_add_private (c, sizeof (GTimelinePrivate));
}

static void
g_timeline_init (GTimeline *timeline)
{
  GTimelinePrivate *priv = G_TIMELINE_GET_PRIVATE (timeline);

  timeline->priv = priv;

  priv->length  = 0.0;
}

static void
reset_time (GTimeline *timeline)
{
  GTimelinePrivate *priv = timeline->priv;
  GDateTime *dt;
  GTimeVal timeval;

  dt = g_date_time_new_now_local ();
  g_date_time_to_timeval (dt, &timeval);

  priv->start_time = (timeval.tv_sec * 1000000 + timeval.tv_usec);
  priv->last_tick = priv->start_time;
}

static void
g_timeline_tick (GPeriodic *periodic,
		 guint64    timestamp,
		 gpointer   user_data)
{
  GTimeline *timeline = G_TIMELINE (user_data);
  GTimelinePrivate *priv = timeline->priv;
  gdouble goal;
  gdouble delta;
  guint64 elapsed;
  gdouble progress;

  elapsed = (timestamp - priv->last_tick);
  delta = ((gdouble)elapsed / (priv->length * 1000));

  priv->progress = CLAMP (priv->progress + delta, 0., 1.);

  if (priv->direction == G_DIRECTION_REVERSE)
    progress = 1.0 - priv->progress;
  else
    progress = priv->progress;

  g_signal_emit (timeline, signals[FRAME], 0, progress);

  if (priv->progress == 1.0)
    {
      if (priv->repeat)
	{
	  g_timeline_reset (timeline);
	}
      else
	{
	  g_timeline_stop (timeline);
	  g_signal_emit (timeline, signals[FINISH], 0);
          priv->progress = 0.;
	}
    }

  priv->last_tick = timestamp;
}

void
g_timeline_start (GTimeline *timeline)
{
  GTimelinePrivate *priv;

  g_return_if_fail (G_IS_TIMELINE (timeline));
  g_return_if_fail (timeline->priv->id == 0);

  priv = timeline->priv;

  reset_time (timeline);

  priv->id = g_periodic_add (priv->periodic,
			     g_timeline_tick,
			     timeline,
			     NULL);
}

void
g_timeline_stop (GTimeline *timeline)
{
  GTimelinePrivate *priv;

  g_return_if_fail (G_IS_TIMELINE (timeline));
  g_return_if_fail (timeline->priv->id != 0);

  priv = timeline->priv;

  g_periodic_remove (priv->periodic,
		     priv->id);
  priv->id = 0;
}

void
g_timeline_reset (GTimeline *timeline)
{
  g_return_if_fail (G_IS_TIMELINE (timeline));

  reset_time (timeline);

  timeline->priv->progress = 0.0;
}

GTimeline *
g_timeline_new (GPeriodic *periodic,
		guint      length)
{
  GTimeline *timeline = g_object_new (G_TYPE_TIMELINE,
				      "periodic", periodic,
				      "length",   length,
				      NULL);

  g_timeline_reset (timeline);

  return timeline;
}

gboolean
g_timeline_is_running (GTimeline *timeline)
{
  g_return_val_if_fail (G_IS_TIMELINE (timeline), FALSE);

  return timeline->priv->id != 0;
}

void
g_timeline_set_length (GTimeline *timeline,
		       guint        length)
{
  GTimelinePrivate *priv;

  g_return_if_fail (G_IS_TIMELINE (timeline));

  priv = timeline->priv;

  if (length != priv->length)
    {
      priv->length = length;
      g_object_notify (G_OBJECT (timeline), "length");
    }
}

guint
g_timeline_get_length (GTimeline *timeline)
{
  g_return_val_if_fail (G_IS_TIMELINE (timeline), 0);

  return timeline->priv->length;
}

void
g_timeline_set_direction (GTimeline  *timeline,
			  GDirection  direction)
{
  g_return_if_fail (G_IS_TIMELINE (timeline));
  g_return_if_fail (timeline->priv->id == 0);

  if (timeline->priv->direction != direction)
    {
      timeline->priv->direction = direction;

      timeline->priv->progress = 1.0 - timeline->priv->progress;
    }
}

GDirection
g_timeline_get_direction (GTimeline *timeline)
{
  g_return_val_if_fail (G_IS_TIMELINE (timeline), G_DIRECTION_FORWARD);

  return timeline->priv->direction;
}

void
g_timeline_set_repeat (GTimeline *timeline,
		       gboolean     repeat)
{
  g_return_if_fail (G_IS_TIMELINE (timeline));

  timeline->priv->repeat = repeat;
}

gboolean
g_timeline_get_repeat (GTimeline *timeline)
{
  g_return_val_if_fail (G_IS_TIMELINE (timeline), FALSE);

  return timeline->priv->repeat;
}
