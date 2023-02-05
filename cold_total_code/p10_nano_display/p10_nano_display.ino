/*
  Quick demo of major drawing operations on a single DMD
 */

#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

SoftDMD dmd(1,3);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0, 32, 16);
// the setup routine runs once when you press reset:
void setup() {
  
  Serial.begin(9600);

   Serial.println("Dipslay receiver");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
  
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

String temperature,humidity,co2;
String recv_buf = "";
int timeout_ms=1000;

//int recv_str()
//{
//   recv_buf = "";
//   int startMillis = millis();
//   do
//    {
//        while (mySerial.available() > 0)
//        {
//         char   ch = mySerial.read();
//            recv_buf += ch;
//            Serial.print((char)ch);
//            delay(2);
//        }
// 
//        if (recv_buf.length()>5)
//            return 1;
// 
//    } while (millis() - startMillis < timeout_ms);  
//  return -1;
//}

// the loop routine runs over and over again forever:
void loop() 
{
   if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
       // display_para();    
 

}


void display_para()
{
  //Serial.println(recv_buf);
  temperature=24;
  humidity=45;
  co2=510;
  const char *MSG = "CH1";
  dmd.drawString(0,0,MSG);
  delay(100);
  MSG = "T:";
  dmd.drawString(0,10,MSG);
  MSG = temperature.c_str();
  dmd.drawString(10,10,MSG);
  MSG = "H:";
  dmd.drawString(0,20,MSG);
  MSG = humidity.c_str();
  dmd.drawString(10,20,MSG);
  MSG = "Co2:";
  dmd.drawString(0,30,MSG);
  MSG = co2.c_str();
  dmd.drawString(0,40,MSG);
  delay(2000);
//  dmd.drawString(5,0,"CH:1");
//  delay(1000);
//  dmd.drawString(0,11,"T:034");
//  delay(1000);
//  dmd.drawString(0,20,"H:045");
//  delay(1000);
//  dmd.drawString(0,29,"CO2:");
//  delay(1000);
//  dmd.drawString(0,38,"0560");
//  delay(1000);

//                 if(Serial.available())
//                 {
//                 ST = Serial.read();
//                 }
//                 
//                if(ST == 'T') 
//                  {
//                   dmd.drawString( 0, 0, "T=");
//                    delay(300);
//                    ST = 0;
//                  }
 }
