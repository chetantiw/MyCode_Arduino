/*
  Quick demo of major drawing operations on a single DMD
 */

#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>


// You can change to a smaller font (two lines) by commenting this line,
// and uncommenting the line after it:
//const uint8_t *FONT = Arial14;
//const uint8_t *FONT = Arial_Black_16;
const uint8_t *FONT = SystemFont5x7;
SoftDMD dmd(1,3);  // DMD controls the entire display

// the setup routine runs once when you press reset:
void setup() {
  dmd.setBrightness(250);
  dmd.selectFont(SystemFont5x7);
   // dmd.selectFont(FONT);
  dmd.begin();

 Serial.begin(9600);
  dmd.drawString(0,4,"MUTECH");
  delay(1000);
  dmd.clearScreen();
}

int n = 123;

// the loop routine runs over and over again forever:
void loop() {
 if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      char a=Serial.read();
    }
  }

  dmd.drawString(5,0,"CH:1");
  delay(1000);
  //dmd.drawString(0,8,"one");
//  delay(1000);
  dmd.drawString(0,11,"T:034");
  delay(1000);
  dmd.drawString(0,20,"H:045");
  delay(1000);
  dmd.drawString(0,29,"CO2:");
  delay(1000);
  dmd.drawString(8,38,"0560");
  delay(1000);
//  if(n % 2 == 0) {
//    dmd.drawFilledBox(0,11,4,15, GRAPHICS_OFF);
//    dmd.drawBox(0,11,4,15);
//  } else {
//    dmd.drawFilledBox(0,11,4,15);
//  }
}
