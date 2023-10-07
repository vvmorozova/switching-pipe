#include "switching_pipe.h"

int main(int argc, char **argv) {
	GstElement	*pipeline, *rtsp, *file, *bin, *sink, *file1, *conv;
	int			switch_time = 0;
	//проверка на правильность агрументов
	//адрес сервера, путь к файлу, интервал переключения

	if (argc < 4) {
		printf("Недостаточно аргументов.\n"); //стоит написать в каком порядке?
		exit(0);
	}
	switch_time = atoi(argv[3]);
	printf("input ergs are checked\n");
	//сделать проверку доступности ртсп и файла?
	pipeline = gst_pipeline_new("pipeline");
	printf("pipe is created\n");
	// rtsp = gst_element_factory_make("rtspsrc", NULL);
	file = gst_element_factory_make("filesrc", NULL);
	file1 = gst_element_factory_make("filesrc", NULL);
	bin = gst_element_factory_make("decodebin", NULL);
	sink = gst_element_factory_make("ximagesink", NULL);
	conv = gst_element_factory_make("videoconvert", NULL);
	printf("elems are created\n");
	// gst_util_set_object_arg (G_OBJECT(rtsp), "location", argv[1]);
	gst_util_set_object_arg (G_OBJECT(file), "location", argv[2]);
	gst_util_set_object_arg (G_OBJECT(file1), "location", argv[1]);
	printf("arguments are set\n");

	// gst_bin_add_many(GST_BIN(pipeline), rtsp, file, bin, sink, NULL);
	gst_bin_add_many(GST_BIN(pipeline), file1, file, bin, conv, sink, NULL);
	printf("elements added to bin\n");
	// gst_element_link_many(rtsp, bin, sink, NULL);
	if (gst_element_link_many(file1, bin, conv, sink, NULL))
		printf("linked succesfully");

	if (gst_element_set_state(pipeline,
			GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
		g_error ("Pipe is failed to start\n");
		return 1;
	}
	sleep(switch_time);
	printf("beging to change src\n");
	gst_element_set_state (rtsp, GST_STATE_NULL);
	// printf("unlink rtsp\n");
	// gst_element_unlink(rtsp, bin, NULL);
	printf("unlink file1\n");
	gst_element_unlink(file1, bin);
	printf("link file\n");
	gst_element_link(file, bin);
	printf("play\n");
	gst_element_set_state(file, GST_STATE_PLAYING);
	printf("Ввод сменен успешно.\n");
}