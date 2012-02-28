/*
 * Communication_Thread.h
 *
 *  Created on: 31 janv. 2012
 *      Author: adrien
 */

#ifndef COMMUNICATION_THREAD_H_
#define COMMUNICATION_THREAD_H_

#include <mqueue.h>
#include "Utils.h"
#include "Communication.h"


class Communication_Thread {
public:
	Communication_Thread();
	virtual ~Communication_Thread();

	/* lance un nouveau thread thread */
	void Launch();


/* Méthodes protégées */
protected:
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* ordre_thread);


	/* méthodes spécifiques à ce thread */
	bool setSpeed(Communication *robot, Pas_Robot pas);

	Pas_Robot getSpeed(Communication *robot);

	Pas_Robot getPosition(Communication *robot);

	bool setObjectif(Communication *robot, Pas_Robot pas);

	bool resetPosition(Communication *robot);

protected:
	/* Identifiant du thread */
	pthread_t thread;

	/* Boîte aux lettres de communication entre ordre et com */
	mqd_t bal_ordre_com;
	/* Boîte aux lettres de communication entre Com et robot */
	mqd_t bal_com_robot;

	/* outil d'envoi des commandes au robot */
	Communication *robot1;
	Communication *robot2;


	/* ordre actuel */
	Msg_Ordre_Com liste;
	int ordreCourantRobot1;
	Pas_Robot pasPreviousTopRobot1;
	Pas_Robot pasPreviousTopRobot2;
	int ordreCourantRobot2;


};

#endif /* COMMUNICATION_THREAD_H_ */
