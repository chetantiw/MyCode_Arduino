#include <WiFi.h>
#include <PubSubClient.h>
#include <DMD32.h>
#include <SPI.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"
#define send_delay 20
#define TOKEN "nkT9xIgwfoxBDxegygLF"
const char* ssid ="MuTech"; //replace this with your wifi name
const char* password ="mpass@w0rd"; //replace with your wifi password
char thingsboardServer[] = "52.66.229.86";
unsigned long lastSend;
int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 3
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
//Timer setup
//create a hardware timer of ESP32
hw_timer_t * timer = NULL;

String message = "";

/*--------------------------------------------------------------------------------------
Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
dmd.scanDisplayBySPI();
}
#define SSerialTxControl 16 //26 //5
#define RS485Transmit HIGH
#define RS485Receive LOW

// pin used in display 2,18,19,21,22,23
// pin used in rs485 16,17,5/26
// pin used in uart 3,1,9,10

byte buffer[15];
String temperature,humidity,co2;
unsigned char request[] = {0x01,0x03,0x00,0x01,0x00,0x03,0x20,0x0C};
int connect_ct = 0;
int send_count = 0;
void setup()
{
// initialize serial ports
Serial.begin(9600);
Serial2.begin(9600);
pinMode(SSerialTxControl,OUTPUT);
Serial.println("Temperature & humidity & co2 Gateway demo......");
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
connect_ct++;
if(connect_ct>=20)
{break;}
}
Serial.println("Connected to WiFi");
Serial.println("EMS Node ");
client.setServer( thingsboardServer, 1883 );

delay(500);
// return the clock speed of the CPU
uint8_t cpuClock = ESP.getCpuFreqMHz();

// Use 1st timer of 4
// devide cpu clock speed on its speed value by MHz to get 1us for each signal of the timer
timer = timerBegin(0, cpuClock, true);
// Attach triggerScan function to our timer
timerAttachInterrupt(timer, &triggerScan, true);
// Set alarm to call triggerScan function
// Repeat the alarm (third parameter)
timerAlarmWrite(timer, 300, true);

// Start an alarm
timerAlarmEnable(timer);

dmd.clearScreen( true );
dmd.selectFont(SystemFont5x7);
// Français, Österreich, Magyarország
const char *MSG = "MuTech";
dmd.drawMarquee(MSG,strlen(MSG),0,0);
MSG = "H,T&CO2";
dmd.drawMarquee(MSG,strlen(MSG),0,10);
MSG = "Sensor";
dmd.drawMarquee(MSG,strlen(MSG),0,20);
delay(2000);

}

void loop()
{
for( int lp=1;lp<5;lp++)
{
Serial.print("Reading sensor: ");
Serial.println(lp);
int r = read_sensor(lp);
if(r)
{
Serial.print("Temp: ");
Serial.print(temperature);
Serial.print(" Celcius, ");
Serial.print("Hum: ");
Serial.print(humidity);
Serial.print("% ");
Serial.print("Co2: ");
Serial.print(co2);
Serial.println("ppm");

display_sensor(String(lp));

message = "{\"Co2\":";
message += co2;
message += ",\"temperature\":";
message += temperature;
message += ",\"humidity\":";
message += humidity;
message += "}";
getAndSendData(lp);

}
else
{
Serial.println("sensor not available at this address..");
}
delay(2000);
}
delay(5000);
}

void display_sensor(String ch)
{

dmd.clearScreen( true );
dmd.selectFont(SystemFont5x7);
// Français, Österreich, Magyarország
const char *MSG = " Ch:";

dmd.drawMarquee(MSG,strlen(MSG),0,0);
MSG = ch.c_str();
dmd.drawMarquee(MSG,strlen(MSG),24,0);

MSG = "T:";
dmd.drawMarquee(MSG,strlen(MSG),0,10);
MSG = temperature.c_str();
dmd.drawMarquee(MSG,strlen(MSG),10,10);

MSG = "H:";
dmd.drawMarquee(MSG,strlen(MSG),0,20);
MSG = humidity.c_str();
dmd.drawMarquee(MSG,strlen(MSG),10,20);

MSG = "CO2:";
dmd.drawMarquee(MSG,strlen(MSG),0,30);
MSG = co2.c_str();
dmd.drawMarquee(MSG,strlen(MSG),0,40);
delay(2000);
}
void getAndSendData(int deviceID){

if ( !client.connected() )
{
reconnect();
}
String payload ="{\"device\":\"LWEN000";
payload += deviceID;
payload +="\"}";
// Send connect
char attributes[512];
payload.toCharArray( attributes, 100 );
client.publish( "v1/gateway/connect", attributes );
Serial.println( attributes );
Serial.println("%. Sent to Thingsboard.");Serial.println( attributes );
Serial.println("%. Sent to Thingsboard.");
// Send payload
delay(100);
payload ="{\"LWEN000";
payload += deviceID;
payload +="\":[ ";
payload +=message;
payload +="]}";
payload.toCharArray( attributes, 100 );
client.publish( "v1/gateway/telemetry", attributes );
Serial.println( attributes );
Serial.println("%. Sent to Thingsboard.");

}

void reconnect() {
// Loop until we're reconnected
connect_ct = 0;
while (!client.connected()) {
status = WiFi.status();
if ( status != WL_CONNECTED) {
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
connect_ct++;
if(connect_ct>=20)
{break;}
}
//Serial.println("Connected to Server");
}
if(connect_ct<=20)
{

Serial.print("Connecting to ThingsBoard node ...");
// Attempt to connect (clientId, username, password)
if ( client.connect("74578990-ac39-11ec-98da-79f58b5c0f0c", TOKEN, NULL) ) {
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
else
{
Serial.print("Wifi not available");
break;
}
}
}
int read_sensor(unsigned char b)
{
unsigned int res=0x0000;
byte t = 0x00;
int ones,tens,hunds,thous;

request[0] = b;
res = CRC16_2(request,6);
t = res & 0x00ff;
request[6] = t;
t = (res & 0xff00)>>8;
request[7] = t;

digitalWrite(SSerialTxControl, RS485Transmit);
delay(10);
Serial2.write(request, sizeof(request));

Serial2.flush();
digitalWrite(SSerialTxControl, RS485Receive);
delay(10);
int ct=0;

while( Serial2.available()<10)
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
int recvLen = Serial2.readBytes(buffer,12);

Serial.print("Rx: ");
int x;
temperature="";
humidity="";
co2="";
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
// temperature += '.';
temperature += ones;

x = buffer[5]<<8;
x = x + (buffer[6]);
ones = x %10;
x = (x / 10);
tens = x % 10;
hunds = x / 10;

humidity = hunds;
humidity += tens;
//humidity += '.';
humidity += ones;

x = buffer[7]<<8;
x = x + (buffer[8]);

ones = x %10;
x = (x / 10);
tens = x % 10;
x = (x / 10);
hunds = x % 10;
thous = x / 10;
co2 = thous;
co2 += hunds;
co2 += tens;
//co2 += '.';
co2 += ones;

return(1);
}
else
{
return(-1);
}

}
unsigned int CRC16_2(byte *buf, int len)
{
// for (int i=0;i<8;i++)
// {
// Serial.print(buf[i],HEX);
// }
// Serial.println("");
unsigned int crc = 0xFFFF;
for (int pos = 0; pos < len; pos++)
{
crc ^= (unsigned int)buf[pos]; // XOR byte into least sig. byte of crc
for (int i = 8; i != 0; i--) { // Loop over each bit
if ((crc & 0x0001) != 0) { // If the LSB is set
crc >>= 1; // Shift right and XOR 0xA001
crc ^= 0xA001;
}
else // Else LSB is not set
crc >>= 1; // Just shift right
}
}
return crc;
}




//-----------------------------------------------------end---------------------------------------
