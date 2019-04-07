/*
 * temp_sensor.c
 *
 * Created: 10.03.2019 10:31:37
 * Author : IoT-team
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "defines.h"
#include "DHT11Lib.h"
#include "uart.h"
#include "settings.h"
#include "hw_delay.h"
#include "WiFiModule.h"
#include "single_bit_io.h"
#include "config_mode.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void init(void);

int main(void){
	
	init();
	config_mode();
	
	char recivedText[64];
	char resultText[64];
	
	WiFi_enable();
	hw_sleep_ms(500);
	WiFi_reset(5000);
	hw_sleep_ms(5000);
	while(1){
		if(WiFi_checkAPconnection() == WiFi_OK){
			strcpy(resultText, "GET / HTTP/1.1\r\nHost: jakitydzien.pl\r\n\r\n");
			WiFI_Status conn_status = WiFi_openConnection("jakitydzien.pl", 80);
			if (conn_status != WiFi_OK)
			{
				sendLine("kupa2");
			}else{
				WiFi_sendData(resultText, strlen(resultText));
				WiFi_readData(recivedText, sizeof recivedText/sizeof *recivedText, 10000);
				WiFi_closeConnection();
			}
		}else{
			sendLine("kupa");
		}
		hw_sleep_ms(60000);
	}
	
	
	
	
	
	
    while (1){
		hw_sleep_ms(1000*getPeriod());
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			DHT11_readData();
		}		
		sprintf(resultText, "%d.%d *C %d.%d hr [%%]",
			DHT11_getTempInt(),
			DHT11_getTempDeci(),
			DHT11_getRHInt(),
			DHT11_getRHDeci());
			
		sendLine(resultText);
	}
}


static void init(void){
	SET(PORT, config_mode_button);
	hw_delay_init();
	usartInit();
	DHT11_init();
	WiFi_init();
	WiFi_disable();
	sei();
}
