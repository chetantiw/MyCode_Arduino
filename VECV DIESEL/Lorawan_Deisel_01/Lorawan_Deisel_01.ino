
#include <ArduinoJson.h>


#define OTAA_PERIOD   (30000)
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
#define OTAA_BAND     (RAK_REGION_IN865)
#define OTAA_DEVEUI   {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x90}
#define OTAA_APPEUI   {0x0E, 0x0D, 0x0D, 0x01, 0x0E, 0x01, 0x02, 0x0E}
#define OTAA_APPKEY   {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x4F}
                  //     2b    7e    15    16    28    ae    d2    a6    ab    f7    15    88    09    cf    4f    4f

/** Packet buffer for sending */
//uint8_t collected_data[64] = { 0 };


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
  
   
  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = OTAA_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = OTAA_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = OTAA_APPKEY;

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(OTAA_BAND)) {
    Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_OTAA))  // Set the network join mode to OTAA
  {
    Serial.
  printf("LoRaWan OTAA - set network join mode is incorrect! \r\n");
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
    Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
    return;
  }
//  if (!api.lorawan.cfm.set(1)) {
//    Serial.printf("LoRaWan OTAA - set confirm mode is incorrect! \r\n");
//    return;
//  }
  api.lorawan.dr.set(4);
  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF"); // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %ums\r\n", OTAA_PERIOD);
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
      if ((elapsed = millis() - last) > OTAA_PERIOD) {
        
        
        
     
      Serial.print("Volume: ");
      Serial.print(litre_count);
      Serial.print(".");
      Serial.print(pulse_count);
      Serial.println("Litres");
      
      
//      Serial.print(pulse_count);
//      Serial.println(" Litres");
//      double cubicFloat = pulse_count / 1000;
//      Serial.println("Cubic Double Litres");
//      Serial.println(cubicFloat);
//
//      long checkit = 10000L * cubicFloat;
//      Serial.println("Long Convert");
//      Serial.println(checkit);
//      
//      int cubiclt = pulse_count / 1000;
//      int cubic_dec = pulse_count % 1000;  
//      Serial.println(cubiclt);
//      Serial.println("CU Litres");  
//      String cubicliter = String(cubiclt);
//      String cubicliterdec = String(cubic_dec);
//      String total = "";
//      if(cubic_dec < 10)
//      {
//        total = cubicliter + ".00" + cubic_dec;
//      }
//      else if(cubic_dec >= 10 && cubic_dec < 99)
//      {
//        total = cubicliter + ".0" + cubic_dec;
//      }
//      else if(cubic_dec > 99)
//      {
//        total = cubicliter + "." + cubic_dec;
//      }
//    
//      Serial.println(total);
//      Serial.println(total.toFloat(),3);
//      cubic_ltr= cubic_ltr * 1000;
//      cubic_ltr = cubic_ltr + pulse_count;
//      
//      cubic_ltr = cubic_ltr / 1000;
//      
//      //cubic_ltr = cubic_ltr + total.toFloat();
//      String final_value = String(cubic_ltr,3);
//      Serial.println("Final Value:");
//      Serial.println(final_value);
//      for(int lp = 0;lp<final_value.length();lp++)
//      {flash_value[lp] = final_value.charAt(lp);}
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
