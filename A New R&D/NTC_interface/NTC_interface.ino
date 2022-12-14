
float Vin=5;     // [V]        
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
#define temp_probe 33




void setup() {

   Serial.begin(9600);
   pinMode(33, INPUT);
   delay(200);
   Serial.println("NTC demo....");

  //parâmetros
  beta=(log(RT1/RT2))/((1/Tone)-(1/Ttwo));
//  Serial.println("beta: ");
//  Serial.println(beta);
  Rinf=R0*exp(-beta/Tzero);
//  Serial.println("Rinf: ");
//  Serial.println(beta);
}

void loop()
{
  Vout=Vin*((float)(analogRead(temp_probe))/4096.0); // calc for ntc
  Rout=(Rt*Vout/(Vin-Vout));
//  Serial.println("Vout: ");
//  Serial.println(Vout);
//  Serial.println("Rout: ");
//  Serial.println(beta);

  TempK=(beta/log(Rout/Rinf)); // calc for temperature
  TempC=TempK-273.15;
//  Serial.println("TempK: ");
//  Serial.println(TempK);


    Serial.println("Temperature: ");
    
    Serial.println(TempC);
    
    Serial.println("C  ");
    Serial.println((TempC * 9)/5 + 32); // C to F
   
    Serial.println("F");
    delay(1200);
}
