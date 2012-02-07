/*
 * IAModule_Thread.cpp
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#include <pthread.h>
#include <math.h>
#include <cstdio>

#include "IAModule_Thread.h"


IAModule_Thread::IAModule_Thread() 
{
	mq_attr att;

	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_Robot_IA);

	bal_robot_ia = mq_open(BAL_ROBOT_IA, O_RDONLY | O_CREAT, S_IRWXU, &att);

	att.mq_msgsize = sizeof(Msg_IA_Ordre);
	bal_ia_ordre = mq_open(BAL_IA_ORDRE, O_WRONLY | O_CREAT, S_IRWXU, &att);
}

IAModule_Thread::~IAModule_Thread()
{
	pthread_cancel(thread);
	mq_close(bal_robot_ia);
	mq_close(bal_ia_ordre);
	mq_unlink(BAL_IA_ORDRE);

	printf("Fin de IAModule\n");
}


void IAModule_Thread::Launch()
{
	pthread_create(&thread, NULL, IAModule_Thread::exec, this);
}



void *IAModule_Thread::exec(void *IA_thread)
{
	IAModule_Thread* thread = (IAModule_Thread*)IA_thread;
	thread->run();
	return 0;
}


void IAModule_Thread::run()
{
	Msg_Robot_IA msg_robot_ia;
	Msg_IA_Ordre msg_ia_ordre;

	/*timespec delay;
	delay.tv_nsec = 1000000; // TODO : Temporaire
	delay.tv_sec = 0;
	*/


	for(;;)
	{
		
		// On attend que la position du robot ou de la balle change	
		if (mq_receive(bal_robot_ia, (char*)&msg_robot_ia, sizeof(Msg_Robot_IA), NULL)!= -1)
		{
			
			printf("Message reçu\n");

			balle = msg_robot_ia.balle;
			robot1 = msg_robot_ia.robot1;
			robot2 = msg_robot_ia.robot2;

			E_DIRECTION_BALLE directionBalle = DefinirDirectionBalle(msg_robot_ia.balle);

			switch (directionBalle)
			{
				case COMING: msg_ia_ordre = Defendre();
					break;
				case LEAVING: msg_ia_ordre = ReplacerDefense();
					break;
				case STOPPED: msg_ia_ordre =  Attaquer();
					break;
			}
			printf("objectif : %d\n", directionBalle);

			/*
			 * on vérifie maintenant si l'ordre a changé pour savoir
			 * s'il faut l'envoyer ou non au donneur d'ordre.
			 */
			if (ObjectifModifie(msg_ia_ordre))
			{
				// envoi
				mq_send(bal_ia_ordre, (char*)&msg_ia_ordre, sizeof(Msg_IA_Ordre), 0);
				// sauvegarde de l'objetctif actuel
				printf("obj1 : %d,%d,%f\n",msg_ia_ordre.ordre_robot1.robot.pos_x,msg_ia_ordre.ordre_robot1.robot.pos_y, msg_ia_ordre.ordre_robot1.robot.angle);
				printf("obj2 : %d,%d,%f\n",msg_ia_ordre.ordre_robot2.robot.pos_x,msg_ia_ordre.ordre_robot2.robot.pos_y, msg_ia_ordre.ordre_robot2.robot.angle);
				dernierOrdre = msg_ia_ordre;
			}


		} // Réception de message

	}// for(;;)
}



/*
 * Cette méthode permet de définir l'action en cours : il faut définir
 * si la balle se rapproche ou s'éloigne de nous. Si la balle est invisible
 * (dans le camp adverse) nous considérons qu'elle s'éloigne ce qui
 * nous placera dans une position défensive.
 */
E_DIRECTION_BALLE IAModule_Thread::DefinirDirectionBalle(Balle balle)
{
	/*
	 * à moins de 2, la vitesse est considérée comme nulle
	 * on utilise un carré pour la valeur absolue
	 */
	if ((balle.vit_y*balle.vit_y)+(balle.vit_x*balle.vit_x) < 4)
		return STOPPED;

	if (balle.vit_y < 0)
		return COMING;


	return LEAVING;
}



/*
 * En défense, on calcul le point d'impact de la balle avec la "ligne de défense"
 * sur laquelle sont positionnés les robot, on se déplace alors
 * en mode "casse-brique" pour intercepter la balle
 */
Msg_IA_Ordre IAModule_Thread::Defendre()
{
	Msg_IA_Ordre ordre;

	/*
	 * première étape : on observe la balle et vérifie si elle va rebondir sur
	 * un mur. Auquel cas, il faut calculer le rebond avant de trouver son
	 * point d'interception avec la ligne de défense.
	 *
	 * Equation de trajectoire de la balle :
	 * Y(X) = Vyballe/Vxballe*(X-Xballe) + Yballe
	 * on résoud : Y(X) = LIGNE_DEFENSE
	 * X = Vxballe/Vyballe*(LIGNE_DEFENSE -Yballe) + Xballe
	 */

	int XDefense = balle.pos_x/balle.pos_y*(LIGNE_DEFENSE - balle.pos_y) + balle.pos_x;

	/*
	 * cas du rebond, il faut recalculer XDefense.
	 */
	if (XDefense < 0 || XDefense > LARGEUR_TERRAIN)
	{
		/*
		 * résolution de X = BORD
		 * (BORD = 0 ou LARGEUR_TERRAIN
		 * Y(0) = -Vyballe/Vxballe*(BORD-Xballe) + Yballe
		 * -> nouvelle formule :
		 * Y1(X) = -Vyballe/Vxballe*X + Y(0)
		 * -> Xdef = -(LIGNE_DEFENSE - Y(0))*Vxballe/Vyballe;
		 */
		int Y0 = -balle.vit_y/balle.vit_x*((XDefense < 0? 0 : LARGEUR_TERRAIN ) -balle.pos_x) + balle.pos_y;

		XDefense = (LIGNE_DEFENSE - Y0)*balle.vit_x/balle.vit_y;
	}

	/*
	 * on détermine maintenant dans quelle moitié de terrain il faut
	 * défendre afin de placer les robots en conséquence.
	 */

	if (XDefense < LARGEUR_TERRAIN/2)
	{
		ordre.ordre_robot1.type_Ordre_Robot = Move;
		ordre.ordre_robot1.robot = INIT_ROBOT1;
		ordre.ordre_robot1.robot.pos_x = XDefense;

		ordre.ordre_robot2.type_Ordre_Robot = Do_nothing;
	} else
	{
		ordre.ordre_robot2.type_Ordre_Robot = Move;
		ordre.ordre_robot2.robot = INIT_ROBOT2;
		ordre.ordre_robot2.robot.pos_x = XDefense;

		ordre.ordre_robot1.type_Ordre_Robot = Do_nothing;

	}

	return ordre;
}

