/*
 * FieldViewer.h
 *
 *  Created on: 24/gen/2012
 *      Author: jetmir
 */

#ifndef FIELDVIEWER_H_
#define FIELDVIEWER_H_

#include <stdio.h>
#include <math.h>
#include <mqueue.h>
#include "Utils.h"
#include "cv.h"
#include "highgui.h"

#define PFILENAME "points.txt"
#define PI 3.14159265

using namespace std;
using namespace cv;

typedef struct Position {
	CvPoint posB, posW;
	CvPoint prevB, prevW;
	float angle, prevA;
} KephPos;

class FieldViewer {

public:

	FieldViewer();

	/* Lance un nouveau thread */
	void Launch();

	virtual ~FieldViewer();

protected:
	/* Boîte aux lettres de communication entre Com et Robot */
	mqd_t bal_com_robot;
	/* Boîte aux lettres de communication entre Video et Robot */
	mqd_t bal_video_robot;
	/* Boîte aux lettres de communication entre Robot et IA */
	mqd_t bal_robot_ia;

	CvCapture * capture;
	IplImage *img, *blue, *leftK, *rightK, *warped, *tmpIm, *white, *red;
	CvMat* mmat;
	CvMoments * moments;
	Position kephG, kephD, ball;
	Robot kephR, kephL;
	Balle balle;

	void Allocate_imgs();
	void compute_Warp(IplImage * img);
	void setupColors(IplImage * img);
	Position FindKeph(IplImage *imgL, int x1, int x2, int y1, int y2);
	void FindBall(IplImage *imgL);
	void FindKG();
	void FindKD();
	void ShowVideo();
	/* Identifiant du thread */
	pthread_t thread;
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* video_thread);
};

#endif /* FIELDVIEWER_H_ */
