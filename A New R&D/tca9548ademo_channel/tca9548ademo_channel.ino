/*

Example: tca9548ademo

Arduino library for Arduino library for Texas Instruments TCA9548A 8-Channel I2C Switch/Multiplexer
version 2019.3.23

---

Copyright (c) 2019, ClosedCube
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation and/or 
   other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <Wire.h>
#include "ClosedCube_TCA9548A.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


Adafruit_MPU6050 mpu;

#define UART_BAUD 115200
#define TCA9548A_I2C_ADDRESS	0x70

ClosedCube::Wired::TCA9548A tca9548a;

void setup()
{
    Wire.begin();
    Serial.begin(UART_BAUD);
    
    Serial.println("ClosedCube TCA9548A Channel Scanner Demo");
 Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
 
    tca9548a.address(TCA9548A_I2C_ADDRESS);

	uint8_t returnCode = 0;
	uint8_t address;
	uint8_t numberOfDevices;

    for( uint8_t channel=0; channel<TCA9548A_MAX_CHANNELS; channel++ ) {
    	Serial.print("Scanning channel #");
    	Serial.print(channel);
    	Serial.println("...");

		returnCode = tca9548a.selectChannel(channel);
		numberOfDevices = 0;

    	if( returnCode == 0 ) {
    		for(address = 0x01; address < 0x7F; address++ ) {
    			Wire.beginTransmission(address);
    			returnCode = Wire.endTransmission();
 
    			if (returnCode == 0) {
      				Serial.print("I2C device = ");
					printAddress(address);
          numberOfDevices++;


          
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);

  
    			} else if ( returnCode == 4) {
      				Serial.print("Unknown error at ");
      				printAddress(address);
    			}    
			  }
			  if (numberOfDevices == 0)
			    Serial.println("No I2C devices found\n");
    	} else {
    		Serial.print("Error scan channel (Code:");
    		Serial.print(returnCode);
    		Serial.println(")");
    	}
    }

}

void printAddress(uint8_t address)  {
	Serial.print("0x");
	if (address<0x10) {
		Serial.print("0");
	}
	Serial.println(address,HEX);
}

void loop()
{
	// do nothing here
}
