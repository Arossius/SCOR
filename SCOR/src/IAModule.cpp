/*
 * IAModule.cpp
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#include "IAModule.h"

IAModule::IAModule() {
	// TODO Auto-generated constructor stub

}

IAModule::~IAModule() {
	// TODO Auto-generated destructor stub
}


void * IAModule::Launch(void * arg)
{
	return 0; // TODO : Retourner quelquechose (Armand)
}


void IAModule::run()
{
	for(;;)
	{




	}
}

E_DIRECTION_BALLE IAModule::DefinirDirectionBalle(Balle balle)
{
	return COMING; // TODO : Renvoyer un vrai truc. J'ai mis une valeur au pif pour que ça compile (Armand)
}

