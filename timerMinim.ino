TimerMinim::TimerMinim(uint32_t interval) 
{
    setInterval(interval);
    _timer = millis();
}

void TimerMinim::setInterval(uint32_t interval) 
{
    _interval = interval;
}

boolean TimerMinim::isReady() 
{
    uint32_t thisMls = millis();
    if (thisMls - _timer >= _interval) 
    {
        do 
        {
            _timer += _interval;
            if (_timer < _interval) break;          // переполнение uint32_t
        } while (_timer < thisMls - _interval);  // защита от пропуска шага
        return true;
    }
    else return false;
}

void TimerMinim::reset() 
{
    _timer = millis();
}
