/*
 * uart.c
 *
 * Created: 16-06-2017 16:23:11
 *  Author: Karol
 */ 

#include "uart.h"
#include <avr/interrupt.h>
#include <stddef.h>
#include <util/atomic.h>
#include <util/delay.h>

volatile static unsigned int usartRxCounter;
volatile static unsigned int usartRxBuffLen;

static char* toSend;
static  char* recivedBuff;

volatile static bool uartTxBusy = false;
volatile static bool newLine = false;

void usartInit(void)
{
	// ustawienie transmisji
	#define BAUD 9600        //9600bps standardowa predkosc transmisji modulu HC-05
	#include <util/setbaud.h> //linkowanie tego pliku musi by�
	//po zdefiniowaniu BAUD

	//ustaw obliczone przez #define warto�ci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |= (1 << U2X);
	#else
	UCSRA &= ~(1 << U2X);
	#endif

	//Ustawiamy pozostale parametry modul USART

	//standardowe parametry transmisji modulu HC-05
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);  //bit�w danych: 8
	//bity stopu:  1
	//parzysto��:  brak
	//wlacz nadajnik i odbiornik oraz ich przerwania odbiornika
	//przerwania nadajnika wlaczamy w funkcji wyslij_wynik()
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
}

bool uratTxIsBusy(void){
	return uartTxBusy;
}


void sendLine(char *text){
	sendData(text);
	while(uartTxBusy);
	sendData("\r\n");
}

void sendData(char *text){
	//Zaczekaj, az bufor nadawania bedzie pusty
	while (!(UCSRA & (1 << UDRE)));
	toSend = text;
	uartTxBusy = true;
	//rozpocz�cie transmisji
	UCSRB |= (1 << UDRIE);
}

void setRxBuffer(char* buffer, uint16_t len){
	usartRxBuffLen = len;
	recivedBuff = buffer;
}

void readLine(char* buffer, uint16_t len){
	setRxBuffer(buffer, len);
	while(!recivedNewLine()){
		_delay_ms(1);
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
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		tmp = newLine;
		newLine = false;
	}
	return tmp;
}

ISR(USART_UDRE_vect)
{
	//sprawdzamy, wyslany bajt jest znakiem konca tesktu
	if (*toSend != '\0'){
		UDR = *toSend;
		toSend++;
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