#include <String.h>
#include "EmonLib.h" 
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include<EEPROM.h>

#define TOKEN "iHZyAt0KjgpQTm470KIt" //vBQB5cOmaTGeCdquQzyF
#define relay 22
#define sw 33 
#define current_probe 32
#define blue_led 14
#define send_time 60000

int msg_delay_time = 10;
int running_time = 0;
int connected_flag=0;
int sent_flag = 0;


EnergyMonitor emon1;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

const unsigned long period = 1000;
float current_sens = 0;
int current_calib = 11; //p2
String sw_status;
double kWh = 0.0;


const char* ssid ="MuTech"; //replace this with your wifi  name
const char* password ="mpass@w0rd"; //replace with your wifi password
char thingsboardServer[] = "52.66.229.86";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

char buffer[64];

void setup() {
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);

  pinMode(sw,INPUT);
  digitalWrite(sw,HIGH);
  pinMode(blue_led, OUTPUT);
  digitalWrite(blue_led,HIGH);
  delay(1000);
  digitalWrite(blue_led,LOW);

  Serial.begin(9600);
  EEPROM.begin(512); 
  Serial.println("Light Control demo...");
  sw_status = EEPROM.readString(10);
  msg_delay_time = EEPROM.readString(20).toInt();
  
  Serial.println(sw_status);
  Serial.println(msg_delay_time);
  delay(4000);
  Serial.println("MODBUS GATEWAY");
  
      WiFi.begin(ssid, password);
      int counter= 0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        counter++;
        if(counter==40) // wait for 40 seconds
        {
          counter=0;
          break;       
        }
        // Set the Relay status even if net not connected        
      }
      Serial.println("Connected to WiFi");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      delay(4000);
      client.setServer(thingsboardServer, 1883 );
      client.setCallback(callback);
      delay(2000);
      Serial.print("Connecting to ThingsBoard node during Setup ...");
      while(!client.connected())
      {
        // Attempt to connect (clientId, username, password)
                          // db4a9990-8e5f-11ed-97d0-a3194889b196
        if ( client.connect("ec9b6460-9709-11ed-97d0-a3194889b196", TOKEN, NULL) )         
        {
          Serial.println( "[DONE]" );
          client.subscribe("v1/devices/me/attributes");
          connected_flag = 1;
        }
        else 
        {
          counter++; // use same counter to break this loop as well
          if(counter == 5) // Wait for 25 second
          {
            counter = 0;
            break;            
          }
          Serial.print( "[FAILED] [ rc = " );
          Serial.print( client.state() );
          Serial.println( " : retrying in 5 seconds]" );
          // Wait 5 seconds before retrying
          delay( 5000 );
        }
      }
      
      current_calib = 11;
      emon1.current(current_probe, current_calib);

if(sw_status=="On")
  {
  digitalWrite(relay,HIGH);
  String payload = "{\"relay\":";
    payload +=1; 
    payload += "}";
    //udpateAttributeToServer(payload);
  }
  else if(sw_status=="Off")
  {
  digitalWrite(relay,LOW);
  String payload = "{\"relay\":";
    payload +=0; 
    payload += "}";
    //udpateAttributeToServer(payload);
  }
        
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
    else
    {
      if(doc["relay"] == 1){
        Serial.println("switch on");
        digitalWrite(relay, HIGH);
        digitalWrite(blue_led,HIGH);
        sw_status = "On";      
      }
      else if(doc["relay"] == 0){
        Serial.println("switch off");
        digitalWrite(relay, LOW);
        digitalWrite(blue_led,LOW);
        sw_status = "Off";
       
      } 
      EEPROM.writeString(10,sw_status);
      EEPROM.commit();
      if(doc["msg_delay_time"] > 0)
      {
        msg_delay_time = doc["msg_delay_time"];
        EEPROM.writeString(20,String(msg_delay_time));
        EEPROM.commit();
          Serial.println("msg delay time updated");
          
      }
      
       
    }
   
  }
}
long lastMsg;
float irmsP;

void loop() {
 
 long start_t = millis();
 
 if(digitalRead(sw) == LOW)
 {
  if(sw_status == "Off")
  {
    digitalWrite(relay,HIGH);
    digitalWrite(blue_led,HIGH);  
    sw_status = "On";
    EEPROM.writeString(10,sw_status);
    EEPROM.commit(); 
    String payload = "{\"relay\":";
    payload +=0; 
    payload += "}";
    //udpateAttributeToServer(payload);
    
  }
  else
  {
    digitalWrite(relay,LOW);  
    digitalWrite(blue_led,LOW);
    sw_status = "Off";
    EEPROM.writeString(10,sw_status);
    EEPROM.commit(); 
    String payload = "{\"relay\":";
    payload +=1; 
    payload += "}";
    //udpateAttributeToServer(payload);
  }
  while(digitalRead(sw) == LOW);  
  delay(300);
 }
 
 if(runEvery(send_time)) {
    
    running_time++;
    if(running_time>=msg_delay_time)
    {
      sent_flag = 0;
      String payload = "{\"sw_status\":\"";
      payload +=sw_status; payload +="\",";
      payload +="\"power\":"; payload +=irmsP; payload +=",";
      payload +="\"instkWh\":"; payload +=kWh;
      payload +="}";
      logData(payload);
      if(sent_flag==1)
      {
        Serial.print("Data sent, resetting kWh to 0 ");
        kWh=0;
      }
      else
      {
        Serial.print("Data Not sent, Not resetting kWh");
      }
      
      running_time=0;
    }
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
  if(connected_flag==1)
  {
    Serial.println(telemetry);
    char attributes[66];
    telemetry.toCharArray(attributes,66);
    client.publish( "v1/devices/me/telemetry", attributes );
    Serial.println("%. Sent to Thingsboard.");
    sent_flag = 1;
  }
  else
  {
    Serial.println("Not connected to network while sending telemetry");  
  }

}
void udpateAttributeToServer(String att)
{
    if ( !client.connected() ) {
    reconnect();
  }
    // Send attribute in payload

  Serial.println(att);
  char attributes[15];
  att.toCharArray(attributes,15);
  client.publish("v1/devices/me/attributes", attributes );
  Serial.println("%. Attribute Sent to Thingsboard.");
}



void reconnect() {
 // Loop until we're reconnected
 int loop_ct = 0;
 connected_flag = 0;
 while (!client.connected()) {
   status = WiFi.status();
   if ( status != WL_CONNECTED) {
       WiFi.begin(ssid, password);
   
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
          loop_ct++;
          if(loop_ct>10)
          {
            Serial.println("Breaking Reconnect Loop");
            return;
          }
       }
    Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("15fee600-9404-11ed-97d0-a3194889b196", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      client.subscribe("v1/devices/me/attributes");
      connected_flag = 1;
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
