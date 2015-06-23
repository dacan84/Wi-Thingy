/*
 * digi_api.h
 *
 * Created: 15/06/2015 12:12:28
 *  Author: Diego
 */ 


#ifndef DIGI_API_H_
#define DIGI_API_H_

#include "bsp.h"
#include "payload.h"

/*..........................................................................*/

/* CONSTANTS */
/*..........................................................................*/
#define START_DELIMITER                         0x7E
#define MAC_ADDRESS_BYTES_LENGTH                0x08
/*..........................................................................*/
#define MIN_PACKET_SIZE                         0x06
#define MAX_PACKET_SIZE                         0x64
#define AT_COMMAND_MINLENGTH                    0x04
#define TRANSMIT_REQUEST_COMMAND_MINLENGTH      0x0E
#define EXPLICIT_ADDRESS_COMMAND_MINLENGTH      0x14
#define REMOTE_AT_COMMAND_MINLENGTH             0x10
/*..........................................................................*/
#define XON                                     0x11
#define XOFF                                    0x13
#define ESCAPE                                  0x7D
/*..........................................................................*/
#define TRANSMIT_REQUEST_RESERVED_H             0xFF
#define TRANSMIT_REQUEST_RESERVED_L             0xFE
/*..........................................................................*/
#define AT_COMMAND_RESPONSE_OK                  0x00
#define AT_COMMAND_RESPONSE_ERROR               0x01
#define AT_COMMAND_RESPONSE_INVALID_COMMAND     0x02
#define AT_COMMAND_RESPONSE_INVALID_PARAMETER   0x03
/*..........................................................................*/
#define MODEM_STATUS_HARDWARE_RESET             0x00
#define MODEM_STATUS_WD_TIMER_RESET             0x01
#define MODEM_STATUS_NETWORK_WOKE_UP            0x0B
#define MODEM_STATUS_NETWORK_SLEEP              0x0C
/*..........................................................................*/
#define TX_STATUS_DELIVERY_SUCCESS              0x00
#define TX_STATUS_DELIVERY_MAC_ACK_FAILURE      0x01
#define TX_STATUS_DELIVERY_INVALID_ENDPOINT     0x15
#define TX_STATUS_DELIVERY_NETWORK_ACK_FAILURE  0x21
#define TX_STATUS_DELIVERY_ROUTE_NOT_FOUND      0x25
/*..........................................................................*/
#define TX_STATUS_DISCOVERY_NO_ROUTE_OVERHEAD   0x00
#define TX_STATUS_DISCOVERY_ROUTE_DISCOVERY     0x02
/*..........................................................................*/
#define PACKET_ACKNOWLEGED                      0x01
#define PACKET_BROADCAST                        0x02
/*..........................................................................*/
#define COMMISSIONING_ONE_PRESS     1
#define DEEP_SLEEP_MODE             8


/*..........................................................................*/

/* FRAME TYPES */
enum XBeePacketFrameType {
	AT_COMMAND = 0x08,
	AT_COMMAND_QUEUE = 0x09,
	TRANSMIT_REQUEST = 0x10,
	EXPLICIT_ADDRESSING = 0x11,
	REMOTE_AT_COMMAND = 0x17,
	AT_COMMAND_RESPONSE = 0x88,
	MODEM_STATUS = 0x8A,
	TRANSMIT_STATUS = 0x8B,
	RECEIVE_PACKET = 0x90,
	EXPLICIT_RX_INDICATOR = 0x91,
	REMOTE_COMMAND_RESPONSE = 0x97
};

/* RX STATES */
enum XBeePacketRxState {
	XBEE_PACKET_RX_START = 0,
	XBEE_PACKET_RX_LENGTH_1 = 1,
	XBEE_PACKET_RX_LENGTH_2 = 2,
	XBEE_PACKET_RX_PAYLOAD = 3,
	XBEE_PACKET_RX_CRC = 4
};

/*..........................................................................*/

/* AT COMMAND*/
typedef struct AtCommand AtCommand;

struct AtCommand {
	uint8_t frameId; /* Frame id */
	uint8_t command[2]; /* AT Command */
	uint8_t value[97]; /* Command value(optional)*/
};

/*..........................................................................*/

/* TRANSMIT REQUEST */
typedef struct TransmitRequest TransmitRequest;

struct TransmitRequest {
	uint8_t frameId; /* Frame id */
	uint8_t destinationAddress[8]; /* 64 bit destination address */
	uint8_t reserved[2];
	uint8_t bcastRadius; /* Broadcast radius */
	uint8_t options; /* Transmit options */
	uint8_t payload[97]; /* Data sent to the destination device */
};

/*..........................................................................*/

/* EXPLICIT ADDRESSING COMMAND FRAME */
typedef struct ExplicitAddressing ExplicitAddressing;

struct ExplicitAddressing {
	uint8_t frameId; /* Frame id */
	uint8_t destinationAddress[8]; /* 64 bit destination address */
	uint8_t reserved[2];
	uint8_t sourceEndpoint; /* Source endpoint for the transmission */
	uint8_t destinationEndpoint; /* Destination endpoint for the transmission */
	uint8_t clusterId[2]; /* Cluster ID used in the transmission */
	uint8_t profileId[2]; /* Profile ID used in the transmission */
	uint8_t bcastRadius; /* Broadcast radius */
	uint8_t options; /* Transmit options */
	uint8_t payload[97]; /* Data sent to the destination device */
};

/*..........................................................................*/

/* REMOTE AT COMMAND */
typedef struct RemoteAtCommand RemoteAtCommand;

struct RemoteAtCommand {
	uint8_t frameId; /* Frame id */
	uint8_t destinationAddress[8]; /* 64 bit destination address */
	uint8_t reserved[2];
	uint8_t options; /* Remote command options */
	uint8_t command[2]; /* AT command */
	uint8_t parameter; /* Command parameter */
};

