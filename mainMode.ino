MainMode::MainMode(uint32_t timeInterval) : Mode() 
{
    timer.setInterval(timeInterval);
}

void MainMode::init(uint8_t param)
{
    _needRefreshTime = true;
    showMode = false;
}

void MainMode::loop() 
{
    Mode::loop();
    if (_needRefreshTime) 
    {
        for (uint8_t i = 0; i < INDI_COUNT; i++) anodeStates[i] = true;
        _needRefreshTime = false;
        syncRTC();
        sendToIndicators(hrs, mins, secs, false);
    }
    if (timer.isReady()) 
    {
        uint8_t dow = getWeekDay();
        if (hrs == alarmHrs && mins == alarmMins && secs == 0 &&
            (
                (alarmMode == 1 || alarmMode == 2) ||
                (alarmMode == 3 && dow != 1 && dow != 7)
                ))
        {
            modeSelector.setMode(Modes::Alarm, 0);
        }

        if (_autoShowWeather && secs == 30)
        {
            flip.resume();
            modeSelector.setMode(Modes::Weather, 0);
        }

        if (showMode) 
        {
            if (showModeSecCounter > SHOW_EFFECTS_MODE_TIMEOUT*2) 
            {
                showMode = false;
                _needRefreshTime = true;
                glitch.resume();
                flip.resume();
            }
            else 
            {
                anodeStates[0] = false;
                anodeStates[1] = false;
                anodeStates[2] = false;
                anodeStates[3] = true;
                anodeStates[4] = false;
                anodeStates[5] = false;
                showModeSecCounter++;
                sendToIndicators(0, modeValue, 0, true);
            }
        }
        else if (secs >= 59) 
        {
            setNewTime();
            newTimeFlag = true;
            _needRefreshTime = true;
        }
        else _needRefreshTime = true;
    }
}

void MainMode::buttonsLoop() 
{
    Effect* effect = NULL;

    if (btnRight.isClick() || irKey == IR_SET)  effect = &flip;
    if (btnMiddle.isClick() || irKey == IR_LED) effect = &backlight;
    if (btnLeft.isClick() || irKey == IR_KD) effect = &glitch;

    if (effect != NULL) 
    {
        effect->switchMode();
        modeValue = effect->mode();
        glitch.suspend();
        flip.suspend();
        showModeSecCounter = 0;
        showMode = true;
    }

    if (irKey == IR_AUTOWEATHER)
    {
        _autoShowWeather = !_autoShowWeather;
        modeValue = _autoShowWeather;
        showModeSecCounter = 0;
        showMode = true;
    }

    if (btnLeft.isHolded()) modeSelector.setMode(Modes::Setup, 0); // date-time setup
    if (btnMiddle.isHolded()) modeSelector.setMode(Modes::Setup, 1); // alarm setup
    if (btnRight.isHolded() || irKey == IR_RADIOONOFF) modeSelector.setMode(Modes::Radio, 0); // radio
    if (irKey == IR_WEATHER) modeSelector.setMode(Modes::Weather, 0); // weather
}
