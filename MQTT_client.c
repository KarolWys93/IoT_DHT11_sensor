/*
 * MQTT_client.c
 *
 * Created: 29.03.2019 21:09:34
 *  Author: Karol
 */ 

#include "MQTT_client.h"

#define MQTT_PROTOCOL_LEVEL		4

#define MQTT_CTRL_CONNECT		0x1
#define MQTT_CTRL_CONNECTACK	0x2
#define MQTT_CTRL_PUBLISH		0x3
#define MQTT_CTRL_PUBACK		0x4
#define MQTT_CTRL_PUBREC		0x5
#define MQTT_CTRL_PUBREL		0x6
#define MQTT_CTRL_PUBCOMP		0x7
#define MQTT_CTRL_SUBSCRIBE		0x8
#define MQTT_CTRL_SUBACK		0x9
#define MQTT_CTRL_UNSUBSCRIBE	0xA
#define MQTT_CTRL_UNSUBACK		0xB
#define MQTT_CTRL_PINGREQ		0xC
#define MQTT_CTRL_PINGRESP		0xD
#define MQTT_CTRL_DISCONNECT	0xE

#define MQTT_QOS_1				0x1
#define MQTT_QOS_0				0x0

/* Adjust as necessary, in seconds */
#define MQTT_CONN_KEEPALIVE		60

#define MQTT_CONN_USERNAMEFLAG	0x80
#define MQTT_CONN_PASSWORDFLAG	0x40
#define MQTT_CONN_WILLRETAIN	0x20
#define MQTT_CONN_WILLQOS_1		0x08
#define MQTT_CONN_WILLQOS_2		0x18
#define MQTT_CONN_WILLFLAG		0x04
#define MQTT_CONN_CLEANSESSION	0x02

#define DEFAULT_BUFFER_SIZE		200
#define DEFAULT_TIMEOUT			10000
#define DEFAULT_CRLF_COUNT		2

int16_t packet_id_counter = 0;

char clientID[] ="";
char will_topic[] = "";
char will_payload[] ="";
uint8_t will_qos = 1;
uint8_t will_retain = 0;

static uint8_t* AddStringToBuf(uint8_t *_buf, const char *_string)
{
	uint16_t _length = strlen(_string);
	_buf[0] = _length >> 8;
	_buf[1] = _length & 0xFF;
	_buf+=2;
	strncpy((char *)_buf, _string, _length);
	return _buf + _length;
}

uint16_t MQTT_connectpacket(uint8_t* packet, char* user, char* pass)
{
	uint8_t*_packet = packet;
	uint16_t _length;

	_packet[0] = (MQTT_CTRL_CONNECT << 4);
	_packet+=2;
	_packet = AddStringToBuf(_packet, "MQTT");
	_packet[0] = MQTT_PROTOCOL_LEVEL;
	_packet++;
	
	_packet[0] = MQTT_CONN_CLEANSESSION;
	if (will_topic && strlen(will_topic) != 0) {
		_packet[0] |= MQTT_CONN_WILLFLAG;
		if(will_qos == 1)
		_packet[0] |= MQTT_CONN_WILLQOS_1;
		else if(will_qos == 2)
		_packet[0] |= MQTT_CONN_WILLQOS_2;
		if(will_retain == 1)
		_packet[0] |= MQTT_CONN_WILLRETAIN;
	}
	if (strlen(user) != 0)
	_packet[0] |= MQTT_CONN_USERNAMEFLAG;
	if (strlen(pass) != 0)
	_packet[0] |= MQTT_CONN_PASSWORDFLAG;
	_packet++;

	_packet[0] = MQTT_CONN_KEEPALIVE >> 8;
	_packet++;
	_packet[0] = MQTT_CONN_KEEPALIVE & 0xFF;
	_packet++;

	if (strlen(clientID) != 0) {
		_packet = AddStringToBuf(_packet, clientID);
		} else {
		_packet[0] = 0x0;
		_packet++;
		_packet[0] = 0x0;
		_packet++;
	}
	if (will_topic && strlen(will_topic) != 0) {
		_packet = AddStringToBuf(_packet, will_topic);
		_packet = AddStringToBuf(_packet, will_payload);
	}

	if (strlen(user) != 0) {
		_packet = AddStringToBuf(_packet, user);
	}
	
	if (strlen(pass) != 0) {
		_packet = AddStringToBuf(_packet, pass);
	}
	_length = _packet - packet;
	packet[1] = _length-2;

	return _length;
}

