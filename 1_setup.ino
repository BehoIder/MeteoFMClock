void setup() 
{
    //Serial.begin(9600);
    Wire.begin();

    // ---------- RTC -----------
    if (rtc.lostPower()) rtc.setTime(1);

    // -------- Radio ---------
    // radio.debugEnable();
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
    if (EEPROM.read(1023) != 255) 
    {   // первый запуск
        EEPROM.put(1023, 255);
        EEPROM.put(EEPROM_CELL_FLIP_MODE, 1);
        EEPROM.put(EEPROM_CELL_BACKLIGHT_MODE, 0);
        EEPROM.put(EEPROM_CELL_GLITCH_MODE, 0);
        EEPROM.put(EEPROM_CELL_ALARM_HOURS, 0);
        EEPROM.put(EEPROM_CELL_ALARM_MINUTES, 0);
        EEPROM.put(EEPROM_CELL_ALARM_MODE, 0);

        // FM Radio settings set all to 0
        EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_L, 0);
        EEPROM.put(EEPROM_CELL_FM_CURRENT_FREQUENCY_H, 0);
        EEPROM.put(EEPROM_CELL_FM_VOLUME, 0);
        for (uint8_t i = 0; i <= 21 /*2byte * 10 + 1*/; i++) EEPROM.put(EEPROM_CELL_FM_0_FREQUENCY_L + i, 0);

    }

    // установить яркость на индикаторы
    for (uint8_t i = 0; i < INDI_COUNT; i++) indiDimm[i] = indiMaxBright;

    // запуск приемника для беспроводного метеодатчика
    vw_set_rx_pin(RF_PIN);
    vw_setup(1200);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}
