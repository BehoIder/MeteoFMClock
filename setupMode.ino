SetupMode::SetupMode(uint32_t timeInterval) : Mode() 
{
    timer.setInterval(timeInterval);
}

void SetupMode::init(uint8_t param)
{
    _param = param;
    if (_param == 0) // time set mode
    { 
        _changeHrs = hrs;
        _changeMins = mins;
        DateTime now = rtc.getTime();
        _changeDay = now.day;
        _changeMonth = now.month;
        _changeYear = now.year;
        _set = &btnLeft;
        _left = &btnMiddle;
        _right = &btnRight;
    }
    else if (_param == 1) // alarm set mode
    { 
        _changeHrs = alarmHrs;
        _changeMins = alarmMins;
        _set = &btnMiddle;
        _left = &btnLeft;
        _right = &btnRight;
    }
    else if (_param == 2) // night mode set mode
    { 
        _changeHrs = nightHrStart;
        _changeMins = nightHrEnd;
        _set = &btnRight;
        _left = &btnLeft;
        _right = &btnMiddle;
    }
    _setupStage = 0;
    _isHoursSelected = true;
    _lampState = true;
}

void SetupMode::loop() 
{
    Mode::loop();
    if (!timer.isReady()) return;
    _lampState = !_lampState;
    anodeStates[4] = false;
    anodeStates[5] = false;
    if (!_isHoursSelected) 
    {
        anodeStates[0] = true;
        anodeStates[1] = true;
        anodeStates[2] = _lampState;
        anodeStates[3] = _lampState;
    }
    else 
    {
        anodeStates[0] = _lampState;
        anodeStates[1] = _lampState;
        anodeStates[2] = true;
        anodeStates[3] = true;
    }

    if (_param == 0) // date-time setup
    { 
        if (_setupStage == 0) // time setup
        { 
            sendToIndicators(_changeHrs, _changeMins, 0, false);
        }
        else if (_setupStage == 1) // year setup
        { 
            sendToIndicators((uint8_t)(_changeYear / 100), (uint8_t)(_changeYear % 100), 0, true); // year
            anodeStates[0] = _lampState;
            anodeStates[1] = _lampState;
            anodeStates[2] = _lampState;
            anodeStates[3] = _lampState;
        }
        else if (_setupStage == 2) // day, month
            sendToIndicators(_changeDay, _changeMonth, 0, true); 
    }
    else if (_param == 1) // alarm setup
    { 
        if (_setupStage == 0) // setup alarm mode
        { 
            sendToIndicators(alarmMode, 0, 0, true);
            anodeStates[0] = _lampState;
            anodeStates[1] = _lampState;
            anodeStates[2] = false;
            anodeStates[3] = false;
        }
        else if (_setupStage == 1) // setup time
        { 
            sendToIndicators(_changeHrs, _changeMins, 0, false);
        }
    }
}

void SetupMode::buttonsLoop() 
{
    if ((_setupStage == 0 && _param != 1) || (_setupStage == 1 && _param == 1) ) { // time setup
        if (_left->isClick()) 
        {
            if (_isHoursSelected)
            {
                _changeHrs--;
                if (_changeHrs < 0) _changeHrs = 23;
            }
            else 
            {
                _changeMins--;
                if (_param == 0 || _param == 1) 
                {
                    if (_changeMins < 0) _changeMins = 59; // time set or alarm time set
                }
                else
                    if (_changeMins < 0) _changeMins = 23; // day-night
            }
        }
        if (_right->isClick()) 
        {
            if (_isHoursSelected) 
            {
                _changeHrs++;
                if (_changeHrs > 23) _changeHrs = 0;
            }
            else 
            {
                _changeMins++;
                if (_param == 0 || _param == 1) 
                {
                    if (_changeMins > 59) _changeMins = 0; // time set or alarm time set
                }
                else
                    if (_changeMins > 23) _changeMins = 0; // day-night
            }
        }
    } 
    if (_setupStage == 0 && _param == 1) // setup alarm mode
    { 
        if (_left->isClick()) 
        {
            if (alarmMode > 0) alarmMode--;
        }
        if (_right->isClick()) 
        {
            if (alarmMode < 3) alarmMode++;
        }
    }
    if (_param == 0 && _setupStage == 2) // setup day-month
    { 
        if (_left->isClick()) 
        {
            if (_isHoursSelected) _changeDay--; else _changeMonth--;
        }
        if (_right->isClick()) 
        {
            if (_isHoursSelected) _changeDay++; else _changeMonth++;
        }
        uint8_t maxDay = getNumberOfDays(_changeYear, _changeMonth);
        if (_changeMonth > 12) _changeMonth = 1;
        if (_changeMonth < 1 ) _changeMonth = 12;
        if (_changeDay > maxDay) _changeDay = 1;
        if (_changeDay < 1) _changeDay= maxDay;
    }
    
    if (_param == 0 && _setupStage == 1) // setup year
    { 
        if (_left->isClick()) _changeYear--;
        if (_right->isClick()) _changeYear++;
        if (_changeYear < 2000) _changeYear = 2000;
        if (_changeYear > 3000) _changeYear = 3000;
    }

    if (_right->isHolded() || _left->isHolded()) {} // reset events

    if (_set->isHolded()) 
    {
        if (_param == 0) // datetime setup
        { 
            if (_setupStage < 2) 
            {
                _isHoursSelected = true;
                _setupStage++;
                return;
            }
            hrs = _changeHrs;
            mins = _changeMins;
            secs = 0;
            rtc.setTime(0, mins, hrs, _changeDay, _changeMonth, _changeYear);
        }
        else if (_param == 1) // alarm
        { 
            if (_setupStage == 0) 
            {
                if (alarmMode > 0) 
                {
                    _setupStage++;
                    return;
                }
            }
            alarmHrs = _changeHrs;
            alarmMins = _changeMins;

            EEPROM.put(EEPROM_CELL_ALARM_HOURS, alarmHrs);
            EEPROM.put(EEPROM_CELL_ALARM_MINUTES, alarmMins);
            EEPROM.put(EEPROM_CELL_ALARM_MODE, alarmMode);
            setPin(PIEZO_PIN, HIGH);
            delay(300);
            setPin(PIEZO_PIN, LOW);
        }
        modeSelector.setMode(Modes::Main, 0);
    }
    if (_set->isClick()) _isHoursSelected = !_isHoursSelected;
}
