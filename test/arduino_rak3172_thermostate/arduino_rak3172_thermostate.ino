#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial ser1(2, 3); // RX, TX
static char recv_buf[128];
static bool is_exist = false;
static bool is_join = false;
static int led = 4;

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
  //  va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    //va_start(args, p_cmd);
   //  p_cmd = va_arg(args, char*); 
    ser1.print(p_cmd);

    Serial.print(p_cmd);
    //Serial.print(p_cmd, args);
   // va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
        return 0;
 
    do
    {
        while (ser1.available() > 0)
        {
            ch = ser1.read();
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
    Serial.println("I am decoding...");
    
    if (p_msg == NULL)
    {
      Serial.println("Received null");
        return;
    }
    char *p_start = NULL;
    char data[128];       // To hold the received bytes as characters
    
    int bytes_len=0;
    p_start = strstr(p_msg, "RX");
    if (p_start && (1 == sscanf(p_start, "RX: \"%s", &data)))
    {
      for (int i=0; i<sizeof(data); i++) {
          if(int(data[i+1])==0) {
            bytes_len = i;
            break;
          }
      }

      // Convert the characters to a byteArray
      int message_len = bytes_len/2+1;
      byte out[message_len];
      auto getNum = [](char c){ return c > '9' ? c - 'A' + 10 : c - '0'; };
      for (int x=0, y=0; x<bytes_len; ++x, ++y)
        out[y] = (getNum(data[x++]) << 4) + getNum(data[x]);
      out[message_len] = '\0';
  
      // Create a JSON document of specified capacity <100> and load the byteArray to it
      StaticJsonDocument<100> doc;
      deserializeJson(doc, out);
  
      // Print the received JSON message by serializing it
      serializeJson(doc, Serial);
      Serial.println();
  
      // Access a specific key value from the JSON formatted message
      Serial.println((const char *)doc["temp"]);
    }
}
 
void setup(void)
{
    Serial.begin(9600);
    ser1.begin(9600);

    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    Serial.print("RAK3172 LORAWAN TEST\r\n");
    delay(8000);

    
     ser1.print("AT\r\n");
     delay(500);
     ser1.print("AT\r\n");
     delay(500);
     
//     if (ser1.available()) 
//     {
//    if(ser1.read() == "OK")
//    {
//      Serial.print("REsponse OK");
//      delay(200);
//     }
    
 // }
    
    if (at_send_check_response("OK", 100, "AT\r\n"))
    {
      is_exist = true;
      at_send_check_response("OK", 1000, "AT+DEVEUI=?\r\n");
      at_send_check_response("OK", 1000, "AT+NWM=1\r\n");
      at_send_check_response("OK", 1000, "AT+NJM=1\r\n");
      at_send_check_response("OK", 1000, "AT+CLASS=C\r\n");
      at_send_check_response("OK", 1000, "AT+BAND=4\r\n");
      at_send_check_response("OK", 1000, "AT+APPKEY=955d7f9afa41c7b15d444b4f8af87fb5\r\n");
  //    at_send_check_response("OK", 1000, "AT+DEVEUI=?\r\n");  //ac1f09fffe054751
  //    at_send_check_response("+PORT: 8", 1000, "AT+PORT=2\r\n");
      delay(200);
    //   Serial.println("  ALL COMMAND CHECK.\r\n");
      is_join = true;
      digitalWrite(4, HIGH);
    }
    else
    {
      is_exist = false;
      Serial.print("No rak3172 module found.\r\n");
    }
}
 
void loop(void)
{
    if (is_exist)
    {
      int ret = 0;
      if (is_join)
      {
        ret = at_send_check_response("+EVT:JOINED", 12000, "AT+JOIN=1:0:10:1\r\n");
      
        if (ret)
            is_join = false; 
        else
        {
            at_send_check_response("OK", 1000, "AT+DEVEUI=?\r\n");
            Serial.print("JOIN failed!\r\n\r\n"); 
            delay(5000);
        }
      }
      else
      {
        
        StaticJsonDocument<200> json;
        json["Tem"] = 27.4;
        json["Hum"] = 4.5;
       // json["flow"] = 12;
        char charArray[64];
        int len = serializeJson(json, charArray);
        char buildBuffer[2] = {0};
        char compositionBuffer[len*3+1] = {0};  // this will hold a string we build

        for (int i = 0; i < len; i++) {
          sprintf( buildBuffer, "%02X ", (uint8_t)charArray[i]);
          compositionBuffer[i*2] = buildBuffer[0];
          compositionBuffer[i*2+1] = buildBuffer[1];
          //strcat( compositionBuffer, buildBuffer);
        }
        
        char cmd[128];
        Serial.print("Sending starts here...");
        sprintf(cmd,"AT+SEND=2:");
        strcat(cmd,compositionBuffer);
        strcat(cmd,"\r\n");
        int br=0;
        do{  delay(2000);
             br++;
             ret = at_send_check_response("+EVT:SEND_CONFIRMED_OK", 10000, cmd);
             if (strstr(recv_buf,"+EVT:RX_1"))
             {
                recv_prase(recv_buf);
             }
             if(br>10)
             {break;}
           }while(ret==0);
           
        delay(5000);
      }
  }
  else
      delay(1000);
}
