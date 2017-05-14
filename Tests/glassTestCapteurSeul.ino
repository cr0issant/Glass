
// sketch to read MPX5700AP or MPX5700GP pressure sensor
// written by Dan Cunningham
// released to public for free use

#include <Wire.h>
#include <SoftwareSerial.h>
#include <Nextion.h>
SoftwareSerial nextionSerial(10, 11); // RX, TX

Nextion nex(nextionSerial, 9600);

int pressureSensor = A0; // Set MAP sensor input on Analog port 0

// ATM is your average barometric pressure in HPA. We will need to subtract this later.
// To mesaure it, this is the value of the pressure variable below when nothing is attached to the sensor.

//float atm = 101.1174; // supposedly what ATM should be for where I live
//float atm = 156.5593; // this is what my sensor read ATM to be and the value I used when I had MPX5700AP
float atm = 1.56; // using 0 for this since I now have a MPX5700GP
int millibar = 0;
int millibar2 = 0;

void setup(){
  
  Serial.begin(9600); // Open serial port
  nextionSerial.begin(9600);
  nex.init();
}

void loop()
{

 // Serial.print("Pression en BAR:\t");
 // Serial.println(kPa/100, 2);   // convert to PSI
 millibar2 = map(CheckPression ( pressureSensor, atm ),-1000, 3000, 0, 120);
 nextionSerial.print("z0.val=");
 nextionSerial.print( millibar2 );
 nextionSerial.write(0xff);
 nextionSerial.write(0xff);
 nextionSerial.write(0xff); 
 
 nextionSerial.print("z1.val=");
 nextionSerial.print( CheckPression ( pressureSensor, atm ) );
 nextionSerial.write(0xff);
 nextionSerial.write(0xff);
 nextionSerial.write(0xff); 
  
 
  delay(1000);
  
}

int CheckPression ( const int CapteurPression1, float atm )
{

  int pressureSensorRaw = analogRead(CapteurPression1); //Reads the sensor raw value on analog port 0
  float pressureSensorVoltage = pressureSensorRaw * (5.0 / 1023.0);  // convert the raw reading to voltage
  float bar = (pressureSensorVoltage + 0.2) * 7/4.5;
  bar = bar - atm;
  int millibar = (int) ( bar * 1000 );
  return millibar;
}


