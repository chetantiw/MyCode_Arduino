#include <String.h>
#include "EmonLib.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include<EEPROM.h>

#define TOKEN "vBQB5cOmaTGeCdquQzyF"
#define relay 22
#define sw 33
#define current_probe 32
#define send_time 5000


EnergyMonitor emon1;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

const unsigned long period = 1000;
float current_sens = 0;
int current_calib = 11; //p2
int sw_status;
double kWh = 0.0;


const char* ssid ="Beware"; //replace this with your wifi  name
const char* password ="#ritesh2010"; //replace with your wifi password
char thingsboardServer[] = "52.66.229.86";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

char buffer[64];

void setup() {
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);

  pinMode(sw,INPUT);
  digitalWrite(sw,HIGH);
 
  Serial.begin(9600);
//  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Light Control demo...");
  sw_status = EEPROM.read(0x01);
  delay(200);
  if(sw_status==1)
  digitalWrite(relay,HIGH);
  else if(sw_status==0)
  digitalWrite(relay,LOW);
 
  WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to WiFi");
      Serial.println("MODBUS GATEWAY");
      client.setServer(thingsboardServer, 1883 );
      client.setCallback(callback);
      current_calib = 11;
      emon1.current(current_probe, current_calib);


   delay(2000);

 
 
 
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "v1/devices/me/attributes"){
    Serial.print("Received: ");
    Serial.println( messageTemp);

    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, messageTemp);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
   
    if(doc["relay"] == 1){
      Serial.println("switch on");
      digitalWrite(relay, HIGH);
      sw_status = 1;      
    }
    else if(doc["relay"] == 0){
      Serial.println("switch off");
      digitalWrite(relay, LOW);
      sw_status = 0;
     
    }
    EEPROM.write(0x01,sw_status);
    EEPROM.commit();
   
  }
}
long lastMsg;
float irmsP;

void loop() {
 
 long start_t = millis();
 
 
 if(digitalRead(sw) == LOW)
 {
  if(sw_status == 0)
  {
  digitalWrite(relay,HIGH);  
  sw_status = 1;
  }
  else
  {
    digitalWrite(relay,LOW);  
    sw_status = 0;
  }
  while(digitalRead(sw) == LOW);  
  delay(300);
 }
 
 if(runEvery(send_time)) {
    //send_flag = 1;
    String payload = "{\"sw_status\":";
    payload +=sw_status; payload +=",";
    payload +="\"power\":"; payload +=irmsP; payload +=",";
    payload +="\"instkWh\":"; payload +=kWh;
    payload +="}";
    logData(payload);
    kWh=0;
  }
 
  double Irms = emon1.calcIrms(1000);
  if(Irms<0.25)
  {
    current_sens = 0;
  }
  else
  {
  current_sens = Irms;
  }
  irmsP = current_sens*225;
 
  long end_t = millis();
  int  diff_t = end_t - start_t;
  float Wh = (irmsP*diff_t)/3600000;
  kWh =  kWh+Wh/1000;
 
 
//  Serial.print("for loop time: ");
//  Serial.println(diff_t);
//  Serial.print("kWh: ");
//  Serial.println(kWh);
//  Serial.print("Current:");// Irms
//  Serial.println(current_sens);
 
client.loop();

 
 

}


void logData(String telemetry){

  if ( !client.connected() ) {
    reconnect();
  }
    // Send payload

  Serial.println(telemetry);
  char attributes[66];
  telemetry.toCharArray(attributes,66);
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println("%. Sent to Thingsboard.");

}



void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   status = WiFi.status();
   if ( status != WL_CONNECTED) {
       WiFi.begin(ssid, password);
   
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
       }
    Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("db4a9990-8e5f-11ed-97d0-a3194889b196", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      client.subscribe("v1/devices/me/attributes");
    }
    else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
 }
}



boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
