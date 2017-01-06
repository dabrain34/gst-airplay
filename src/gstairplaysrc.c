/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2017 scerveau <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-airplay
 *
 * FIXME:Describe airplay here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m airplaysrc ! airplay ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstairplaysrc.h"

GST_DEBUG_CATEGORY_STATIC (gst_airplay_src_debug);
#define GST_CAT_DEFAULT gst_airplay_src_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_airplay_parent_class parent_class
G_DEFINE_TYPE (GstAirplaySrc, gst_airplay_src, GST_TYPE_ELEMENT);

static void gst_airplay_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_airplay_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

/* GObject vmethod implementations */

static void
gst_airplay_src_loop (gpointer user_data)
{
  GstAirplaySrc *thiz;

  thiz = GST_AIRPLAY_SRC (user_data);

  gst_task_stop (thiz->task);
}

static void
gst_airplay_src_task_cleanup (GstAirplaySrc * thiz)
{

  /* given that the pads are removed on the parent class at the paused
   * to ready state, we need to exit the task and wait for it
   */
  if (thiz->task) {
    gst_task_stop (thiz->task);
    gst_task_join (thiz->task);
    gst_object_unref (thiz->task);
    thiz->task = NULL;
  }
}

static void
gst_airplay_src_task_setup (GstAirplaySrc * thiz)
{
  /* to pop from the libtorrent async system */
#if HAVE_GST_1
  thiz->task = gst_task_new (gst_airplay_src_loop, thiz, NULL);
#else
  thiz->task = gst_task_create (gst_airplay_src_loop, thiz);
#endif
  gst_task_set_lock (thiz->task, &thiz->task_lock);
  gst_task_start (thiz->task);
}

static gboolean
gst_airplay_src_setup (GstAirplaySrc * thiz)
{

  gst_airplay_src_task_setup (thiz);

  return TRUE;
}

static void
gst_airplay_src_cleanup (GstAirplaySrc * thiz)
{
  gst_airplay_src_task_cleanup (thiz);
}

static GstStateChangeReturn
gst_airplay_src_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret;
  GstAirplaySrc * thiz = GST_AIRPLAY_SRC (element);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_airplay_src_setup (thiz);
      break;

    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (gst_airplay_src_parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_airplay_src_cleanup (thiz);
      break;

    case GST_STATE_CHANGE_READY_TO_NULL:
      break;

    default:
      break;
  }

  return ret;
}

/* initialize the airplay's class */
static void
gst_airplay_src_class_init (GstAirplaySrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *element_class;

  gobject_class = (GObjectClass *) klass;
  element_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_airplay_set_property;
  gobject_class->get_property = gst_airplay_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(element_class,
    "airplay",
    "airplay/source",
    "Receives airplay data",
    "scerveau <<scerveau@gmail.com>>");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));

  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_airplay_src_change_state);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_airplay_src_init (GstAirplaySrc * filter)
{

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_airplay_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
	GstAirplaySrc *filter = GST_AIRPLAY_SRC (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_airplay_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstAirplaySrc *filter = GST_AIRPLAY_SRC (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
airplay_src_init (GstPlugin * airplaysrc)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template airplay' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_airplay_src_debug, "airplaysrc",
      0, "airplaysrc");

  return gst_element_register (airplaysrc, "airplaysrc", GST_RANK_NONE,
      GST_TYPE_AIRPLAY_SRC);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "airplay"
#endif

/* gstreamer looks for this structure to register airplays
 *
 * exchange the string 'Template airplay' with your airplay description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "airplay",
    "airplay source element",
	airplay_src_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)