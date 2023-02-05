/*
  Quick demo of major drawing operations on a single DMD
 */

#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>


SoftDMD dmd(1,3);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0, 32, 16);
// the setup routine runs once when you press reset:
void setup() {
  
  Serial.begin(9600);
  dmd.setBrightness(250);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  delay(10); 
  dmd.drawString(0,0,"MU...");
  delay(100);
  dmd.drawString(0,10,"TECH");
  delay(100);
  dmd.drawString(0,20,"T-H-C");
  delay(100);
  dmd.drawString(0,30,"MONI.");
  delay(4000);
  dmd.clearScreen();
  delay(300);
}

String temperature,humidity,co2,ST;



// the loop routine runs over and over again forever:
void loop() {


             display_para();

            }


void display_para()
{                
      const char *MSG = "CH1";
      dmd.drawString(0,0,MSG);
      delay(100);            
               while(Serial.available())
                 {
                 ST = char(Serial.read());
                
                 }

  MSG = "T:";
  dmd.drawString(0,10,MSG);
  MSG = ST.c_str();
  dmd.drawString(10,10,MSG);
  delay(2000);



                 

 }
