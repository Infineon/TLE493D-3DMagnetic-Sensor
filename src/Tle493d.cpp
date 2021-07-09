/** @file Tle493d.cpp
 *  @brief Arduino library to control Infineon's Magnetic 3D Sensor TLE493D families
 *	@author Yuxi Sun
 *	@author Florian Schmidt
 *	@bug reset sequence freezes the W2B6 sensor, fast mode not working
 *	@bug User manual recommands INT=0 in fast mode however only disabling INT works
 *	@bug wake up mode not configured correctly (WA bit = 0)
 */

#include "Tle493d.h"
#include "./util/RegMask.h"
#include "./util/BusInterface2.h"
#include <math.h>

Tle493d::Tle493d(AccessMode_e mode, TypeAddress_e productType) : mMode(mode), mProductType(productType)
{
	mXdata = 0;
	mYdata = 0;
	mZdata = 0;
	mTempdata = 0;
}

Tle493d::~Tle493d(void)
{
	mInterface.bus->end();
}

void Tle493d::begin(void)
{
	begin(Wire, mProductType, FALSE, 1);
}

void Tle493d::begin(TwoWire &bus, TypeAddress_e slaveAddress, bool reset, uint8_t oneByteRead)
{
	initInterface(&mInterface, &bus, slaveAddress, tle493d::resetValues);
	mInterface.bus->begin();
	if (reset)
	{
		resetSensor();
	}

	//1-byte protocol -> PR = 1
	setRegBits(tle493d::PR, oneByteRead);
	 //disable interrupt
	setRegBits(tle493d::INT, 1);
	calcParity(tle493d::FP);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);

	// get all register data from sensor
	tle493d::readOut(&mInterface);

	//correct reset values for other product types
	switch (mProductType)
	{
	case TLE493D_A1:
		setRegBits(tle493d::IICadr, 0x01);
		break;
	case TLE493D_A2:
		setRegBits(tle493d::IICadr, 0x10);
		break;
	case TLE493D_A3:
		setRegBits(tle493d::IICadr, 0x11);
		break;
	default:
		break;
	}

	// default: master controlled mode
	setAccessMode(mMode);
	calcParity(tle493d::CP);
	calcParity(tle493d::FP);

	//write out the configuration register
	tle493d::writeOut(&mInterface, tle493d::CONFIG_REGISTER);
	//write out MOD1 register
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);

	// make sure the correct setting is written 
	tle493d::writeOut(&mInterface, tle493d::CONFIG_REGISTER);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
	delay(TLE493D_STARTUPDELAY);
}

bool Tle493d::setAccessMode(AccessMode_e mode)
{
	bool ret = BUS_OK;

	switch (mode)
	{
	case MASTERCONTROLLEDMODE:
		setRegBits(tle493d::MODE, MASTERCONTROLLEDMODE);
		// in master cotrolled mode TRIG has to be set
		// if PR = 0 (2-byte read protocol these bits have no effect)
		setRegBits(tle493d::TRIG, 1); //trigger on read of address 00h, set to 2 or 3 to trigger on read of 06h
		break;
	case FASTMODE:
		setRegBits(tle493d::MODE, FASTMODE);
		setRegBits(tle493d::WA, 1);
		setRegBits(tle493d::WU, 1);
		setRegBits(tle493d::XH2, 0x7);
		setRegBits(tle493d::XL2, 0x0);
		tle493d::writeOut(&mInterface, tle493d::WAKEUP_REGISTER);

		//User manual recommands INT=0 in fast mode however only disabling INT works
		setRegBits(tle493d::INT, 1);
		//no clock stretching (INT=1 & CA=0) in fast mode
		setRegBits(tle493d::CA, 1);
		setRegBits(tle493d::TRIG, 0);

		break;

	case LOWPOWERMODE:
		//for wake up mode T must be 0, CP must be odd, and CF = 1
		setRegBits(tle493d::WA, 0x1);
		setRegBits(tle493d::WU, 0x1);
		setRegBits(tle493d::XH2, 0x7);
		setRegBits(tle493d::XL2, 0x0);
		tle493d::writeOut(&mInterface, tle493d::WAKEUP_REGISTER);

		//set update rate: fastest is 000b, slowest 111b
		setRegBits(tle493d::PRD, 0);
		tle493d::writeOut(&mInterface, tle493d::MOD2_REGISTER);

		//INT must be disabled
		setRegBits(tle493d::CA, 0);
		setRegBits(tle493d::INT, 1);
		setRegBits(tle493d::MODE, LOWPOWERMODE);
		break;
	}

	if (ret != BUS_ERROR)
	{
		mMode = mode;
	}
	return ret;
}

