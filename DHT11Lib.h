/*
 * DHT11Lib.h
 *
 * Created: 25-05-2017 21:38:56
 *  Author: Karol
 */ 


#ifndef DHT11LIB_H_
#define DHT11LIB_H_

#include <avr/io.h>
#include <stdbool.h>
#include "defines.h"

//functions

/**
   The function initialize data line.
*/
void DHT11_init(void);

/**
   The function read data from DHT11 and save them to buffer.
   After invoking this function, data returned by "DHT11_getXXX" functions are up-to-date.
   This function can executing longer than 10 ms.
   This function contains sensitive sections, therefore the interrupts must disabled in time of executing.
   Function return true if reading data was successful.
*/
bool DHT11_readData(void);

/**
   The function return integer part of temperature.
*/
uint8_t DHT11_getTempInt(void);

/**
   The function return decimal part of temperature.
*/
uint8_t DHT11_getTempDeci(void);

/**
   The function return integer part of RH value.
*/
uint8_t DHT11_getRHInt(void);

/**
   The function return decimal part of RH value.
*/
uint8_t DHT11_getRHDeci(void);

/**
   The function return true if crc is correct.
*/
bool DHT11_crcCheck(void);

#endif /* DHT11LIB_H_ */