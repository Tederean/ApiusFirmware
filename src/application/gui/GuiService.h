#ifndef _GuiService_
#define _GuiService_

#include <Arduino.h>
#include <application/utils/CommunicationData.h>

using namespace std;

namespace Services
{
  namespace Gui
  {

    void Initialize();

    void Update(CommunicationData *communicationData);

  } // namespace Gui
} // namespace Services

#endif
