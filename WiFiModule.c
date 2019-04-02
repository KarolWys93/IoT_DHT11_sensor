/*
 * WiFiModule.c
 *
 * Created: 15.03.2019 23:40:59
 *  Author: Karol
 */ 

#include "WiFiModule.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//private variables
static char wifi_cmdBuffer[64];


WiFI_Status WiFi_reset(){
	WiFI_Status status = WiFi_ERROR;
	sendLine("AT+RST");
	while(1){
		readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer / sizeof *wifi_cmdBuffer, 10000);
		if(strcmp(wifi_cmdBuffer, "ready") == 0){
			status = WiFi_OK;
			break;
		}
	}
	return status;
}

WiFI_Status WiFi_SetNetwork(char* SSID, char* password){
	WiFI_Status status = WiFi_ERROR;

	strcpy(wifi_cmdBuffer, "AT+CWJAP=\"");
	strcat(wifi_cmdBuffer, SSID);
	strcat(wifi_cmdBuffer, "\",\"");
    strcat(wifi_cmdBuffer, password);
	strcat(wifi_cmdBuffer, "\"");

	sendLine(wifi_cmdBuffer);
	readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer/sizeof *wifi_cmdBuffer, 10000);
	
	if (strcmp(wifi_cmdBuffer, "OK") == 0)
	{
		status = WiFi_OK;
	}

	return status;
}

WiFI_Status WiFi_checkAPconnection(){
	WiFI_Status status = WiFi_ERROR;
	bool waitForAnswer = true;

	sendLine("AT+CWJAP?");
	do{
		readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer / sizeof *wifi_cmdBuffer, 10000);
		if(strcmp(wifi_cmdBuffer, "OK") == 0){
			waitForAnswer = false;
			status = WiFi_OK;
		}else if(strcmp(wifi_cmdBuffer, "ERROR") == 0){
			waitForAnswer = false;
		}
	}while(waitForAnswer);

	return status;
}

WiFI_Status WiFi_openConnection(char* adress, uint16_t port){
		WiFI_Status status = WiFi_ERROR;
		bool waitForAnswer = true;
		
		char portAsString[6];
		sprintf(portAsString, "%u", port);
		strcpy(wifi_cmdBuffer, "AT+CIPSTART=\"TCP\",\"");
		strcat(wifi_cmdBuffer, adress);
		strcat(wifi_cmdBuffer, "\",");
		strcat(wifi_cmdBuffer, portAsString);

		sendLine(wifi_cmdBuffer);
		do{
			readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer / sizeof *wifi_cmdBuffer, 10000);
			if(strcmp(wifi_cmdBuffer, "OK") == 0){
				waitForAnswer = false;
				status = WiFi_OK;
			}else if(strcmp(wifi_cmdBuffer, "ERROR") == 0){
				waitForAnswer = false;
				status = WiFi_ERROR;
			}else if(strcmp(wifi_cmdBuffer, "no ip") == 0){
				waitForAnswer = false;
				status = WiFi_NO_IP;
			}else if(strcmp(wifi_cmdBuffer, "ALREAY CONNECT") == 0){
				waitForAnswer = false;
				status = WiFi_ALREADY_CONNECT;
			}else if(strcmp(wifi_cmdBuffer, "DNS Fail") == 0){
				waitForAnswer = false;
				status = WiFi_DNS_FAIL;
			}
		}while(waitForAnswer);

		return status;
}

WiFI_Status WiFi_closeConnection(){
	WiFI_Status status = WiFi_ERROR;
	bool waitForAnswer = true;

	sendLine("AT+CIPCLOSE");
	do{
		readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer / sizeof *wifi_cmdBuffer, 10000);
		if(strcmp(wifi_cmdBuffer, "OK") == 0){
			waitForAnswer = false;
			status = WiFi_OK;
		}else if(strcmp(wifi_cmdBuffer, "ERROR") == 0){
			waitForAnswer = false;
		}
	}while(waitForAnswer);

	return status;
}

WiFI_Status WiFi_sendData(char* data, uint16_t dataLength){
	WiFI_Status status = WiFi_ERROR;
	bool waitForAnswer = true;
	char sign = '\0';

	sprintf(wifi_cmdBuffer, "AT+CIPSEND=%d", dataLength);

	sendLine(wifi_cmdBuffer);

	do{
		sign = readChar(0);
		if(sign == '>'){
			sendData(data, dataLength);
			do{
				readLine(wifi_cmdBuffer, sizeof wifi_cmdBuffer / sizeof *wifi_cmdBuffer, 10000);	//TODO add timeout
				if(strcmp(wifi_cmdBuffer, "SEND OK") == 0){
					waitForAnswer = false;
					status = WiFi_OK;
				}else{
					waitForAnswer = false;
				}
			}while(waitForAnswer);
			status = WiFi_OK;
			waitForAnswer = false;
		}else if(sign == 'l'){
			waitForAnswer = false;
		}
	}while(waitForAnswer);

	return status;
}
uint16_t WiFi_readData(char* data, uint16_t bufferLen, uint32_t timeout){
	return 15;
}

