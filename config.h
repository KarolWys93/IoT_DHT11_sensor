/*
 * config.h
 *
 * Created: 14.03.2019 22:18:58
 *  Author: Karol
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include <avr/io.h>

/* WIFI config */
uint8_t isWiFiConfigChanged();
void getWiFiPassword(char* passwordBuffer, uint8_t len);
void getSSID(char* ssidBuffer, uint8_t len);

void setWiFiPassword(char* passwordBuffer, uint8_t len);
void setSSID(char* ssidBuffer, uint8_t len);

/* connection settings */
void getHost(char* hostNameBuffer, uint8_t len);
uint16_t getPort();
void getTopic(char* topicBuffer, uint8_t len);

void setHost(char* hostNameBuffer, uint8_t len);
void setPort(uint16_t port);
void setTopic(char* topicBuffer, uint8_t len);

/* other settings */
uint16_t getPeriod();
void setPeriod(uint16_t period);

#endif /* CONFIG_H_ */