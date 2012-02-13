/*
 * main.cpp
 *
 *  Created on: 24/gen/2012
 *      Author: jetmir
 */

#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "FieldAnalyse.h"

using namespace std;
using namespace cv;

int main(int argc, char * argv[]) {

	FieldAnalyse field;
	field.Launch();
	sleep(50);
	return 0;
}



