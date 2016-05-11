#include <MKRPing.h>
#include "HX711.h"
#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>

// outcomment this line to enable debugging mode
//#define DEBUG

// set this to 1 or 2, determines the current arduinos ID
#define ARDUINO_NUM 1

#define calibration_factor 25050.0
#define WEIGHTDELAY 10
#define DOUT  12
#define CLK  11
#define TRIGGER 4
#define SENSOR_NUM 4
#define THRESHOLD_A 40
#define THRESHOLD_B 40
#define THRESHOLD_C 40
#define THRESHOLD_D 40

bool hasChanged = true;
double weight = 0.0;
unsigned long counter;
bool detecting;
bool fuckingWithTheSystem = false;
int trashThreshold = 2;
int widthThreshold[SENSOR_NUM] = {
  THRESHOLD_A,
  THRESHOLD_B,
  THRESHOLD_C,
  THRESHOLD_D,
};
bool activated = false;
int detectTime = 0;
int fuckingTime = 0;
float* distance;
String output = "";

HX711 scale(DOUT, CLK);

// initialize the US sensor. Set pin 2 and 4 as echo pins, 3 as trigger.
uint8_t echos[SENSOR_NUM] = {
  7,
  8,
  9,
  10,
};
MKRPing sensor(TRIGGER, echos, SENSOR_NUM);

bool objectDetected();
float calculateWeight();

void setup() {
  // initializes the communication between arduinos
  Wire.begin(ARDUINO_NUM);

  // starts the serial
  Serial.begin(115200); 
  while (!Serial){
    delay(1);
  }

  // initializes and tares the scale
  scale.set_scale(calibration_factor); 
  scale.tare(); 

  // initializes the sensor
  sensor.begin();
}

void loop() {
  if (fuckingWithTheSystem == false){
   if (objectDetected() == true) {
     if (!hasChanged) {
      output = "0.0";
     }
     else{
      hasChanged = false;
      counter = millis();
      weight = calculateWeight();
     }
   }
 } else{
  if (distance[0] > 0 && distance[0] >= widthThreshold[0] && distance[1] > 0 && distance[1] >= widthThreshold[1] && distance[2] > 0 && distance[2] >= widthThreshold[2] && distance[3] > 0 && distance[3] >= widthThreshold[3]){
    fuckingTime ++;
    if (fuckingTime > 30000){
      fuckingTime = 0;
      detectTime = 0;
      fuckingWithTheSystem = false;
    }
  }
 }

 if (!hasChanged) {
   if (((calculateWeight() - trashThreshold) > weight && (millis() - counter) > 700) ||
       (millis() - counter >= 1000)) {
     float added = (calculateWeight() - weight);
     if(added < 0){
      added = 0;
     }
     output = added;
     hasChanged = true;
   }
 }

  // wait 1 millisecond
  delay(1);
}

bool objectDetected(){
  distance = sensor.read();

  // print distance of sensors if debugging
  #ifdef DEBUG
  Serial.print("Sensor 1: ");
  Serial.println(distance[0]);
  Serial.print("Sensor 2: ");
  Serial.println(distance[1]);
  Serial.print("Sensor 3: ");
  Serial.println(distance[2]);
  Serial.print("Sensor 4: ");
  Serial.println(distance[3]);
  Serial.println();
  #endif
  
  if ((distance[0] > 0 && distance[0] <= widthThreshold[0]) || (distance[1] > 0 && distance[1] <= widthThreshold[1]) || (distance[2] > 0 && distance[2] <= widthThreshold[2]) || (distance[3] > 0 && distance[3] <= widthThreshold[3])){
    activated = true;
    detecting = true;
    detectTime ++;
    if(detectTime >= 30000){
      fuckingWithTheSystem = true;
      activated = false;
      detecting = false;
      detectTime = 0;
      return false;
    }
  } else if (distance[0] > 0 && distance[0] >= widthThreshold[0] && distance[1] > 0 && distance[1] >= widthThreshold[1] && distance[2] > 0 && distance[2] >= widthThreshold[2] && distance[3] > 0 && distance[3] >= widthThreshold[3]){
    detecting = false;
    detectTime = 0;
  }

  if(activated == true && detecting == false){
    activated = false;
    return true;
  }
  return false;
}

float calculateWeight() {
  // print weight if debugging
  #ifdef DEBUG
  Serial.print("Weight: ");
  Serial.println(scale.get_units()*100);
  Serial.println();
  #endif
  
  return scale.get_units()*100;
}

/**'
 * This gets called when the master arduino requests data from this arduino
 */
void requestEvent() {
  // send the data to the master arduino
  Wire.write(output.c_str());
  output = "";
}

