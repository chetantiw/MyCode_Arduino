#include <SoftwareSerial.h>
#include <SimpleModbusSlaveSoftwareSerial.h>

 #include <Arduino.h>                                   
 #include "DHT.h"


String decvID =     "ENV0001";                    //43
String deviceID = "\"ENV0001\"";

#define DHTPIN 3     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
unsigned long getDataTimer = 0;
/* This example code has 9 holding registers. 6 analogue inputs, 1 button, 1 digital output
   and 1 register to indicate errors encountered since started.
   Function 5 (write single coil) is not implemented so I'm using a whole register
   and function 16 to set the onboard Led on the Atmega328P.
   
   The modbus_update() method updates the holdingRegs register array and checks communication.

   Note:  
   The Arduino serial ring buffer is 128 bytes or 64 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 122 bytes or 61 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   118 bytes or 59 registers.
 
   Using the FTDI USB to Serial converter the maximum bytes you can send is limited 
   to its internal buffer which is 60 bytes or 30 unsigned int registers. 
 
   Thus:
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 54 bytes or 27 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   50 bytes or 25 registers.
 
   Since it is assumed that you will mostly use the Arduino to connect to a 
   master without using a USB to Serial converter the internal buffer is set
   the same as the Arduino Serial ring buffer which is 128 bytes.
*/
 

// Using the enum instruction allows for an easy method for adding and 
// removing registers. Doing it this way saves you #defining the size 
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

//////////////// registers of your slave ///////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  PWM_0,
  PWM_1,
  ADC0,
  ADC1,
  ADC2,
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
#define deviceID      1     // this device address

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
  Serial.begin(9600); 
  modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
   Serial.println(F("DHTxx Intilized!"));
    dht.begin();
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
       holdingRegs[1] = h;    //  temperature as Celsius (the default)
       holdingRegs[2] = t;    //  humidity as relative     
       holdingRegs[3] = hic;  //  heat index in Fahrenheit (the default)
       holdingRegs[4] = hif;  //  heat index in Celsius (isFahreheit = false)
      // holdingRegs[5] = co2;    //  co2 
  delayMicroseconds(100);
        
}
