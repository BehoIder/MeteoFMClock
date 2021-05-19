#ifndef RGB_BACKLIGHT

BacklightEffect::BacklightEffect(uint32_t timerInterval) : Effect() 
{
    EEPROM.get(EEPROM_CELL_BACKLIGHT_MODE, currentMode);
    timer.setInterval(timerInterval);
}

void BacklightEffect::setMode(uint8_t mode)
{
    currentMode = mode;
    if (currentMode == 0) setPin(BACKL_PIN, 0);
    EEPROM.put(EEPROM_CELL_BACKLIGHT_MODE, currentMode);
}

void BacklightEffect::switchMode() 
{
    uint8_t mode = currentMode + 1;
    if (mode > 2) mode = 0;
    setMode(mode);
}

void BacklightEffect::loop()
{
    if (!timer.isReady() || suspended) return;
    if (currentMode == 1) setPWM(BACKL_PIN, backlMaxBright);
    if (currentMode == 2) 
    {
        if (backlMaxBright > 0) 
        {
            if (_directionToBright) 
            {
                if (!effectExecution) 
                {
                    effectExecution = true;
                    timer.setInterval(backlMaxBright / 2);
                }
                _backlBrightCounter += backlStep;
                if (_backlBrightCounter >= backlMaxBright) 
                {
                    _directionToBright = false;
                    _backlBrightCounter = backlMaxBright;
                }
            }
            else 
            {
                _backlBrightCounter -= backlStep;
                if (_backlBrightCounter <= BACKL_MIN_BRIGHT) 
                {
                    _directionToBright = true;
                    _backlBrightCounter = BACKL_MIN_BRIGHT;
                    timer.setInterval(BACKL_PAUSE);
                    effectExecution = false;
                }
            }
            setPWM(BACKL_PIN, getPWM_CRT(_backlBrightCounter));
        }
        else 
        {
            setPin(BACKL_PIN, 0);
        }
    }
}

#endif