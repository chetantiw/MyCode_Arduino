#include <DMD32.h> 
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define RXD2 16
#define TXD2 17

// Reminder: the buffer size optimizations here, in particular the isrBufSize that only accommodates
// a single 8N1 word, are on the basis that any char written to the loopback SoftwareSerial adapter gets read
// before another write is performed. Block writes with a size greater than 1 would usually fail. 


//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 3
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

   //Timer setup
  //create a hardware timer  of ESP32
  hw_timer_t * timer = NULL;
char buffer[67];

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
  dmd.scanDisplayBySPI();
}


void setup() {
	Serial.begin(9600);
	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
	Serial.println("\LED Matrix started");

   // return the clock speed of the CPU
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  
  // Use 1st timer of 4 
  // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
  timer = timerBegin(0, cpuClock, true);
  // Attach triggerScan function to our timer 
  timerAttachInterrupt(timer, &triggerScan, true);
  // Set alarm to call triggerScan function  
  // Repeat the alarm (third parameter) 
  timerAlarmWrite(timer, 300, true);

  // Start an alarm 
  timerAlarmEnable(timer);

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   delay(2000);

  
 
	
}
  int i=0,len=0;

void loop() {


  while (Serial2.available()>67);
  {
    len = Serial2.readBytes(buffer,68);
  //  Serial.println(buffer);
    i++;
  }

 if(buffer[0]=='{')
 {
   Serial.print("string: ");
   String s = String(buffer); 
   Serial.println(s);

   dmd.clearScreen( true );
   dmd.selectFont(SystemFont5x7);
   // Français, Österreich, Magyarország
   const char *MSG = " Ch:";
   
   dmd.drawMarquee(MSG,strlen(MSG),0,0);
   dmd.drawChar(24,0,char(buffer[17]),GRAPHICS_NORMAL);
  
   MSG = "T:";
   dmd.drawMarquee(MSG,strlen(MSG),0,10);
   dmd.drawChar(10,10,char(buffer[34]),GRAPHICS_NORMAL);
   dmd.drawChar(16,10,char(buffer[35]),GRAPHICS_NORMAL);
   dmd.drawChar(21,10,char(buffer[36]),GRAPHICS_NORMAL);
   dmd.drawChar(26,10,char(buffer[37]),GRAPHICS_NORMAL);

    
   MSG = "H:";
   dmd.drawMarquee(MSG,strlen(MSG),0,20);
   dmd.drawChar(10,20,char(buffer[51]),GRAPHICS_NORMAL);
   dmd.drawChar(16,20,char(buffer[52]),GRAPHICS_NORMAL);
   dmd.drawChar(21,20,char(buffer[53]),GRAPHICS_NORMAL);
   dmd.drawChar(26,20,char(buffer[54]),GRAPHICS_NORMAL);

   MSG = "CO2:";
   dmd.drawMarquee(MSG,strlen(MSG),0,30);
   dmd.drawChar(0,40,char(buffer[63]),GRAPHICS_NORMAL);
   dmd.drawChar(6,40,char(buffer[64]),GRAPHICS_NORMAL);
   dmd.drawChar(12,40,char(buffer[65]),GRAPHICS_NORMAL);
   if((buffer[66])!='}'){
   dmd.drawChar(18,40,char(buffer[66]),GRAPHICS_NORMAL);
   }
   
   
   
   Serial2.flush();
  }
  //long start=millis();
  //long timer=start;
}


boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
