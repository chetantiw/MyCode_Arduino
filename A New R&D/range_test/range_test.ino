
byte buffer[4] = {0x7f,0x00,0x7F,0xff};
long kwh;
void setup() {
  // put your setup code here, to run once:

   Serial.begin(9600);
   
          ((byte*)&kwh)[3]= buffer[0];
         ((byte*)&kwh)[2]= buffer[1];
         ((byte*)&kwh)[1]= buffer[2];
         ((byte*)&kwh)[0]= buffer[3];

     Serial.println("kWh: ");
     Serial.println(String(kwh));
          

   
}

void loop() {
  // put your main code here, to run repeatedly:

}
