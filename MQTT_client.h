/*
 * MQTT_client.h
 *
 * Created: 01.04.2019 22:31:42
 *  Author: Karol
 */ 


#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

uint16_t MQTT_connectpacket(uint8_t* packet, char* user, char* pass);

uint16_t MQTT_publishPacket(uint8_t *packet, const char *topic, char *data, uint8_t qos);

uint16_t MQTT_subscribePacket(uint8_t *packet, const char *topic, uint8_t qos);


#endif /* MQTT_CLIENT_H_ */