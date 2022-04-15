#pragma once

#include <Arduino.h>
#include <framework/common/Event.h>

using namespace std;


typedef enum CommandID
{
  CommandID_Initialize = 1,
  CommandID_Update = 2,
} CommandID;


typedef struct InitializationCommand
{

  char Tile0[40];
  char Tile1[40];

  char Chart0[20];
  char Chart1[20];
  char Chart2[20];
  char Chart3[20];

  char Chart4[20];
  char Chart5[20];
  char Chart6[20];
  char Chart7[20];

} InitializationCommand;


typedef struct UpdateCommand
{

  int16_t Ratio0;
  int16_t Ratio1;
  int16_t Ratio2;
  int16_t Ratio3;

  int16_t Ratio4;
  int16_t Ratio5;
  int16_t Ratio6;
  int16_t Ratio7;

  char Text0[10];
  char Text1[10];
  char Text2[10];
  char Text3[10];

  char Text4[10];
  char Text5[10];
  char Text6[10];
  char Text7[10];

} UpdateCommand;


namespace Services
{
  namespace Communication
  {

    void Initialize();


    extern Event<InitializationCommand> RecievedInitializationCommandEvent;

    extern Event<UpdateCommand> RecievedUpdateCommandEvent;
  }
}