
int pwm_pin1 =4;
int pwm_pin2 =5;

void setup() 
   {

     Serial.begin(9600);
     pinMode(pwm_pin1, OUTPUT);
     pinMode(pwm_pin2, OUTPUT);
     digitalWrite(pwm_pin1,LOW);
     digitalWrite(pwm_pin2,LOW);
    }

// the loop function runs over and over again forever
void loop() 
    {
     int current = analogRead(A0);
     int intencity = analogRead(A1);
     Serial.print("C=");
     Serial.println(current);
     //Serial.print("I=");
     //Serial.println(intencity);
     //digitalWrite(pwm_pin1,LOW);
     //digitalWrite(pwm_pin2,LOW);
    // delay(3000);   
     digitalWrite(pwm_pin1,HIGH);
     digitalWrite(pwm_pin2,HIGH);
     delay(3000);   
        
  
    }
