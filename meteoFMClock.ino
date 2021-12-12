#define INDI_COUNT 6 // nixie count

// ======================= ЭФФЕКТЫ =======================
// эффекты перелистывания часов
// Выбранный активен при запуске и меняется кнопками
// 0 - нет эффекта
// 1 - плавное угасание и появление (рекомендуемая скорость: 100-150)
// 2 - перемотка по порядку числа (рекомендуемая скорость: 50-80)
// 3 - перемотка по порядку катодов в лампе (рекомендуемая скорость: 30-50)
// 4 - поезд (рекомендуемая скорость: 50-170)
// 5 - резинка (рекомендуемая скорость: 50-150)
const uint8_t FLIP_SPEED[] = {0, 130, 50, 40, 70, 70}; // скорость эффектов, мс (количество не меняй)


// =======================  ЯРКОСТЬ =======================
#define RGB_BACKLIGHT       // включите для адресной RGB подсветки

#define AUTO_BRIGHT_DAY_NIGHT_THRESOLD 150 // порог переключения подбирается по типу фоторезистора

#define INDI_BRIGHT 20      // яркость цифр дневная (1 - 24) !на 24 могут быть фантомные цифры!
#define INDI_BRIGHT_N 5    // яркость ночная (1 - 24)

// =======================  ГЛЮКИ =======================
#define GLITCH_MIN 30       // минимальное время между глюками, с
#define GLITCH_MAX 120      // максимальное время между глюками, с

// ======================  МИГАНИЕ =======================
#define DOT_TIME 500        // время мигания точки, мс
#define DOT_TIMER 10        // шаг яркости точки, мс
#define DOT_BRIGHT 100      // яркость точки дневная (1 - 255)
#define DOT_BRIGHT_N 20     // яркость точки ночная (1 - 255)

// ==================  АНТИОТРАВЛЕНИЕ ====================
#define BURN_TIME 10        // период обхода индикаторов в режиме очистки, мс
#define BURN_LOOPS 3        // количество циклов очистки за каждый период
#define BURN_PERIOD 15      // период антиотравления, минут

#define SHOW_EFFECTS_MODE_TIMEOUT 2  // таймаут индикации режима подсветки, глюков, эффектов (сек)

// nano PWM pins
// 3, 5, 6, 9, 10, 11

// пины
#define KEY0_PIN 10   // hours
#define KEY1_PIN 9    // hours 
#define KEY2_PIN 5    // minutes
#define KEY3_PIN 6    // minutes
#define KEY4_PIN 7    // seconds
#define KEY5_PIN 8    // seconds
#define DOT_PIN 4     // dot
#define BTNS_PIN 3    // buttons
#define BACKL_PIN 12  // backlight
#define RF_PIN 2      // RF reciever
#define PIEZO_PIN 3   // piezo
#define IRDA_PIN 11   // IR reciever


// decoder
#define DECODER0_PIN A0
#define DECODER1_PIN A1
#define DECODER2_PIN A2
#define DECODER3_PIN A3

// A4 SDA I2C
// A5 SCL I2C

#define BUTTONS_PIN A7 // buttons
#define LIGHT_SENSOR A6 // light sensor

#define BACKL_BRIGHT 60     // day mode backlight bright (0 - 255)
#define BACKL_BRIGHT_N 4    // night mode backlight bright (0 - 255, 0 - off)
#define BACKL_MIN_BRIGHT 2  // breath mode min bright (0 - 255)
#define BACKL_PAUSE 1000    // breath mode dark duration, ms
#define BACKL_STEP 2        // breath mode backlight step (day)
#define BACKL_STEP_N 2      // breath mode backlight step (night)

#define ALM_TIMEOUT 30     // alarm auto-off minutes

// ячейки EEPROM
#define EEPROM_CELL_FLIP_MODE 0
#define EEPROM_CELL_BACKLIGHT_MODE 1
#define EEPROM_CELL_GLITCH_MODE 2
#define EEPROM_CELL_ALARM_HOURS 5
#define EEPROM_CELL_ALARM_MINUTES 6
#define EEPROM_CELL_ALARM_MODE 7
#define EEPROM_CELL_FM_CURRENT_FREQUENCY_L 8
#define EEPROM_CELL_FM_CURRENT_FREQUENCY_H 9
#define EEPROM_CELL_FM_VOLUME 10
#define EEPROM_CELL_FM_0_FREQUENCY_L 11 // to 29
#define EEPROM_CELL_FM_0_FREQUENCY_H 12 // to 30



const uint8_t digitMask[] = { 0, 1, 4, 5, 8, 9, 12, 13, 2, 3 };   // decoder mask in8-2, in14?
const uint8_t opts[] = { KEY0_PIN, KEY1_PIN, KEY2_PIN, KEY3_PIN, KEY4_PIN, KEY5_PIN };  // nixie order
const uint8_t cathodeMask[] = { 1, 0, 2, 9, 3, 8, 4, 7, 5, 6 }; // cathode mask in8-2, in14?
