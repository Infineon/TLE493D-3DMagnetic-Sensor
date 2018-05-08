/** @file Tle493d_w2b6.cpp
 *  @brief Arduino library to control Infineon's Magnetic 3D Sensor TLE493D-W2B6
 *	@author Yuxi Sun
 *	@author Florian ?
 *	@bug reset freezes the sensor, fast mode not working
 *	@bug User manual recommands INT=0 in fast mode however only disabling INT works
 *	@bug wake up mode not configured correctly (WA bit = 0)
 */


#include "Tle493d_w2b6.h"
#include "./util/RegMask.h"
#include "./util/BusInterface2.h"
#include <math.h>

Tle493d_w2b6::Tle493d_w2b6(AccessMode_e mode, TypeAddress_e productType):mMode(mode),mProductType(productType)
{
	mXdata = 0;
	mYdata = 0;
	mZdata = 0;
	mTempdata = 0;
}


Tle493d_w2b6::~Tle493d_w2b6(void)
{
	mInterface.bus->end();
}

void Tle493d_w2b6::begin(void)
{
	begin(Wire, mProductType, FALSE, 1);
}

void Tle493d_w2b6::begin(TwoWire &bus, TypeAddress_e slaveAddress, bool reset, uint8_t oneByteRead)
{
	//TURN ON THE SENSOR
	pinMode(LED2, OUTPUT);
	digitalWrite(LED2, HIGH);

	initInterface(&mInterface, &bus, slaveAddress, tle493d_w2b6::resetValues);

	mInterface.bus->begin();

	if(reset == true)
	{
		resetSensor();
	}

	// get all register data from sensor
	tle493d_w2b6::readOut(&mInterface);

	//1-byte protocol -> PR = 1
	setRegBits(tle493d_w2b6::PR, oneByteRead);
	
	//correct reset values for other product types
	switch(mProductType)
	{
		case TLE493D_W2B6_A1:
			setRegBits(tle493d_w2b6::IICadr, 0x01);
			setRegBits(tle493d_w2b6::ID, 0x01);
			break;
		case TLE493D_W2B6_A2:
			setRegBits(tle493d_w2b6::IICadr, 0x10);
			setRegBits(tle493d_w2b6::ID, 0x10);
			break;
		case TLE493D_W2B6_A3:
			setRegBits(tle493d_w2b6::IICadr, 0x11);
			setRegBits(tle493d_w2b6::ID, 0x11);
			break;
		default: break;
	}

	// default: master controlled mode
	setAccessMode(mMode);
	calcParity(tle493d_w2b6::CP);
	calcParity(tle493d_w2b6::FP);

	//write out the configuration register
	tle493d_w2b6::writeOut(&mInterface, 0x10);
	//write out MOD1 register
	tle493d_w2b6::writeOut(&mInterface, 0x11);

	delay(TLE493D_W2B6_STARTUPDELAY);
}

