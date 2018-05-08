#ifndef TLE493D_W2B6_BUSIF_2_H_INCLUDED
#define TLE493D_W2B6_BUSIF_2_H_INCLUDED

#include "BusInterface.h"

#define BUS_ERROR 	1
#define BUS_OK 		0

namespace tle493d_w2b6
{
	
void initInterface(BusInterface_t *interface, TwoWire *bus, uint8_t adress, const uint8_t *resetValues);
bool readOut(BusInterface_t *interface);
bool readOut(BusInterface_t *interface, uint8_t count);
bool writeOut(BusInterface_t *interface, uint8_t regAddr);
bool writeOut(BusInterface_t *interface, uint8_t regAddr, uint8_t count);

}

#endif
