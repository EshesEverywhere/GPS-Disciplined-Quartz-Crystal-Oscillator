#include <FreqCount.h>
#include <Wire.h>

const int VCXO_PIN = 9;
const int GPS_PPS_PIN   = 2;
volatile unsigned long freq = 0;
volatile int sampleTimes;
volatile bool sample = LOW;
volatile int dev = 0;
volatile int transmit = 2048;
volatile int prevTransmit = 2048;
uint8_t packet[3];

void GPS_interrupt()  {
  sample = !sample;
  if(!sample){
    dev = freq - 20000000;
    dev = (dev)*2048/75;
    transmit = prevTransmit - dev;
    prevTransmit = transmit;
    setVoltage(transmit);
    Serial.println(transmit);
    Serial.println(freq);
    freq = 0;
  }
 // if(sampleTimes = 0){
 //     sample = LOW;
 // }
 // sampleTimes = sampleTimes - 1;
}

void setVoltage(int inputTransmit) {
  packet[0] = (0x40);
  packet[1] = (inputTransmit/16);
  packet[2] = (inputTransmit%16) <<4;
  Wire.beginTransmission(0x62);
  Wire.write(packet[0]);
  Wire.write(packet[1]);
  Wire.write(packet[2]);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Booting Up");
  Wire.begin();
  Wire.begin(0x62);
  Wire.setClock(400000);
  pinMode(VCXO_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), GPS_interrupt, RISING);
  FreqCount.begin(1);
  Wire.beginTransmission(0x62);
  setVoltage(transmit);
 // sampleTimes = 1;
}

void loop() {
  while(sample){
    if (FreqCount.available()){
     freq = freq + FreqCount.read();
   }
  }
}
