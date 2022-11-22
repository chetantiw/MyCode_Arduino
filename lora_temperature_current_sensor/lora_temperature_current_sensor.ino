
 
 //********************************************************************/

#include <SPI.h> 
#include <TinyLoRa.h>   
/********************************************************************/

// Net  Session Key (MSB) 01    02    03    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   45 
uint8_t NwkSkey[16] = { 0x01, 0x02, 0x03, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0x45 };

// App Session Key (MSB)  91    92    93    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    35 
uint8_t AppSkey[16] = { 0x91, 0x92, 0x93, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0x35 };

// Device Address (MSB)  01   33   64   85
uint8_t DevAddr[4] = { 0x01,0x33,0x64,0x85 };

TinyLoRa lora = TinyLoRa(2, 10, 9);

//********************************************************************/

const unsigned long sendInterval = 30000; 
int alarm = 0;
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
//float TOTAL = 0;
float LS = 0;
const int input = 3;

// Data wire is plugged into pin 2 on the Arduino 
const int sensorIn = A6;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 55;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;


int REL1  =4;
int led1 =5;
int led2 =6;
int led3 =7;

int temp_count = 0;
int curr_count = 0;
int flow_count = 0;

int temperature;

 
void setup(void) 
{    pinMode(input,INPUT);

     pinMode(REL1, OUTPUT);
     digitalWrite(REL1,HIGH);
     pinMode(led1, OUTPUT);
     digitalWrite(led1,LOW);
     pinMode(led2, OUTPUT);
     digitalWrite(led2,LOW);
     pinMode(led3, OUTPUT);
     digitalWrite(led3,LOW);

 // start serial port 
    Serial.begin(9600); 
 
    alarm = 0;

   // Initialize LoRa
    //Serial.print("Starting LoRa...");
    // define multi-channel sending
    lora.setChannel(MULTI);
    // set datarate
    lora.setDatarate(SF7BW125);
    if(!lora.begin())
    {
      Serial.println("Failed");
      //Serial.println("Check your radio");
      while(true);
    }
    
   
    Serial.println("LoRa  succeeded.");
    //Serial.println("Condition Monitoring demo start...");
   // Serial.println();
    delay(20000);
 
} 



void loop(void) 
{ 

 
 temperature = analogRead(A2);  //temp adc
 temperature = temperature*0.488;
 delay(100);
 Serial.print("T: "); 
 Serial.println(temperature); // Why "byIndex"?  
 Voltage = getVPP();
// Serial.println(Voltage);
 if(Voltage<0.05)
 Voltage = 0;
 VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 Serial.print("C: "); 
 Serial.println(AmpsRMS); 
 X = pulseIn(input, HIGH);
 Y = pulseIn(input, LOW);
 TIME = X + Y;
 FREQUENCY = 1000000/TIME;
 WATER = FREQUENCY/7.5;
 LS = WATER/60;
 if(FREQUENCY >= 0)
 {
    if(isinf(FREQUENCY))
    {
       WATER  = 0.00;
       flow_count++;
    }
    else
    {
      flow_count = 0;
    }
    Serial.print("F: ");
    Serial.print(WATER);
    Serial.println(" L/M");
 }
  if((AmpsRMS>5.5) )
  {
     alarm = 1;
     digitalWrite(led1,HIGH);
  }
 

  if(temperature>=50)
  {
    temp_count++;
    if(temp_count>20)
    {
       alarm = 3;
       digitalWrite(led2,HIGH);
    }
    
  }
  else
  {
     temp_count = 0;
  }

  if(flow_count>10)
  {
       alarm = 4;
       digitalWrite(led3,HIGH);
      
      
  }
  int send_ct=0;
  if(alarm>0)
  {
    digitalWrite(REL1,LOW);
    while(1)
    {
      
      send_ct++;
      delay(5000);
      if(send_ct<5)
      {
         send_data();
      }
    }
    
  }
  

  if (runEvery(sendInterval) )
  {
     send_data();
  }
   delay(1000);   

} 


void send_data()
{
String sendStr = "{";
    sendStr += "\"";
    sendStr += "NodeID\":";
    sendStr += "\"LWCMS0001";
    sendStr += "\",";
    
    sendStr += "\"";
    sendStr += "Temperature\":";
    sendStr += String(temperature);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "Current\":";
    sendStr += String(AmpsRMS);
    sendStr += ",";

//    sendStr += "\"";
//    sendStr += "Voltage\":";
//    sendStr += String(voltage);
//    sendStr += ",";

    sendStr += "\"";
    sendStr += "Flow\":";
    sendStr += String(WATER);
    sendStr += ",";
    sendStr += "\"";
    sendStr += "Alarm\":";
    sendStr += String(alarm);
    sendStr += "}";

 //   Serial.println("Sending LoRa Data...");   
    Serial.println(sendStr);
    char data[sendStr.length()+1];
    sendStr.toCharArray(data,sendStr.length()+1);
//    for(int i=0;i<sizeof(data);i++)
//    {
//       Serial.print(data[i]);
//    }
//    Serial.println();
    lora.sendData(data, sizeof(data), lora.frameCounter);

 //   Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
    lora.frameCounter++;
    

   // Serial.println("Send Message!");  
  
}

float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
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
