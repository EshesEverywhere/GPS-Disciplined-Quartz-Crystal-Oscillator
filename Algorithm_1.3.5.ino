#include <Wire.h>
#include <stdint.h>

const int vcxoPin = 9;
const int gpsPin   = 2;

volatile int64_t freq = 0;
volatile int sampleCount;
volatile int prevSampleCount;
volatile int64_t dev;
volatile int64_t adjust;
volatile int64_t prevAdjust;
uint8_t packet1[3];
uint8_t packet2[3];
volatile int state;

void gpsInterrupt()  {
  freq = GPT1_CNT;
  GPT1_CR |= GPT_CR_EN;
  if(sampleCount == 0){
    state = 2;
  }
  Serial.println(freq);
  sampleCount = sampleCount - 1;
}

void calcTransmit(int64_t inputFreq){
  //1 second sampling
  Serial.println("Calculating");
  if((19999700 <= inputFreq) && (inputFreq <= 20000300)){
    dev = inputFreq - 20000000;
    adjust = (dev)*((pow(2,13))/75);
    if(abs(adjust) > 5400){
      sampleCount = 1;
      prevSampleCount = 1;
    }
    else if((abs(adjust) <= 5400) && (abs(adjust) > 540)){
      sampleCount = 10;
      prevSampleCount = 10;
    }
    else if((abs(adjust) <= 540)){
      sampleCount = 100;
      prevSampleCount = 100;
    }
    adjust = prevAdjust - adjust;
    prevAdjust = adjust;
    setVoltage16(adjust);
  }
  //10 second sampling
  else if((199997000 <= inputFreq) && (inputFreq <= 200003000)){
    dev = inputFreq - 200000000;
    adjust = (dev)*((pow(2,13))/750);
    if(abs(adjust) > 5400){
      sampleCount = 1;
      prevSampleCount = 1;
    }
    else if((abs(adjust) <= 5400) && (abs(adjust) > 540)){
      sampleCount = 10;
      prevSampleCount = 10;
    }
    else if((abs(adjust) <= 540)){
      sampleCount = 100;
      prevSampleCount = 100;
    }
    adjust = prevAdjust - adjust;
    prevAdjust = adjust;
    setVoltage16(adjust);
  }
  //100 seconds
  else if ((1999970000 <= inputFreq) && (inputFreq <= 2000030000)){
    dev = inputFreq - 2000000000;
    adjust = (dev)*((pow(2,13))/7500);
    if(abs(adjust) > 5400){
      sampleCount = 1;
      prevSampleCount = 1;
    }
    else if((abs(adjust) <= 5400) && (abs(adjust) > 540)){
      sampleCount = 10;
      prevSampleCount = 10;
    }
    else if((abs(adjust) <= 540)){
      sampleCount = 100;
      prevSampleCount = 100;
    }
    adjust = prevAdjust - adjust;
    prevAdjust = adjust;
    setVoltage16(adjust);
  }
  else{
    Serial.println("Frequency Out Of Adjustment Range");
    sampleCount = 1;
  }
  Serial.println(prevAdjust);
  freq = 0;
  GPT1_CR |= GPT_CR_SWR;
  GPT1_SR = 0x3F; // clear all prior status
  GPT1_CR =  GPT_CR_CLKSRC(3); 
}

void startGPT(){
  CCM_CSCMR1 &= ~64;
  CCM_CBCDR &= ~512;
  CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON) ;        // enable GPT1 module
  GPT1_CR = 0;
  GPT1_SR = 0x3F; // clear all prior status
  GPT1_CR =  GPT_CR_CLKSRC(3);                     // | GPT_CR_FRR ;    // 3 external clock
  *(portConfigRegister(25)) = 1;                   // ALT 1  Clock pin is 25
  //GPT1_CR |= GPT_CR_EN;
  GPT1_CR &= ~GPT_CR_EN;
}


void setVoltage16(int inputTransmit) {
  packet1[0] = (0x40);
  packet1[1] = (inputTransmit/256);
  packet1[2] = (0x00);
  packet2[0] = (0x40);
  packet2[1] = (inputTransmit%256);
  packet2[2] = (0x00);
  Wire.beginTransmission(0x62);
  Wire.write(packet1[0]);
  Wire.write(packet1[1]);
  Wire.write(packet1[2]);
  Wire.endTransmission();
  Wire.beginTransmission(0x63);
  Wire.write(packet2[0]);
  Wire.write(packet2[1]);
  Wire.write(packet2[2]);
  Wire.endTransmission();
}

//Method to display to the LCD

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.begin(0x62);
  Wire.begin(0x63);
  Wire.setClock(400000);
  attachInterrupt(digitalPinToInterrupt(gpsPin), gpsInterrupt, RISING);
  startGPT();
  adjust = 32768;
  prevAdjust = 32768;
  setVoltage16(adjust);
  state = 1;
  sampleCount = 1;
}

void loop() {
  switch (state){
    case 1:
      break;
    case 2:
      calcTransmit(freq);
      //display to user 
      state = 1;
      break;
  }
}