#pragma once
#include <GyverButton.h>
#include "timerMinim.h"
#include "effect.h"
#include "irCodes.h"

#define EFFECTS_COUNT 6

enum Modes : uint8_t
{
	Main, // main - clock mode
	Setup, // setup date-time, alarm
	Alarm, // alarm mode
	Weather, // meteo sensors values
	Radio // FM radio
};


// Base class for all modes
class Mode 
{
private:
	TimerMinim _syncTimer = TimerMinim(900);
protected:
	virtual void buttonsLoop() {}
	TimerMinim timer = TimerMinim();
	Effect** effects;
	uint32_t irKey = 0;
	uint8_t showModeSecCounter, modeValue;
	bool showMode = false;
public:
	Mode();
	~Mode();
	virtual void loop();
	virtual void init(uint8_t param) = 0;
};

class MainMode : public Mode 
{
private:
	bool _needRefreshTime = false, 
		_autoShowWeather = false; // automatically switch to weather mode every minute 
protected:
	void buttonsLoop();
public:
	MainMode(uint32_t timeInterval);
	void loop();
	void init(uint8_t param);
};

class SetupMode : public Mode 
{
private:
	int8_t _changeHrs, _changeMins;
	uint8_t _changeDay, _changeMonth;
	uint16_t _changeYear;
	uint8_t _param; // 0 = time setup, 1 = alarm setup, 2 = night mode setup (unavailable with light sensor using)
	bool _isHoursSelected, _lampState;
	uint8_t _setupStage = 0;
	GButton* _set;
	GButton* _left;
	GButton* _right;
protected:
	void buttonsLoop();
public:
	SetupMode(uint32_t timeInterval);
	void loop();
	void init(uint8_t param);
};

class AlarmMode : public Mode 
{
private:
	bool _lampState;
	uint32_t _alarmTimeCounter, _timeInterval;
	void stopAlarm();
protected:
	void buttonsLoop();
public:
	AlarmMode(uint32_t timeInterval);
	void loop();
	void init(uint8_t param);
};

class WeatherMode : public Mode
{
private:
	uint8_t _secCounter = 0;
protected:
	void buttonsLoop();
public:
	WeatherMode::WeatherMode(uint32_t timeInterval);
	void loop();
	void init(uint8_t param);
};

class RadioMode : public Mode
{
private:
	uint16_t _currentFrequency = 0;
	uint8_t _volume = 0;
	bool _showVolume = false;
	bool _setPreset = false;
	uint16_t _frequencyPresets[10];
	const uint32_t _irPresetsKeys[10] { IR_0, IR_1, IR_2, IR_3, IR_4, IR_5, IR_6, IR_7, IR_8, IR_9 };
	void _tryGetPresetFrequency();
	void _trySetPresetFrequency();
protected:
	void buttonsLoop();
public:
	RadioMode::RadioMode(uint32_t timeInterval);
	void loop();
	void init(uint8_t param);
	void setFrequency(uint16_t currentFrequency) { _currentFrequency = currentFrequency; }
	void show();
	static void seekProgress(uint16_t currentFrequency);
};