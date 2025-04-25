
#include "LowPassFilter.h"



LowPassFilter::LowPassFilter(float *cutoffFrequency){
	_cutoffFrequency = cutoffFrequency;
	output.x = 0.0;
	output.y = 0.0;
	output.z = 0.0;
}


xyzFloat LowPassFilter::update(xyzFloat input){
	float deltaTime = (float(esp_timer_get_time()) - lastTimeFilter) / 1000000.0;

	ePow = 1-exp(-deltaTime * 2 * M_PI * *_cutoffFrequency);

	output.x += (input.x - output.x) * ePow;
	output.y += (input.y - output.y) * ePow;
	output.z += (input.z - output.z) * ePow;

	lastTimeFilter = esp_timer_get_time();

	return output;
}
