/*
 * temp_sensor.c
 *
 * Created: 10.03.2019 10:31:37
 * Author : IoT-team
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "defines.h"
#include "DHT11Lib.h"
#include "uart.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void init(void);

int main(void)
{
	char recivedText[64];
	char resultText[64];
	
	init();	
	sendLine("Hello");
    while (1) 
    {
		readLine(recivedText, sizeof recivedText / sizeof *recivedText);
		sendLine(recivedText);
		if(strcmp(recivedText, "cucumber\r\n") == 0){
			DHT11_readData();
			sprintf(resultText, "%d.%d *C %d.%d hr [%%]",
				DHT11_getTempInt(),
				DHT11_getTempDeci(),
				DHT11_getRHInt(),
				DHT11_getRHDeci());
			
			sendLine(resultText);	
		}else{
			sendLine("Nope");	
		}
    }
}


static void init(void){
	DHT11_init();
	usartInit();
	
	sei();
}
