/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2024 varshi Bhatt <<user@hostname.org>>
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
 * SECTION:element-drawshapes
 *
 * FIXME:Describe drawshapes here.
 *
 * <refsecrow2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! drawshapes ! fakesink silent=TvarE
 * ]|
 * </refsecrow2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstdrawshapes.h"

GST_DEBUG_CATEGORY_STATIC (gst_drawshapes_debug);
#define GST_CAT_DEFAULT gst_drawshapes_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};



//variables var;

void check_func(GstDrawshapes *filter,guint width,guint height);

enum
{
	PROP_0,
	PROP_DRAW_COORDINATE,
	PROP_SHAPE,
	PROP_SHAPE_SIZE,
	PROP_SHAPE_COLOR,
	PROP_FILL_MODE,
	PROP_COPY_COORDINATES,
	PROP_ALPHA,
	PROP_STRICT_MODE,
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("video/x-raw,"
			"format=(string)NV12,"
			"width=(int)[1,1920],"
			"height=(int)[1,1080],"
			"framerate=(fraction)[1,60]")
		);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("video/x-raw,"
			"format=(string)NV12,"
			"width=(int)[1,1920],"
			"height=(int)[1,1080],"
			"framerate=(fraction)[1,60]")
		);

#define gst_drawshapes_parent_class parent_class
G_DEFINE_TYPE (GstDrawshapes, gst_drawshapes, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (drawshapes, "drawshapes", GST_RANK_NONE,
		GST_TYPE_DRAWSHAPES);

static void gst_drawshapes_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_drawshapes_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_drawshapes_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_drawshapes_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the drawshapes's class */
	static void
gst_drawshapes_class_init (GstDrawshapesClass * klass)
{
	GST_DEBUG("I am class init\n");
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_drawshapes_set_property;
	gobject_class->get_property = gst_drawshapes_get_property;

	g_object_class_install_property (gobject_class, PROP_STRICT_MODE,
			g_param_spec_boolean ("strict-mode", "strict-mode", "to set strict mode\
				\n\t\t\twhich will throw error and quit in case of any issue ",FALSE, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_SHAPE_SIZE,
			g_param_spec_int ("shape-size", "shape-size", "size of the shape from centre of object to its edge",
				-214748364,2147483647,10, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_ALPHA,
			g_param_spec_int ("alpha", "alpha", "alpha value to set opacity 0=transperent 100=opaque",
				-214748364,214748364,100, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_DRAW_COORDINATE,
			gst_param_spec_array("draw-coordinate", "draw-coordinate"
				,"coordinates'<x,y>' where the shape centre should be placed",
				g_param_spec_int("axis","axis","x-y axis",-214748364,2147483647,0,G_PARAM_READWRITE),
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_COPY_COORDINATES,
			gst_param_spec_array("copy_coordinate", "copy_coordinate"
				,"coordinates'<x,y>'of shape from where the pixels are to be copied",
				g_param_spec_int("axis","axis","x-y axis",-214748364,2147483647,0,G_PARAM_READWRITE),
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_SHAPE,
			g_param_spec_string("shape", "shape","shape you want to draw\
				\n\t\t\t\tsquare\n\t\t\t\trectangle\
				\n\t\t\t\ttriangle\n\t\t\t\tcircle","square",
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_SHAPE_COLOR,
			g_param_spec_string("shape-color", "shape-color","colour of the shape\
				\n\t\t\t\tred\n\t\t\t\tblue\n\t\t\t\tgreen","red",
				G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, PROP_FILL_MODE,
			g_param_spec_string("fill-mode", "fill-mode","fill type of the shape\
				\n\t\t\t\tno-fill = just the boundary of the shape\
				\n\t\t\t\tcolor = fill it with colour\
				\n\t\t\t\tcopy = fills it with copied pixels","no-fill",
				G_PARAM_READWRITE));

	gst_element_class_set_details_simple (gstelement_class,
			"Drawshapes",
			"element",
			"draws shapes over a nv12 video stream", "rushikumar Bhatt <<rushi@softnautics.org>>");

	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance stvarcture
 */
	static void
gst_drawshapes_init (GstDrawshapes * filter)
{
	GST_DEBUG("I am draw shapes init\n");
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->mode = FALSE;
	filter->alpha = 100;
	filter->fill = "no-fill";
	filter->shape = "square";
	filter->colour = "red";

}

	static void
gst_drawshapes_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec)
{
	GST_DEBUG("I am set prop\n");
	GstDrawshapes *filter = GST_DRAWSHAPES (object);
	const GValue *val ;
	switch (prop_id) 
	{
		case PROP_STRICT_MODE:
			filter->mode = g_value_get_boolean (value);
			GST_DEBUG("case of prop mode called\n");
			break;

		case PROP_SHAPE:
			GST_DEBUG("case of prop shape called\n");
			filter->shape = g_value_dup_string (value);
			break;

		case PROP_SHAPE_COLOR:
			GST_DEBUG("case of prop colour called\n");
			filter->colour = g_value_dup_string (value);
			break;

		case PROP_FILL_MODE:
			GST_DEBUG("case of prop fill called\n");
			filter->fill = g_value_dup_string (value);
			break;

		case PROP_SHAPE_SIZE:
			GST_DEBUG("case of shape size called\n");
			GST_DEBUG("shape size is %d\n",filter->size);
			filter->size = g_value_get_int(value);
			break;

		case PROP_ALPHA:
			GST_DEBUG("case of alpha called\n");
			filter->alpha = g_value_get_int(value);
			break;

		case PROP_DRAW_COORDINATE:
			GST_DEBUG("case of draw cordinate\n");
			for (gint i = 0 ; i<2 ; i++)
			{
				val = gst_value_array_get_value (value, i);
				filter->coordinate[i] = g_value_get_int(val);
			}
			break;
		case PROP_COPY_COORDINATES:
			GST_DEBUG("case of copy cordinate\n");
			for (gint i = 0 ; i<2 ; i++)
			{
				val = gst_value_array_get_value (value, i);
				filter->copy_coor[i] = g_value_get_int(val);
			}
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

	static void
gst_drawshapes_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec)
{
	GST_DEBUG("I am get prop\n");
	GstDrawshapes *filter = GST_DRAWSHAPES (object);
	GValue x = G_VALUE_INIT;
	GValue y = G_VALUE_INIT;
	GValue xx = G_VALUE_INIT;
	GValue yy = G_VALUE_INIT;
	switch (prop_id) {

		case PROP_STRICT_MODE:
			g_value_set_boolean (value, filter->mode);
			break;

		case PROP_SHAPE:
			g_value_set_string (value, filter->shape);
			break;

		case PROP_SHAPE_COLOR:
			g_value_set_string (value, filter->colour);
			break;

		case PROP_FILL_MODE:
			g_value_set_string (value, filter->fill);
			break;

		case PROP_SHAPE_SIZE:
			g_value_set_int (value, filter->size);
			GST_DEBUG("hii \n\n\n i am  get prop \ncase of shape size called\n");
			GST_DEBUG("shape size is %d\n",filter->size);
			break;

		case PROP_ALPHA:
			g_value_set_int (value, filter->alpha);
			break;

		case PROP_DRAW_COORDINATE:
			g_value_init(&x, G_TYPE_INT);
			g_value_init(&y, G_TYPE_INT);
			g_value_set_int(&x,filter->coordinate[0]);
			g_value_set_int(&y,filter->coordinate[1]);
			gst_value_array_append_and_take_value(value,&x);
			gst_value_array_append_and_take_value(value,&y);
			break;
		case PROP_COPY_COORDINATES:
			g_value_init(&xx, G_TYPE_INT);
			g_value_init(&yy, G_TYPE_INT);
			g_value_set_int(&xx,filter->copy_coor[0]);
			g_value_set_int(&yy,filter->copy_coor[1]);
			gst_value_array_append_and_take_value(value,&xx);
			gst_value_array_append_and_take_value(value,&yy);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
	static gboolean
gst_drawshapes_sink_event (GstPad * pad, GstObject * parent,
		GstEvent * event)
{
	GST_DEBUG("I am sink event\n");
	GstDrawshapes *filter;
	gboolean ret;

	filter = GST_DRAWSHAPES (parent);

	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
			GST_EVENT_TYPE_NAME (event), event);

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_STREAM_START:
			ret = gst_pad_event_default (pad, parent, event);
			break;
		case GST_EVENT_CAPS:
			GstCaps *caps;
			GstStructure *structure;
			gst_event_parse_caps (event, &caps);
			structure = gst_caps_get_structure(caps, 0);
			gst_structure_get_int(structure, "width", &filter->var.width);
			gst_structure_get_int(structure, "height", &filter->var.height);
			check_func(filter, filter->var.width, filter->var.height);

			gchar *capsString = gst_caps_to_string(caps);
			GST_DEBUG("\n\nthose are the caps\n %s",capsString);
			ret = gst_pad_push_event (filter->srcpad, event);
			break;
		case GST_EVENT_SEGMENT:
			ret = gst_pad_event_default (pad, parent, event);
			break;
		default:
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;
}

/* chain function
 * this function does the actual processing
 */

int square_func(guint8 *data, gint width, gint height, gint x1, gint y1,
		gint y_val, gint u_val, gint v_val, gint len, gint lenn,
		gint fill, gfloat alpha, gint copy, gint xc, gint yc )
{
	gint row,col,y,u;
	gint y_blended,u_blended,v_blended;
	gint temp;
	//	if (alpha<0)
	//		alpha=100;

	alpha=alpha/100;

	if(!copy){
		xc=x1;
		yc=y1;
	}

	gint rowc,colc,yy,uu,temp1;
	rowc=yc-len;
	colc=xc-len;

	for (row=y1-len ; row<=y1+len ; row++,rowc++)
	{
		if(row<0 || rowc<0)
			continue;
		if(row>height-1 || rowc>height-1)
			break;
		for(col=x1-len,colc=xc-len ; col<=x1+len ; col++,colc++)
		{
			if (col<0 || colc<0)
				continue;
			if(col>width-1 || colc>width-1)
				break;
			temp=0;
			if(row%2)
				temp=width/2;

			y = (row*width) + col ;
			u = (width*height) + ((row*width)/2 ) + col - temp;

			temp1=0;
			if(rowc%2)
				temp1=width/2;
			yy = (rowc*width) + colc ;
			uu=(width*height) + ((rowc*width)/2 ) + colc - temp1;

			if(fill)
				goto next;

			if (row == (y1-len) || row == (y1+len) || col == x1-lenn || col == x1+lenn)
			{
next:                  
				if (row >= (y1-len) || row <= (y1+len) || col >= x1-lenn || col <= x1+lenn)

				{
					if (copy)
						y_val = data[yy];		
					y_blended =((1-alpha) * data[y]) + (alpha * y_val); 
					data[y]=y_blended;   

					if(u%2)
						u-=1;
					if (uu%2)
						uu-=1;
					if (copy)
						u_val = data[uu++];
					u_blended = ((1-alpha) * data[u]) + (alpha * u_val);
					data[u++] = u_blended; 
					if (copy)
						v_val = data[uu];
					v_blended = ((1-alpha) * data[u]) + (alpha * v_val);
					data[u] = v_blended;
				}
			}
		}
	}

	return 0;

}

int circle_func(guint8 *data, gint width, gint height, gint x1, gint y1,
		gint y_val, gint u_val, gint v_val, gint len, gint lenn,
		gint fill, gfloat alpha, gint copy, gint xc, gint yc )
{
	gint row,col,y,u;
	gint y_blended,u_blended,v_blended;
	gint temp;
	//	if (alpha<0)
	//		alpha=100;
	alpha=alpha/100;

	if(!copy){
		xc=x1;
		yc=y1;
	}

	gint rowc,colc,yy,uu,temp1;
	rowc = yc-len;
	colc = xc-len;

	gint num1,num2,calc_radius;

	for (row=y1-len ; row<=y1+len ; row++,rowc++)
	{
		if(row<0 || rowc<0)
			continue;
		if(row>height-1 || rowc>height-1)
			break;
		for(col=x1-len,colc=xc-len ; col<=x1+len ; col++,colc++)
		{
			if (col<0 || colc<0)
				continue;
			if(col>width-1 || colc>width-1)
				break;
			temp = 0;
			if(row%2)
				temp = width/2;

			num1= row-y1;
			num2= col-x1;
			calc_radius = sqrt((pow(num1,2)) + (pow(num2,2)));

			y = (row*width) + col ;
			u=(width*height) + ((row*width)/2 ) + col - temp ;

			temp1=0;
			if(rowc%2)
				temp1=width/2;
			yy = (rowc*width) + colc ;
			uu=(width*height) + ((rowc*width)/2 ) + colc - temp1;

			if(fill)
				goto next;
			if(calc_radius == len)
			{
next:                  
				if(calc_radius <= len)
				{
					if (copy)
						y_val = data[yy];		
					y_blended =((1-alpha) * data[y]) + (alpha * y_val); 
					data[y]=y_blended;   

					if(u%2)
						u-=1;
					if (uu%2)
						uu-=1;
					if (copy)
						u_val = data[uu++];
					u_blended = ((1-alpha) * data[u]) + (alpha * u_val);
					data[u++] = u_blended; 
					if (copy)
						v_val = data[uu];
					v_blended = ((1-alpha) * data[u]) + (alpha * v_val);
					data[u] = v_blended;
				}
			}
		}
	}

	return 0;

}

//triangleeeeeeeee
int triangle_func(guint8 *data, gint width, gint height, gint xx, gint yy,
		gint y_val, gint u_val, gint v_val, gint len, gint lenn, 
		gint fill, gint alpha, gint copy, gint xc, gint yc)
{
	gint y_blended,u_blended,v_blended;
	gint x1,y1,x2,y2,x3,y3;   //x1 y1 top coordinates of triangle, x2 y2 left coordinates, x3 y3 right coordinates
	x1 = xx;
	y1 = yy - len;

	x2 = xx - (0.866 *len);  //root 3 / 2 =0.866 
	y2 = yy + (len/2);

	x3 = xx + (0.866 *len);
	y3 = yy + (len/2);
	
	if(!copy){
		xc=xx;
		yc=yy;
	}

	alpha=alpha/100;
	
	gint row,col,y,u,temp;		

	gfloat m1 = tan( (M_PI)/3);         //1.73;  //(y3-y1)/(x3-x1) ;
					    //	gfloat m1 = 1.73;
	gfloat m2=-1.73;

	gfloat c1 =  y1 - (m1*x1) ;
	gfloat c2 =  y1 - (m2*x1) ;

	//	gint x4,y4,dist1,dist2;
	//	x4 = x1-(1.73 *len) ;
	//	y4 = y1 ;

	gint y_copy,uu;
	gint rowc, colc, temp1;

	rowc=yc-len;  //-1;

	for(row = y1 ; row<=y3 ; row++,rowc++){

		if(row<0 || rowc<0)
			continue;
		if(row>height-1 || rowc>height-1)
			break;
		for (col=x2,colc=xc-(0.86*len) ; col<=x3 ; col++,colc++)
		{
			if (col<0 || colc<0)
				continue;
			if(col>width-1 || colc>width-1)
				break;
			y = (row*width) + col ;
			y_copy = (rowc*width)+ colc ; 
			temp=0;
			if(row%2)
				temp=width/2;
			u=(width*height) + ((row*width)/2 ) + col - temp;
			temp1=0;
			if (rowc%2)
				temp1=width/2;	
			uu=(width*height) + ((rowc*width)/2 ) + colc - temp1;
			if (!fill)
				goto here;
			if(row >= ((col * m1)+c1-1) && row >= ((col * m2)+c2) )
			{
				if (copy)
					y_val = data[y_copy];		
				y_blended =((1-alpha) * data[y]) + (alpha * y_val); 
				data[y]=y_blended; 
				if(u%2)
					u-=1;
				if(uu%2)
					uu-=1;
				if (copy)
					u_val = data[uu++];		
				u_blended = ((1-alpha) * data[u]) + (alpha * u_val);
				data[u++] = u_blended; 

				if (copy)
					v_val = data[uu];		
				v_blended = ((1-alpha) * data[u]) + (alpha * v_val);
				data[u] = v_blended;
			}
			continue;
here:
			if(row == y2)
			{
				data[y] = y_val;
				if(u%2){
					data[++u] = u_val;
					data[--u] = v_val;
					continue;
				}
				data[u++] = u_val;
				data[u] = v_val;
			}
			//this line(//)			
			if(row >= ((col * m2)-1+c2)  && row < ((col*(m2))+2+c2)   )
			{ 
				data[y] = y_val;
				if(u%2){
					data[++u] = u_val;
					data[--u] = v_val;
					continue;
				}
				data[u++] = u_val;
				data[u] = v_val;
			}

			// this line ( \\ )	
			if(row >= ((col * m1)-1+c1)  && row < ((col*m1)+2+c1)   )
			{
				data[y] = y_val;
				if(u%2){
					data[++u] = u_val;
					data[--u] = v_val;
					continue;
				}
				data[u++] = u_val;
				data[u] = v_val;
			}

			/*
			// this line with distance formula (//)
			dist1 = sqrt(pow(row-y4,2) + pow(col-x4,2));
			dist2 = sqrt(pow(row-y3,2) + pow(col-x3,2));

			if(dist1 == dist2 || dist1 == dist2+1 || dist1== dist2+2)		
			{
			data[y] = y_val;
			if(u%2){
			data[++u] = u_val;
			data[--u] = v_val;
			continue;
			}
			data[u++] = u_val;
			data[u] = v_val;

			}
			*/
			//this line  (//)
		}
	}
	return 0 ;
}

int rectangle_func(guint8 *data, gint width, gint height, gint x1, gint y1,
		gint y_val, gint u_val, gint v_val, gint len, gint lenn,
		gint fill, gfloat alpha, gint copy, gint xc, gint yc )
{
	gint row,col,y,u;
	gint y_blended,u_blended,v_blended;
	gint temp;
	if (alpha<0)
		alpha=100;
	alpha=alpha/100;

	if(!copy){
		xc=x1;
		yc=y1;
	}

	gint rowc,colc,yy,uu,temp1;
	rowc=yc-len;
	colc=xc-lenn;

	for (row=y1-len ; row<=y1+len ; row++,rowc++)
	{
		if(row<0 || rowc<0)
			continue;
		if(row>height-1 || rowc>height-1)
			break;
		for(col=x1-lenn,colc=xc-lenn ; col<=x1+lenn ; col++,colc++)
		{
			if (col<0 || colc<0)
				continue;
			if(col>width-1 || colc>width-1)
				break;
			temp=0;
			if(row%2)
				temp=width/2;

			y = (row*width) + col ;
			u=(width*height) + ((row*width)/2 ) + col - temp;

			temp1=0;
			if(rowc%2)
				temp1=width/2;
			yy = (rowc*width) + colc ;
			uu=(width*height) + ((rowc*width)/2 ) + colc - temp1;

			if(fill)
				goto next;
			if (row == (y1-len) || row == (y1+len) || col == x1-lenn || col == x1+lenn)
			{
next:                  
				if (row >= (y1-len) || row <= (y1+len) || col >= x1-lenn || col <= x1+lenn)
				{

					if (copy)
						y_val = data[yy];		
					y_blended =((1-alpha) * data[y]) + (alpha * y_val); 
					data[y]=y_blended;   

					if(u%2)
						u-=1;
					if (uu%2)
						uu-=1;
					if (copy)
						u_val = data[uu++];
					u_blended = ((1-alpha) * data[u]) + (alpha * u_val);
					data[u++] = u_blended; 
					if (copy)
						v_val = data[uu];
					v_blended = ((1-alpha) * data[u]) + (alpha * v_val);
					data[u] = v_blended;

				}
			}
		}
	}

	return 0;

}

void er(gchar *a,GstDrawshapes *filter)
{
	g_critical("%s",a);
	filter->var.count+=1;
}

void check_func(GstDrawshapes *filter,guint width,guint height){
	filter->var.x1=0;
	filter->var.y1=0;
	filter->var.copy=0;
	filter->var.var1=90;
	//all defaults
	if(!filter->size){
		er("no shape size given",filter);
	}

	if(filter->coordinate[0] > width || filter->coordinate[1] > filter->var.height
			|| filter->coordinate[0] < 0  || filter->coordinate[1] < 0)
		(filter->mode) ? er("enter coordinates within the range ",filter) 
			:g_warning(" coordinates out of range, drawing on default values") ;

	if(!filter->coordinate[0] || !filter->coordinate[1]){
		filter->coordinate[0] = filter->var.width/2;
		filter->var.x = filter->var.width/2;
		filter->var.y = filter->var.height/2;
		filter->coordinate[1] = filter->var.height/2;
	}

	if(!filter->shape)
		filter->shape = "square" ;

	if(!filter->colour)
		filter->colour = "red" ;

	if(!filter->fill)
		filter->fill = "no-fill" ;

	if(!filter->mode)
		filter->mode=0;

	if(!filter->alpha)
		filter->alpha=100;

	if(!strcmp (filter->fill,"copy")){
		if(!filter->copy_coor[0] || !filter->copy_coor[1])
			er("no copy coordinates given",filter);
	}

	if(filter->copy_coor[0] && filter->copy_coor[1])
	{
		if(strcmp (filter->fill,"copy")){
			er("no fill mode given",filter);
		}
	}
	//defaults done

	if(filter->colour)
	{
		if (!strcmp(filter->colour,"red")){
			filter->var.y_val = 81 ;	
			filter->var.u_val = 90 ;
			filter->var.v_val = 240 ; 
		}

		else if (!strcmp(filter->colour,"blue")){
			filter->var.y_val = 41 ;	
			filter->var.u_val = 240 ;
			filter->var.v_val = 110 ; 
		}

		else if (!strcmp(filter->colour,"green")){
			filter->var.y_val =149; // 145 ;	
			filter->var.u_val = 43; //54 ;
			filter->var.v_val =21;// 34 ; 
		}
		else{
			(filter->mode) ? er("enter colour red, green, or blue",filter) 
				:g_warning("enter colour red , green, or blue ") ;
		}
	}

	if (filter->coordinate)
	{
		filter->var.x = filter->coordinate[0];
		filter->var.y = filter->coordinate[1];
		if (filter->var.x>filter->var.width || filter->var.y>filter->var.height)
		{
			filter->var.x = filter->var.width/2;
			filter->var.y = filter->var.height/2;
			(filter->mode) ? er("enter coordinates within the range ",filter) 
				:g_warning(" coordinates out of range, drawing on default values") ;
		}
	}

	if (filter->size)
	{	
		filter->var.len = filter->size;
		if (filter->var.len+filter->var.y > filter->var.height || filter->var.len+filter->var.x > width
				|| filter->var.y-filter->var.len < 0 || filter->var.x-filter->var.len < 0)
			(filter->mode) ? er("shape is going out of range",filter) 
				:g_warning("shape is going out of range ") ;

	}

	if (filter->fill)
	{
		//	fill = filter->fill ;
		if(!strcmp("no-fill",filter->fill))
			filter->var.fill=0;

		else if(!strcmp("color",filter->fill))
			filter->var.fill=1;

		else if(!strcmp("copy",filter->fill))
		{
			filter->var.fill=2;
			filter->var.copy=1;
			if (filter->copy_coor)
			{
				filter->var.x1 = filter->copy_coor[0];
				filter->var.y1 = filter->copy_coor[1];
				if (filter->var.x1>filter->var.width || filter->var.y1>filter->var.height )
				{
					filter->var.x1 = filter->var.width/2;
					filter->var.y1 = filter->var.height/2;
					(filter->mode) ? er("copy coordinates out of range",filter) 
						:g_warning("enter copy coordinates within the range ") ;
				}
			}

		}
		else{ 
			(filter->mode) ? er("enter appropriate fill mode",filter) 
				:g_warning("enter appropriate fill mode") ;
		}
	}

	if(filter->alpha)
	{
		filter->var.alpha = filter->alpha;
		if(filter->var.fill==0)
			filter->var.alpha = 100;

		
		if (filter->alpha > 100 || filter->alpha < 0){
		
			filter->var.alpha = 100;
			(filter->mode) ? er("enter alpha between 0 to 100",filter) 
				:g_warning("enter alpha between 0 to 100") ;
		}
	}


	if(filter->shape)
	{
		if (!strcmp(filter->shape,"square"))
			filter->shapee=0;

		else if (!strcmp(filter->shape,"rectangle"))
			filter->shapee=1;

		else if (!strcmp(filter->shape,"circle"))
			filter->shapee=2;

		else if(!strcmp(filter->shape,"triangle"))
			filter->shapee=3;		

		else
			filter->var.shape=4;
	}
}
/*
//cairo start
void cairo_draww(GstBuffer* buf,gint len,GstDrawshapes *filter)
{

	cairo_surface_t *surface;
	cairo_t *cr;
	
	gint xx=len;//+len/2;
	gint yy=len;//+len/2;
	
	
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 2*len,1.5*len);
//	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, filter->var.width, filter->var.height);
	cr = cairo_create (surface);
	cairo_set_line_width (cr,1);
	cairo_set_source_rgba (cr, 0, 0, 1,1);

//triangle(move to line to )	
	cairo_move_to(cr,xx,yy-len);
	cairo_line_to(cr,xx+(0.866*len),yy+(len/2));
//	cairo_move_to(cr,xx,yy-len);
	cairo_line_to(cr,xx-(0.866*len),yy+(len/2));
	cairo_line_to(cr,xx,yy-len);
//	cairo_line_to(cr,xx+(0.866*len),yy+(len/2));
	cairo_stroke(cr);
//	cairo_fill(cr);


//rectangle(cr,x,y,length,width)
//	cairo_rectangle (cr, 260, 240, 200, 200);
//	cairo_stroke (cr);
//

//arc(cr,x,y,radius,angle1,angle2)
//	cairo_arc(cr, 640, 720, 600, 0, 2 * M_PI);
//	cairo_stroke (cr);
//	cairo_fill(cr);


//	cairo_paint(cr);

	cairo_surface_flush (surface);
	unsigned char *data = cairo_image_surface_get_data(surface);
	//unsigned char
	gint r,g,b;
	gdouble a;
	gint Y,U,V,y,u;//v;
	gint temp;
	unsigned char *addr;
	GstMapInfo map;


cairo_surface_write_to_png(surface, "output.png");
		Y = 41;//145; // 0.257 * r + 0.504 * g + 0.098 * b +  16;
 		U =240;// 54;//-0.148 * r - 0.291 * g + 0.439 * b + 128;
  		V =110;// 34;// 0.439 * r - 0.368 * g - 0.071 * b + 128;
if (gst_buffer_map(buf, &map, GST_MAP_READWRITE)) {
		guint8 *dataa = map.data;
	
	for (gint i=0 ; i<filter->var.height ; i++){

		for (gint j=0 ; j<filter->var.width ; j++){
		
		addr=data;
		if(i<1.5*len && j<2*len)
			//addr = data + (filter->var.width*i*4) + (j*4);
			addr = data + (2*len*i*4) + (j*4);
		b=addr[0];
//		g=addr[1];
//		r=addr[2];
		a=addr[3];
//		g_print("alpha is %d  \t %d \n",b,addr[0]);
//	break;	
//	addr[0]=dataa[y];
//		addr[2]=255;
//		addr[0]=0;
//		addr[1]=0;
//		addr[3]=255;
		if (b==0)
			continue;
		a=a/255;
//		Y = 41; //145 ;// 0.257 * r + 0.504 * g + 0.098 * b +  16;
//		U =240; // 54 ;//-0.148 * r - 0.291 * g + 0.439 * b + 128;
//		V =110; // 34 ;// 0.439 * r - 0.368 * g - 0.071 * b + 128;
		//g_print("alpha is %f\n",a);	
		y = (i*(filter->var.width)) + j ;
		temp=0;
		if(i%2)
			temp=(filter->var.width)/2;
		u=((filter->var.width)*(filter->var.height)) + ((i*(filter->var.width))/2 ) + j - temp;
	
		dataa[y] = ((1-a)*dataa[y]) +  (a * Y) ;//     Y;
				//	y_blended =((1-alpha) * data[y]) + (alpha * y_val); 
		if(u%2)
			u--;
			//	{
		//		dataa[u++]=V;
		//		dataa[u]=U;
		//		continue;
		//	}
		//dataa[u++]=U;
		dataa[u] = ((1-a)*dataa[u]) +  (a * U) ;//     Y;
		u++;
		dataa[u] = ((1-a)*dataa[u]) +  (a * V) ;//     Y;
	//	dataa[u]=V;
	}
}
}
//	cairo_surface_flush (surface);
//cairo_surface_write_to_png(surface, "output_new.png");

}
//cairo endd
*/
	static GstFlowReturn
gst_drawshapes_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GST_DEBUG("I am only chain func\n");
	GstDrawshapes *filter;

	filter = GST_DRAWSHAPES (parent);
	//call the cairo function			
//	cairo_draww(buf,filter->var.len,filter);
	if (filter->var.count)
		return GST_FLOW_ERROR;	
	GstMapInfo map;
	if (gst_buffer_map(buf, &map, GST_MAP_READWRITE)) {
		//		guint size = map.size;
		guint8 *data = map.data;
		

		switch 	(filter->shapee){
			case 0:
				square_func(data, filter->var.width, filter->var.height, filter->var.x, filter->var.y, 
						filter->var.y_val,filter->var.u_val, filter->var.v_val, filter->var.len, 
						filter->var.len, filter->var.fill, filter->var.alpha, filter->var.copy, 
						filter->var.x1, filter->var.y1);
				break;
			case 1:
				rectangle_func(data, filter->var.width, filter->var.height, filter->var.x, filter->var.y,
					       	filter->var.y_val,filter->var.u_val, filter->var.v_val, filter->var.len,
					       	filter->var.len*2,filter->var.fill, filter->var.alpha, filter->var.copy,
					       	filter->var.x1, filter->var.y1);
				break;
			case 2:
				circle_func(data, filter->var.width, filter->var.height, filter->var.x, filter->var.y,
					       	filter->var.y_val,filter->var.u_val, filter->var.v_val, filter->var.len, 
						filter->var.len, filter->var.fill, filter->var.alpha, filter->var.copy, 
						filter->var.x1, filter->var.y1);
				break;
			case 3:
				triangle_func(data, filter->var.width, filter->var.height, filter->var.x, filter->var.y,
					       	filter->var.y_val, filter->var.u_val, filter->var.v_val,filter->var.len,
					       	filter->var.len, filter->var.fill, filter->var.alpha, filter->var.copy,
					       	filter->var.x1, filter->var.y1);
				break;
			case 4:
				square_func(data, filter->var.width, filter->var.height, filter->var.x, filter->var.y, 
						filter->var.y_val, filter->var.u_val, filter->var.v_val,filter->var.len,
					       	filter->var.len, filter->var.fill, filter->var.alpha, filter->var.copy,
					       	filter->var.x1, filter->var.y1);
				break;
			default :
				break;

		}


		gst_buffer_unmap(buf, &map);

}

	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
	static gboolean
drawshapes_init (GstPlugin * drawshapes)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template drawshapes' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_drawshapes_debug, "drawshapes",
			0, "Template drawshapes");

	return GST_ELEMENT_REGISTER (drawshapes, drawshapes);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstdrawshapes"
#endif

/* gstreamer looks for this stvarcture to register drawshapess
 *
 * exchange the string 'Template drawshapes' with your drawshapes description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		drawshapes,
		"drawshapes",
		drawshapes_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
