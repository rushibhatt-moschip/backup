#include "gst/gst.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define MAX_LINE_LENGTH 256


static gboolean print_fps(gpointer sink) {
    gchar *last_message;
    g_object_get(sink, "last-message", &last_message, NULL);
    gchar **tokens = g_strsplit(last_message, " ", -1);
            gint rendered_frames = 0;
    for (int i = 0; tokens[i] != NULL; i++) {
            if (g_str_has_prefix(tokens[i], "rendered:")) {
                rendered_frames = atoi(tokens[i+1]);
                g_print("rushi\n\nRendered frames: %d\n", rendered_frames);
                break;
            }
        }
    if (last_message != NULL) {
        g_print("%s\n", last_message);
        g_free(last_message);
    }
    return TRUE; // Return TRUE to keep the timer running
}

int var=0;

	static gboolean
bus_call (GstBus     *bus,
		GstMessage *msg,
		gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {

		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;

		case GST_MESSAGE_ERROR:
		       	{
				gchar  *debug;
				GError *error;
				gst_message_parse_error (msg, &error, &debug);
				g_free (debug);
			//	g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
			//	g_printerr("Debugging information: %s\n", debug ? debug : "none");
			//	g_printerr ("Error: %s\n", error->message);
				if (!strcmp(error->message,"Internal data stream error."))
					var=0;
				else
					var=1;
				g_error_free (error);
				g_main_loop_quit (loop);
				break;
			}
		default:
			break;
	}

	return TRUE;
}

	int