bool Tle493d_w2b6::setAccessMode(AccessMode_e mode)
{
	bool ret = BUS_OK;
	
	switch(mode){
		case MASTERCONTROLLEDMODE:
			setRegBits(tle493d_w2b6::MODE, MASTERCONTROLLEDMODE);
			// in master cotrolled mode TRIG has to be set
			// if PR = 0 (2-byte read protocol these bits have no effect)
			setRegBits(tle493d_w2b6::TRIG, 1); //trigger on read of address 00h, set to 2 or 3 to trigger on read of 06h
			break;
		case FASTMODE: 
			setRegBits(tle493d_w2b6::MODE, FASTMODE);
			setRegBits(tle493d_w2b6::WA, 1);
			setRegBits(tle493d_w2b6::WU, 1);
			setRegBits(tle493d_w2b6::XH2, 0x7);
			setRegBits(tle493d_w2b6::XL2, 0x0);
			tle493d_w2b6::writeOut(&mInterface, 0x0D);

			//User manual recommands INT=0 in fast mode however only disabling INT works
			setRegBits(tle493d_w2b6::INT, 1);
			//no clock stretching (INT=1 & CA=0) in fast mode
			setRegBits(tle493d_w2b6::CA, 1);
			setRegBits(tle493d_w2b6::TRIG, 0);

			break;

		case LOWPOWERMODE: 
			//for wake up mode T must be 0, CP must be odd, and CF = 1
			setRegBits(tle493d_w2b6::WA, 0x1);
			setRegBits(tle493d_w2b6::WU, 0x1);
			setRegBits(tle493d_w2b6::XH2, 0x7);
			setRegBits(tle493d_w2b6::XL2, 0x0);
			tle493d_w2b6::writeOut(&mInterface, 0x0D);

			//set update rate: fastest is 000b, slowest 111b
			setRegBits(tle493d_w2b6::PRD, 0);
			tle493d_w2b6::writeOut(&mInterface, 0x13);

			//INT must be disabled
			setRegBits(tle493d_w2b6::CA, 0);
			setRegBits(tle493d_w2b6::INT, 1);
			setRegBits(tle493d_w2b6::MODE, LOWPOWERMODE);
			break;
	}

	if ( ret != BUS_ERROR )
	{
		mMode = mode;
	}
	return ret;
}

void Tle493d_w2b6::enableInterrupt(void)
{
	setRegBits(tle493d_w2b6::INT, 1);
	calcParity(tle493d_w2b6::FP);
	tle493d_w2b6::writeOut(&mInterface,0x11);
}

void Tle493d_w2b6::disableInterrupt(void)
{
	setRegBits(tle493d_w2b6::INT, 0);
	calcParity(tle493d_w2b6::FP);
	tle493d_w2b6::writeOut(&mInterface,0x11);
}

void Tle493d_w2b6::enableCollisionAvoidance(void)
{
	setRegBits(tle493d_w2b6::CA, 1);
	calcParity(tle493d_w2b6::FP);
	tle493d_w2b6::writeOut(&mInterface,0x11);
}

void Tle493d_w2b6::disableCollisionAvoidance(void)
{
	setRegBits(tle493d_w2b6::CA, 0);
	calcParity(tle493d_w2b6::FP);
	tle493d_w2b6::writeOut(&mInterface,0x11);
}

void Tle493d_w2b6::enableTemp(void)
{
	setRegBits(tle493d_w2b6::DT, 0);
	calcParity(tle493d_w2b6::CP);
	tle493d_w2b6::writeOut(&mInterface,0x10);
}

void Tle493d_w2b6::disableTemp(void)
{
	setRegBits(tle493d_w2b6::DT, 1);
	calcParity(tle493d_w2b6::CP);
	tle493d_w2b6::writeOut(&mInterface,0x10);
}

