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
	
	
	sendLine("Config mode");
	sendLine("-----------");
	sendLine("");
	
	sendLine("SSID:");
	getSSID(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	
	sendLine("Pass:");
	sendLine("********");
	
	sendLine("Host:");
	getHost(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);

	sprintf(resultText, "Port: %u", getPort());
	sendLine(resultText);

	sendLine("Topic:");
	getTopic(resultText, sizeof resultText / sizeof *resultText);
	sendLine(resultText);
	
	sprintf(resultText, "Period: %u", getPeriod());
	sendLine(resultText);
	
	
	 while (1)
	 {
		 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
		 sendLine(recivedText);
		 
		 if (strcmp(recivedText, "SSID") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setSSID(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Pass") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setWiFiPassword(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Host") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setHost(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Port") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setPort(atoi(recivedText));
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Topic") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setTopic(recivedText, sizeof recivedText / sizeof *recivedText);
			 sendLine("ok");
		 }
		 
		 if (strcmp(recivedText, "Period") == 0)
		 {
			 sendLine(">");
			 readLine(recivedText, sizeof recivedText / sizeof *recivedText);
			 setPeriod(atoi(recivedText));
			 sendLine("ok");
		 }
	 }
}