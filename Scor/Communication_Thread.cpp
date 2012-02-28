/*
 * Communication_Thread.cpp
 *
 *  Created on: 31 janv. 2012
 *      Author: adrien
 */

#include <pthread.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "Communication_Thread.h"

Communication_Thread::Communication_Thread()
{
	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_Ordre_Com);

	bal_ordre_com = mq_open(BAL_ORDRE_COM, O_RDONLY | O_NONBLOCK | O_CREAT, S_IRWXU, &att);

	Msg_Ordre_Com msg_ordre_com;
	while (mq_receive(bal_ordre_com, (char*)&msg_ordre_com, sizeof(Msg_Ordre_Com), NULL)!= -1) {}


	att.mq_msgsize = sizeof(Msg_Com_Robot);
	bal_com_robot = mq_open(BAL_COM_ROBOT, O_WRONLY | O_CREAT | O_NONBLOCK, S_IRWXU, &att);




	robot1 = new Communication();
	printf("Communication Thread : opening First Khepera  : %i\n", robot1->Open(FICHIER_ROBOT_1));

	robot2 = new Communication();
	printf("Communication Thread : opening Second Khepera : %i\n", robot2->Open(FICHIER_ROBOT_2));


	for (int i = 0; i<4; i++)
	{
		liste.ordres[i].robot1.pas_gauche = 0;
		liste.ordres[i].robot1.pas_droite = 0;
		liste.ordres[i].robot2.pas_gauche = 0;
		liste.ordres[i].robot2.pas_droite = 0;

	}
	ordreCourantRobot1 = 4;
	ordreCourantRobot2 = 4;

	pasPreviousTopRobot1.pas_droite = 0;
	pasPreviousTopRobot1.pas_gauche = 0;

	pasPreviousTopRobot2.pas_droite = 0;
	pasPreviousTopRobot2.pas_gauche = 0;



}

Communication_Thread::~Communication_Thread()
{

	pthread_cancel(thread);

	delete robot1;
	delete robot2;

	printf("fin tache Com\n");
	mq_close(bal_ordre_com);
	mq_close(bal_com_robot);
	unlink(BAL_COM_ROBOT);

}

void Communication_Thread::Launch()
{
	pthread_create(&thread, NULL, Communication_Thread::exec, this);
}


void* Communication_Thread::exec(void* Com_thread)
{
	Communication_Thread* thread = (Communication_Thread*)Com_thread;
	thread->run();
	return 0;
}


void Communication_Thread::run()
{
	Msg_Ordre_Com msg_ordre_com;

	resetPosition(robot1);
	resetPosition(robot2);

	for(;;)
	{
		// si on a reçu un message
		//printf("tache : %d\n",bal_ordre_com);
		if (mq_receive(bal_ordre_com, (char*)&msg_ordre_com, sizeof(Msg_Ordre_Com), NULL)!= -1)
		{

			liste = msg_ordre_com;
			ordreCourantRobot1 = 0;
			ordreCourantRobot2 = 0;

			pasPreviousTopRobot1.pas_droite = 0;
			pasPreviousTopRobot1.pas_gauche = 0;

			pasPreviousTopRobot2.pas_droite = 0;
			pasPreviousTopRobot2.pas_gauche = 0;


			Pas_Robot reinit = {0,0};
			if (msg_ordre_com.ordres[0].robot1.pas_droite == 0 && msg_ordre_com.ordres[0].robot1.pas_gauche == 0)
			{
				ordreCourantRobot1 = 4;
			}else
			{
				setSpeed(robot1,reinit);
				resetPosition(robot1);
				setObjectif(robot1, liste.ordres[ordreCourantRobot1].robot1);
			}

			if (msg_ordre_com.ordres[0].robot2.pas_droite == 0 && msg_ordre_com.ordres[0].robot2.pas_gauche == 0)
			{
				ordreCourantRobot2 = 4;
			}
			else
			{
				setSpeed(robot2,reinit);
				resetPosition(robot2);
				setObjectif(robot2, liste.ordres[ordreCourantRobot2].robot2);
			}




			sleep(1);


		}else
		{
			Pas_Robot vitesse = getSpeed(robot1);
			if (vitesse.pas_droite == 0 && vitesse.pas_gauche == 0 && ordreCourantRobot1 < 3)
			{
				resetPosition(robot1);

				pasPreviousTopRobot1.pas_droite = 0;
				pasPreviousTopRobot1.pas_gauche = 0;

				setObjectif(robot1, liste.ordres[++ordreCourantRobot1].robot1);
			}


			vitesse = getSpeed(robot2);
			if (vitesse.pas_droite == 0 && vitesse.pas_gauche == 0 && ordreCourantRobot2 < 3)
			{
				resetPosition(robot2);

				pasPreviousTopRobot2.pas_droite = 0;
				pasPreviousTopRobot2.pas_gauche = 0;

				setObjectif(robot2, liste.ordres[++ordreCourantRobot2].robot2);
			}


		}//sinon



		usleep(50000);
	}
}


