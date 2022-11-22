#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define TOKEN "2yoji5xLyESAEVmyLM2S"

const char* ssid ="MuTech"; //replace this with your wifi  name
const char* password ="mpass@w0rd"; //replace with your wifi password
char thingsboardServer[] = "103.150.186.172";
unsigned long lastSend;
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
PubSubClient client(wifiClient);



#define SSerialTxControl 5
#define RS485Transmit    HIGH
#define RS485Receive     LOW


byte buffer[15];
String temperature,humidity;
unsigned char request[] = {0x01,0x03,0x00,0x01,0x00,0x02,0x20,0x0B}; 


void setup()

{
      // initialize serial ports
      Serial.begin(9600);
      Serial2.begin(9600);
      pinMode(SSerialTxControl,OUTPUT);
      Serial.println("NetStack Temperature & humidity Gateway ");
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to WiFi");
      Serial.println("MODBUS GATEWAY");
      client.setServer(thingsboardServer, 1883 );
      
      delay(500);
}

int rES;

void loop()
{
 
 
 if ( millis() - lastSend > 30000 ) { // Update and send only after 1 seconds
       for(int lp=1;lp<=4;lp++)
       {
          temperature="";
          humidity="";
          Serial.print("Reading sensor ");
          Serial.println(lp);
          rES = read_sensor(lp); 
          if(rES)
          {
            Serial.print("Temp: ");
            Serial.print(temperature);
            Serial.print(" Celcius,  "); 
            Serial.print("Hum: ");
            Serial.print(humidity);
            Serial.println("%");
            
            String ID = "G1TH00";
            ID +=lp;
            logData(ID);
          } 
          else
          {
            Serial.println("sensor not available at this address..");
          }
        delay(1000);
       }
       lastSend = millis();
  }
  client.loop();
}

void update_device_status(int d,int s)
{
  if(s)
  {
     String topic = "{\"device\":\"";
     topic += "G1TH00"+d;
     topic += " \"}";
     char attributes[50];
     topic.toCharArray( attributes, 100 );
     client.publish( "v1/gateway/connect", attributes );
     Serial.println( attributes );
  }
  else
  {
    String topic = "{\"device\":\"";
     topic += "G1TH00"+d;
     topic += " \"}";
     char attributes[50];
     topic.toCharArray( attributes, 100 );
     client.publish( "v1/gateway/diconnect", attributes );
     Serial.println( attributes );
  }
}



void logData(String ID){

  if ( !client.connected() ) {
    reconnect();
  }
  String payload ="{\"";
  payload += ID;
  payload +="\":[{";
  payload +="\"Temperature\":"; payload +=temperature; payload +=",";
  payload +="\"Humidity\":"; payload +=humidity;
  payload +="}]}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
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
   
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
       }
    Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("41bc9130-e5f4-11eb-ab98-631c0ba4df0c", TOKEN, NULL) ) {
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


int read_sensor(unsigned char b)
{
   unsigned int res=0x0000;
   byte t = 0x00;
   int ones,tens,hunds;
   
     request[0] = b;
     res = CRC16_2(request,6);
     t = res & 0x00ff;
     request[6] = t;
     t = (res & 0xff00)>>8;
     request[7] = t;
  
    Serial.print("Tx: ");
    for(int i=0;i<8;i++)
    {
       Serial.print(request[i], HEX);
       //delay(100);
    }
    Serial.println();

   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   Serial2.write(request, sizeof(request));
      
   Serial2.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
    
    while( Serial2.available()<8)
    {
        ct++;
        if(ct==1500)
        {
          break;
        }
        delay(1);
     }
     if(ct==1500)
     {
       return(0);
     }
   int recvLen = Serial2.readBytes(buffer,10);
   
   Serial.print("Rx: ");
    
   Serial.print(buffer[3], HEX); Serial.print(" ");
   Serial.print(buffer[4], HEX); Serial.print(" ");
   Serial.print(buffer[5], HEX); Serial.print(" ");
   Serial.print(buffer[6], HEX); Serial.print(" "); 
   Serial.println();
   
  // Convert to Float
  int x;
  temperature="";
  humidity="";
  
  if(recvLen>0)
  {
    x = buffer[3]<<8;
    x = x + (buffer[4]);  
   

    ones = x %10;
    x = (x / 10);
    tens = x % 10;
    hunds = x / 10;

    
    temperature = hunds;
    temperature += tens;
    temperature += '.';
    temperature += ones;
    
    x=0;ones=0;tens=0;hunds=0;
    x = buffer[5]<<8;
    x = x + (buffer[6]);
     ones = x %10;
    x = (x / 10);
    tens = x % 10;
    hunds = x / 10;
    
    humidity = hunds;
    humidity += tens;
    humidity += '.';
    humidity += ones;
    

    // ((byte*)&x)[1]= buffer[3];
    // ((byte*)&x)[0]= buffer[4];
    // ((byte*)&x)[1]= buffer[5];
   //  ((byte*)&x)[0]= buffer[6];
   return(1);
  }
  else
  {
    return(-1); 
  } 
 
}

unsigned int CRC16_2(byte *buf, int len)
{  
//  for (int i=0;i<8;i++)
//  {
//     Serial.print(buf[i],HEX);
//  }
 // Serial.println("");
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}
