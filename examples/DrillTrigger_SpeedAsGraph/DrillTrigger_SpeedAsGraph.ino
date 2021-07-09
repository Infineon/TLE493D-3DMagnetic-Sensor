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
  int spe =0; //Speed =0
  double z = Tle493dMagnetic3DSensor.getZ(); 
  // Serial.println("_________________________________");
  // Serial.println("                     ");
  int zSign = z < 0 ? -1 : 1; //zSign: sign of the perpendicular z- component of the magnet. is used to make the magnet orientation regardless/no meaning
  z = z * zSign; //change sign of speed if needed
  double x = Tle493dMagnetic3DSensor.getX(); /* *zSign */
  double y = Tle493dMagnetic3DSensor.getY();  /* *zSign */
  double sp = -1;
  int sp_int=-1;
  int count=0;
  int number_char=-1;
  int counter=0;

 //** readout & print sensor data
  //Serial.print("zsign = ");
  //Serial.print(zSign);
  //Serial.print(" ; (x= ");
  //Serial.print(x);
  //Serial.print(" ; (y= ");
  //Serial.print(y);
  //Serial.print(") ; z=");
  //Serial.println(z);

  //** Speed calculation
  sp=(atan2(x*zSign,z)+1)*1250; // calculation of the speed sp; zsign used for independency of the magnetic orientation
  if (sp<0) sp=0; // cutting negative speed
  if (sp>2500) sp=2500; //cutting higher speed limit to 2500
  sp_int=2501-sp; //change 
  //Serial.print("Speed= ");
   //Serial.println(sp_int);
   
  //* convert speed into numbers of characters
  number_char=sp_int/25; //calculaion of the requiered number of characters representing the speed
  // Serial.print("Characters for speed = ");
  // Serial.println(number_char);

  //** print out number of characters
  delay(5); 
  Serial.println();
  // missing of automatic cal algorythm
  do { //giving out the required characters representing the speed.
    count=count+1;
  Serial.print("*"); 
} while (count < number_char);
 

}
