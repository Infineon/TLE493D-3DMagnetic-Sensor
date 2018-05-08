#ifndef TLE493D_W2B6_BUSIF_H_INCLUDED
#define TLE493D_W2B6_BUSIF_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#define TLE493D_W2B6_NUM_REG		23

namespace tle493d_w2b6
{

typedef struct 
{
	TwoWire *bus;
	uint8_t adress;
	uint8_t regData[TLE493D_W2B6_NUM_REG];
} BusInterface_t;

}

#endif