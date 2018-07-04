/**
 *  @page The Arduino Library for Infineon's Magnetic 3D Sensor TLE493D-W2B6
 *  @section introduction Introduction
 * 	The TLE493D-W2B6 sensor family comes with I2C interface and wake-up function. This sensor family TLE493D offers
 * 	accurate three dimensional sensing with extremely low-power consumption.
 *
 *  @image latex "TLE493DW2B6.jpg"
 *
 *	@section main_features Main Features
 *
 *	@subsection interrupts Interrupts
 * 	Interrupts can be sent from the sensor to the microcontroller to notify the completion of a measurement and its ADC
 * 	conversion. Values read directly after interrupts are guaranteed to be consistent. Without interrupts values read might
 * 	be stale.
 *
 * 	@subsection collision_avoidance Collision Avoidance and Clock Stretching
 *	In case of a collision, the sensor interrupt disturbs the I2C clock, causing an additional SCL pulse which shifts the
 *	data read out by one bit. If collision avoidance is activated, the sensor monitors the start/stop conditions, and suppresses
 *	interrupts in between.
 *
 *	When interrupts are disabled, this feature becomes clock stretching, that is, the data read out only starts after the ADC conversion is
 *	finished. Thus it can be avoided that during an ADC conversion old or corrupted measurement results are read out, which
 *	may occur when the ADC is writing to a register while this is being read out by the microcontroller. When clock stretching
 *	is enabled, the sensor pulls the SCL line down during ongoing ADC conversions, reading of sensor registers or the transmission
 *	of valid ACKs.
 *
 *	Two register bits (CA and INT) work together for different configurations. APIs to modify these two bits are not offered
 *	since they have to be set according to different operating modes (master controlled mode, low power mode and fast mode)
 *	for the sensor to work.
 *  
 *
 *  @subsection wake_up Wake Up Mode
 *	Wake up mode is intended to be used with low power mode or fast mode. This mode disables interrupts within a user-specified
 *	range, so that interrupts are generated only when relevant data are available.
 */

#ifndef TLE493D_H_INCLUDED
#define TLE493D_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>
#include "./util/BusInterface.h"
#include "./util/Tle493d_conf.h"

typedef enum Tle493d_Error
{
	TLE493D_NO_ERROR = 0,
	TLE493D_BUS_ERROR = 1,
	TLE493D_FRAME_ERROR = 2
} Tle493d_Error_t;

class Tle493d
{
  public:
	enum TypeAddress_e
	{
		TLE493D_A0 = 0x35,
		TLE493D_A1 = 0x22,
		TLE493D_A2 = 0x78,
		TLE493D_A3 = 0x44
	};

	/**
	 * @enum TypeAddress_e Defines four types of the sensor family which are supported by this library and their corresponding addresses
	 * 		 The addresses can be concatenated with 0 or 1 for reading or writing
	 */

	enum AccessMode_e
	{
		LOWPOWERMODE = 0,
		MASTERCONTROLLEDMODE = 1,
		FASTMODE = 3,
	};
	/**
	 * @enum AccessMode_e Enumerates the three available modes; number 2 is reserved
	 *		 In low power mode cyclic measurements and ADC-conversions are carried out with a update rate; the wake-up function is already configured for this mode, so that
	 *		 the sensor can continue making magnetic field measurements. With this configuration the microcontroller will only consume power and access the sensor
	 *		 if relevant measurement data is available.
	 *		 In master controlled mode the sensor powered down if when it is not triggered. This library configures to ADC start before sending first MSB of data registers
	 *		 In fast mode the measurements and ADC-conversions are running continuously.
	 */

	/**
	 * @brief Sets the operating mode of the sensor
	 * @param mode MASTERCONTROLLEDMODE,LOWPOWERMODE or FASTMODE, default is MASTERCONTROLLEDMODE
	 */
	bool setAccessMode(AccessMode_e mode);

	/**
	 * @brief Constructor of the sensor class.
	 * @param mode Operating mode of the sensor; default is the master controlled mode
	 * @param productType The library supports product types from A0 to A3; default is type A0
	 */
	Tle493d(AccessMode_e mode = MASTERCONTROLLEDMODE,
			TypeAddress_e productType = TLE493D_A0);

	/**
	 * @brief Destructor
	 */
	~Tle493d(void);
	/**
	 * @brief Starts the sensor
	 */
	void begin(void);

	/**
	 * @brief Starts the sensor
	 * @param bus The I2C bus
	 * @param slaveAddress The 7-bit slave address as defined in @ref Tle493d_Type
	 * @param reset If a reset should be initiated before starting the sensor
	 * @param oneByteRead If one-byte read protocol should be used. Otherwise the two-byte protocol is available
	 */
	void begin(TwoWire &bus, TypeAddress_e slaveAddress, bool reset,
			   uint8_t oneByteRead);

	/**
	 * @brief Enables temperature measurement; by default already enabled
	 */
	void enableTemp(void);

	/**
	 * @brief Disables temperature measurement to reduce power consumption
	 */
	void disableTemp(void);

	/**
	 * @brief Reads measurement results from sensor
	 */
	Tle493d_Error updateData(void);

	/**
	 * @return the Cartesian x-coordinate
	 */
	float getX(void);
	/**
	 * @return the Cartesian y-coordinate
	 */
	float getY(void);
	/**
	 * @return the Cartesian z-coordinate
	 */
	float getZ(void);

	/**
	 * @return norm of the magnetic field vector sqrt(x^2 + y^2 + z^2)
	 */
	float getNorm(void);

	/**
	 * @return the Azimuth angle arctan(y/x)
	 */
	float getAzimuth(void);

	/**
	 * @return the angle in polar coordinates arctan(z/(sqrt(x^2+y^2)))
	 */
	float getPolar(void);

	/**
	 * @return the temperature value
	 */
	float getTemp(void);

	/**
	 * @brief Resets the sensor
	 * @bug	not working
	 */
	void resetSensor(void);

	void readDiagnosis(uint8_t (&diag)[7]);

  protected:
	tle493d::BusInterface_t mInterface;
	/**
	 * @brief Stores new values into the bus interface; for this function to take effect the function writeOut() should be called afterwards
	 * @param Register mask index as defined in @ref Registers_e
	 * @param Value to be written into the register field specified by the register index
	 */
	void setRegBits(uint8_t regMaskIndex, uint8_t data);

	/**
	 * @brief Returns the value of a register field
	 * @param Register mask index as defined in @ref Registers_e
	 */
	uint8_t getRegBits(uint8_t regMaskIndex);

  private:
	const TypeAddress_e mProductType;
	AccessMode_e mMode;
	int16_t mXdata;
	int16_t mYdata;
	int16_t mZdata;
	int16_t mTempdata;

	/**
	 * @brief Enables interrupts
	 */
	void enableInterrupt(void);

	/**
	 * @brief Disables interrupts; Sensor read-out suppressed during ongoing ADC conversion
	 */
	void disableInterrupt(void);

	/**
	 * @brief Enables collision avoidance (clock stretching)
	 */
	void enableCollisionAvoidance(void);

	/**
	 * @brief Disables collision avoidance; readouts may collide with ADC conversion
	 */
	void disableCollisionAvoidance(void);

	/**
	 * @brief Sets FP (fuse parity) and CP (configuration parity)
	 */
	void calcParity(uint8_t regMaskIndex);

	/**
	 * @brief Concatenates the upper bits and lower bits of magnetic or temperature measurements
	 */
	int16_t concatResults(uint8_t upperByte, uint8_t lowerByte, bool isB);
};

#endif /* TLE493D_H_INCLUDED */
