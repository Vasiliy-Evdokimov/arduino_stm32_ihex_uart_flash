#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define MAX_HEX_COUNT 16

#define RECORD_LENGTH 16

#define ADDRESS_HIGH 0x0800
#define ADDRESS_LOW 0xC000

#define FLASH_SECTOR_NO 3

#define STR_LENGTH (RECORD_LENGTH * 2 + 1)
#define PACK_LENGTH (STR_LENGTH + 11)
#define MSG_LENGTH (PACK_LENGTH + 2)

#define MSG_INITTRANSMIT	  1
#define MSG_READYTOTRANSMIT	2
#define MSG_WAITFORPART		  3
#define MSG_DATAPART    	  4
#define MSG_COMPLETED   	  5

#define ERR_EMPTYSTRING		  1
#define ERR_BADSTRINGSTART	2
#define ERR_BADSTRINGSIZE	  3
#define ERR_CRCCHECKFAIL	  4

typedef struct {
	uint8_t size;
	uint16_t address;
	uint8_t type;
	uint8_t data[RECORD_LENGTH];
} hexString;

uint8_t getCRC(uint8_t aDataLen, uint16_t aAddr, uint8_t aType, uint8_t *aData);

void makeOutputArray(char *aText, char aOut[][PACK_LENGTH], char *aOutSize);

uint8_t parseStr(uint8_t *aStr, hexString *aHex);

#endif
