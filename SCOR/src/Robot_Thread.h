/*
 * Robot_Thread.h
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#ifndef ROBOT_THREAD_H_
#define ROBOT_THREAD_H_

#include <mqueue.h>
#include "Utils.h"

class Robot_Thread
{
public:
	Robot_Thread();
	virtual ~Robot_Thread();

	void Launch();

protected:

	mqd_t bal_com_robot;
	mqd_t bal_video_robot;

	pthread_t thread;

	void run();
	static void* exec(void* robot_thread);
};

#endif /* ROBOT_THREAD_H_ */
