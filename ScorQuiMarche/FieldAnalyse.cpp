/*
 * FieldAnalyse.cpp
 *
 *  Created on: 12/feb/2012
 *      Author: jetmir
 */

#include "FieldAnalyse.h"

CvPoint p_corners[4];
bool waiting_corners;
int corners;

void on_mouse(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		p_corners[corners].x = x;
		p_corners[corners].y = y;
		corners++;
		waiting_corners = corners < 4;
		printf("mouse click x = %d  y = %d\n", x, y);
	}
}

void FieldAnalyse::Allocate_imgs() {
	const int w = img->width;
	const int h = img->height;
	treat = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	warped = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
}

FieldAnalyse::FieldAnalyse() {
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_RDONLY | O_CREAT, S_IRWXU, NULL);
	bal_video_robot = mq_open(BAL_VIDEO_ROBOT, O_RDONLY | O_NONBLOCK | O_CREAT,
			S_IRWXU, NULL);
	bal_robot_ia = mq_open(BAL_ROBOT_IA, O_WRONLY | O_CREAT, S_IRWXU, NULL);

	/* On lit du fichier*/
	//= cvCaptureFromAVI("/home/jetmir/out2.avi");
	capture = cvCaptureFromAVI("/home/jetmir/out2.avi");//= cvCaptureFromCAM(0);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	IplImage * imgL = cvQueryFrame(capture);
	imgL = cvQueryFrame(capture);
	corners = 0;
	waiting_corners = true;
	img = imgL;
	Allocate_imgs();
	compute_Warp();
	img = warped;
	setup_colors((char*) "White Color", white);
	setup_colors((char*) "Red Color", red);
	setup_colors((char*) "Blue color", blue);
	//Prepare parameters
	kephD.box[XMIN] = img->width / 2;
	kephD.box[XMAX] = img->width;
	kephD.box[YMIN] = 0;
	kephD.box[YMAX] = img->height / 2;

	kephG.box[XMIN] = img->width / 2;
	kephG.box[XMAX] = img->width;
	kephG.box[YMIN] = img->height / 2;
	kephG.box[YMAX] = img->height;

	balle.box[XMAX] = img->width;
	balle.box[XMIN] = 0;
	balle.box[YMIN] = 0;
	balle.box[YMAX] = img->height;

	ball_frame = 0;
	buffer_full = false;
}

FieldAnalyse::~FieldAnalyse() {
	//TODO
}

void FieldAnalyse::run() {
	Msg_Vid_Robot msg_vid_robot;
	IplImage * imgL = cvQueryFrame(capture);
	imgL = cvQueryFrame(capture);
	img = imgL;
	int fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	pthread_t kD, kG, b;
	kephD_t = new ThreadAnalyse(&kephD);
	kephG_t = new ThreadAnalyse(&kephG);
	balle_t = new ThreadAnalyse(&balle);
	Position k1, k2, bP;
	int btop[BALL_F] = { 1, 2, 3, 4, 0 };
	char v[80] = { 0 };
	char v2[80] = { 0 };
	CvFont font;
	double hScale = 0.5;
	double vScale = 0.5;
	int lineWidth = 1;
	CvPoint print;
	cvNamedWindow("ShowVid", CV_WINDOW_AUTOSIZE);
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX | CV_FONT_ITALIC, hScale, vScale,
			0, lineWidth);

	kephD.img = warped;
	kephG.img = warped;
	balle.img = warped;
	balle.treat = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U,
			1);
	kephG.treat = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U,
			1);
	kephD.treat = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U,
			1);

	balle.position = &(bP.posB);
	balle.color = red;
	do {
		img = cvQueryFrame(capture);
		compute_Warp();

		//BLUE COLOR   and BALL
		kephD.color = blue;
		kephG.color = blue;
		kephD.position = &(k1.posB);
		kephG.position = &(k2.posB);

		kD = kephD_t->Launch();
		kG = kephG_t->Launch();
		b = balle_t->Launch();

		//WHITE COLOR
		pthread_join(kD, NULL);
		pthread_join(kG, NULL);
		kephD.color = white;
		kephG.color = white;
		kephD.position = &(k1.posW);
		kephG.position = &(k2.posW);

		kD = kephD_t->Launch();
		kG = kephG_t->Launch();

		//ANGLE
		pthread_join(b, NULL);
		//SPEED
		if (!buffer_full && ball_frame != BALL_F) {
			balle_h[ball_frame++] = bP;
			if (ball_frame == BALL_F) {
				ball_frame = 0;
				buffer_full = true;
			}
		} else {
			palla.pos_x = bP.posB.x;
			palla.pos_y = bP.posB.y;
			palla.vit_x = (bP.posB.x - balle_h[btop[ball_frame]].posB.x)
					/ BALL_F;
			palla.vit_y = (bP.posB.y - balle_h[btop[ball_frame]].posB.y)
					/ BALL_F;
			balle_h[ball_frame++] = bP;
			ball_frame %= BALL_F;
		}
		pthread_join(kD, NULL);
		r1.pos_x = (k1.posB.x + k1.posW.x) / 2;
		r1.pos_y = (k1.posB.y + k1.posW.y) / 2;
		r1.angle = atan2(k1.posW.x - k1.posB.x, k1.posW.y - k1.posB.y) * 180
				/ PI;
		pthread_join(kG, NULL);
		r2.pos_x = (k2.posB.x + k2.posW.x) / 2;
		r2.pos_y = (k2.posB.y + k2.posW.y) / 2;
		r2.angle = atan2(k2.posW.x - k2.posB.x, k2.posW.y - k2.posB.y) * 180
				/ PI;
		sprintf(v, "%s Position %d %d angle %6.3f", "K1", r1.pos_x, r1.pos_y,
				r1.angle);
		sprintf(v2, "%s Position %d %d angle %6.3f", "K2", r2.pos_x, r2.pos_y,
				r2.angle);
		print.x = 10;
		print.y = 20;
		cvPutText(warped, v, print, &font, cvScalar(255, 255, 0));
		print.y = 40;
		cvPutText(warped, v2, print, &font, cvScalar(255, 255, 0));
		sprintf(v, "%s vx:%4.2f vy:%4.2f", "Ball here: ", palla.vit_x,
				palla.vit_y);
		cvPutText(warped, v, bP.posB, &font, cvScalar(255, 255, 0));
		sprintf(v, "K1");
		cvPutText(warped, v, k1.posW, &font, cvScalar(255, 255, 0));
		sprintf(v, "K2");
		cvPutText(warped, v, k2.posW, &font, cvScalar(255, 255, 0));
		cvShowImage("ShowVid", warped);
		//send messages
		msg_vid_robot.robot1 = r1;
		msg_vid_robot.robot2 = r2;
		msg_vid_robot.balle = palla;
		if(r1.pos_x < 640 && r1.pos_x > 0 && r1.pos_y < 480 && r1.pos_y > 0 
			&& r2.pos_x < 640 && r2.pos_x > 0 && r2.pos_y < 480 && r2.pos_y > 0
			&& palla.pos_x < 640 && palla.pos_x > 0 && palla.pos_y < 480 && palla.pos_y > 0)
		    mq_send(bal_video_robot, (char*) &msg_vid_robot, sizeof(Msg_Vid_Robot),0);
		cvWaitKey(1000/fps);
	} while (true);

	//pthread_t wait_me = threadAnalyse.Launch();
	//pthread_join(wait_me, NULL);
}

