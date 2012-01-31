
/*************************************************************************
    Communication
                             -------------------
    début                : Novembre 2011
    copyright            : (C) 2011 SCOR Tournament Project
*************************************************************************/

//---------- Interface de la classe <Communication> (fichier Communication) ------
#if ! defined ( COMMUNICATION_H )
#define COMMUNICATION_H

//--------------------------------------------------- Interfaces utilisées
#include <string>
#include <termios.h>

using namespace std;
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//------------------------------------------------------------------------


class Communication
{
//----------------------------------------------------------------- PUBLIC

public:

	enum BaudRate
	{
	  CCOM_BR19200,
	  CCOM_BR9600,
	  CCOM_BR4800,
	  CCOM_BR2400,
	  CCOM_BR1200,
	  CCOM_BR600
	};

	enum ErrCOM
	{
	  ER_CCOM 			= -1000,
	  ER_PORT_COM_NOT_OPENED 	= Communication::ER_CCOM-1,
	  ER_PORT_COM_ALREADY_OPENED 	= Communication::ER_CCOM-2,
	  ER_BAUD_RATE_UNDEFINED 	= Communication::ER_CCOM-3,
	  ER_CAR_SIZE_UNDEFINED  	= Communication::ER_CCOM-4,
	  ER_PARITY_MODE_UNDEFINED  	= Communication::ER_CCOM-5,
	  ER_COUNT_STOP_BIT_UNDEFINED 	= Communication::ER_CCOM-6,
	  ER_CONFIGURATION_UNAPPLICABLE = Communication::ER_CCOM-7
	};
	
	enum Parity
	{
	  CCOM_NONE,
	  CCOM_EVEN, /* pair */
	  CCOM_ODD
	};

	enum DataBits
	{
	  CCOM_8_BITS,
	  CCOM_7_BITS,
	  CCOM_6_BITS,
	  CCOM_5_BITS
	};

	enum StopBits
	{
	  CCOM_1_BIT,
	  CCOM_2_BITS
	};

//---------------------------------------------------- Méthodes publiques
		void Close ();
		// Ferme le port COM, interromp la communication

		int Open ( string port );
		// Ouvre le port COM, configure la connexion.

		int ReceiveData(string & data);
		// Mode d'emploi :
		//		Lit sur le port COM jusqu'à avoir au moins MIN_MSG_SIZE (3)
		//	caractères et avoir lu la suite de caractères "\r\c".
		//		Le contenu de <data> est automatiquement réinitialisé.
		//		<data> ne contiendra pas les marqueurs de fin de message.
		//		Renvoie le nombre de caractères lus sauf les marqueurs de fin de
		// 	message.
		//
		// Contrat :
		//	Le port COM a dû préalablement être ouvert par appel à la méthode
		//	<Open>
		//

		int SendData(string & data);
		// Mode d'emploi :
		//	Envoie le message <data> sur le port de communication préalablement initialisé.
		//	Renvoie le nombre de caractères écrits.
		//
		// Contrat :
		//  <data> ne doit pas contenir le doublet de fin de message.
		//	La méthode <Open> a dû être appelée auparavant avec succès.
		//

		bool SetPortConfiguration(BaudRate baudRate, DataBits countBits, Parity parity, StopBits countBitsStop);
		
		
//-------------------------------------------- Constructeurs - destructeur
    Communication ( );
    
    
    virtual ~Communication( );
       

protected:
//----------------------------------------------------- Attributs protégés

private:
//------------------------------------------------------- Attributs privés
    int fileDescriptor;	// Le descripteur du fichier du robot
    bool portOpened;	// stocke l'état de la connexion
    termios configuration;

};

#endif // FIELD_H

