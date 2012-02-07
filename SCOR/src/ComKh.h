/*
 * ComKh.h
 *
 *  Created on: 7 févr. 2012
 *      Author: adrien
 */

#ifndef COMKH_H_
#define COMKH_H_

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>


#define TRUE 1
#define FALSE 0
//typedef int bool;

#define msleep(t) usleep(t*1000)

#define kMyErrReturn            -1
static struct termios sOriginalTTYAttrs;

#define K_MSG_SIZE		512
#define	K_DEFAULT_BAUD		38400
#define	K_DEFAULT_TIMEOUT	5
#define	K_NB_RETRY		3

/*---------------------------------------------------------------------------

  Management of the serial line

  ---------------------------------------------------------------------------*/

#define clear(var, mask)                  var &= (~(mask))
#define set(var, mask)                    var |= (mask)
#define let(var, mask)
#define unused(var, mask)
#define	serial_close(fd)                  close(fd)
#define	serial_read(fd, buffer, size)     read(fd, buffer, size)
#define	serial_write(fd, buffer, size)    write(fd, buffer, size)



class ComKh {
public:
	ComKh();
	virtual ~ComKh();
	int OpenSerialPort(const char *deviceFilePath, long baudrate);
	void EmptyBuffer();
	int Readline(char *result, int size);
	int init(const char *port);
	int sendMsg(char *message, char *result, int size);
	void Close();

private:
    int fileDescriptor;	// Le descripteur du fichier du robot
    bool portOpened;	// stocke l'état de la connexion
    termios configuration;

};

#endif /* COMKH_H_ */
