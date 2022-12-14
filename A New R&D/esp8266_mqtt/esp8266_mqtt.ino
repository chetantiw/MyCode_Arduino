

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



#define TOKEN "Ce4umkqOMIlrJjfMcwWd"

const char* ssid ="MuTech"; //replace this with your wifi  name
const char* password ="mpass@w0rd"; //replace with your wifi password
char thingsboardServer[] = "3.136.11.234";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

String message = "";



void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println("ESP8266 AS PUBLISHER");
  client.setServer( thingsboardServer, 1883 );

  lastSend = 0;
 
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

   if ( millis() - lastSend > 5000 ) { // Update and send only after 1 seconds

    
        getAndSendData();
        lastSend = millis();
    }
   
   
    
  

  delay(10);
  client.loop();
}  


void getAndSendData(){
 
 
  String payload ="{\"temperature\":\"";
  payload += 32;
  payload +="\"}";

  // Send connect
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "esp32/temperature", attributes );
  Serial.println( attributes );
  Serial.println( attributes );
  Serial.println("%. temp Sent to broker.");
  // Send payload
  delay(100);
  payload ="{\"humidity\":\"";
  payload += 88;
  payload +="\"}";

  payload.toCharArray( attributes, 100 );
  client.publish( "esp32/humidity", attributes );
  Serial.println( attributes );
  Serial.println("%. hum Sent to broker.");


}



void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   status = WiFi.status();
   if ( status != WL_CONNECTED) {
       WiFi.begin(ssid, password);
    // WiFi.begin("HiLink","uos@2020");
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
       }
    Serial.println("Connected to AP");
    }
    Serial.print("Connecting to server node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("a3477310-276d-11ec-b532-4301d810315e",TOKEN , NULL) ) {
      Serial.println( "[DONE]" );
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
