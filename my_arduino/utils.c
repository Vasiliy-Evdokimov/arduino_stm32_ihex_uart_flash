#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"

uint8_t getCRC(uint8_t aDataLen, uint16_t aAddr, uint8_t aType, uint8_t *aData) {
    uint8_t res = 0;
    res += aDataLen;
    res += aAddr >> 8;
    res += aAddr & 0xFF;
    res += aType;
    for (int i = 0; i < aDataLen; i++)
        res += aData[i];
    res = (uint8_t)(0x100 - res);
    return res;
}

void makeOutputArray(char *aText, char aOut[][PACK_LENGTH], char *aOutSize) {
    uint16_t textLen = strlen(aText);
    char buf[STR_LENGTH] = {0};
    char buf2[2] = {0};
    uint16_t crc_start = 0;
    uint16_t addr = ADDRESS_LOW;
    uint8_t dataType = 0;
    *aOutSize = 0;
    //
    uint8_t addr_high[] = {ADDRESS_HIGH >> 8, ADDRESS_HIGH & 0xFF};
    sprintf(aOut[(*aOutSize)++], ":02000004%04x%02x", ADDRESS_HIGH, getCRC(2, 0, 4, addr_high));
    //
    crc_start = 0;
    int j = 0;
    for (int i = 0; i < textLen; i++) {
        j++;
        //
        sprintf(buf, "%s%02x", buf, aText[i]);
        //
        if ((j == RECORD_LENGTH) || (i == textLen - 1)) {
            sprintf(aOut[(*aOutSize)++], ":%02x%04x%02x%s%02x",
                j, addr, dataType, buf,
                getCRC(j, addr, dataType, &aText[crc_start])
            );
            //
            crc_start = i + 1;
            addr += RECORD_LENGTH;
            memset(buf, 0, STR_LENGTH);
            j = 0;
        }
    }
    sprintf(aOut[(*aOutSize)++], ":00000001ff");
}

uint8_t parseStr(uint8_t *aStr, hexString *aHex) {
    //correct string format is :LLAAAATT[DD..]CC
    //started with colon, at least 11 symbols (with colon)
    //
	uint16_t aStrSize = strlen(aStr);
	//
    if (aStrSize == 0)
        return ERR_EMPTYSTRING;
    if (aStr[0] != ':')
        return ERR_BADSTRINGSTART;
    if (aStrSize < 11)
        return ERR_BADSTRINGSIZE;
    //
    uint8_t *endPtr;
    //
    uint8_t dataLen = 0;
    uint8_t LL[2] = {0};
    strncat(LL, &aStr[1], 2);
    printf("LL = %s\n", LL);
    dataLen = (uint8_t)strtol(LL, &endPtr, 16);
    printf("dataLen = %x\n", dataLen);
    aHex->size = dataLen;
    //
    uint16_t dataAddr = 0;
    uint8_t AAAA[4] = {0};
    strncat(AAAA, &aStr[3], 4);
    printf("AAAA = %s\n", AAAA);
    dataAddr = (uint16_t)strtol(AAAA, &endPtr, 16);
    printf("dataAddr = %x\n", dataAddr);
    aHex->address = dataAddr;
    //
    uint8_t dataType = 0;
    uint8_t TT[2] = {0};
    strncat(TT, &aStr[7], 2);
    printf("TT = %s\n", TT);
    dataType = (uint16_t)strtol(TT, &endPtr, 16);
    printf("dataType = %x\n", dataType);
    aHex->type = dataType;
    //
    uint8_t dataCRC = 0;
    uint8_t CC[2] = {0};
    strncat(CC, &aStr[9 + dataLen * 2], 2);
    printf("CC = %s\n", CC);
    dataCRC = (uint8_t)strtol(CC, &endPtr, 16);
    printf("dataCRC = %x\n", dataCRC);
    //
    printf("data = ");
    uint8_t DD[2] = {0};
    uint8_t dataArr[dataLen];
    for (int i = 0; i < dataLen; i++) {
        DD[0] = 0;
        DD[1] = 0;
        strncat(DD, &aStr[9 + i * 2], 2);
        printf("%s", DD);
        dataArr[i] = (uint8_t)strtol(DD, &endPtr, 16);
    }
    memset(aHex->data, 0, RECORD_LENGTH);
    memcpy(aHex->data, dataArr, dataLen);
    //
    printf("\n");
    //
    uint8_t checkCRC = getCRC(dataLen, dataAddr, dataType, dataArr);
    printf("checkCRC = %x\n", checkCRC);
    //
    if (dataCRC != checkCRC)
    	return ERR_CRCCHECKFAIL;
    //
    return 0;
}
