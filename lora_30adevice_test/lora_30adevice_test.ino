#include <loraid.h>

long interval = 10000;    // 10 s interval to send message
long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

int relay1 = 14;
int relay2 = 5;
int relay3 = 3;
void setup() {
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  digitalWrite(relay1,LOW);
  digitalWrite(relay2,LOW);
  digitalWrite(relay3,LOW);
  // Setup loraid access
  lora.init();

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(2);
  
  // Put Antares Key and DevAddress here
  lora.AccessKey((unsigned char *)"8878f39f897b9a50:bd6b3446f4c13871", (unsigned char *)"00000001");
}

void loop() {
  // put your main code here, to run repeatedly:
  char myStr[50];
  unsigned long currentMillis = millis();

  // Check interval overflow
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 

    sprintf(myStr, "Ini data LoRa ke-%d", counter); 
    lora.sendToAntares((unsigned char *)myStr, strlen(myStr), 0);
  }
 
  // Check Lora RX
  lora.update();



   digitalWrite(relay1,HIGH);
   digitalWrite(relay2,HIGH);
   digitalWrite(relay3,HIGH);
   delay(1000);
   digitalWrite(relay1,LOW);
   digitalWrite(relay2,LOW);
   digitalWrite(relay3,LOW);
   delay(1000);

//    if(switch_1==1)
//  {
//    digitalWrite(relay1,HIGH);
//    digitalWrite(relay2,HIGH);
//    digitalWrite(relay3,HIGH);
//  }
//  else if(switch_1==0)
//  {
//  digitalWrite(relay1,LOW);
//  digitalWrite(relay2,LOW);
//  digitalWrite(relay3,LOW);
//  }
}
