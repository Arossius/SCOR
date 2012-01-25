//============================================================================
// Name        : SCOR.cpp
// Author      : Arossius
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "Utils.h"
using namespace std;

int main() {
	/* http://linux.die.net/man/3/mq_open */
	mqd_t coucou = mq_open("coucou", O_RDONLY);
	mq_unlink("coucou");
	mq_close(coucou);
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
