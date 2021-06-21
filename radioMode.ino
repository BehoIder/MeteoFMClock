RadioMode::RadioMode(uint32_t timeInterval)
{
	timer.setInterval(timeInterval);
	// load last frequency from EEPROM
	uint8_t lo, hi;
	EEPROM.get(EEPROM_CELL_FM_CURRENT_FREQUENCY_L, lo);
	EEPROM.get(EEPROM_CELL_FM_CURRENT_FREQUENCY_H, hi);
	_currentFrequency = makeWord(hi, lo);
	// load volume from EEPROM
	EEPROM.get(EEPROM_CELL_FM_VOLUME, _volume);
	// load presets from EEPROM
	for (uint8_t i = 0; i < 10; i++)
	{
		EEPROM.get(EEPROM_CELL_FM_0_FREQUENCY_L + i*2, lo);
		EEPROM.get(EEPROM_CELL_FM_0_FREQUENCY_H + i*2, hi);
		_frequencyPresets[i] = makeWord(hi, lo);
	}
}

void RadioMode::init(uint8_t param)
{
	// disable dot blinking
	blink.suspend();
	// enable radio
	radio.enable(true);
	// set current frequency
	if (_currentFrequency != 0) radio.setFrequency(_currentFrequency);
	else _currentFrequency = radio.getFrequency();
	// set volume
	radio.setVolume(_volume);
}

void RadioMode::loop()
{
	Mode::loop();
	if (!timer.isReady()) return;
	show();
}

void RadioMode::seekProgress(uint16_t currentFrequency)
{ 
	RadioMode*  mode = ((RadioMode*)modeSelector.getMode());
	mode->setFrequency(currentFrequency);
	mode->show();
}

void RadioMode::show()
{
	uint8_t h = 0;
	uint8_t m = 0;
	uint8_t s = 0;
	for (uint8_t i= 0; i < INDI_COUNT; i++) anodeStates[i] = false;

	if (showMode) // show preset
	{		
		anodeStates[0] = true;
		h = modeValue;
		if (showModeSecCounter > 20) showMode = false; // 20 = 1 second
		else showModeSecCounter++;
	}
	else if (_showVolume) // show current volume
	{
		anodeStates[4] = true;
		anodeStates[5] = true;
		s = _volume;
		if (showModeSecCounter > 20) _showVolume = false; // 20 = 1 second
		else showModeSecCounter++;
	}
	else // show frequency
	{
		bool lampStates = true;
		if (_setPreset)
		{
			showModeSecCounter++;
			if (showModeSecCounter > 10) lampStates = false;
			if (showModeSecCounter > 20) showModeSecCounter = 0;
		}

		anodeStates[2] = lampStates;
		anodeStates[3] = lampStates;
		anodeStates[4] = lampStates;

		m = _currentFrequency / 100;
		s = _currentFrequency % 100;
		if (_currentFrequency >= 10000)
		{
			anodeStates[1] = lampStates;
			h = 1;
			m = (_currentFrequency - 10000) / 100;
			s = (_currentFrequency - 10000) % 100;
		}
	}
	sendToIndicators(h, m, s, true);
}

void RadioMode::buttonsLoop()
{
	Effect* effect = NULL;
	if (irKey == IR_KD) effect = &glitch;
	if (irKey == IR_LED) effect = &backlight;
	if (effect != NULL)
	{
		effect->switchMode();
		modeValue = effect->mode();
		glitch.suspend();
		flip.suspend();
		showModeSecCounter = 0;
		showMode = true;
	}

	// seek down
	if (btnLeft.isHolded() || irKey == IR_BACKWARD)
	{
		radio.seekDown(seekProgress);
		_currentFrequency = radio.getFrequency();
		//Serial.println(_currentFrequency);
		EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_L, lowByte(_currentFrequency));
		EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_H, highByte(_currentFrequency));
	}

	// seek up
	if (btnMiddle.isHolded() || irKey == IR_FORWARD)
	{
		radio.seekUp(seekProgress);
		_currentFrequency = radio.getFrequency();
		//Serial.println(_currentFrequency);
		EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_L, lowByte(_currentFrequency));
		EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_H, highByte(_currentFrequency));
	}

	// decrease volume
	if ((btnLeft.isClick() || irKey == IR_VOLDOWN || irKey == IR_VOLDOWN1))
	{
		if (_volume != 0) radio.setVolume(--_volume);
		showModeSecCounter = 0;
		_showVolume = true;
		EEPROM.put(EEPROM_CELL_FM_VOLUME, _volume);
	}

	// increase volume
	if ((btnMiddle.isClick() || irKey == IR_VOLUP || irKey == IR_VOLUP1))
	{
		if (_volume < MAXVOLUME) radio.setVolume(++_volume);
		showModeSecCounter = 0;
		_showVolume = true;
		EEPROM.put(EEPROM_CELL_FM_VOLUME, _volume);
	}

	// return to clock mode 
	if (btnRight.isHolded() || irKey == IR_RADIOONOFF)
	{
		radio.enable(false);
		blink.resume();
		modeSelector.setMode(Modes::Main, 0);
	}

	// setting preset
	if (irKey != 0 && _setPreset) _trySetPresetFrequency();
	if (irKey == IR_SET)
	{
		_setPreset = !_setPreset;
		if (_setPreset) showModeSecCounter = 0;
	}

	if (irKey != 0 && !_setPreset) _tryGetPresetFrequency();
}

void RadioMode::_tryGetPresetFrequency()
{
	for (uint8_t i = 0; i < 10; i++)  
		if (irKey == _irPresetsKeys[i] && _frequencyPresets[i] != 0)
		{
			_currentFrequency = _frequencyPresets[i];
			radio.setFrequency(_currentFrequency);
			modeValue = i * 10;
			showMode = true;
			showModeSecCounter = 0;
		}
}

void RadioMode::_trySetPresetFrequency()
{
	for (uint8_t i = 0; i < 10; i++)  
		if (irKey == _irPresetsKeys[i])
		{
			_setPreset = false;
			_frequencyPresets[i] = _currentFrequency;
			EEPROM.put(EEPROM_CELL_FM_0_FREQUENCY_L + i*2, lowByte(_currentFrequency));
			EEPROM.put(EEPROM_CELL_FM_0_FREQUENCY_H + i*2, highByte(_currentFrequency));
			modeValue = i * 10;
			showMode = true;
			showModeSecCounter = 0;
		}
	_setPreset = false;
}