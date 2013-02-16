#pragma once
#include <Source/HwLayer/Bios.h>

class CPin
{
public:
	enum {
		PortA = BIOS::GPIO::PortA << 8,
		PortB = BIOS::GPIO::PortB << 8,
		PortC = BIOS::GPIO::PortC << 8,
		PortD = BIOS::GPIO::PortD << 8
	};

private:
	int m_nPort;
	int m_nPin;

public:
	void Create( int nPort, int nPin )
	{
		m_nPort = nPort;
		m_nPin = nPin;
	}

	void Create( int nPortPin )
	{
		m_nPort = nPortPin >> 8;
		m_nPin = nPortPin & 0xff;
		_ASSERT( m_nPort >= BIOS::GPIO::PortA && m_nPort <= BIOS::GPIO::PortD );
		_ASSERT( m_nPin >= 0 && m_nPin < 32 );
	}

	void High()
	{
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateSimpleOutput );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, true ); 
	}

	void Low()
	{
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateSimpleOutput );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, false ); 
	}

	void Float()
	{
		// with pull up
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateInput | BIOS::GPIO::StateInputPull );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, true ); 
	}

	bool Get()
	{
		return BIOS::GPIO::GetPin( m_nPort, m_nPin );
	}
};
