#pragma once
#include "Pin.h"
#include "Delay.h"

/*
	Usage:
	// connect device to UART-TX pin, no pullup necessary when the cable is short

	m_Dht.Create( BIOS::GPIO::PortA, 9 );
	if ( m_Dht.Init() && m_Dht.Read() && m_Dht.CheckCrc() )
	{
		float fTemp = m_Dht.GetTemperature();
		float fHum  = m_Dht.GetHumidity();	
	}

	Based on http://learn.adafruit.com/dht
           http://playground.arduino.cc/Main/DHT11Lib

	calling GetDewPoint cause device to freeze, probably the math functions are not linked
*/

class CDHT11 : public CPin, CDelay
{
	ui8 m_buffer[5];

public:
	ui8* GetBuffer()
	{
		return m_buffer;
	}

	bool Init()
	{
		// REQUEST SAMPLE
		Low();
		DelayMs(250);
		High();
		DelayUs(40);
		Float();

		// ACKNOWLEDGE or TIMEOUT
		if ( Delay( 1000, true ) == 1000 )
			return false;
		if ( Delay( 1000, false ) == 1000 )
			return false;

		return true;
	}

	int Delay(int nUs, bool bLevel)
	{
		int i;
		nUs *= 2;
		for ( i = 0; i < nUs; i += 3 )
		{
			if ( Get() != bLevel )
				break;
			DelayUs(1); // single loop takes probably 1.5us, multiply 3/2 x
		}
		if ( i >= nUs-3 )
			i = nUs;

		return i / 2;
	}

	bool Read()
	{
		// BUFFER TO RECEIVE
		ui8 cnt = 7;
		ui8 idx = 0;

		// EMPTY BUFFER
		for (int i=0; i<5; i++) 
			m_buffer[i] = 0;

		// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
		for (int i=0; i<40; i++)
		{
			if ( Delay( 200, true ) == 200 )
				return false;

			if ( Delay( 200, false ) == 200 )
				return false;

			int nTime = Delay( 200, true );

			if ( nTime > 25 )
				m_buffer[idx] |= 1 << cnt;

			if ( cnt == 0 )
			{
				cnt = 7;
				idx++;
			} else
				cnt--;
		}
		return true;
	}

	int GetHumidity()
	{
		return m_buffer[0];
	}

	int GetTemperature()
	{
		return m_buffer[2];
	}

	bool CheckCrc()
	{
		return ( m_buffer[4] == (ui8)(m_buffer[0] + m_buffer[2]) ) ? true : false;
	}

	float GetDewPoint()
	{
		const float a = 17.271f;
		const float b = 237.7f;
		float temp = (a * GetTemperature()) / (b + GetTemperature()) + log(GetHumidity()/100.0f);
		float Td = (b * temp) / (a - temp);
		return Td;
	}
};
