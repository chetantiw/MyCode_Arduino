
/*
 *{"L1":1,
 "L2":1,
 "TD":30}
 */
#include <TheThingsNetwork.h>
#include <TheThingsMessage.h>
#include <SoftwareSerial.h>

SoftwareSerial ser1(2, 3); // RX, TX
// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
//  const char *devAddr = "02017201";   //0004A30B00EEFA23
//  const char *nwkSKey = "70076e868fcee0f2a1b3887610e850b6";
//  const char *appSKey = "a6f734d42a163db7ea8728901d06fc29";


  // Set your AppEUI and AppKey
                      

const char *appEui = "0004A30B00EC97CC";
 const char *appKey = "fb0a785ab9b50eb0fc1eafca34896994";

#define loraSerial ser1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

int relay1 = 4;
int relay2 = 5;
int relay3 = 6;
int relay4 = 7;
int relay5 = 8;

const int sensorIn = A2;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 55;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

int switch_1,switch_2,current,time_d=0;
void send_packet();

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
  pinMode(relay5,OUTPUT);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  ttn.onMessage(message);

 // debugSerial.println("-- PERSONALIZE");
///  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey, -1, 10000, CLASS_C);

  
}

void loop()
{
  debugSerial.println("-- LOOP");

  Voltage = getVPP();
  Voltage = Voltage;
  //Serial.println(Voltage);
  if(Voltage<0.05)
  Voltage = 0;
  
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = (VRMS * 1000)/mVperAmp;

  //Serial.print(AmpsRMS);
  //Serial.print(" Amps RMS  ---  ");
  Watt = (AmpsRMS*230);      // 1.3 is an empirical calibration factor
  //Serial.print(Watt);
  //Serial.println(" W");
 
  send_packet();
  debugSerial.println("current: "+String(AmpsRMS));
  Serial.println();
 
  
  if(switch_1==1)
  {
    digitalWrite(relay1,HIGH);
  }
  else if(switch_1==0)
  {
    digitalWrite(relay1,LOW);
  }
  if(switch_2==1)
  {
    digitalWrite(relay2,HIGH);
  }
  else if(switch_2==0)
  {
    digitalWrite(relay2,LOW);
  }
  
  delay(15000);
}


void send_packet()
{
     
   int len = 41;
  
   byte payload[len];

   String load_string =  "{\"light_1\":";
   load_string +=  String(switch_1).charAt(0);
   load_string +=  ",\"light_2\":";
   load_string +=  String(switch_2).charAt(0);
   load_string += ",\"current\":";
   load_string +=  String(AmpsRMS).charAt(0);
   load_string +=  String(AmpsRMS).charAt(1);
   load_string +=  String(AmpsRMS).charAt(2);
   load_string +=  String(AmpsRMS).charAt(3);
   load_string +=  "}";
   load_string.toCharArray(payload, len);
   ttn.sendBytes(payload, sizeof(payload));
   Serial.println(load_string);
}




void message(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println("-- MESSAGE");
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ":");

  for (int i = 0; i < size; i++)
  {
    debugSerial.write((payload[i]));
  }
  debugSerial.println();

  if(payload[0]=='{')
  {
     int LightNo = (int)(payload[3]-0x30); //(String)(((payload[3]-0x30)*10)+(payload[4]-0x30));
     if(LightNo == 1)
      switch_1 = (int)(payload[6]-0x30);
     if(LightNo == 2)
        switch_2 = (int)(payload[6]-0x30);
        
     //time_d = (int)(((payload[24]-0x30)*10)+(payload[25]-0x30));

     Serial.println("Received_1: ");
      Serial.println(LightNo);
     Serial.println(switch_1);
     Serial.println(switch_2);
     //Serial.println(time_d);
     
  }
 
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
