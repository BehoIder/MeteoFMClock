#pragma once
#include "mode.h"

class ModeSelector 
{
private:
	Mode* _currentMode = NULL;
	Mode* _setupMode = new SetupMode(500L);
	Mode* _mainMode = new MainMode(500L);
	Mode* _alarmMode = new AlarmMode(500L);
	Mode* _weatherMode = new WeatherMode(1000L);
	Mode* _radioMode = new RadioMode(50L);
public:
	ModeSelector() {}
	ModeSelector(Modes mode) { setMode(mode, 0); }
	void setMode(Modes mode, uint8_t param);
	Mode* getMode() { return _currentMode; }
};
