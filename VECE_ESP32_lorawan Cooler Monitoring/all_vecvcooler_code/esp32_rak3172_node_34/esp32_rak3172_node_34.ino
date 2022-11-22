#include <ArduinoJson.h>
#include "EmonLib.h"                  
#include <FlowMeter.h> 
#include<EEPROM.h>
 
#define temp_probe 33
#define current_probe 32
#define flow_probe 26
#define relay 22
#define red_led 23
#define yellow_led 4
#define blue_led 14
#define white_led 12

#define setCHAdd 10
#define setCLAdd 20
#define setTHAdd 30
#define setFLAdd 40
#define tcwtAdd 50
#define ccwtAdd 60
#define fcwtAdd 70
#define motor_swAdd 80
#define flag_Add 90  //p1

EnergyMonitor emon1; 
FlowMeter *Meter;

float Vin=3.5;     // [V]        
float Rt=10000;    // Resistor t [ohm]
float R0=10000;    // value of rct in T0 [ohm]
float Tzero=298.15;   // use T0 in Kelvin [K]
float Vout=0.0;    // Vout in A0 
float Rout=0.0;    // Rout in A0
// use the datasheet to get this data.
float Tone=273.15;      // [K] in datasheet 0º C
float Ttwo=373.15;      // [K] in datasheet 100° C
float RT1=35563;   // [ohms]  resistence in T1
float RT2=549;    // [ohms]   resistence in T2
float beta=0.0;    // initial parameters [K]
float Rinf=0.0;    // initial parameters [ohm]   
float TempK=0.0;   // variable output
float TempC=0.0;   // variable output
const unsigned long period = 1000;
float current_sens = 0;
int current_calib = 0; //p2
int flow = 0;
int send_count;
int sens_count1,sens_count2,sens_count3,sens_count4;

float setCH,setCL,setTH,setFL;
int tcwt,ccwt,fcwt;
int motor_sw;
int flag=0;
long start_time;
int recv_flag = 0; 
int time_Tosend = 2;
int time_Tosend_error = 0;

static char recv_buf[512];
static bool is_exist = false;
static bool is_join = false;


void MeterISR()
{
    Meter->count();
}

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    Serial2.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
        return 0;
 
    do
    {
        while (Serial2.available() > 0)
        {
            ch = Serial2.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
            return 1;
 
    } while (millis() - startMillis < timeout_ms);
    
    return 0;
}
 
