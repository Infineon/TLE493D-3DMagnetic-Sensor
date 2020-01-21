### Instruction Direction Indicator

**Mechanics Description:** The Direction Indicator has nine dedicated positions using a 3 x 3 matrix. They are numbered from 1 to 9 on the backside. The Magnet can be moved by a small needle closer to the sensor. A hole is on the opposite site. The AddOn needs to be mounted on a 3DKit2Go or a shield2Go. The magnet shall be above the sensor.
[![DirectionIndicator2Go](https://www.infineon.com/export/sites/default/media/products/Sensors/Direction-indicator-2GO.jpg_33495184.jpg "DirectionIndicator2Go")](https://www.infineon.com/export/sites/default/media/products/Sensors/Direction-indicator-2GO.jpg_33495184.jpg "DirectionIndicator2Go")

The 3D print files can be found [here](https://www.infineon.com/cms/en/tools/landing/infineon-for-makers/kits-2go/ "here") in the ***Do-it-yourself*** section. (coming soon)
Following magnet is[ used](https://www.supermagnete.de/data_sheet_S-05-05-N.pdf " used") for this add on.

------------


**Code & Calibration:**
By running the code, the position is displayed. The code has pre-calibration parameters implemented. Unfortunately they do not work 100% properly for a detection. In order to gain the individual calibration parameters of your device please
1) Move the stick to position 1, insert "c1" and press return 
  --> New calibration parameters will be calculated and displayed.
2) Move the stick to position 9, insert "c9" and press return.
--> New calibration parameters will be calculated and displayed.

Now the device is ready to use. The serial monitor indicates the position.
Note: The calibration parameters will change by moving the magnet closer/more far to/from the sensor.
- For further information about the 3D sensor please click [here](https://www.infineon.com/cms/en/product/promopages/sensors-2go/#3d-magnetic-sensor-2go "here").
