
#include <SoftwareSerial.h>
#include <SPI.h>              // include libraries
#include <TinyLoRa.h>

#define SSerialRX        3
#define SSerialTX        5
#define SSerialTxControl 4
#define RS485Transmit    HIGH
#define RS485Receive     LOW

String deviceID1 = "\"ENV010011\"";
int reset_pin =  6;

SoftwareSerial RS485Serial(SSerialRX, SSerialTX);
String logdata;
int byteRead;

byte buffer[15];
long kwh = 0;

float read_meter(char para);
void get_pmu_data();

// Net  Session Key (MSB) 07    08    22    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   cc
uint8_t NwkSkey[16] = { 0x07, 0x08, 0x22, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xcc };

// App Session Key (MSB)  98    24    0c    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    cd 
uint8_t AppSkey[16] = { 0x98, 0x24, 0x0c, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xcd };

// Device Address (MSB)  01   f8   20   4d
uint8_t DevAddr[4] = { 0x01,0xf8,0x20,0x4d };


const unsigned int sendInterval = 5;

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
int send_count = 0;
void loop() 
{
   
 // String logdata = "{\"V1\":240.96,\"V2\":245.24,\"V3\":241.39,\"I1\":0.00,\"I2\":10.47,\"I3\":0.00,\"IT\":10.47,\"AP\":2555.10,\"PF\":0.82,\"KWH\":7222.00,\"VV1\":421.59,\"VV2\":422.73,\"VV3\":416.43,\"FQ\":49.89}";

   logdata = "{\"meterID\":"+deviceID1;
   get_pmu_data();
   Serial.println("Sending LoRa Data...");   
   Serial.println(logdata);
   char data[logdata.length()+1];
   logdata.toCharArray(data,logdata.length()+1);
   for(int i=0;i<sizeof(data);i++)
   {
      Serial.print(data[i]);
   }
   Serial.println();
   lora.sendData(data, sizeof(data), lora.frameCounter);
   Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
   lora.frameCounter++;
   send_count++;
   delay(30000);

   if(send_count>4)
   {
      digitalWrite(reset_pin, LOW); 
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


void get_pmu_data()
{
  float y = read_meter('1');
  
  logdata += ",\"V1\":\"";
  logdata += String(y);
  logdata += "\",";
 // Serial.println(logdata);
   y = read_meter('2');
 // Serial.println(logdata);
  logdata += "\"V2\":\"";
  logdata += String(y);
  logdata += "\",";
 

  y = read_meter('3');
  logdata += "\"V3\":\"";
  logdata += String(y);
  logdata += "\",";
 // Serial.println(logdata); 
  y = read_meter('4');
  logdata += "\"I1\":\"";
  logdata += String(y);
  logdata += "\","; 
   
   y = read_meter('5');
  logdata += "\"I2\":\"";
  logdata += String(y);
  logdata += "\","; 
//  Serial.println(logdata); 
   y = read_meter('6');
  logdata += "\"I3\":\"";
  logdata += String(y);
  logdata += "\","; 
  y = read_meter('7');
  logdata += "\"IT\":\"";
  logdata += String(y);
  logdata += "\","; 
  y = read_meter('8');
  logdata += "\"AP\":\"";
  logdata += String(y);
  logdata += "\","; 
  

  y = read_meter('9');
  logdata += "\"PF\":\"";
  logdata += String(y);
  logdata += "\","; 
  
  y = read_meter('a');
  logdata += "\"KWH\":\"";
  logdata += String(kwh);
  logdata += "\","; 

  y = read_meter('b');
  logdata += "\"VV1\":\"";
  logdata += String(y);
  logdata += "\","; 

  y = read_meter('c');
  logdata += "\"VV2\":\"";
  logdata += String(y);
  logdata += "\","; 

  y = read_meter('d');
  logdata += "\"VV3\":\"";
  logdata += String(y);
  logdata += "\","; 

  y = read_meter('e');
  logdata += "\"FQ\":\"";
  logdata += String(y);
  logdata += "\"}"; 
  
}

float read_meter(char para)
{
  
  
   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   
   if(para=='1')  // voltage line 1
   {     
      unsigned char request[] = {0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};
      RS485Serial.write(request, sizeof(request));
      
   }
   else if(para=='2')  // voltage line 2
   {     
      unsigned char request[] = {0x01,0x03,0x00,0x14,0x00,0x02,0x84,0x0F};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='3')  // voltage line 3
   {     
      unsigned char request[] = {0x01,0x03,0x00,0x28,0x00,0x02,0x44,0x03};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='4')  // current line 1
   {     
      byte request[] = {0x01,0x03,0x00,0x04,0x00,0x02,0x85,0xCA};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='5')  // current line 2
   {     
      byte request[] = {0x01,0x03,0x00,0x18,0x00,0x02,0x44,0x0C};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='6')  // current line 3
   {     
      byte request[] = {0x01,0x03,0x00,0x2C,0x00,0x02,0x05,0xC2};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='7')  // total current 
   {     
      byte request[] = {0x01,0x03,0x00,0x40,0x00,0x02,0xC5,0xDF};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='8')  //   aperent power
   {     
      byte request[] = {0x01,0x03,0x00,0x48,0x00,0x02,0x44,0x1D};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='9')   //    system power factor
   {     
      byte request[] = {0x01,0x03,0x00,0x42,0x00,0x02,0x64,0x1F};
      RS485Serial.write(request, sizeof(request));
   }
   else if(para=='a')  //    //  active energy terrif one
   {     
      byte request[] = {0x01,0x03,0x01,0xD6,0x00,0x02,0x24,0x0F};
      RS485Serial.write(request, sizeof(request));
   }
  //------------------------------------------------------------------------------
  else if(para=='b')  //    //  v1-v2 voltage
   {     
      byte request[] = {0x01,0x03,0x00,0x02,0x00,0x02,0x65,0xCB};
      RS485Serial.write(request, sizeof(request));
   } 

   else if(para=='c')  //    //  v2-v3 voltage
   {     
      byte request[] = {0x01,0x03,0x00,0x16,0x00,0x02,0x25,0xCF};
      RS485Serial.write(request, sizeof(request));
   } 
   else if(para=='d')  //    //  v3-v1 voltage
   {     
      byte request[] = {0x01,0x03,0x00,0x2A,0x00,0x02,0xE5,0xC3};
      RS485Serial.write(request, sizeof(request));
   }    
   else if(para=='e')  //    //  system frequency 
   {     
      byte request[] = {0x01,0x03,0x00,0x4A,0x00,0x02,0xE5,0xDD};
      RS485Serial.write(request, sizeof(request));
   }
  
    

   RS485Serial.flush();
    //Serial.print("Tx: ");
  //  for(int i=0;i<8;i++)
  //  {
  //     Serial.print(request[i], HEX);
  //     //delay(100);
  //  }
  //  Serial.println();
  
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
   if(ct==1500)
   {
     return(0);
   }
    
    
  
    int recvLen = RS485Serial.readBytes(buffer,10);
    /*Serial.print("Rx: ");
  
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  
   Serial.print(buffer[3], HEX); Serial.print(" ");
   Serial.print(buffer[4], HEX); Serial.print(" ");
   Serial.print(buffer[5], HEX); Serial.print(" ");
   Serial.print(buffer[6], HEX); Serial.print(" "); 
   Serial.println();*/
  
  // Convert to Float
  float x;
  if(recvLen>0)
  {
   float x;
   
   
   if(para=='a')
   {
        kwh = buffer[3]<<24;
        kwh = kwh + (buffer[4]<<16);
        kwh = kwh + (buffer[5]<<8);
        kwh = kwh + buffer[6];
      
   }
   else
   {
       ((byte*)&x)[3]= buffer[3];
       ((byte*)&x)[2]= buffer[4];
       ((byte*)&x)[1]= buffer[5];
       ((byte*)&x)[0]= buffer[6];
   }
   
   
   // Convert to Float
  
//   Serial.print("V1: ");
//   Serial.print(x,2);
//   Serial.println(" Volts"); 
   return(x);
  }
  else
  {
      Serial.print("wrong crc ");
      
  } 

 
}
