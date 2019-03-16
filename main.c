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
#include "config.h"
#include "hw_delay.h"
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
	
	sprintf(resultText, "change?: %u", isWiFiConfigChanged());
	sendLine(resultText);
	
	getSSID(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	
	getWiFiPassword(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	
	getHost(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	
	sprintf(resultText, "port: %u", getPort());
	sendLine(resultText);

	getTopic(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);	
	
	sprintf(resultText, "period: %u", getPeriod());
	sendLine(resultText);
	
    while (1) 
    {
		readLine(recivedText, sizeof recivedText / sizeof *recivedText);
		sendLine(recivedText);
		if(strcmp(recivedText, "cucumber") == 0){
			uint32_t t1 = getCurrentTime();
			hw_sleep_ms(1000);
			uint32_t t2 = getCurrentTime();
			sprintf(resultText, "t1: %lu t2: %lu", t1, t2);
			sendLine(resultText);
			ATOMIC_BLOCK(ATOMIC_FORCEON){
				DHT11_readData();
			}		
			sprintf(resultText, "%d.%d *C %d.%d hr [%%]",
				DHT11_getTempInt(),
				DHT11_getTempDeci(),
				DHT11_getRHInt(),
				DHT11_getRHDeci());
			
			sendLine(resultText);	
		}
		
		if (strcmp(recivedText, "ssid") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setSSID(recivedText, sizeof recivedText / sizeof *recivedText);
			sendLine("ok");
		}
		
		if (strcmp(recivedText, "pass") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setWiFiPassword(recivedText, sizeof recivedText / sizeof *recivedText);
			sendLine("ok");
		}
		
		if (strcmp(recivedText, "host") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setHost(recivedText, sizeof recivedText / sizeof *recivedText);
			sendLine("ok");
		}
		
		if (strcmp(recivedText, "port") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setPort(atoi(recivedText));
			sendLine("ok");
		}
		
		if (strcmp(recivedText, "topic") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setTopic(recivedText, sizeof recivedText / sizeof *recivedText);
			sendLine("ok");
		}
		
		if (strcmp(recivedText, "period") == 0)
		{
			sendLine(">");
			readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			setPeriod(atoi(recivedText));
			sendLine("ok");
		}
    }
}


static void init(void){
	DHT11_init();
	usartInit();
	hw_delay_init();
	sei();
}
