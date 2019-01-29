/*********************************
 * SMU Hardware Abstraction Layer
 * 
 *        dummy for testing
 *********************************/

#include "SMU_HAL_dummy.h"


 
 int8_t SMU_HAL_dummy::fltSetCommitVoltageSource(float fVoltage) {
   return nowValueV = fVoltage;
 }
 
 int8_t SMU_HAL_dummy::fltSetCommitCurrentSource(float fCurrent, int8_t up_down_both) {
  return setValueI = fCurrent;                         
 }

 int SMU_HAL_dummy::init() {
  lastSampleMilli = millis();
   return 0; 
 }
 
 bool SMU_HAL_dummy::dataReady() {
  if (lastSampleMilli + 100 < millis()) {
    lastSampleMilli = millis();
    return true;
  }
  return false;
 }
 
 float SMU_HAL_dummy::measureVoltage(){

  int r = random(2);
  if (r == 0) {
    nowValueV = nowValueV + random(50) / 10000.0;
  } else if (r == 1) {
    nowValueV = nowValueV - random(50) / 10000.0;
  }

  return nowValueV;
 }
 
 float SMU_HAL_dummy::measureCurrent(){

  float simulatedLoad = 10.0; //ohm
  nowValueI = nowValueV / simulatedLoad;
  
  nowValueI =  nowValueI +  nowValueI * (random(0, 10) / 1000.0); // 0.0% - 0.1% error
  return nowValueI;
 }

 boolean SMU_HAL_dummy::compliance(){
   return abs(setValueI) < abs(nowValueI);
 }
 

    


