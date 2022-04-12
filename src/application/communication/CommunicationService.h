#ifndef _SerialService_
#define _SerialService_

#include <Arduino.h>
#include <framework/common/Event.h>
#include <application/utils/CommunicationData.h>

using namespace std;

namespace Services
{
  namespace Communication
  {

    void Initialize();


    extern Event<CommunicationData> RecievedDataEvent;

  } // namespace Communication
} // namespace Services

#endif
