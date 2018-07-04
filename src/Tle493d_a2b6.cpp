#include "Tle493d_a2b6.h"
#include "./util/RegMask.h"
#include "./util/BusInterface2.h"

void Tle493d_a2b6::setUpdateRate(uint8_t updateRate){
	if(updateRate != 0 && updateRate != 1)
	{
		updateRate = 1; // slow
	}
	// PRD of A2B6 only convers the highest bit
	setRegBits(tle493d::PRD, updateRate << 2);
	tle493d::writeOut(&mInterface, 0x13);
}