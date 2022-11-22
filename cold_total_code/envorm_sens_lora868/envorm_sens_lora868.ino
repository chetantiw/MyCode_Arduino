#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial
#include "DHT.h"
#include <SPI.h> 
#include <TinyLoRa.h>             // include libraries


#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define RX_PIN 15                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 16                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
DHT dht(DHTPIN, DHTTYPE);
unsigned long getDataTimer = 0;

String deviceID = "\"ENV0001\"";


// Net  Session Key (MSB) 01    02    03    8f    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   c0
uint8_t NwkSkey[16] = { 0x01, 0x02, 0x03, 0x8f, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xc0 };

// App Session Key (MSB)  91    92    93    0e    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    cb 
uint8_t AppSkey[16] = { 0x91, 0x92, 0x93, 0x0e, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xcb };

// Device Address (MSB)  01   33   64   65
uint8_t DevAddr[4] = { 0x01,0x33,0x64,0x65 };

const unsigned long sendInterval = 30000;

float f,t,h;

TinyLoRa lora = TinyLoRa(2, 10, 9);

void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback
    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
     Serial.println("LoRa Enviromental sensor");
    Serial.println("CO2, Humidity, Temperature");
    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
    Serial.println(F("DHTxx Intilized!"));
    dht.begin();
  
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
  int CO2;
  
   delay(3000);
    
        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        Serial.print("CO2 (ppm): ");                      
        Serial.println(CO2);                                               
        getDataTimer = millis();
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        delay(500);
         h = dht.readHumidity();
        // Read temperature as Celsius (the default)
         t = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
         f = dht.readTemperature(true);
      
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
          Serial.println(F("Failed to read from DHT sensor!"));
          return;
        }
      
        // Compute heat index in Fahrenheit (the default)
        float hif = dht.computeHeatIndex(f, h);
        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(t, h, false);
      
        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("°C "));
        Serial.print(f);
        Serial.print(F("°F  Heat index: "));
        Serial.print(hic);
        Serial.print(F("°C "));
        Serial.print(hif);
        Serial.println(F("°F"));

 
  
   
  if (runEvery(sendInterval))
  {
    String sendStr = "{";
    sendStr += "\"";
    sendStr += "NodeID\":";
    sendStr += deviceID;
    sendStr += ",";
    
    sendStr += "\"";
    sendStr += "Temperature\":";
    sendStr += String(t);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "Humidity\":";
    sendStr += String(h);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "CO2\":";
    sendStr += String(CO2);
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
