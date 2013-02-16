#pragma once
#include "Pin.h"
#include "Delay.h"

//http://blinkm-projects.googlecode.com/svn/trunk/blinkm_examples/arduino/BlinkMSoftI2CDemo/SoftI2CMaster.cpp

class CI2CPin : public CPin
{
public:
	void High()
	{
		Float();
	}
};

class CI2C : public CDelay
{
	enum {
		I2C_ACK = 1, 
		I2C_NAK = 0
	};

	CI2CPin m_Sda;
	CI2CPin m_Scl;

	int m_nRead;

public:
	void Create( int nSdaPortPin, int nSclPortPin )
	{
		m_Sda.Create( nSdaPortPin );
		m_Scl.Create( nSclPortPin );

		Init();
	}

	void begin()
	{
		start();
		m_nRead = 0;
	}

	ui8 beginTransmission(ui8 address)
	{
		start(); 
		ui8 rc = write( (address<<1) | 0 ); // clr read bit
		return rc;
	}

	ui8 requestFrom(ui8 address, int nReadCount)
	{
		m_nRead = nReadCount;
		start();
		ui8 rc = write((address<<1) | 1); // set read bit
		return rc;
	}

	ui8 endTransmission(void)
	{
		stop();
		return 0;
	}

	ui8 read()
	{
		if ( m_nRead > 1 )
		{
			m_nRead--;
			return _read( I2C_ACK );
		}
		return _read( I2C_NAK );
	}

	ui8 write( ui8 c )
	{
		return _write( c );
	}

private:	
	void Delay()
	{
		DelayUs(10);
	}

	void writebit( ui8 c )
	{
		if ( c )
			m_Sda.High();
		else
			m_Sda.Low();

		m_Scl.High();
		Delay();

		m_Scl.Low();
		Delay();

		if ( c ) 
			m_Sda.Low();
		Delay();
	}

	ui8 readbit(void)
	{
		m_Sda.High();
		m_Scl.High();
		Delay();

		ui8 c = m_Sda.Get();
		m_Scl.Low();
		Delay();

		return c;
	}

	void Init()
	{
		m_Sda.High();
		m_Scl.High();
		Delay();
	}

	void start(void)
	{
		m_Sda.High();
		m_Scl.High();
		Delay();

		m_Sda.Low();
		Delay();

		m_Scl.Low();
		Delay();
	}

	void stop(void)
	{
		m_Scl.High();
		Delay();

		m_Sda.High();
		Delay();
	}

	ui8 _write( ui8 c )
	{
		for ( int i = 0; i < 8; i++ ) 
		{
			writebit( c & 128 );
			c <<= 1;
		}
		return readbit();
	}

	ui8 _read( ui8 ack )
	{
		ui8 res = 0;

		for ( int i=0;i<8;i++) 
		{
			res <<= 1;
			res |= readbit();  
		}

		if ( ack )
			writebit( 0 );
		else
			writebit( 1 );

		Delay();
		return res;
	}
};
