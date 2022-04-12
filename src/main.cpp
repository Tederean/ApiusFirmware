#include <Arduino.h>
#include <application/gui/GuiService.h>
#include <application/display/DisplayService.h>
#include <application/utils/CommunicationData.h>
#include <application/communication/CommunicationService.h>
#include <framework/services/SystemService.h>

void loop()
{
	Services::System::LoopEvent.Invoke(nullptr);
}

void OnRecievedDataEvent(CommunicationData *communicationData)
{
	Services::Gui::Update(communicationData);
}

void setup()
{
	Services::System::Initialize();

	Services::Display::Initialize();
	Services::Gui::Initialize();
	
	Services::Communication::Initialize();

	Services::Communication::RecievedDataEvent.Subscribe(OnRecievedDataEvent);
}