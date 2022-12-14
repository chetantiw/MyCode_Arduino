// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
#define relay 22
EnergyMonitor emon1;  
double current_sens = 0;// Create an instance



void setup()
{  
  Serial.begin(9600);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);
  emon1.current(32, 12);             // Current: input pin, calibration.
}

void loop()
{
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  current_sens = Irms;
 // Serial.print(Irms*230.0);	       // Apparent power
  //Serial.print(" ");
  Serial.println(current_sens);		       // Irms
}
