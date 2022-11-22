
#include "MHZ19.h" 
#include "SparkFunHTU21D.h"
#include <SoftwareSerial.h>
#include <SimpleModbusSlaveSoftwareSerial.h>


//Create an instance of the object
HTU21D myHumidity;
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;  
long dmillis;
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

#define RX            2    // Arduino defined pin (PB0, package pin #5)
#define TX            3     // Arduino defined pin (PB1, package pin #6)
#define RS485_EN      4     // pin to set transmission mode on RS485 chip (PB2, package pin #7)
#define BAUD_RATE     9600  // baud rate for serial communication
#define deviceID      2     // this device address

// SoftwareSerial mySerial(receive pin, transmit pin)
SoftwareSerial rs485(RX, TX); 




void setup() {

  modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
  Serial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
  myMHZ19.begin(Serial);                                // *Serial(Stream) refence must be passed to library begin(). 
  myHumidity.begin();
  myMHZ19.autoCalibration(); 
  delay(2000);
      
}

void loop() {      
  // Reading temperature or humidity takes about 250 milliseconds!
  float humd = myHumidity.readHumidity();
  delay(5);
  float temp =myHumidity.readTemperature();
  delay(5);
  int CO2 = myMHZ19.getCO2();


 
 holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
 holdingRegs[1] = temp;    //  temperature as Celsius (the default)
 holdingRegs[2] = humd;    //  humidity as relative
 holdingRegs[3] = CO2;  //  heat index in Fahrenheit (the default)
//  holdingRegs[4] = hic;  //  heat index in Celsius (isFahreheit = false)
  delayMicroseconds(50);   
  
    
}
