/*
 * main.cpp
 *
 *  Created on: 24/gen/2012
 *      Author: jetmir
 */

#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "FieldViewer.h"

using namespace std;
using namespace cv;


int main(int argc, char * argv[]) {
	/* On lit du fichier*/
	CvCapture * capture = cvCaptureFromAVI("/home/jetmir/out.avi.old");
	IplImage * img = cvQueryFrame(capture);
	FieldViewer *fieldViewer = new FieldViewer(img);
	fieldViewer->ShowVideo(capture);
	//fieldViewer->FindKephD(img);

	return 0;
}



