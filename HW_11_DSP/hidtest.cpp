#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255

#define NUM_DATAPOINTS 10000

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;

	int i,j; //Counters
	short accels[NUM_DATAPOINTS][3]; //List of datasets with x,y,z accelerations
	unsigned char msb; //Temp variable to hold most significant byte of 16-bit...
	//two's compliment
	unsigned char lsb; //Least significant byte

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// SM: Send a message (cmd 0x1). The first byte is the report number (0x0).
	// buf[0] = 0x0;
	// buf[1] = 0x01;//Tells the pic to read the buffer 
	// printf("Enter row to display:\n");
	// scanf("%d",&buf[2]);
	// printf("Enter text to display:\n");
	// scanf("%s",&buf[3]);
	// res = hid_write(handle, buf, 65);

	// Print out the first two elements of the returned buffer.
	// for (i = 0; i < 2; i++)
	// 	printf("buf[%d]: %d\n", i, buf[i]);

	for(j=0;j<=NUM_DATAPOINTS; j++){
		// Request accel data (cmd 0x02) NUM_DATAPOINTS times. 

		//Initial read of buffer
		buf[0] = 0x00; 	//The first byte is the report number (0x0).
		buf[1] = 0x02; //Instruct the PIC to reply with data
		res = hid_write(handle, buf, 65);
		// Read requested stat
		res = hid_read(handle, buf, 65);

		//As long as the host is not supposed to read the packet
		//(empty packet) refresh the buffer with a new read
		while(buf[0]!=1){
		buf[0] = 0x00; 	//The first byte is the report number (0x0).
		buf[1] = 0x02; //Instruct the PIC to reply with data
		res = hid_write(handle, buf, 65);
		// Read requested stat
		res = hid_read(handle, buf, 65);
		}	

		//Loop reads all 3 accelerations
		for (i=0;i<=2;i++)
		{
			lsb = buf[2*i+1];//Read least significant byte of an axis-accel
			msb = buf[2*i+2];//Read most significant  byte of an axis-accel
			accels[j][i] = (msb << 8 ) | (lsb & 0xff); //Combine lsb and msb
		}
		// printf("Datapoint: %d, ax: %hi, ay: %hi, az: %hi\n",j,accels[j][0],accels[j][1],accels[j][2]);
	}

	// // Finalize the hidapi library
	res = hid_exit();

	//Print collected data to an output file
	FILE *fp; //File pointer
	//Double slashes necessary in file path
	fp = fopen("C:\\Users\\Sherif\\Documents\\Courses\\ME433_Advanced_Mechatronics\\Homework\\HW8\\accel_data.txt","w");
	for(j=0;j<=NUM_DATAPOINTS; j++){
		fprintf(fp,"%3.2f %3.2f %3.2f\n",((float)accels[j][0])/16000,((float)accels[j][1])/16000,((float)accels[j][2])/16000);
	}
	fclose(fp);

	// while(1){}//Keep terminal open before exiting for debugging
	return 0;
}