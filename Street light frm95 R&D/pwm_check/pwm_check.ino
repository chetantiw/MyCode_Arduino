
int lamp1 =5;
int lamp2 =3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(lamp1,OUTPUT);
  digitalWrite(lamp1,HIGH);
    pinMode(lamp2,OUTPUT);
  digitalWrite(lamp2,HIGH);
  Serial.println("PWM demo...");

}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(lamp1,0);
  analogWrite(lamp2,0);
  Serial.println("PWM: 0");
  delay(3000);
  analogWrite(lamp1,100);
  analogWrite(lamp2,100);
   Serial.println("PWM: 100");
  delay(3000);
    analogWrite(lamp1,150);
  analogWrite(lamp2,150);
   Serial.println("PWM: 150");
  delay(3000);
     analogWrite(lamp1,200);
  analogWrite(lamp2,200);
   Serial.println("PWM: 200");
  delay(3000);
      analogWrite(lamp1,225);
  analogWrite(lamp2,225);
   Serial.println("PWM: 225");
  delay(3000);
     analogWrite(lamp1,255);
  analogWrite(lamp2,255);
   Serial.println("PWM: 255");
  delay(3000);
 
 
}
