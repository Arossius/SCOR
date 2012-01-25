/*
 * Robot_Thread.cpp
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#include "Robot_Thread.h"

#include <pthread.h>

Robot_Thread::Robot_Thread()
{
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_RDONLY);
	bal_video_robot = mq_open(BAL_VIDEO_ROBOT, O_RDONLY);
}

Robot_Thread::~Robot_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_com_robot);
	mq_close(bal_video_robot);
	// TODO Auto-generated destructor stub
}

void Robot_Thread::Launch()
{
	pthread_create(&thread, NULL, Robot_Thread::exec, this);
	run();
}

void Robot_Thread::run()
{
	for ( ; ; )
	{

	}
}

void* Robot_Thread::exec(void* robot_thread)
{
	Robot_Thread* thread = (Robot_Thread*)robot_thread;
	thread->run();
	return 0;
}
