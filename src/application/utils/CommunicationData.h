#ifndef _CommunicationData_
#define _CommunicationData_

#include <Arduino.h>

using namespace std;


typedef struct CommunicationData
{
  float CpuUtilization;
  float CpuWattage;
  float CpuTemperature;
  float CpuMemory;

  float GpuUtilization;
  float GpuWattage;
  float GpuTemperature;
  float GpuMemory;

} CommunicationData;


#endif