/*..........................................................................*/

/* AT COMMAND RESPONSE */
typedef struct AtCommandResponse AtCommandResponse;

struct AtCommandResponse {
	uint8_t frameId; /* Frame id */
	uint8_t command[2]; /* AT command */
	uint8_t status; /* Command status */
	uint8_t value[97]; /* Command value(optional)*/
};

/*..........................................................................*/

/* MODEM STATUS */
typedef struct ModemStatus ModemStatus;

struct ModemStatus {
	uint8_t status; /* Modem status */
};

/*..........................................................................*/

/* TRANSMIT STATUS */
typedef struct TransmitStatus TransmitStatus;

struct TransmitStatus {
	uint8_t frameId; /* Frame ID */
	uint8_t reserved[2]; /* Reserved */
	uint8_t retryCount; /* Number of retransmissions */
	uint8_t deliveryStatus; /* Delivery status */
	uint8_t discoveryStatus; /* Discovery status */
};

/*..........................................................................*/

/* RECEIVE PACKET */
typedef struct ReceivePacket ReceivePacket;

struct ReceivePacket {
	uint8_t frameId; /* Frame id */
	uint8_t sourceAddress[8]; /* 64 bit source address */
	uint8_t reserved[2];
	uint8_t options; /* Receive options */
	uint8_t payload[97]; /* Data received */
};

/*..........................................................................*/

/* EXPLICIT RX INDICATOR */
typedef struct ExplicitRxIndicator ExplicitRxIndicator;

struct ExplicitRxIndicator {
	uint8_t frameId; /* Frame id */
	uint8_t sourceAddress[8]; /* 64 bit source address */
	uint8_t reserved[2];
	uint8_t sourceEndpoint; /* Source endpoint for the transmission */
	uint8_t destinationEndpoint; /* Destination endpoint for the transmission */
	uint8_t clusterId; /* Cluster ID used in the transmission */
	uint8_t profileId; /* Profile ID used in the transmission */
	uint8_t options; /* Receive options */
	uint8_t payload[97]; /* Data received */
};

/*..........................................................................*/

/* REMOTE COMMAND RESPONSE */
typedef struct RemoteCommandResponse RemoteCommandResponse;

struct RemoteCommandResponse {
	uint8_t frameId; /* Frame id */
	uint8_t sourceAddress[8]; /* 64 bit source(remote) address */
	uint8_t reserved[2];
	uint8_t command[2]; /* AT command */
	uint8_t status; /* AT command status */
	uint8_t value[97]; /* Command data */
};

/*..........................................................................*/

/* XBEE PACKET*/
typedef struct XBeePacket XBeePacket;

struct XBeePacket {
	uint8_t apiId; /* Frame type identifier */
	union {
		uint8_t payload[100];
		AtCommand atCommand;
		TransmitRequest transmitRequest;
		ExplicitAddressing explicitAddressing;
		RemoteAtCommand remoteAtCommand;
		AtCommandResponse atCommandResponse;
		ModemStatus modemStatus;
		TransmitStatus transmitStatus;
		ReceivePacket receivePacket;
		ExplicitRxIndicator explicitRxIndicator;
		RemoteCommandResponse remoteCommandResponse;
	}frame;
	uint16_t length; /* Number of bytes between the length and the checksum */
	uint8_t checksum; /* Checksum calculated as FF - sum(fields)*/
	uint8_t rxState; /* Reception state */
	uint8_t *dataPtr; /* Ponter to structure itself */
	uint8_t index;
	uint8_t lastByte;
};

/*..........................................................................*/
/* INIT */

void XBeeInit(void);

void XBeeJoin(void);

void XBeeReset(void);

/*..........................................................................*/
/* UTIL METHODS */

void XBeeCreatePacket(XBeePacket * const packet);

uint8_t XBeeEscape(uint8_t value);

/*..........................................................................*/

/* TX METHODS */

bool XBeeSendPacket(XBeePacket * const packet);

void XBeeCreateCompleteATCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command,uint8_t* params, uint8_t paramsLength);

void XBeeCreateATCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command);

void XBeeCreateTransmitRequestPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* destinationAddress, uint8_t radius, uint8_t options,
uint8_t* payload, uint8_t dataLength);

void XBeeCreateExplicitAddressingPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* destinationAddress, uint8_t sourceEndpoint,
uint8_t destinationEndpoint, uint8_t* clusterId, uint8_t* profileId,
uint8_t radius, uint8_t options,uint8_t* payload, uint8_t dataLength);

void XBeeCreateRemoteAtCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command, uint8_t param, uint8_t* destinationAddress,
uint8_t options);

/*..........................................................................*/

/* RX METHODS */

bool XBee_readPacket(XBeePacket * const packet);

bool XBee_readAtCommandResponsePacket(XBeePacket* packet, uint8_t* frameId,
uint8_t** command, uint8_t* status, uint8_t* value);

bool XBee_readModemStatusPacket(XBeePacket* packet, uint8_t* status);

bool XBee_readTransmitStatusPacket(XBeePacket* packet, uint8_t* frameId,
uint8_t* retryCount, uint8_t* deliveryStatus, uint8_t* discoveryStatus);

bool XBee_readReceivePacket(XBeePacket* packet, uint8_t* frameId,
uint8_t** sourceAddress, uint8_t* options, uint8_t** payload,
uint8_t* length);

bool XBee_readRemoteCommandResponsePacket(XBeePacket* packet,uint8_t* frameId,
uint8_t** sourceAddress,uint8_t** command, uint8_t* status, uint8_t* value);


#endif /* DIGI_API_H_ */