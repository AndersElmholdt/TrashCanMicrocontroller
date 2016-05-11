#ifndef MKRPing_h
#define MKRPing_h

#include "Arduino.h"

#define TIMEOUT 500

class MKRPing
{
   public:
     MKRPing(uint8_t trigger, uint8_t echos[], uint8_t arraySize);

     void begin();
	 float* read();
     //float* distance();
   private:
     uint8_t _trigger;
	 uint8_t* _echos;
	 uint8_t _arraySize;
     float* _distance;
	 void improvedPulse();
	 bool* _hasPulsed;
	 float* _time;
	 unsigned long _startTime;
	 unsigned long* _times;
};

#endif
