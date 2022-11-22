

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define TOKEN "Fvm8a9MEtkFNTPDF7hc5"

const char* ssid ="MuTech"; //replace this with your wifi  name
const char* password ="mpass@w0rd"; //replace with your wifi password
char thingsboardServer[] = "52.66.229.86";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const long frequency = 433;  // LoRa Frequency

const int csPin = D8;          // LoRa radio chip select
const int resetPin = D1;        // LoRa radio reset
const int irqPin = D2;          // change for your board; must be a hardware interrupt pin
int i;
void LoRa_txMode();
void LoRa_rxMode();
boolean runEvery(unsigned long );
void LoRa_sendMessage(String );
bool recv_flag;

String message = "";

void onReceive(int packetSize) {
 
  message="";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  message.replace('#',' ');
  Serial.print("Gateway Receive: ");
  Serial.println(message);
  recv_flag=true;
  ESP.wdtEnable(0);
 
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

int reset_ct=0;

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  // WiFi.begin("HiLink","uos@2020");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println("ESP8266 AS PUBLISHER");
  client.setServer( thingsboardServer, 1883 );

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println("Tx: invertIQ enable");
  Serial.println("Rx: invertIQ disable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
 
  LoRa_rxMode();
  i=1;
 
  lastSend = 0;
 
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

   if ( millis() - lastSend > 30000 ) { // Update and send only after 1 seconds

     Serial.print("Reading ID: ");
    Serial.println(i);
   
    if(i==1)
    {
      LoRa_sendMessage("ENV0001"); // send a message
    }
    else if(i==2)
    {
      LoRa_sendMessage("ENV0002"); // send a message
    }
    else if(i==3)
    {
      LoRa_sendMessage("ENV0003"); // send a message
    }
    else if(i==4)
    {
      LoRa_sendMessage("ENV0004"); // send a message
    }
    
   
    delay(2000);  
    if(recv_flag==true)
    {
        getAndSendData();
       
        message = "";
        recv_flag=false;
    }
   
    i++;
    if(i==4)
    { i=1;

    }
   
    lastSend = millis();
  }

  delay(10);
  client.loop();
}  



void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
  delay(10);
 
 
}

void getAndSendData(){
 
  //payload formation begins here
  DynamicJsonBuffer jsonBuffer(1024);
  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
  }
  String id = root["meterID"];
  String payload ="{\"device\":\"";
  payload += id;
  payload +="\"}";

  // Send connect
  char attributes[512];
  payload.toCharArray( attributes, 512 );
  client.publish( "v1/gateway/connect", attributes );
  Serial.println( attributes );
  Serial.println("%. Sent to Mutch server.");Serial.println( attributes );
  Serial.println("%. Sent to Mutch server.");
  // Send payload
  delay(100);
  payload ="{\"";
  payload += id;
  payload +="\":[ ";
  payload +=message; //"\"Temp\":"; payload +=10; payload +=",";
 // payload +="\"Humi\":"; payload +=20;
  payload +="]}";

  payload.toCharArray( attributes, 512 );
  client.publish( "v1/gateway/telemetry", attributes );
  Serial.println( attributes );
  Serial.println("%. Sent to Thingsboard.");


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
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("1c27e3b0-cc23-11eb-9957-5f9dd9d8c61e", TOKEN, NULL) ) {
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
