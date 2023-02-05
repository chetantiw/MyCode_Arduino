#include <String.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define TOKEN "vBQB5cOmaTGeCdquQzyF"

const char* ssid ="khaleej"; //replace this with your wifi  name
const char* password ="Glaniv@103"; //replace with your wifi password
char thingsboardServer[] = "52.66.229.86";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

char buffer[64];

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/



void setup() {
  Serial.begin(9600);
//  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Light Control demo...");
  
  WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to WiFi");
      Serial.println("MODBUS GATEWAY");
      client.setServer(thingsboardServer, 1883 );
      client.setCallback(callback);
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
    Serial.print( messageTemp);
   /* if(relay == 1){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(relay == 0){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }  */
  }
}
long lastMsg;

void loop() {

client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    int kWh = 2345;
    // String topic = "{\"device\":";
    String payload = "{\"Status\":";
    payload +=1; payload +=",";
    payload +="\"kWh\":"; payload +=kWh;
    payload +="}";
    logData(payload);
    
  }
  

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
