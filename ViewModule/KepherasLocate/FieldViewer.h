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
#include "cv.h"
#include "highgui.h"

#define PFILENAME "points.txt"
#define PI 3.14159265

using namespace std;
using namespace cv;

typedef struct KephPos{
	CvPoint posB,posW;
	CvPoint prevB,prevW;
	float_t angle,prevA;
} KephPos;

class FieldViewer {
private:
	IplImage *img, *blue, *leftK, *rightK, *warped, *tmpIm, *white;
	CvMat* mmat;
	CvMoments * moments;
	KephPos kephG, kephD;
	void Allocate_imgs();
	void compute_Warp(IplImage * img);
public:
	FieldViewer(IplImage * img);
	KephPos FindKeph(IplImage *imgL, int x1, int x2, int y1, int y2);
	void FindKG();
	void FindKD();
	void ShowVideo(CvCapture * capture);
	virtual ~FieldViewer();
};

#endif /* FIELDVIEWER_H_ */
