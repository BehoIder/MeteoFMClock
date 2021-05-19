DotBlinkingEffect::DotBlinkingEffect(uint32_t timerInterval) : Effect() 
{
    timer.setInterval(timerInterval);
    _timerInterval = timerInterval;
}

void DotBlinkingEffect::loop() 
{
    if (!timer.isReady() || suspended) return;
    if (_resetCounter >= 1000) 
    {
        _resetCounter = 0;
        effectExecution = true;
        _dotBrightStep = dotMaxBright * _timerInterval * 2 / DOT_TIME;
        if (_dotBrightStep == 0) _dotBrightStep = 1;
    }
    if (effectExecution) 
    {
        if (_directionToBright) 
        {
            _dotBrightCounter += _dotBrightStep;
            if (_dotBrightCounter >= dotMaxBright) 
            {
                _directionToBright = false;
                _dotBrightCounter = dotMaxBright;
            }
        }
        else 
        {
            _dotBrightCounter -= _dotBrightStep;
            if (_dotBrightCounter <= 0) 
            {
                _directionToBright = true;
                _dotBrightCounter = 0;
                effectExecution = false;
            }
        }
        setPin(DOT_PIN, _dotBrightCounter != 0);
    }
    _resetCounter += _timerInterval;
}

void DotBlinkingEffect::suspend() 
{
    Effect::suspend();
    setPin(DOT_PIN, 0);
}
