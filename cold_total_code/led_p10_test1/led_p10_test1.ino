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

  // Circle with a line at a tangent to it
//  dmd.drawCircle(24,8,5);
//  dmd.drawLine(14,9,28,15);
//
//  // Outline box containing a filled box
//  dmd.drawBox(6,10,11,15);
//  dmd.drawFilledBox(8,12,9,13);
  dmd.drawString(0,4,"MUTECH");
  delay(1000);
  dmd.clearScreen();
}

int n = 123;

// the loop routine runs over and over again forever:
void loop() {
//  dmd.drawString(0,0,String(n));
//  n = n + 1;
//  delay(1000);

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
