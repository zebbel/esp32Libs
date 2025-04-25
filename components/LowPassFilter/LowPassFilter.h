
#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include <stdio.h>
#include <math.h>
#include "esp_timer.h"

#include "xyzFloat.h"

class LowPassFilter{
	private:
		xyzFloat output;
		float *_cutoffFrequency;
		float ePow;
		uint64_t lastTimeFilter = esp_timer_get_time();
	public:
		//constructors
		LowPassFilter(float *cutoffFrequency);
		//functions
		xyzFloat update(xyzFloat input);
};


#endif
