#include "BusInterface2.h"

void tle493d::initInterface(BusInterface_t *interface, TwoWire *bus, uint8_t adress, const uint8_t *resetValues)
{
	interface->bus = bus;
	interface->adress = adress;

	// this sensor use different values to initialize registers :/
	for (uint8_t i = 0; i < TLE493D_NUM_REG; i++)
	{
		interface->regData[i] = resetValues[i];
	}
}

bool tle493d::readOut(BusInterface_t *interface)
{
	return readOut(interface, TLE493D_NUM_REG);
}

bool tle493d::readOut(BusInterface_t *interface, uint8_t count)
{
	bool ret = BUS_ERROR;
	int i;
	if (count > TLE493D_NUM_REG)
	{
		count = TLE493D_NUM_REG;
	}
	uint8_t received_bytes = interface->bus->requestFrom(interface->adress, count);
	if (received_bytes == count)
	{
		for (i = 0; i < count; i++)
		{
			interface->regData[i] = interface->bus->read();
		}
		ret = BUS_OK;
	}
	return ret;
}

// write out to a specific register
bool tle493d::writeOut(BusInterface_t *interface, uint8_t regAddr)
{
	bool ret = BUS_ERROR;
	interface->bus->beginTransmission(interface->adress);

	interface->bus->write(regAddr);
	interface->bus->write(interface->regData[regAddr]);

	if (interface->bus->endTransmission() == 0)
	{
		ret = BUS_OK;
	}
	return ret;
}