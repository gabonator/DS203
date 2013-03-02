#ifndef __TOOLBOX_BUFFEREDIO_H__
#define __TOOLBOX_BUFFEREDIO_H__

class CBufferedWriter : public CSerialize
{
	ui8* m_pData;
	int m_nOffset;
	int m_nSize;
	FILEINFO m_FileInfo;

public:
	void Open( PSTR strName )
	{
		m_pData = (ui8*)BIOS::DSK::GetSharedBuffer();
		m_nOffset = 0;
		m_nSize = 0;

		if ( !BIOS::DSK::Open( &m_FileInfo, strName, BIOS::DSK::IoWrite ) )
		{
			_ASSERT(0);
			return;
		}
#ifdef _WIN32
		memset(m_pData, 0, FILEINFO::SectorSize);
#endif
	}

	virtual CBufferedWriter& operator <<( PSTR str )
	{
		CStream stream(str);
		*this << stream;
		return *this;
	}

	virtual CBufferedWriter& operator <<( PCSTR str )
	{
		CStream stream((char*)str);
		*this << stream;
		return *this;
	}

	virtual CBufferedWriter& operator <<( ui32 dwData )
	{
		*this << CStream(dwData);
		return *this;
	}

	virtual CBufferedWriter& operator <<( ui16 wData )
	{
		*this << CStream(wData);
		return *this;
	}

	virtual CBufferedWriter& operator <<( ui8 dwData )
	{
		*this << CStream(dwData);
		return *this;
	}

	virtual CBufferedWriter& operator <<( const CStream& stream_ )
	{
		// ugly conversion, GCC requires const
		CStream& stream = *(CStream*)&stream_;
		for (int i = 0; i < stream.GetLength(); i++ )
		{
			m_pData[m_nOffset++] = stream[i];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::DSK::Write( &m_FileInfo, m_pData );
#ifdef _WIN32
				memset( m_pData, 0x20, FILEINFO::SectorSize );
#endif
			}
		}
		m_nSize += stream.GetLength();
		return *this;
	}

	void Close()
	{
		if ( m_nOffset > 0 )
			BIOS::DSK::Write( &m_FileInfo, m_pData );
		BIOS::DSK::Close( &m_FileInfo, m_nSize );
	}
};

class CBufferedReader : public CSerialize
{
	ui8* m_pData;
	int m_nOffset;
	FILEINFO m_FileInfo;

public:
	bool Open( PSTR strName )
	{
		m_pData = (ui8*)BIOS::DSK::GetSharedBuffer();
		m_nOffset = 0;

		if ( !BIOS::DSK::Open( &m_FileInfo, strName, BIOS::DSK::IoRead ) )
		{
			//_ASSERT(0);
			return false;
		}
		BIOS::DSK::Read( &m_FileInfo, m_pData );
		return true;
	}

	CBufferedReader& operator >>( PSTR str )
	{
		// unsafe!
		int i;
		int nLimit = 64;
		for ( i = 0; i < nLimit-1; i++ )
		{
			str[i] = m_pData[m_nOffset++];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::DSK::Read( &m_FileInfo, m_pData );
			}
			if ( str[i] == '\n' )
				break;
		}
		if ( str[i] == '\r' )
			i--;
		str[i] = 0;
		return *this;
	}
	
	CBufferedReader& operator >>( ui32 &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	CBufferedReader& operator >>( ui8 &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	CBufferedReader& operator >>( const ui8 &i )
	{
		// ugly! but MSVC requires const...
		CStream stream(*(ui8*)&i);
		return *this >> stream;
	}

	virtual CBufferedReader& operator >>( const CStream& stream_ )
	{
		// ugly conversion, GCC requires const
		CStream& stream = *(CStream*)&stream_;
		for (int i = 0; i < stream.GetLength(); i++ )
		{
			stream[i] = m_pData[m_nOffset++];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::DSK::Read( &m_FileInfo, m_pData );
			}
		}
		return *this;
	}

	void Close()
	{
		BIOS::DSK::Close( &m_FileInfo );
	}
};

