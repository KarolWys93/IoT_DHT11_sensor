/*
 * DHT11Lib.c
 *
 * Created: 25-05-2017 21:39:16
 *  Author: Karol
 */

#include "DHT11Lib.h"
#include <util/delay.h>


// IN/OUT macros
#define DHT11_OUT() SET(DDR, DHT11_line)
#define DHT11_IN() CLR(DDR, DHT11_line)

// High/Low macros
#define DHT11_HIGH() SET(PORT, DHT11_line)
#define DHT11_LOW() CLR(PORT, DHT11_line)

//read pin state
#define  DHT11_LineState() GET(DHT11_line)

//Data buffer
uint8_t DHT11_dataBuffer[5];

//static functions prototypes
bool startPulse(void);


void DHT11_init(void){
	DHT11_LOW();
	DHT11_IN();
}

bool DHT11_readData(void){
	for (uint8_t i = 0; i<5; i++)
	{
		DHT11_dataBuffer[i] = 0;
	}
	if(!startPulse()){
		return false;
	}
	while(!DHT11_LineState());	//waiting for end of response signal
	while(DHT11_LineState());	//waiting for end of preparing data (waiting for LOW)

	//start of reading data
	
	for(uint8_t i = 0; i < 5; i++){
		for(uint8_t j = 0; j < 8; j++){
			while(!DHT11_LineState());	//waiting for high state
			_delay_us(35);
			if(DHT11_LineState()){
				DHT11_dataBuffer[i] |= (1<<(7-j));
			}
			while(DHT11_LineState());	//waiting for low state
		}
	}	
	return true;
}

uint8_t DHT11_getTempInt(void){
	return DHT11_dataBuffer[2];	
}
uint8_t DHT11_getTempDeci(void){
	return DHT11_dataBuffer[3];
}

uint8_t DHT11_getRHInt(void){
	return DHT11_dataBuffer[0];
}

uint8_t DHT11_getRHDeci(void){
	return DHT11_dataBuffer[1];
}

bool DHT11_crcCheck(void){
	uint8_t sum = 0; 
	for (uint8_t i = 0; i<4; i++)
	{
		sum += DHT11_dataBuffer[i];
	}
	if (sum == DHT11_dataBuffer[4])
	{
		return true;
	}
	else{
		return false;
	}
}

bool startPulse(void){
	bool state = true;
	//start pulse
	DHT11_LOW();
	DHT11_OUT();	//set line to low state
	_delay_ms(25);
	//end of start pulse
	
	//wait for DHT response pulse
	DHT11_IN();
	_delay_us(25);
	
	if(DHT11_LineState()){
		state = false;
	}else{
		state = true;
	}
	return state;
}