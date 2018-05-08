#include <Tle493d_w2b6.h>

Tle493d_w2b6 Tle493dMagnetic3DSensor = Tle493d_w2b6();
void setup() {
  Serial.begin(9600);
  while (!Serial);
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

