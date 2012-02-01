/*
 * Ordre_Thread.cpp
 *
 *  Created on: 27 janv. 2012
 *      Author: adrien
 */

#include "Ordre_Thread.h"

#include <pthread.h>

#include <semaphore.h>

#include <math.h>

#include "Robot_Thread.h"

#include <iostream>

#include <errno.h>

extern sem_t sem_pos;

extern Robot robot1;
extern Robot robot2;


Ordre_Thread::Ordre_Thread()
{
	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_IA_Ordre);
	bal_ia_ordre = mq_open(BAL_IA_ORDRE, O_RDONLY | O_CREAT, S_IRWXU, &att);
	att.mq_msgsize = sizeof(Msg_Ordre_Com);
	bal_ordre_com = mq_open(BAL_ORDRE_COM, O_WRONLY | O_CREAT | O_NONBLOCK, S_IRWXU, &att);
}

Ordre_Thread::~Ordre_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_ia_ordre);
	mq_close(bal_ordre_com);
	mq_unlink(BAL_ORDRE_COM);
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
	Msg_IA_Ordre msg_IA_Ordre;
	Msg_Ordre_Com msg_Ordre_Com;
	for ( ; ; )
	{
		mq_receive(bal_ia_ordre, (char*)&msg_IA_Ordre, sizeof(Msg_IA_Ordre), NULL);
		double angle = 0.0;
		double diff_angle = 0.0;
		int dist_x;
		int dist_y;
		int dist;
		switch (msg_IA_Ordre.ordre_robot1.type_Ordre_Robot)
		{
		case Do_nothing :
			msg_Ordre_Com.ordres[0].robot1.pas_droite = 0;
			msg_Ordre_Com.ordres[0].robot1.pas_gauche = 0;
			break;
		case Move :
			angle = atan2(msg_IA_Ordre.ordre_robot1.robot.pos_y - robot1.pos_y, msg_IA_Ordre.ordre_robot1.robot.pos_x - robot1.pos_x) * 180 / M_PI;
			diff_angle = angle - robot1.angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}
			msg_Ordre_Com.ordres[0].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			dist_x = robot1.pos_x - msg_IA_Ordre.ordre_robot1.robot.pos_x;
			dist_y = robot1.pos_y - msg_IA_Ordre.ordre_robot1.robot.pos_y;
			dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot1.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot1.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot1.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}

			msg_Ordre_Com.ordres[2].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot1.pas_droite = 0;
			msg_Ordre_Com.ordres[3].robot1.pas_gauche = 0;


			break;
		case Shoot_Left :
			angle = atan2(msg_IA_Ordre.ordre_robot1.robot.pos_y - robot1.pos_y, msg_IA_Ordre.ordre_robot1.robot.pos_x - robot1.pos_x) * 180 / M_PI;
			diff_angle = angle - robot1.angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}
			msg_Ordre_Com.ordres[0].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			dist_x = robot1.pos_x - msg_IA_Ordre.ordre_robot1.robot.pos_x;
			dist_y = robot1.pos_y - msg_IA_Ordre.ordre_robot1.robot.pos_y;
			dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot1.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot1.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot1.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}

			msg_Ordre_Com.ordres[2].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot1.pas_droite = - 90 / CONVERSION_ANGLE;
			msg_Ordre_Com.ordres[3].robot1.pas_gauche = 90 / CONVERSION_ANGLE;

			break;
		case Shoot_Right:
			angle = atan2(msg_IA_Ordre.ordre_robot1.robot.pos_y - robot1.pos_y, msg_IA_Ordre.ordre_robot1.robot.pos_x - robot1.pos_x) * 180 / M_PI;
			diff_angle = angle - robot1.angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}
			msg_Ordre_Com.ordres[0].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			int dist_x = robot1.pos_x - msg_IA_Ordre.ordre_robot1.robot.pos_x;
			int dist_y = robot1.pos_y - msg_IA_Ordre.ordre_robot1.robot.pos_y;
			int dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot1.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot1.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot1.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}

			msg_Ordre_Com.ordres[2].robot1.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot1.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot1.pas_droite = 90 / CONVERSION_ANGLE;
			msg_Ordre_Com.ordres[3].robot1.pas_gauche = - 90 / CONVERSION_ANGLE;

			break;
		}
		switch (msg_IA_Ordre.ordre_robot2.type_Ordre_Robot)
		{
		case Do_nothing :
			msg_Ordre_Com.ordres[0].robot2.pas_droite = 0;
			msg_Ordre_Com.ordres[0].robot2.pas_gauche = 0;
			break;
		case Move :
			angle = atan2(msg_IA_Ordre.ordre_robot2.robot.pos_y - robot2.pos_y, msg_IA_Ordre.ordre_robot2.robot.pos_x - robot2.pos_x) * 180 / M_PI;
			diff_angle = angle - robot2.angle;
			if (diff_angle > 180)
			{
				diff_angle =- 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle =+ 360;
			}
			msg_Ordre_Com.ordres[0].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			dist_x = robot2.pos_x - msg_IA_Ordre.ordre_robot2.robot.pos_x;
			dist_y = robot2.pos_y - msg_IA_Ordre.ordre_robot2.robot.pos_y;
			dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot2.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot2.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot2.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle =- 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle =+ 360;
			}

			msg_Ordre_Com.ordres[2].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot2.pas_droite = 0;
			msg_Ordre_Com.ordres[3].robot2.pas_gauche = 0;
			break;
		case Shoot_Left :
			angle = atan2(msg_IA_Ordre.ordre_robot2.robot.pos_y - robot2.pos_y, msg_IA_Ordre.ordre_robot2.robot.pos_x - robot2.pos_x) * 180 / M_PI;
			diff_angle = angle - robot2.angle;
			if (diff_angle > 180)
			{
				diff_angle =- 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle =+ 360;
			}
			msg_Ordre_Com.ordres[0].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			dist_x = robot2.pos_x - msg_IA_Ordre.ordre_robot2.robot.pos_x;
			dist_y = robot2.pos_y - msg_IA_Ordre.ordre_robot2.robot.pos_y;
			dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot2.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot2.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot2.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle =- 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle =+ 360;
			}

			msg_Ordre_Com.ordres[2].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot2.pas_droite = - 90 / CONVERSION_ANGLE;
			msg_Ordre_Com.ordres[3].robot2.pas_gauche = 90 / CONVERSION_ANGLE;

			break;
		case Shoot_Right:
			angle = atan2(msg_IA_Ordre.ordre_robot2.robot.pos_y - robot2.pos_y, msg_IA_Ordre.ordre_robot2.robot.pos_x - robot2.pos_x) * 180 / M_PI;
			diff_angle = angle - robot2.angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}
			msg_Ordre_Com.ordres[0].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[0].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			int dist_x = robot2.pos_x - msg_IA_Ordre.ordre_robot2.robot.pos_x;
			int dist_y = robot2.pos_y - msg_IA_Ordre.ordre_robot2.robot.pos_y;
			int dist = sqrt(dist_x * dist_x + dist_y * dist_y);
			msg_Ordre_Com.ordres[1].robot2.pas_droite = dist / CONVERSION_LONGUEUR;
			msg_Ordre_Com.ordres[1].robot2.pas_gauche = dist / CONVERSION_LONGUEUR;

			diff_angle = msg_IA_Ordre.ordre_robot2.robot.angle - angle;
			if (diff_angle > 180)
			{
				diff_angle -= 360;
			}
			else if (diff_angle < -180)
			{
				diff_angle += 360;
			}

			msg_Ordre_Com.ordres[2].robot2.pas_droite = diff_angle / CONVERSION_ANGLE; // TODO vérifier qu'il faut bien mettre un pas positif
			msg_Ordre_Com.ordres[2].robot2.pas_gauche = - diff_angle / CONVERSION_ANGLE;

			msg_Ordre_Com.ordres[3].robot2.pas_droite = 90 / CONVERSION_ANGLE;
			msg_Ordre_Com.ordres[3].robot2.pas_gauche = - 90 / CONVERSION_ANGLE;

			break;
		}
		mq_send(bal_ordre_com, (char*)&msg_Ordre_Com, sizeof(Msg_Ordre_Com), 0);
	}
}
