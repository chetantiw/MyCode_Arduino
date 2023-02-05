
#include <SimpleModbusSlaveSoftwareSerial.h>                                  
#include "DHT.h"



#define DHTPIN 3     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);                                      
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
unsigned long getDataTimer = 0;                                            // Constructor for library
long dmillis;


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
#define deviceID      1
// SoftwareSerial mySerial(receive pin, transmit pin)
SoftwareSerial rs485(RX, TX); 
 
void setup()
{

  Serial.begin(9600); 
  modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
  dht.begin();
  Serial.println("dht modbus");
 
}

void loop()
{        float f,t,h;
         int co2=450;
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


        
       holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
       holdingRegs[1] = t;    //  temperature as Celsius (the default)
       holdingRegs[2] = h;    //  humidity as relative     
       holdingRegs[3] = co2;  
       Serial.print("H:");
       Serial.println(h);
       Serial.print("T:");
       Serial.println(t);
       Serial.print("C:");
       Serial.println(co2);
   
  delayMicroseconds(100);
        
}
