#include "switching_pipe.h"

static GMainLoop *loop;

typedef struct PipelineData {
	GstElement	*pipeline;
	GstElement *bin;
	GstElement *conv;
	GstElement *bin1;
	GstElement *conv1;
	GstElement *sink;
} PipeData;

gboolean is_source1_active = 1;

static void pad_added_handler (GstElement *src, GstPad *new_pad, PipeData *data) {
	GstPad *sink_pad; 
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

	if (is_source1_active)
		sink_pad = gst_element_get_static_pad (data->conv1, "sink");
	// else
	// 	sink_pad = gst_element_get_static_pad (data->conv, "sink");
	g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

	/* If our converter is already linked, we have nothing to do here */
	if (gst_pad_is_linked (sink_pad)) {
		g_print ("We are already linked. Ignoring.\n");
		goto exit;
	}

	/* Check the new pad's type */
	new_pad_caps = gst_pad_get_current_caps (new_pad);
	new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
	new_pad_type = gst_structure_get_name (new_pad_struct);
	if (!g_str_has_prefix (new_pad_type, "video/x-raw")) {
		g_print ("It has type '%s' which is not raw video. Ignoring.\n", new_pad_type);
		goto exit;
	}

	/* Attempt the link */
	ret = gst_pad_link (new_pad, sink_pad);
	if (GST_PAD_LINK_FAILED (ret)) {
		g_print ("Type is '%s' but link failed.\n", new_pad_type);
	} else {
		g_print ("Link succeeded (type '%s').\n", new_pad_type);
	}

	exit:
	/* Unreference the new pad's caps, if we got them */
	if (new_pad_caps != NULL)
		gst_caps_unref (new_pad_caps);

	/* Unreference the sink pad */
	gst_object_unref (sink_pad);
}


static gboolean switch_sources(gpointer data) {
    GstElement *pipeline = GST_ELEMENT(data);

    // Изменяем источник видео путем переключения между двумя filesrc элементами
    GstElement *filesrc1 = gst_bin_get_by_name(GST_BIN(pipeline), "file_src");
    GstElement *filesrc2 = gst_bin_get_by_name(GST_BIN(pipeline), "file_src_1");
  	is_source1_active = !is_source1_active;

    if (is_source1_active) {
        gst_element_set_state(filesrc1, GST_STATE_NULL);
        gst_element_set_state(filesrc2, GST_STATE_PLAYING);
    } else {
        gst_element_set_state(filesrc1, GST_STATE_PLAYING);
        gst_element_set_state(filesrc2, GST_STATE_NULL);
    }

    return G_SOURCE_CONTINUE;
}

int main(int argc, char **argv) {
	PipeData	data;
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
	if (data.pipeline = gst_pipeline_new("pipeline"))
		printf("pipe is created\n");
	else {
		printf("no pipe created\n");
		return 1;
	}
	data.bin = gst_element_factory_make("uridecodebin", "uridecode_bin");
	data.bin1 = gst_element_factory_make("uridecodebin", "uridecode_bin1");
	data.sink = gst_element_factory_make("xvimagesink", "sink"); 
	data.conv = gst_element_factory_make("autovideoconvert", "conv");
	data.conv1 = gst_element_factory_make("autovideoconvert", "conv1");
	if (!data.pipeline || !data.bin || !data.conv || !data.bin1 || !data.conv1 || !data.sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }
    char *uri, *tmp;
    char* path = realpath(argv[2], NULL);
    if(path == NULL){
        printf("cannot find file with name[%s]\n", argv[2]);
        return 0;
    } else{
        printf("path[%s]\n", path);
        // free(path);
    }
    GST_ERROR("Write uri\n");
    uri = (char *)malloc(strlen("file://") + strlen(path) + 1);
    strcpy(uri, "file://");
    strcat(uri, path);
    GST_ERROR("uri = %s\n", uri);
	
    gst_util_set_object_arg (G_OBJECT(data.bin1), "uri", uri);
    gst_util_set_object_arg (G_OBJECT(data.bin), "uri", argv[1]);
	printf("arguments are set\n");

	gst_bin_add_many(GST_BIN(data.pipeline), data.bin1, data.conv1, data.sink, NULL);
	GST_ERROR("elements added to bin\n");


	if (is_source1_active) {
		// if (!gst_element_link(data.file1, data.bin1))
			GST_ERROR("no link 12");
		
		if (!gst_element_link(data.conv1, data.sink))
			GST_ERROR("no link 45");
		g_signal_connect (data.bin1, "pad-added", G_CALLBACK (pad_added_handler), &data);
		gst_element_set_state(data.bin, GST_STATE_NULL);
		gst_element_set_state(data.conv, GST_STATE_NULL);
		gst_element_set_state(data.bin1, GST_STATE_PLAYING);
		gst_element_set_state(data.conv1, GST_STATE_PLAYING);
		// gst_element_set_state(data.sink, GST_STATE_PLAYING);
		gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	}
	// g_timeout_add(switch_time, switch_sources, data.pipeline);
	if (is_source1_active) {
		gst_element_set_state(data.bin1, GST_STATE_PLAYING);
        GstPad* bin1_src_pad = gst_element_get_static_pad(data.bin1, "src");
		gst_pad_activate_mode (bin1_src_pad, GST_PAD_MODE_PUSH, TRUE);
        gst_element_link(data.bin1, data.conv1);
        gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
		// gst_element_link(data.bin1, data.capsfilter1);
		// gst_element_link(data.bin1, data.conv1);
	    GST_DEBUG_BIN_TO_DOT_FILE(data.pipeline, GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
		}
	GST_WARNING("Running ...\n");
	loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

	gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(data.pipeline));
    g_main_loop_unref(loop);
}