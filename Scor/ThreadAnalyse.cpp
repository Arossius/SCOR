/*
 * ThreadAnalyse.cpp
 *
 *  Created on: 11/feb/2012
 *      Author: jetmir
 */

#include "ThreadAnalyse.h"

ThreadAnalyse::ThreadAnalyse(Field *fieldC) {
	field = fieldC;
	moments = (CvMoments*) malloc(sizeof(CvMoments));
}

ThreadAnalyse::~ThreadAnalyse() {
	// TODO Auto-generated destructor stub
}

void ThreadAnalyse::run() {
	int img_step = field->img->widthStep;
	int img_channels = field->img->nChannels;
	uchar * img_data = (uchar *) field->img->imageData;
	char key = -1;

	field->treat->origin = field->img->origin;
	int filtered_step = field->treat->widthStep;
	int filtered_channels = field->treat->nChannels;
	uchar * filtered_data_b = (uchar *) field->treat->imageData;

	for (int i = 0; i < field->img->width; i++) {
		for (int j = 0; j < field->img->height; j++) {
			if (i > field->box[XMIN] && i < field->box[XMAX] && j
					> field->box[YMIN] && j < field->box[YMAX] && (img_data[j
					* img_step + i * img_channels]) > field->color[MINB]
					&& (img_data[j * img_step + i * img_channels])
							< field->color[MAXB] && (img_data[j * img_step + i
					* img_channels + 1]) > field->color[MING] && (img_data[j
					* img_step + i * img_channels + 1]) < field->color[MAXG]
					&& (img_data[j * img_step + i * img_channels + 2])
							> field->color[MINR] && (img_data[j * img_step + i
					* img_channels + 2]) < field->color[MAXR]) {
				filtered_data_b[j * filtered_step + i * filtered_channels]
						= 255;
			} else {
				filtered_data_b[j * filtered_step + i * filtered_channels] = 0;
			}
		}
	}

	//cvErode(field->treat, field->treat, 0, 1);

	cvMoments(field->treat, moments, 1);

	// The actual moment values B
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);

	field->position->x = (int) moment10 / area;
	field->position->y = (int) moment01 / area;

	pthread_exit(NULL);
}

void *ThreadAnalyse::exec(void *analyse_thread) {
	ThreadAnalyse* thread = (ThreadAnalyse*) analyse_thread;
	thread->run();
	return 0;
}

pthread_t ThreadAnalyse::Launch() {
	pthread_create(&thread, NULL, ThreadAnalyse::exec, this);
	return thread;
}
