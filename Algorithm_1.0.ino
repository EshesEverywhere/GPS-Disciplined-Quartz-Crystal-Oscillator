#include <FreqCount.h>
#include <Wire.h>

const int VCXO_PIN = 9;
const int GPS_PPS_PIN   = 2;
volatile unsigned long cnt = 0;
unsigned long prv_cnt = 0;
volatile bool run = LOW;
volatile int state;
volatile int dev = 0;

void GPS_interrupt()  {
  run = !run;
  if(!run){
    prv_cnt = cnt;
    dev = prv_cnt - 20000000;
    dev = (dev+300)*2048/75;
    Serial.print("Dev is: ");
    Serial.println(dev);
    Serial.print("Count is: ");
    Serial.println(prv_cnt);
    Serial.print("Time is: ");
    Serial.println(millis());
    Wire.beginTransmission(1);
    Wire.write(dev);
  }
  cnt = 0;
}

void Calc_Adjustment()  {
  dev = prv_cnt - 20000000;
  dev = (prv_cnt+300)*2048/75;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(VCXO_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), GPS_interrupt, RISING); 
  FreqCount.begin(1);
}

void loop() {
  while(run){
    if (FreqCount.available()){
     cnt = cnt + FreqCount.read();
   }
  }
}
