/**
 * Example of OTAA device      
 * Authors: 
 *        Ivan Moreno
 *        Eduardo Contreras
 *  June 2019
 * 
 * This code is beerware; if you see me (or any other collaborator 
 * member) at the local, and you've found our code helpful, 
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

 //{"SW":1}
 //{"SW":0}
#include <lorawan.h>

// OTAA credentials
const char *devEui = "f90b2b3bacd4790b";
const char *appEui = "f90b2b3bacd4790c";
const char *appKey = "2902c5560ad3f7ea192699210f0c2ed0";

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

String st = "";

const sRFM_pins RFM_pins = {
  .CS = 10,
  .RST = 9,
  .DIO0 = 2,
  .DIO1 = 15,

};

int status_pin =3;
int switch_pin =5;


void setup() {
  // Setup loraid access
 pinMode(status_pin, INPUT);
 pinMode(switch_pin, OUTPUT);

digitalWrite(status_pin,HIGH);
digitalWrite(switch_pin,LOW);
  
  Serial.begin(115200);
  delay(2000);
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }
  st = "{\"light_status\":\"OFF\"}";
  st.toCharArray(myStr, 23);
  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF9BW125);

  // set channel to random
  lora.setChannel(MULTI);
  
  // Put OTAA Key and DevAddress here
  lora.setDevEUI(devEui);
  lora.setAppEUI(appEui);
  lora.setAppKey(appKey);

  // Join procedure
  bool isJoined;
  do {
    Serial.println("Joining...");
    isJoined = lora.join();
    
    //wait for 10s to try again
    delay(10000);
  }while(!isJoined);
  Serial.println("Joined to network");
}

void loop() {
  // Check interval overflow
  if(millis() - previousMillis > interval) {
    previousMillis = millis(); 

    //sprintf(myStr, "Counter-%d", counter); 

    Serial.print("Sending: ");
    Serial.println(myStr);
    
    lora.sendUplink(myStr, strlen(myStr), 0);
    counter++;
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
      Serial.println(outStr);
     outStr[6];
      if(outStr[6]=='1')
      {
         Serial.println("light On");
         digitalWrite(switch_pin,HIGH);
         
      }
      else if(outStr[6]=='0')
      {
         Serial.println("light Off");
         digitalWrite(switch_pin,LOW);
      }
      delay(500);
             
      int state = digitalRead(status_pin);
      if(state==LOW)
      {
          st = "{\"light_status\":\"On\"}";
          st.toCharArray(myStr, 22);
      }
      else
      {
          st = "{\"light_status\":\"Off\"}";
          st.toCharArray(myStr, 23);
      }
         
      

   
      
    }
    
 
    
  
  
  // Check Lora RX
  lora.update();
}
