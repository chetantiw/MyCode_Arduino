#include <Arduino.h>
#include <SPI.h> 
#include <TinyLoRa.h>             // include libraries

String deviceID = "\"LWED0061\"";

// Net  Session Key (MSB) 11    22    33    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   c0
uint8_t NwkSkey[16] = { 0x11, 0x22, 0x33, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xc0 };

// App Session Key (MSB)  92    93    94    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    cb 
uint8_t AppSkey[16] = { 0x92, 0x93, 0x94, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xcb };

// Device Address (MSB)  11   22   44   66
uint8_t DevAddr[4] = { 0x11,0x22,0x44,0x66 };

const unsigned long sendInterval = 30000;

int opt_sense = 14;
int pre_val = false;
int interval = 1;

unsigned long frame_counter = 0;


TinyLoRa lora = TinyLoRa(2, 10, 9);

void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback
    pinMode(opt_sense,INPUT);
    digitalWrite(opt_sense,HIGH);
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
    Serial.println("LoRa Envorimental sense");
    Serial.println();
    
   
    delay(2000);
    
}

void loop()
{  
 
  int val = digitalRead(opt_sense);
 
  if(val!=pre_val)
  {
   
      
        String sendStr = "{";
        sendStr += "\"";
        sendStr += "NodeID\":";
        sendStr += deviceID;
        sendStr += ",";
        sendStr += "\"";
        sendStr += "Obstacle\":";
        sendStr += val;
        sendStr += "}";
        Serial.println("Sending LoRa Data...");   
        Serial.println(sendStr);
        char data[sendStr.length()+1];
        sendStr.toCharArray(data,sendStr.length()+1);
        for(int i=0;i<sizeof(data);i++)
        {
          Serial.print(data[i]);
        }
        Serial.println();
        lora.sendData(data, sizeof(data), lora.frameCounter);
        Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
        lora.frameCounter++;
        Serial.println("Send Message!");
        delay(1000);
     
    pre_val = val;
  }

   if( runEvery(interval*60000))
    {
        String sendStr = "{";
        sendStr += "\"";
        sendStr += "NodeID\":";
        sendStr += deviceID;
        sendStr += ",";
        sendStr += "\"";
        sendStr += "Obstacle\":";
        sendStr += val;
        sendStr += ",";
        sendStr += "}";
        Serial.println("Sending LoRa Data...");   
        Serial.println(sendStr);
        char data[sendStr.length()+1];
        sendStr.toCharArray(data,sendStr.length()+1);
        for(int i=0;i<sizeof(data);i++)
        {
          Serial.print(data[i]);
        }
        Serial.println();
        lora.sendData(data, sizeof(data), lora.frameCounter);
        Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
        lora.frameCounter++;
        Serial.println("Send Message!");
        delay(1000);
    
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
