/*
 * Msp430F5529 uart comm test
 *
 * Author: Rejina
 * Date: 5 July 2016
 *
 * Commit git hub:8 July
 */

#include<stdio.h>
#include<fcntl.h>
#include<termios.h>
//#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include "readerComm.h"


pthread_t uartRxT;

int main (int argc, char* argv[])
{
	int file, count;
	//char str[80];
	pthread_attr_t attr;
	void *status;

	if(argc != 2)
	{

		printf("Invalid number of arguments, exiting!");
		return -2;

	}

	if((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY ))< 0)
	{
		perror("UART: Failed to open the file");
		return -1;
	}

	struct termios options; //oriopt
	tcgetattr(file, &options);
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;

	options.c_cc[VMIN]  = 1;	// Non Canonical input mode (as we didnt set ICANON for c_lflag)
	options.c_cc[VTIME] = 10;	// 1 sec timeout
	tcflush(file, TCIFLUSH);
	//oriopt = options;
	//cfmakeraw(&options);

	tcsetattr(file, TCSANOW, &options);

	unsigned char cmd[] = {0x01, 0x89, 0x32};	//"Tx From Beaglebone ";

	if ((count = write(file, (char*)cmd, 3)<0)){
	      perror("Failed to write to the output\n");
	      //tcsetattr(file, TCSAFLUSH, &oriopt); 		//before exit, undo raw setting
	      return -1;
	 }else{
		 printf("Byte sent: %d \n", count);
	 }

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Create Uart Rx thread
	if(pthread_create(&uartRxT, &attr, uartRxWait, (void *)&file)){
	    	perror("GPIO: Failed to create the poll thread");

	    	return -1;
	}

	pthread_attr_destroy(&attr);
	pthread_join(uartRxT,&status);

	//tcsetattr(file, TCSAFLUSH, &oriopt);

	close(file);
	pthread_exit(NULL);

	return 0;

}

// UART RX thread
void* uartRxWait(void *fd){

	unsigned char receive[100];
	int count;

	while(1){
		if ((count = read(*(int *)fd, (void *)receive, 100))<0)
		{
			perror("Failed to read from input \n");
		}else{

			receive[count]=0;

			if(receive[2]==0x32){
				printf("Header received \n");
				// TBD start the ADC capture & store the data in file (takes time)
				// Thus, doing Memory swapping here(ping pong mode)
				//
			}
			printf("Byte received [%d] \n", count);
			printf("Value of Pi = 0x%.2x\n", receive[0]);

		}
	}

}





