
#include<stdio.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>


int main (int argc, char* argv[])
{
	int file, count;
	/*if(argc != 2)
	{

		printf("Invalid number of arguments, exiting!");
		return -2;

	}*/

	if((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY))< 0)
	{
		perror("UART: Failed to open the file");
		return -1;
	}

	struct termios options;
	tcgetattr(file, &options);
	options.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;
	options.c_cc[VMIN]  = 0;
	options.c_cc[VTIME] = 10;	// 1 sec timeout
	tcflush(file, TCIFLUSH);
	tcsetattr(file, TCSANOW, &options);
	char *strSend;
	sprintf(strSend, "%x", 0x32);

	if((count = write(file, strSend,strlen(strSend)+1))<0)		//argv[1]
	{
		perror("Failed to write to the output");
	}

	close(file);


	return 0;


}





