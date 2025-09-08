#include "pid.h"

PID::PID(float *input, float *setpoint, float *output, float outputMin, float outputMax, float Kp, float Ki, float Kd){
    _input = input;
    _setpoint = setpoint;
    _output = output;
    _outputMin = outputMin;
    _outputMax = outputMax;
    setGains(Kp, Ki, Kd);
    _timeStep = 1000;
    _lastStep = esp_timer_get_time() / 1000;  // ESP-IDF equivalent for millis()
}

void PID::setGains(float Kp, float Ki, float Kd){
        _Kp = Kp;
        _Ki = Ki;
        _Kd = Kd;
}

void PID::setOutputRange(float outputMin, float outputMax) {
    _outputMin = outputMin;
    _outputMax = outputMax;
}

void PID::setTimeStep(unsigned long timeStep){
    _timeStep = timeStep;
}

void PID::runDirekt() {
    _dT = (esp_timer_get_time() / 1000) - _lastStep;   //calculate time since last update

    if(_dT >= _timeStep){                     //if long enough, do PID calculations
        _lastStep = esp_timer_get_time() / 1000;
        _error = *_setpoint - *_input;
        _integral += (_error + _previousError) / 2 * _dT / 1000.0;   //Riemann sum integral
        _integral = std::min(_integral, _outputMax);
        float _dError = (_error - _previousError) / _dT / 1000.0;   //derivative
        _previousError = _error;
        float PID = (_Kp * _error) + (_Ki * _integral) + (_Kd * _dError);
        *_output = std::clamp(PID, _outputMin, _outputMax);
    }
  
}

void PID::runReverse() {
    _dT = (esp_timer_get_time() / 1000) - _lastStep;   //calculate time since last update

    if(_dT >= _timeStep){                     //if long enough, do PID calculations
        _lastStep = esp_timer_get_time() / 1000;
        _error = *_input - *_setpoint;
        _integral += (_error + _previousError) / 2 * _dT / 1000.0;   //Riemann sum integral
        _integral = std::max(_integral, _outputMin);
        float _dError = (_error - _previousError) / _dT / 1000.0;   //derivative
        _previousError = _error;
        float PID = (_Kp * _error) + (_Ki * _integral) + (_Kd * _dError);
        *_output = std::clamp(PID, _outputMin, _outputMax);
    }
  
}

void PID::reset(){
    _lastStep = esp_timer_get_time() / 1000;
    _integral = 0;
    _previousError = 0;
}

float PID::getIntegral(){
    return _integral;
}

void PID::setIntegral(float integral){
    _integral = integral;
}