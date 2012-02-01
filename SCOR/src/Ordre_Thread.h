/*
 * Ordre_Thread.h
 *
 *  Created on: 27 janv. 2012
 *      Author: adrien
 */

#ifndef ORDRE_THREAD_H_
#define ORDRE_THREAD_H_

#include <mqueue.h>
#include "Utils.h"

class Ordre_Thread {
public:
	Ordre_Thread();
	virtual ~Ordre_Thread();

	/* Lance un nouveau thread */
	void Launch();

protected:

	/* Identifiant du thread */
	pthread_t thread;

	/* Boîte aux lettres de communication entre IA et Ordre */
	mqd_t bal_ia_ordre;
	/* Boîte aux lettres de communication entre Ordre et com*/
	mqd_t bal_ordre_com;

	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* ordre_thread);
	double calculerAngle(int posx, int posy);
};

#endif /* ORDRE_THREAD_H_ */
