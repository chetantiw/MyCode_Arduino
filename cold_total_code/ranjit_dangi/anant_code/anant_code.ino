 
int pwm_pin1 =4;
int pwm_pin2 =5;
int pwm_pin3 =6;
int pwm_pin4 =7;
void setup() 
   {

    
     pinMode(pwm_pin1, OUTPUT);
     pinMode(pwm_pin2, OUTPUT);
     pinMode(pwm_pin3, OUTPUT);
     pinMode(pwm_pin4, OUTPUT);
     
     digitalWrite(pwm_pin1,LOW);
     digitalWrite(pwm_pin2,LOW);
     digitalWrite(pwm_pin3,LOW);
     digitalWrite(pwm_pin4,LOW);
  
    }

// the loop function runs over and over again forever
void loop() 
    {
         digitalWrite(pwm_pin1,HIGH);
         digitalWrite(pwm_pin2,HIGH);
         digitalWrite(pwm_pin3,HIGH);
         digitalWrite(pwm_pin4,HIGH);
         delay(2000);
       
         digitalWrite(pwm_pin1,LOW);
         digitalWrite(pwm_pin2,LOW);
         digitalWrite(pwm_pin3,LOW);
         digitalWrite(pwm_pin4,LOW);
         delay(1000);

    
        
  
    }
