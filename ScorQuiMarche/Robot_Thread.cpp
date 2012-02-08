/*
 * Robot_Thread.cpp
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#include "Robot_Thread.h"

#include <pthread.h>

#include <math.h>

#include <semaphore.h>

#include <iostream>

sem_t sem_pos;

/* Permet de stocker la position actuelle du robot 1 */
Robot robot1;
/* Permet de stocker la position actuelle du robot 2 */
Robot robot2;

Robot_Thread::Robot_Thread()
{
	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_Com_Robot);
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_RDONLY | O_CREAT | O_NONBLOCK, S_IRWXU, &att);
	att.mq_msgsize = sizeof(Msg_Vid_Robot);
	bal_video_robot = mq_open(BAL_VIDEO_ROBOT, O_RDONLY | O_NONBLOCK | O_CREAT, S_IRWXU, &att);
	att.mq_msgsize = sizeof(Msg_Robot_IA);
	bal_robot_ia = mq_open(BAL_ROBOT_IA, O_WRONLY | O_CREAT, S_IRWXU, &att);
	sem_init(&sem_pos, 0, 1); // TODO Faire ça ailleurs, (il y a des hotels pour ça :D)
}

Robot_Thread::~Robot_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_com_robot);
	mq_close(bal_video_robot);
	mq_close(bal_robot_ia);
	mq_unlink(BAL_ROBOT_IA);
	sem_close(&sem_pos);
}

void Robot_Thread::Launch()
{
	pthread_create(&thread, NULL, Robot_Thread::exec, this);
}

void Robot_Thread::run()
{
	Msg_Com_Robot msg_com_robot;
	Msg_Vid_Robot msg_vid_robot;
	timespec delay;
	delay.tv_nsec = 1000000; // TODO : Temporaire
	delay.tv_sec = 0;

	for ( ; ; )
	{
		bool modified = false;
		if (mq_timedreceive(bal_com_robot, (char*)&msg_com_robot, sizeof(Msg_Com_Robot), NULL, &delay) != -1)
		{
			modified = true;
			setNewPosition(robot1, msg_com_robot.robot1);
			setNewPosition(robot2, msg_com_robot.robot2);
		}
		if (mq_receive(bal_video_robot, (char*)&msg_vid_robot, sizeof(Msg_Vid_Robot), NULL) != -1)
		{
			modified = true;
			copyPositions(msg_vid_robot.robot1, msg_vid_robot.robot2, msg_vid_robot.balle);
		}
		if (modified)
		{
			Msg_Robot_IA msg_robot_ia;

			msg_robot_ia.robot1.angle = robot1.angle;
			msg_robot_ia.robot1.pos_x = robot1.pos_x;
			msg_robot_ia.robot1.pos_y = robot1.pos_y;

			msg_robot_ia.robot2.angle = robot2.angle;
			msg_robot_ia.robot2.pos_x = robot2.pos_x;
			msg_robot_ia.robot2.pos_y = robot2.pos_y;

			msg_robot_ia.balle.pos_x = balle.pos_x;
			msg_robot_ia.balle.pos_y = balle.pos_y;
			msg_robot_ia.balle.vit_x = balle.vit_x;
			msg_robot_ia.balle.vit_y = balle.vit_y;


			std::cout << "position robot1 : " << msg_robot_ia.robot1.pos_x << ", " << msg_robot_ia.robot1.pos_y << ", " << msg_robot_ia.robot1.angle << std::endl;
			std::cout << "position robot2 : " << msg_robot_ia.robot2.pos_x << ", " << msg_robot_ia.robot2.pos_y << ", " << msg_robot_ia.robot2.angle << std::endl;
			mq_send(bal_robot_ia, (char*)&msg_robot_ia, sizeof(Msg_Robot_IA), 0);
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
	sem_wait(&sem_pos);
	if (pas_robot.pas_droite < pas_robot.pas_gauche)
	{
		int pas_angle = (pas_robot.pas_gauche - pas_robot.pas_droite) / 2;
		robot.pos_x += (pas_robot.pas_droite + pas_angle) * CONVERSION_LONGUEUR * cos(robot.angle * M_PI / 180);
		robot.pos_y += (pas_robot.pas_droite + pas_angle) * CONVERSION_LONGUEUR * sin(robot.angle * M_PI / 180);
		robot.angle += pas_angle * CONVERSION_ANGLE;
	}
	else
	{
		int pas_angle = (pas_robot.pas_droite - pas_robot.pas_gauche) / 2;
		robot.pos_x += (pas_robot.pas_gauche + pas_angle) * CONVERSION_LONGUEUR * cos(robot.angle * M_PI / 180);
		robot.pos_y += (pas_robot.pas_gauche + pas_angle) * CONVERSION_LONGUEUR * sin(robot.angle * M_PI / 180);
		robot.angle += pas_angle * CONVERSION_ANGLE;
	}
	sem_post(&sem_pos);
}

void Robot_Thread::copyPositions(Robot robot_1, Robot robot_2, Balle balle)
{
	sem_wait(&sem_pos);
	robot1.pos_x = robot_1.pos_x;
	robot1.pos_y = robot_1.pos_y;
	robot1.angle = robot_1.angle;
	robot2.pos_x = robot_2.pos_x;
	robot2.pos_y = robot_2.pos_y;
	robot2.angle = robot_2.angle;
	sem_post(&sem_pos);
	this->balle.pos_x = balle.pos_x;
	this->balle.pos_y = balle.pos_y;
	this->balle.vit_x = balle.vit_x;
	this->balle.vit_y = balle.vit_y;
}
