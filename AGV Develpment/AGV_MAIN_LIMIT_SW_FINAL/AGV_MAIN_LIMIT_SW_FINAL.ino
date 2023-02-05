#include <SPI.h>  
#include <LoRa.h>


int pwm_pin = 7;
int dir_pin = 6;
int tsp_pin1 = 3;
int tsp_pin2 = 5;
int led_pin1 = 2;
int led_pin2 = 4;

//int front_obst_l = 37;  //
int front_obst_r = 39;
//int back_obst_l = 38;
int back_obst_r = 36;  //


int sw1 = 48;
int sw2 = 49;

int stop_counter = 0;
int limit;

#define dist_read 36 // attach pin D2 Arduino to pin Echo of JSN-SR04T

void forward();
void revrse();




void setup()
{
  Serial.begin(115200);
  pinMode(led_pin1, OUTPUT);
  pinMode(tsp_pin1, INPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(tsp_pin2, INPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(pwm_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
 // pinMode(front_obst_l,INPUT);
  pinMode(front_obst_r,INPUT);
 // pinMode(back_obst_l,INPUT);
  pinMode(back_obst_r,INPUT);
 
 // digitalWrite(limit_sw, HIGH);
  digitalWrite(led_pin1, HIGH);
  digitalWrite(tsp_pin1, HIGH);
  digitalWrite(led_pin2, HIGH);
  digitalWrite(tsp_pin2, HIGH);
  digitalWrite(back_obst_r, HIGH);
  digitalWrite(front_obst_r, HIGH);
  digitalWrite(dir_pin, HIGH);
  digitalWrite(sw1, HIGH);
  digitalWrite(sw2, HIGH);
  analogWrite(pwm_pin, 230);
  
  Serial.println("Welcome to AGV demo");
  Serial.println();
  stop_counter = 0;
  limit = 1;
  delay(1500);
}

int sens_flag = 0;
void loop()  
{
  
  if(digitalRead(sw1)==LOW && stop_counter<limit)
  {
       forward();
       delay(4000);
       while(1)
       {
             if(digitalRead(front_obst_r)==LOW)
             {
                  Serial.println("front obstcle");
                  digitalWrite(led_pin1,LOW);
                  analogWrite(pwm_pin, 230);//100
                  Serial.println("stopped");
                  digitalWrite(led_pin1,HIGH);
                  sens_flag = 1;
                  while(digitalRead(front_obst_r)==LOW);
             }
             else 
             {
                if(sens_flag==1)
                {
                   forward();
                   sens_flag = 0;
                }
             }
             if(digitalRead(tsp_pin1)==LOW)
             {
                 
                
                  digitalWrite(led_pin1,LOW);
                  analogWrite(pwm_pin, 180);//100
                  Serial.println("speed 180f");
                  while(digitalRead(tsp_pin1)==LOW);
                  delay(100);
                  digitalWrite(led_pin1,HIGH);
                  delay(1000);
                
                
             }
             else if(digitalRead(tsp_pin2)==LOW )
             {
                 digitalWrite(led_pin2,LOW);
                 analogWrite(pwm_pin, 230);//100
                 Serial.println("stopped");
                 digitalWrite(led_pin2,HIGH);
                 stop_counter = 1;
                 break;
             }
                             
        
       }  
  } 
  if(digitalRead(sw2)==LOW && stop_counter>0)
  {
    revrse();
    delay(2500);
    while(1)
    {
        if( digitalRead(back_obst_r)==LOW)
             {
                Serial.println("back obstacle");

                 digitalWrite(led_pin1,LOW);
                  analogWrite(pwm_pin, 230);//100
                  Serial.println("stopped");
                  digitalWrite(led_pin1,HIGH);
                  sens_flag = 1;
                  while( digitalRead(back_obst_r)==LOW);
             }
             else
             {
                if(sens_flag==1)
                {
                  revrse();
                  sens_flag = 0;
                }
             }
        if(digitalRead(tsp_pin2)==LOW)
        {
          
          digitalWrite(led_pin2,LOW);
          analogWrite(pwm_pin, 180);//100
          Serial.println("speed 180f");
          while(digitalRead(tsp_pin2)==LOW);
          delay(100);
          digitalWrite(led_pin2,HIGH);
          delay(1000);
          
        
        }
        if(digitalRead(tsp_pin1)==LOW )
        {
          digitalWrite(led_pin1,LOW);
          analogWrite(pwm_pin, 230);//100
          Serial.println("stopped");
          digitalWrite(led_pin1,HIGH);
          stop_counter=0;
          break;
       }              
    }
                 
  }
                 
  

}


void forward()
{
  digitalWrite(dir_pin, HIGH);
  Serial.println("forward");
  delay(800);
  analogWrite(pwm_pin, 200);//100
  Serial.println("speed 200f");
  delay(500);
 // analogWrite(pwm_pin, 180);//100
 // Serial.println("speed 180f");
 // delay(1000);
  analogWrite(pwm_pin, 160);//100
  Serial.println("speed 160f");
//  delay(1000);
 // analogWrite(pwm_pin, 100);//100
 // Serial.println("speed 100f");
  


  
}

void revrse()
{
  digitalWrite(dir_pin, LOW);
  Serial.println("backward");
  delay(800);
 // analogWrite(pwm_pin, 180);//100
//  Serial.println("speed 180f");
//  delay(1000);
  analogWrite(pwm_pin, 160);//100
  Serial.println("speed 160f");
//  delay(1000);
//  analogWrite(pwm_pin, 140);//100
//  Serial.println("speed 140f");
//  delay(1000);
//  analogWrite(pwm_pin, 130);//100
//  Serial.println("speed 130f");
//  delay(1000);
//  analogWrite(pwm_pin, 100);//100
//  Serial.println("speed 100f");
  
}
