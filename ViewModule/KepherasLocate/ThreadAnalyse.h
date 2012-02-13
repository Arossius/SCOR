/*
 * ThreadAnalyse.h
 *
 *  Created on: 11/feb/2012
 *      Author: jetmir
 */

#ifndef THREADANALYSE_H_
#define THREADANALYSE_H_

#define PFILENAME "points.txt"
#define PI 3.14159265

#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "highgui.h"


#define XMIN 0
#define XMAX 1
#define YMIN 2
#define YMAX 3

#define MINB 0
#define MAXB 1
#define MINR 2
#define MAXR 3
#define MING 4
#define MAXG 5


using namespace std;
using namespace cv;



typedef struct Position {
	CvPoint posB, posW;
	float_t angle;
} Position;



typedef struct Field {
	int box[4];
	int *color;
	IplImage *img;
	IplImage *treat;
	CvPoint *position;
} Field;

class ThreadAnalyse {
public:
	ThreadAnalyse(Field *field);
	virtual ~ThreadAnalyse();

	pthread_t Launch();
protected:
	CvMoments * moments;
	Field * field;
	CvMat* mmat;

	/* Identifiant du thread */
	pthread_t thread;
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* analyse_thread);

};

#endif /* THREADANALYSE_H_ */
