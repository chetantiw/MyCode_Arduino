#include <SimpleModbusMaster.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX

#define baud 9600 //Baud rate for MODBUS RTU
#define timeout 1000 //Maximum time for slave to respond (ms)
#define polling 2000 // Maximum scan rate of master to allow slave to return to idle (ms)
#define retry_count 10 //Maximum retries if slave returns response timeout or error
#define TxEnablePin 2 //Pin to set RS485 interface to transmit or receive (high to Transmit, sets DE and RE high. DE must be high to transmit and RE low to receive)
#define TOTAL_NO_OF_REGISTERS 2 //Used to size the array for returned data from slave to the total number of unsigned 16 bit registers to be read

#define ActiveImport 0x5000

#define ActiveImportAccum 0x5460

#define CurrentVoltage 0x5B00

//Create a packet for each distinct request for data (slave ID, function code, start address and number of registers to be returned) and the total number of packets
enum
{
PACKET1,
TOTAL_NO_OF_PACKETS // leave this last entry
};

unsigned int iCnt = 0;
//Create an array of packets
Packet packets[TOTAL_NO_OF_PACKETS];

//Create an array to hold returned data in its raw form of unsigned integers
unsigned int regs[TOTAL_NO_OF_REGISTERS]; //Array size seems to need to be 1 more than the total number of registers (i.e. 0 to n)

unsigned int OldRegs=0; //Used to store previous value to see if data returned by slave has changed - FOR INITIAL TESTING

//Variables used for time delay - FOR INITIAL TESTING
long previousmillis = 0;
long interval = 10000; ///time in ms to hold led on after value changes
unsigned long currentmillis = millis();

void setup() {
// Initialize packets (pointer to packet, slave device ID, function code constant, address to read (zero based),number of registers to read,subscript of first returned value in regs array
//This would be repeated if there wss more than one packet defined
//On the ABB B21 (set as ID 6) we use function code 3 Read Holding Registers, start address 20483 is the least significant word (16 bits) of the 4-word active import register, read one register and store in the first subscript of the regs array
//The returned value is in 1/100ths of a kWh. To read the whole value we would beed to read four registers starting at 20480 and shift and cast them into a single value. On this meter the most significant word is the first word.
// modbus_construct(&packets[PACKET1], 6, READ_HOLDING_REGISTERS, 20483, 1, 0);
modbus_construct(&packets[PACKET1], 6, READ_INPUT_REGISTERS, ActiveImport, 1, 0);

mySerial.begin(9600);

// Initialize the Modbus comms using the default hardware serial port (Pins D0 and D1) and parameters at the top of the code, packet definitions and array to hold data locally
modbus_configure( (HardwareSerial*) &mySerial, baud, SERIAL_8E1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);

Serial.begin(9600,SERIAL_8E1);
Serial.println("Lets start again...");
Serial.println("id");
Serial.println(packets[0].id);
Serial.println("function");
Serial.println(packets[0].function);
Serial.println("address");
Serial.println(packets[0].address);
Serial.println("data");
Serial.println(packets[0].data);
Serial.println("local_start_address");
Serial.println(packets[0].local_start_address);

}

void loop() {
modbus_update(); //Poll the slave

//Control LED to indicate value has just changed - FOR INITIAL TESTING
currentmillis=millis(); //Store current time
if(regs[0]!=OldRegs){
iCnt++;
}
else {
if(currentmillis - previousmillis >= interval){

char str[80];

sprintf(str , "%d Hello %d %d ", iCnt , regs[0] , OldRegs );
Serial.println(str);
iCnt = 0;

previousmillis=currentmillis;
}
}

//Store the latest value to use as old value on next iteration round loop
OldRegs=regs[0];
}
