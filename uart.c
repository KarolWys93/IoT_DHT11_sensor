/*
 * uart.c
 *
 * Created: 16-06-2017 16:23:11
 *  Author: Karol
 */ 

#include "uart.h"
#include "hw_delay.h"
#include <avr/interrupt.h>
#include <stddef.h>
#include <util/atomic.h>
#include <string.h>

volatile static unsigned int usartRxCounter;
volatile static unsigned int usartRxBuffLen;

static char* toSendData;
static uint8_t toSendLen = 0;
static  char* recivedBuff;

volatile static bool uartTxBusy = false;
volatile static bool newLine = false;

void usartInit(void)
{
	// ustawienie transmisji
	#define BAUD 9600        //9600bps standardowa predkosc transmisji modulu HC-05
	#include <util/setbaud.h> //linkowanie tego pliku musi byæ
	//po zdefiniowaniu BAUD

	//ustaw obliczone przez #define wartoœci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |= (1 << U2X);
	#else
	UCSRA &= ~(1 << U2X);
	#endif

	//Ustawiamy pozostale parametry modul USART

	//standardowe parametry transmisji modulu HC-05
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);  //bitów danych: 8
	//bity stopu:  1
	//parzystoœæ:  brak
	//wlacz nadajnik i odbiornik oraz ich przerwania odbiornika
	//przerwania nadajnika wlaczamy w funkcji wyslij_wynik()
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
}

bool uratTxIsBusy(void){
	return uartTxBusy;
}


void sendLine(char *text){
	sendData(text, strlen(text));
	while(uartTxBusy);
	sendData("\r\n", 2);
}

void sendData(char *text, uint8_t len){
	//Zaczekaj, az bufor nadawania bedzie pusty
	while (!(UCSRA & (1 << UDRE)));
	toSendData = text;
	toSendLen = len;
	uartTxBusy = true;
	//rozpoczêcie transmisji
	UCSRB |= (1 << UDRIE);
}

void setRxBuffer(char* buffer, uint16_t len){
	usartRxBuffLen = len;
	recivedBuff = buffer;
}

void readLine(char* buffer, uint16_t len){
	setRxBuffer(buffer, len);
	while(!recivedNewLine()){
		hw_delay_ms(1);
	}
	setRxBuffer(NULL, 0);
}

char readChar(){
		char buffer[2] = {'\0', '\0'};
		readLine(buffer, 2);
		return buffer[0];
}

bool recivedNewLine(void){
	bool tmp = false;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		tmp = newLine;
		newLine = false;
	}
	return tmp;
}

ISR(USART_UDRE_vect)
{
	//sprawdzamy, wyslany bajt jest znakiem konca tesktu
	if (toSendLen != 0){
		UDR = *toSendData;
		toSendData++;
		toSendLen--;
	}else{
		uartTxBusy = false;
		UCSRB &= ~(1 << UDRIE); //wylacz przerwania pustego bufora nadawania
	}
}

ISR(USART_RXC_vect)
{
	if (recivedBuff == NULL)
	{
		char tmp = UDR;
		return;
	}
	
	recivedBuff[usartRxCounter] = UDR;
	
	if(recivedBuff[usartRxCounter] == '\n'){
		//recivedBuff[++usartRxCounter] = '\0';
		recivedBuff[--usartRxCounter] = '\0';
		newLine = true;
		usartRxCounter = 0;
	}else{
		usartRxCounter++;
	}
	
	
	if(usartRxCounter >= usartRxBuffLen-1){
		recivedBuff[usartRxCounter] = '\0';
		usartRxCounter = 0;
		newLine = true;
	}
}