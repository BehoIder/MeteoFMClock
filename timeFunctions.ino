// отправить на индикаторы

void sendToIndicators(uint8_t hours, uint8_t minutes, uint8_t seconds, bool notTime)
{
    indiDigits[0] = hours / 10;
    indiDigits[1] = hours % 10;
    indiDigits[2] = minutes / 10;
    indiDigits[3] = minutes % 10;
    indiDigits[4] = seconds / 10;
    indiDigits[5] = seconds % 10;
}

// для эффектов
void setNewTime() 
{
    int8_t newHrs = hrs, newMins = mins, newSecs;
    newSecs = secs + 1; 
    if (newSecs >= 60) { newSecs = 0; newMins++; if (newMins >= 60) { newMins = 0; newHrs++; if (newHrs >= 24) newHrs = 0; } }
    newTime[0] = (uint8_t)newHrs / 10;
    newTime[1] = (uint8_t)newHrs % 10;
    newTime[2] = (uint8_t)newMins / 10;
    newTime[3] = (uint8_t)newMins % 10;
    newTime[4] = secs;
    newTime[5] = secs;
}

void syncRTC() 
{
    DateTime now = rtc.getTime();
    secs = now.second;
    mins = now.minute;
    hrs = now.hour;
}

uint8_t getNumberOfDays(uint16_t year, uint8_t month)
{
    if (month == 2)
    {
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) return 29;
        else return 28;
    }
    else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) return 31;
    return 30;
}

uint8_t getWeekDay()
{
    DateTime now = rtc.getTime();
    int weekday = (now.day += now.month < 3 ? now.year-- : now.year - 2, 23 * now.month / 9 + now.day + 4 + now.year / 4 - now.year / 100 + now.year / 400) % 7;
}
