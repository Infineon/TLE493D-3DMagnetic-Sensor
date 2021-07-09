/**
* For basic setup just create a Tle493d() object. If you want to use the wake up mode please use Tle493d_w2b6(). Also
* the setUpdateRate() method is slightly different for different variants
*/


#include <Tle493d.h>

Tle493d Tle493dMagnetic3DSensor = Tle493d();

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
  Tle493dMagnetic3DSensor.enableTemp();
}

void loop() {
  Tle493dMagnetic3DSensor.updateData();

  Serial.print(Tle493dMagnetic3DSensor.getX());
  Serial.print(" ; ");
  Serial.print(Tle493dMagnetic3DSensor.getY());
  Serial.print(" ; ");
  Serial.println(Tle493dMagnetic3DSensor.getZ());
  
  delay(500);
}

