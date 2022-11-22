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

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial(15, 16); // RX, TX

const long frequency = 433;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin
int i;
void LoRa_txMode();
void LoRa_rxMode();
boolean runEvery(unsigned long );
void LoRa_sendMessage(String );


String message = "";

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);


    // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  while (!mySerial);
  LoRa.setPins(csPin, resetPin, irqPin);
  LoRa.setSpreadingFactor(13);
  LoRa.setTxPower(20);
  
  if (!LoRa.begin(frequency)) {
 //   Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

 // Serial.println("LoRa init succeeded.#");
//  Serial.println();
//  Serial.println("LoRa Simple Gateway#");
//  Serial.println("Only receive messages from nodes#");
 // Serial.println("Tx: invertIQ enable#");
//  Serial.println("Rx: invertIQ disable#");
 // Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
   mySerial.println("serial test");
  LoRa_rxMode();
  i=1;
}

void onReceive(int packetSize) {
  
  message="";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  
  //Serial.print("Gateway Receive: ");
  Serial.println(message);
  mySerial.println(message);

  
}

void onTxDone() {
//  Serial.println("TxDone#");
  LoRa_rxMode();
}


void loop() {
  
  if (runEvery(5000)) 
  { // repeat every 5000 millis


    if(i==1)
    {
      LoRa_sendMessage("ENV0001"); // send a message
    }
    else if(i==2)
    {
      LoRa_sendMessage("ENV0002"); // send a message
    }
    else if(i==3)
    {
      LoRa_sendMessage("ENV0003"); // send a message
    }
    else if(i==4)
    {
      LoRa_sendMessage("ENV0004"); // send a message
    }
   
    
    i++;
    if(i==5)
    { i=1;
    }
    
   // Serial.print("Reading ID: ");
  //  Serial.println(i);
  }
}



void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String msg) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(msg);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}


boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
