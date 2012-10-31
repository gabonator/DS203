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
	}

	virtual CBufferedWriter& operator <<( PSTR str )
	{
		CStream stream(str);
		*this << stream;
		return *this;
	}

	virtual CBufferedWriter& operator <<( CStream& stream )
	{
		for (int i = 0; i < stream.GetLength(); i++ )
		{
			m_pData[m_nOffset++] = stream[i];
			if ( m_nOffset == FILEINFO::SectorSize )
			{
				m_nOffset = 0;
				BIOS::DSK::Write( &m_FileInfo, m_pData );
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
	void Open( PSTR strName )
	{
		m_pData = (ui8*)BIOS::DSK::GetSharedBuffer();
		m_nOffset = 0;

		if ( !BIOS::DSK::Open( &m_FileInfo, strName, BIOS::DSK::IoRead ) )
		{
			_ASSERT(0);
			return;
		}
		BIOS::DSK::Read( &m_FileInfo, m_pData );
	}

	CBufferedReader& operator >>( PSTR str )
	{
		// unsafe!
		int i;
		int nLimit = 32;
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
		str[i] = 0;
		return *this;
	}
	
	CBufferedReader& operator >>( ui32 &i )
	{
		CStream stream(i);
		return *this >> stream;
	}

	virtual CBufferedReader& operator >>( CStream& stream )
	{
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

#endif