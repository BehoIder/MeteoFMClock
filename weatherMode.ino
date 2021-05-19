WeatherMode::WeatherMode(uint32_t timeInterval) : Mode()
{
    timer.setInterval(timeInterval);
}

void WeatherMode::init(uint8_t param) 
{ 
    _secCounter = 0;
}

void WeatherMode::loop()
{
    Mode::loop();
    if (!timer.isReady()) return;
    syncRTC();
    if (_secCounter > 12)  modeSelector.setMode(Modes::Main, 0);

    float press = bme280.readPressure();
    float hum = bme280.readHumidity();
    float temp = bme280.readTemperature();

    anodeStates[2] = true;
    anodeStates[3] = true;
    anodeStates[4] = false;
    anodeStates[5] = true;

    if (_secCounter <= 3)
    {
        if (pressureExt.value == 0.0)
        {
            anodeStates[0] = true;
            anodeStates[1] = true;
            sendToIndicators(88, 88, 1, true);
        }
        else
        {
            anodeStates[0] = false;
            anodeStates[1] = false;
            sendToIndicators(0, round(temperatureExt.value), 1, true);
        }
    }
    else if (_secCounter <= 6)
    {
        if (pressureExt.value == 0.0)
        {
            anodeStates[0] = true;
            anodeStates[1] = true;
            sendToIndicators(88, 88, 2, true);
        }
        else
        {
            anodeStates[0] = false;
            anodeStates[1] = true;
            uint16_t pressureMmHg = round(pressureExt.value * 0.75006156);
            sendToIndicators(pressureMmHg / 100, pressureMmHg % 100, 2, true);
        }
    }
    else if (_secCounter <= 9)
    {
        anodeStates[0] = false;
        anodeStates[1] = false;
        sendToIndicators(0, round(temp), 3, true);
    }
    else if (_secCounter <= 12)
    {
        anodeStates[0] = false;
        anodeStates[1] = false;
        sendToIndicators(0, round(hum), 4, true);
    }
    _secCounter++;
}

void WeatherMode::buttonsLoop()
{
}