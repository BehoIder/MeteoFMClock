#include "CL6017S.h"

#define I2C_ADDRESS 0x10
#define I2C_ADDRESS2 0x60

// ----- Register Definitions -----
#define RADIO_REG_RSSI  0x00 // read-only
#define RADIO_REG_RSSI_RSSI 0x00FF
#define RADIO_REG_RSSI_STEREO 0x0100
#define RADIO_REG_RSSI_SEEKFAIL 0x0200
#define RADIO_REG_RSSI_SEEKCOMPLETE 0x0400
#define RADIO_REG_RSSI_HLSI 0x0800
#define RADIO_REG_RSSI_I2CMODE 0x8000

#define RADIO_REG_READCHAN  0x01 // read-only
#define RADIO_REG_READCHAN_CHAN 0x03FF

#define RADIO_REG_QOS  0x02 // read-only

#define RADIO_REG_CTRL    0x03 // read-write
#define RADIO_REG_CTRL_HLSI 0x0001
#define RADIO_REG_CTRL_HLSICTRL 0x0004
#define RADIO_REG_CTRL_BASS 0x0008
#define RADIO_REG_CTRL_HCC 0x0010
#define RADIO_REG_CTRL_STEREO   0x0020
#define RADIO_REG_CTRL_MONO   0x0040
#define RADIO_REG_CTRL_SEEKUP 0x0080
#define RADIO_REG_CTRL_SEEK   0x0100
#define RADIO_REG_CTRL_TUNE    0x0200
#define RADIO_REG_CTRL_SMUTE    0x0400
#define RADIO_REG_CTRL_RMUTE    0x800
#define RADIO_REG_CTRL_LMUTE    0x1000
#define RADIO_REG_CTRL_MUTE    0x2000
#define RADIO_REG_CTRL_RESET  0x4000
#define RADIO_REG_CTRL_DISABLE 0x8000

#define RADIO_REG_CHANVOL    0x04 // read-write
#define RADIO_REG_CHANVOL_CHAN   0x03FF
#define RADIO_REG_CHANVOL_VOL   0x3C00

#define RADIO_REG_SYSCONFIG    0x05 // read-write
#define RADIO_REG_SYSCONFIG_BAND 0xC000

#define MAXVOLUME 15

uint16_t CL6017S::_read16()
{
    uint8_t hiByte = Wire.read();
    uint8_t loByte = Wire.read();
    return((hiByte << 8) + loByte);
}

inline void CL6017S::_write16(uint16_t val)
{
    Wire.write(uint8_t((val & 0xFF00) >> 8));
    Wire.write(uint8_t(val & 0x00FF));
}

void CL6017S::_readRSSIRegister()
{
    Wire.requestFrom(I2C_ADDRESS, 2);
    registers[RADIO_REG_RSSI] = _read16();
    DEBUG_STR(registers[RADIO_REG_RSSI]);
}

void CL6017S::_readRegisters(uint8_t count = 6)
{
    Wire.requestFrom(I2C_ADDRESS, (count * 2));
    for (int i = 0; i < count; i++) 
    {
        registers[i] = _read16();
        DEBUG_STR(registers[i]);
    }
}

void CL6017S::_saveCtrlRegister()
{
    Wire.beginTransmission(I2C_ADDRESS);
    _write16(registers[RADIO_REG_CTRL]);
    uint8_t result = Wire.endTransmission();
    if (result != 0) DEBUG_STR("Bump!!!");
}

void CL6017S::_saveRegisters()
{
    DEBUG_STR("_saveRegisters");
    Wire.beginTransmission(I2C_ADDRESS);
    for (int i = 3; i < 6; i++) _write16(registers[i]);
    uint8_t result = Wire.endTransmission();
    if (result != 0) DEBUG_STR("Bump!!!");
}

inline void CL6017S::_setRegisterBit(uint8_t rgstr, uint16_t bit, bool value)
{
    if (value) registers[rgstr] |= bit;
    else registers[rgstr] &= (~bit);
}

bool CL6017S::init()
{
    bool result = false; // no chip found yet.
    Wire.beginTransmission(I2C_ADDRESS);
    result = Wire.endTransmission();
    if (result == 0) // success result
    {
        DEBUG_STR("radio found");
        _readRegisters();

        registers[RADIO_REG_CTRL] = 0xE481;
        registers[RADIO_REG_CHANVOL] = 0x3EF8;
        registers[RADIO_REG_SYSCONFIG] = 0x1F40;

        _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_DISABLE, 0);
        _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_MUTE, 0);
        _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 1);
        _saveRegisters();
        result = true;
    }
    return(result);
}


/// switch the power off/on
void CL6017S::enable(bool switchOn)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_DISABLE, !switchOn);
    _saveCtrlRegister();
}


