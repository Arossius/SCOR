/*************************************************************************
    Communication
                             -------------------
    début                : Novembre 2011
    copyright            : (C) 2011 SCOR Tournament Project
*************************************************************************/

//---------- Réalisation de la classe <Communication> (fichier Communication.cpp) --

//---------------------------------------------------------------- INCLUDE

//-------------------------------------------------------- Include système
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>

//------------------------------------------------------ Include personnel
#include "Communication.h"

//------------------------------------------------------------- Constantes
/* Taille minimale d'un message envoyé par le Khepera avec les caratères
 * de fin de message
 */
static const unsigned int MIN_MSG_SIZE = 3; // 1 caractère + 2 caractères fin de message

/* Caractères de fin de chaîne envoyé par le Khepera pour identifier la fin
 * d'un message.
 */
static const char END_RCV_COMMAND_1 = 13;
static const char END_RCV_COMMAND_2 = 10;



//----------------------------------------------------- Méthodes publiques
void Communication::Close()
{
#ifdef MAP
	cout << "Fermeture du port COM" << endl;
#endif
	if(portOpened)
		close(fileDescriptor);
	portOpened = false;
}


int Communication::Open(string port)
{
#ifdef MAP
	cout << "Ouverture du port COM : " << port << endl;
#endif
	if(portOpened)
	{
	  return ER_PORT_COM_ALREADY_OPENED;
	}

	/* Ouverture du port COErreurM <port> */
	if((fileDescriptor = open(port.c_str(),O_RDWR | O_NOCTTY | O_NONBLOCK))<0)
	{
	  return ER_PORT_COM_NOT_OPENED;
	}

	portOpened = true;
	
	SetPortConfiguration(Communication::CCOM_BR9600, Communication::CCOM_8_BITS, Communication::CCOM_NONE, Communication::CCOM_1_BIT);

	
	return true;
} // --- Fin de <Open>




bool Communication::SetPortConfiguration(BaudRate baudRate, DataBits countBits, Parity parity, StopBits countBitStop)
{
	if(!portOpened)
	{
	  return ER_PORT_COM_NOT_OPENED;;
	}
	
	
	/* Initialisation du port COM */
	// Modes locaux
	configuration.c_lflag &= ~ECHO;
	// Modes d'entrée
	configuration.c_iflag = IGNBRK | IGNPAR;
	// Modes de sortie
	configuration.c_oflag = 0;
	// Modes de contrôle
	configuration.c_cflag = CREAD | CLOCAL;
	configuration.c_cflag = configuration.c_cflag & ~CRTSCTS;
	
	// Vitesse
	switch(baudRate)
	{
		case Communication::CCOM_BR600  :	configuration.c_cflag |= B600; break;
		case Communication::CCOM_BR1200 :	configuration.c_cflag |= B1200; break;
		case Communication::CCOM_BR2400 :	configuration.c_cflag |= B2400; break;
		case Communication::CCOM_BR4800 :	configuration.c_cflag |= B4800; break;
		case Communication::CCOM_BR9600 :	configuration.c_cflag |= B9600; break;
		case Communication::CCOM_BR19200:	configuration.c_cflag |= B19200; break;
		default:
			return Communication::ER_BAUD_RATE_UNDEFINED;
	}
	// Longueur des caractères
	switch(countBits)
	{
		case Communication::CCOM_5_BITS :	configuration.c_cflag |= CS5; break;
		case Communication::CCOM_6_BITS :	configuration.c_cflag |= CS6; break;
		case Communication::CCOM_7_BITS :	configuration.c_cflag |= CS7; break;
		case Communication::CCOM_8_BITS :	configuration.c_cflag |= CS8; break;
		default:
			return Communication::ER_CAR_SIZE_UNDEFINED;
	}
	// Parité
	switch(parity)
	{
		case Communication::CCOM_NONE : /* rien à faire */ break;
		case Communication::CCOM_ODD	 : configuration.c_cflag |= PARODD;
		case Communication::CCOM_EVEN : configuration.c_cflag |= PARENB; break;
		default:
			return Communication::ER_PARITY_MODE_UNDEFINED;
	}
	// Nombre de bits de stop
	switch(countBitStop)
	{
		case Communication::CCOM_1_BIT  : /* rien à faire */ break;
		case Communication::CCOM_2_BITS : configuration.c_cflag |= CSTOPB; break;
		default:
			return Communication::ER_COUNT_STOP_BIT_UNDEFINED;
	}

	
	// Envoi de la configuration
	if(tcsetattr(fileDescriptor,TCSANOW,&configuration)==-1)
	{
	  return Communication::ER_CONFIGURATION_UNAPPLICABLE;
	}
	tcflush(fileDescriptor,TCIOFLUSH);
	
	return true;
} // --- Fin de <SetPortConfiguration>




int Communication::ReceiveData(string & data)
// ALgorithme :
//	Répéter
//		Lire caractère
//		Affecter caractère lu à chaîne de destination
//	Répéter jusqu'à au moins avoir lu 3 caractères et que les 2 derniers
//	correspondent aux marqueurs de fin de message.
//
{
	char c_car;
	data.clear();
	do
	{
		if(read(fileDescriptor,&c_car,1) > 0)
		{
			data += c_car;
		}
		else
			usleep(500);
	}

	// Pour faire bien, il faudrait sortir la condition de retour et
	// la rendre indépendante du Khepera ... mais là on a pas le temps
	while(data.length() < MIN_MSG_SIZE ||
			( data[data.length()-2] != END_RCV_COMMAND_1
			&& data[data.length()-1] != END_RCV_COMMAND_2));

	data.resize(data.length()-2);
	return data.length();
} // --- Fin de <ReceiveData>


int Communication::SendData(string & data)
{
	return write(fileDescriptor,data.c_str(),data.length());
}


Communication::Communication ( )
{
#ifdef MAP
    cout << "Appel au constructeur de <Communication>" << endl;
#endif
    portOpened = false;
}

Communication::~Communication()
{
#ifdef MAP
	cout << "Appel au destructeur de <Communication>" << endl;
#endif
	if (portOpened) close(fileDescriptor);
}