static void recv_prase(char *p_msg)
{
   // Serial.println("I am decoding...");
  Serial.println("checking for data packets");
   
  //  char *p_start = NULL;
   char data[128];       // To hold the received bytes as characters
   recv_flag = 1;
    int bytes_len=0;
   // p_start = strstr(p_msg, "RX");

   String rec(p_msg);
   int stInd = rec.indexOf("7b");
   int endInd = rec.indexOf("7d");
   String dataFiltered = rec.substring(stInd,endInd+3);
   //Serial.println(dataFiltered);
   int len = dataFiltered.length();
   byte byteArray[128] = {0};
   dataFiltered.toCharArray(data,len);
   int ret=0;
   char temp[len];
   for(int lp = 0;lp<len;lp++)
   {
       char c = data[lp*2];
       byteArray[lp]=nibble(c)<<4;
       c = data[lp*2+1];
       byteArray[lp] += nibble(c);
      // Serial.println((char)byteArray[lp]);
       temp[lp]=(char)byteArray[lp];
   }
 // Serial.println(temp);
 // EEPROM.writeString(savedParaAdd, String(temp));
 // EEPROM.commit();
 // Serial.println("stringData write in EEPROM is Successful"); 
  // Create a JSON document of specified capacity <100> and load the byteArray to it
  StaticJsonDocument<100> doc;
  deserializeJson(doc, temp);
  // Print the received JSON message by serializing it
   serializeJson(doc, Serial);
  Serial.println();

 
   if(!doc.containsKey("mxC"))
   {
     Serial.println("setCH is NULL");
   }
   else
   {
     setCH = doc["mxC"];
     EEPROM.writeString(setCHAdd, String(setCH));
     EEPROM.commit();
     Serial.println(setCH);
   }
   if(!doc.containsKey("miC"))
   {
     Serial.println("setCL is NULL");
   }
   else
   {
     setCL = doc["miC"];
     EEPROM.writeString(setCLAdd, String(setCL));
     EEPROM.commit();
     Serial.println(setCL);
   }
   if(!doc.containsKey("mxT"))
    {
     Serial.println("setTH is NULL");
    }
    else
    {
     setTH = doc["mxT"];
     EEPROM.writeString(setTHAdd, String(setTH));
     EEPROM.commit();
     Serial.println(setTH);
    }

   if(!doc.containsKey("miF"))
   {
     Serial.println("setFL is NULL");
   }
   else
   {
     
     setFL = doc["miF"];
     EEPROM.writeString(setFLAdd, String(setFL));
     EEPROM.commit();
     Serial.println(setFL);
   }
   if(!doc.containsKey("tcwt"))
   {
     Serial.println("tcwt is NULL");
   }
   else
   {
     
     tcwt = (doc["tcwt"]);
     EEPROM.writeString(tcwtAdd, String(tcwt));
     EEPROM.commit();
     Serial.println(tcwt);
   }
   if(!doc.containsKey("ccwt"))
   {
     Serial.println("ccwt is NULL");
   }
   else
   {
     
     ccwt = (doc["ccwt"]);
     EEPROM.writeString(ccwtAdd, String(ccwt));
     EEPROM.commit();
     Serial.println(ccwt);
   }
   if(!doc.containsKey("fcwt"))
   {
     Serial.println("fcwt is NULL");
   }
   else
   {
     
     fcwt = (doc["fcwt"]);
     EEPROM.writeString(fcwtAdd, String(fcwt));
     EEPROM.commit();
     Serial.println(fcwt);
   }
   if(!doc.containsKey("motor_sw"))
   {
     Serial.println("motor_sw is NULL");
   }
   else
   {
     
     int motor_sw = (doc["motor_sw"]);
     
     if(motor_sw==0)
     {
       digitalWrite(relay,LOW);
       Serial.println("motor switched off from the server command");
     }
     else if(motor_sw==1)
     {
       digitalWrite(relay,HIGH);
       Serial.println("motor switched on from the server command");
       flag=0;
       digitalWrite(red_led,LOW);
       digitalWrite(yellow_led,LOW);
       digitalWrite(blue_led,LOW);
      
       sens_count1=0;
       sens_count2=0;
       sens_count3=0;
       sens_count4=0;
       time_Tosend_error = 0;
       time_Tosend = 0;;
       
     }

      EEPROM.writeString(motor_swAdd, String(motor_sw));
      EEPROM.commit();
     
     
   }
  
  
}


byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}


void init_lora()
{
    Serial2.flush();
    at_send_check_response("OK", 100, "AT\r\n");
   
    delay(500);
    if (at_send_check_response("OK", 100, "AT\r\n"))
    {
      is_exist = true;
      at_send_check_response("OK", 1000, "AT+DEVEUI=?\r\n");
      at_send_check_response("OK", 1000, "AT+NWM=1\r\n");
      at_send_check_response("OK", 1000, "AT+NJM=1\r\n");
      at_send_check_response("OK", 1000, "AT+CLASS=A\r\n");
      at_send_check_response("OK", 1000, "AT+BAND=3\r\n");
      at_send_check_response("OK", 1000, "AT+APPKEY=ecf63c332de0fb729cc572881b537a8e\r\n");
      delay(200);
      is_join = true;
     
    }
    else
    {
      is_exist = false;
      Serial.print("No rak3172 module found.\r\n");
      ESP.restart();
    }  
  
}


