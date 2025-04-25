#ifndef PID_H
#define PID_H

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include "esp_timer.h"

#define DIRECT 0
#define REVERSE 1

class PID{
    public:
        /**
         * @brief Creates a new PID object.
         *
         * This constructor initializes a new PID object with the provided parameters. The PID controller will
         * use the pointers to the input, setpoint, and output variables to update the PID calculations dynamically.
         *
         * @param[in] input Pointer to the variable holding the input value.
         * @param[in] setpoint Pointer to the variable holding the setpoint value.
         * @param[in] output Pointer to the variable holding the output value.
         * @param[in] outputMin The minimum range value for the output. This defines the lower limit of the output.
         * @param[in] outputMax The maximum range value for the output. This defines the upper limit of the output.
         * @param[in] Kp Proportional gain. This gain determines how much the output will respond proportionally to the current error.
         * @param[in] Ki Integral gain. This gain determines how much the output will respond based on the accumulation of past errors.
         * @param[in] Kd Derivative gain. This gain determines how much the output will respond based on the rate of change of the error.
         */
        PID(float *input, float *setpoint, float *output, float outputMin, float outputMax, float Kp, float Ki, float Kd);

        /**
         * @brief Manual adjustment of PID gains.
         *
         * This function allows for manual tuning of the PID controller gains. By adjusting the proportional,
         * integral, and derivative gains, the behavior of the PID controller can be modified to better
         * suit the specific control requirements.
         *
         * @param[in] Kp Proportional gain.
         * @param[in] Ki Integral gain.
         * @param[in] Kd Derivative gain.
         */
        void setGains(float Kp, float Ki, float Kd);

        /**
         * @brief Manual (re)adjustment of output range.
         *
         * This function allows for manual adjustment of the output range. The `outputMin` and `outputMax` values
         * define the allowable range for the output variable.
         *
         * @param[in] outputMin The minimum range value for the output.
         * @param[in] outputMax The maximum range value for the output.
         */
        void setOutputRange(float outputMin, float outputMax);

        /**
         * @brief Manual adjustment of PID time interval for calculations.
         *
         * This function sets the time interval (in milliseconds) at which the PID calculations are allowed to run.
         * By default, this interval is set to 1000 milliseconds.
         *
         * @param[in] timeStep The time interval in milliseconds for PID calculations.
         */
        void setTimeStep(unsigned long timeStep);

        /**
         * @brief Automatically runs PID calculations at a certain time interval.
         *
         * This function should be called repeatedly from the main loop. It will only perform PID calculations
         * when the specified time interval has passed. The function reads the input and setpoint values, updates
         * the output, and performs necessary calculations.
         */
        void runDirekt();
        void runReverse();

        /**
         * @brief Resets internal PID calculation values.
         *
         * This function resets the internal values used in PID calculations, such as the integral and derivative terms.
         * It only clears the current calculations and does not stop the PID controller from running.
         */
        void reset();

        /**
         * @brief Get the current value of the integral.
         *
         * This function returns the current value of the error integral. It is useful for storing the state
         * of the PID controller after a power cycle.
         *
         * @return The current value of the error integral.
         */
        float getIntegral();

        /**
         * @brief Override the current value of the integral.
         *
         * This function allows for overriding the current value of the error integral. It is useful for resuming
         * the state of the PID controller after a power cycle. This function should be called after `run()` is called
         * for the first time, otherwise the value will be reset.
         *
         * @param[in] integral The value of the error integral to be used.
         */
        void setIntegral(float integral);

    private:
        float _Kp, _Ki, _Kd;
        float _integral, _error, _previousError;
        float *_input, *_setpoint, *_output;
        float _outputMin, _outputMax;
        unsigned long _timeStep, _lastStep, _dT;
};

#endif