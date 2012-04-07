#include "Generator.h"
#include <Source/Gui/MainWnd.h>
             
struct SWaveData {
  const char* pszName;
  const ui16 nCount;
  const ui16* pWave;
  const ui16 nOversampling;
};

const ui16 pSineWave[] =
			{0x000,0x027,0x08E,0x130,0x209,0x311,0x441,0x58F,0x6F0,    // 90
			0x85A,0x9C0,0xB19,0xC59,0xD76,0xE68,0xF26,0xFAB,0xFF3,    // 180
			0xFFF,0xFD7,0xF70,0xECE,0xDF5,0xCED,0xBBD,0xA6F,0x90E,    // 270
			0x7A4,0x63E,0x4E5,0x3A5,0x288,0x196,0x0D8,0x053,0x00B};   // 360   
const ui16 pTriangleWave[] =
			{0x000,0x0E3,0x1C6,0x2AA,0x38D,0x471,0x554,0x638,0x71B,    // 90
			0x7FF,0x8E2,0x9C6,0xAA9,0xB8D,0xC70,0xD54,0xE37,0xF1B,    // 180
			0xFFE,0xF1B,0xE37,0xD54,0xC70,0xB8D,0xAA9,0x9C6,0x8E2,    // 270
			0x7FF,0x71B,0x638,0x554,0x471,0x38D,0x2AA,0x1C6,0x0E3};    // 360   
const ui16 pSawWave[] =
			{0x000,0x075,0x0EA,0x15F,0x1D4,0x249,0x2BE,0x333,0x3A8,    // 90
			0x41D,0x492,0x507,0x57C,0x5F1,0x666,0x6DB,0x750,0x7C5,    // 180
			0x83A,0x8AF,0x924,0x999,0xA0E,0xA83,0xAF8,0xB6D,0xBE2,    // 270
			0xC57,0xCCC,0xD41,0xDB6,0xE2B,0xEA0,0xF15,0xF8A,0xFFF};    // 360    
const ui16 pSquareWave[] =
			{0x000,0xfff};    // 90

const ui16 pCardiac128Wave[] = {
/*
0x571, 0x55a, 0x556, 0x557, 0x55a, 0x558, 0x561, 0x56b, 0x56a, 0x55f, 0x556, 0x54e, 0x55d, 0x561, 0x560, 0x565, 
0x567, 0x570, 0x57c, 0x591, 0x5aa, 0x5bc, 0x5b0, 0x59a, 0x590, 0x576, 0x557, 0x549, 0x545, 0x549, 0x54c, 0x550, 
0x550, 0x54d, 0x540, 0x550, 0x560, 0x55d, 0x558, 0x514, 0x413, 0x43f, 0x567, 0x77e, 0xa88, 0xc21, 0xccc, 0x92d, 
0x5a3, 0x41a, 0x333, 0x3b9, 0x3ff, 0x43b, 0x458, 0x475, 0x487, 0x496, 0x4b8, 0x4d4, 0x4e6, 0x4f4, 0x50f, 0x523, 
0x52d, 0x543, 0x555, 0x566, 0x56e, 0x577, 0x589, 0x597, 0x5a5, 0x5ae, 0x5ba, 0x5bb, 0x5c9, 0x5dc, 0x5e5, 0x5ec, 
0x5e7, 0x5ed, 0x5f6, 0x5f5, 0x5ee, 0x5ec, 0x5e1, 0x5c6, 0x5b0, 0x58a, 0x550, 0x515, 0x4db, 0x4b3, 0x49f, 0x48f, 
0x485, 0x484, 0x486, 0x497, 0x4ac, 0x4bf, 0x4c9, 0x4d0, 0x4de, 0x4f1, 0x500, 0x50d, 0x518, 0x51b, 0x51c, 0x525, 
0x530, 0x530, 0x531, 0x530, 0x52c, 0x533, 0x539, 0x537, 0x53b, 0x53d, 0x540, 0x547, 0x54f, 0x549, 0x548, 0x53f
*/
	0x3bd, 0x397, 0x391, 0x391, 0x396, 0x393, 0x3a2, 0x3b3, 0x3b1, 0x39f, 0x38f, 0x383, 0x39b, 0x3a3, 0x3a0, 0x3aa, 
	0x3ac, 0x3bc, 0x3d0, 0x3f2, 0x41c, 0x439, 0x425, 0x401, 0x3f1, 0x3c5, 0x392, 0x37a, 0x374, 0x37a, 0x37f, 0x386, 
	0x385, 0x381, 0x36c, 0x385, 0x3a1, 0x39c, 0x394, 0x321, 0x176, 0x1bf, 0x3ad, 0x728, 0xc38, 0xee3, 0xfff, 0x9f6, 
	0x410, 0x181, 0x000, 0x0e0, 0x154, 0x1b8, 0x1e8, 0x219, 0x238, 0x24f, 0x288, 0x2b8, 0x2d6, 0x2ed, 0x319, 0x33a, 
	0x34c, 0x371, 0x38f, 0x3aa, 0x3b7, 0x3c7, 0x3e4, 0x3fc, 0x413, 0x423, 0x437, 0x439, 0x450, 0x470, 0x47f, 0x48a, 
	0x482, 0x48b, 0x49a, 0x499, 0x48e, 0x48a, 0x477, 0x44a, 0x426, 0x3e6, 0x386, 0x324, 0x2c3, 0x280, 0x25f, 0x244, 
	0x234, 0x233, 0x235, 0x251, 0x275, 0x295, 0x2a6, 0x2b0, 0x2c8, 0x2e7, 0x300, 0x316, 0x328, 0x32e, 0x32f, 0x33f, 
	0x350, 0x351, 0x352, 0x351, 0x34a, 0x356, 0x35f, 0x35d, 0x363, 0x367, 0x36b, 0x377, 0x384, 0x37a, 0x378, 0x369 };

