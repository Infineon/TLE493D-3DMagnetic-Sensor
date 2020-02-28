/**
* For basic setup just create a Tle493d() object. If you want to use the wake up mode please use Tle493d_w2b6(). Also
* the setUpdateRate() method is slightly different for different variants
*/


#include <Tle493d.h>

Tle493d Tle493dMagnetic3DSensor = Tle493d();

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Tle493dMagnetic3DSensor.begin();
  Tle493dMagnetic3DSensor.enableTemp();
  double X_min=0;
  double X_max=0;
  double Z_min=0;
  double Z_max=0;
}

void loop() {
  Tle493dMagnetic3DSensor.updateData();
  int spe =0;
  double gain_default=1250;
  double z = Tle493dMagnetic3DSensor.getZ(); 
  Serial.println("_________________________________");
  Serial.println("                     ");
  int zSign = z < 0 ? -1 : 1;
  z = z * zSign;
  
  double x = Tle493dMagnetic3DSensor.getX(); /* *zSign */
  //cal for gain. check for new x extrema
  if (x<X_min) {
  X_min=x)
  }
  if (x>X_max) {
  X_max=x)
  }
  // ----
  
  double y = Tle493dMagnetic3DSensor.getY();  /* *zSign */
  //calibration for gain. check for new y extrema
  if (y<Y_min) {
  Y_min=y)
  }
  if (y>Y_max) {
  Y_max=y)
  }
  
  
  double sp = -1;
  int sp_int=-1;
  int number_char=-1;
  int counter=0;

  Serial.print("zsign = ");
  Serial.print(zSign);
  Serial.print(" ; (x= ");
  Serial.print(x);
  Serial.print(" ; (y= ");
  Serial.print(y);
  Serial.print(") ; z=");
  Serial.println(z);
  sp=(atan2(x*zSign,z)+1)*gain;
  if (sp<0) sp=0;
  if (sp>2500) sp=2500;
  sp_int=2501-sp;
  Serial.print("Speed= ");
  Serial.println(sp_int);
  number_char=sp_int/50;
  Serial.print("Characters for speed = ");
  Serial.println(number_char);
  delay(500); 
 

}
