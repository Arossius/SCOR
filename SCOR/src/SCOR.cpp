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
#include "Ordre_Thread.h"
#include "IAModule_Thread.h"
using namespace std;

extern Robot robot1;
extern Robot robot2;


void TestCom()
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
}







void testOrdre_Com()
{
	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_IA_Ordre);

	mqd_t bal_ia_ordre = mq_open(BAL_IA_ORDRE, O_WRONLY | O_CREAT, S_IRWXU, &att);

	Communication_Thread comTH;
	comTH.Launch();

	Ordre_Thread ordreTH;
	ordreTH.Launch();


	string a("aaaaaaaaaaaaaaa");
	while (a != "exit")
	{
		cin >> a;
		Msg_IA_Ordre msg;
		msg.ordre_robot1.type_Ordre_Robot = Shoot_Left;
		msg.ordre_robot1.robot.angle = 10;
		msg.ordre_robot1.robot.pos_x = 1000;
		msg.ordre_robot1.robot.pos_y = 5000;
		msg.ordre_robot2.type_Ordre_Robot = Do_nothing;


		mq_send(bal_ia_ordre, (char*)&msg, sizeof(Msg_IA_Ordre), 0);

		/*mq_receive(coucou2, (char*)&msg2, sizeof(Msg_Ordre_Com), NULL);
		cout << msg2.ordres[0].robot1.pas_droite << endl;
		cout << msg2.ordres[0].robot1.pas_gauche << endl;
		cout << msg2.ordres[1].robot1.pas_droite << endl;
		cout << msg2.ordres[1].robot1.pas_gauche << endl;
		cout << msg2.ordres[2].robot1.pas_droite << endl;
		cout << msg2.ordres[2].robot1.pas_gauche << endl;
		cout << msg2.ordres[3].robot1.pas_droite << endl;
		cout << msg2.ordres[3].robot1.pas_gauche << endl;*/
	}



	mq_close(bal_ia_ordre);
	mq_unlink(BAL_IA_ORDRE);
	cout << "c'est fini" << endl;


}




void testIA_Ordre_Com()
{
	robot1.angle = 0;
	robot1.pos_x = 0;
	robot1.pos_y = 0;

	robot2.angle = 180;
	robot2.pos_x = 480;
	robot2.pos_y = 0;

	mq_attr att;
	att.mq_maxmsg = 10;
	att.mq_msgsize = sizeof(Msg_Robot_IA);

	mqd_t bal_robot_ia = mq_open(BAL_ROBOT_IA, O_WRONLY | O_CREAT, S_IRWXU, &att);

	Communication_Thread comTH;
	comTH.Launch();

	Ordre_Thread ordreTH;
	ordreTH.Launch();

	IAModule_Thread iamoduleTH;
	iamoduleTH.Launch();



	string a("aaa");
	while (a != "exit")
	{

		cout << "a" <<endl;
		cin >> a;
		cout << "a" <<endl;
		Msg_Robot_IA msg;

		msg.balle.pos_x = 120;
		msg.balle.pos_y = 80;
		msg.balle.vit_x = 0;
		msg.balle.vit_y = 0;


		msg.robot1.pos_x = 0;
		msg.robot1.pos_y = 0;
		msg.robot1.angle = 0;

		msg.robot2.pos_x = 480;
		msg.robot2.pos_y = 0;
		msg.robot2.angle = -90;


		cout << "mq_send" <<endl;
		int i = mq_send(bal_robot_ia, (char*)&msg, sizeof(Msg_Robot_IA), 0);
		cout << "i=" << i << endl;


		sleep(15);

		msg.balle.pos_x = LARGEUR_TERRAIN/8;
		msg.balle.pos_y = LONGUEUR_TERRAIN/4;
		msg.balle.vit_x = 0;
		msg.balle.vit_y = 0;


		msg.robot1 = INIT_ROBOT1;

		msg.robot2.pos_x = 480;
		msg.robot2.pos_y = 0;
		msg.robot2.angle = -90;


		cout << "mq_send" <<endl;
		i = mq_send(bal_robot_ia, (char*)&msg, sizeof(Msg_Robot_IA), 0);
		cout << "i=" << i << endl;

	}



	mq_close(bal_robot_ia);
	mq_unlink(BAL_ROBOT_IA);
	cout << "c'est fini" << endl;


}






int main()
{

	//testOrdre_Com();
	//TestCom();
	testIA_Ordre_Com();

	return 0;
}



