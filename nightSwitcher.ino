NightSwitcherEffect::NightSwitcherEffect(uint32_t timerInterval) : Effect() 
{
    timer.setInterval(timerInterval);
}

void NightSwitcherEffect::loop() 
{
    if (!timer.isReady() || suspended) return;
    uint16_t val = analogRead(LIGHT_SENSOR);
    //Serial.println(val);
    if (val < AUTO_BRIGHT_DAY_NIGHT_THRESOLD) 
    {
        indiMaxBright = INDI_BRIGHT_N;        
        dotMaxBright = DOT_BRIGHT_N;
        backlMaxBright = BACKL_BRIGHT_N;
        backlStep = BACKL_STEP_N;
    }
    else 
    {
        indiMaxBright = INDI_BRIGHT;        
        dotMaxBright = DOT_BRIGHT;
        backlMaxBright = BACKL_BRIGHT;
        backlStep = BACKL_STEP;
    }

    for (uint8_t i = 0; i < INDI_COUNT; i++) indiDimm[i] = indiMaxBright;
}
