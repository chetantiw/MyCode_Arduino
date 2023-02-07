

uint8_t interruptPin1 = PB3;

int pulse_count = 0;

void blink1()
{
  pulse_count++;

}



void setup()
{
  // initialize serial communcation at 115200 bits per second
  Serial.begin(115200);

  Serial.println("RAKwireless Arduino Interrupt Example");
  Serial.println("------------------------------------------------------");
  
  pinMode(interruptPin1, INPUT_PULLUP);
  attachInterrupt(interruptPin1, blink1, FALLING);
  
}

void loop()
{
  Serial.print("Volume: ");
  Serial.print(pulse_count);
  Serial.println(" Litres");
  delay(1000);
}
