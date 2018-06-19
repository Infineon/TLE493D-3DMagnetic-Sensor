#ifndef TLE493D_BUSIF_H_INCLUDED
#define TLE493D_BUSIF_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#define TLE493D_NUM_REG		23

namespace tle493d
{

typedef struct 
{
	TwoWire *bus;
	uint8_t adress;
	uint8_t regData[TLE493D_NUM_REG];
} BusInterface_t;

}

#endif