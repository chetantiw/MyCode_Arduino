
#define SSerialTxControl 5   //26      
#define RS485Transmit    HIGH
#define RS485Receive     LOW


//   pin used in display   2,18,19,21,22,23
//   pin used in rs485     16,17,5
//   pin used in uart      3,1,9,10




byte buffer[15];
String temperature,humidity,co2;
unsigned char request[] = {0x01,0x00,0x00,0x01,0x00,0x05,0x20,0x0B};


void setup()

{
      // initialize serial ports
      Serial.begin(9600);
      Serial2.begin(9600);
      pinMode(SSerialTxControl,OUTPUT);
      Serial.println("Temperature & humidity & co2 Gateway demo......");
      
     // digitalWrite(dere,HIGH); //Enable high, RS485 shield waiting to transmit data
    //  Serial2.write(command,n); //Test send command Auto,Read to RS485
      delay(500);
}



void loop()
{

   
   int r = read_sensor(1); 
 
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
  
   // Serial.print("Tx: ");
   // for(int i=0;i<8;i++)
   // {
   //    Serial.print(request[i], HEX);
       //delay(100);
   // }
  //  Serial.println();

   digitalWrite(SSerialTxControl, RS485Transmit);
   delay(10);
   Serial2.write(request, sizeof(request));
      
   Serial2.flush();
   digitalWrite(SSerialTxControl, RS485Receive);
   delay(10);
   int ct=0;
    
    while( Serial2.available()<15)
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
   int recvLen = Serial2.readBytes(buffer,16);
   
   Serial.print("Rx: ");
    
  // Serial.print(buffer[3], HEX); Serial.print(" ");
 //  Serial.print(buffer[4], HEX); Serial.print(" ");
  // Serial.print(buffer[5], HEX); Serial.print(" ");
  // Serial.print(buffer[6], HEX); Serial.print(" "); 
  // Serial.print(buffer[7], HEX); Serial.print(" ");
 //  Serial.print(buffer[8], HEX); Serial.print(" ");
 //  Serial.println();
   
  // Convert to Float
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
    
    x = buffer[11]<<8;
    x = x + (buffer[12]);
    
    ones = x %10;
    x = (x / 10);
    tens = x % 10;
    x = (x / 10);
    hunds = x % 10;
    thous = x / 10;

    co2  = thous;
    co2 += hunds;
    co2 += tens;
    //co2 += '.';
    co2 += ones;
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
