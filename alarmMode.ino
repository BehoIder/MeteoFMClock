AlarmMode::AlarmMode(uint32_t timeInterval) : Mode() 
{
    timer.setInterval(timeInterval);
    _timeInterval = timeInterval;
}

void AlarmMode::init(uint8_t param)
{
    _lampState = true;
    _alarmTimeCounter = 0;
}

void AlarmMode::loop() 
{
    Mode::loop();
    if (!timer.isReady()) return;
    syncRTC();
    sendToIndicators(hrs, mins, secs, false);
    _alarmTimeCounter += _timeInterval;
    if (_alarmTimeCounter > ALM_TIMEOUT*60000) stopAlarm();
    _lampState = !_lampState;
    for(uint8_t i = 0; i< INDI_COUNT; i++) anodeStates[i] = _lampState;
    if (_lampState) return;
    setPin(PIEZO_PIN, HIGH);
    delay(100);
    setPin(PIEZO_PIN, LOW);
    delay(100);
    setPin(PIEZO_PIN, HIGH);
    delay(100);
    setPin(PIEZO_PIN, LOW);
}

void AlarmMode::buttonsLoop() 
{
    if (btnMiddle.isClick() || btnLeft.isClick() || btnRight.isClick()) stopAlarm();
    if (btnMiddle.isHolded() || btnLeft.isHolded() || btnRight.isHolded()) {} // reset events
}

void AlarmMode::stopAlarm() 
{
    if (alarmMode == 1) 
    {
        alarmMode = 0;
        EEPROM.put(EEPROM_CELL_ALARM_MODE, alarmMode);
    }
    modeSelector.setMode(Modes::Main, 0);
}
