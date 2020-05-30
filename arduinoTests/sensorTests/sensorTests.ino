
////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT 2020 Ventilators In Peoria (VIP)
// See license.txt in for more info
//
// THE SOFTWARE IS PROVIDED "AS IS". IT IS NOT FIT FOR ANY PARTICULAR PURPOSE,
// CONFORMS TO NO REGULATORY BODY REQUIREMENTS, AND IS GENERALLY NOT SAFE
// FOR ANY APPLICATION INVOLVING HUMAN LIFE. USE AT YOUR OWN RISK.
////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>

//////////////////////////////////////////////////////
// BME280 Sensor over I2C
// Amazon Module is on address 0x76
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
#define BME280_ADDRESS 0x76
Adafruit_BME280 bme; 

//////////////////////////////////////////////////////
// Loop Timing
#define MAIN_LOOP_TS_MS 30
long loopStartTime_us = 0;
double cpuLoad = 0;
long loopOverruns = 0;
unsigned long loopCounter = 0;

void markLoopStart(){
  loopStartTime_us = micros();
}

void markLoopEnd(){
  loopCounter++;
  long delayDur = (long)(MAIN_LOOP_TS_MS * 1000L) - (micros() - loopStartTime_us);
  if(delayDur > 0){
    if(delayDur > 65529){
      delay(delayDur/1000);
    } else {
       delayMicroseconds((unsigned int)delayDur);
    }
    cpuLoad = 100.0 * ( 1.0 - ((double)delayDur/1000.0)/(double)MAIN_LOOP_TS_MS);
  } else {
    //Error! Loop time overrun
    loopOverruns++;
    cpuLoad = 100.0;
  }
}

//////////////////////////////////////////////////////
// Communication with RaspberryPi

void sendPiData(float val1, float val2, float val3){
  char sendPacket[75];
  sprintf(sendPacket, "%d.%02d,%d.%02d,%d.%02d", //sprintf doesn't support floats, so do some tricks
  (int)val1, (int)(val1*100)%100,
  (int)val2, (int)(val2*100)%100,
  (int)val3, (int)(val3*100)%100);
  Serial.println(sendPacket);
}


//////////////////////////////////////////////////////
// Main Code

#define CH1_PIN A0
#define CH2_PIN A1
#define CH3_PIN A2

void setup() {
  // Init serial connection for Pi data
  Serial.begin(115200);
  while(!Serial);    // time to get serial running

  //Init BME280 sensor
  bme.begin(BME280_ADDRESS);
}


void loop() {
  markLoopStart();

  float ch1Val = analogRead(CH1_PIN);
  float ch2Val = bme.readTemperature();
  float ch3Val = cpuLoad;

  sendPiData(ch1Val, ch2Val, ch3Val);

  markLoopEnd();
}
