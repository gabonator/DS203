#include "Import.h"

bool CImport::LoadWave(char* strName /*= NULL*/)
{
	CBufferedReader reader;
	if ( !reader.Open( strName ) )
		return false;

	typedef ui16 WORD;
	typedef ui32 DWORD;

	struct WAVEFORMATEX 
	{
	  WORD  wFormatTag;
	  WORD  nChannels;
	  DWORD nSamplesPerSec;
	  DWORD nAvgBytesPerSec;
	  WORD  nBlockAlign;
	  WORD  wBitsPerSample;
	  WORD  cbSize;
	} WaveFormat;

	ui32 nJunkSize = 14*4;
	ui32 dwCheckSize = sizeof( WAVEFORMATEX ) + 4096*2 + 6*4 + nJunkSize;
	ui32 dwRiffChunk, dwRiffSize;
	reader >> dwRiffChunk >> dwRiffSize;

	if ( dwRiffChunk != ToDword('R', 'I', 'F', 'F') || dwRiffSize != dwCheckSize )
	{
		reader.Close();
		return false;
	}

	ui32 dwWaveChunk, dwFmtChunk, dwFmtSize;
	reader >> dwWaveChunk >> dwFmtChunk >> dwFmtSize;
	if ( dwWaveChunk != ToDword('W', 'A', 'V', 'E') || 
		dwFmtChunk != ToDword('f', 'm', 't', ' ') ||
		dwFmtSize != sizeof(WaveFormat) )
	{
		reader.Close();
		return false;
	}

	reader >> CStream(&WaveFormat, sizeof(WaveFormat));

	ui32 dwDataChunk, dwDataSize;
	reader >> dwDataChunk >> dwDataSize;
	if ( dwDataChunk != ToDword('d', 'a', 't', 'a') || dwDataSize != 4096*2 )
	{
		reader.Close();
		return false;
	}

	for (int i=0; i< (int)BIOS::ADC::GetCount(); i++)
	{
		BIOS::ADC::SSample& Sample = *(BIOS::ADC::SSample*)&BIOS::ADC::GetAt(i);
		reader 
			>> Sample.CH[0]
			>> Sample.CH[1];
	}

	// junk
	ui32 dwJunkChunk, dwJunkSize, dwJunkHead;
	reader >> dwJunkChunk >> dwJunkSize >> dwJunkHead;

	if ( dwJunkChunk != ToDword('J', 'U', 'N', 'K') || 
		dwJunkHead != ToDword('D', 'S', 'W', '1') ||
		dwJunkSize != nJunkSize )
	{
		reader.Close();
		return false;
	}

	ui32 dwKey = 0;
	while ( dwKey != ToDword('E', 'N', 'D', 27) )
	{
		reader >> dwKey;
		switch ( dwKey )
		{
			case ToDword('C', 'H', '1', ' '):
			{
				ui32 dwResolution, dwCoupling, dwPosition;
				reader >> dwResolution >> dwCoupling >> dwPosition;
				Settings.CH1.Resolution = (CSettings::AnalogChannel::eResolution)dwResolution;
				Settings.CH1.Coupling = (CSettings::AnalogChannel::eCoupling)dwCoupling;
				Settings.CH1.u16Position = (ui16)dwPosition;
			}
			break;
			case ToDword('C', 'H', '2', ' '):
			{
				ui32 dwResolution, dwCoupling, dwPosition;
				reader >> dwResolution >> dwCoupling >> dwPosition;
				Settings.CH2.Resolution = (CSettings::AnalogChannel::eResolution)dwResolution;
				Settings.CH2.Coupling = (CSettings::AnalogChannel::eCoupling)dwCoupling;
				Settings.CH2.u16Position = (ui16)dwPosition;
			}
			break;
			case ToDword('T', 'I', 'M', 'E'):
			{
				ui32 dwResolution;
				reader >> dwResolution;
				Settings.Time.Resolution = (CSettings::TimeBase::EResolution)dwResolution;
			}
			break;
			case ToDword('E', 'N', 'D', 27):
			break;
			default:
				_ASSERT(0);
				break;
		}
	}

	reader.Close();
	return true;
}
