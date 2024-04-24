#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 240
#define DEFAULT_FRAMERATE 30
#define DEFAULT_FORMAT "NV12"

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

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
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
main (int   argc,
      char *argv[])
{
  GMainLoop *loop;

  GstElement *pipeline, *source, *cropportion, *sink;
  GstBus *bus;
  guint bus_watch_id;

  /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);
  /* Create gstreamer elements */
  pipeline = gst_pipeline_new ("video-crop");
  source   = gst_element_factory_make ("videotestsrc", "video-source");
  cropportion  = gst_element_factory_make ("cropportion", "cropportion");
  sink     = gst_element_factory_make ("autovideosink", "video-output");

  if (!pipeline || !source || !cropportion || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }

  int width, height;
  char *format;
  int rate;
  if (strcmp ("NA", argv[1]))
  	format = argv[1];
  else
  	format = DEFAULT_FORMAT;

  if (strcmp ("NA", argv[2])) 
  	width = atoi (argv[2]);
  else
  	width = DEFAULT_WIDTH;

  if (strcmp ("NA", argv[3])) 
  	height = atoi (argv[3]);
  else
  	height = DEFAULT_HEIGHT;

  if (strcmp ("NA", argv[4])) 
  	rate = atoi (argv[4]);
  else
  	rate = DEFAULT_FRAMERATE;

  GstCaps *caps;
  caps = gst_caps_new_simple ("video/x-raw",
	       "format", G_TYPE_STRING, format,
	       "width", G_TYPE_INT, width,
	       "height", G_TYPE_INT, height,
	       "framerate", GST_TYPE_FRACTION, rate, 1,
	       NULL);

  if (strcmp ("NA", argv[5])) {
	  int shape_size;
	  shape_size = atoi (argv[5]);
	  GValue size = G_VALUE_INIT;
	  g_value_init (&size, G_TYPE_INT);
	  g_value_set_int (&size, shape_size);
	  g_object_set_property (G_OBJECT (cropportion), "shape-size", &size);
  }

  if (strcmp ("NA", argv[6])) {
	  int x_cor, y_cor;
	  GValue *crop_coordinates = g_new0 (GValue, 1);
	  g_value_init (crop_coordinates, GST_TYPE_ARRAY);
	  char *token;
    	  token = strtok(argv[6], ",");
     
          x_cor = atoi(token); // Convert token to integer
          token = strtok(NULL, ","); // Get next token
	  y_cor = atoi (token);

	  GValue x = G_VALUE_INIT;
	  g_value_init (&x, G_TYPE_INT);
	  g_value_set_int (&x, x_cor);
	  gst_value_array_append_and_take_value (crop_coordinates, &x);

  	  GValue y = G_VALUE_INIT;
 	  g_value_init (&y, G_TYPE_INT);
  	  g_value_set_int (&y, y_cor);
  	  gst_value_array_append_and_take_value (crop_coordinates, &y);

  	  g_object_set_property (G_OBJECT (cropportion), "crop-coordinate", crop_coordinates);
  	  g_free (crop_coordinates);
  }


  if (strcmp ("NA", argv[7])) {
	  int index;
	  index = atoi (argv[7]);
	  GValue shape = G_VALUE_INIT;
	  g_value_init (&shape, G_TYPE_ENUM);
	  g_value_set_enum (&shape, index); 
	  g_object_set_property (G_OBJECT (cropportion), "shape", &shape);
  }
  
  if (strcmp ("NA", argv[8])) {
	  int color;
	  color = atoi (argv[8]);
	  GValue fill_color = G_VALUE_INIT;
	  g_value_init (&fill_color, G_TYPE_ENUM);
	  g_value_set_enum (&fill_color, color); 
	  g_object_set_property (G_OBJECT (cropportion), "fill-color", &fill_color);
  }

  if (strcmp ("NA", argv[9])) {
	  GValue *resolution = g_new0 (GValue, 1);
	  g_value_init (resolution, GST_TYPE_ARRAY);

	  int scale_w, scale_h;

	  char *token;
    	  token = strtok(argv[9], ",");
     
          scale_w = atoi(token); // Convert token to integer
          token = strtok(NULL, ","); // Get next token
	  scale_h = atoi (token);

	  GValue width = G_VALUE_INIT;
	  g_value_init (&width, G_TYPE_INT);
	  g_value_set_int (&width, scale_w);
	  gst_value_array_append_and_take_value (resolution, &width);

	  GValue height = G_VALUE_INIT;
	  g_value_init (&height, G_TYPE_INT);
	  g_value_set_int (&height, scale_h);
	  gst_value_array_append_and_take_value (resolution, &height);

	  g_object_set_property (G_OBJECT (cropportion), "scale-size", resolution);
	  g_free (resolution);
  }

  if (strcmp ("NA", argv[10])) {
	  int mode;
	  mode = atoi (argv[10]);
	  GValue strict_mode = G_VALUE_INIT;
	  g_value_init (&strict_mode, G_TYPE_BOOLEAN);
	  g_value_set_boolean (&strict_mode, mode);
	  g_object_set_property (G_OBJECT (cropportion), "strict-mode", &strict_mode);
  }
  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, cropportion, sink, NULL);

  /* we link the elements together */
  /* video-test-source -> cropportion -> autovideosink */
  
  gst_element_link_filtered (source, cropportion, caps);
  gst_element_link (cropportion, sink);
  gst_caps_unref (caps);
  
  /* Set the pipeline to "playing" state*/
  //g_print ("Now playing: %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);


  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);


  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
}

