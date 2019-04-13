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
#include "rand8bit.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void init(void);
static void work(void);
static WiFI_Status changeWiFiConfig();

int main(void){
	init();
	config_mode();
	work();
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
	WiFiConfig wifiConfig;
	getWiFiConfig(&wifiConfig);
	return WiFi_SetNetwork(wifiConfig.ssid, wifiConfig.password);
}

static void generateDeviceID(void){
	if (!isDeviceIDok())
	{
		char buffor[32];
		memset(buffor, 0, 32);
		sendLine("AT+CIPSTAMAC?");
		do{
			readLine(buffor, 32, 5000);
		}while(buffor[0] != '+' || buffor[2] != 'I');	//1 and 3 letter of +CIPSTAMAC:
		
		uint8_t mac_sum = 0;
		for (uint8_t i = 0; i < strlen(buffor); i++)
		{
			mac_sum += buffor[i];
		}
		setSeed(mac_sum);
		for(uint8_t i = 0; i < DEVICE_ID_BUFFER_SIZE-2; i++)
		{
			uint8_t sign = (generateRand())%62;
			if (sign < 10) {sign+=48;}
			else if(sign >= 10 && sign < 36) {sign+=55;}
			else {sign+=61;}
			buffor[i] = sign;
		}
		buffor[DEVICE_ID_BUFFER_SIZE-1] = 0;
		setDeviceID(buffor);
	}
}

static void startWiFi(){
	WiFi_enable();
	hw_sleep_ms(500);
	WiFi_reset(5000);
	generateDeviceID();
	if (isWiFiConfigChanged())
	{
		changeWiFiConfig();
		hw_sleep_ms(10000);
	}
}

static void work(void){
	char recivedText[64];
	char sendBuffor[64];
	
	startWiFi();
	
	MqttConfig mqttConfig;
	getMQTTConfig(&mqttConfig);
	
	char deviceID[DEVICE_ID_BUFFER_SIZE];
	getDeviceID(deviceID);
	
	char result[32];
	
	while (1){
		while(WiFi_checkAPconnection() != WiFi_OK){
			hw_sleep_ms(5000);
		}
		
		uint16_t len = MQTT_connectpacket((uint8_t *)sendBuffor, deviceID, mqttConfig.mqtt_user, mqttConfig.mqtt_pass);
		while(WiFi_openConnection(mqttConfig.host, mqttConfig.port) != WiFi_OK){
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
		
		len = MQTT_publishPacket((uint8_t *) sendBuffor, mqttConfig.topic, result, 0, 1);
		hw_sleep_ms(1000);
		WiFi_sendData(sendBuffor, len);
		hw_sleep_ms(500);
		WiFi_closeConnection();
		hw_sleep_ms(1000*getPeriod());
	}
}
