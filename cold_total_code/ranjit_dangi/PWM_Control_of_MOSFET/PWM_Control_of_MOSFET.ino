#define Up 2                                             // Up button on pin 2
#define Down 3                                           // Down button on pin 3
int stateUp;                                             // Variable for state of Up button
int stateDown;                                           // Variable for state of Down button

void setup() {
 
 pinMode(Up, INPUT_PULLUP);                              // Apply internal pull up for Up button
 pinMode(Down, INPUT_PULLUP);                            // Apply internal pull up for Down button
 
  DDRB |= (1 << DDB1) | (1 << DDB2);                     // Set ports
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Fast PWM mode    
  TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10);    // Fast PWM mode, no clock prescaling possible
  OCR1A = 3240;                                          // Start PWM just below MOSFET turn on
  ICR1 = 8191;                                           // Set the number of PWM steps
Serial.begin(9600);                                      // Start communication with serial monitor
}

void loop() {
 stateUp = digitalRead(Up);                              // Read Up button state
 if(stateUp == LOW) {                                    // If pressed...
 OCR1A++;                                                // Increase PWM setting
 }
 stateDown = digitalRead(Down);                          // Read Down button state
 if(stateDown == LOW) {                                  // If pressed...
 OCR1A--;                                                // Decrease PWM setting
 }
 
 delay(100);
 Serial.println(OCR1A);                                  // Print current PWM setting on serial monitor

}
