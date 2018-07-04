
#ifndef TLE493D_W2B6_H_INCLUDED
#define TLE493D_W2B6_H_INCLUDED
#include "Tle493d.h"
class Tle493d_w2b6 : public Tle493d
{
    using Tle493d::Tle493d;
  public:
    /**
	 * @brief The Wake Up threshold range disabling /INT pulses between upper threshold and lower threshold is limited
	 *  	  to a window of the half output range. Here the adjustable range can be set with a ratio of size [-1,1]
	 * 		  When all the measurement values Bx, By and Bz are within this range INT is disabled.
	 * 		  If the arguments are out of range or any upper threshold is smaller than the lower one, the function
	 * 		  returns without taking effect.
	 * @param xh Upper threshold in x direction
	 * @param xl Lower threshold in x direction
	 * @param yh Upper threshold in y direction
	 * @param yl Lower threshold in y direction
	 * @param zh Upper threshold in z direction
	 * @param zl Lower threshold in z direction
	 */
    void setWakeUpThreshold(float xh, float xl, float yh, float yl, float zh,
                            float zl);

    /**
	 * @brief Checks if WA bit is set. When not interrupt configuration is as specified by the CA and INT bits.
	 */
    bool wakeUpEnabled(void);

    /**
	 * @brief Sets the update rate in low power mode
	 * @param updateRate Update rate which is an unsigned integer from the 0 (the fastest) to 7 (the highest)
	 */
    void setUpdateRate(uint8_t updateRate);
};
#endif