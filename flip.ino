FlipEffect::FlipEffect() : Effect() 
{
    EEPROM.get(EEPROM_CELL_FLIP_MODE, currentMode);
    timer.setInterval(FLIP_SPEED[currentMode]);
}

void FlipEffect::setMode(uint8_t mode)
{
    _indiBrightCounter = indiMaxBright;
    for (uint8_t i = 0; i < 4; i++) indiDimm[i] = indiMaxBright;
    currentMode = mode;
    timer.setInterval(FLIP_SPEED[currentMode]);
    EEPROM.put(EEPROM_CELL_FLIP_MODE, currentMode);
}

void FlipEffect::switchMode() 
{
    uint8_t newMode = 0;
    if (currentMode != 5) newMode = currentMode + 1;
    setMode(newMode);
}

void FlipEffect::loop()
{
    if (!newTimeFlag || suspended) return;
    switch (currentMode)
    {
    case 0:
    default:
        sendToIndicators(hrs, mins, secs, false);
        newTimeFlag = false;
        break;
    case 1:
        if (!effectExecution) 
        {
            effectExecution = true;
            // запоминаем, какие цифры поменялись и будем менять их яркость
            for (uint8_t i = 0; i < 4; i++)
            {
                if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
                else flipIndics[i] = false;
            }
        }
        if (timer.isReady()) 
        {
            if (!_directionToBright) 
            {
                _indiBrightCounter--;            // уменьшаем яркость
                if (_indiBrightCounter <= 0) // если яроксть меньше нуля
                {   
                    _directionToBright = true;   // меняем направление изменения
                    _indiBrightCounter = 0;        // обнуляем яркость
                    sendToIndicators(hrs, mins, secs, false);          // меняем цифры
                }
            }
            else 
            {
                _indiBrightCounter++;                        // увеличиваем яркость
                if (_indiBrightCounter >= indiMaxBright) // достигли предела
                {   
                    _directionToBright = false;              // меняем направление
                    _indiBrightCounter = indiMaxBright;        // устанавливаем максимум
                    // выходим из цикла изменения
                    effectExecution = false;
                    newTimeFlag = false;
                }
            }
            for (uint8_t i = 0; i < 4; i++)
                if (flipIndics[i]) indiDimm[i] = _indiBrightCounter;   // применяем яркость
        }
        break;
    case 2:
        if (!effectExecution) 
        {
            effectExecution = true;
            // запоминаем, какие цифры поменялись и будем менять их
            for (uint8_t i = 0; i < 4; i++)
            {
                if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
                else flipIndics[i] = false;
            }
        }

        if (timer.isReady()) 
        {
            uint8_t flipCounter = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (flipIndics[i]) {
                    indiDigits[i]--;
                    if (indiDigits[i] < 0) indiDigits[i] = 9;
                    if (indiDigits[i] == newTime[i]) flipIndics[i] = false;
                }
                else flipCounter++;        // счётчик цифр, которые не надо менять
            }
            if (flipCounter == 4) 
            {   // если ни одну из 4 цифр менять не нужно
              // выходим из цикла изменения
                effectExecution = false;
                newTimeFlag = false;
            }
        }
        break;
    case 3:
        if (!effectExecution) 
        {
            effectExecution = true;
            // запоминаем, какие цифры поменялись и будем менять их
            for (uint8_t i = 0; i < 4; i++)
            {
                if (indiDigits[i] != newTime[i]) 
                {
                    flipIndics[i] = true;
                    for (uint8_t c = 0; c < 10; c++)
                    {
                        if (cathodeMask[c] == indiDigits[i]) _startCathode[i] = c;
                        if (cathodeMask[c] == newTime[i]) _endCathode[i] = c;
                    }
                }
                else flipIndics[i] = false;
            }
        }
        if (timer.isReady()) 
        {
            uint8_t flipCounter = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (flipIndics[i]) {
                    if (_startCathode[i] > _endCathode[i]) 
                    {
                        _startCathode[i]--;
                        indiDigits[i] = cathodeMask[_startCathode[i]];
                    }
                    else if (_startCathode[i] < _endCathode[i]) 
                    {
                        _startCathode[i]++;
                        indiDigits[i] = cathodeMask[_startCathode[i]];
                    }
                    else flipIndics[i] = false;
                }
                else flipCounter++;
            }
            if (flipCounter == 4) 
            {   // если ни одну из 4 цифр менять не нужно
              // выходим из цикла изменения
                effectExecution = false;
                newTimeFlag = false;
            }
        }
        break;
    case 4:
        if (!effectExecution) 
        {
            effectExecution = true;
            _currentLamp = 0;
            _trainLeaving = true;
            timer.reset();
        }
        if (timer.isReady()) 
        {
            if (_trainLeaving) 
            {
                for (uint8_t i = 3; i > _currentLamp; i--) indiDigits[i] = indiDigits[i - 1];
                anodeStates[_currentLamp] = 0;
                _currentLamp++;
                if (_currentLamp >= 4) 
                {
                    _trainLeaving = false; //coming
                    _currentLamp = 0;
                }
            }
            else 
            {
                for (uint8_t i = _currentLamp; i > 0; i--) indiDigits[i] = indiDigits[i - 1];
                indiDigits[0] = newTime[3 - _currentLamp];
                anodeStates[_currentLamp] = 1;
                _currentLamp++;
                if (_currentLamp >= 4) 
                {
                    effectExecution = false;
                    newTimeFlag = false;
                }
            }
        }
        break;
    case 5:
        if (!effectExecution) 
        {
            effectExecution = true;
            _flipEffectStages = 0;
            timer.reset();
        }
        if (timer.isReady()) 
        {
            switch (_flipEffectStages++) 
            {
            case 1:
                anodeStates[3] = 0; 
                break;
            case 2:
                anodeStates[2] = 0;
                indiDigits[3] = indiDigits[2];
                anodeStates[3] = 1; 
                break;
            case 3:
                anodeStates[3] = 0; 
                break;
            case 4:
                anodeStates[1] = 0;
                indiDigits[2] = indiDigits[1];
                anodeStates[2] = 1; 
                break;
            case 5:
                anodeStates[2] = 0;
                indiDigits[3] = indiDigits[1];
                anodeStates[3] = 1; 
                break;
            case 6:
                anodeStates[3] = 0; 
                break;
            case 7:
                anodeStates[0] = 0;
                indiDigits[1] = indiDigits[0];
                anodeStates[1] = 1; 
                break;
            case 8:
                anodeStates[1] = 0;
                indiDigits[2] = indiDigits[0];
                anodeStates[2] = 1; 
                break;
            case 9:
                anodeStates[2] = 0;
                indiDigits[3] = indiDigits[0];
                anodeStates[3] = 1; 
                break;
            case 10:
                anodeStates[3] = 0;
                break;
            case 11:
                indiDigits[0] = newTime[3];
                anodeStates[0] = 1; 
                break;
            case 12:
                anodeStates[0] = 0;
                indiDigits[1] = newTime[3];
                anodeStates[1] = 1; 
                break;
            case 13:
                anodeStates[1] = 0;
                indiDigits[2] = newTime[3];
                anodeStates[2] = 1; 
                break;
            case 14:
                anodeStates[2] = 0;
                indiDigits[3] = newTime[3];
                anodeStates[3] = 1; 
                break;
            case 15:
                indiDigits[0] = newTime[2];
                anodeStates[0] = 1; 
                break;
            case 16:
                anodeStates[0] = 0;
                indiDigits[1] = newTime[2];
                anodeStates[1] = 1; 
                break;
            case 17:
                anodeStates[1] = 0;
                indiDigits[2] = newTime[2];
                anodeStates[2] = 1; 
                break;
            case 18:
                indiDigits[0] = newTime[1];
                anodeStates[0] = 1; 
                break;
            case 19:
                anodeStates[0] = 0;
                indiDigits[1] = newTime[1];
                anodeStates[1] = 1; 
                break;
            case 20:
                indiDigits[0] = newTime[0];
                anodeStates[0] = 1; 
                break;
            case 21:
                effectExecution = false;
                newTimeFlag = false;
                break;
            }
        }
        break;
    }
}
