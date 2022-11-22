//********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 14 
int mo_pin1 =3;
int mo_pin2 =5;
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
     digitalWrite(mo_pin1,LOW);
     digitalWrite(mo_pin2,LOW);
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensors.begin(); 
 
} 
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
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire
     // read the input on analog pin 0:
  int sensorValue = analogRead(A1);
  // print out the value you read:
   Serial.print("  current is: "); 
  Serial.println(sensorValue); 

     digitalWrite(mo_pin1,HIGH);
     digitalWrite(mo_pin2,HIGH);
   delay(1000); 

     digitalWrite(mo_pin1,LOW);
     digitalWrite(mo_pin2,LOW);
} 
