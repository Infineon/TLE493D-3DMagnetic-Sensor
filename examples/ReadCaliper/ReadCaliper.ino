#include <Tle493d.h>
#include <Tle493d_a2b6.h>
#include <Tle493d_w2b6.h>

#define CLK 9
#define DATA 8
#define RQ 4

#define SAMPLE 5

volatile uint8_t raw[13];
volatile int bitIndex = 0;
volatile int nibbleIndex = 0;
volatile bool readingFinished = false;

long timer;

Tle493d_w2b6 Tle493dMagnetic3DSensor = Tle493d_w2b6(Tle493d::MASTERCONTROLLEDMODE);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(1000000);

  
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT_PULLUP);
  pinMode(RQ, OUTPUT_OPENDRAIN);

  Tle493dMagnetic3DSensor.begin();
  Tle493dMagnetic3DSensor.setUpdateRate(3);
  attachInterrupt(digitalPinToInterrupt(CLK),clkLow, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  float x=0,y=0,z=0;
  
  
  
  digitalWrite(RQ,LOW);
  while(!readingFinished);
  float value = raw[5]*100000 + raw[6]*10000 + raw[7]*1000 + raw[8]*100 + raw[9]*10 + raw[10];
  value *= pow(10.0,-1.0*raw[11]);
  if(raw[4])
  {
    value *= -1.0;
  }
  if(raw[12])
  {
    value *= 25.4;  //if inch: calculate to mm
  }

  for(int i = 0; i < SAMPLE; i++)
  {
    Tle493dMagnetic3DSensor.updateData();
    x += Tle493dMagnetic3DSensor.getX();
    y += Tle493dMagnetic3DSensor.getY();
    z += Tle493dMagnetic3DSensor.getZ();
  }

  x /= 2*SAMPLE;
  y /= 2*SAMPLE;
  z /= 2*SAMPLE;
  Serial.print(value);
  Serial.print("\t");
  Serial.print((x+Tle493dMagnetic3DSensor.getX())/2.0);
  Serial.print("\t");
  Serial.print((y+Tle493dMagnetic3DSensor.getY())/2.0);
  Serial.print("\t");
  Serial.println((z+Tle493dMagnetic3DSensor.getZ())/2.0);

  readingFinished = false;
}

void clkLow()
{
  if(bitIndex == 0) //first element of this nibble
  {
    raw[nibbleIndex] = 0;  //reset nibble to 0
  }
  raw[nibbleIndex] |= digitalRead(DATA)<<bitIndex;
  bitIndex = (bitIndex+1)%4;
  if(bitIndex == 0)
    nibbleIndex++;
  
  if(nibbleIndex >= 13)
  {
    nibbleIndex = 0;
    readingFinished = true;
    digitalWrite(RQ, HIGH);
  }
}
