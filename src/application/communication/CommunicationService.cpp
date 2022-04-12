#include <Arduino.h>

#include <application/utils/CommunicationData.h>
#include <application/communication/CommunicationService.h>
#include <framework/services/SystemService.h>
#include <ArduinoJson.h>

using namespace std;

namespace Services
{
  namespace Communication
  {

    const timespan_t SerialReadInterval_us = 22 * 1000;


    Event<void> SerialReadEvent;

    Event<CommunicationData> RecievedDataEvent;


    void Initialize();

    void OnSerialReadEvent(void *args);
    
    
    void Initialize()
    {
      Serial.begin(115200);
      Serial.setDebugOutput(false);

      SerialReadEvent.Subscribe(OnSerialReadEvent);
      Services::System::InvokeLater(&SerialReadEvent, SerialReadInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void OnSerialReadEvent(void *args)
    {
      if (Serial.available() > 50)
      {
        StaticJsonDocument<300> jsonDeserializer;

        auto deserializationResult = deserializeJson(jsonDeserializer, Serial);

        if (deserializationResult == DeserializationError::Ok) 
        {
          CommunicationData communicationData =
          {
            .CpuUtilization = jsonDeserializer["CpuUtilization"].as<float>(),
            .CpuWattage = jsonDeserializer["CpuWattage"].as<float>(),
            .CpuTemperature = jsonDeserializer["CpuTemperature"].as<float>(),
            .CpuMemory = jsonDeserializer["CpuMemory"].as<float>(),

            .GpuUtilization = jsonDeserializer["GpuUtilization"].as<float>(),
            .GpuWattage = jsonDeserializer["GpuWattage"].as<float>(),
            .GpuTemperature = jsonDeserializer["GpuTemperature"].as<float>(),
            .GpuMemory = jsonDeserializer["GpuMemory"].as<float>(),
          };

          RecievedDataEvent.Invoke(&communicationData);
        }
      }
    }

  } // namespace Communication
} // namespace Services