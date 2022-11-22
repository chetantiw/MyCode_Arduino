/*--------------------------------------------------------------------------------------

 dmd_latin_chars

 This LIibrary (DMD32) and example are  fork of original DMD library  was modified to work on ESP32 
Modified by: Khudhur Alfarhan  // Qudoren@gmail.com
1/Oct./2020


   Demo and example Latin-1 encoding project for the Freetronics DMD, a 512 LED matrix display
   panel arranged in a 32 x 16 layout.

See http://www.freetronics.com/dmd for resources and a getting started guide.

This example code is in the public domain. It demonstrates using the ISO-8859-1
(Latin-1) extended character set. Thanks to Xavier Seignard for contributing
Latin-1 support.

*******************************
HOW TO ENTER Latin-1 CHARACTERS
*******************************

Unfortunately entering Latin-1 characters like Ã or è is not as simple as just
typing them. Arduino Sketches are saved in Unicode UTF-8 format, but the DMD library
does not understand Unicode (it's too complex.)

To enter the characters as Latin-1, look at the codepage layout here and look for
the hexadecimal digit that corresponds to the character you want.
https://en.wikipedia.org/wiki/ISO/IEC_8859-1#Codepage_layout

For example, á (lowercase a, rising diacritical mark) has hex value 00E0 in Latin-1.
To translate this into a constant string, replace the leading 00 with \x - so the
string could be "The Portugese for rapid is r\xE0pido".

To be safe, the string may also need to be separated in its own quote marks - ie
"The Portugese for rapid is r""\xE0""pido"

When you compile the sketch, the compiler will join all these strings up into one
long string - however without the quotes around the \x it may try to include additional
characters as part of the hexadecimal sequence.

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/

#include <DMD32.h> 
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define SSerialTxControl 26      
#define RS485Transmit    HIGH
#define RS485Receive     LOW

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 3
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

   //Timer setup
  //create a hardware timer  of ESP32
  hw_timer_t * timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
  dmd.scanDisplayBySPI();
}


byte buffer[15];
String temperature,humidity,co2;
unsigned char request[] = {0x01,0x03,0x00,0x01,0x00,0x05,0x20,0x0B};


/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
    // initialize serial ports
      Serial.begin(9600);
      Serial2.begin(9600);
      pinMode(SSerialTxControl,OUTPUT);
      Serial.println("Temperature & humidity & co2 Gateway demo......");
      
     // digitalWrite(dere,HIGH); //Enable high, RS485 shield waiting to transmit data
    //  Serial2.write(command,n); //Test send command Auto,Read to RS485
      delay(500);


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
    dmd.clearScreen( true );
   dmd.selectFont(SystemFont5x7);
   // Français, Österreich, Magyarország
   const char *MSG = " Ch:1";
   dmd.drawMarquee(MSG,strlen(MSG),0,0);
   MSG = "T: 32";
   dmd.drawMarquee(MSG,strlen(MSG),0,10);

   MSG = "H: 54";
   dmd.drawMarquee(MSG,strlen(MSG),0,20);

   MSG = "CO2:";
   dmd.drawMarquee(MSG,strlen(MSG),0,30);
   MSG = "543";
   dmd.drawMarquee(MSG,strlen(MSG),0,40);
   delay(2000);
//   long start=millis();
//   long timer=start;
  // while(1){

  // }
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   
   for( char lp=1;lp<5;lp++)
 {
    Serial.print("Reading sensor: ");
    Serial.write(lp);
    Serial.println();
    int r = read_sensor(lp); 
    if(r)
    {
      Serial.print("Temp: ");
      Serial.print(temperature);
      Serial.print(" Celcius,  "); 
      Serial.print("Hum: ");
      Serial.print(humidity);
      Serial.print("%  ");
      Serial.print("Co2: ");
      Serial.print(co2);
      Serial.println("ppm");


    // display_sensor(String(lp));
   

      


      
    } 
    else
    {
      Serial.println("sensor not available at this address..");
    }
  delay(2000);
 }
   
   
  delay(5000); 
   
   
  
}


int read_sensor(unsigned char b)
{
   unsigned int res=0x0000;
   byte t = 0x00;
   int ones,tens,hunds,thous;
   
     request[0] = b;
     res = CRC16_2(request,6);
     t = res & 0x00ff;
     request[6] = t;
     t = (res & 0xff00)>>8;
     request[7] = t;
  
   // Serial.print("Tx: ");
   // for(int i=0;i<8;i++)
   // {
   //    Serial.print(request[i], HEX);
       //delay(100);
   // }
  //  Serial.println();

   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   Serial2.write(request, sizeof(request));
      
   Serial2.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
    
    while( Serial2.available()<15)
    {
        ct++;
        if(ct==1500)
        {
          break;
        }
        delay(1);
     }
     if(ct==1500)
     {
       return(0);
     }
   int recvLen = Serial2.readBytes(buffer,16);
   
   Serial.print("Rx: ");
    
  // Serial.print(buffer[3], HEX); Serial.print(" ");
 //  Serial.print(buffer[4], HEX); Serial.print(" ");
  // Serial.print(buffer[5], HEX); Serial.print(" ");
  // Serial.print(buffer[6], HEX); Serial.print(" "); 
  // Serial.print(buffer[7], HEX); Serial.print(" ");
 //  Serial.print(buffer[8], HEX); Serial.print(" ");
 //  Serial.println();
   
  // Convert to Float
  int x;
  temperature="";
  humidity="";
  co2="";
  if(recvLen>0)
  {
    x = buffer[3]<<8;
    x = x + (buffer[4]);
   

    ones = x %10;
    x = (x / 10);
    tens = x % 10;
    hunds = x / 10;

    
    temperature = hunds;
    temperature += tens;
   // temperature += '.';
    temperature += ones;
    
    x = buffer[5]<<8;
    x = x + (buffer[6]);
     ones = x %10;
    x = (x / 10);
    tens = x % 10;
    hunds = x / 10;
    
    humidity = hunds;
    humidity += tens;
    //humidity += '.';
    humidity += ones;
    
    x = buffer[11]<<8;
    x = x + (buffer[12]);
    
    ones = x %10;
    x = (x / 10);
    tens = x % 10;
    x = (x / 10);
    hunds = x % 10;
    thous = x / 10;

    co2  = thous;
    co2 += hunds;
    co2 += tens;
    //co2 += '.';
    co2 += ones;
    // ((byte*)&x)[1]= buffer[3];
    // ((byte*)&x)[0]= buffer[4];
    // ((byte*)&x)[1]= buffer[5];
   //  ((byte*)&x)[0]= buffer[6];
   return(1);
  }
  else
  {
    return(-1); 
  } 
 
}

unsigned int CRC16_2(byte *buf, int len)
{  
//  for (int i=0;i<8;i++)
//  {
//     Serial.print(buf[i],HEX);
//  }
 // Serial.println("");
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}
