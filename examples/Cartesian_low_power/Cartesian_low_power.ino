#include <Tle493d_w2b6.h>

/**
* This example demonstrates the use of low poewr mode of sensor TLE493d-W2B6
*/


Tle493d_w2b6 Tle493dMagnetic3DSensor = Tle493d_w2b6(Tle493d::LOWPOWERMODE);
void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  //If using the MS2Go-Kit: Enable following lines to switch on the sensor
  // ***
  // pinMode(LED2, OUTPUT);
  // digitalWrite(LED2, HIGH);
  // delay(50);
  // ***
  
  
  Tle493dMagnetic3DSensor.begin();
  // The highest adjustable range is used for all three directions, i.e., within half of the total value range INT is disabled 
  // this has no effect on the TLE493d-A2B6 sensor
  Tle493dMagnetic3DSensor.setWakeUpThreshold(1,-1,1,-1,1,-1);

  //The update rate is set to 3 (fastest is 0 and slowest is 7)
  Tle493dMagnetic3DSensor.setUpdateRate(3);
}

void loop() {
  Tle493dMagnetic3DSensor.updateData();

  Serial.print(Tle493dMagnetic3DSensor.getX());
  Serial.print(" ; ");
  Serial.print(Tle493dMagnetic3DSensor.getY());
  Serial.print(" ; ");
  Serial.println(Tle493dMagnetic3DSensor.getZ());

  delay(10);
}
