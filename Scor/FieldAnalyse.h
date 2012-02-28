/*
 * FieldAnalyse.h
 *
 *  Created on: 12/feb/2012
 *      Author: jetmir
 */

#ifndef FIELDANALYSE_H_
#define FIELDANALYSE_H_

#include <iostream>
#include <math.h>
#include <mqueue.h>
#include "cv.h"
#include "highgui.h"
#include "Utils.h"
#include "ThreadAnalyse.h"

#define BALL_F 5

using namespace std;
using namespace cv;

class FieldAnalyse {
public:
	FieldAnalyse();
	virtual ~FieldAnalyse();
	void Launch();
protected:
	/* Boîte aux lettres de communication entre Com et Robot */
	mqd_t bal_com_robot;
	/* Boîte aux lettres de communication entre Video et Robot */
	mqd_t bal_video_robot;
	/* Boîte aux lettres de communication entre Robot et IA */
	mqd_t bal_robot_ia;
	CvMat* mmat;

	Position balle_h[5];
	bool buffer_full;
	int ball_frame;
	int white[6], blue[6], red[6];
	ThreadAnalyse *kephD_t, *kephG_t, *balle_t;
	Field kephD, kephG, balle;
	CvCapture * capture;
	IplImage *img, *warped, *treat,*terrain;
	Robot r1,r2;
	Balle palla;
	void Allocate_imgs();
	void compute_Warp();
	void setup_colors(char *color, int *color_r);
	/* Identifiant du thread */
	pthread_t thread;
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* analyse_thread);
};

#endif /* FIELDANALYSE_H_ */
