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
	void Launch(void * arg);


/* Méthodes protégées */
protected:
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* ordre_thread);

protected:
	/* Identifiant du thread */
	pthread_t thread;

	/* Boîte aux lettres de communication entre ordre et com */
	mqd_t bal_ordre_com;
	/* Boîte aux lettres de communication entre Com et robot */
	mqd_t bal_com_robot;

	/* outil d'envoi des commandes au robot */
	Communication com;
};

#endif /* COMMUNICATION_THREAD_H_ */
