/*
 * Utils.h
 *
 *  Created on: 18 janv. 2012
 *      Author: adrien
 */

#ifndef UTILS_H_
#define UTILS_H_

struct Robot
{
	int pos_x;
	int pos_y;
	double angle;
};

struct Balle
{
	int pos_x;
	int pos_y;
	double vit_x;
	double vit_y;
};

struct Msg_Vid_Robot
{
	Robot robot1;
	Robot robot2;
	Balle balle;
};

typedef Msg_Vid_Robot Msg_Robot_IA ;

struct Msg_IA_Ordre
{
	Robot robot1;
	Robot robot2;
};

struct Pas_Robot
{
	int pas_gauche;
	int pas_droite;
};

struct Msg_Com_Robot
{
	Pas_Robot robot1;
	Pas_Robot robot2;
};

typedef Msg_Com_Robot Msg_Ordre_Com;

#endif /* UTILS_H_ */

