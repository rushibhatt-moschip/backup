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


//enum for properties 
//can add more properies
enum
{
	PROP_0,
	PROP_NEXT,
	PROP_SILENT
};

/*void on_new_sample(GstElement *sink) {
  GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
  GstBuffer *buffer = gst_sample_get_buffer(sample);

// Get the data pointer from the buffer
GstMapInfo map;
if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
// Print the content of the buffer
g_print("Buffer content: %.*s\n", (int)map.size, (gchar *)map.data);

// Unmap the buffer
gst_buffer_unmap(buffer, &map);
}

// Clean up
gst_sample_unref(sample);
}
*/

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 *
 */

//created static pad template
//description of pad 
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


//setter and getter funciton for property
static void gst_my_filter_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_my_filter_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

//global declaration of sink_event function and chain function
static gboolean gst_my_filter_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_filter_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the myfilter's class */
	static void
gst_my_filter_class_init (GstMyFilterClass * klass)
{
	g_print("\ni am my_filter_class_init \n\n");

	// creating attributes(variables) 
	GObjectClass *gobject_class; //attribute of type GObjectClass
	GstElementClass *gstelement_class; //variable of type GstElementClass


	// type casting GstMyFilterClass type pointer to GObjectClass and assigning to variable named gobject_class
	gobject_class = (GObjectClass *) klass;  
	gstelement_class = (GstElementClass *) klass;

	//defining methods( virtual function) 
	gobject_class->set_property = gst_my_filter_set_property;
	gobject_class->get_property = gst_my_filter_get_property;

	g_object_class_install_property (gobject_class, PROP_SILENT,
			g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
				FALSE, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_NEXT,
			g_param_spec_boolean ("next", "next", "does nothing",
				TRUE, G_PARAM_READWRITE));

	//registering element metadata (ie factory name ,class, discription, author.) 
	gst_element_class_set_details_simple (gstelement_class,
			"MyFilter",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Rushi Bhatt <<user@hostname.org>>");

	//registering pad(which are defined above globally) templates 
	//to the element class
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
	//creates pad from the static template
	//from the pad template which is regitsered with element class in the class_init function  
	//"sinkpad" is defined in myfilter.h file  
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

	// setting event function this function is called
	// whenever some events occur 
	gst_pad_set_event_function (filter->sinkpad,      // saying that hear the event occuring on sinkpad
			GST_DEBUG_FUNCPTR (gst_my_filter_sink_event));//this is the name of function which will be called on event	


	// setting chain function this function does the real job
	// takes the input from the sink pad and processes it 
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_my_filter_chain));

	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);

	//adds the pad (which we created few lines above) to the element
	//this function emmits pad- added signal on the element
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);


	//creates pad from the static template
	//from the pad template which is regitsered with element class in the class_init function  
	//"srcpad" is defined in myfilter.h file  
	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);


	//initial values of proeperties
	filter->next = FALSE;
	filter->silent = FALSE;
}

	static void
