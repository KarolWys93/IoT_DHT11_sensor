/*
 * config_mode.c
 *
 * Created: 26.03.2019 23:44:14
 *  Author: Karol
 */ 

#include "config_mode.h"
#include <avr/io.h>
#include "uart.h"
#include "config.h"
#include "single_bit_io.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void config_mode(void){
	
	//if button is release, return from config mode
	if (GET(config_mode_button))
	{
		return;
	}
	
	char recivedText[64];
	char resultText[64];
	memset(recivedText, 0, 64);
	memset(recivedText, 0, 64);
	
	sendLine("Config mode");
	sendLine("-----------");
	sendLine("");
	
	sendLine("SSID:");
	getSSID(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	sendLine("");
	
	sendLine("Pass:");
	sendLine("********");
	sendLine("");
	
	sendLine("Host:");
	getHost(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	sendLine("");

	sprintf(resultText, "Port: %u", getPort());
	sendLine(resultText);
	sendLine("");

	sendLine("Topic:");
	getTopic(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	sendLine("");
	
	sprintf(resultText, "Period: %u", getPeriod());
	sendLine(resultText);
	
	 while (1)
	 {
		 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
		 sendLine(recivedText);
		 
		 if (strcmp(recivedText, "SSID") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setSSID(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Pass") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setWiFiPassword(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Host") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setHost(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Port") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setPort(atoi(recivedText));
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Topic") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setTopic(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Period") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText, 0);
			 setPeriod(atoi(recivedText));
			 sendLine("ok");
		 }
	 }
}