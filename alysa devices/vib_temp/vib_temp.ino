// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>
#include <SPI.h> 
#include <TinyLoRa.h>             // include libraries

String deviceID = "\"VB000031\"";

// Net  Session Key (MSB) 1a    2b    3c    8a    b4    bf    c6    23    68    1d    38    a3    0d    e4    4e   c0
uint8_t NwkSkey[16] = { 0x1a, 0x2b, 0x3c, 0x8a, 0xb4, 0xbf, 0xc6, 0x23, 0x68, 0x1d, 0x38, 0xa3, 0x0d, 0xe4, 0x4e, 0xc0 };

// App Session Key (MSB)  99    98    97    06    f9    49    29    d6    f1    a7    67    4a    d1    25    e1    cb 
uint8_t AppSkey[16] = { 0x99, 0x98, 0x97, 0x06, 0xf9, 0x49, 0x29, 0xd6, 0xf1, 0xa7, 0x67, 0x4a, 0xd1, 0x25, 0xe1, 0xcb };

// Device Address (MSB)  11   22   44   66
uint8_t DevAddr[4] = { 0x43,0x54,0x65,0x76 };

const unsigned long sendInterval = 30000;



unsigned long frame_counter = 0;


TinyLoRa lora = TinyLoRa(2, 10, 9);
Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  // Initialize LoRa
    Serial.print("Starting LoRa...");
    // define multi-channel sending
    lora.setChannel(MULTI);
    // set datarate
    lora.setDatarate(SF7BW125);
    if(!lora.begin())
    {
      Serial.println("Failed");
      Serial.println("Check your radio");
      while(true);
    }
    
   
    Serial.println("LoRa init succeeded.");
    Serial.println("LoRa Envorimental sense");
    Serial.println();
    
   
    delay(2000);
  Serial.println(" MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);

   if (runEvery(sendInterval))
  {
    String sendStr = "{";
    sendStr += "\"";
    sendStr += "NodeID\":";
    sendStr += deviceID;
    sendStr += ",";
    
    sendStr += "\"";
    sendStr += "Temperature\":";
    sendStr += String(temp.temperature);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "x_axis_vib\":";
    sendStr += String(a.acceleration.x);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "y_axis_vib\":";
    sendStr += String(a.acceleration.y);
    sendStr += ",";

    sendStr += "\"";
    sendStr += "z_axis_vib\":";
    sendStr += String(a.acceleration.z);
    sendStr += "}";

    Serial.println("Sending LoRa Data...");   
   Serial.println(sendStr);
   char data[sendStr.length()+1];
   sendStr.toCharArray(data,sendStr.length()+1);
   for(int i=0;i<sizeof(data);i++)
   {
      Serial.print(data[i]);
   }
   Serial.println();
   lora.sendData(data, sizeof(data), lora.frameCounter);
   Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
   lora.frameCounter++;
    

    Serial.println("Send Message!");
  }
   
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