// ----- Volume control -----
void CL6017S::setVolume(uint8_t newVolume)
{
    if (newVolume > MAXVOLUME) newVolume = MAXVOLUME;
    DEBUG_STR(newVolume);
    uint16_t volume = newVolume << 10;
    DEBUG_STR(volume);
    registers[RADIO_REG_CHANVOL] &= (~RADIO_REG_CHANVOL_VOL);
    registers[RADIO_REG_CHANVOL] |= volume;
    _saveRegisters();
}


uint8_t CL6017S::getVolume()
{
    return registers[RADIO_REG_CHANVOL] >> 10 & (~RADIO_REG_CHANVOL_VOL);
}


void CL6017S::setBassBoost(bool switchOn)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_BASS, switchOn);
    _saveCtrlRegister();
}


bool CL6017S::getBassBoost()
{
    return registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_BASS;
}


void CL6017S::setMono(bool switchOn)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_MONO, switchOn);
    _saveCtrlRegister();
}


bool CL6017S::getMono()
{
    return registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_MONO;
}


void CL6017S::setMute(bool switchOn)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_MUTE, switchOn);
    _saveCtrlRegister();
}

bool CL6017S::getMute()
{
    return registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_MUTE;
}


void CL6017S::setSoftMute(bool switchOn)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SMUTE, switchOn);
    _saveCtrlRegister();
}

bool CL6017S::getSoftMute()
{
    return registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_SMUTE;
}


void CL6017S::setFrequency(uint16_t newFreq)
{
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 0);
    _saveCtrlRegister();
    uint16_t chanFreq = (newFreq - 7000) / 5;
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 1);
    registers[RADIO_REG_CHANVOL] &= (~RADIO_REG_CHANVOL_CHAN);
    registers[RADIO_REG_CHANVOL] |= chanFreq;
    _saveRegisters();
}


void CL6017S::seekUp(void(*seekProgress)(uint16_t currentFrequency) = NULL)
{
    if (_isSeeking) return;
    _isSeeking = true;  
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 0);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEKUP, 1);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 0);
    _saveCtrlRegister();
    delay(1);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 1);
    _saveCtrlRegister();
    bool found = false;
    uint16_t chan;
    while (!found)
    {
        _readRegisters(2);
        if (registers[RADIO_REG_RSSI] & RADIO_REG_RSSI_SEEKFAIL)
        {
            _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 0);
            _saveCtrlRegister();
            delay(1);
            _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 1);
            _saveCtrlRegister();
        }
        if (registers[RADIO_REG_RSSI] & RADIO_REG_RSSI_SEEKCOMPLETE)
        {
            found = true;
        }
        chan = registers[RADIO_REG_READCHAN] & RADIO_REG_READCHAN_CHAN;
        if (seekProgress != NULL && chan < 760) seekProgress(chan * 5 + 7000);
        delay(50);
    }
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 1);
    registers[RADIO_REG_CHANVOL] &= (~RADIO_REG_CHANVOL_CHAN);
    registers[RADIO_REG_CHANVOL] |= chan;
    _saveRegisters();
    _isSeeking = false;
}

void CL6017S::seekDown(void(*seekProgress)(uint16_t currentFrequency) = NULL)
{
    if (_isSeeking) return;
    _isSeeking = true;
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 0);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEKUP, 0);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 0);
    _saveCtrlRegister();
    delay(1);
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 1);
    _saveCtrlRegister();
    bool found = false;
    uint16_t chan;
    while (!found)
    {
        _readRegisters(2);
        chan = registers[RADIO_REG_READCHAN] & RADIO_REG_READCHAN_CHAN;
        if (seekProgress != NULL && chan < 760) seekProgress(chan * 5 + 7000);
        if (registers[RADIO_REG_RSSI] & RADIO_REG_RSSI_SEEKFAIL)
        {
            _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 0);
            _saveCtrlRegister();
            delay(1);
            _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_SEEK, 1);
            _saveCtrlRegister();
        }
        else if (registers[RADIO_REG_RSSI] & RADIO_REG_RSSI_SEEKCOMPLETE)
        {
            found = true;
        }
        delay(50);
    }
    _setRegisterBit(RADIO_REG_CTRL, RADIO_REG_CTRL_TUNE, 1);
    registers[RADIO_REG_CHANVOL] &= (~RADIO_REG_CHANVOL_CHAN);
    registers[RADIO_REG_CHANVOL] |= chan;
    _saveRegisters();
    _isSeeking = false;
}
uint16_t CL6017S::getFrequency()
{
    _readRegisters(2);
    uint16_t chan = registers[RADIO_REG_READCHAN] & RADIO_REG_READCHAN_CHAN;
    DEBUG_STR(chan);
    return chan*5 + 7000;
}

void CL6017S::debugEnable(bool enable)
{
    _debugEnabled = enable;
}