uint16_t MQTT_publishPacket(uint8_t *packet, const char *topic, char *data, uint8_t qos)
{
	uint8_t *_packet = packet;
	uint16_t _length = 0;
	uint16_t Datalen=strlen(data);

	_length += 2;
	_length += strlen(topic);
	if(qos > 0) {
		_length += 2;
	}
	_length += Datalen;
	_packet[0] = MQTT_CTRL_PUBLISH << 4 | qos << 1;
	_packet++;
	do {
		uint8_t encodedByte = _length % 128;
		_length /= 128;
		if ( _length > 0 ) {
			encodedByte |= 0x80;
		}
		_packet[0] = encodedByte;
		_packet++;
	} while ( _length > 0 );
	_packet = AddStringToBuf(_packet, topic);
	if(qos > 0) {
		_packet[0] = (packet_id_counter >> 8) & 0xFF;
		_packet[1] = packet_id_counter & 0xFF;
		_packet+=2;
		packet_id_counter++;
	}
	memmove(_packet, data, Datalen);
	_packet+= Datalen;
	_length = _packet - packet;

	return _length;
}

uint16_t MQTT_subscribePacket(uint8_t *packet, const char *topic, uint8_t qos)
{
	uint8_t *_packet = packet;
	uint16_t _length;

	_packet[0] = MQTT_CTRL_SUBSCRIBE << 4 | MQTT_QOS_1 << 1;
	_packet+=2;

	_packet[0] = (packet_id_counter >> 8) & 0xFF;
	_packet[1] = packet_id_counter & 0xFF;
	_packet+=2;
	packet_id_counter++;

	_packet = AddStringToBuf(_packet, topic);

	_packet[0] = qos;
	_packet++;

	_length = _packet - packet;
	packet[1] = _length-2;

	return _length;
}

//int main()
//{
	//char buf[4];
	//uint8_t _buffer[150];
	//uint16_t len;
	//
	//#ifdef SUBSRCIBE_DEMO
	//long KeepAliveTime;
	////PWM_init();
	//#endif
//
	//#ifdef PUBLISH_DEMO
	////ADC_Init();
	//#endif
//
	//while(1)
	//{
		//MQTT_ConnectToServer();
		//len = MQTT_connectpacket(_buffer);
		//sendPacket(_buffer, len);
		//len = readPacket(_buffer, 1000);
//
		//#ifdef PUBLISH_DEMO
		//while(TCPClient_connected())
		//{
			//memset(_buffer, 0, 150);
			//memset(buf, 0, 4);
			////itoa(((float)ADC_Read(0)/10.4), buf, 10);	/* Read ADC channel 0 and publish it in range 0-100 */
			//len = MQTT_publishPacket(_buffer, "Nivya151/feeds/test", buf, 1);/* topic format: "username/feeds/aio_feed" e.g. "Nivya151/feeds/test" */
			//sendPacket(_buffer, len);
		//}
		//TCPClient_Close();
		//_delay_ms(1000);
		//#endif
		//
		//#ifdef SUBSRCIBE_DEMO
		//uint8_t valuePointer=0;
		//memset(_buffer, 0, 150);
		//len = MQTT_subscribePacket(_buffer, "Nivya151/feeds/test", 1);/* topic format: "username/feeds/aio_feed" e.g. "Nivya151/feeds/test" */
		//sendPacket(_buffer, len);
		//KeepAliveTime = (MQTT_CONN_KEEPALIVE * 1000L);
		//while(KeepAliveTime > 0)		/* Read subscription packets till Alive time */
		//{
			//len = readPacket(_buffer, 1000);
			//for (uint16_t i=0; i<len;i++)
			//{
				//for (uint8_t k = 0; k < 4; k++)/* here 4 is aio_feed char length e.g. aio_feed "test" has length of 4 char */
				//buf[k] = _buffer[i + k];
				//if (strstr(buf, AIO_FEED) != 0)
				//{
					//valuePointer = i + 4;
					//OCR0 = 2.5 * StringToUint16(_buffer + valuePointer);
					//i = len;
				//}
			//}
			//_delay_ms(1);
			//KeepAliveTime--;
		//}
		//if(TCPClient_connected()) TCPClient_Close();
		//_delay_ms(1000);
		//#endif
	//}
//}
