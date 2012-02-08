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
int mB_val = 8, mW_val = 42, mR_val = 60;

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
	red = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	tmpIm = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
	warped = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
}

void FieldViewer::ShowVideo() {
	Msg_Vid_Robot msg_vid_robot;
	float velocity = 0;
	int key = -1;
	cvNamedWindow("ShowVid", CV_WINDOW_NORMAL);
	cvShowImage("ShowVid", warped);
	do {
		img = cvQueryFrame(capture);
		warpImage(img, warped, mmat);
		FindKD();
		FindKG();
		FindBall(warped);
		balle.pos_x = ball.posB.x;
		balle.pos_y = ball.posB.y;
		balle.vit_x = (ball.posB.x - ball.prevB.x) * 15;
		balle.vit_y = (ball.posB.y - ball.prevB.y) * 15;
		CvFont font;
		double hScale = 0.5;
		double vScale = 0.5;
		int lineWidth = 1;
		CvPoint print;

		cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX | CV_FONT_ITALIC, hScale,
				vScale, 0, lineWidth);
		char v[80] = { 0 };
		char v2[80] = { 0 };
		sprintf(v, "%s Blue %d %d Blanche %d %d angle %6.3f", "K1",
				kephD.posB.x, kephD.posB.y, kephD.posW.x, kephD.posW.y,
				kephD.angle);
		sprintf(v2, "%s Blue %d %d Blanche %d %d angle %6.3f", "K2",
				kephG.posB.x, kephG.posB.y, kephG.posW.x, kephG.posW.y,
				kephG.angle);
		print.x = 10;
		print.y = 20;
		cvPutText(warped, v, print, &font, cvScalar(255, 255, 0));
		print.y = 40;
		cvPutText(warped, v2, print, &font, cvScalar(255, 255, 0));

		velocity = sqrt(pow((ball.posB.x - ball.prevB.x), 2) + pow((ball.posB.y
				- ball.prevB.y), 2));

		sprintf(v, "%s %4.2f", "Ball here: ", velocity);

		cvPutText(warped, v, ball.posB, &font, cvScalar(255, 255, 0));

		sprintf(v, "K1");
		cvPutText(warped, v, kephD.posW, &font, cvScalar(255, 255, 0));
		sprintf(v, "K2");
		cvPutText(warped, v, kephG.posW, &font, cvScalar(255, 255, 0));

		cvShowImage("ShowVid", warped);
		key = cvWaitKey(1000 / 15);
		msg_vid_robot.robot1 = kephR;
		msg_vid_robot.robot2 = kephL;
		msg_vid_robot.balle = balle;
		mq_send(bal_video_robot, (char*) &msg_vid_robot, sizeof(Msg_Vid_Robot),
				0);
	} while (key == -1);
	cvDestroyWindow("ShowVid");
}

void FieldViewer::FindKD() {
	kephD = FindKeph(warped, 0, warped->height / 2, warped->width / 2 + 15,
			warped->width);

	kephD.angle = atan2(kephD.posW.x - kephD.posB.x, kephD.posW.y
			- kephD.posB.y) * 180 / PI;
	kephR.angle = kephD.angle;
	kephR.pos_x = (kephD.posB.x + kephD.posW.x) / 2;
	kephR.pos_y = (kephD.posB.y + kephD.posW.y) / 2;
}

void FieldViewer::FindKG() {

	kephG = FindKeph(warped, warped->height / 2, warped->height, warped->width
			/ 2 + 15, warped->width);
	kephG.angle = atan2(kephG.posW.x - kephG.posB.x, kephG.posW.y
			- kephG.posB.y) * 180 / PI;
	kephL.angle = kephG.angle;
	kephL.pos_x = (kephG.posB.x + kephG.posW.x) / 2;
	kephL.pos_y = (kephG.posB.y + kephG.posW.y) / 2;
}

FieldViewer::FieldViewer() {
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_RDONLY | O_CREAT, S_IRWXU, NULL);
	bal_video_robot = mq_open(BAL_VIDEO_ROBOT, O_RDONLY | O_NONBLOCK | O_CREAT,
			S_IRWXU, NULL);
	bal_robot_ia = mq_open(BAL_ROBOT_IA, O_WRONLY | O_CREAT, S_IRWXU, NULL);

	/* On lit du fichier*/
	//= cvCaptureFromAVI("/home/jetmir/out2.avi");
	capture = cvCaptureFromCAM(0);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	IplImage * imgL = cvQueryFrame(capture);
	imgL = cvQueryFrame(capture);

	corners = 0;
	waiting_corners = true;
	moments = (CvMoments*) malloc(sizeof(CvMoments));
	img = imgL;
	ball.posB.x = 0;
	ball.posB.y = 0;
	ball.prevB.x = 0;
	ball.prevB.y = 0;

	Allocate_imgs();

	compute_Warp(imgL);
	setupColors(warped);
}

