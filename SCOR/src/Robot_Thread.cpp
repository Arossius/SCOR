/*
 * Robot_Thread.cpp
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#include "Robot_Thread.h"

#include <pthread.h>

#include <math.h>

Robot_Thread::Robot_Thread() :
robot1(),
robot2()
{
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_RDONLY | O_CREAT);
	bal_video_robot = mq_open(BAL_VIDEO_ROBOT, O_RDONLY | O_NONBLOCK | O_CREAT);
}

Robot_Thread::~Robot_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_com_robot);
	mq_close(bal_video_robot);
}

void Robot_Thread::Launch()
{
	pthread_create(&thread, NULL, Robot_Thread::exec, this);
}

void Robot_Thread::run()
{
	Msg_Com_Robot* msg_com_robot = new Msg_Com_Robot();
	Msg_Vid_Robot* msg_vid_robot = new Msg_Vid_Robot();
	timespec delay;
	delay.tv_nsec = 1000000; // TODO : Temporaire
	delay.tv_sec = 0;

	for ( ; ; )
	{
		if (mq_timedreceive(bal_com_robot, (char*)msg_com_robot, sizeof(Msg_Com_Robot), NULL, &delay) != -1)
		{
			setNewPosition(robot1, msg_com_robot->robot1);
			setNewPosition(robot2, msg_com_robot->robot2);
		}
		if (mq_receive(bal_video_robot, (char*)msg_vid_robot, sizeof(Msg_Vid_Robot), NULL) != -1)
		{

		}
	}
}

void* Robot_Thread::exec(void* robot_thread)
{
	Robot_Thread* thread = (Robot_Thread*)robot_thread;
	thread->run();
	return 0;
}

void Robot_Thread::setNewPosition(Robot& robot, Pas_Robot pas_robot)
{
	if (pas_robot.pas_droite < pas_robot.pas_gauche)
	{
		robot.pos_x += pas_robot.pas_droite * CONVERSION_LONGUEUR * cos(robot.angle);
		robot.pos_y += pas_robot.pas_droite * CONVERSION_LONGUEUR * sin(robot.angle);
		robot.angle += (pas_robot.pas_gauche - pas_robot.pas_droite) * CONVERSION_ANGLE;
	}
	else
	{
		robot.pos_x += pas_robot.pas_gauche * CONVERSION_LONGUEUR * cos(robot.angle);
		robot.pos_y += pas_robot.pas_gauche * CONVERSION_LONGUEUR * sin(robot.angle);
		robot.angle += (pas_robot.pas_droite - pas_robot.pas_gauche) * CONVERSION_ANGLE;
	}
}

void Robot_Thread::copyPositions(Robot robot1, Robot robot2, Balle balle)
{
	this->robot1.pos_x = robot1.pos_x;
	this->robot1.pos_y = robot1.pos_y;
	this->robot1.angle = robot1.angle;
	this->robot2.pos_x = robot2.pos_x;
	this->robot2.pos_y = robot2.pos_y;
	this->robot2.angle = robot2.angle;
	this->balle.pos_x = balle.pos_x;
	this->balle.pos_y = balle.pos_y;
	this->balle.vit_x = balle.vit_x;
	this->balle.vit_y = balle.vit_y;
}
