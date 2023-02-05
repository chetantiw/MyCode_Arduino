
String message = "";
//#include <SoftwareSerial.h>
/*--------------------------------------------------------------------------------------
Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/

#define SSerialTxControl PA1
#define RS485Transmit HIGH
#define RS485Receive LOW

//SoftwareSerial mySerial(5, 6);

byte buffer[15];
String para1,para2;
unsigned char request[] = {0x01,0x03,0x00,0x00,0x00,0x02,0x71,0xCB};
int connect_ct = 0;
int send_count = 0;

void setup()
{
  
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(SSerialTxControl,OUTPUT);
  Serial.println("MODBUS Ready demo ");
  delay(2000);

}

void loop()
{
  
  Serial.print("Reading sensor: ");
  
  int r = read_sensor(1);
  if(r)
  {
    Serial.print("Register 1: ");
    Serial.print(para1);
    Serial.print(",");
    Serial.print("Register 2: ");
    Serial.println(para2);
  }
  else
  {
     Serial.println("sensor not available at this address..");
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
    if(ct==100)
    {
       break;
    }
    delay(1);
  }
  if(ct==1500)
  {
      return(0);
  }
  int recvLen = Serial1.readBytes(buffer,9);
  
  Serial.print("Rx: ");
  int x;
  para1="";
  para2="";
  
  if(recvLen>0)
  {
  x = buffer[3]<<8;
  x = x + (buffer[4]);
  ones = x %10;
  x = (x / 10);
  tens = x % 10;
  hunds = x / 10;
  
  para1 = String(hunds);
  para1 += '.';
  para1 += String(tens);
  para1 += String(ones);
  
  x = buffer[5]<<8;
  x = x + (buffer[6]);
  ones = x %10;
  x = (x / 10);
  tens = x % 10;
  hunds = x / 10;
  
  para2 = String(hunds);
  para2 += '.';
  para2 += String(tens);
  para2 += String(ones);
  
  
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