void Tle493d_w2b6::setWakeUpThreshold(float xh_th, float xl_th, float yh_th, float yl_th, float zh_th, float zl_th){
	//all thresholds should be within [-1,1], upper thresholds should be greater than lower thresholds
	if(xh_th>1 || xl_th<-1 || xl_th>xh_th ||
		yh_th>1 || yl_th<-1 || yl_th>yh_th||
		zh_th>1 || zl_th<-1 || zl_th>zh_th)
		return;

	int16_t xh = TLE493D_W2B6_MAX_THRESHOLD * xh_th; int16_t xl = TLE493D_W2B6_MAX_THRESHOLD * xl_th;
	int16_t yh = TLE493D_W2B6_MAX_THRESHOLD * yh_th; int16_t yl = TLE493D_W2B6_MAX_THRESHOLD * yl_th;
	int16_t zh = TLE493D_W2B6_MAX_THRESHOLD * zh_th; int16_t zl = TLE493D_W2B6_MAX_THRESHOLD * zl_th;

	setRegBits(tle493d_w2b6::XL, (xl&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::XL2, xl&TLE493D_W2B6_LSB_MASK);
	setRegBits(tle493d_w2b6::XH, (xh&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::XH2, xh&TLE493D_W2B6_LSB_MASK);

	setRegBits(tle493d_w2b6::YL, (yl&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::YL2, yl&TLE493D_W2B6_LSB_MASK);
	setRegBits(tle493d_w2b6::YH, (yh&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::YH2, yh&TLE493D_W2B6_LSB_MASK);

	setRegBits(tle493d_w2b6::ZL, (zl&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::ZL2, zl&TLE493D_W2B6_LSB_MASK);
	setRegBits(tle493d_w2b6::ZH, (zh&TLE493D_W2B6_MSB_MASK) >> 3); setRegBits(tle493d_w2b6::ZH2, zh&TLE493D_W2B6_LSB_MASK);

	//write out registers
	for(uint8_t i = tle493d_w2b6::regMasks[tle493d_w2b6::XL].byteAdress; i<=tle493d_w2b6::regMasks[tle493d_w2b6::ZL2].byteAdress; i++)
		tle493d_w2b6::writeOut(&mInterface, i);
}

void Tle493d_w2b6::setUpdateRate(uint8_t updateRate){
	if(updateRate>7) updateRate = 7;
	setRegBits(tle493d_w2b6::PRD, updateRate);
	tle493d_w2b6::writeOut(&mInterface, 0x13);
}

Tle493d_w2b6_Error_t Tle493d_w2b6::updateData(void)
{
	Tle493d_w2b6_Error_t ret = TLE493D_W2B6_NO_ERROR;

	if (readOut(&mInterface, TLE493D_W2B6_MEASUREMENT_READOUT) != BUS_OK)
	{
		ret = TLE493D_W2B6_BUS_ERROR;
	}
	//no concatenation for 8 bit resolution
	mXdata = concatResults(getRegBits(tle493d_w2b6::BX1), getRegBits(tle493d_w2b6::BX2), true);
	mYdata = concatResults(getRegBits(tle493d_w2b6::BY1), getRegBits(tle493d_w2b6::BY2), true);
	mZdata = concatResults(getRegBits(tle493d_w2b6::BZ1), getRegBits(tle493d_w2b6::BZ2), true);
	mTempdata = concatResults(getRegBits(tle493d_w2b6::TEMP1), getRegBits(tle493d_w2b6::TEMP2), false);

	return ret;
}


float Tle493d_w2b6::getX(void)
{
	return static_cast<float>(mXdata) * TLE493D_W2B6_B_MULT;
}


float Tle493d_w2b6::getY(void)
{
	return static_cast<float>(mYdata) * TLE493D_W2B6_B_MULT;
}


float Tle493d_w2b6::getZ(void)
{
	return static_cast<float>(mZdata) * TLE493D_W2B6_B_MULT;
}


float Tle493d_w2b6::getTemp(void)
{
	return static_cast<float>(mTempdata-TLE493D_W2B6_TEMP_OFFSET) * TLE493D_W2B6_TEMP_MULT + TLE493D_W2B6_TEMP_25;
}


float Tle493d_w2b6::getNorm(void)
{
	return TLE493D_W2B6_B_MULT * sqrt(pow(static_cast<float>(mXdata), 2) + pow(static_cast<float>(mYdata), 2) + pow(static_cast<float>(mZdata), 2));
}


float Tle493d_w2b6::getAzimuth(void)
{
	return atan2(static_cast<float>(mYdata), static_cast<float>(mXdata));
}


float Tle493d_w2b6::getPolar(void)
{
	return atan2(static_cast<float>(mZdata), sqrt(pow(static_cast<float>(mXdata), 2) + pow(static_cast<float>(mYdata), 2)));
}

bool Tle493d_w2b6::wakeUpEnabled(void){
	//TODO not returning the correct value even when wake up is enabled
	return (bool)getRegBits(tle493d_w2b6::WA);
}

/* CAUTION: If the microcontroller is reset, the communication with the sensor may be corrupted, possibly causing the
	sensor to enter an incorrect state. After a reset, the sensor must be reconfigured to the desired settings.
*/
void Tle493d_w2b6::resetSensor()
{
	//TODO: this sequence kills the sensor
	mInterface.bus->begin();
	mInterface.bus->write(0xFF);
	mInterface.bus->end();
	
	mInterface.bus->begin();
	mInterface.bus->write(0xFF);
	mInterface.bus->end();

	mInterface.bus->begin();
	mInterface.bus->write(0x00);
	mInterface.bus->end();

	mInterface.bus->begin();
	mInterface.bus->write(0x00);
	mInterface.bus->end();
	
	delayMicroseconds(TLE493D_W2B6_RESETDELAY);
}

void Tle493d_w2b6::readDiagnosis(uint8_t (&diag) [7]){
	//P, FF, CF, T, PD3, PD0, FRM
	diag[0] = getRegBits(tle493d_w2b6::CP);
	diag[1] = getRegBits(tle493d_w2b6::FF);
	diag[2] = getRegBits(tle493d_w2b6::CF);
	diag[3] = getRegBits(tle493d_w2b6::T);
	diag[4] = getRegBits(tle493d_w2b6::PD3);
	diag[5] = getRegBits(tle493d_w2b6::PD0);
	diag[6] = getRegBits(tle493d_w2b6::FRM);
}
void Tle493d_w2b6::setRegBits(uint8_t regMaskIndex, uint8_t data)
{
	if(regMaskIndex < TLE493D_W2B6_NUM_OF_REGMASKS)
	{
		tle493d_w2b6::setToRegs(&(tle493d_w2b6::regMasks[regMaskIndex]), mInterface.regData, data);
	}
}

uint8_t Tle493d_w2b6::getRegBits(uint8_t regMaskIndex)
{
	if(regMaskIndex < TLE493D_W2B6_NUM_OF_REGMASKS)
	{
		const tle493d_w2b6::RegMask_t *mask = &(tle493d_w2b6::regMasks[regMaskIndex]);
		return tle493d_w2b6::getFromRegs(mask, mInterface.regData);
	}
	else
		return 0;
}

void Tle493d_w2b6::calcParity(uint8_t regMaskIndex)
{
	// regMaskIndex should be FP or CP, both odd parity
	// FP: parity of register 11 and the upper 3 bits (PRD) of 13
	// CP: registers 7-10 without WA, TST and PH bit. Affects CF bit in registre 6, thus CP has to be corrected
	//     after startup or reset. If CP is incorrect during a write cycle wake up is disabled 
	
	if(regMaskIndex != tle493d_w2b6::CP && regMaskIndex != tle493d_w2b6::FP)
		return;
	
	uint8_t y = 0x00;
	// set parity bit to 1
	// algorithm will calculate an even parity and replace this bit, 
	// so parity becomes odd
	setRegBits(regMaskIndex, 1);

	if(regMaskIndex == tle493d_w2b6::FP)
	{
		y ^= mInterface.regData[17];
		y ^= (mInterface.regData[19]<<5); //upper 3 bits
	}
	else if(regMaskIndex == tle493d_w2b6::CP)
	{
		uint8_t i;
		for(i = 7; i <= 12; i++)
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
	setRegBits(regMaskIndex, y&0x01);
}


int16_t Tle493d_w2b6::concatResults(uint8_t upperByte, uint8_t lowerByte, bool isB)
{
	//this function is register specific
	int16_t value=0x0000;	//16-bit signed integer for 12-bit values of sensor
	if(isB)
	{
		value= (uint16_t)upperByte<<8;
		value|=((uint16_t)lowerByte&0x0F)<<4;
	}
	else
	{
		//temperature measurement has 2 LSB
		value=(uint16_t)upperByte<<8;
		value|=((uint16_t)lowerByte&0xC0)<<6;
		value |= 0x06; //append bit 1 and 0
	}
	value>>=4;	//right shift of 2's complement fills MSB with 1's
	return value;
}