void Tle493d::enableInterrupt(void)
{
	setRegBits(tle493d::INT, 0);
	calcParity(tle493d::FP);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
}

void Tle493d::disableInterrupt(void)
{
	setRegBits(tle493d::INT, 1);
	calcParity(tle493d::FP);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
}

void Tle493d::enableCollisionAvoidance(void)
{
	setRegBits(tle493d::CA, 0);
	calcParity(tle493d::FP);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
}

void Tle493d::disableCollisionAvoidance(void)
{
	setRegBits(tle493d::CA, 1);
	calcParity(tle493d::FP);
	tle493d::writeOut(&mInterface, tle493d::MOD1_REGISTER);
}

void Tle493d::enableTemp(void)
{
	setRegBits(tle493d::DT, 0);
	calcParity(tle493d::CP);
	tle493d::writeOut(&mInterface, tle493d::CONFIG_REGISTER);
}

void Tle493d::disableTemp(void)
{
	setRegBits(tle493d::DT, 1);
	calcParity(tle493d::CP);
	tle493d::writeOut(&mInterface, tle493d::CONFIG_REGISTER);
}

Tle493d_Error_t Tle493d::updateData(void)
{
	Tle493d_Error_t ret = TLE493D_NO_ERROR;

	if (readOut(&mInterface, TLE493D_MEASUREMENT_READOUT) != BUS_OK)
	{
		ret = TLE493D_BUS_ERROR;
	}
	//no concatenation for 8 bit resolution
	mXdata = concatResults(getRegBits(tle493d::BX1), getRegBits(tle493d::BX2), true);
	mYdata = concatResults(getRegBits(tle493d::BY1), getRegBits(tle493d::BY2), true);
	mZdata = concatResults(getRegBits(tle493d::BZ1), getRegBits(tle493d::BZ2), true);
	mTempdata = concatResults(getRegBits(tle493d::TEMP1), getRegBits(tle493d::TEMP2), false);

	return ret;
}

float Tle493d::getX(void)
{
	return static_cast<float>(mXdata) * TLE493D_B_MULT;
}

float Tle493d::getY(void)
{
	return static_cast<float>(mYdata) * TLE493D_B_MULT;
}

float Tle493d::getZ(void)
{
	return static_cast<float>(mZdata) * TLE493D_B_MULT;
}

float Tle493d::getTemp(void)
{
	return static_cast<float>(mTempdata - TLE493D_TEMP_OFFSET) * TLE493D_TEMP_MULT + TLE493D_TEMP_25;
}

float Tle493d::getNorm(void)
{
	return TLE493D_B_MULT * sqrt(pow(static_cast<float>(mXdata), 2) + pow(static_cast<float>(mYdata), 2) + pow(static_cast<float>(mZdata), 2));
}

float Tle493d::getAzimuth(void)
{
	return atan2(static_cast<float>(mYdata), static_cast<float>(mXdata));
}

float Tle493d::getPolar(void)
{
	return atan2(static_cast<float>(mZdata), sqrt(pow(static_cast<float>(mXdata), 2) + pow(static_cast<float>(mYdata), 2)));
}