/*
 * Cette méthode se contente de demander aux robots de se replacer
 * à leurs positions de départ, au milieu de leur zone du terrain
 * pour être prêts à défendre.
 */
Msg_IA_Ordre IAModule_Thread::ReplacerDefense()
{
	Msg_IA_Ordre ordre;

	ordre.ordre_robot1.type_Ordre_Robot = Move;
	ordre.ordre_robot1.robot = INIT_ROBOT1;

	ordre.ordre_robot2.type_Ordre_Robot = Move;
	ordre.ordre_robot2.robot = INIT_ROBOT2;

	return ordre;
}

/*
 * Cas de l'attaque : il faut placer le robot à côté de la balle
 * et lancer l'ordre de tir (du bon côté)
 */
Msg_IA_Ordre IAModule_Thread::Attaquer()
{
	Msg_IA_Ordre ordre;
	/*
	 * simplification : les deux côtés du terrain sont les mêmes à un
	 * offset près, il suffit donc de traiter un seul cas et rajouter
	 * LARGEUR_TERRAIN/2 pour traiter la moitié droite.
	 */
	bool terrain_droite = false;
	Robot robot = robot1;
	if (balle.pos_x > LARGEUR_TERRAIN/2)
	{
		terrain_droite = true;
		robot = robot2;
	}


	/*
	 * on se place toujours légèrement derrière la balle
	 */
	Robot positionObjectif;
	positionObjectif.pos_y = balle.pos_y - RAYON_ROBOT/2;
	positionObjectif.angle = ANGLE_VISEE;
	/*
	 * le traitement est maintenant le même
	 * on se place à côté de la balle (en restant du même
	 * côté si on est déjà décalé) et on tire du bon côté.
	 */

	Type_Ordre_Robot tir = Shoot_Left;
	if (robot.pos_x < balle.pos_x)
	{
		positionObjectif.pos_x = balle.pos_x - 3*RAYON_ROBOT;
		tir = Shoot_Right;
	}
	else
	{
		positionObjectif.pos_x = balle.pos_x + 3*RAYON_ROBOT;
	}

	if (terrain_droite)
	{
		ordre.ordre_robot1.type_Ordre_Robot = Do_nothing;

		ordre.ordre_robot2.type_Ordre_Robot = tir;
		ordre.ordre_robot2.robot = positionObjectif;
	}
	else
	{
		ordre.ordre_robot1.type_Ordre_Robot = tir;
		ordre.ordre_robot1.robot = positionObjectif;

		ordre.ordre_robot2.type_Ordre_Robot = Do_nothing;
	}

	return ordre;
}


bool IAModule_Thread::ObjectifModifie(Msg_IA_Ordre  msg_ia_ordre)
{
	/*
	 * si un ordre à changé, l'objectif est bien modifié
	 */
	if (   msg_ia_ordre.ordre_robot1.type_Ordre_Robot != dernierOrdre.ordre_robot1.type_Ordre_Robot
		|| msg_ia_ordre.ordre_robot2.type_Ordre_Robot != dernierOrdre.ordre_robot2.type_Ordre_Robot )
		return true;

	if (   msg_ia_ordre.ordre_robot1.type_Ordre_Robot == Move
		|| msg_ia_ordre.ordre_robot1.type_Ordre_Robot == Shoot_Left
		|| msg_ia_ordre.ordre_robot1.type_Ordre_Robot == Shoot_Right)
	{
		int newX = msg_ia_ordre.ordre_robot1.robot.pos_x;
		int newY = msg_ia_ordre.ordre_robot1.robot.pos_y;
		int oldX = dernierOrdre.ordre_robot1.robot.pos_x;
		int oldY = dernierOrdre.ordre_robot1.robot.pos_y;

		if (   (newX - oldX)*(newX - oldX) > 100
			|| (newY - oldY)*(newY - oldY) > 100)
			return true;
	}
	if (   msg_ia_ordre.ordre_robot2.type_Ordre_Robot == Move
		|| msg_ia_ordre.ordre_robot2.type_Ordre_Robot == Shoot_Left
		|| msg_ia_ordre.ordre_robot2.type_Ordre_Robot == Shoot_Right)
	{
		int newX = msg_ia_ordre.ordre_robot2.robot.pos_x;
		int newY = msg_ia_ordre.ordre_robot2.robot.pos_y;
		int oldX = dernierOrdre.ordre_robot2.robot.pos_x;
		int oldY = dernierOrdre.ordre_robot2.robot.pos_y;

		if (   (newX - oldX)*(newX - oldX) > 100
			|| (newY - oldY)*(newY - oldY) > 100)
			return true;
	}

	return false;
}
