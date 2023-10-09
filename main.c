#include "switching_pipe.h"


int main(int argc, char **argv) {
	GstElement	*pipeline, *rtsp, *file, *bin, *sink, *file1, *conv;
	GstPad		*bin_pad;
	GstBus *bus;
	GstMessage *msg;
	int			switch_time = 0;
	gboolean terminate = FALSE;
	//проверка на правильность агрументов
	//адрес сервера, путь к файлу, интервал переключения

	 gst_init(&argc, &argv);
	if (argc < 4) {
		printf("Недостаточно аргументов.\n"); //стоит написать в каком порядке?
		exit(0);
	}
	switch_time = atoi(argv[3]);
	printf("input args are checked\n");
	//сделать проверку доступности ртсп и файла?
	if (pipeline = gst_pipeline_new("pipeline"))
		printf("pipe is created\n");
	else {
		printf("no pipe created\n");
		return 1;
	}

	// rtsp = gst_element_factory_make("rtspsrc", NULL);
	file = gst_element_factory_make("filesrc", "file_src");
	file1 = gst_element_factory_make("filesrc", "file_src_1");
	bin = gst_element_factory_make("decodebin", "decode_bin");
	sink = gst_element_factory_make("ximagesink", "sink");
	conv = gst_element_factory_make("videoconvert", "conv");
	if (!pipeline || !file || !bin || !conv || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }
	// gst_util_set_object_arg (G_OBJECT(rtsp), "location", argv[1]);
	// gst_util_set_object_arg (G_OBJECT(file), "location", argv[2]);
	gst_util_set_object_arg (G_OBJECT(file1), "location", argv[1]);
	printf("arguments are set\n");

	// gst_bin_add_many(GST_BIN(pipeline), rtsp, file, bin, sink, NULL);
	gst_bin_add_many(GST_BIN(pipeline), file1, bin, conv, sink, NULL);
	GST_ERROR("elements added to bin\n");
	// gst_element_link_many(rtsp, bin, sink, NULL);
	// gst_element_link_many(file1, bin, conv, sink, NULL)
		// GST_ERROR("linked succesfully");
	if (!gst_element_link(file1, bin))
		GST_ERROR("no link 12");

	if (!gst_element_link(conv, sink))
		GST_ERROR("no link 34");

	if (gst_element_set_state(pipeline,
			GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
		g_error ("Pipe is failed to start\n");
		return 1;
	}

	
	while (!gst_element_link(bin, conv)) {
		GST_ERROR("no link 23");
		g_usleep(100000);
	}
    
	bus = gst_element_get_bus (pipeline);
	do {
		msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
			GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

		/* Parse message */
		if (msg != NULL) {
		GError *err;
		gchar *debug_info;

		switch (GST_MESSAGE_TYPE (msg)) {
			case GST_MESSAGE_ERROR:
			gst_message_parse_error (msg, &err, &debug_info);
			g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
			g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error (&err);
			g_free (debug_info);
			terminate = TRUE;
			break;
			case GST_MESSAGE_EOS:
			g_print ("End-Of-Stream reached.\n");
			terminate = TRUE;
			break;
			case GST_MESSAGE_STATE_CHANGED:
			/* We are only interested in state-changed messages from the pipeline */
			if (GST_MESSAGE_SRC (msg) == GST_OBJECT (pipeline)) {
				GstState old_state, new_state, pending_state;
				gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
				g_print ("Pipeline state changed from %s to %s:\n",
					gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
			}
			break;
			default:
			/* We should not reach here */
			g_printerr ("Unexpected message received.\n");
			break;
		}
		gst_message_unref (msg);
		}
	} while (!terminate);

	/* Free resources */
	gst_object_unref (bus);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (pipeline);
	return 0;
	// sleep(switch_time);
	// GST_ERROR("begining to change src\n");
	// gst_element_set_state (rtsp, GST_STATE_NULL);
	// // printf("unlink rtsp\n");
	// // gst_element_unlink(rtsp, bin, NULL);
	// GST_ERROR("unlink file1\n");
	// gst_element_unlink(file1, bin);
	// printf("link file\n");
	// gst_element_link(file, bin);
	// printf("play\n");
	// gst_element_set_state(file, GST_STATE_PLAYING);
	// printf("Ввод сменен успешно.\n");
}