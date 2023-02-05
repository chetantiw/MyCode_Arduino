
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

float read_meter(char para);
void get_pmu_data();

// Net  Session Key (MSB) 07    08    22    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   c8
uint8_t NwkSkey[16] = { 0x07, 0x08, 0x22, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xc8 };

// App Session Key (MSB)  98    24    0c    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    c9 
uint8_t AppSkey[16] = { 0x98, 0x24, 0x0c, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xc9 };

// Device Address (MSB)  01   f8   20   49
uint8_t DevAddr[4] = { 0x01,0xf8,0x20,0x49 };

const unsigned int sendInterval = 5;

TinyLoRa lora = TinyLoRa(2, 10, 9);



void setup() {
 
  digitalWrite(reset_pin, HIGH);
  RS485Serial.begin(9600);
  Serial.begin(9600);
  while (! Serial);
 // pinMode(reset_pin, OUTPUT);
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
  
 
}

void loop() 
{
 unsigned char send_data[172];
 if(runEvery(30000))
 {
   send_count++;
 }
 if(send_count==send_interval)
 {
   int k=0,t = 0;
   for(t = 0;t<sizeof(send_data);t++)
   {
       send_data[t] = 0; 
       
   }
   
   Serial.println("Reading Data 1");   
   read_meter(1,0,28);
   for(t = 3;t<59;t++)
   {
       send_data[k+t-3] = buffer[t]; 
       
   }
   Serial.println(t);
   k = 56;
   delay(500);
   Serial.println("Reading Data 2");   
   read_meter(1,28,28);
   for(t = 3;t<59;t++)
   {
       send_data[k+t-3] = buffer[t];        
   }
   Serial.println(t);
   k = 112;
   delay(500);
   Serial.println("Reading Data 3");   
   read_meter(1,56,22);
   for(t = 3;t<47;t++)
   {
       send_data[k+t-3] = buffer[t];        
   }
   Serial.println(t);

    k = 156;
   delay(500);
   Serial.println("Reading Data 4");   
   read_meter(1,470,8);
   for(t = 3;t<19;t++)
   {
       send_data[k+t-3] = buffer[t]; 
       
   }
   Serial.println(t);

   for (int i = 0; i < sizeof(send_data); i++) {
      Serial.print(send_data[i], HEX);
    }
    Serial.println();
    Serial.print("Full Length: ");Serial.println(sizeof(send_data));
   
   lora.sendData(send_data, sizeof(send_data), lora.frameCounter);
   Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
   
   lora.frameCounter++;
   reset_count++;
   send_count = 0;
 }
 if(reset_count>reset_interval)
 {
   digitalWrite(reset_pin, LOW); 
 }

  
}



float read_meter(int slave_add,int data_add,int reg_count)
{
   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   int t;
   unsigned char request[] = {0x01,0x03,0x00,0x00,0x00,0x20,0x44,0x12};
   request[0] = slave_add;
   request[2] = (data_add & 0xFF00)>>8;
   request[3] = (data_add & 0x00FF);
   request[4] = (reg_count & 0xFF00)>>8;
   request[5] = (reg_count & 0x00FF);
   int res = CRC16_2(request,6);
   t = res & 0x00ff;
   request[6] = t;
   t = (res & 0xff00)>>8;
   request[7] = t;

    Serial.println("tx: ");

    for (int i = 0; i < sizeof(request); i++) {
      Serial.print(request[i], HEX);
    }
    Serial.println();
  
   
   RS485Serial.write(request, sizeof(request));
   RS485Serial.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
   int rec_count = reg_count*2;
   while( RS485Serial.available()<(rec_count+6))
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
   int recvLen = RS485Serial.readBytes(buffer,rec_count+8);
    Serial.print("Rx: ");
    Serial.println(recvLen);
    for (int i = 0; i < recvLen; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  
  // Convert to Float
  if(recvLen>0){
  float x;
    
   ((byte*)&x)[3]= buffer[3];
   ((byte*)&x)[2]= buffer[4];
   ((byte*)&x)[1]= buffer[5];
   ((byte*)&x)[0]= buffer[6];
   
 //Convert to Float
   Serial.print("V1: ");
   Serial.print(x,2);
   Serial.println(" Volts"); 

  long y;
   
   ((byte*)&y)[3]= buffer[3];
   ((byte*)&y)[2]= buffer[4];
   ((byte*)&y)[1]= buffer[5];
   ((byte*)&y)[0]= buffer[6];
   
 //Convert to Float
   Serial.print("kWh: ");
   Serial.print(y);
   Serial.println(" kWh"); 
   Serial.println(buffer[3],HEX);
   Serial.println(buffer[4],HEX);
   Serial.println(buffer[5],HEX);
   Serial.println(buffer[6],HEX);
   
   
   return(x);
  }
  else
  {
      Serial.print("wrong crc ");
      
  } 

}

unsigned int CRC16_2(byte *buf, int len)
{  
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }

  return crc;
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
