#ifndef __CL6017S_h__
#define __CL6017S_h__

#include <Arduino.h>
#include <Wire.h>

#define DEBUG_STR(txt)           if (_debugEnabled) { Serial.print('>'); Serial.println(txt); }

class CL6017S 
{
public:
	bool	init();  /// Initialize library and the chip.
	void	enable(bool switchOn);  /// Enable/disable all radio functions.

	// ----- Audio features -----
	void	setVolume(uint8_t newVolume); /// Control the volume output of the radio chip in the range 0..15.
	uint8_t	getVolume();                  /// Retrieve the current output volume in the range 0..15.

	void	setMute(bool switchOn);       /// Control the mute mode of the radio chip.
	bool    getMute();                    /// Retrieve the current mute mode setting.

	void    setSoftMute(bool switchOn);   /// Control the softmute mode (mute on low signals) of the radio chip.
	bool    getSoftMute();                /// Retrieve the current soft mute mode setting.

	void    setBassBoost(bool switchOn);  /// Control the bass boost mode of the radio chip.
	bool    getBassBoost();               /// Retrieve the current bass boost mode setting.

	void	setFrequency(uint16_t newF);   /// Start using the new frequency for receiving.
	uint16_t	getFrequency();            /// Retrieve the current tuned frequency.

	void	seekUp(void(*seekProgress)(uint16_t currentFrequency) = nullptr);   /// Start a seek upwards from the current frequency.
	void	seekDown(void(*seekProgress)(uint16_t currentFrequency) = nullptr); /// Start a seek downwards from the current frequency.

	void	setMono(bool switchOn);   /// Control the mono mode of the radio chip.
	bool	getMono();                /// Retrieve the current mono mode setting.

	void	debugEnable(bool enable = true);

	void	reset();

private:
	bool _debugEnabled;
	bool _isSeeking;
	uint16_t registers[6]; /// Registers for controlling the radio chip.
	uint16_t _read16();
	void _write16(uint16_t val);
	void _readRegisters(uint8_t count = 6);
	void _readRSSIRegister();
	void _saveRegisters();
	void _saveCtrlRegister();
	void _setRegisterBit(uint8_t rgstr, uint16_t bit, bool value);
};


#endif