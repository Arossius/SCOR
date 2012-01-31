/*
 * Communication_Thread.cpp
 *
 *  Created on: 31 janv. 2012
 *      Author: adrien
 */

#include <pthread.h>
#include <iostream>
#include <cstdio>
#include "Communication_Thread.h"

Communication_Thread::Communication_Thread()
{
	bal_ordre_com = mq_open(BAL_ORDRE_COM, O_RDONLY | O_NONBLOCK | O_CREAT);
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_WRONLY | O_NONBLOCK | O_CREAT);

	printf("Communication Thread : opening /dev/ttyUSB0 : %i\n", com.Open("/dev/ttyUSB0"));
}

Communication_Thread::~Communication_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_ordre_com);
	mq_close(bal_com_robot);


}

void Communication_Thread::Launch(void *arg)
{
	pthread_create(&thread, NULL, Communication_Thread::exec, this);
}


void* Communication_Thread::exec(void* Com_thread)
{
	Communication_Thread* thread = (Communication_Thread*)Com_thread;
	thread->run();
	return 0;
}


void Communication_Thread::run()
{
	for(;;)
	{



	}
}