/* CAUTION: If the microcontroller is reset, the communication with the sensor may be corrupted, possibly causing the
	sensor to enter an incorrect state. After a reset, the sensor must be reconfigured to the desired settings.
*/
void Tle493d::resetSensor()
{
	//TODO: tle493d-w2b6 freezes after being reset
	// mInterface.bus->begin();
	// mInterface.bus->write(0xFF);
	// mInterface.bus->end();

	// mInterface.bus->begin();
	// mInterface.bus->write(0xFF);
	// mInterface.bus->end();

	// mInterface.bus->begin();
	// mInterface.bus->write(0x00);
	// mInterface.bus->end();

	// mInterface.bus->begin();
	// mInterface.bus->write(0x00);
	// mInterface.bus->end();

	delayMicroseconds(TLE493D_RESETDELAY);
}

void Tle493d::readDiagnosis(uint8_t (&diag)[7])
{
	//P, FF, CF, T, PD3, PD0, FRM
	diag[0] = getRegBits(tle493d::CP);
	diag[1] = getRegBits(tle493d::FF);
	diag[2] = getRegBits(tle493d::CF);
	diag[3] = getRegBits(tle493d::T);
	diag[4] = getRegBits(tle493d::PD_3);
	diag[5] = getRegBits(tle493d::PD_0);
	diag[6] = getRegBits(tle493d::FRM);
}
void Tle493d::setRegBits(uint8_t regMaskIndex, uint8_t data)
{
	if (regMaskIndex < TLE493D_NUM_OF_REGMASKS)
	{
		tle493d::setToRegs(&(tle493d::regMasks[regMaskIndex]), mInterface.regData, data);
	}
}

uint8_t Tle493d::getRegBits(uint8_t regMaskIndex)
{
	if (regMaskIndex < TLE493D_NUM_OF_REGMASKS)
	{
		const tle493d::RegMask_t *mask = &(tle493d::regMasks[regMaskIndex]);
		return tle493d::getFromRegs(mask, mInterface.regData);
	}
	else
		return 0;
}

void Tle493d::calcParity(uint8_t regMaskIndex)
{
	// regMaskIndex should be FP or CP, both odd parity
	// FP: parity of register 11 and the upper 3 bits (PRD) of 13
	// CP: registers 7-10 without WA, TST and PH bit. Affects CF bit in registre 6, thus CP has to be corrected
	//     after startup or reset. If CP is incorrect during a write cycle wake up is disabled

	if (regMaskIndex != tle493d::CP && regMaskIndex != tle493d::FP)
		return;

	uint8_t y = 0x00;
	// set parity bit to 1
	// algorithm will calculate an even parity and replace this bit,
	// so parity becomes odd
	setRegBits(regMaskIndex, 1);

	if (regMaskIndex == tle493d::FP)
	{
		y ^= mInterface.regData[17];
		y ^= (mInterface.regData[19] >> 5); //upper 3 bits
	}
	else if (regMaskIndex == tle493d::CP)
	{
		uint8_t i;
		for (i = 7; i <= 12; i++)
		{
			// combine XL through ZH
			y ^= mInterface.regData[i];
		}
		y ^= (mInterface.regData[13] & 0x7F); //ignoring WA
		y ^= (mInterface.regData[14] & 0x3F); //ignoring TST
		y ^= (mInterface.regData[15] & 0x3F); //ignoring PH
		y ^= mInterface.regData[16];
	}
	// combine all bits of this byte (assuming each register is one byte)
	y = y ^ (y >> 1);
	y = y ^ (y >> 2);
	y = y ^ (y >> 4);
	// parity is in the LSB of y
	setRegBits(regMaskIndex, y & 0x01);
}

int16_t Tle493d::concatResults(uint8_t upperByte, uint8_t lowerByte, bool isB)
{
	//this function is register specific
	int16_t value = 0x0000; //16-bit signed integer for 12-bit values of sensor
	if (isB)
	{
		value = (uint16_t)upperByte << 8;
		value |= ((uint16_t)lowerByte & 0x0F) << 4;
	}
	else
	{
		//temperature measurement has 2 LSB
		value = (uint16_t)upperByte << 8;
		value |= ((uint16_t)lowerByte & 0xC0) << 6;
		value |= 0x06; //append bit 1 and 0
	}
	value >>= 4; //right shift of 2's complement fills MSB with 1's
	return value;
}
