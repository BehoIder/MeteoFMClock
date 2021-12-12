WeatherMode::WeatherMode(uint32_t timeInterval) : Mode()
{
    timer.setInterval(timeInterval);
}

void WeatherMode::init(uint8_t param) 
{ 
    _secCounter = 0;
    blink.suspend();
}

void WeatherMode::loop()
{
    Mode::loop();
    if (!timer.isReady()) return;
    syncRTC();
    if (_secCounter > 12)
    {
        blink.resume();
        modeSelector.setMode(Modes::Main, 0); // return to clock mode after 12 seconds remaining
    }

    float press = bme280.readPressure();
    float hum = bme280.readHumidity();
    float temp = bme280.readTemperature();

    anodeStates[2] = true;
    anodeStates[3] = true;
    anodeStates[4] = false;
    anodeStates[5] = true;

    // external temperature
    if (_secCounter <= 3)
    {
        if (pressureExt.value == 0.0) // send "8888" if wireless sensor not responding 
        {
            anodeStates[0] = true;
            anodeStates[1] = true;
            sendToIndicators(88, 88, 1, true);
        }
        else
        {
            anodeStates[0] = false;
            anodeStates[1] = false;
            if (temperatureExt.value < 0) setPin(DOT_PIN, HIGH); else setPin(DOT_PIN, LOW); // dot lights for negative values 
            sendToIndicators(0, round(abs(temperatureExt.value)), 1, true);
        }
    }
    // pressure from wireless sensor, you may use value from internal sensor too
    else if (_secCounter <= 6)
    {
        if (pressureExt.value == 0.0) // send "8888" if wireless sensor not responding 
        {
            anodeStates[0] = true;
            anodeStates[1] = true;
            sendToIndicators(88, 88, 2, true);
        }
        else
        {
            anodeStates[0] = false;
            anodeStates[1] = true;
            setPin(DOT_PIN, LOW);
            uint16_t pressureMmHg = round(pressureExt.value * 0.75006156);
            sendToIndicators(pressureMmHg / 100, pressureMmHg % 100, 2, true);
        }
    }
    // internal temperature
    else if (_secCounter <= 9)
    {
        anodeStates[0] = false;
        anodeStates[1] = false;
        if (temp < 0) setPin(DOT_PIN, HIGH); else setPin(DOT_PIN, LOW); // dot lights for negative values 
        sendToIndicators(0, round(abs(temp)), 3, true);
    }
    // internal humidity
    else if (_secCounter <= 12)
    {
        anodeStates[0] = false;
        anodeStates[1] = false;
        setPin(DOT_PIN, LOW);
        sendToIndicators(0, round(hum), 4, true);
    }
    _secCounter++;
}

void WeatherMode::buttonsLoop()
{
}