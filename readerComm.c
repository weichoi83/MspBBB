/*
 * Msp430F5529 uart comm test
 *
 * Author: Rejina
 * Date: 5 July 2016
 *
 */

#include<stdio.h>
#include<fcntl.h>
#include<termios.h>
//#include<string.h>
#include<unistd.h>

int main (int argc, char* argv[])
{
	int file, count;
	//char str[80];

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

	struct termios oriopt, options;
	tcgetattr(file, &options);
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;

	options.c_cc[VMIN]  = 1;	// Non Canonical input mode (as we didnt set ICANON for c_lflag)
	options.c_cc[VTIME] = 10;	// 1 sec timeout
	tcflush(file, TCIFLUSH);
	oriopt = options;
	cfmakeraw(&options);

	tcsetattr(file, TCSANOW, &options);
	/*
	char *strSend;
	sprintf(strSend, "%x", 0x32);

	if((count = write(file, strSend,strlen(strSend)+1))<0)		//argv[1]
	{
		perror("Failed to write to the output");
	}
	*/
	unsigned char cmd[] = {0x01, 0x00, 0x32};	//"Tx From Beaglebone "; 		// {0x01, 0x00, 0x32};

	if ((count = write(file, (char*)cmd, 3)<0)){
	      perror("Failed to write to the output\n");
	      tcsetattr(file, TCSAFLUSH, &oriopt); 		//before exit, undo raw setting
	      return -1;
	 }else{
		 printf("Byte sent: %d \n", count);
	 }

	unsigned char receive[100];
	if ((count = read(file, (void *)receive, 100))<0)
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
		//sprintf(str, "Value of Pi = 0x%.2x", receive[0]);
		//puts(str);
		printf("Value of Pi = 0x%.2x\n", receive[0]);

	}
	tcsetattr(file, TCSAFLUSH, &oriopt);
	close(file);



	return 0;


}





