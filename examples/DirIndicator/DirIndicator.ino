#include <Tle493d.h>

/*Hysteresis Settings for Phi
 * e.g.: Lower Hysteresis Threshold at 3/7 of value and Higher Threshold at 5/7:
 *  HYST_DIV 7
 *  HI_FACTOR 5
 *  LO_FACTOR 3
 */
#define PHI_HYST_DIV 3.0
#define PHI_HYST_HI_FACTOR 2.0
#define PHI_HYST_LO_FACTOR 1.0

//Hysteresis Settings for xr
#define XR_HYST_DIV PHI_HYST_DIV
#define XR_HYST_HI_FACTOR PHI_HYST_HI_FACTOR
#define XR_HYST_LO_FACTOR PHI_HYST_LO_FACTOR

volatile double phiThr;
volatile double phiOffset = 0;
volatile double phiPosThr = 0;
volatile double phiNegThr = 0;
volatile double xrThr;
volatile double xrOffset = 0;
volatile double xrPosThr = 0;
volatile double xrNegThr = 0;
int curPoint = 9;

Tle493d sensor = Tle493d();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1000000);
  
  //If using the MS2Go-Kit: Enable following lines to switch on the sensor
  // ***
  // pinMode(LED2, OUTPUT);
  // digitalWrite(LED2, HIGH);
  // delay(50);
  // ***
  
  
  sensor.begin();
  sensor.disableTemp();
  double phiRU = 14.0; //P9
  double xrRU = 14.69 / 66.04; // P9
  generateCalibData(phiRU, xrRU, 9); 
}

void resetCalib()
{
  phiThr = 0;
  phiOffset = 0;
  phiPosThr = 0;
  phiNegThr = 0;
  xrThr = 0;
  xrOffset = 0;
  xrPosThr = 0;
  xrNegThr = 0;
  double phiRU = 14.0; //P9
  double xrRU = 14.69 / 66.04; // P9
  generateCalibData(phiRU, xrRU, curPoint); 
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor.updateData();
  double z = sensor.getZ();
  int zSign = z < 0 ? -1 : 1;
  z = z * zSign;
  double x = sensor.getX() * zSign;
  double y = sensor.getY() * zSign;
  
  double phi = getPhi(y, z);
  double xr = x/z;
  curPoint = interpretePhi(phi) + 3*interpreteXr(xr);
  Serial.println();
  Serial.print("P");
  Serial.println(curPoint);
  Serial.print("Phi = ");
  Serial.print(phi)  ;
  Serial.print(" ,XR = ");
  Serial.println(xr, 4);
  Serial.print("Thresholds: PhiThr = ");
  Serial.print(phiThr);
  Serial.print(" , XR = ");
  Serial.println(xrThr, 4);
  delay(50);
}

void serialEvent()
{
  if(Serial.available() >= 2)
  {
    char c = Serial.read();
    if(c == 'c')
    {
      int p = Serial.parseInt();
      p = p>9? 9 : p;
      p = p<1? 1 : p;
      sensor.updateData();
      double z = sensor.getZ();
      int zSign = z < 0 ? -1 : 1;
      z = z * zSign;
      double x = sensor.getX() * zSign;
      double y = sensor.getY() * zSign;
      double xr = x/z;
      double phi = atan2(y,z)*180.0/PI;
      generateCalibData(phi, xr, p);
      Serial.print("new cal values for P");
      Serial.println(p);
      delay(5000);
    }
    else if(c == 'r')
    {
      resetCalib();
    }
    while(Serial.available())
      Serial.read();
      
  }
}

double getPhi(double y, double z)
{
  double phi = atan2(y, z)*180.0/PI;
  return phi;
}

void generateCalibData(double phi, double xr, int point)
{
  if(point >= 7 || point <= 3)  //xr Threshold will be set for 1,2,3 or 7,8,9 but not for 4,5,6
  {
    xrThr = abs((xr-xrOffset)/3.0);
    if(point <=3)
      xrPosThr = xrThr;
    else if(point >= 7)
      xrNegThr = -xrThr;
  }
  else if(point == 5)//offset calibration will be done just for 5
  {
    xrOffset = xr;
    phiOffset = phi;
  }
  
  if(point%3 != 2)  //phi Threshold will just be set in case that the lever is left or right (1,3,4,6,7,9)
  {
    phiThr = abs((phi-phiOffset)/3.0);
    if(point % 3 == 1)
      phiNegThr = -phiThr;
    else if(point %3 == 0)
      phiPosThr = phiThr;
  }
}

int interpretePhi(double phi)
{
  double checkNeg = -phiThr;
  if(phiNegThr != 0)
    checkNeg = phiNegThr;
  double checkPos = phiThr;
  if(phiPosThr != 0)
    checkPos = phiPosThr;  

  
  if( (phi-phiOffset) < checkNeg)  //P1,P4,P7
  {
    if(curPoint%3 == 2) //if point was in middle position before
    {
      phiThr = (phiThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; //set lower Hysteresis Threshold active
      phiNegThr = (phiNegThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; 
      phiPosThr = (phiPosThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; 
    }
    return 1;
  }
  else if( (phi-phiOffset) > checkPos)  //P3,P6,P9
  {
    if(curPoint%3 == 2) //if point was in middle position before
    {
      phiThr = (phiThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; //set lower Hysteresis Threshold active
      phiNegThr = (phiNegThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; 
      phiPosThr = (phiPosThr / PHI_HYST_HI_FACTOR) * PHI_HYST_LO_FACTOR; 
    }
    return 3;
  }
  else  //P2,P5,P8
  {
    if(curPoint%3 != 2) //if point wasn't in middle position before
    {
      phiThr = (phiThr / PHI_HYST_LO_FACTOR) * PHI_HYST_HI_FACTOR;  //set upper Hysteresis Threshold active
      phiNegThr = (phiNegThr / PHI_HYST_LO_FACTOR) * PHI_HYST_HI_FACTOR;
      phiPosThr = (phiPosThr / PHI_HYST_LO_FACTOR) * PHI_HYST_HI_FACTOR;
    }
    return 2;
  }
}

int interpreteXr(double xr)
{
  double checkNeg = -xrThr;
  if(phiNegThr != 0)
    checkNeg = xrNegThr;
  double checkPos = xrThr;
  if(xrPosThr != 0)
    checkPos = xrPosThr;  
    
  if( (xr-xrOffset) > checkPos)  //P1, P2, P3
  {
    if(curPoint > 3) //if point was in middle position before
    {
      xrThr = (xrThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;   //set lower Hystersis Threshold active
      xrNegThr = (xrNegThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;
      xrPosThr = (xrPosThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;
    }
    return 0;
  }
  else if( (xr-xrOffset) < checkNeg)  //P7, P8, P9
  {
    if(curPoint < 7) //if point wasn't in middle position before
    {
      xrThr = (xrThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;   //set lower Hystersis Threshold active
      xrNegThr = (xrNegThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;
      xrPosThr = (xrPosThr / XR_HYST_HI_FACTOR) * XR_HYST_LO_FACTOR;
    }
    return 2;
  }
  else    //P4, P5, P6
  {
    if(curPoint < 4 || curPoint > 6) //if point wasn't in middle position before
    {
      xrThr = (xrThr / XR_HYST_LO_FACTOR) * XR_HYST_HI_FACTOR;   //set upper Hystersis Threshold active
      xrNegThr = (xrNegThr / XR_HYST_LO_FACTOR) * XR_HYST_HI_FACTOR;
      xrPosThr = (xrPosThr / XR_HYST_LO_FACTOR) * XR_HYST_HI_FACTOR;
    }
    return 1;
  }
}
