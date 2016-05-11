#include "Arduino.h"
#include "MKRPing.h"

/*MKRPing::MKRPing(uint8_t trigger, uint8_t echo)
{
  _trigger = trigger;
  _echos = echo;
}*/

MKRPing::MKRPing(uint8_t trigger, uint8_t echos[], uint8_t arraySize)
{
	_trigger = trigger;
	_echos = echos;
	_arraySize = arraySize;
}

void MKRPing::begin()
{
  //Serial.println("wtf");
  pinMode(_trigger, OUTPUT);
  /*Serial.print("Setting pin ");
  Serial.print(_trigger);
  Serial.println(" as TRIGGER");*/
  for (int i = 0; i < _arraySize; i++) {
	  pinMode(_echos[i], INPUT);
	 /* Serial.print("Setting pin ");
	  Serial.print(_echos[i]);
	  Serial.println(" as ECHO");*/
  }
  _time = (float*) malloc(sizeof(float)*4);
  _distance = (float*)malloc(sizeof(float)*4);
  _times = (unsigned long*)malloc(sizeof(float)*4);
}

/*float* MKRPing::distance()
{
  return distance;
}*/

float* MKRPing::read()
{
  digitalWrite(_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);
  improvedPulse();
 
  for (int i = 0; i < _arraySize; i++) {
	  _distance[i] = (_time[i]/2) / 29.41176;
  }
  return _distance;

  /*duration = pulseIn(_echo, HIGH);
  distance = (duration/2) / 29.41176;
  return distance;*/
}

void MKRPing::improvedPulse() {
	_startTime = millis();

	for (int i = 0; i < _arraySize; i++) {
		_time[i] = 0;
	}
	
	bool finishedPulse = false;
	do {
		finishedPulse = true;
		for (int i = 0; i < _arraySize; i++) {
			bool curVal = digitalRead(_echos[i]);
			if (_time[i] == 0 && curVal == HIGH) {
				_time[i] = TIMEOUT;
				_times[i] = micros();
			}
			if (_time[i] == TIMEOUT && curVal == LOW) {
				_time[i] = micros() - _times[i] + 1;
			}
			else if (_time[i] == TIMEOUT || _time[i] == 0){
				finishedPulse = false;
			}
		}
		delayMicroseconds(10);
	} while (!finishedPulse && millis() - _startTime < TIMEOUT);
}

