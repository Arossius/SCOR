/*
 * IAModule_Thread.h
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#ifndef IAModule_Thread_H_
#define IAModule_Thread_H_

#include <mqueue.h>
#include "Utils.h"


enum E_DIRECTION_BALLE {
	COMING,
	LEAVING,
	STOPPED
};




class IAModule_Thread {
public:
	IAModule_Thread();
	virtual ~IAModule_Thread();

	/* lance un nouveau thread thread */
	void Launch();


/* Méthodes protégées */
protected:
	/* Boucle principal du thread  */
	void run();
	/* Méthode obligatoire pour exécuter le thread */
	static void* exec(void* ordre_thread);


	/*
	 * Méthodes spécifiques à ce thread
	*/
	E_DIRECTION_BALLE DefinirDirectionBalle(Balle balle);

	Msg_IA_Ordre Defendre();

	Msg_IA_Ordre ReplacerDefense();

	Msg_IA_Ordre Attaquer();

	bool ObjectifModifie(Msg_IA_Ordre  msg_ia_ordre);


/* Attributs protégés */
protected:
	/* Identifiant du thread */
	pthread_t thread;

	/* Boîte aux lettres de communication entre Robot et IA */
	mqd_t bal_robot_ia;

	/* Boîte aux lettres de communication entre IA et Ordre */
	mqd_t bal_ia_ordre;

	/* Rappel du dernier ordre donné */	
	Msg_IA_Ordre dernierOrdre;

	/* données sur la balle et les robots */
	Balle balle;
	Robot robot1;
	Robot robot2;

};

#endif /* IAModule_Thread_H_ */