ui16 pRamWave[128*4];

const SWaveData Waves[] = 
	{
		{"sine", 36, pSineWave, 0 },
		{"triangle", 36, pTriangleWave, 0 },
		{"sawtooth", 36, pSawWave, 0 },
		{"square", 2, pSquareWave, 0 },
		{"cardio", 128, pCardiac128Wave, 2 },
		{NULL, 0, NULL, 0}		
	};

/*static*/ ui16*	CWndMenuGenerator::GetRamDac()
{
	return pRamWave;
}

ui16 _GetCount(ui8 nWaveIndex)
{
	ui16 nCount = Waves[nWaveIndex].nCount;
	ui16 nSmooth = Waves[nWaveIndex].nOversampling;

	return nCount << nSmooth;
}

ui16* _GetWave(ui8 nWaveIndex)
{
	const ui16* pWave = Waves[nWaveIndex].pWave;
	ui16 nCount = Waves[nWaveIndex].nCount;
	ui16 nSmooth = Waves[nWaveIndex].nOversampling;
	_ASSERT( _GetCount(nWaveIndex) <= COUNT( pRamWave ) );

	if ( nSmooth <= 0 )
	{
		for (int i=0; i<nCount; i++)
			pRamWave[i] = pWave[i];
	} else
	{
		ui16* pSample = pRamWave;
		ui8 nSmoothVals = 1<<nSmooth;
		for (int i=0; i<nCount; i++)
		{
			ui16 nSample0 = pWave[i];
			ui16 nSample1 = pWave[(i+1)%nCount];
			
			for (ui8 j=0; j<nSmoothVals; j++)
			{
				ui32 nInterpolation = nSample0;
				nInterpolation += ( ( (ui32)(nSample1-nSample0) ) * j ) >> nSmooth;
				*pSample++ = (ui16)nInterpolation;
			}
		}
	}
	return pRamWave;
}

