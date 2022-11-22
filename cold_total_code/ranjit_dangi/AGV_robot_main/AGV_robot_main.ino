#include <SPI.h>  
#include "RF24.h" 
#include "nRF24L01.h"
RF24 myRadio (48,49); //9,10 /48,49 
struct package
{
  int id=0;
  int nodeID = 25;
  char  text[6] ="ys";
};

byte addresses[][6] = {"0001"}; 


int pwm_pin = 7; 
int dir_pin = 6;
int tsp_pin = 3;
int led_pin = 2;


typedef struct package Package;
Package data;

void setup() 
{
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  pinMode(tsp_pin, INPUT);
  pinMode(pwm_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
  digitalWrite(tsp_pin, HIGH);
  digitalWrite(dir_pin, HIGH);
  analogWrite(pwm_pin, 230);
  delay(100);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_2MBPS ) ; 
  myRadio.openWritingPipe( addresses[0]);
  delay(1500);
}


void loop()  
{
  myRadio.startListening();
  
  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read( &data, sizeof(data) );
    }
    Serial.print("\nPackage:");
    Serial.print(data.id);
    Serial.print("\n");
    Serial.println(data.nodeID);
    Serial.println(data.text);

   // delay(100);
   
     myRadio.stopListening();
     delay(100);
         if(data.text[0]=='c')
         {
          if(data.text[1]=='m')
           { 
              
             //  data.nodeID = 1;
                 data.text[0] = 'y';
                 data.text[1] = 's';
                 myRadio.write(&data, sizeof(data)); 
                 data.id = data.id + 1;
                 delay(200);
                 data.text[0] = 'y';
                 data.text[1] = 's';
                 myRadio.write(&data, sizeof(data)); 
                 data.id = data.id + 1;
                 delay(100);
                 digitalWrite(dir_pin, HIGH);
                 Serial.println("forward");
                 delay(1000);
                 analogWrite(pwm_pin, 100);//100
                 Serial.println("speed 100f");

                 while(1)
                 {
                    if(digitalRead(tsp_pin)==LOW)
                    {
                       digitalWrite(led_pin,LOW);
                       analogWrite(pwm_pin, 200);//100
                       Serial.println("speed 200f");
                       while(digitalRead(tsp_pin)==LOW);
                       delay(100);
                       digitalWrite(led_pin,HIGH);
                    }

                    if(digitalRead(tsp_pin)==LOW)
                    {
                       digitalWrite(led_pin,LOW);
                       analogWrite(pwm_pin, 230);//100
                       Serial.println("Stopped");
                       delay(100);
                    }
                 }
                 

                 
                 Serial.println("feedback sent"); 
      
           }
        }
    
    
  }

  delay(2000);
  

}
