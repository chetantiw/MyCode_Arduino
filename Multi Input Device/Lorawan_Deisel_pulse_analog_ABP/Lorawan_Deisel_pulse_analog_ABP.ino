
#include <ArduinoJson.h>


#define ABP_PERIOD   (30000)
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
#define ABP_BAND RAK_REGION_EU868
 
                  //    01    02    03    08
#define ABP_DEVADDR  {0x01, 0x02, 0x03, 0x08}
                  //    B4    91    CC    10    17    0E    89    04    33   CA    5B    13     1E    74    20    04
#define ABP_APPSKEY  {0xB4, 0x91, 0xCC, 0x10, 0x17, 0x0E, 0x89, 0x04, 0x33, 0xCA, 0x5B, 0x13, 0x1E, 0x74, 0x20, 0x04}
                  //    BF    9B    75    BC    D6    08    06    DD    80    ED    B8   E6     83    29    9D    01
#define ABP_NWKSKEY  {0xBF, 0x9B, 0x75, 0xBC, 0xD6, 0x08, 0x06, 0xDD, 0x80, 0xED, 0xB8, 0xE6, 0x83, 0x29, 0x9D, 0x04}

                    //  e4     9f    92     dc  0a    c3     49    fa
#define OTAA_DEVEUI   {0xe4, 0x9f, 0x92, 0xdc, 0x0a, 0xc3, 0x49, 0xfa}                 

String message = "";
String prevalue = "";

uint8_t interruptPin1 = PB5;

int pulse_count = 0;
long litre_count = 0;
float cubic_ltr = 0;


void blink1()
{
  pulse_count++;
  if(pulse_count==10)
  {
    litre_count++;
    pulse_count = 0;
  }

}

byte buffer[15];

int connect_ct = 0;
int send_count = 0;


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
  pinMode(interruptPin1, INPUT_PULLUP);
  attachInterrupt(interruptPin1, blink1, FALLING);
  Serial.begin(115200);
  Serial.println("RAKwireless LoRaWan DIESEL METER");
  Serial.println("------------------------------------------------------");
  uint8_t flash_value[4];

//  litre_count = 0;
//        flash_value[0] = litre_count & 0x000000FF;
//        flash_value[1] = (litre_count & 0x0000FF00)>>8;
//        flash_value[2] = (litre_count & 0x00FF0000)>>16;
//        flash_value[3] = (litre_count & 0xFF000000)>>24;
//        api.system.flash.set(7000, flash_value,4);
//        pulse_count=0;
//        flash_value[0] = pulse_count;
//        api.system.flash.set(7100, flash_value,1);

 //  delay(100);

  
  api.system.flash.get(7100, flash_value, 1);
  pulse_count = flash_value[0]; 
  
  api.system.flash.get(7000, flash_value,4);
  
  litre_count = flash_value[0];
  litre_count |= (flash_value[1]<<8);
  litre_count |= (flash_value[2]<<16);
  litre_count |= (flash_value[3]<<24);
  
  Serial.println("flashvalue:");

  Serial.print(litre_count);
      Serial.print(".");
      Serial.print(pulse_count);
      Serial.println("Litres");
  
//  for(int i=0; i<10;i++)
//  {
//    prevalue += (char)flash_value[i];
//  }
//  cubic_ltr = prevalue.toFloat();
  
 // Serial.println(cubic_ltr,3);
  
   uint8_t node_device_eui[8] = OTAA_DEVEUI;
  // ABP Device Address MSB first
  uint8_t node_dev_addr[4] = ABP_DEVADDR;
  // ABP Application Session Key
  uint8_t node_app_skey[16] = ABP_APPSKEY;
  // ABP Network Session Key
  uint8_t node_nwk_skey[16] = ABP_NWKSKEY;

  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
    Serial.printf("LoRaWan ABP - set device addr is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appskey.set(node_app_skey, 16)) {
    Serial.printf
  ("LoRaWan ABP - set application session key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
    Serial.printf
  ("LoRaWan ABP - set network session key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.band.set(ABP_BAND)) {
    Serial.printf("LoRaWan ABP - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan ABP - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_ABP)) // Set the network join mode to ABP
  {
    Serial.
  printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
    return;
  }


  /** Wait for Join success */
  Check_Lora_Connect();

  if (!api.lorawan.adr.set(true)) {
    Serial.printf
  ("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(1)) {
    Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
    return;
  }
  api.lorawan.dr.set(4);
  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF"); // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
  
  
}

void Check_Lora_Connect()
{
  int break_ctr = 0;
  while (api.lorawan.njs.get() == 0) {
    break_ctr++;
    Serial.print("Wait for LoRaWAN join...");
    api.lorawan.join();
    delay(10000);
    if(break_ctr == 6)
     {
      break;
     }
  }
}
void uplink_routine()
{
  Check_Lora_Connect();
  int ret = 0; 
 StaticJsonDocument<200> json;
 json["milliLitre"] = String(pulse_count);
 json["Litre"] = String(litre_count);
 
 char charArray[128];
 int len = serializeJson(json, charArray);
 char buildBuffer[2] = {0};
 char compositionBuffer[128] = {0};  // this will hold a string we build
    
 for (int i = 0; i < len; i++) {
     sprintf( buildBuffer, "%02X ", (uint8_t)charArray[i]);
     //Serial.println(buildBuffer);
     compositionBuffer[i*2] = buildBuffer[0];
     compositionBuffer[i*2+1] = buildBuffer[1];
 }
 //Serial.println(sizeof(compositionBuffer));
// Serial.print("Length:");
 //Serial.println(len);
 /** Send the data package */
  if (api.lorawan.send(len*2, (uint8_t *) & compositionBuffer, 2, true, 1)) {
    Serial.println("Sending is requested");
    //Serial.println(compositionBuffer);
  } else {
    Serial.println("Sending to Server");
  }
}


void loop()
{
  static uint64_t last = 0;
  static uint64_t elapsed;
 // Serial.println(pulse_count);
  if(pulse_count > 0)
  {
      if ((elapsed = millis() - last) > ABP_PERIOD) {
      
      Serial.print("Volume: ");
      Serial.print(litre_count);
      Serial.print(".");
      Serial.print(pulse_count);
      Serial.println("Litres");
  
        uint8_t flash_value[4];
        flash_value[0] = litre_count & 0x000000FF;
        flash_value[1] = (litre_count & 0x0000FF00)>>8;
        flash_value[2] = (litre_count & 0x00FF0000)>>16;
        flash_value[3] = (litre_count & 0xFF000000)>>24;
        api.system.flash.set(7000, flash_value,4);
        flash_value[0] = pulse_count;
        api.system.flash.set(7100, flash_value,1);
        last = millis();
        uplink_routine();
      }
      
  }
  //Serial.printf("Try sleep %ums..", OTAA_PERIOD);
  //api.system.sleep.all(OTAA_PERIOD);
  //Serial.println("Wakeup..");



    //delay(210);
}
