
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
// Watchdog setup and configuration
#include <avr/wdt.h>

void wdog_start(){
  wdt_enable(WDTO_250MS);
}

void wdog_reset(){
  wdt_reset();
}

// Watchdog Reset Handler
ISR(WDT_vect)
{
  // TODO - write to EEPROM? Something to track the fact we are resetting unexpectedly
}

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
#define MAIN_LOOP_TS_MS 50
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
// Siargo Ltd fs6122 sensor
// see https://www.servoflo.com/download-archive/data-sheets/254-mass-flow-vacuum-sensors/1220-fs6122-datasheet
// and https://www.servoflo.com/download-archive/application-notes/212-mass-flow/1256-fs-i2c-communication
// and https://www.digikey.com/product-detail/en/siargo-ltd/FS6122-250F250-0P0-0/2257-FS6122-250F250-0P0-0-ND/11657804?utm_adgroup=Siargo%20Ltd&utm_source=google&utm_medium=cpc&utm_campaign=Shopping_DK%2BSupplier_Other&utm_term=&utm_content=Siargo%20Ltd&gclid=EAIaIQobChMIkJmP45_c6QIVCtvACh1LvwBoEAYYASABEgL9X_D_BwE
#define FS6122_ADDRESS 0x01 //Default
#define FS6122_FILTER_DEPTH 32 //0-128
#define FS6122_CMD_WRITE_FILTER_DEPTH byte(0x0B)
#define FS6122_CMD_CAL_FLOW_RATE byte(0x1C)
#define FS6122_CMD_CAL_PRESSURE byte(0x24)
#define FS6122_CMD_READ_FLOW_RATE byte(0x83)

void fs6122_init(){
  Wire.begin();

  //Configure filter depth
  Wire.beginTransmission(FS6122_ADDRESS);
  Wire.write(FS6122_CMD_WRITE_FILTER_DEPTH);
  Wire.write(byte(FS6122_FILTER_DEPTH));
  Wire.endTransmission();
}

void fs6122_zeroFlowCal(){
  // Must be done when there is guarnteed zero flow in the pipe
  // This function blocks for up to a second, and should not be done in periodic tasking
  Wire.beginTransmission(FS6122_ADDRESS);
  Wire.write(FS6122_CMD_CAL_FLOW_RATE);
  Wire.write(byte(0xFF)); //arbitrary byte
  Wire.endTransmission();
  delay(300);

  Wire.beginTransmission(FS6122_ADDRESS);
  Wire.write(FS6122_CMD_CAL_PRESSURE);
  Wire.write(byte(0xFF)); //arbitrary byte
  Wire.endTransmission();
  delay(300);
}

float fs6122_readPressure_SmlpM(){
  long reading = 0;

  Wire.beginTransmission(FS6122_ADDRESS);
  Wire.write(FS6122_CMD_READ_FLOW_RATE);
  Wire.endTransmission();

  Wire.requestFrom(FS6122_ADDRESS, 4); 
  
  if (4 <= Wire.available()) { // if two bytes were received
    for(int byteIdx = 0; byteIdx < 4; byteIdx++){
      reading = reading << 8;    // shift high byte to be high 8 bits
      reading |= Wire.read(); // receive low byte as lower 8 bits
    }
    return (float)reading;
  } else {
    //ERROR - sensor did not return data. TODO - return something meaningful
    return -42;
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

  //Init Flowmeter
  fs6122_init();
  fs6122_zeroFlowCal();

  wdog_start();
}


void loop() {
  markLoopStart();

  float ch1Val = fs6122_readPressure_SmlpM();
  float ch2Val = bme.readTemperature();
  float ch3Val = cpuLoad;

  sendPiData(ch1Val, ch2Val, ch3Val);


  wdog_reset();
  markLoopEnd();
}