void send_telematry()
{
            
 int ret = 0;
 StaticJsonDocument<200> json;
 json["T"] = String(TempC);
 json["C"] = String(current_sens);
 json["F"] = String(flow);
 json["A"] = String(flag);
 char charArray[64];
 int len = serializeJson(json, charArray);
 char buildBuffer[2] = {0};
 char compositionBuffer[len*2+1] = {0};  // this will hold a string we build
    
 for (int i = 0; i < len; i++) {
     sprintf( buildBuffer, "%02X ", (uint8_t)charArray[i]);
     compositionBuffer[i*2] = buildBuffer[0];
     compositionBuffer[i*2+1] = buildBuffer[1];
 }
 char cmd[128];
 sprintf(cmd,"AT+SEND=2:");
 strcat(cmd,compositionBuffer);
 strcat(cmd,"\r\n");
 int br = 0; 
 Serial2.flush();         
 do{
     br++;
     ret = at_send_check_response("+EVT:SEND_CONFIRMED_OK", 10000, cmd);
     if (strstr(recv_buf,"+EVT:RX_1"))
     {
        recv_prase(recv_buf);
     }
     if(br>10)
     {break;}
   }while(ret==0);

}


int read_sensors()
{
    
    Vout=Vin*((float)(analogRead(temp_probe))/4096.0); // calc for ntc
    Rout=(Rt*Vout/(Vin-Vout));
    TempK=(beta/log(Rout/Rinf)); // calc for temperature
    TempC=TempK-273.15;
    double Irms = emon1.calcIrms(1480);
    if(Irms<0.10)
    current_sens = 0;
    else
    current_sens = Irms;
    
    Serial.println("Temperature: ");
    Serial.println(TempC);
    Serial.println("C  ");
    delay(500);
    Serial.println("Current: ");
    Serial.println(current_sens);
    Serial.println("Amp");
    delay(500);
    delay(period);
    Meter->tick(period);
    flow=Meter->getCurrentFlowrate();
    Serial.println("flow: " + String(flow) + " l/min, " + String(Meter->getTotalVolume())+ " l total.");
     
    if(current_sens > setCH && flag == 0)
    {
       sens_count4++;
       // Serial.println(sens_count3);
       if(sens_count4 > round(ccwt/5))
       {
           flag = 1;
           sens_count4 = 0;
           digitalWrite(red_led,HIGH);
       }
            
            
     }
     if(current_sens < setCL && flag == 0)
     {
          sens_count3++;
           
         // Serial.println(sens_count3);
          if(sens_count3 > round(ccwt/5))
          {
              flag = 2;
              sens_count3 = 0;
              digitalWrite(red_led,HIGH);
        
          }
          
            
     }
     if(flow <= setFL && flag == 0)
     {
          sens_count2++;
           
          // Serial.println(sens_count2);
          if(sens_count2 > round(fcwt/5))
          {
             flag = 4;
             sens_count2 = 0; 
             digitalWrite(blue_led,HIGH);
               
          }
            
            
      }
      if(TempC > setTH && flag == 0)  
      {
          sens_count1++;
          // Serial.println(sens_count1);
          if(sens_count1 > round(tcwt/5))
          {
              flag = 3;
              sens_count1 = 0;
              digitalWrite(yellow_led,HIGH);
       
           }
           
           
      }
      if(motor_sw == 1 && flag > 0)
      {
         digitalWrite(relay,LOW);
         Serial.print("motor shuting down, alarm value: ");
         Serial.println(flag);
        
      }
  return(flag);
}

void config_sensors()
{
    current_calib = 11;
    beta=(log(RT1/RT2))/((1/Tone)-(1/Ttwo));
    Rinf=R0*exp(-beta/Tzero);
    emon1.current(current_probe, current_calib); 
    Meter = new FlowMeter(digitalPinToInterrupt(flow_probe), UncalibratedSensor, MeterISR, RISING); 
    
} 




