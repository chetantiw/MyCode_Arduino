
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

byte buffer[15];

float read_meter(char para);
void get_pmu_data();
long kwh = 0;

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
  
   logdata = "{";
   get_pmu_data();
   Serial.println("Reading Data...");   
   Serial.println(logdata);
   Serial.println(logdata.length());
   
  
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
  //  Serial.print("Rx: ");
  
  //  for (int i = 0; i < recvLen; i++) {
  //    Serial.print(buffer[i], HEX);
  //  }
  //  Serial.println();
  
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
   long tmp,tr;
   if(para=='a')
   {
       ((byte*)&kwh)[3]= buffer[3];
         ((byte*)&kwh)[2]= buffer[4];
         ((byte*)&kwh)[1]= buffer[5];
         ((byte*)&kwh)[0]= buffer[6];
      
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
