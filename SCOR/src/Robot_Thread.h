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

const double CONVERSION_LONGUEUR = 1.0;
const double CONVERSION_ANGLE = 0.171428; // Converti un pas en angle (en degré)

class Robot_Thread
{
public:
	Robot_Thread();
	virtual ~Robot_Thread();

	/* Lance un nouveau thread */
	void Launch();

protected:

	/* Boîte aux lettres de communication entre Com et Robot */
	mqd_t bal_com_robot;
	/* Boîte aux lettres de communication entre Video et Robot */
	mqd_t bal_video_robot;
	/* Boîte aux lettres de communication entre Robot et IA */
	mqd_t bal_robot_ia;

	/* Permet de stocker les position et vitesse actuelles de la balle */
	Balle balle;

	/* Identifiant du thread */
	pthread_t thread;

	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* robot_thread);

	/* Permet de déterminer la nouvelle posiiton du robot à partir d'un déplacement du robot */
	void setNewPosition(Robot& robot, Pas_Robot pas_robot);

	void copyPositions(Robot robot1, Robot robot2, Balle balle);
};

#endif /* ROBOT_THREAD_H_ */
