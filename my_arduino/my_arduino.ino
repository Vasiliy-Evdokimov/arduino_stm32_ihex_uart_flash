#include <stdio.h>
//
#include <SoftwareSerial.h>
SoftwareSerial uart1(2, 3);
//
#include "utils.c"

const char * msg_descr[] = {
    "MSG_INITTRANSMIT",
    "MSG_READYTOTRANSMIT",
    "MSG_WAITFORPART",
    "MSG_DATAPART",
    "MSG_COMPLETED"
};

char tx[MSG_LENGTH];
char rx[2];

char outArr[MAX_HEX_COUNT][PACK_LENGTH];
char outArrSz = 0;

char sourceText[] =
    "This is the final project of STM32 learning course."
    "(c) Vasiliy Evdokimov, Saint-Petersburg, 21.01.2023";

void setup()
{
  uart1.begin(9600);
  Serial.begin(9600);
  delay(500);
  //
  Serial.println("=== SOURCE TEXT ===");
  Serial.println("\"" + (String)sourceText + "\"");
  //
  makeOutputArray(sourceText, outArr, &outArrSz);
  //
  Serial.println("=== GENERATED HEX ===");
  for (int i = 0; i < outArrSz; i++) {
    for (int j = 0; j < PACK_LENGTH; j++)
      Serial.print((char)outArr[i][j]);
    Serial.println();
  }
}

char buf[100] = {0};
char buf2[PACK_LENGTH] = {0};
char rx0, rx1;

void loop()
{
  if (uart1.available() > 0)
  {
    uart1.readBytes(rx, 2);
    rx0 = rx[0];
    rx1 = rx[1];
    //
    buf[0] = '\0';
    sprintf(buf, "STM request: %s;%d", msg_descr[rx0 - 1], rx1);
    Serial.println(buf);
    //
    uart1.flush();
    //
    buf2[0] = '\0';
    tx[0] = '\0';
    if (rx0 == MSG_INITTRANSMIT) {
      tx[0] = MSG_READYTOTRANSMIT;
      tx[1] = outArrSz;
      Serial.println("=== TRANSMIT STARTED ===");
    } else
    //
    if (rx0 == MSG_WAITFORPART) {
      tx[0] = MSG_DATAPART;
      tx[1] = rx1;
      for (int i = 0; i < PACK_LENGTH; i++) {
        buf2[i] = outArr[rx1][i];
        tx[i + 2] = buf2[i];
      }
    } else
    //
    if (rx0 == MSG_COMPLETED) {
      Serial.println("=== TRANSMIT FINISHED ===");
    }
    //  
    buf[0] = '\0';
    if (tx[0] != 0) {
      sprintf(buf, "Arduino response: %s;%d%s", msg_descr[tx[0] - 1], tx[1], buf2);
      Serial.println(buf);
      //
      uart1.write(tx, MSG_LENGTH);
    }
  }
  delay(500);
}
