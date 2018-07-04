
#ifndef TLE493D_A2B6_H_INCLUDED
#define TLE493D_A2B6_H_INCLUDED

#include "Tle493d.h"

class Tle493d_a2b6 : public Tle493d
{
    using Tle493d::Tle493d;
  public:
    void setUpdateRate(uint8_t updateRate);
};

#endif