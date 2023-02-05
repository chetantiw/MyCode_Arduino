
#include <SoftwareSerial.h>
#include <SPI.h>              // include libraries
#include <TinyLoRa.h>   

#define SSerialRX        3
#define SSerialTX        5
#define SSerialTxControl 4
#define RS485Transmit    HIGH
#define RS485Receive     LOW


int reset_pin =  6;

int send_interval = 1;
int reset_interval = 2;
int send_count = 0;
int reset_count = 0;
 static unsigned long previousMillis = 0;

SoftwareSerial RS485Serial(SSerialRX, SSerialTX);
String logdata;
int byteRead;

byte buffer[64];
float read_meter0();
float read_meter1();
float read_meter2();


// Net  Session Key (MSB) 07    08    22    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   c8
uint8_t NwkSkey[16] = { 0x07, 0x08, 0x22, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xc8 };

// App Session Key (MSB)  98    24    0c    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    c9 
uint8_t AppSkey[16] = { 0x98, 0x24, 0x0c, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xc9 };

// Device Address (MSB)  01   f8   20   49
uint8_t DevAddr[4] = { 0x01,0xf8,0x20,0x49 };

TinyLoRa lora = TinyLoRa(2, 10, 9);

void setup() {
 
  digitalWrite(reset_pin, HIGH);
  RS485Serial.begin(9600);
  Serial.begin(9600);
  while (! Serial);
  pinMode(reset_pin, OUTPUT);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);
  
  // Initialize LoRa
  Serial.print("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(MULTI);
  // set datarate
  lora.setDatarate(SF7BW125);
  if(!lora.begin())
  {
    Serial.println("Failed");
    Serial.println("Check your radio");
    while(true);
  }
  
 
  Serial.println("LoRa init succeeded.");
  Serial.println("LoRa EMS node");
  Serial.println();
  delay(3000);
  
 
}

void loop() 
{
 unsigned char send_data[55];
 if(runEvery(30000))
 {
   send_count++;
 }
 if(send_count==send_interval)
 {
   int t = 0;
   for(t = 0;t<55;t++)
   {
       send_data[t] = 0; 
       
   }
   
   Serial.println("Reading Data 0");   
   read_meter0();
   
   for(t = 3;t<54;t++)
   {
       send_data[t-3] = buffer[t]; 
       
   }
   Serial.println(t);
//   int k = t;
//   delay(500);
//   Serial.println("Reading Data 1");   
//   read_meter1();
//   for( t = 3;t<34;t++)
//   {
//       send_data[(k+t)-3] = buffer[t]; 
//       
//      
//   }
//   Serial.println(t);
//   k = k+t;
//   delay(500);
//   Serial.println("Reading Data 2");   
//   read_meter2();
//   for( t = 3;t<9;t++)
//   {
//       send_data[(k+t)-3] = buffer[t]; 
//       
//   }
//    Serial.println(t);
   
   delay(500);
  
   Serial.println("sending data to server....");
   for (int i = 0; i < sizeof(send_data); i++) {
      Serial.print(send_data[i], HEX);
    }
    Serial.println();
    String s = "{\"t\":32}";
    char test[15];
    s.toCharArray(test,12);
  
   lora.sendData(test, sizeof(test), lora.frameCounter);
   Serial.print("Frame Counter: ");
   Serial.println(lora.frameCounter);
   lora.frameCounter++;
   reset_count++;
   send_count = 0;
 }
 if(reset_count>reset_interval)
 {
   digitalWrite(reset_pin, LOW); 
 }

 
  
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

   x =0;
   
   ((byte*)&x)[14]= buffer[11];
   ((byte*)&x)[13]= buffer[12];
   ((byte*)&x)[12]= buffer[13];
   ((byte*)&x)[11]= buffer[14];
   
 //Convert to Float
   Serial.print("L1 C: ");
   Serial.print(x,2);
   Serial.println(" Amps"); 
   
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
   unsigned char request[] = {0x01,0x03,0x00,0x32,0x00,0x0E,0x65,0xC1};
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   while( RS485Serial.available()<34)
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
   int recvLen = RS485Serial.readBytes(buffer,36);
    Serial.print("Rx: ");
    Serial.println(recvLen);
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  // Convert to Float
  float x;
  if(recvLen>0)
  {
   float x;
   
   ((byte*)&x)[17]= buffer[14];
   ((byte*)&x)[16]= buffer[15];
   ((byte*)&x)[15]= buffer[16];
   ((byte*)&x)[14]= buffer[17];
   
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
   unsigned char request[] = {0x01,0x03,0x01,0xD6,0x00,0x02,0x24,0x0F};
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   while( RS485Serial.available()<8)
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
   int recvLen = RS485Serial.readBytes(buffer,10);
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
 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

// data 0
//Received: 01 03 40 43 69 EF 9E 43 69 E8 72 40 7E 76 C8 3F 7F BE 76 3F 7D 70 A4 44 66 23 22 C2 2A 53 F8 44 68 88 4A 40 C3 85 1E 41 1B AE 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0F A0 