Position FieldViewer::FindKeph(IplImage *imgL, int x1, int x2, int y1, int y2) {
	Position position;
	int img_step = warped->widthStep;
	int img_channels = warped->nChannels;
	uchar * img_data = (uchar *) warped->imageData;

	blue->origin = warped->origin;
	int filtered_step = blue->widthStep;
	int filtered_channels = blue->nChannels;
	uchar * filtered_data_b = (uchar *) blue->imageData;

	white->origin = warped->origin;
	int filtered_step_w = white->widthStep;
	int filtered_channels_w = white->nChannels;
	uchar * filtered_data_w = (uchar *) white->imageData;

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			if ((((img_data[i * img_step + j * img_channels]) > (mB_val
					+ img_data[i * img_step + j * img_channels + 1]))
					&& ((img_data[i * img_step + j * img_channels]) > (mB_val
							+ img_data[i * img_step + j * img_channels + 2]))))
				filtered_data_b[i * filtered_step + j * filtered_channels]
						= 255;
			else {
				filtered_data_b[i * filtered_step + j * filtered_channels] = 0;
			}
			if ((img_data[i * img_step + j * img_channels] + mW_val >= 255
					&& img_data[i * img_step + j * img_channels + 1] + mW_val
							>= 255 && img_data[i * img_step + j * img_channels
					+ 2] + mW_val >= 255))
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
	cvErode(white, white, 0, 3);
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

void FieldViewer::FindBall(IplImage * imgL) {
	int x = imgL->width, y = imgL->height;
	int img_step = imgL->widthStep;
	int img_channels = imgL->nChannels;
	uchar * img_data = (uchar *) imgL->imageData;

	red->origin = imgL->origin;
	int filtered_step_r = red->widthStep;
	int filtered_channels_r = red->nChannels;
	uchar * filtered_data_r = (uchar *) red->imageData;
	int i = 0, j = 0;
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			if (((img_data[i * img_step + j * img_channels + 2]) > (mR_val
					+ img_data[i * img_step + j * img_channels]))
					&& ((img_data[i * img_step + j * img_channels + 2])
							> (mR_val + img_data[i * img_step + j
									* img_channels + 1])))
				filtered_data_r[i * filtered_step_r + j * filtered_channels_r]
						= 255;
			else
				filtered_data_r[i * filtered_step_r + j * filtered_channels_r]
						= 0;
		}
	}
	cvErode(red, red, 0, 1);
	cvMoments(red, moments, 1);
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);

	ball.prevB.x = ball.posB.x;
	ball.prevB.y = ball.posB.y;

	ball.posB.x = (int) moment10 / area;
	ball.posB.y = (int) moment01 / area;
}

void FieldViewer::setupColors(IplImage *img) {
	int key = -1;
	int x = img->width, y = img->height;
	int img_step = img->widthStep;
	int img_channels = img->nChannels;
	uchar * img_data = (uchar *) img->imageData;

	blue->origin = img->origin;
	int filtered_step = blue->widthStep;
	int filtered_channels = blue->nChannels;
	uchar * filtered_data_b = (uchar *) blue->imageData;

	white->origin = img->origin;
	int filtered_step_w = white->widthStep;
	int filtered_channels_w = white->nChannels;
	uchar * filtered_data_w = (uchar *) white->imageData;

	red->origin = img->origin;
	int filtered_step_r = red->widthStep;
	int filtered_channels_r = red->nChannels;
	uchar * filtered_data_r = (uchar *) red->imageData;

	cvNamedWindow("WhiteSetup", CV_WINDOW_NORMAL);
	cvNamedWindow("Image", CV_WINDOW_NORMAL);
	cvNamedWindow("BlueSetup", CV_WINDOW_NORMAL);
	cvNamedWindow("RedSetup", CV_WINDOW_NORMAL);
	createTrackbar("WhiteS", "WhiteSetup", &mW_val, 256, NULL);
	createTrackbar("BlueS", "BlueSetup", &mB_val, 256, NULL);
	createTrackbar("RedS", "RedSetup", &mR_val, 256, NULL);
	cvShowImage("Image", img);
	do {
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				if ((((img_data[i * img_step + j * img_channels]) > (mB_val
						+ img_data[i * img_step + j * img_channels + 1]))
						&& ((img_data[i * img_step + j * img_channels])
								> (mB_val + img_data[i * img_step + j
										* img_channels + 2]))))
					filtered_data_b[i * filtered_step + j * filtered_channels]
							= 255;
				else
					filtered_data_b[i * filtered_step + j * filtered_channels]
							= 0;
				if ((img_data[i * img_step + j * img_channels] + mW_val >= 255
						&& img_data[i * img_step + j * img_channels + 1]
								+ mW_val >= 255 && img_data[i * img_step + j
						* img_channels + 2] + mW_val >= 255))
					filtered_data_w[i * filtered_step_w + j
							* filtered_channels_w] = 255;
				else
					filtered_data_w[i * filtered_step_w + j
							* filtered_channels_w] = 0;

				if (((img_data[i * img_step + j * img_channels + 2]) > (mR_val
						+ img_data[i * img_step + j * img_channels]))
						&& ((img_data[i * img_step + j * img_channels + 2])
								> (mR_val + img_data[i * img_step + j
										* img_channels + 1])))
					filtered_data_r[i * filtered_step_r + j
							* filtered_channels_r] = 255;
				else
					filtered_data_r[i * filtered_step_r + j
							* filtered_channels_r] = 0;

			}
		}
		//cvErode(white, white, 0, 3);
		cvEqualizeHist(white, white);
		cvShowImage("WhiteSetup", white);
		//cvErode(blue, blue, 0, 1);
		cvEqualizeHist(blue, blue);
		cvShowImage("BlueSetup", blue);
		//cvErode(red, red, 0, 1);
		cvShowImage("RedSetup", red);

		key = cvWaitKey(300);
	} while (key == -1);
	cvDestroyAllWindows();

}

FieldViewer::~FieldViewer() {
	cvReleaseCapture(&capture);
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

void FieldViewer::run() {
	ShowVideo();
}

void* FieldViewer::exec(void* view_thread) {
	FieldViewer* thread = (FieldViewer*) view_thread;
	thread->run();
	return 0;
}

void FieldViewer::Launch() {
	pthread_create(&thread, NULL, FieldViewer::exec, this);
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
		try {
			cvWarpPerspective(img, warped, mmat);
		} catch (Exception ex) {
			cout << "Exception in performWarp()\n";
		}
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