/*virtual*/ void CWndMenuGenerator::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuGenerator", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);

	m_proWave.Create( (const char**)CSettings::Generator::ppszTextWave,
		(NATIVEENUM*)&Settings.Gen.Wave, CSettings::Generator::_WaveMax );
	m_proFreq.Create( (si16*)&m_nFreq, 0, 20000 );
	m_proBpm.Create( (si16*)&m_nBpm, 1, 10000 );

	m_proPsc.Create( (si16*)&Settings.Gen.nPsc, 0, 32000 );
	m_proArr.Create( (si16*)&Settings.Gen.nArr, 0, 32000 );
	
	m_itmWave.Create( &m_proWave, "Wave", RGB565(b00040), this );
	m_itmFreq.Create( &m_proFreq, "Freq", RGB565(b00040), this );
	m_itmBpm.Create( &m_proBpm, "BPM", RGB565(b00040), this );
	m_itmPsc.Create( &m_proPsc, "Psc", RGB565(404040), this);
	m_itmArr.Create( &m_proArr, "Arr", RGB565(404040), this);

	//m_itmFrequency.Create("Freq\n1 kHz", RGB565(b00040), 2, this);
	//m_itmSingle.Create("Loop", RGB565(b00040), 2, this);
	//m_itmTransfer.Create("Transfer", RGB565(ffff00), 1, this);
	//m_itmFile.Create("File...", RGB565(ffff00), 1, this);
	
	OnMessage( &m_itmWave, ToWord('i', 'u'), 0 ); // force update
}

/*virtual*/ void CWndMenuGenerator::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	#define CPUCLOCK (72 MHz)
	#define MHz *1000000

	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwShow );
		return;
	}

	ui8 bWaveChanged = ( pSender == &m_itmWave );
	if ( code == ToWord('i', 'u') && pSender == &m_itmBpm )
	{
		// bpm = CPUCLOCK * 60 / 20 / (Settings.Gen.nArr+1) / nCount
		// Settings.Gen.nArr = CPUCLOCK * 3 / (bpm * nCount) -1

		ui8 nWaveIndex = (ui8)(NATIVEENUM)Settings.Gen.Wave;
		ui32 nTemp = CPUCLOCK * 3;
		ui32 nDiv = m_nBpm * _GetCount(nWaveIndex);
		nTemp /= nDiv;
		if (nTemp > 32000)
			nTemp = 32000;
		Settings.Gen.nArr = (si16) nTemp-1;
		m_itmPsc.Invalidate();
		m_itmArr.Invalidate();
	}

	if ( code == ToWord('i', 'u') )
	{
		if ( Settings.Gen.Wave == CSettings::Generator::_Sq )
			m_itmPsc.m_dwFlags &= ~WsNoActivate;
		else
			m_itmPsc.m_dwFlags |= WsNoActivate;

		ui8 nWaveIndex = (ui8)(NATIVEENUM)Settings.Gen.Wave;

		switch (Settings.Gen.Wave)
		{
		case CSettings::Generator::_Sq:
			{
				ui32 nTemp = CPUCLOCK;
				ui32 nDiv = (Settings.Gen.nPsc+1);
				nDiv *= (Settings.Gen.nArr+1);
				m_nBpm = (si16)( nTemp * 60 / nDiv );
				nTemp /= nDiv;
				m_nFreq = (si16)nTemp;

				m_itmFreq.m_dwFlags |= WsNoActivate;
				m_itmFreq.Invalidate();

				BIOS::GEN::ConfigureSq( Settings.Gen.nPsc, Settings.Gen.nArr, (Settings.Gen.nArr+1)>>1);
			}
			break;
		default:
			{
				ui32 nTemp = CPUCLOCK;
				ui32 nDiv = 20;
				nDiv *= (Settings.Gen.nArr+1);
				nDiv *= _GetCount(nWaveIndex); 
				m_nBpm = (si16)( nTemp * 3 / (nDiv/ 20) );
				nTemp /= nDiv;
				m_nFreq = (si16)nTemp;

				m_itmFreq.m_dwFlags |= WsNoActivate;
				m_itmFreq.Invalidate();
				if ( bWaveChanged )
					BIOS::GEN::ConfigureWave( _GetWave(nWaveIndex), _GetCount(nWaveIndex) );
				BIOS::GEN::ConfigureWaveRate( Settings.Gen.nArr );
			}
		}

		m_itmBpm.Invalidate();

		if ( pSender == &m_itmWave )
		{
			MainWnd.m_wndSignalGraph.Setup(Waves[nWaveIndex].pWave, Waves[nWaveIndex].nCount);
			MainWnd.m_wndSignalGraph.Invalidate();
			if ( Settings.Gen.Wave == CSettings::Generator::_Sq )
				m_itmBpm.m_dwFlags |= CWnd::WsNoActivate;
			else
				m_itmBpm.m_dwFlags &= ~CWnd::WsNoActivate;
		}
	}
}

