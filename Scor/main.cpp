/*
 * main.cpp
 *
 *  Created on: 24/gen/2012
 *      Author: jetmir
 */

#include <stdio.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "FieldAnalyse.h"
#include "Communication_Thread.h"
#include "IAModule_Thread.h"
#include "Ordre_Thread.h"
#include "Robot_Thread.h"

using namespace std;
using namespace cv;


int main(int argc, char * argv[]) {

	Communication_Thread comTH;
	comTH.Launch();

	Ordre_Thread ordreTH;
	ordreTH.Launch();

	FieldAnalyse fieldAnalyse;
	fieldAnalyse.Launch();

	IAModule_Thread iamoduleTH;
	iamoduleTH.Launch();

	Robot_Thread robotTH;
	robotTH.Launch();



	string a("aaa");
	while (a != "q")
	{

		std::cout << "tapez 'q' pour quitter" <<endl;
		cin >> a;

		if (a == "q")
			break;

	}



	cout << "c'est fini" << endl;


	return 0;
}



