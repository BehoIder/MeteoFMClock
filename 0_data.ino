#include "timerMinim.h"
#include "effect.h"
#include "mode.h"
#include "modeSelector.h"
#include "IRCodes.h"

#include "CL6017S.h"
#include <GyverBME280.h>
#include <microDS3231.h>
#include <GyverButton.h>
#include <EEPROM.h>
#include <VirtualWire.h>
#include <IRremote.h>

#ifdef RGB_BACKLIGHT
    #include <Adafruit_NeoPixel.h>
    #define LED_COUNT INDI_COUNT
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, BACKL_PIN, NEO_GRB + NEO_KHZ800);
#endif

typedef union transmission_packet
{
    float value;
    uint8_t packet[sizeof(float)];
};

MicroDS3231 rtc;
CL6017S radio;
GyverBME280 bme280;
IRrecv irRecv(IRDA_PIN);

// переменные
volatile int8_t indiDimm[INDI_COUNT];      // dimming value (0-24)
volatile int8_t indiCounter[INDI_COUNT];   // nixie counter (0-24)
volatile int8_t indiDigits[INDI_COUNT];    // nixie digit (0-10)
volatile int8_t curIndi;                   // current nixie (0-INDI_COUNT)

int8_t hrs, mins, secs, nightHrStart, nightHrEnd;
uint8_t alarmMode; // 0 = disabled, 1 = run once, 2 = everyday, 3 = workdays
int8_t alarmHrs, alarmMins;
uint8_t indiMaxBright = INDI_BRIGHT, dotMaxBright = DOT_BRIGHT;
#ifdef BACKL_PIN
    uint8_t backlMaxBright = BACKL_BRIGHT;
    uint8_t backlStep = BACKL_STEP;
#endif

bool newTimeFlag;
bool flipIndics[INDI_COUNT];
uint8_t newTime[INDI_COUNT];
bool anodeStates[] = { true, true, true, true, true, true };
GButton btnMiddle = GButton(BTNS_PIN, LOW_PULL, NORM_OPEN);
GButton btnLeft = GButton(BTNS_PIN, LOW_PULL, NORM_OPEN);
GButton btnRight = GButton(BTNS_PIN, LOW_PULL, NORM_OPEN);

ModeSelector modeSelector = ModeSelector(Modes::Main);

GlitchEffect glitch = GlitchEffect(1000L);
FlipEffect flip = FlipEffect();
DotBlinkingEffect blink = DotBlinkingEffect(DOT_TIMER);
NightSwitcherEffect night = NightSwitcherEffect(1000L);
LampBurningEffect burn = LampBurningEffect(BURN_PERIOD * 60000L);

#ifdef RGB_BACKLIGHT
RGBBacklightEffect backlight = RGBBacklightEffect(10L);
#else
BacklightEffect backlight = BacklightEffect(10L);
#endif

transmission_packet temperatureExt;
transmission_packet pressureExt;
uint8_t minutesAfterLastSync;

#ifndef RGB_BACKLIGHT
const uint8_t CRTgamma[256] PROGMEM = {
    0,    0,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    2,    2,    2,
    3,    3,    3,    3,    3,    3,    4,    4,
    4,    4,    4,    5,    5,    5,    5,    6,
    6,    6,    7,    7,    7,    8,    8,    8,
    9,    9,    9,    10,   10,   10,   11,   11,
    12,   12,   12,   13,   13,   14,   14,   15,
    15,   16,   16,   17,   17,   18,   18,   19,
    19,   20,   20,   21,   22,   22,   23,   23,
    24,   25,   25,   26,   26,   27,   28,   28,
    29,   30,   30,   31,   32,   33,   33,   34,
    35,   35,   36,   37,   38,   39,   39,   40,
    41,   42,   43,   43,   44,   45,   46,   47,
    48,   49,   49,   50,   51,   52,   53,   54,
    55,   56,   57,   58,   59,   60,   61,   62,
    63,   64,   65,   66,   67,   68,   69,   70,
    71,   72,   73,   74,   75,   76,   77,   79,
    80,   81,   82,   83,   84,   85,   87,   88,
    89,   90,   91,   93,   94,   95,   96,   98,
    99,   100,  101,  103,  104,  105,  107,  108,
    109,  110,  112,  113,  115,  116,  117,  119,
    120,  121,  123,  124,  126,  127,  129,  130,
    131,  133,  134,  136,  137,  139,  140,  142,
    143,  145,  146,  148,  149,  151,  153,  154,
    156,  157,  159,  161,  162,  164,  165,  167,
    169,  170,  172,  174,  175,  177,  179,  180,
    182,  184,  186,  187,  189,  191,  193,  194,
    196,  198,  200,  202,  203,  205,  207,  209,
    211,  213,  214,  216,  218,  220,  222,  224,
    226,  228,  230,  232,  233,  235,  237,  239,
    241,  243,  245,  247,  249,  251,  253,  255,
};

byte getPWM_CRT(byte val) {
    return pgm_read_byte(&(CRTgamma[val]));
}
#else
const uint8_t RGBpresets[10][3] =
{
    { 0, 0, 128 },
    { 64, 0, 128 },
    { 0, 64, 128 },
    { 0, 128, 0 },
    { 0, 128, 64 },
    { 64, 128, 0 },
    { 128, 0, 0 },
    { 128, 0, 64 },
    { 128, 64, 0 },
    { 128, 128, 128 }
};
uint8_t RGBcurrentPreset = 0;
#endif

// fast digitalWrite
void setPin(uint8_t pin, uint8_t x) 
{
  switch (pin) 
  {
    case 3:  // 2B
      bitClear(TCCR2A, COM2B1);
      break;
    case 5: // 0B
      bitClear(TCCR0A, COM0B1);
      break;
    case 6: // 0A
      bitClear(TCCR0A, COM0A1);
      break;
    case 9: // 1A
      bitClear(TCCR1A, COM1A1);
      break;
    case 10: // 1B
      bitClear(TCCR1A, COM1B1);
      break;
    case 11: // 2A
      bitClear(TCCR2A, COM2A1);
      break;
  }

  if (pin < 8) bitWrite(PORTD, pin, x);
  else if (pin < 14) bitWrite(PORTB, (pin - 8), x);
  else if (pin < 20) bitWrite(PORTC, (pin - 14), x);
  else return;
}

// fast analogWrite
void setPWM(uint8_t pin, uint16_t duty) 
{
  if (duty == 0) setPin(pin, LOW);
  else 
  {
    switch (pin) 
    {
      case 4:
        bitSet(TCCR0A, COM0B1);
        OCR0B = duty;
        break;
      case 5:
        bitSet(TCCR0A, COM0B1);
        OCR0B = duty;
        break;
      case 6:
        bitSet(TCCR0A, COM0A1);
        OCR0A = duty;
        break;
      case 10:
        bitSet(TCCR1A, COM1B1);
        OCR1B = duty;
        break;
      case 9:
        bitSet(TCCR1A, COM1A1);
        OCR1A = duty;
        break;
      case 3:
        bitSet(TCCR2A, COM2B1);
        OCR2B = duty;
        break;
      case 11:
        bitSet(TCCR2A, COM2A1);
        OCR2A = duty;
        break;
      default:
        break;
    }
  }
}