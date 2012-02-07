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
	FieldViewer field = FieldViewer();
	field.Launch();
	sleep(20);
	return 0;
}



