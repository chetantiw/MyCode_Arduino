
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

  current = analogRead(A0);
 
  send_packet();
  debugSerial.println("current: "+String(current));
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
     
   int len = 38;
   if(current >= 10 && current <=99)
    len = 39;
   else if(current >99)
     len = 40
   byte payload[len];

   String load_string =  "{light_1\":";
   load_string +=  String(switch_1).charAt(0);
   load_string +=  ",\"light_2\":";
   load_string +=  String(switch_2).charAt(0);
   load_string += ",\"current\":";
   load_string +=  String(current).charAt(0);
   if(current >=10)
      load_string +=  String(current).charAt(1);
   if(current > 99)
      load_string +=  String(current).charAt(2);
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
     switch_1 = (int)(payload[6]-0x30);
     switch_2 = (int)(payload[15]-0x30);
     time_d = (int)(((payload[24]-0x30)*10)+(payload[25]-0x30));

     Serial.println("Received_1: ");
     Serial.println(switch_1);
     Serial.println(switch_2);
     Serial.println(time_d);
     
  }
 
}
