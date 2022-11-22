#include <SoftwareSerial.h>
#include <SimpleModbusSlaveSoftwareSerial.h>
#include <Arduino.h>                                   
#include "DHT.h"
#include "MHZ19.h" 

String decvID =     "ENV0001";                    //43
String deviceID = "\"ENV0001\"";

#define DHTPIN 3     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
#define RX_PIN 0                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 1                                           // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
unsigned long getDataTimer = 0;
MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
//unsigned long getDataTimer = 0;

enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  PWM_0,
  PWM_1,
  ADC2,
  ADC3,
  ADC4,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

#define RX            5    // Arduino defined pin (PB0, package pin #5)
#define TX            6     // Arduino defined pin (PB1, package pin #6)
#define RS485_EN      7     // pin to set transmission mode on RS485 chip (PB2, package pin #7)
#define BAUD_RATE     9600  // baud rate for serial communication
#define deviceID      1    // this device address

// SoftwareSerial mySerial(receive pin, transmit pin)
SoftwareSerial rs485(RX, TX); 
String message = "";
float f,t,h;
void setup()
{
  /* parameters(
                SoftwareSerial* comPort
                long baudrate, 
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size)
                
     The transmit enable pin is used in half duplex communication to activate a MAX485 or similar
     to deactivate this mode use any value < 2 because 0 & 1 is reserved for Rx & Tx
  */
  //Serial.begin(9600); 
  modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
  //Serial.println(F("DHTxx Intilized!"));
  dht.begin();
  Serial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
  myMHZ19.begin(Serial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration(); 
}

void loop()
{
         h = dht.readHumidity();
        // Read temperature as Celsius (the default)
         t = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
         f = dht.readTemperature(true);
      
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
          //Serial.println(F("Failed to read from DHT sensor!"));
          return;
        }
      
        // Compute heat index in Fahrenheit (the default)
        float hif = dht.computeHeatIndex(f, h);
        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(t, h, false);
      
//        Serial.print(F("Humidity: "));
//        Serial.print(h);
//        Serial.print(F("%  Temperature: "));
//        Serial.print(t);
//        Serial.print(F("°C "));
//        Serial.print(f);
//        Serial.print(F("°F  Heat index: "));
//        Serial.print(hic);
//        Serial.print(F("°C "));
//        Serial.print(hif);
//        Serial.println(F("°F"));

                int CO2; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        //Serial.print("CO2 (ppm): ");                      
        //Serial.println(CO2);    
       holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
       holdingRegs[1] = h;    //  temperature as Celsius (the default)
       holdingRegs[2] = t;    //  humidity as relative     
       holdingRegs[3] = hic;  //  heat index in Fahrenheit (the default)
       holdingRegs[4] = hif;  //  heat index in Celsius (isFahreheit = false)
        if(CO2<400)
         {
             holdingRegs[5] = 400;    //  co2                                
         }
         if(CO2>400)
         {
            holdingRegs[5] = CO2;    //  co2                             
         }
       
  delayMicroseconds(100);
        
}
