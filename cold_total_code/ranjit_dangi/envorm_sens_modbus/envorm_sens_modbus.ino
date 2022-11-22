 #include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial
#include "DHT.h"
#include <SimpleModbusSlaveSoftwareSerial.h>

String decvID =     "ENV0001";                    //43
String deviceID = "\"ENV0001\"";

#define DHTPIN 3     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define RX_PIN 14                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 15                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
DHT dht(DHTPIN, DHTTYPE);
unsigned long getDataTimer = 0;



String message = "";
float f,t,h;

enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  PWM_0,
  PWM_1,
  ADC4,
  ADC3,
  ADC2,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

#define RX            5     // Arduino defined pin (PB0, package pin #5)
#define TX            6     // Arduino defined pin (PB1, package pin #6)
#define RS485_EN      7     // pin to set transmission mode on RS485 chip (PB2, package pin #7)
#define BAUD_RATE     9600  // baud rate for serial communication
#define deviceID      1     // this device address

// SoftwareSerial mySerial(receive pin, transmit pin)
SoftwareSerial rs485(RX, TX); 
void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
    modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
   
    Serial.println("MODBUS Enviromental sensor");
    Serial.println("CO2, Humidity, Temperature");

  
  
    Serial.println();


    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
    Serial.println(F("DHTxx Intilized!"));
    dht.begin();
   
    delay(2000);
    
}

void loop()
{  
  int CO2;

  
    
  
   if (runEvery(200)) { // repeat every 1000 millis 
    
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
        
       holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
       holdingRegs[1] = t;    //  temperature as Celsius (the default)
       holdingRegs[2] = h;    //  humidity as relative
       holdingRegs[3] = CO2;    //  co2 
//  holdingRegs[3] = hif;  //  heat index in Fahrenheit (the default)
//  holdingRegs[4] = hic;  //  heat index in Celsius (isFahreheit = false)
  delayMicroseconds(50);
        
//
//   holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
//  for (byte i = ADC1; i < TOTAL_ERRORS; i++)
//  {
//    holdingRegs[i] = analogRead(i);
//    delayMicroseconds(50);    
//  }

   
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
