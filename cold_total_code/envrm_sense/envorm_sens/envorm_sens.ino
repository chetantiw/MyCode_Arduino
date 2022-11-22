#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial
#include "DHT.h"
#include <SPI.h>              // include libraries
#include <LoRa.h>

String decvID =     "ENV0001";                    //43
String deviceID = "\"ENV0001\"";

#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define RX_PIN 15                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 16                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
DHT dht(DHTPIN, DHTTYPE);
unsigned long getDataTimer = 0;

const long frequency = 433;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin
String message = "";
float f,t,h;

void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
 
    Serial.println("LoRa Enviromental sensor");
    Serial.println("CO2, Humidity, Temperature");

    LoRa.setPins(csPin, resetPin, irqPin);
    LoRa.setSpreadingFactor(13);
    LoRa.setTxPower(20);

    if (!LoRa.begin(frequency)) {
      Serial.println("LoRa init failed. Check your connections.");
      while (true);                       // if failed, do nothing
    }
    
    LoRa.onReceive(onReceive);
    LoRa.onTxDone(onTxDone);
    LoRa_rxMode();

    Serial.println("LoRa init succeeded.");
    Serial.println();
   
    Serial.println("Tx: invertIQ disable");
    Serial.println("Rx: invertIQ enable");
    Serial.println();


    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
    Serial.println(F("DHTxx Intilized!"));
    dht.begin();
   
    delay(2000);
    
}

void loop()
{  
  int CO2;
  
   if (runEvery(2000)) { // repeat every 1000 millis 
    
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

  

   
  }
   
  if(message==decvID)
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
   
    LoRa_sendMessage(sendStr); // send a message
    message = "";
    Serial.println("Send Message!");
  }
   

   
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String msg) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(msg);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  
  message = "";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
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
