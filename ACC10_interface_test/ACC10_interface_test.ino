/*
  LoRa Simple Gateway/Node Exemple

  This code uses InvertIQ function to create a simple Gateway/Node logic.

  Gateway - Sends messages with enableInvertIQ()
          - Receives messages with disableInvertIQ()

  Node    - Sends messages with disableInvertIQ()
          - Receives messages with enableInvertIQ()

  With this arrangement a Gateway never receive messages from another Gateway
  and a Node never receive message from another Node.
  Only Gateway to Node and vice versa.

  This code receives messages and sends a message every second.

  InvertIQ function basically invert the LoRa I and Q signals.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on InvertIQ register 0x33.

  created 05 August 2018
  by Luiz H. Cassettari
*/
#include <SoftwareSerial.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>

#define SSerialRX        2
#define SSerialTX        3


SoftwareSerial RS485Serial(SSerialRX, SSerialTX);

int byteRead;

byte buffer[15];

void setup() {
  
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  RS485Serial.begin(1200);
  
  
  Serial.print("configuring device:");

 
}

void loop() {
    unsigned char request[] = {0xFE, 0xA6, 0x00, 0x01, 0xA7,0x02};
   // unsigned char request[] = {0x31, 0x32, 0x33, 0x34, 0x35};
    
    RS485Serial.write(request, sizeof(request));
    
   
       
    int ct=0;
    while( RS485Serial.available()>0)
    {
      unsigned char r =  RS485Serial.read();
      Serial.print(r, HEX);
      ct++;
    }
     
//   int recvLen = RS485Serial.readBytes(buffer,8);
//   Serial.println(recvLen);
//   Serial.print(buffer[1], HEX); Serial.print(" ");
//   Serial.print(buffer[2], HEX); Serial.print(" ");
   Serial.println(ct);


   delay(2000);
   
}
