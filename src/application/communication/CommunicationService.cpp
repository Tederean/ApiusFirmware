#include <Arduino.h>
#include <application/communication/CommunicationService.h>
#include <framework/services/SystemService.h>
#include <ArduinoJson.h>

using namespace std;

namespace Services
{
  namespace Communication
  {

    const timespan_t UpdateTimeoutTimespan = 5 * 1000 * 1000;


    Event<InitializationCommand> RecievedInitializationCommandEvent;

    Event<UpdateCommand> RecievedUpdateCommandEvent;

    Event<void> UpdateTimeoutEvent;


    void Initialize();

    void OnUpdateTimeoutEvent(void *args);

    void OnLoopEvent(void *args);
    
    
    void Initialize()
    {
      Serial.begin(115200);
      Serial.setRxBufferSize(512);
      Serial.setDebugOutput(false);

      UpdateTimeoutEvent.Subscribe(OnUpdateTimeoutEvent);

      Services::System::LoopEvent.Subscribe(OnLoopEvent);
    }

    void OnUpdateTimeoutEvent(void *args)
    {
      UpdateCommand updateCommand =
      {
        .Ratio0 = 0,
        .Ratio1 = 0,
        .Ratio2 = 0,
        .Ratio3 = 0,

        .Ratio4 = 0,
        .Ratio5 = 0,
        .Ratio6 = 0,
        .Ratio7 = 0,
      };

      strncpy(updateCommand.Text0, "", sizeof(updateCommand.Text0) / sizeof(char));
      strncpy(updateCommand.Text1, "", sizeof(updateCommand.Text1) / sizeof(char));
      strncpy(updateCommand.Text2, "", sizeof(updateCommand.Text2) / sizeof(char));
      strncpy(updateCommand.Text3, "", sizeof(updateCommand.Text3) / sizeof(char));

      strncpy(updateCommand.Text4, "", sizeof(updateCommand.Text4) / sizeof(char));
      strncpy(updateCommand.Text5, "", sizeof(updateCommand.Text5) / sizeof(char));
      strncpy(updateCommand.Text6, "", sizeof(updateCommand.Text6) / sizeof(char));
      strncpy(updateCommand.Text7, "", sizeof(updateCommand.Text7) / sizeof(char));

      RecievedUpdateCommandEvent.Invoke(&updateCommand);
    }

    void OnLoopEvent(void *args)
    {
      if (Serial.available() > 50)
      {
        StaticJsonDocument<512> jsonDeserializer;

        auto deserializationResult = deserializeJson(jsonDeserializer, Serial);

        if (deserializationResult == DeserializationError::Ok) 
        {
          auto commandID = jsonDeserializer["CommandID"].as<uint8_t>();

          if (commandID == CommandID::CommandID_Initialize)
          {
            InitializationCommand initializationCommand;
            
            strncpy(initializationCommand.Tile0, jsonDeserializer["Tile0"].as<String>().c_str(), sizeof(initializationCommand.Tile0) / sizeof(char));
            strncpy(initializationCommand.Tile1, jsonDeserializer["Tile1"].as<String>().c_str(), sizeof(initializationCommand.Tile1) / sizeof(char));
             
            strncpy(initializationCommand.Chart0, jsonDeserializer["Chart0"].as<String>().c_str(), sizeof(initializationCommand.Chart0) / sizeof(char));
            strncpy(initializationCommand.Chart1, jsonDeserializer["Chart1"].as<String>().c_str(), sizeof(initializationCommand.Chart1) / sizeof(char));
            strncpy(initializationCommand.Chart2, jsonDeserializer["Chart2"].as<String>().c_str(), sizeof(initializationCommand.Chart2) / sizeof(char));
            strncpy(initializationCommand.Chart3, jsonDeserializer["Chart3"].as<String>().c_str(), sizeof(initializationCommand.Chart3) / sizeof(char));

            strncpy(initializationCommand.Chart4, jsonDeserializer["Chart4"].as<String>().c_str(), sizeof(initializationCommand.Chart4) / sizeof(char));
            strncpy(initializationCommand.Chart5, jsonDeserializer["Chart5"].as<String>().c_str(), sizeof(initializationCommand.Chart5) / sizeof(char));
            strncpy(initializationCommand.Chart6, jsonDeserializer["Chart6"].as<String>().c_str(), sizeof(initializationCommand.Chart6) / sizeof(char));
            strncpy(initializationCommand.Chart7, jsonDeserializer["Chart7"].as<String>().c_str(), sizeof(initializationCommand.Chart7) / sizeof(char));

            RecievedInitializationCommandEvent.Invoke(&initializationCommand);
            return;
          }

          if (commandID == CommandID::CommandID_Update)
          {
            UpdateCommand updateCommand =
            {
              .Ratio0 = jsonDeserializer["Ratio0"].as<int16_t>(),
              .Ratio1 = jsonDeserializer["Ratio1"].as<int16_t>(),
              .Ratio2 = jsonDeserializer["Ratio2"].as<int16_t>(),
              .Ratio3 = jsonDeserializer["Ratio3"].as<int16_t>(),

              .Ratio4 = jsonDeserializer["Ratio4"].as<int16_t>(),
              .Ratio5 = jsonDeserializer["Ratio5"].as<int16_t>(),
              .Ratio6 = jsonDeserializer["Ratio6"].as<int16_t>(),
              .Ratio7 = jsonDeserializer["Ratio7"].as<int16_t>(),
            };

            strncpy(updateCommand.Text0, jsonDeserializer["Text0"].as<String>().c_str(), sizeof(updateCommand.Text0) / sizeof(char));
            strncpy(updateCommand.Text1, jsonDeserializer["Text1"].as<String>().c_str(), sizeof(updateCommand.Text1) / sizeof(char));
            strncpy(updateCommand.Text2, jsonDeserializer["Text2"].as<String>().c_str(), sizeof(updateCommand.Text2) / sizeof(char));
            strncpy(updateCommand.Text3, jsonDeserializer["Text3"].as<String>().c_str(), sizeof(updateCommand.Text3) / sizeof(char));

            strncpy(updateCommand.Text4, jsonDeserializer["Text4"].as<String>().c_str(), sizeof(updateCommand.Text4) / sizeof(char));
            strncpy(updateCommand.Text5, jsonDeserializer["Text5"].as<String>().c_str(), sizeof(updateCommand.Text5) / sizeof(char));
            strncpy(updateCommand.Text6, jsonDeserializer["Text6"].as<String>().c_str(), sizeof(updateCommand.Text6) / sizeof(char));
            strncpy(updateCommand.Text7, jsonDeserializer["Text7"].as<String>().c_str(), sizeof(updateCommand.Text7) / sizeof(char));

            RecievedUpdateCommandEvent.Invoke(&updateCommand);


            Services::System::InvokeCancel(&UpdateTimeoutEvent);
            Services::System::InvokeLater(&UpdateTimeoutEvent, UpdateTimeoutTimespan, SchedulingBehaviour::OneShot);
            return;
          }
        }
      }
    }
  }
}