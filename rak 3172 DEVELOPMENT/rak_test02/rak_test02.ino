/*************************************
 * Download RAK1901/RAK1902 library from https://downloads.rakwireless.com/RUI/RUI3/Library/
 * Besides, install them into Arduino IDE
 *************************************/


void uplink_routine();

#define SMART_FARM_PERIOD   (10000)
/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define SMART_FARM_BAND     (RAK_REGION_US915)
#define SMART_FARM_DEVEUI   {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x89}
#define SMART_FARM_APPEUI   {0x0E, 0x0D, 0x0D, 0x01, 0x0E, 0x01, 0x02, 0x0E}
#define SMART_FARM_APPKEY   {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3F}
String message = "";


#define SSerialTxControl PA1
#define RS485Transmit HIGH
#define RS485Receive LOW


byte buffer[15];
String para1,para2;
unsigned char request[] = {0x01,0x03,0x00,0x00,0x00,0x02,0x71,0xCB};
int connect_ct = 0;
int send_count = 0;


/** Packet buffer for sending */
char collected_data[128] = { 0 };
uint8_t a=0;
uint8_t b=0;
uint8_t c=0;
uint8_t d=0;
void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
  }
}

void joinCallback(int32_t status)
{
  Serial.printf("Join status: %d\r\n", status);
}

void sendCallback(int32_t status)
{
  if (status == 0) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
}

void setup()
{
  Serial.begin(115200, RAK_AT_MODE);
  Serial1.begin(9600);
  pinMode(SSerialTxControl,OUTPUT);
  Serial.println("RAKwireless Smart Farm Example");
  Serial.println("------------------------------------------------------");

  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = SMART_FARM_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = SMART_FARM_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = SMART_FARM_APPKEY;
  

  if (!api.system.lpm.set(1)) {
    Serial.printf
	("LoRaWan Smart Farm - set low power mode is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.nwm.set(1)) {
    Serial.printf
	("LoRaWan Smart Farm - set network working mode is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    Serial.printf
	("LoRaWan Smart Farm - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    Serial.printf
	("LoRaWan Smart Farm - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.
	printf("LoRaWan Smart Farm - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(SMART_FARM_BAND)) {
    Serial.printf("LoRaWan Smart Farm - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf
	("LoRaWan Smart Farm - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_OTAA))	// Set the network join mode to OTAA
  {
    Serial.printf
	("LoRaWan Smart Farm - set network join mode is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.join())	// Join to Gateway
  {
    Serial.printf("LoRaWan Smart Farm - join fail! \r\n");
    return;
  }

  Serial.println("++++++++++++++++++++++++++");
  Serial.println("RUI3 Environment Sensing");
  Serial.println("++++++++++++++++++++++++++");

//  Wire.begin();			// Start I2C Bus
//  Serial.printf("RAK1901 init %s\r\n", th_sensor.init()? "success" : "fail");	// Check if RAK1901 init success
//  Serial.printf("RAK1902 init %s\r\n", p_sensor.init()? "success" : "fail");	// Check if RAK1902 init success

  /** Wait for Join success */
  while (api.lorawan.njs.get() == 0) {
    Serial.print("Wait for LoRaWAN join...");
    api.lorawan.join();
    delay(10000);
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf
	("LoRaWan Smart Farm - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    Serial.
	printf("LoRaWan Smart Farm - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(1)) {
    Serial.printf
	("LoRaWan Smart Farm - set confirm mode is incorrect! \r\n");
    return;
  }
  api.lorawan.dr.set(4);

  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");	// Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");	// Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);	// Check Device Address
  Serial.printf("Uplink period is %ums\r\n", SMART_FARM_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
  if (api.system.timer.create(RAK_TIMER_0, (RAK_TIMER_HANDLER)uplink_routine, RAK_TIMER_PERIODIC) != true) {
    Serial.printf("LoRaWan Smart Farm - Creating timer failed.\r\n");
    return;
  }
  if (api.system.timer.start(RAK_TIMER_0, SMART_FARM_PERIOD, NULL) != true) {
    Serial.printf("LoRaWan Smart Farm - Starting timer failed.\r\n");
    return;
  }
}

void uplink_routine()
{
  /** Get sensor RAK1901 values */
  //th_sensor.update();

  float temp_f = 23;//th_sensor.temperature();

//  Serial.printf("T %.2f H %.2f P %.2f\r\n", temp_f, humid_f, press_f);
//
 //uint16_t t = (uint16_t) (temp_f * 10.0);
uint8_t t=45;
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
  
  delay(3000);
  /** Cayenne Low Power Payload */
  //String s = "{\"T\":23.34,\"H\":44.89}";
 // s.toCharArray(collected_data,s.length());
  uint8_t data_len = 0;//s.length();
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = (uint8_t) 'T';
  collected_data[data_len++] = data_len+48;
  collected_data[data_len++] = data_len+48;
  
 
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", collected_data[i]);
    Serial.println( collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *) & collected_data, 2, true, 1)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }
}

void loop()
{
  /* Destroy this busy loop and use timer to do what you want instead,
   * so that the system thread can auto enter low power mode by api.system.lpm.set(1); */
  api.system.scheduler.task.destroy();


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
  a=hunds;
  b=tens;
  c=ones;
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
