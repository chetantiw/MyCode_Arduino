
#include <SoftwareSerial.h>


#define SSerialRX        3
#define SSerialTX        5
#define SSerialTxControl 4
#define RS485Transmit    HIGH
#define RS485Receive     LOW


int reset_pin =  6;

SoftwareSerial RS485Serial(SSerialRX, SSerialTX);
String logdata;
int byteRead;

byte buffer[80];
float read_meter0();
float read_meter1();
float read_meter2();

void setup() {
 
  digitalWrite(reset_pin, HIGH);
  RS485Serial.begin(9600);
  Serial.begin(9600);
  while (! Serial);
 // pinMode(reset_pin, OUTPUT);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);
  
  // Initialize LoRa
  
 
}

void loop() 
{
 Serial.println("Reading Data 0");   
 read_meter0();
 delay(2000);
 Serial.println("Reading Data 1");   
 read_meter1();
 delay(2000);
 Serial.println("Reading Data 2");   
 read_meter2();
 delay(2000);
  
}

float read_meter0()
{
   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   unsigned char request[] = {0x01,0x03,0x00,0x00,0x00,0x20,0x44,0x12};
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   while( RS485Serial.available()<70)
   {
      ct++;
      if(ct==1500)
      {
        break;
      }
      delay(1);
   }
   if(ct==2500)
   {
     return(0);
   }
   int recvLen = RS485Serial.readBytes(buffer,72);
    Serial.print("Rx: ");
    Serial.println(recvLen);
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  
  // Serial.print(buffer[3], HEX); Serial.print(" ");
  // Serial.print(buffer[4], HEX); Serial.print(" ");
  // Serial.print(buffer[5], HEX); Serial.print(" ");
  // Serial.print(buffer[6], HEX); Serial.print(" "); 
  // Serial.println();
  
  // Convert to Float
  float x;
  if(recvLen>0)
  {
   float x;
   
   ((byte*)&x)[3]= buffer[3];
   ((byte*)&x)[2]= buffer[4];
   ((byte*)&x)[1]= buffer[5];
   ((byte*)&x)[0]= buffer[6];
   
 //Convert to Float
   Serial.print("V1: ");
   Serial.print(x,2);
   Serial.println(" Volts"); 
   return(x);
  }
  else
  {
      Serial.print("wrong crc ");
      
  } 

}

float read_meter1()
{
   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   unsigned char request[] = {0x01,0x03,0x00,0x40,0x00,0x0C,0x44,0x1B};
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   while( RS485Serial.available()<30)
   {
      ct++;
      if(ct==1500)
      {
        break;
      }
      delay(1);
   }
   if(ct==2500)
   {
     return(0);
   }
   int recvLen = RS485Serial.readBytes(buffer,32);
    Serial.print("Rx: ");
    Serial.println(recvLen);
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  
  // Serial.print(buffer[3], HEX); Serial.print(" ");
  // Serial.print(buffer[4], HEX); Serial.print(" ");
  // Serial.print(buffer[5], HEX); Serial.print(" ");
  // Serial.print(buffer[6], HEX); Serial.print(" "); 
  // Serial.println();
  
  // Convert to Float
  float x;
  if(recvLen>0)
  {
   float x;
   
   ((byte*)&x)[3]= buffer[3];
   ((byte*)&x)[2]= buffer[4];
   ((byte*)&x)[1]= buffer[5];
   ((byte*)&x)[0]= buffer[6];
   
 //Convert to Float
   Serial.print("Total Current: ");
   Serial.print(x,2);
   Serial.println(" Amps"); 
   return(x);
  }
  else
  {
      Serial.print("wrong crc ");
      
  } 

}

float read_meter2()
{
   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   unsigned char request[] = {0x01,0x03,0x01,0xD6,0x00,0x20,0xA4,0x16};
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   while( RS485Serial.available()<70)
   {
      ct++;
      if(ct==1500)
      {
        break;
      }
      delay(1);
   }
   if(ct==2500)
   {
     return(0);
   }
   int recvLen = RS485Serial.readBytes(buffer,72);
    Serial.print("Rx: ");
    Serial.println(recvLen);
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  
  // Serial.print(buffer[3], HEX); Serial.print(" ");
  // Serial.print(buffer[4], HEX); Serial.print(" ");
  // Serial.print(buffer[5], HEX); Serial.print(" ");
  // Serial.print(buffer[6], HEX); Serial.print(" "); 
  // Serial.println();
  
  // Convert to Float
  long x;
  if(recvLen>0)
  {
   long x;
   
   ((byte*)&x)[3]= buffer[3];
   ((byte*)&x)[2]= buffer[4];
   ((byte*)&x)[1]= buffer[5];
   ((byte*)&x)[0]= buffer[6];
   
 //Convert to Float
   Serial.print("Energy: ");
   Serial.print(x);
   Serial.println(" kWh"); 
   return(x);
  }
  else
  {
      Serial.print("wrong crc ");
      
  } 

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
