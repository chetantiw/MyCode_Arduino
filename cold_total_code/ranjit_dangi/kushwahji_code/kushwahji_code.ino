 
int pwm_pin1 =4;
int pwm_pin2 =5;
int stus_pin =13;
int t=0;
void setup() 
   {

    
     pinMode(pwm_pin1, OUTPUT);
     pinMode(pwm_pin2, OUTPUT);
     pinMode(stus_pin, INPUT);
     
     digitalWrite(pwm_pin1,LOW);
     digitalWrite(pwm_pin2,LOW);
     digitalWrite(stus_pin,HIGH);
  
    }

// the loop function runs over and over again forever
void loop() 
    {

         t=digitalRead(stus_pin);
         if(t==LOW)
         {
                 
         digitalWrite(pwm_pin1,LOW);
         digitalWrite(pwm_pin2,LOW);
         delay(600000 );
         digitalWrite(pwm_pin1,HIGH);
         digitalWrite(pwm_pin2,HIGH);
         delay(60000);
         }
         else
            {
             digitalWrite(pwm_pin1,LOW);
             digitalWrite(pwm_pin2,LOW);
             delay(1000);
            }
        
  
    }
