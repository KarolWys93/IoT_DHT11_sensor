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
#include "MQTT_client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void init(void);
static WiFI_Status changeWiFiConfig();

int main(void){
	
	init();
	config_mode();
	
	char recivedText[64];
	char sendBuffor[64];
	
	WiFi_enable();
	hw_sleep_ms(500);
	WiFi_reset(5000);
	hw_sleep_ms(5000);
	if (isWiFiConfigChanged())
	{
		changeWiFiConfig();
	}
	
	char host[65];
	char topic[65];
	getHost(host, 65);
	getTopic(topic, 65);
	
	char result[32];
	
    while (1){
		while(WiFi_checkAPconnection() != WiFi_OK){
			hw_sleep_ms(5000);
		}
		
		uint16_t len = MQTT_connectpacket((uint8_t *)sendBuffor, "", "");
		while(WiFi_openConnection(host, getPort()) != WiFi_OK){
			hw_sleep_ms(1000);
			WiFi_closeConnection();
			hw_sleep_ms(1000);
		}
		WiFi_sendData(sendBuffor, len);
		
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			DHT11_readData();
		}
		
		sprintf(result, "%d.%d *C %d.%d hr [%%]",
			DHT11_getTempInt(),
			DHT11_getTempDeci(),
			DHT11_getRHInt(),
			DHT11_getRHDeci());
			
		len = MQTT_publishPacket((uint8_t *) sendBuffor, topic, result, 0);
		hw_sleep_ms(1000);
		WiFi_sendData(sendBuffor, len);
		hw_sleep_ms(500);
		WiFi_closeConnection();
		hw_sleep_ms(1000*getPeriod());
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

static WiFI_Status changeWiFiConfig(){
	char wifiName[25];
	char wifiPass[25];
	
	getSSID(wifiName, 25);
	getWiFiPassword(wifiPass, 25);
	return WiFi_SetNetwork(wifiName, wifiPass);
}
