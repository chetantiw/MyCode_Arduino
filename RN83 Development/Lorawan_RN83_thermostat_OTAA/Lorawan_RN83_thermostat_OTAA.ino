
#include <TheThingsNetwork.h>
#include <TheThingsMessage.h>
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


SoftwareSerial ser1(2, 3); // RX, TX

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
//  const char *devAddr = "02017201";   //0004A30B00EEFA23
//  const char *nwkSKey = "70076e868fcee0f2a1b3887610e850b6";
//  const char *appSKey = "a6f734d42a163db7ea8728901d06fc29";


  // Set your AppEUI and AppKey 8df70b8ed0d13e05
const char *appEui = "8df70b8ed0d13e05";//"77c300f252cc1e4c";
 const char *appKey = "586a1bfb6c8fb96b3f5e3626fb1f25b6";//"29fc061d902887eab73d162ad434f7a6";

#define loraSerial ser1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

int relay1 = 4;
int relay2 = 5;
int relay3 = 6;
int relay4 = 7;
int relay5 = 8;

int set_temp,fan_speed,Mode,current_temp;
void send_packet();

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
  pinMode(relay5,OUTPUT);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  ttn.onMessage(message);

 // debugSerial.println("-- PERSONALIZE");
///  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey, -1, 10000, CLASS_C);

  set_temp = 25;
  fan_speed = 1;
  Mode = 1;
}

void loop()
{
  debugSerial.println("-- LOOP");

  int temp = analogRead(A0);
  current_temp = (temp/2);
  send_packet();
  Serial.println();
  debugSerial.print("set Temp: ");
  debugSerial.println(set_temp);
  debugSerial.print("fan Speed: ");
  debugSerial.println(fan_speed);
  debugSerial.print("Mode: ");
  debugSerial.println(Mode);

  if(current_temp>=set_temp)
  {
     digitalWrite(relay1,HIGH);  
  }
  else if(current_temp<=set_temp)
  {
    digitalWrite(relay1,LOW);
  }
  
  if(fan_speed==1)
  {
    digitalWrite(relay3,HIGH);
    digitalWrite(relay4,LOW);
    digitalWrite(relay5,LOW);
  }
  else if(fan_speed==2)
  {
    digitalWrite(relay3,LOW);
    digitalWrite(relay4,HIGH);
    digitalWrite(relay5,LOW);
  }
  else if(fan_speed==3)
  {
    digitalWrite(relay3,LOW);
    digitalWrite(relay4,LOW);
    digitalWrite(relay5,HIGH);
  }
  
  delay(30000);
}


void send_packet()
{
     
   int len = 71;
   byte payload[len];

   String load_string = "{\"set_temperature\":";
   load_string +=  String(set_temp).charAt(0);
   load_string +=  String(set_temp).charAt(1);
   load_string +=  ",\"current_temperature\":";
   load_string +=  String(current_temp).charAt(0);
   load_string +=  String(current_temp).charAt(1);
   load_string +=  ",\"Mode\":";
   load_string +=  String(Mode).charAt(0);
   load_string +=  ",\"Fan_Speed\":";
   load_string +=  String(fan_speed).charAt(0);
   load_string +=  "}";
   load_string.toCharArray(payload, len);
   ttn.sendBytes(payload, sizeof(payload));
   Serial.println(load_string);
}




void message(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println("-- MESSAGE");
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ":");

  for (int i = 0; i < size; i++)
  {
    debugSerial.write((payload[i]));
  }
  debugSerial.println();

  
 
}