class CBufferedReader2 : public CSerialize
{
	ui8* m_pData;
	int m_nOffset;

public:
	bool Open( PSTR strName )
	{
		m_pData = (ui8*)BIOS::DSK::GetSharedBuffer();
		m_nOffset = 0;

		if ( BIOS::FAT::Open( strName, BIOS::DSK::IoRead ) != BIOS::FAT::EOk )
			return false;
		return BIOS::FAT::Read( m_pData ) == BIOS::FAT::EOk;
	}
	ui32 GetFileSize()
	{
		return BIOS::FAT::GetFileSize();
	}
	CBufferedReader2& operator >>( PSTR str )
	{
		// unsafe!
		int i;
		int nLimit = 64;
		for ( i = 0; i < nLimit-1; i++ )
		{
			str[i] = m_pData[m_nOffset++];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::FAT::EResult eResult = BIOS::FAT::Read( m_pData );
				_ASSERT( eResult == BIOS::FAT::EOk );
			}
			if ( str[i] == '\n' )
				break;
		}
		if ( str[i] == '\r' )
			i--;
		str[i] = 0;
		return *this;
	}
	
	CBufferedReader2& operator >>( ui32 &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	CBufferedReader2& operator >>( ui8 &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	CBufferedReader2& operator >>( int &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	CBufferedReader2& operator >>( const ui8 &i )
	{
		// ugly! but MSVC requires const...
		CStream stream(*(ui8*)&i);
		return *this >> stream;
	}

	virtual CBufferedReader2& operator >>( const CStream& stream_ )
	{
		// ugly conversion, GCC requires const
		CStream& stream = *(CStream*)&stream_;
		for (int i = 0; i < stream.GetLength(); i++ )
		{
			stream[i] = m_pData[m_nOffset++];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::FAT::EResult eResult = BIOS::FAT::Read( m_pData );
				_ASSERT( eResult == BIOS::FAT::EOk );
			}
		}
		return *this;
	}

	void Close()
	{
		BIOS::FAT::Close();
	}
};

class CBufferedWriter2 : public CSerialize
{
	ui8* m_pData;
	int m_nOffset;
	int m_nSize;

public:
	bool Open( PSTR strName )
	{
		m_pData = (ui8*)BIOS::DSK::GetSharedBuffer();
		m_nOffset = 0;
		m_nSize = 0;

		if ( BIOS::FAT::Open( strName, BIOS::DSK::IoWrite ) != BIOS::FAT::EOk )
		{
			_ASSERT(0);
			return false;
		}

#ifdef _WIN32
		memset(m_pData, 0, FILEINFO::SectorSize);
#endif
		return true;
	}

	virtual CBufferedWriter2& operator <<( PSTR str )
	{
		CStream stream(str);
		*this << stream;
		return *this;
	}

	virtual CBufferedWriter2& operator <<( PCSTR str )
	{
		CStream stream((char*)str);
		*this << stream;
		return *this;
	}

	virtual CBufferedWriter2& operator <<( ui32 dwData )
	{
		*this << CStream(dwData);
		return *this;
	}

	virtual CBufferedWriter2& operator <<( int dwData )
	{
		*this << CStream(dwData);
		return *this;
	}

	virtual CBufferedWriter2& operator <<( ui16 wData )
	{
		*this << CStream(wData);
		return *this;
	}

	virtual CBufferedWriter2& operator <<( ui8 dwData )
	{
		*this << CStream(dwData);
		return *this;
	}

	virtual CBufferedWriter2& operator <<( const CStream& stream_ )
	{
		// ugly conversion, GCC requires const
		CStream& stream = *(CStream*)&stream_;
		for (int i = 0; i < stream.GetLength(); i++ )
		{
			m_pData[m_nOffset++] = stream[i];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::FAT::EResult eResult = BIOS::FAT::Write( m_pData );
				_ASSERT( eResult == BIOS::FAT::EOk );
#ifdef _WIN32
				memset( m_pData, 0x20, FILEINFO::SectorSize );
#endif
			}
		}
		m_nSize += stream.GetLength();
		return *this;
	}

	void Close()
	{
		if ( m_nOffset > 0 )
			BIOS::FAT::Write( m_pData );
		BIOS::FAT::Close( m_nSize );
	}
};

#endif