void *FieldAnalyse::exec(void *analyse_thread) {
	FieldAnalyse* thread = (FieldAnalyse*) analyse_thread;
	thread->run();
	return 0;
}

void FieldAnalyse::Launch() {
	pthread_create(&thread, NULL, FieldAnalyse::exec, this);
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

void FieldAnalyse::compute_Warp() {
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

void FieldAnalyse::setup_colors(char *color, int color_r[6]) {
	cvNamedWindow("Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow(color, CV_WINDOW_AUTOSIZE);
	createTrackbar("bluemin", "Image", &color_r[MINB], 256, NULL);
	createTrackbar("bluemax", "Image", &color_r[MAXB], 256, NULL);
	createTrackbar("redmin", "Image", &color_r[MINR], 256, NULL);
	createTrackbar("redmax", "Image", &color_r[MAXR], 256, NULL);
	createTrackbar("greenmin", "Image", &color_r[MING], 256, NULL);
	createTrackbar("greenmax", "Image", &color_r[MAXG], 256, NULL);
	int img_step = img->widthStep;
	int img_channels = img->nChannels;
	uchar * img_data = (uchar *) img->imageData;
	char key = -1;

	treat->origin = img->origin;
	int filtered_step = treat->widthStep;
	int filtered_channels = treat->nChannels;
	uchar * filtered_data_b = (uchar *) treat->imageData;

	do {
		for (int i = 0; i < img->width; i++) {
			for (int j = 0; j < img->height; j++) {
				if ((img_data[j * img_step + i * img_channels]) > color_r[MINB]
						&& (img_data[j * img_step + i * img_channels])
								< color_r[MAXB] && (img_data[j * img_step + i
						* img_channels + 1]) > color_r[MING] && (img_data[j
						* img_step + i * img_channels + 1]) < color_r[MAXG]
						&& (img_data[j * img_step + i * img_channels + 2])
								> color_r[MINR] && (img_data[j * img_step + i
						* img_channels + 2]) < color_r[MAXR]) {
					filtered_data_b[j * filtered_step + i * filtered_channels]
							= 255;
				} else {
					filtered_data_b[j * filtered_step + i * filtered_channels]
							= 0;
				}
			}
		}

		//cvErode(treat, treat, 0, 1);
		cvShowImage("Image", img);
		cvShowImage(color, treat);
		key = cvWaitKey(200);
	} while (key == -1);
	cvDestroyAllWindows();
}
