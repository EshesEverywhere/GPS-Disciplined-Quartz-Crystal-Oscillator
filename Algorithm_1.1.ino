#include <FreqCount.h>
#include <Wire.h>

const int VCXO_PIN = 9;
const int GPS_PPS_PIN   = 2;
volatile unsigned long cnt = 0;
unsigned long prv_cnt = 0;
volatile bool run = LOW;
volatile bool calcNeeded = LOW;
volatile int state;
volatile int dev = 0;
volatile int prev_dev = 0;
volatile int transmit = 8191;

void GPS_interrupt()  {
  run = !run;
  if(!run){
    prv_cnt = cnt;
    Calc_Adjustment();
  }
  cnt = 0;
}

void Calc_Adjustment()  {
    dev = prv_cnt - 20000000;
    dev = (dev)*2048/75;
    transmit = prev_dev+dev;
//    decimalToBinary(transmit);
    Serial.print("Dev is: ");
    Serial.println(dev);
    Serial.print("Transmit is: ");
    Serial.println(transmit);
    Serial.print("Count is: ");
    Serial.println(prv_cnt);
    Serial.print("Time is: ");
    Serial.println(millis());
    Wire.endTransmission(1);
    Wire.beginTransmission(1);
    Wire.write(transmit);
    prev_dev = dev;
}

void setup() {

  Serial.begin(9600);

  Wire.begin();

  pinMode(VCXO_PIN, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), GPS_interrupt, RISING);

  FreqCount.begin(1);

  Wire.beginTransmission(1);
  Wire.write(transmit);
}

void loop() {
  while(run){
    if (FreqCount.available()){
     cnt = cnt + FreqCount.read();
   }
  }
}
