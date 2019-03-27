/*
 * config.c
 *
 * Created: 14.03.2019 23:40:57
 *  Author: Karol
 */ 

#include "config.h"
#include <avr/eeprom.h>

#define WIFI_CONF_BUFFER_SIZE 25
#define CONNECTION_CONF_BUFFER_SIZE 65

/* Default values */
EEMEM uint8_t configChange = 1;
EEMEM char ssid_eeprom[WIFI_CONF_BUFFER_SIZE] = "wifi_name";
EEMEM char password_eeprom[WIFI_CONF_BUFFER_SIZE] = "";

EEMEM char host_eeprom[CONNECTION_CONF_BUFFER_SIZE] = "host.example.com";
EEMEM char topic_eeprom[CONNECTION_CONF_BUFFER_SIZE] = "topic/example";
EEMEM uint16_t port_eeprom = 1883;

EEMEM uint16_t period_eeprom = 2;


/* WIFI config */
static void setConfigChanged(uint8_t value){
	eeprom_write_byte(&configChange, value);
}


uint8_t isWiFiConfigChanged(){
	uint8_t isChanged = eeprom_read_byte(&configChange);
	if (isChanged != 0)
	{
		setConfigChanged(0);
	}
	return isChanged;
}

void getWiFiPassword(char* passwordBuffer, uint8_t len){
	if(len > WIFI_CONF_BUFFER_SIZE){
		len = WIFI_CONF_BUFFER_SIZE;	
	}
	eeprom_read_block(passwordBuffer, password_eeprom, len);
	passwordBuffer[len - 1] = '\0';
}

void getSSID(char* ssidBuffer, uint8_t len){
	if(len > WIFI_CONF_BUFFER_SIZE){
		len = WIFI_CONF_BUFFER_SIZE;
	}
	eeprom_read_block(ssidBuffer, ssid_eeprom, len);
	ssidBuffer[len - 1] = '\0';
}

void setWiFiPassword(char* passwordBuffer, uint8_t len){
	if(len > WIFI_CONF_BUFFER_SIZE){
		len = WIFI_CONF_BUFFER_SIZE;
	}
	eeprom_write_block(passwordBuffer, password_eeprom, len);
	setConfigChanged(1);
}
void setSSID(char* ssidBuffer, uint8_t len){
	if(len > WIFI_CONF_BUFFER_SIZE){
		len = WIFI_CONF_BUFFER_SIZE;
	}
	eeprom_write_block(ssidBuffer, ssid_eeprom, len);
	setConfigChanged(1);
}


/* connection settings */
void getHost(char* hostNameBuffer, uint8_t len){
		if(len > CONNECTION_CONF_BUFFER_SIZE){
			len = CONNECTION_CONF_BUFFER_SIZE;
		}
		eeprom_read_block(hostNameBuffer, host_eeprom, len);
		hostNameBuffer[len - 1] = '\0';
}

uint16_t getPort(){
	return eeprom_read_word(&port_eeprom);
}
void getTopic(char* topicBuffer, uint8_t len){
		if(len > CONNECTION_CONF_BUFFER_SIZE){
			len = CONNECTION_CONF_BUFFER_SIZE;
		}
		eeprom_read_block(topicBuffer, topic_eeprom, len);
		topicBuffer[len - 1] = '\0';
}

void setHost(char* hostNameBuffer, uint8_t len){
		if(len > CONNECTION_CONF_BUFFER_SIZE){
			len = CONNECTION_CONF_BUFFER_SIZE;
		}
		eeprom_write_block(hostNameBuffer, host_eeprom, len);
}
void setPort(uint16_t port){
	eeprom_write_word(&port_eeprom, port);
}
void setTopic(char* topicBuffer, uint8_t len){
		if(len > CONNECTION_CONF_BUFFER_SIZE){
			len = CONNECTION_CONF_BUFFER_SIZE;
		}
		eeprom_write_block(topicBuffer, topic_eeprom, len);
}

/* other settings */
uint16_t getPeriod(){
	return eeprom_read_word(&period_eeprom);
}
void setPeriod(uint16_t period){
	eeprom_write_word(&period_eeprom, period);
}
