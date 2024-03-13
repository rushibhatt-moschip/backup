/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2024 Rushi Bhatt <<user@hostname.org>>
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
 * SECTION:element-myfilter
 *
 * FIXME:Describe myfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! myfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmyfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_my_filter_debug);
#define GST_CAT_DEFAULT gst_my_filter_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};


enum
{
	PROP_0,
	PROP_NEXT,
	PROP_DRAW,
	PROP_COPY,
	PROP_SHAPE,
	PROP_SHAPE_SIZE,
	PROP_COLOUR,
	PROP_FILL,
	PROP_ALPHA,
	PROP_MODE,
	PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 *
 */

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("ANY")
		);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("ANY")
		);


#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (my_filter, "my_filter", GST_RANK_NONE,
		GST_TYPE_MYFILTER);


static void gst_my_filter_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_my_filter_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_my_filter_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_filter_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);


	static void
gst_my_filter_class_init (GstMyFilterClass * klass)
{
	g_print("\ni am my_filter_class_init \n\n");

	GObjectClass *gobject_class; //attribute of type GObjectClass
	GstElementClass *gstelement_class; //variable of type GstElementClass


	gobject_class = (GObjectClass *) klass;  
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_my_filter_set_property;
	gobject_class->get_property = gst_my_filter_get_property;

	g_object_class_install_property (gobject_class, PROP_SILENT,
			g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
				FALSE, G_PARAM_READWRITE));
 g_object_class_install_property (gobject_class, PROP_MODE,
                        g_param_spec_boolean ("mode", "mode", "strict mode or not",
                                FALSE, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_NEXT,
			g_param_spec_boolean ("next", "next", "does nothing",
				TRUE, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_SHAPE_SIZE,
			g_param_spec_int ("size", "size", "takes size",
				0,240,10, G_PARAM_READWRITE));
g_object_class_install_property (gobject_class, PROP_ALPHA,
                        g_param_spec_int ("alpha", "alpha", "takes alpha value",
                                0,100,100, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_DRAW,
			gst_param_spec_array("coordinate", "coordinate"
				,"takes coordinates input",
				g_param_spec_int("axis","axis","x-y axis",0,560,120,G_PARAM_READWRITE),
				G_PARAM_READWRITE));
	
	g_object_class_install_property (gobject_class, PROP_COPY,
			gst_param_spec_array("coor", "coor"
				,"takes copy coordinates input",
				g_param_spec_int("axis","axis","x-y axis",0,560,120,G_PARAM_READWRITE),
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_SHAPE,
			g_param_spec_string("shape", "shape","takes string","rushi",
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_COLOUR,
			g_param_spec_string("colour", "colour","takes colour name","red",
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_FILL,
			g_param_spec_string("fill", "fill","takes fill type","yes",
				G_PARAM_READWRITE));

	gst_element_class_set_details_simple (gstelement_class,
			"MyFilter",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Rushi Bhatt <<user@hostname.org>>");

	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&src_factory));//creates GstPadTemplate from static pad template
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
	static void
gst_my_filter_init (GstMyFilter * filter)
{

	g_print("i am my_filter_init \n");
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

	gst_pad_set_event_function (filter->sinkpad,      // saying that hear the event occuring on sinkpad
			GST_DEBUG_FUNCPTR (gst_my_filter_sink_event));//this is the name of function which will be called on event	


	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_my_filter_chain));

	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);

	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->shape = "rshi";
	filter->next = FALSE;
	filter->silent = FALSE;
	filter->size = 10;
	filter->fill= "yes";
}

	static void
gst_my_filter_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec)
{
	GstMyFilter *filter = GST_MYFILTER (object);
	const GValue *val ;

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			g_print("case of prop silent called\n");
			break;
		case PROP_MODE:
                        filter->mode = g_value_get_boolean (value);
                        g_print("case of prop mode called\n");
                        break;

		case PROP_NEXT:
			g_print("case of prop next called\n");
			filter->next = g_value_get_boolean (value);
			break;

		case PROP_SHAPE:
			g_print("case of prop shape called\n");
			filter->shape = g_value_dup_string (value);
		//	g_print("\n\n\n\nstring is %s\n",filter->shape);
			break;

		case PROP_COLOUR:
			g_print("case of prop colour called\n");
			filter->colour = g_value_dup_string (value);
		//	g_print("\n\n\n\ncolour is is %s\n",filter->colour);
			break;

		case PROP_FILL:
			g_print("case of prop fill called\n");
			filter->fill = g_value_dup_string (value);
		//	g_print("\n\n\nfill colour  is %s\n",filter->fill);
			break;

		case PROP_SHAPE_SIZE:
			g_print("case of shape size called\n");
			filter->size = g_value_get_int(value);
			break;

		case PROP_ALPHA:
                        g_print("case of alpha called\n");
                        filter->alpha = g_value_get_int(value);
                        break;

		case PROP_DRAW:
			g_print("case of draw cordinate\n");
			for (gint i = 0 ; i<2 ; i++)
			{
				val = gst_value_array_get_value (value, i);
				filter->coordinate[i] = g_value_get_int(val);
		//		g_print("value is %d\n",filter->coordinate[i]);
			}
			break;
 		case PROP_COPY:
                        g_print("case of copy cordinate\n");
                        for (gint i = 0 ; i<2 ; i++)
                        {
                                val = gst_value_array_get_value (value, i);
                                filter->coor[i] = g_value_get_int(val);
                  //              g_print("value is %d\n",filter->coor[i]);
                        }
                        break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

	static void
gst_my_filter_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec)
{
	GstMyFilter *filter = GST_MYFILTER (object);
	GValue x = G_VALUE_INIT;
	GValue y = G_VALUE_INIT;
	GValue xx = G_VALUE_INIT;
	GValue yy = G_VALUE_INIT;
	switch (prop_id) {

		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		 case PROP_MODE:
                        g_value_set_boolean (value, filter->mode);
                        break;

		case PROP_NEXT:
			g_value_set_boolean (value, filter->next);
			break;

		case PROP_SHAPE:
			g_value_set_string (value, filter->shape);
			break;

		case PROP_COLOUR:
			g_value_set_string (value, filter->colour);
			break;

		case PROP_FILL:
			g_value_set_string (value, filter->fill);
			break;

		case PROP_SHAPE_SIZE:
			g_value_set_int (value, filter->size);
			break;

		case PROP_ALPHA:
                        g_value_set_int (value, filter->alpha);
                        break;
		
		case PROP_DRAW:
			g_value_init(&x, G_TYPE_INT);
			g_value_init(&y, G_TYPE_INT);
			g_value_set_int(&x,filter->coordinate[0]);
			g_value_set_int(&y,filter->coordinate[1]);
			gst_value_array_append_and_take_value(value,&x);
			gst_value_array_append_and_take_value(value,&y);
			break;
		case PROP_COPY:
                        g_value_init(&xx, G_TYPE_INT);
                        g_value_init(&yy, G_TYPE_INT);
                        g_value_set_int(&xx,filter->coor[0]);
                        g_value_set_int(&yy,filter->coor[1]);
                        gst_value_array_append_and_take_value(value,&xx);
                        gst_value_array_append_and_take_value(value,&yy);
                        break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

	static gboolean
gst_my_filter_sink_event (GstPad * pad, GstObject * parent,
		GstEvent * event)
{
	GstMyFilter *filter;
	gboolean ret;

	filter = GST_MYFILTER (parent);

	GST_LOG_OBJECT (filter, " Received %s event: %" GST_PTR_FORMAT,
			GST_EVENT_TYPE_NAME (event), event); // this macro gets the name of event that occured

	switch (GST_EVENT_TYPE (event)) {

		case GST_EVENT_STREAM_START:
			g_print("i am my_filter_sink_event and calling frome stream start  \n");
			ret = gst_pad_event_default (pad, parent, event);
			break;

		case GST_EVENT_CAPS:
			GstCaps *caps;
			g_print("i am my_filter_sink_event and calling frome the caps  \n");

			gst_event_parse_caps (event, &caps);
			gchar *capsString = gst_caps_to_string(caps);
			g_print("\n\nthose are the caps\n %s\n\n\n",capsString);
			ret = gst_pad_push_event (filter->srcpad, event);
			break;

		case GST_EVENT_SEGMENT:
			g_print("i am my_filter_sink_event and calling frome segment  \n");
			ret = gst_pad_event_default (pad, parent, event);
			break;

		case GST_EVENT_EOS:
			g_print("i am my_filter_sink_event and calling frome end of stream  \n");
			ret = gst_pad_event_default (pad, parent, event);
			break;

		default:
			ret = gst_pad_event_default (pad, parent, event);
			g_print("i am my_filter_sink_event and calling frome default  \n");
			break;
	}
	return ret;
}

/* chain function
 * this function does the actual processing
 */
	static GstFlowReturn
gst_my_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	g_print("i am my_filter_chain \n");
	GstMyFilter *filter;
	filter = GST_MYFILTER (parent);
	gint a=9;
	g_print("asnwer is %f \n",sqrt(a));
	GstMapInfo map;
	if (gst_buffer_map(buf, &map, GST_MAP_READWRITE)) {
		// Access buffer values
		guint size = map.size;
		guint8 *data = map.data;
		guint width = 320 ;
		guint height = 240;
		gint x=120;
		gint y1=160;
		guint y,u;
		gint len=20;
		guint v=0;
		gint t,p;
//horizontal lines
		for (guint row = 0; row < height; row++) {			
			for (guint col = (y1-len) ; col < (y1+len+1); col++) {   //set the length of line
				y=(row*width)+col;
				u=(width*height) + ((row*width)/2 ) + col;
				
				if (row == (x-len) || row == (x+len))            // set the position of line
				{	
					data[y] = 81 ;
					//data[t] = 81 ; 
					if(u%2){
						data[++u] = 95;
					       	data[--u] = 250;
						continue;
					}

					data[u++] = 95;
					data[u] = 250;
				}	
			}
		}

//vertical lines
             for (guint col = 0; col < width; col++) {                         
			for (guint row = (x-len); row < (x+len); row++) {         // set the length of line
				y=(row*width)+col;
                                u=(width*height) + ((row*width)/2 ) + col;
 
                                if (col == (y1-len) || col == (y1+len))           // set the position of line
                                {	v+=1;
                                        data[y] = 81 ;
					if(!(v%2))
						continue;	
                                        if(u%2){
                                                data[++u] = 95;
                                                data[--u] = 250;
                                                continue;
                                        }

                                        data[u++] = 95;
                                        data[u] = 250;
                                }
                        }
                }



	 gst_buffer_unmap(buf, &map);
	}
	
	
	
	
	
	
	if (filter->next == TRUE)
	{
		g_print("\n\nhahaha  doing nothing\n\n");
	}
	if (filter->silent == FALSE)
	{
		g_print ("I'm plugged, therefore I'm in.\n");
		g_print("Received buffer with size (in bytes): %zu\n", gst_buffer_get_size(buf));
	}

	return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
	static gboolean
myfilter_init (GstPlugin * myfilter)
{
	g_print("i am onlyy myfilter_init() \n");
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template myfilter' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_my_filter_debug, "myfilter",
			0, "Template myfilter");

	return GST_ELEMENT_REGISTER (my_filter, myfilter);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmyfilter"
#endif

/* gstreamer looks for this structure to register myfilters
 *
 * exchange the string 'Template myfilter' with your myfilter description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		myfilter,
		"my_filter",
		myfilter_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
