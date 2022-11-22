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

String decvID =     "LWED0061";                    //43
String deviceID = "\"LWED0061\"";

const long frequency = 433;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

SoftwareSerial RS485Serial(SSerialRX, SSerialTX);

int byteRead;

byte buffer[15];
String logdata;
float read_meter(char para);
void get_pmu_data();

String message = "";




void setup() {
  
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  RS485Serial.begin(1200);
  
  
  Serial.print("configuring device:");
  Serial.println(decvID); 
  
//  LoRa.setPins(csPin, resetPin, irqPin);
//
//  if (!LoRa.begin(frequency)) {
//    Serial.println("LoRa init failed. Check your connections.");
//    while (true);                       // if failed, do nothing
//  }
//
//  
//
//  Serial.println("LoRa init succeeded.");
//  Serial.println();
//  Serial.println("LoRa RS485 Node");
//  Serial.println("Only receive messages from EMS gateways");
//  Serial.println("Tx: invertIQ disable");
//  Serial.println("Rx: invertIQ enable");
//  Serial.println();
//  LoRa.setTxPower(20);
//  
//  LoRa.onReceive(onReceive);
//  LoRa.onTxDone(onTxDone);
//  LoRa_rxMode();
  delay(5000);
  logdata = "{\"meterID\":"+deviceID;
  get_pmu_data();
  Serial.println(logdata);
}

void loop() {
  
   
  // if(message==decvID)
   {
          
//          Serial.println("I am in 2");
//          Serial.println(logdata);
//          LoRa_sendMessage(logdata); // send a message
//          Serial.println("data sent!");
//          logdata = "";
//          message="";
//          logdata = "{\"meterID\":"+deviceID;
          get_pmu_data();

          delay(500);
   }
   
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
 
}

void onReceive(int packetSize) {
  message = ""; 

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
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


void get_pmu_data()
{
  long y = read_meter('2');
  logdata = "{\"meterID\":"+deviceID;
  logdata += ",\"CO2LEVEL\":\"";
  logdata += String(y);
  logdata += "\",";
  logdata += "\"}#"; 
  Serial.println(logdata);
  
 
  
}

float read_meter(char para)
{
  
  
  
   delay(10);
   
   if(para=='1')  // voltage line 1
   {     
      unsigned char request[] = {0xFE, 0xA6, 0x00, 0x01, 0xA7};
      RS485Serial.write(request, sizeof(request));
      
   }
   else if(para=='2')  // voltage line 2
   {     
      unsigned char request[] = {0xFE, 0xA6, 0x00, 0x1F, 0xC5};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='3')  // voltage line 3
   {     
      unsigned char request[] = {0xFE, 0xA6, 0x00, 0x01, 0xA7};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='4')  // current line 1
   {     
      byte request[] = {0xFE, 0xA6, 0x00, 0x01, 0xA7};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='5')  // current line 2
   {     
      byte request[] = {0xFE, 0xA6, 0x00, 0x01, 0xA7};
      RS485Serial.write(request, sizeof(request));
   }
  
   
 //  RS485Serial.flush();
       
    int ct=0;
    while( RS485Serial.available()<8)
    {
        ct++;
        if(ct==5000)
        {
          break;
        }
        delay(1);
     }
     if(ct==1500)
     {
       return(0);
     }
    
    
  
   int recvLen = RS485Serial.readBytes(buffer,8);
  
   Serial.println(recvLen);
   Serial.print(buffer[1], HEX); Serial.print(" ");
   Serial.print(buffer[2], HEX); Serial.print(" ");
   Serial.println();
  
 
  if(recvLen>0)
  {
   long x;
   long tmp,tr;
   x = buffer[1]*256+buffer[2];
 
   return(x);
  }

 
}
