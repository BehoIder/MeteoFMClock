void setup() 
{
    Serial.begin(9600);
    Wire.begin();
    Wire.setClock(2000);
    // ---------- RTC -----------
    if (rtc.lostPower()) rtc.setTime(1);

    // -------- Radio ---------
    radio.debugEnable();
    radio.init();
    radio.enable(false);

    // -------- BME ---------
    bme280.begin();

    // -------- IRDA ---------
    irRecv.enableIRIn(); // enable the receiver

    // настройка пинов на выход
    pinMode(DECODER0_PIN, OUTPUT);
    pinMode(DECODER1_PIN, OUTPUT);
    pinMode(DECODER2_PIN, OUTPUT);
    pinMode(DECODER3_PIN, OUTPUT);
    pinMode(KEY0_PIN, OUTPUT);
    pinMode(KEY1_PIN, OUTPUT);
    pinMode(KEY2_PIN, OUTPUT);
    pinMode(KEY3_PIN, OUTPUT);
    pinMode(KEY4_PIN, OUTPUT);
    pinMode(KEY5_PIN, OUTPUT);
    pinMode(DOT_PIN, OUTPUT);
    pinMode(BACKL_PIN, OUTPUT);
    pinMode(PIEZO_PIN, OUTPUT);

    pinMode(LIGHT_SENSOR, INPUT);


    // разрешаем прерывания COMPA
    TCCR2A |= (1 << WGM21);   // включить CTC режим для COMPA
    TIMSK2 |= (1 << OCIE2A);  // включить прерывания по совпадению COMPA

      // EEPROM
    if (EEPROM.read(1023) != 100) 
    {   // первый запуск
        EEPROM.put(1023, 100);
        EEPROM.put(EEPROM_CELL_FLIP_MODE, 1);
        EEPROM.put(EEPROM_CELL_BACKLIGHT_MODE, 0);
        EEPROM.put(EEPROM_CELL_GLITCH_MODE, 0);
        EEPROM.put(EEPROM_CELL_ALARM_HOURS, 0);
        EEPROM.put(EEPROM_CELL_ALARM_MINUTES, 0);
        EEPROM.put(EEPROM_CELL_ALARM_MODE, 0);
    }

    // установить яркость на индикаторы
    for (uint8_t i = 0; i < INDI_COUNT; i++) indiDimm[i] = indiMaxBright;

    EEPROM.get(EEPROM_CELL_ALARM_HOURS, alarmHrs);
    EEPROM.get(EEPROM_CELL_ALARM_MINUTES, alarmMins);
    EEPROM.get(EEPROM_CELL_ALARM_MODE, alarmMode);

    // запуск приемника для беспроводного метеодатчика
    vw_set_rx_pin(RF_PIN);
    vw_setup(1200);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}
