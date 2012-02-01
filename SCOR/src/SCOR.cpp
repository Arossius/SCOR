//============================================================================
// Name        : SCOR.cpp
// Author      : Arossius
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "Utils.h"
#include "Communication_Thread.h"

using namespace std;

int main()
{
	/* http://linux.die.net/man/3/mq_open */

	Communication_Thread comTH;

	comTH.Launch();

	mqd_t bal_ordre_com;
	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_Ordre_Com);


	bal_ordre_com = mq_open(BAL_ORDRE_COM, O_WRONLY | O_NONBLOCK | O_CREAT, S_IRWXU, &att);
	cout <<bal_ordre_com <<endl;

	string cmd;
//	mqd_t coucou = mq_open("coucou", O_RDONLY);
	while (1)
	{
		int pasGauche, pasDroite;
		char cmd[5]="  ";
		cout << "commande :" <<endl;

		cin >> cmd;

		cout << "après cin" <<endl;
		cout << "--------- " << cmd << " ---------" << endl;

		if (strcmp(cmd,"exit") == 0)
			break;

		Msg_Ordre_Com msg_ordre_com;

		for (int i = 0; i <4; i++)
		{
			switch (cmd[i])
			{
			case 'Z' :
				pasGauche = 1000;
				pasDroite  = 1000;
				break;
			case 'Q' :
				pasGauche = -525;
				pasDroite  = 525;
				break;
			case 'S' :
				pasGauche = -1000;
				pasDroite  = -1000;
				break;
			case 'D' :
				pasGauche = 525;
				pasDroite  = -525;
				break;
			}

			// ROBOT 1
			msg_ordre_com.ordres[i].robot1.pas_gauche = pasGauche;
			msg_ordre_com.ordres[i].robot1.pas_droite = pasDroite;


		}


		// ROBOT 2
		msg_ordre_com.ordres[0].robot2.pas_gauche = pasGauche;
		msg_ordre_com.ordres[0].robot2.pas_droite = pasDroite;

		for (int i = 1; i<4; i++)
		{
			msg_ordre_com.ordres[i].robot2.pas_gauche = 0;
			msg_ordre_com.ordres[i].robot2.pas_droite = 0;
		}


		mq_send(bal_ordre_com, (char*)&msg_ordre_com, sizeof(Msg_Ordre_Com), 0);
		//préparation de la réception

	}
	cout << "fin boucle" <<endl;
	mq_close(bal_ordre_com);
	mq_unlink(BAL_ORDRE_COM);
	return 0;
}
