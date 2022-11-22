
#define motor_pin            7
#define direction_pin        6
#define tsop_pin             3
#define led_pin              2
int i = 0;

void setup() {
              Serial.begin(9600);
              Serial.println("AGV TESTING");
              
              pinMode(motor_pin, OUTPUT);
              pinMode(direction_pin, OUTPUT);
              pinMode(led_pin, OUTPUT);
              pinMode(tsop_pin, INPUT);
              
              digitalWrite(motor_pin, LOW);
              digitalWrite(direction_pin, LOW);
              digitalWrite(led_pin, HIGH);
              digitalWrite(tsop_pin, HIGH);
              delay(1000);
             }

void loop() {
              analogWrite(motor_pin, 0);
              digitalWrite(direction_pin, LOW);
              delay(3000);
              analogWrite(motor_pin, 50);
              digitalWrite(direction_pin, LOW);
              delay(3000);
              analogWrite(motor_pin, 100);
              digitalWrite(direction_pin, LOW);
              delay(3000);
              analogWrite(motor_pin, 150);
              digitalWrite(direction_pin, LOW);
              delay(3000);
              analogWrite(motor_pin, 200);
              digitalWrite(direction_pin, LOW);
              delay(3000);
              
//              digitalWrite(motor_pin, LOW);
//              digitalWrite(direction_pin, HIGH);
//              delay(1000);
//              digitalWrite(motor_pin, 100);
//              digitalWrite(direction_pin, HIGH);
//              delay(2000);
//              digitalWrite(motor_pin, 175);
//              digitalWrite(direction_pin, HIGH);
//              delay(2000);
//              digitalWrite(motor_pin, 250);
//              digitalWrite(direction_pin, HIGH);
//              delay(2000);

            }
