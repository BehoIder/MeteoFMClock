#pragma once

#define GLITCH_START_SECOND 7
#define GLITCH_END_SECOND 55

class Effect 
{
protected:
	boolean effectExecution = false, suspended = false; 
	TimerMinim timer = TimerMinim(); 
	uint8_t currentMode;
public:
	Effect() { }
	virtual void loop() {}
	virtual void setMode(uint8_t mode) {}
	virtual void switchMode() {}
	virtual void suspend() { suspended = true; }
	virtual void resume() { suspended = false; }
	uint8_t mode() { return currentMode; }
};

#ifndef RGB_BACKLIGHT
class BacklightEffect : public Effect 
{
private:
	bool _directionToBright = true;
	uint8_t _backlBrightCounter = 0;
public:
	BacklightEffect(uint32_t timerInterval);
	void setMode(uint8_t mode);
	void switchMode();
	void loop();
};
#endif

#ifdef RGB_BACKLIGHT
class RGBBacklightEffect : public Effect
{
private:
	bool _directionToBright = true;
	uint8_t _backlBrightCounter = 0;
public:
	RGBBacklightEffect(uint32_t timerInterval);
	void setMode(uint8_t mode);
	void switchMode();
	void loop();
	void suspend();
};
#endif

class DotBlinkingEffect : public Effect 
{
private:
	uint8_t _dotBrightStep, _dotBrightCounter = 0;
	uint32_t _resetCounter, _timerInterval;
	bool _directionToBright = true;
public:
	DotBlinkingEffect(uint32_t timerInterval);
	void loop();
	void suspend();
};


class GlitchEffect : public Effect 
{
private:
	bool _indiState; 
	uint8_t _glitchCounter, _glitchMax, _glitchIndic;
	uint32_t _timerInterval;
	uint32_t _glitchMinSeconds = GLITCH_MIN, _glitchMaxSeconds = GLITCH_MAX;

public:
	GlitchEffect(uint32_t timerInterval);
	void setMode(uint8_t mode);
	void switchMode();
	void loop();
};


class FlipEffect : public Effect 
{
private:
	uint8_t _currentLamp, _flipEffectStages = 0, _indiBrightCounter;
	uint8_t _startCathode[4], _endCathode[4];
	bool _trainLeaving;
	bool _directionToBright = true;
public:
	FlipEffect();
	void setMode(uint8_t mode);
	void switchMode();
	void loop();
};

class NightSwitcherEffect : public Effect 
{
private:
public:
	NightSwitcherEffect(uint32_t timerInterval);
	void loop();
};

class LampBurningEffect : public Effect 
{
private:
public:
	LampBurningEffect(uint32_t timerInterval);
	void loop();
};