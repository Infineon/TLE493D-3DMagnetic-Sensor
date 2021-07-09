// this examples is to be used with processing/sine_generator.pde
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
}

void loop() {
  Tle493dMagnetic3DSensor.updateData();
  Serial.print(Tle493dMagnetic3DSensor.getAzimuth());
  Serial.print("\t");
  if(Tle493dMagnetic3DSensor.getAzimuth()>0)
    Serial.println(Tle493dMagnetic3DSensor.getPolar());
  else
    Serial.println(-Tle493dMagnetic3DSensor.getPolar());
}
