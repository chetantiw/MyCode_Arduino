
String message = "";

/*--------------------------------------------------------------------------------------
Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/

#define SSerialTxControl PA1
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
Serial1.begin(9600);
pinMode(SSerialTxControl,OUTPUT);
Serial.println("Temperature & humidity & co2 Gateway demo......");




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





}
else
{
Serial.println("sensor not available at this address..");
}
delay(2000);
}
delay(5000);
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
Serial1.write(request, sizeof(request));

Serial1.flush();
digitalWrite(SSerialTxControl, RS485Receive);
delay(10);
int ct=0;

while( Serial1.available()<10)
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
int recvLen = Serial1.readBytes(buffer,12);

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
