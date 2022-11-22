
#include <SimpleModbusSlaveSoftwareSerial.h>                                
#include "MHZ19.h" 
#include "SparkFunHTU21D.h"

//Create an instance of the object
HTU21D myHumidity;
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                             // Constructor for library
long dmillis;

enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
 ADC6,
 ADC7,
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

#define RX            5   // Arduino defined pin (PB0, package pin #5)
#define TX            6     // Arduino defined pin (PB1, package pin #6)
#define RS485_EN      7         // pin to set transmission mode on RS485 chip (PB2, package pin #7)
#define BAUD_RATE     9600  // baud rate for serial communication
#define deviceID      2
// this device address

// SoftwareSerial mySerial(receive pin, transmit pin)
SoftwareSerial rs485(RX, TX); 
//String message = "";

void setup()
{

 //Serial.begin(9600); 
  modbus_configure(&rs485, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);

  Serial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
  myMHZ19.begin(Serial);                                // *Serial(Stream) refence must be passed to library begin(). 
  myHumidity.begin();
  myMHZ19.autoCalibration(); 

}

void loop()
{
        
         float humd = myHumidity.readHumidity();
         float temp = myHumidity.readTemperature();


//  Serial.print(" Temperature:");
//  Serial.print(temp, 1);
//  Serial.print("C");
//  Serial.print(" Humidity:");
//  Serial.print(humd, 1);
//  Serial.print("%");

        int CO2; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        //Serial.print("CO2 (ppm): ");                      
        //Serial.println(CO2);    
       holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
       holdingRegs[1] = temp;    //  temperature as Celsius (the default)
       holdingRegs[2] = humd;    //  humidity as relative     
      // holdingRegs[3] = 0;  //  heat index in Fahrenheit (the default)
      // holdingRegs[4] = 0;  //  heat index in Celsius (isFahreheit = false)
        if(CO2<=400)
         {
             holdingRegs[3] = 400;    //  co2                                
         }
         if(CO2>400)
         {
            holdingRegs[3] = CO2;    //  co2                             
         }
       
  delayMicroseconds(50);
        
}