main (int   argc, char *argv[])
{
	GMainLoop *loop;

	GstElement *pipeline, *videotestsrc, *drawshapes, *autovideosink;
	GstBus *bus;
	guint bus_watch_id;

	/* Initialisation */
	gst_init (&argc, &argv);

	/* Check input arguments */
	// if (argc != 2) {
	// g_printerr ("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
	// return -1;
	// }

	//reults openening
	FILE *results= fopen("result.csv","r");
	if(results==NULL)
	{
		results= fopen("result.csv","w");
		fprintf(results, "width,height,format,framerate,shape-size,"
				"x-coordinate, y-coordinate,shape,shape-color,"
				"fill-mode,x-copy_ coordinate, y-copy_coordinate,"
				"alpha,strict-mode,results\n");
		fclose(results);
	}

	//file openening
	FILE *file = fopen("testcases.csv", "r");

	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	int row;
	printf("Enter the test case: ");
	scanf("%d", &row);

	char line[MAX_LINE_LENGTH];
	char *token;
	int current_row = 0;

	char fil_mode[MAX_LINE_LENGTH], color[MAX_LINE_LENGTH], name[MAX_LINE_LENGTH], format[MAX_LINE_LENGTH];
	int x_cor, y_cor, x_copy, y_copy, size, alpha, rate, width, height;
	bool strict;

	// Read and discard lines until reaching the desired row
		while (current_row < row && fgets(line, sizeof(line), file) != NULL) {
		current_row++;
		}

		if (current_row != row) {
		printf("Row %d not found in the file.\n", row);
		fclose(file);
		return 1;
		}
			

//	fgets(line, sizeof(line), file);
	
//	while(fgets(line, sizeof(line), file) != NULL){	

		loop = g_main_loop_new (NULL, FALSE);

		pipeline       = gst_pipeline_new ("shapes-drawing");
		videotestsrc   = gst_element_factory_make ("videotestsrc","source");
		drawshapes     = gst_element_factory_make ("drawshapes",  "draw");
		autovideosink  = gst_element_factory_make ("fpsdisplaysink", "output");

		if (!pipeline || !videotestsrc ||!drawshapes || !autovideosink) {
			g_printerr ("One element could not be created. Exiting.\n");
			return -1;
		}

		token = strtok(line, ",");

		if (token != NULL) {
			width = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			height = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			strcpy(format, token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			rate=atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			size = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			x_cor = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			y_cor = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			strcpy(name, token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			strcpy(color, token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			strcpy(fil_mode, token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			x_copy = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			y_copy = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			alpha = atoi(token);
		}

		token = strtok(NULL, ",");
		if (token != NULL) {
			strict = atoi(token);
		}

	//	printf("%d %d %d %d\n", x_cor, y_cor, width, height);

		//caps settingg
		GstCaps *caps;
		caps = gst_caps_new_simple ("video/x-raw",
				"format", G_TYPE_STRING, format,
				"width", G_TYPE_INT, width,
				"height", G_TYPE_INT, height,
				"framerate", GST_TYPE_FRACTION, rate, 1,
				NULL);

		// property setting 
		
		//size
		GValue shape_size = G_VALUE_INIT;
		g_value_init (&shape_size, G_TYPE_INT);
		g_value_set_int (&shape_size, size);
		g_object_set_property (G_OBJECT (drawshapes), "shape-size", &shape_size);
		g_value_unset(&shape_size);  // Clean up the GValue


		//draw coordinate
		GValue *draw_coordinate = g_new0 (GValue, 1);
		g_value_init (draw_coordinate, GST_TYPE_ARRAY);

		GValue x = G_VALUE_INIT;
		g_value_init (&x, G_TYPE_INT);
		g_value_set_int (&x, x_cor);
		gst_value_array_append_and_take_value (draw_coordinate, &x);

		GValue y = G_VALUE_INIT;
		g_value_init (&y, G_TYPE_INT);
		g_value_set_int (&y, y_cor);
		gst_value_array_append_and_take_value (draw_coordinate, &y);

		g_object_set_property (G_OBJECT (drawshapes), "draw-coordinate", draw_coordinate);
		g_free (draw_coordinate);


		//shape name
		GValue shape_name = G_VALUE_INIT;
		g_value_init (&shape_name,G_TYPE_STRING);
		g_value_set_string (&shape_name,name);
		g_object_set_property (G_OBJECT (drawshapes), "shape", &shape_name);


		//shape color
		GValue shape_color = G_VALUE_INIT;
		g_value_init (&shape_color,G_TYPE_STRING);
		g_value_set_string (&shape_color,color);
		g_object_set_property (G_OBJECT (drawshapes), "shape-color", &shape_color);


		//fill mode
		GValue fill_mode = G_VALUE_INIT;
		g_value_init (&fill_mode,G_TYPE_STRING);
		g_value_set_string (&fill_mode,fil_mode);
		g_object_set_property (G_OBJECT (drawshapes), "fill-mode", &fill_mode);



		//copy coordinates
		GValue *copy_coordinate = g_new0 (GValue, 1);
		g_value_init (copy_coordinate, GST_TYPE_ARRAY);

		GValue xc = G_VALUE_INIT;
		g_value_init (&xc, G_TYPE_INT);
		g_value_set_int (&xc, x_copy);
		gst_value_array_append_and_take_value (copy_coordinate, &xc);

		GValue yc = G_VALUE_INIT;
		g_value_init (&yc, G_TYPE_INT);
		g_value_set_int (&yc, y_copy);
		gst_value_array_append_and_take_value (copy_coordinate, &yc);

		g_object_set_property (G_OBJECT (drawshapes), "copy-coordinate", copy_coordinate);
		g_free (copy_coordinate);


		//alpha
		GValue shape_alpha = G_VALUE_INIT;
		g_value_init (&shape_alpha, G_TYPE_INT);
		g_value_set_int (&shape_alpha, alpha);
		g_object_set_property (G_OBJECT (drawshapes), "alpha", &shape_alpha);


		//strict mode
		g_object_set(drawshapes, "strict-mode", strict, NULL);  // Simpler way to set properties

 		//fps sink property set 
		//g_object_set(autovideosink, "text-overlay", 0, NULL);


		/* we add a message handler */
		bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
		bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
		gst_object_unref (bus);

		/* we add all elements into the pipeline */
		gst_bin_add_many (GST_BIN (pipeline),
				videotestsrc, drawshapes, autovideosink, NULL);

		//  gst_element_link_many(videotestsrc,drawshapes,autovideosink,NULL);
		gst_element_link_filtered (videotestsrc, drawshapes, caps);
		gst_element_link (drawshapes, autovideosink);
		gst_caps_unref (caps);
		//  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), decoder);


		/* Set the pipeline to "playing" state*/
	//	g_print ("Now playing: \n"); //argv[1]);
		gst_element_set_state (pipeline, GST_STATE_PLAYING);
	    	
		g_timeout_add_seconds(1, (GSourceFunc)print_fps, autovideosink);


		/* Iterate */
		g_print ("Running...\n");
		g_main_loop_run (loop);


		char *array[]={"FAIL","PASS"};
		results= fopen("result.csv","a");
		fprintf(results, "%d,%d,%s,%d/1,"
				"%d, %d, %d, %s,"
				"%s, %s, %d, %d, %d, %d, %s\n", width, height, format, rate, size, 
				x_cor, y_cor, name, color,fil_mode, x_copy, y_copy, alpha, strict, array[var]);


		/* Out of the main loop, clean up nicely */
		g_print ("Returned, stopping playback\n");
		gst_element_set_state (pipeline, GST_STATE_NULL);

		g_print ("Deleting pipeline\n");
		gst_object_unref (GST_OBJECT (pipeline));
		g_source_remove (bus_watch_id);
		g_main_loop_unref (loop);
//	}
	fclose(file);
	fclose(results);
	return 0;
}

