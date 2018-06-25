#include "Tle493d_w2b6.h"
#include "./util/RegMask.h"
#include "./util/BusInterface2.h"

void Tle493d_w2b6::setWakeUpThreshold(float xh_th, float xl_th, float yh_th, float yl_th, float zh_th, float zl_th){
	//all thresholds should be within [-1,1], upper thresholds should be greater than lower thresholds
	if(xh_th>1 || xl_th<-1 || xl_th>xh_th ||
		yh_th>1 || yl_th<-1 || yl_th>yh_th||
		zh_th>1 || zl_th<-1 || zl_th>zh_th)
		return;

	int16_t xh = TLE493D_MAX_THRESHOLD * xh_th; int16_t xl = TLE493D_MAX_THRESHOLD * xl_th;
	int16_t yh = TLE493D_MAX_THRESHOLD * yh_th; int16_t yl = TLE493D_MAX_THRESHOLD * yl_th;
	int16_t zh = TLE493D_MAX_THRESHOLD * zh_th; int16_t zl = TLE493D_MAX_THRESHOLD * zl_th;

	setRegBits(tle493d::XL, (xl&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::XL2, xl&TLE493D_LSB_MASK);
	setRegBits(tle493d::XH, (xh&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::XH2, xh&TLE493D_LSB_MASK);

	setRegBits(tle493d::YL, (yl&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::YL2, yl&TLE493D_LSB_MASK);
	setRegBits(tle493d::YH, (yh&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::YH2, yh&TLE493D_LSB_MASK);

	setRegBits(tle493d::ZL, (zl&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::ZL2, zl&TLE493D_LSB_MASK);
	setRegBits(tle493d::ZH, (zh&TLE493D_MSB_MASK) >> 3); setRegBits(tle493d::ZH2, zh&TLE493D_LSB_MASK);

	//write out registers
	for(uint8_t i = tle493d::regMasks[tle493d::XL].byteAdress; i<=tle493d::regMasks[tle493d::ZL2].byteAdress; i++)
		tle493d::writeOut(&mInterface, i);
}

bool Tle493d_w2b6::wakeUpEnabled(void){
	//TODO not returning the correct value even when wake up is enabled
	return (bool)getRegBits(tle493d::WA);
}


void Tle493d_w2b6::setUpdateRate(uint8_t updateRate){
	if(updateRate>7) updateRate = 7;
	setRegBits(tle493d::PRD, updateRate);
	tle493d::writeOut(&mInterface, 0x13);
}