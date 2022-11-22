
int pwm_pin1 =3;
int pwm_pin2 =5;

void setup() 
   {
     digitalWrite(pwm_pin1,HIGH);
     digitalWrite(pwm_pin2,HIGH);
    
     pinMode(pwm_pin1, OUTPUT);
     pinMode(pwm_pin2, OUTPUT);
     digitalWrite(pwm_pin1,HIGH);
     digitalWrite(pwm_pin2,HIGH);
    }

// the loop function runs over and over again forever
void loop() 
    {
         analogWrite(pwm_pin1,0);
         delay(4000);
       
         analogWrite(pwm_pin1,40);
         delay(4000);

         analogWrite(pwm_pin1,80);
         delay(4000);
    
        
  
    }
