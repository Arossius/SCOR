/*
 * ComKh.cpp
 *
 *  Created on: 7 févr. 2012
 *      Author: adrien
 */
#include "ComKh.h"

ComKh::ComKh()
{
	portOpened = false;
}

ComKh::~ComKh() {


}


/*---------------------------------------------------------------------------

  Open and initialize serial connection

  Specify the serial device in the K_DEV environment variable.
  If K_DEV is not defined K_DEFAULT_DEV (#defined in k_talk.h) is used

  ---------------------------------------------------------------------------*/

int ComKh::init(const char *port)
{
	fileDescriptor=OpenSerialPort(port, K_DEFAULT_BAUD);

	if (fileDescriptor > 0)
	{
	  portOpened = true;
	}
	else
	{
	portOpened = false;
	}

  return fileDescriptor;
}



int ComKh::OpenSerialPort(const char *deviceFilePath, long baudrate)
{
    int fileDescriptor;
    struct termios options;

    fileDescriptor = open(deviceFilePath, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fileDescriptor == kMyErrReturn)
    {
        printf("Error opening serial port %s - %s(%d).\n",
               deviceFilePath, strerror(errno), errno);
        return kMyErrReturn;
    }

    do {
        if (fcntl(fileDescriptor, F_SETFL, 0) == kMyErrReturn)
        {
            printf("Error clearing O_NDELAY %s - %s(%d).\n",
                deviceFilePath, strerror(errno), errno);
            break;
        }

        // Get the current options and save them for later reset.
        if (tcgetattr(fileDescriptor, &sOriginalTTYAttrs) == kMyErrReturn)
        {
            printf("Error getting tty attributes %s - %s(%d).\n",
                deviceFilePath, strerror(errno), errno);
            break;
        }


        // Set raw input, one second timeout.
        // These options are documented in the man page for termios.
        options = sOriginalTTYAttrs;
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_oflag &= ~OPOST;
        options.c_cc[ VMIN ] = 0;
        options.c_cc[ VTIME ] = K_DEFAULT_TIMEOUT;
        options.c_ispeed = baudrate;
        options.c_ospeed = baudrate;
        clear(options.c_cflag, CSIZE|PARENB);            // Control modes
        set(options.c_cflag, CS8|CSTOPB);


        // Set the options.
        if (tcsetattr(fileDescriptor, TCSANOW, &options) == kMyErrReturn)
        {
            printf("Error setting tty attributes %s - %s(%d).\n",
                deviceFilePath, strerror(errno), errno);
            break;
        }

        return fileDescriptor;
    } while (0);

    // Failure path
    close(fileDescriptor);
    return kMyErrReturn;
}



/*---------------------------------------------------------------------------

  Read a line until and including '\n'
  Terminates result buffer with '\0' and returns number of bytes read

  ---------------------------------------------------------------------------*/

int ComKh::Readline(char *result, int size)
{
  int  nrd, rsize;
  char c;

  rsize = 0;

  do
  {
    nrd = read(fileDescriptor,&c,1);
    if (nrd < 0) break;
    result[rsize] = c;
    rsize += nrd;
    printf("---%c---",c);

  }
  while (nrd==1 && rsize < size && c != '\n');
  printf("END READ\n");
  result[rsize] = '\0';

  if (nrd>=0)
    return(rsize);
  else {
    perror("serial_readline(): error");
    return(nrd);
  }
}


/*---------------------------------------------------------------------------

  Empty input buffer

  ---------------------------------------------------------------------------*/

void ComKh::EmptyBuffer()
{
  char drain[K_MSG_SIZE];
  int  rsize;

  do {
    rsize = Readline(drain, K_MSG_SIZE);
  }
  while (rsize > 0); // Read till nothing more to read
}




/*---------------------------------------------------------------------------

  Send message line and receive result line.

  Checks that the first character of the result line is the lower case
  of the first character of the message line. Tries K_NB_RETRY times

  ---------------------------------------------------------------------------*/

int ComKh::sendMsg(const char *message, char *result, int size)
{
  int  rd, rsize, tries = 0;
  char respond = tolower(message[0]);

  result[0] = 0;

  do  {
      int wr;
      if((wr=serial_write(fileDescriptor,message,strlen(message))) <=0)
         {
         if (wr<0) perror("Error attempting to write\n");
             else printf("Nothing written\n");
         }

    rd = Readline(result, size);
    if (rd < 0) {                               // File error
      perror("k_talk(): read/write error");
      break;
    }
    else if (rd == 0)                           // No response
      tries++;

    else if (result[0] != respond) {            // Incorrect answer
      fputc(result[0],stderr);
      perror("k_talk(): protocol error");
      EmptyBuffer();
      tries++;
    }
    else {                                      // Correct answer

      // Strip terminating '\r\n' if any
      if ((rsize = strlen(result)) >= 2)  {
        if (result[rsize-1] == '\n') result[rsize-1] = '\0';
        if (result[rsize-2] == '\r') result[rsize-2] = '\0';
      }
      return TRUE;
    }
  }
  while (tries < K_NB_RETRY);

  if (rd == 0)
    perror("k_talk(): robot not responding");

  return FALSE;
}



/*---------------------------------------------------------------------------

  Close the serial connection

  ---------------------------------------------------------------------------*/

void ComKh::Close()
{
    // Reset the serial port to the state we found it in.
    if (tcsetattr(fileDescriptor, TCSANOW, &sOriginalTTYAttrs)
                     == kMyErrReturn)
    {
        printf("Error resetting tty attributes - %s(%d).\n",
            strerror(errno), errno);
    }
    close(fileDescriptor);
}

