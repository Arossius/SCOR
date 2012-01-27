/*
 * Ordre_Thread.cpp
 *
 *  Created on: 27 janv. 2012
 *      Author: adrien
 */

#include "Ordre_Thread.h"

#include <pthread.h>

Ordre_Thread::Ordre_Thread()
{
	bal_ia_ordre = mq_open(BAL_IA_ORDRE, O_RDONLY | O_CREAT | O_NONBLOCK);
}

Ordre_Thread::~Ordre_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_ia_ordre);
}

void Ordre_Thread::Launch()
{
	pthread_create(&thread, NULL, Ordre_Thread::exec, this);
}

void* Ordre_Thread::exec(void* robot_thread)
{
	Ordre_Thread* thread = (Ordre_Thread*)robot_thread;
	thread->run();
	return 0;
}

void Ordre_Thread::run()
{
	for ( ; ; )
	{
	}
}
