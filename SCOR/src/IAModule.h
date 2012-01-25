/*
 * IAModule.h
 *
 *  Created on: 25 janv. 2012
 *      Author: adrien
 */

#ifndef IAMODULE_H_
#define IAMODULE_H_

#include "Utils.h"


enum E_DIRECTION_BALLE {
	COMING,
	LEAVING,
	STOPPED
};






class IAModule {
public:
	IAModule();
	virtual ~IAModule();


	static void * Launch(void * arg);

protected:
	virtual void run();

	E_DIRECTION_BALLE DefinirDirectionBalle(Balle balle);




private:
	Msg_IA_Ordre dernierOrdre;
	Balle balle;
	Robot robot1;
	Robot robot2;

};

#endif /* IAMODULE_H_ */