gst_my_filter_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec)
{
	g_print("i am my_filter_set_property \n");
	GstMyFilter *filter = GST_MYFILTER (object);

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			g_print("case of prop silent called\n");
			break;
		case PROP_NEXT:
			g_print("case of prop next called\n");
			filter->next = g_value_get_boolean (value);
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
	g_print("i am my_filter_get_property \n");
	GstMyFilter *filter = GST_MYFILTER (object);

	switch (prop_id) {
		case PROP_SILENT:

			g_value_set_boolean (value, filter->silent);
			g_print("silent is %d\n",filter->silent);
			break;
		case PROP_NEXT:
			g_value_set_boolean (value, filter->next);
			// filter->next = g_value_get_boolean (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */

//event funciton 
//will be called whenever event occurs
// events such as stream start, caps ,segment , EOS
	static gboolean
gst_my_filter_sink_event (GstPad * pad, GstObject * parent,
		GstEvent * event)
{
	GstMyFilter *filter;
	gboolean ret;

	filter = GST_MYFILTER (parent);

	GST_LOG_OBJECT (filter, " Received %s event: %" GST_PTR_FORMAT,
			GST_EVENT_TYPE_NAME (event), event); // this macro gets the name of event that occured

	// g_print("event name is  %s \n", GST_EVENT_TYPE_NAME (event));  // to print name of event

	//switch case to handle different events accordingly
	switch (GST_EVENT_TYPE (event)) {

		case GST_EVENT_STREAM_START:
			g_print("i am my_filter_sink_event and calling frome stream start  \n");
			ret = gst_pad_event_default (pad, parent, event);
			break;

		case GST_EVENT_CAPS:
			GstCaps *caps;
			g_print("i am my_filter_sink_event and calling frome the caps  \n");

			//take caps and store it in the variable caps
			gst_event_parse_caps (event, &caps);
			//converts caps(type=GstCaps (a structure)) to type=string for ease of printing   
			gchar *capsString = gst_caps_to_string(caps);
			g_print("\n\nthose are the caps\n %s\n\n\n",capsString);
			/* do something with the caps */
			// and forward
			// pushes event to next element to notify the3 change 
			ret = gst_pad_push_event (filter->srcpad, event);

			//calls default event handler which is defined internally
			//ret = gst_pad_event_default (pad, parent, event);
			break;


			/*this event is used to define segment of media
format: The format of the segment, specifying the units for start, stop, and position fields (eg-GST_FORMAT_TIME)
flags: Flags indicating additional information about the segment (e.g., whether the segment is scheduled to repeat).
start: The start position of the segment.
stop: The stop position of the segment.
time: The current position within the segment.
position> The current position in stream time,duration of segment,rate,applied_rate
*/
		case GST_EVENT_SEGMENT:
			g_print("i am my_filter_sink_event and calling frome segment  \n");
			//not working
			//gst_event_parse_segment (event, seg);
			// gchar *segString = gst_event_to_string(caps);
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
//this fyunction will receive and process data on the sinkpad
	static GstFlowReturn
gst_my_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	g_print("i am my_filter_chain \n");
	GstMyFilter *filter;

	GstMapInfo map;
	if (gst_buffer_map(buf, &map, GST_MAP_READWRITE)) {
		// Access buffer values
		guint size = map.size;
		guint8 *data = map.data;


		//my styyle to print and check size

		guint k=0;
		guint i=0;
		g_print("%d \n ",size);

		/* prints whole buffer
		   while(data[i])
		   {

		   g_print("%d ",data[i]);
		//g_print("%02X ",data[i]); // in hex
		g_print("  ");
		i++;
		}
		*/
		/*


		   while(data[i] == 145)
		   {
		   k++;
		   g_print("%02X ",data[i]);
		   g_print("  ");
		   i++;
		   }
		   g_print("\n%d\n ",k);
		   g_print("\ni is %d\n ",i);
		   while(data[i] == 54)
		   {
		   k++;
		   g_print("%02X ",data[i]);
		   i+=2;
		   }

		   i++;
		   g_print("\n%d\n ",k);
		   while(data[i] == 34)
		   {
		   k++;
		   g_print("%02X ",data[i]);
		   i+=2;
		   }
		   g_print("\n%d\n ",k);

*/
		void bin(unsigned n)
		{
			/* step 1 */
			if (n > 1)
				bin(n / 2);

			/* step 2 */
			printf("%d", n % 2);
		}

		guint width = 320 ;  
		guint height = 240; 
		guint p=160;//right  (320 max)row
		guint q=120;//down    (240 max)col
		guint length=30;
		guint lenx,leny;
		lenx= length;
		lenx*= lenx; // radius squared
		guint y,u,v;
		int gtu=0;
		short int rus;
		for (guint row = 0; row < height; row++) {
			for (guint col = 0; col < width; col++) {
				//just prints the value

				y=(row*width)+col;
				u=(width*height) + ((row*width)/2 ) + col;
				//   v=(width*height) + ((row*col)/4);
				if (row == 120 )

				{            gtu++;
					data[y] = 3;
					//data[u++] = 90;
					//data[u] = 240;

					if(u%2){
						data[++u] = 95; 
						//rus=data[u];
						//g_print("\n");

						//g_print("y is 0b%d  ", rus);
						//bin(rus);

						data[--u] = 250;
						continue;
					}
					data[u++] = 95;     
					data[u] = 250;
					g_print("\n final %d  ", gtu);
				} 


				/* {
				   g_print("u is %d  ", data[u]);
				   rus=data[];
				   bin(rus);
				   g_print("\n");
				   }*/




				/*print the yuv values
				  g_print("y is %d  ", data[y]);

				  if(u%2){
				  g_print("u is %d  ", data[++u]);     
				  g_print("v  is %d  ", data[--u]);

				  g_print("\n");
				  continue;
				  }
				  g_print("u is %d  ", data[u++]);     
				  g_print("v is %d  ", data[u]);
				  g_print("\n");
				  */
				k++;    
			}   
		}
		g_print("ans is %d",k);


		// g_print("%d %d %d ", data[row * width + col],
		//data[76800 + (row / 2) * (width / 2) + (col / 2)],
		//data[ 96000+ (row / 2) * (width / 2) + (col / 2)]);
		// g_print("\n end line\n");
		//  g_print("Y:%02X U:%02X V:%02X ", data[row * width + col],
		//print value end




		//modify value start circle
		/*   leny = ((col-q) * (col-q)) + ((row-p) * (row-p));   // measured lenght 
		     if (leny <= lenx){ 				
		     data[(row * width) + col]=81;                //y
		     data[76800 + (row / 2) * (width / 2) + (col / 2)]=240;//u
		     data[ 96000+ (row / 2) * (width / 2) + (col / 2)]=90;//v
		     }*/
		//circle end


		/*		//modify value start square
				if (row >= (q-length) && row <= (q+length)) {
				if(col >= (p-length) && col <=  (p+length)){

				data[(row * width) + col]=81;                //y
				data[76800 + (row / 2) * (width / 2) + (col / 2)]=240;//u
				data[ 96000+ (row / 2) * (width / 2) + (col / 2)]=90;//v
				}
				}
		//square end 
		*/


		//for (guint i = 0; i < size; i++) {
		//
		//          g_print("%02X ", data[i]);
		//
		//    }
		//      g_print("\nrushi \n\n");
		//
		// Process buffer values as needed
		// Example: Print buffer content
		//g_print("Buffer content: %.*s\n", size, (gchar *)data);
		//  g_print("Buffer content: %u\n",*data);
		// Unmap the buffer
		gst_buffer_unmap(buf, &map);
	}
	/*   */
	filter = GST_MYFILTER (parent);
	if (filter->next == TRUE)
	{
		g_print("\n\nhahaha  doing nothing\n\n");
	}
	if (filter->silent == FALSE)
	{
		g_print ("I'm plugged, therefore I'm in.\n");
		g_print("Received buffer with size (in bytes): %zu\n", gst_buffer_get_size(buf));
	}
	/* just push out the incoming buffer without touching it */
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