void init_hardware()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    EEPROM.begin(512); 
    delay(500);
    
    pinMode(temp_probe, INPUT);
    pinMode(relay, OUTPUT);
    pinMode(red_led, OUTPUT);
    pinMode(yellow_led, OUTPUT);
    pinMode(blue_led, OUTPUT);
    pinMode(white_led, OUTPUT);
   

     
    digitalWrite(red_led,HIGH);
    delay(1000);
    digitalWrite(yellow_led,HIGH);
    delay(1000);
    digitalWrite(blue_led,HIGH);
    delay(1000);
    digitalWrite(white_led,HIGH);
    delay(1000); 
    digitalWrite(red_led,LOW);
    digitalWrite(yellow_led,LOW);
    digitalWrite(blue_led,LOW);
    digitalWrite(white_led,LOW); 
}


void loading_memory()
{
  Serial.println("temp_flow_current demo....02");
   
    setCH = EEPROM.readString(setCHAdd).toFloat();
    delay(200);
    setCL = EEPROM.readString(setCLAdd).toFloat();
    delay(200);
    setTH = EEPROM.readString(setTHAdd).toFloat();
    delay(200);
    setFL = EEPROM.readString(setFLAdd).toFloat();
    delay(200);
    tcwt = EEPROM.readString(tcwtAdd).toFloat();
    delay(200);
    ccwt = EEPROM.readString(ccwtAdd).toFloat();
    delay(200);
    fcwt = EEPROM.readString(fcwtAdd).toFloat();
    delay(200);
    motor_sw = EEPROM.readString(motor_swAdd).toFloat();
    delay(200);
    if(motor_sw==1  )
    {
       digitalWrite(relay,HIGH);
    }
    else
    {
      digitalWrite(relay,LOW);
       
    }

    time_Tosend = 2;
    time_Tosend_error = 0;
     
    Serial.println("Saved limits: setCH,setCL,setTH,setFL,tcwt,ccwt,fcwt,motor_sw");
    Serial.println(setCH); 
    Serial.println(setCL); 
    Serial.println(setTH); 
    Serial.println(setFL); 
    Serial.println(tcwt); 
    Serial.println(ccwt); 
    Serial.println(fcwt); 
    Serial.println(motor_sw); 
    
}

void setup(void)
{
   
   init_hardware();
   loading_memory(); 
   init_lora();
   config_sensors(); 
   delay(5000);  
  
}

void loop(void)
{
     
    if (is_exist)
    {
      int ret = 0;
      start_time = millis();
      if (is_join)
      {
        ret = at_send_check_response("+EVT:JOINED", 12000, "AT+JOIN=1:0:10:1\r\n");
        if (ret)
        {
            is_join = false; 
            digitalWrite(white_led,HIGH);
         
           
        }
        else
        {
            at_send_check_response("OK", 1000, "AT+DEVEUI=?\r\n");
            Serial.print("JOIN failed!\r\n\r\n"); 
            delay(5000);
            digitalWrite(white_led,LOW);
        }
        
      }
      else
      {
               
        int flag = read_sensors();
        send_count++;
        if(send_count > time_Tosend || flag > 0 )
        {       
           if(flag > 0)
           {
            if(send_count > time_Tosend_error)
            {
                send_telematry();
                send_count = 0;
                time_Tosend_error = 40;
            }
           }
           else
           {
            send_telematry();
            send_count = 0;
            time_Tosend = 180;
           }
           
        }
     
        delay(1866);
       
       
       
      }
      if((at_send_check_response("AT+NJS=0", 1000, "AT+NJS=?\r\n")))
      {
         is_join = true;
         Serial.println("device disconnected");
      }

      Serial.print("Loop time: ");
      Serial.println(millis()-start_time);
  }
  else
  {
       Serial.print("No lora module found"); 
       delay(1000);
  }
}
