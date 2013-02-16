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
		if ( m_nRead > 0 )
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
/*
	ui8 send(ui8 data)
	{
		return i2c_write(data);
	}

	void send(ui8* data, ui8 quantity)
	{
		for(ui8 i = 0; i < quantity; ++i)
			send(data[i]);
	}

	void send(char* data)
	{
    send((ui8*)data, strlen(data));
	}

	void send(int data)
	{
		send((ui8)data);
	}
*/
	void writebit( ui8 c )
	{
		if ( c )
			m_Sda.High();
		else
			m_Sda.Low();

		m_Scl.High();
		DelayUs(50);

		m_Scl.Low();
		DelayUs(50);

		if ( c ) 
			m_Sda.Low();
		DelayUs(50);
	}

	ui8 readbit(void)
	{
		m_Sda.High();
    m_Scl.High();
    DelayUs(50);

		ui8 c = m_Sda.Get();
		m_Scl.Low();
		DelayUs(50);

    return c;
	}

	void Init()
	{
		m_Sda.High();
		m_Scl.High();
		DelayUs(50);
	}

	void start(void)
	{
		m_Sda.High();
		m_Scl.High();
		DelayUs(50);
   
    m_Sda.Low();
		DelayUs(50);

    m_Scl.Low();
		DelayUs(50);
	}

	void stop(void)
	{
		m_Scl.High();
		DelayUs(50);

		m_Sda.High();
		DelayUs(50);
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

		DelayUs(50);
		return res;
	}
/*
	ui8 receive( ui8 ack )
	{
		return read( ack );
	}

	ui8 receive()
	{
		return read( I2C_ACK );
	}

	ui8 receiveLast()
	{
		return read( I2C_NAK );
	}
*/
};
