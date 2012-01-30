/*
 * FieldViewer.cpp
 *
 *  Created on: 24/gen/2012
 *      Author: jetmir
 */

#include "FieldViewer.h"

CvPoint p_corners[4];
bool waiting_corners;
int corners;
int mB_val = 50, mW_val = 39, mR_val = 100;

void on_mouse(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		p_corners[corners].x = x;
		p_corners[corners].y = y;
		corners++;
		waiting_corners = corners < 4;
		printf("mouse click x = %d  y = %d\n", x, y);
	}
}

void onTrackbarSlideB(int pos) {
	mB_val = pos;
}

void onTrackbarSlideW(int pos) {
	mW_val = pos;
}

void warpImage(IplImage * imgL, IplImage * tmpIm, CvMat * mmat) {
	try {
		cvWarpPerspective(imgL, tmpIm, mmat);
	} catch (Exception ex) {
		cout << "Exception in performWarp()\n";
	}
}

void FieldViewer::Allocate_imgs() {
	const int w = img->width;
	const int h = img->height;
	leftK = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	rightK = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	blue = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	white = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	tmpIm = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
	warped = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
}

void FieldViewer::ShowVideo(CvCapture * capture) {
	cvNamedWindow("ShowVid", CV_WINDOW_NORMAL);
	cvShowImage("ShowVid", warped);
	while (true) {
		img = cvQueryFrame(capture);
		warpImage(img, warped, mmat);
		FindKD();
		FindKG();

		CvFont font;
		double hScale = 1.0;
		double vScale = 1.0;
		int lineWidth = 1;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale,
				vScale, 0, lineWidth);
		char v[40] = { 0 };
		char v2[40] = { 0 };
		sprintf(v, "%s %.2f", "K1", kephD.angle);
		sprintf(v2, "%s %.2f", "K2", kephG.angle);

		cvPutText(warped, v, kephD.posW, &font, cvScalar(255, 255, 0));
		cvPutText(warped, v2, kephG.posB, &font, cvScalar(255, 255, 0));
		cvShowImage("ShowVid", warped);
		cvWaitKey(1000 / 15);
	}
	cvDestroyWindow("ShowVid");
}

void FieldViewer::FindKD() {
	kephD = FindKeph(warped, 0, warped->height / 2, warped->width / 2,
			warped->width);
	float m, mP;
	if ((kephD.posW.x - kephD.posB.x))
		m = (kephD.posW.y - kephD.posB.y) / (kephD.posW.x - kephD.posB.x);
	if (m != 0)
		mP = -1 / m;
	kephD.angle = atan(mP) * 180 / PI;
}

void FieldViewer::FindKG() {
	kephG = FindKeph(warped, warped->height / 2, warped->height, warped->width
			/ 2, warped->width);
	float m, mP;
	if ((kephG.posW.x - kephG.posB.x))
		m = (kephG.posW.y - kephG.posB.y) / (kephG.posW.x - kephG.posB.x);
	if (m != 0)
		mP = -1 / m;
	try {
		kephG.angle = atan(mP) * 180 / PI;
	} catch (Exception ex) {
		cout << ex.err;
	}
}

FieldViewer::FieldViewer(IplImage * imgL) {
	corners = 0;
	waiting_corners = true;
	moments = (CvMoments*) malloc(sizeof(CvMoments));
	img = imgL;
	Allocate_imgs();
	compute_Warp(img);
}

