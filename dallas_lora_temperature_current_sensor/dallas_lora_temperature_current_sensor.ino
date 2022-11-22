//********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SPI.h> 
#include <TinyLoRa.h>   
/********************************************************************/

// Net  Session Key (MSB) 01    02    03    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e    48
uint8_t NwkSkey[16] = { 0x01, 0x02, 0x03, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0x48 };

// App Session Key (MSB)  91    92    93    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    38 
uint8_t AppSkey[16] = { 0x91, 0x92, 0x93, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0x38 };

// Device Address (MSB)  01   33   64   88
uint8_t DevAddr[4] = { 0x01,0x33,0x64,0x88 };

const unsigned long sendInterval = 30000;
int alarm = 0;

TinyLoRa lora = TinyLoRa(2, 10, 9);

//********************************************************************/

// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 14 
int mo_pin1 =3;
int mo_pin2 =5;
//int reset_pin = 7;
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 
void setup(void) 
{ 
     pinMode(mo_pin1, OUTPUT);
     pinMode(mo_pin2, OUTPUT);
     //pinMode(reset_pin, OUTPUT);
     digitalWrite(mo_pin1,HIGH);
     digitalWrite(mo_pin2,HIGH);
     //digitalWrite(reset_pin,HIGH);
 // start serial port 
    Serial.begin(9600); 
 
 // Start up the library 
    sensors.begin(); 

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
    Serial.println("Condition Monitoring demo start...");
    Serial.println();
    digitalWrite(mo_pin1,HIGH);
    digitalWrite(mo_pin2,HIGH);
    delay(2000);
    
 
} 
int temp_cout = 0;

void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 Serial.print(" Requesting ..."); 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.println("DONE"); 
/********************************************************************/
 Serial.print("Temperature is: "); 
 Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"? 
 int temp = sensors.getTempCByIndex(0); 
 
 int sensorValue = analogRead(A1);
 sensorValue = sensorValue*1.58;
 // print out the value you read:
 Serial.print("  current is: "); 
 Serial.println(sensorValue); 



  if(sensorValue>3500)
  {
     alarm = 1;
     digitalWrite(mo_pin1,LOW);
     digitalWrite(mo_pin2,LOW);
     
  }
  if(temp>45)
  {
    temp_cout++;
    if(temp_cout>50)
    {
     alarm = 3;
     digitalWrite(mo_pin1,LOW);
     digitalWrite(mo_pin2,LOW);
     delay(100);
    }
    
  }
  else
  {
     temp_cout = 0;
  }

     

  if (runEvery(sendInterval))
  {
    String sendStr = "{";
    sendStr += "\"";
    sendStr += "NodeID\":";
    sendStr += "\"LWCMS0004";
    sendStr += "\",";
    
    sendStr += "\"";
    sendStr += "Temperature\":";
    sendStr += String(sensors.getTempCByIndex(0));
    sendStr += ",";

    sendStr += "\"";
    sendStr += "Current\":";
    sendStr += String(sensorValue);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "Voltage\":";
    sendStr += String(230.1);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "Flow\":";
    sendStr += String(50);
    sendStr += ",";
    sendStr += "\"";
    sendStr += "Alarm\":";
    sendStr += String(alarm);
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
  }
   delay(500);

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
