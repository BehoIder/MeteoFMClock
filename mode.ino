void ModeSelector::setMode(Modes mode, uint8_t param)
{
	if (mode == Modes::Main)  _currentMode = _mainMode;
	else if (mode == Modes::Setup) _currentMode = _setupMode;
	else if (mode == Modes::Alarm) _currentMode = _alarmMode;
	else if (mode == Modes::Weather) _currentMode = _weatherMode;
	else if (mode == Modes::Radio) _currentMode = _radioMode;
	_currentMode->init(param);
}

Mode::Mode()
{
	effects = new Effect*[EFFECTS_COUNT] { &glitch, & flip, & backlight, & blink, & night, & burn };
	EEPROM.get(EEPROM_CELL_ALARM_HOURS, alarmHrs);
	EEPROM.get(EEPROM_CELL_ALARM_MINUTES, alarmMins);
	EEPROM.get(EEPROM_CELL_ALARM_MODE, alarmMode);
}

Mode::~Mode()
{
	delete effects;
}

void Mode::loop() 
{
	// RF read
	if (_syncTimer.isReady() && secs >= 59 && pressureExt.value != 0)
	{
		minutesAfterLastSync++;
		if (minutesAfterLastSync >= 30) pressureExt.value = 0; // reset values after wireless meteo sensor not responded during 30 minutes
	}
	uint8_t buf[16];
	uint8_t buflen = 16;
	if (vw_get_message(buf, &buflen))
	{
		memcpy(temperatureExt.packet, buf, sizeof(float));
		memcpy(pressureExt.packet, buf + sizeof(float), sizeof(float));
		minutesAfterLastSync = 0;
	}

	// IR read
	if (irRecv.decode())
	{
		IRData* irResults = irRecv.read();
		irKey = irResults->decodedRawData;
		delete irResults;
		irRecv.resume();
	}

	// buttons read
	int analog = analogRead(BUTTONS_PIN);
	btnLeft.tick(analog <= 1023 && analog > 1000);
	btnMiddle.tick(analog <= 820 && analog > 690);
	btnRight.tick(analog <= 280 && analog > 120);


	buttonsLoop();
	irKey = 0;

	// current effects
	for (byte i = 0; i < EFFECTS_COUNT; i++) effects[i]->loop();
};