KephPos FieldViewer::FindKeph(IplImage *imgL, int x1, int x2, int y1, int y2) {
	/*cvNamedWindow("KephD", CV_WINDOW_AUTOSIZE);

	 IplImage *tpl = cvLoadImage("/home/jetmir/red-ball.jpg");

	 IplImage *res = cvCreateImage(cvSize(imgL->width - tpl->width + 1,
	 imgL->height - tpl->height + 1), IPL_DEPTH_32F, 1);

	 perform template matching
	 cout << imgL->width << "," << imgL->height << "\n";
	 cout << tpl->width << "," << tpl->height << "\n";
	 cout << res->width << "," << res->height << "\n";

	 cvCopyImage(imgL,img);
	 try {
	 cvWarpPerspective(img, warped, mmat);
	 } catch (Exception ex) {
	 cout << "Exception in performWarp()\n";
	 }

	 //	CvRect rect = cvRect(warped->width/2, 0, warped->width, warped->height/2);

	 //	cvSetImageROI(warped, rect);

	 cvMatchTemplate(warped, tpl, res, CV_TM_SQDIFF);
	 find best matches location
	 CvPoint minloc, maxloc;
	 double minval, maxval;
	 cvMinMaxLoc(res, &minval, &maxval, &minloc, &maxloc, 0);

	 draw rectangle
	 cvRectangle(warped, cvPoint(minloc.x, minloc.y), cvPoint(minloc.x
	 + tpl->width, minloc.y + tpl->height), CV_RGB(255, 0, 0), 1, 0, 0);

	 // cvResetImageROI(img);
	 cvShowImage("KephD", warped);
	 cvWaitKey(10000);*/

	KephPos position;
	int img_step = warped->widthStep;
	int img_channels = warped->nChannels;
	uchar * img_data = (uchar *) warped->imageData;
	/*
	 cvNamedWindow("Blue", CV_WINDOW_NORMAL);
	 cvCreateTrackbar("minB", "Blue", &mB_val, 256, onTrackbarSlideB);

	 cvNamedWindow("White", CV_WINDOW_NORMAL);
	 cvCreateTrackbar("minW", "White", &mW_val, 256, onTrackbarSlideW);*/

	blue->origin = warped->origin;
	int filtered_step = blue->widthStep;
	int filtered_channels = blue->nChannels;
	uchar * filtered_data_b = (uchar *) blue->imageData;

	white->origin = warped->origin;
	int filtered_step_w = white->widthStep;
	int filtered_channels_w = white->nChannels;
	uchar * filtered_data_w = (uchar *) white->imageData;

	for (int i = 0; i < x2; i++) {
		for (int j = 0; j < y2; j++) {
			if ((((img_data[i * img_step + j * img_channels]) > (mB_val
					+ img_data[i * img_step + j * img_channels + 1]))
					&& ((img_data[i * img_step + j * img_channels]) > (mB_val
							+ img_data[i * img_step + j * img_channels + 2])))
					&& i > x1 && j > y1)
				filtered_data_b[i * filtered_step + j * filtered_channels]
						= 255;
			else {
				filtered_data_b[i * filtered_step + j * filtered_channels] = 0;
			}
			if ((img_data[i * img_step + j * img_channels] + mW_val >= 255
					&& img_data[i * img_step + j * img_channels + 1] + mW_val
							>= 255 && img_data[i * img_step + j * img_channels
					+ 2] + mW_val >= 255) && i > x1 && j > y1)
				filtered_data_w[i * filtered_step_w + j * filtered_channels_w]
						= 255;
			else
				filtered_data_w[i * filtered_step_w + j * filtered_channels_w]
						= 0;
		}
	}
	cvErode(blue, blue, 0, 2);
	cvMoments(blue, moments, 1);

	// The actual moment values B
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);
	cvErode(white, white, 0, 4);
	cvMoments(white, moments, 1);
	position.posB.x = (int) moment10 / area;
	position.posB.y = (int) moment01 / area;
	// The actual moment values W
	moment10 = cvGetSpatialMoment(moments, 1, 0);
	moment01 = cvGetSpatialMoment(moments, 0, 1);
	area = cvGetCentralMoment(moments, 0, 0);

	position.posW.x = (int) moment10 / area;
	position.posW.y = (int) moment01 / area;
	/*	cvShowImage("Blue", blue);
	 cvShowImage("White", white);*/
	return position;
}

FieldViewer::~FieldViewer() {
	// TODO Auto-generated destructor stub
}

bool loadPoints(CvPoint c[4]) {
	FILE *fp;
	fp = fopen(PFILENAME, "r");
	if (fp == NULL) {
		return false;
	} else {
		for (int i = 0; i < 4; i++) {
			fscanf(fp, "%d %d\n", &c[i].x, &c[i].y);
		}
		return true;
	}
}

void savePoints(CvPoint c[4]) {
	FILE *fp;
	fp = fopen(PFILENAME, "w");
	if (fp != NULL) {
		for (int i = 0; i < 4; i++) {
			fprintf(fp, "%d %d\n", c[i].x, c[i].y);
		}
		fclose(fp);
	}
}

void FieldViewer::compute_Warp(IplImage * img) {
	if (loadPoints(p_corners)) {
		CvPoint2D32f c1[4] = { cvPoint2D32f(p_corners[0].x, p_corners[0].y),
				cvPoint2D32f(p_corners[1].x, p_corners[1].y), cvPoint2D32f(
						p_corners[2].x, p_corners[2].y), cvPoint2D32f(
						p_corners[3].x, p_corners[3].y) };

		CvPoint2D32f c2[4] = { cvPoint2D32f(img->width, img->height),
				cvPoint2D32f(img->width, 0), cvPoint2D32f(0, 0), cvPoint2D32f(
						0, img->height) };

		mmat = cvCreateMat(3, 3, CV_32FC1);
		mmat = cvGetPerspectiveTransform(c1, c2, mmat);
	} else {
		cvNamedWindow("Terrain", CV_WINDOW_AUTOSIZE);
		cvSetMouseCallback("Terrain", on_mouse);
		cvShowImage("Terrain", img);
		while (waiting_corners)
			cvWaitKey(500);
		CvPoint2D32f c1[4] = { cvPoint2D32f(p_corners[0].x, p_corners[0].y),
				cvPoint2D32f(p_corners[1].x, p_corners[1].y), cvPoint2D32f(
						p_corners[2].x, p_corners[2].y), cvPoint2D32f(
						p_corners[3].x, p_corners[3].y) };

		CvPoint2D32f c2[4] = { cvPoint2D32f(img->width, img->height),
				cvPoint2D32f(img->width, 0), cvPoint2D32f(0, 0), cvPoint2D32f(
						0, img->height) };

		mmat = cvCreateMat(3, 3, CV_32FC1);
		mmat = cvGetPerspectiveTransform(c1, c2, mmat);

		try {
			cvWarpPerspective(img, warped, mmat);
		} catch (Exception ex) {
			cout << "Exception in performWarp()\n";
		}
		cvDestroyWindow("Terrain");
		savePoints(p_corners);
	}
}
