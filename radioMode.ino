RadioMode::RadioMode(uint32_t timeInterval)
{
	timer.setInterval(timeInterval);
}

void RadioMode::init(uint8_t param)
{
	radio.enable(true);
	_frequency = radio.getFrequency();
	_volume = radio.getVolume();
}

void RadioMode::loop()
{
	Mode::loop();
	if (!timer.isReady()) return;
	if (_volumeCounter > 20) _showVolume = false; // 20 = 1 секунда
	else _volumeCounter++;
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
	anodeStates[0] = false;
	anodeStates[1] = false;
	anodeStates[2] = false;
	anodeStates[3] = false;
	anodeStates[4] = false;
	anodeStates[5] = false;

	if (_showVolume)
	{
		anodeStates[4] = true;
		anodeStates[5] = true;
		s = _volume;
	}
	else
	{
		anodeStates[2] = true;
		anodeStates[3] = true;
		anodeStates[4] = true;

		m = _frequency / 100;
		s = _frequency % 100;
		if (_frequency >= 10000)
		{
			anodeStates[1] = true;
			h = 1;
			m = (_frequency - 10000) / 100;
			s = (_frequency - 10000) % 100;
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
		_modeValue = effect->mode();
		glitch.suspend();
		flip.suspend();
		_showModeSecCounter = 0;
		_showMode = true;
	}

	if (btnLeft.isHolded() || irKey == IR_BACKWARD)
	{
		radio.seekDown(seekProgress);
		delay(10);
		_frequency = radio.getFrequency();
	}

	if (btnMiddle.isHolded() || irKey == IR_FORWARD)
	{
		radio.seekUp(seekProgress);
		delay(10);
		_frequency = radio.getFrequency();
	}

	if ((btnLeft.isClick() || irKey == IR_VOLDOWN || irKey == IR_VOLDOWN1))
	{
		if (_volume != 0) radio.setVolume(--_volume);
		_volumeCounter = 0;
		_showVolume = true;
	}

	if ((btnMiddle.isClick() || irKey == IR_VOLUP || irKey == IR_VOLUP1))
	{
		if (_volume < MAXVOLUME) radio.setVolume(++_volume);
		_volumeCounter = 0;
		_showVolume = true;
	}

	if (btnRight.isHolded() || irKey == IR_RADIOONOFF)
	{
		radio.enable(false);
		modeSelector.setMode(Modes::Main, 0); // main
	}
}