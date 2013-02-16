#pragma once
#include "I2C.h"

// https://raw.github.com/adafruit/Adafruit-BMP085-Library/master/CBMP085.h

#define ARDUINO 0xA60

class CArduino 
{
public:
	CI2C Wire;

	void _delay_ms(int n)
	{
		CDelay::DelayMs(n);
	}
};

class CBMP085 : public CArduino
{
public:
	enum {
		I2CADDR = 0x77,
		ULTRALOWPOWER = 0,
		STANDARD = 1,
		HIGHRES = 2,
		ULTRAHIGHRES = 3,
		CAL_AC1 = 0xAA,
		CAL_AC2 = 0xAC,
		CAL_AC3 = 0xAE,
		CAL_AC4 = 0xB0,
		CAL_AC5 = 0xB2,
		CAL_AC6 = 0xB4,
		CAL_B1 = 0xB6,
		CAL_B2 = 0xB8,
		CAL_MB = 0xBA,
		CAL_MC = 0xBC,
		CAL_MD = 0xBE,
		CONTROL = 0xF4, 
		TEMPDATA = 0xF6,
		PRESSUREDATA = 0xF6,
		READTEMPCMD = 0x2E,
		READPRESSURECMD = 0x34
	};

public:
  bool begin(ui8 mode = ULTRAHIGHRES)  // by default go highres
  {
    if (mode > ULTRAHIGHRES) 
      mode = ULTRAHIGHRES;
    oversampling = mode;

    Wire.begin();

    if (read8(0xD0) != 0x55) return false;

    /* read calibration data */
    ac1 = read16(CAL_AC1);
    ac2 = read16(CAL_AC2);
    ac3 = read16(CAL_AC3);
    ac4 = read16(CAL_AC4);
    ac5 = read16(CAL_AC5);
    ac6 = read16(CAL_AC6);

    b1 = read16(CAL_B1);
    b2 = read16(CAL_B2);

    mb = read16(CAL_MB);
    mc = read16(CAL_MC);
    md = read16(CAL_MD);
  #if (DEBUG == 1)
    Serial.print("ac1 = "); Serial.println(ac1, DEC);
    Serial.print("ac2 = "); Serial.println(ac2, DEC);
    Serial.print("ac3 = "); Serial.println(ac3, DEC);
    Serial.print("ac4 = "); Serial.println(ac4, DEC);
    Serial.print("ac5 = "); Serial.println(ac5, DEC);
    Serial.print("ac6 = "); Serial.println(ac6, DEC);

    Serial.print("b1 = "); Serial.println(b1, DEC);
    Serial.print("b2 = "); Serial.println(b2, DEC);

    Serial.print("mb = "); Serial.println(mb, DEC);
    Serial.print("mc = "); Serial.println(mc, DEC);
    Serial.print("md = "); Serial.println(md, DEC);
  #endif
  	return true;
  }

  float readTemperature(void)
  {
    si32 UT, X1, X2, B5;     // following ds convention
    float temp;

    UT = readRawTemperature();

  #if DEBUG == 1
    // use datasheet numbers!
    UT = 27898;
    ac6 = 23153;
    ac5 = 32757;
    mc = -8711;
    md = 2868;
  #endif

    // step 1
    X1 = (UT - (si32)ac6) * ((si32)ac5) / pow(2,15);
    X2 = ((si32)mc * pow(2,11)) / (X1+(si32)md);
    B5 = X1 + X2;
    temp = (B5+8)/pow(2,4);
    temp /= 10;
    
    return temp;
  }

