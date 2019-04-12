/*
 * config_mode.c
 *
 * Created: 26.03.2019 23:44:14
 *  Author: Karol
 */ 

#include "config_mode.h"
#include <avr/io.h>
#include "uart.h"
#include "settings.h"
#include "single_bit_io.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void printConfig(void){
		WiFiConfig wifiConfig;
		MqttConfig mqttConfig;
		getWiFiConfig(&wifiConfig);
		getMQTTConfig(&mqttConfig);
		char buffer[20];
		memset(buffer, 0, 20);
			
		sendLine("Config mode");
		sendLine("-----------");
		
		sendLine("SSID:");
		sendLine(wifiConfig.ssid);

		sendLine("Pass:");
		sendLine("********");

		sendLine("Host:");
		sendLine(mqttConfig.host);

		sprintf(buffer, "Port: %u", mqttConfig.port);
		sendLine(buffer);

		sendLine("Topic:");
		sendLine(mqttConfig.topic);
		
		sendLine("MQTT_User:");
		sendLine(mqttConfig.mqtt_user);
		
		sendLine("MQTT_Pass:");
		sendLine("********");

		sprintf(buffer, "Period: %u", getPeriod());
		sendLine(buffer);
}

static void sendOK(void){
	sendLine("ok");
}

static void getSetting(char *buffer, uint16_t len){
	sendLine(">");
	readLine(buffer, len, 0);
}

void config_mode(void){
	
	//if button is release, return from config mode
	if (GET(config_mode_button))
	{
		return;
	}
	
	char textBuffer[64];
	memset(textBuffer, 0, 64);
	printConfig();
	
	 while (1)
	 {
		 readLine(textBuffer, sizeof textBuffer / sizeof *textBuffer, 0);
		 sendLine(textBuffer);
		 
		 if (strcmp(textBuffer, "SSID") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setSSID(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "Pass") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setWiFiPassword(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "Host") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setHost(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "Port") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setPort(atoi(textBuffer));
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "Topic") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setTopic(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "MQTT_User") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setMqttUser(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
				  
		 if (strcmp(textBuffer, "MQTT_Pass") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setMqttPass(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 sendOK();
		 }
		 
		 if (strcmp(textBuffer, "Period") == 0)
		 {
			 getSetting(textBuffer, sizeof textBuffer / sizeof *textBuffer);
			 setPeriod(atoi(textBuffer));
			 sendOK();
		 }
	 }
}