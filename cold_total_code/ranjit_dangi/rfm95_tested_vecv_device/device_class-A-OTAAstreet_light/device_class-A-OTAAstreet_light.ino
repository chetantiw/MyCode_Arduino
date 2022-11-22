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
const char *devEui = "3d66c7c102957142";
const char *appEui = "0000000000000000";
const char *appKey = "e19e8c4c4356a13adbf0c20875b8caeb";

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

String st = "";
const int sensorIn = A0;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 55;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

const sRFM_pins RFM_pins = {
  .CS = 10,
  .RST = 9,
  .DIO0 = 2,
  .DIO1 = 15,

};

int status_pin =20;
int switch_pin1 =3;
int switch_pin2 =5;


void setup() {
  // Setup loraid access
 pinMode(status_pin, INPUT);
 pinMode(switch_pin1, OUTPUT);
 pinMode(switch_pin2, OUTPUT);
digitalWrite(status_pin,HIGH);
digitalWrite(switch_pin1,LOW);
digitalWrite(switch_pin2,LOW);
  
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

   Voltage = getVPP();
  Voltage = Voltage;
  Serial.print("voltage =");
  Serial.println(Voltage);
  if(Voltage<0.05)
  Voltage = 0;
  
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = (VRMS * 1000)/mVperAmp;

  
  Serial.print("current=");
  Serial.println(AmpsRMS);
  Watt = (AmpsRMS*230);      // 1.3 is an empirical calibration factor
  Serial.print("power=");
  Serial.println(Watt);


  
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
//     outStr[6];
//      if(outStr[6]=='1')
//      {
//         Serial.println("light On");
//         digitalWrite(switch_pin,HIGH);
//         
//      }
//      else if(outStr[6]=='0')
//      {
//         Serial.println("light Off");
//         digitalWrite(switch_pin,LOW);
//      }
//      delay(500);
//             
//      int state = digitalRead(status_pin);
//      if(state==LOW)
//      {
//          st = "{\"light_status\":\"On\"}";
//          st.toCharArray(myStr, 22);
//      }
//      else
//      {
//          st = "{\"light_status\":\"Off\"}";
//          st.toCharArray(myStr, 23);
//      }
         
      

   
      
    }
    
 
    
  
  
  // Check Lora RX
  lora.update();
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