  float readAltitude(float sealevelPressure = 101325) // std atmosphere
  {
    float altitude;
    float pressure = readPressure();
    altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));
    return altitude;
  }

  si32 readPressure(void)
  {
    si32 UT, UP, B3, B5, B6, X1, X2, X3, p;
    ui32 B4, B7;

    UT = readRawTemperature();
    UP = readRawPressure();

  #if DEBUG == 1
    // use datasheet numbers!
    UT = 27898;
    UP = 23843;
    ac6 = 23153;
    ac5 = 32757;
    mc = -8711;
    md = 2868;
    b1 = 6190;
    b2 = 4;
    ac3 = -14383;
    ac2 = -72;
    ac1 = 408;
    ac4 = 32741;
    oversampling = 0;
  #endif

    // do temperature calculations
    X1=(UT-(si32)(ac6))*((si32)(ac5))/pow(2,15);
    X2=((si32)mc*pow(2,11))/(X1+(si32)md);
    B5=X1 + X2;

  #if DEBUG == 1
    Serial.print("X1 = "); Serial.println(X1);
    Serial.print("X2 = "); Serial.println(X2);
    Serial.print("B5 = "); Serial.println(B5);
  #endif

    // do pressure calcs
    B6 = B5 - 4000;
    X1 = ((si32)b2 * ( (B6 * B6)>>12 )) >> 11;
    X2 = ((si32)ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((si32)ac1*4 + X3) << oversampling) + 2) / 4;

  #if DEBUG == 1
    Serial.print("B6 = "); Serial.println(B6);
    Serial.print("X1 = "); Serial.println(X1);
    Serial.print("X2 = "); Serial.println(X2);
    Serial.print("B3 = "); Serial.println(B3);
  #endif

    X1 = ((si32)ac3 * B6) >> 13;
    X2 = ((si32)b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((ui32)ac4 * (ui32)(X3 + 32768)) >> 15;
    B7 = ((ui32)UP - B3) * (ui32)( 50000UL >> oversampling );

  #if DEBUG == 1
    Serial.print("X1 = "); Serial.println(X1);
    Serial.print("X2 = "); Serial.println(X2);
    Serial.print("B4 = "); Serial.println(B4);
    Serial.print("B7 = "); Serial.println(B7);
  #endif

    if (B7 < 0x80000000) {
      p = (B7 * 2) / B4;
    } else {
      p = (B7 / B4) * 2;
    }
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

  #if DEBUG == 1
    Serial.print("p = "); Serial.println(p);
    Serial.print("X1 = "); Serial.println(X1);
    Serial.print("X2 = "); Serial.println(X2);
  #endif

    p = p + ((X1 + X2 + (si32)3791)>>4);
  #if DEBUG == 1
    Serial.print("p = "); Serial.println(p);
  #endif
    return p;
  }

  ui16 readRawTemperature(void)
  {
    write8(CONTROL, READTEMPCMD);
    _delay_ms(5);
  #if DEBUG == 1
    Serial.print("Raw temp: "); Serial.println(read16(TEMPDATA));
  #endif
    return read16(TEMPDATA);
  }

  ui32 readRawPressure(void) {
    ui32 raw;

    write8(CONTROL, READPRESSURECMD + (oversampling << 6));

    if (oversampling == ULTRALOWPOWER) 
      _delay_ms(5);
    else if (oversampling == STANDARD) 
      _delay_ms(8);
    else if (oversampling == HIGHRES) 
      _delay_ms(14);
    else 
      _delay_ms(26);

    raw = read16(PRESSUREDATA);

    raw <<= 8;
    raw |= read8(PRESSUREDATA+2);
    raw >>= (8 - oversampling);

   /* this pull broke stuff, look at it later?
    if (oversampling==0) {
      raw <<= 8;
      raw |= read8(PRESSUREDATA+2);
      raw >>= (8 - oversampling);
    }
   */

  #if DEBUG == 1
    Serial.print("Raw pressure: "); Serial.println(raw);
  #endif
    return raw;
  }

private:
  ui8 read8(ui8 addr)
  {
    ui8 ret;

    Wire.beginTransmission(I2CADDR); // start transmission to device 
  #if (ARDUINO >= 100)
    Wire.write(addr); // sends register address to read from
  #else
    Wire.send(addr); // sends register address to read from
  #endif
    Wire.endTransmission(); // end transmission
    
    Wire.beginTransmission(I2CADDR); // start transmission to device 
    Wire.requestFrom(I2CADDR, 1);// send data n-bytes read
  #if (ARDUINO >= 100)
    ret = Wire.read(); // receive DATA
  #else
    ret = Wire.receive(); // receive DATA
  #endif
    Wire.endTransmission(); // end transmission

    return ret;
  }

  ui16 read16(ui8 addr)
  {
    ui16 ret;

    Wire.beginTransmission(I2CADDR); // start transmission to device 
  #if (ARDUINO >= 100)
    Wire.write(addr); // sends register address to read from
  #else
    Wire.send(addr); // sends register address to read from
  #endif
    Wire.endTransmission(); // end transmission
    
    Wire.beginTransmission(I2CADDR); // start transmission to device 
    Wire.requestFrom(I2CADDR, 2);// send data n-bytes read
  #if (ARDUINO >= 100)
    ret = Wire.read(); // receive DATA
    ret <<= 8;
    ret |= Wire.read(); // receive DATA
  #else
    ret = Wire.receive(); // receive DATA
    ret <<= 8;
    ret |= Wire.receive(); // receive DATA
  #endif
    Wire.endTransmission(); // end transmission

    return ret;
  }

  void write8(ui8 addr, ui8 data)
  {
    Wire.beginTransmission(I2CADDR); // start transmission to device 
  #if (ARDUINO >= 100)
    Wire.write(addr); // sends register address to read from
    Wire.write(data);  // write data
  #else
    Wire.send(addr); // sends register address to read from
    Wire.send(data);  // write data
  #endif
    Wire.endTransmission(); // end transmission
  }


  ui8 oversampling;

  si16 ac1, ac2, ac3, b1, b2, mb, mc, md;
  ui16 ac4, ac5, ac6;
};

