/*
 * Utils.h
 *
 *  Created on: 18 janv. 2012
 *      Author: adrien
 */

#ifndef UTILS_H_
#define UTILS_H_

/* Definition des structures utilisées pour les échanges entre tâches */

/* Structure définissant la position d'un robot */

struct Robot
{
	int pos_x;
	int pos_y;
	double angle;
};

/* Structure définissant la position et la vitesse d'une balle */

struct Balle
{
	int pos_x;
	int pos_y;
	double vit_x;
	double vit_y;
};

/* Définit les ordres possibles que l'IA peut donner à un robot */

enum Type_Ordre_Robot
{
	Do_nothing,
	Move,
	Shoot_Left,
	Shoot_Right
};

/* Structure définissant un ordre donnée à un robot */

struct Ordre_Robot
{
	/* Type d'ordre */
	Type_Ordre_Robot type_Ordre_Robot;
	/* éventuel information complémentaire */
	Robot robot;
};

/* Structure définissant les messages échangés dans la boîte aux lettres
 * Bal_Vid_Robot */

struct Msg_Vid_Robot
{
	Robot robot1;
	Robot robot2;
	Balle balle;
};

/* Structure définissant les messages échangés dans la boîte aux lettres
 * Bal_Robot_IA */

typedef Msg_Vid_Robot Msg_Robot_IA ;

/* Structure définissant les messages échangés dans la boîte aux lettres
 * Bal_IA_Ordre */

struct Msg_IA_Ordre
{
	Ordre_Robot ordre_robot1;
	Ordre_Robot ordre_robot2;
};

/* Structure définissant un mouvement à effectuer par un robot */

struct Pas_Robot
{
	int pas_gauche;
	int pas_droite;
};

/* Structure définissant les messages échangés dans la boîte aux lettres
 * Bal_Com_Robot */


struct Pas_Robots
{
	Pas_Robot robot1;
	Pas_Robot robot2;
};

struct Msg_Ordre_Com
{
	Pas_Robots ordres[4];
};



/* Structure définissant les messages échangés dans la boîte aux lettres
 * Bal_Ordre_Com */

typedef Pas_Robots Msg_Com_Robot;

/* Fin de la définition des structures */

/* Définition des noms utilisés pour les boites aux lettres. */

#define BAL_VIDEO_ROBOT "bal_video_robot"
#define BAL_COM_ROBOT "bal_com_robot"
#define BAL_IA_ORDRE "bal_ia_ordre"
#define BAL_ROBOT_IA "bal_robot_ia"
#define BAL_ORDRE_COM "/bal_ordre_com"


/*
 * Dimensions du robot
 */
const int RAYON_ROBOT = 50;
const int ANGLE_VISEE = 90;

/* Définition de la carte en coordonnées (X,Y) de déplacement
 * des robots et de la balle ainsi que des positions de départ
 * des robots
 */
const int LARGEUR_TERRAIN = 480;
const int LONGUEUR_TERRAIN = 640;

const int LIGNE_DEFENSE =  LONGUEUR_TERRAIN/8;

const Robot INIT_ROBOT1 = { LARGEUR_TERRAIN/4, LIGNE_DEFENSE, ANGLE_VISEE-90 };
const Robot INIT_ROBOT2 = { 3*LARGEUR_TERRAIN/4, LIGNE_DEFENSE, ANGLE_VISEE + 90 };



/*
 * Descripteurs de fichiers des deux robots
 */
#define FICHIER_ROBOT_1 "/dev/ttyUSB0"
#define FICHIER_ROBOT_2 "/dev/ttyUSB1"



#endif /* UTILS_H_ */

