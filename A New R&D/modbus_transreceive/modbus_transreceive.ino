#include <ModbusRtu.h>
#include <SoftwareSerial.h>

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0); // this is master and RS-232 or USB-FTDI via software serial

/**
 * This is an structe which contains a query to an slave device
 */
modbus_t telegram;

unsigned long u32wait;

SoftwareSerial mySerial(5, 6);//Create a SoftwareSerial object so that we can use software serial. Search "software serial" on Arduino.cc to find out more details.

void setup() {
  Serial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.begin( &mySerial, 9600 ); // begin the ModBus object. The first parameter is the address of your SoftwareSerial address. Do not forget the "&". 9600 means baud-rate at 9600
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
  Serial.println("modbus_transreceive");
  delay(300);
}

void loop() {
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
  Serial.println("test case1");
  delay(300);
    telegram.u8id = 0x01; // slave address
    telegram.u8fct = 0x04; // function code (this one is registers read)
    telegram.u16RegAdd = 0x03E8; // start address in slave
    telegram.u16CoilsNo = 0x05; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
//    Serial.println(telegram);
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 2000; 
      Serial.println("test case2");
      delay(300);
        Serial.println(au16data[0]);//Or do something else!
        Serial.println(au16data[1]);//Or do something else!
        Serial.println(au16data[2]);//Or do something else!
        Serial.println(au16data[3]);//Or do something else!
        Serial.println(au16data[4]);//Or do something else!
        Serial.println(au16data[5]);//Or do something else!
        Serial.println(au16data[6]);//Or do something else!
        Serial.println(au16data[7]);//Or do something else!
        Serial.println(au16data[8]);//Or do something else!
        Serial.println(au16data[9]);//Or do something else!
        Serial.println(au16data[10]);//Or do something else!
        Serial.println(au16data[11]);//Or do something else!
        Serial.println(au16data[12]);//Or do something else!
        Serial.println(au16data[13]);//Or do something else!
        Serial.println(au16data[14]);//Or do something else!
        Serial.println(au16data[15]);//Or do something else!
    }
    break;
  }
}
