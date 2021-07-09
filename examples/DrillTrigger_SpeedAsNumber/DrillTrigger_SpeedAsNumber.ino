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
  double X_min=0;
  double X_max=0;
  double Z_min=0;
  double Z_max=0;
  Tle493dMagnetic3DSensor.updateData();
  int spe =0;
  double gain_default=1250;
  double z = Tle493dMagnetic3DSensor.getZ(); 
  double x = Tle493dMagnetic3DSensor.getX(); /* *zSign */
  double y = Tle493dMagnetic3DSensor.getY(); 
  Serial.println("_________________________________");
  Serial.println("                     ");
  int zSign = z < 0 ? -1 : 1;
  z = z * zSign;
  
  
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
  sp=(atan2(x*zSign,z)+1)*gain_default;
  if (sp<0) sp=0;
  if (sp>2500) sp=2500;
  sp_int=2501-sp;
  Serial.print("Speed= "); //if at no pull at the trigger is bigger than zero some kind of correction or calibration needs to be implemented
  Serial.println(sp_int);
  delay(1500); 
 

}