bool Communication_Thread::setSpeed(Communication *robot, Pas_Robot pas)
{

	char buffer[500];
	sprintf(buffer, "D,%d,%d\n", pas.pas_gauche, pas.pas_droite);
	string msg = buffer;

//	printf("%s",msg.c_str());
	robot->SendData(msg);

	//préparation de la réception
	string recv;
	robot->ReceiveData(recv);
	//printf(recv.c_str());

	//robot->sendMsg(msg.c_str(), buffer, 500);

	if (buffer[0] == 'd')
		return true;


	return false;
}

Pas_Robot Communication_Thread::getSpeed(Communication *robot)
{
	string msg = "E\n";
	//char *buffer;
	robot->SendData(msg);

	string recv;
	robot->ReceiveData(recv);

	/*
	 * réponse de la forme : e,pas_gauche,pas_droit\n
	 */
	//uffer = new char[500];
	//robot->sendMsg(msg.c_str(), buffer, 500);
	//string recv = buffer;

	Pas_Robot pas;
	pas.pas_gauche = atoi( recv.substr(2, recv.find_last_of(',')).c_str() );
	pas.pas_droite = atoi( recv.substr(recv.find_last_of(','),recv.length()).c_str()-1 );

	return pas;


}


Pas_Robot Communication_Thread::getPosition(Communication *robot)
{
	string msg = "H\n";
	//printf("getPos\n");
	robot->SendData(msg);

	string recv;
	robot->ReceiveData(recv);

	//char buffer[500];
	//robot->sendMsg(msg.c_str(), buffer, 500);
	//string recv = buffer;

	/*
	 * réponse de la forme : e,pas_gauche,pas_droit\n
	 */
	Pas_Robot pas;
	pas.pas_gauche = atoi( recv.substr(2, recv.find_last_of(',')).c_str() );
	pas.pas_droite = atoi( recv.substr(recv.find_last_of(','),recv.length()).c_str()-1 );

	//printf("getPos\n");

	return pas;


}



bool Communication_Thread::setObjectif(Communication *robot, Pas_Robot pas)
{

	char buffer[500];
	sprintf(buffer, "C,%d,%d\n", pas.pas_gauche, pas.pas_droite);
	string msg = buffer;

	printf("%s",msg.c_str());
	robot->SendData(msg);

	//préparation de la réception
	string recv;
	robot->ReceiveData(recv);
	//printf("recv : %s", recv.c_str());

	//robot->sendMsg(msg.c_str(), buffer, 500);

	if (buffer[0] == 'c')
		return true;


	return false;

}


bool Communication_Thread::resetPosition(Communication *robot)
{
	string msg = "G,0,0\n";

	robot->SendData(msg);
	//préparation de la réception
	string recv;
	robot->ReceiveData(recv);
	//char buffer[500];
	//robot->sendMsg(msg.c_str(), buffer, 500);

	if (recv[0] == 'g')
		return true;


	return false;

}
