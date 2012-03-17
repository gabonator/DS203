#include "MainWnd.h"
//#include <Source/Framework/Eval.h>
#include <Source/Core/Sdk.h>


#include <stdarg.h>

/*static*/ CMainWnd	*CMainWnd::m_pInstance = NULL;

//innt px = 0, py = 0;
//	#define xprintf(...) px += BIOS::LCD::Printf(px<<3, py*14, RGB565(ff0000), RGB565(000000), __VA_ARGS__)


class MT
{ 
public:
  int val;
  MT()
  {
    val = 177;
  }
/*
  static MT* get()
  {
    static MT m;
    return &m;
  }*/
};

static const MT mt;

void CMainWnd::Create()
{
	m_pInstance = this;

	// image correctly uploaded in ROM ?
	_ASSERT( 
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][0] == '1' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][1] == 's' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][2] == 0 );


/*
//      stest();

	char strTest[32];
//	strcpy(strTest, "(15+(7*2-8)/2)*2+4");
	strcpy(strTest, "fun()");
	CEval e;

	CEval::CEvalOperand op = e.Eval( strTest );



	xprintf("Result = "); 

	switch ( op.m_eType )
	{
		case CEval::CEvalOperand::eoError: 
			xprintf("Error"); 
			break;
		case CEval::CEvalOperand::eoFloat: 
			xprintf("%d (float->int)", op.GetInteger()); 
			break;
		case CEval::CEvalOperand::eoInteger: 
			xprintf("%d (si32)", op.m_Data.m_lData); 
			break;
		case CEval::CEvalOperand::eoString: 
			op.m_Data.m_pString[ op.m_Data.m_pData32[1] ] = 0;
			xprintf("%s (string)", op.m_Data.m_pString); 
			break;
		case CEval::CEvalOperand::eoStream:  {
			CStream* pStream = op.m_Data.m_pStream;
			xprintf(" (stream, ptr=%x)", (int)pStream); 
			xprintf("tst"); 

			int ns = pStream->GetSize();
			xprintf(" (len=%d)", ns); 

			for ( int i = 0; i < pStream->GetSize(); i++ )
				xprintf("%c", pStream->Get()); 
			break; }
		case CEval::CEvalOperand::eoNone: 
			xprintf("none");
			break;
		default:
			xprintf("unknown");
			break;
	}

//BIOS::LCD::Printf(0, 0, RGB565(ff0000), RGB565(000000), "Result=%d", op.GetLong());
//static int Printf (int x, int y, unsigned short clrf, unsigned short clrb, const char* format, ...);


	py++; px=0;

	FILEINFO f;

	unsigned int t = 1;


	xprintf("test=%d %d %d", (int)1, (int)2, (int)3); py++; px=0;

	t = BIOS::DSK::Open(&f, (si8*)"TEST1   TXT", BIOS::DSK::IoWrite); xprintf("Open=%d", t); py++; px=0;
//	xprintf("m=%d s=%d, cl=%x %x %x, d=%x", f.nMode, f.nSectors, f.pCluster[0], f.pCluster[1], f.pCluster[2], f.pDirAddr[0]); py+=2; px=0;
	char* buf = (char*)BIOS::DSK::GetSharedBuffer();
	memset(	buf, 0, FILEINFO::SectorSize);
	strcpy( buf, "Tak toto je kruty experiment!");
	t=BIOS::DSK::Write( &f, (ui8*)buf ); _ASSERT( t );  xprintf("Write=%d", t); py++; px=0;
//	xprintf("m=%d s=%d, cl=%x %x %x, d=%x", f.nMode, f.nSectors, f.pCluster[0], f.pCluster[1], f.pCluster[2], f.pDirAddr[0]); py+=2; px=0;
	t=BIOS::DSK::Close( &f );_ASSERT( t ); xprintf("Close=%d", t); py++; px=0;
	//xprintf("m=%d s=%d, cl=%x %x %x, d=%x", f.nMode, f.nSectors, f.pCluster[0], f.pCluster[1], f.pCluster[2], f.pDirAddr[0]); py+=2; px=0;


	ui32 lTick=BIOS::GetTick();	
	while (BIOS::GetTick()-lTick < 10000) ;
*/
	
	BIOS::DBG::Print("set= %x !\n", (int)CSettings::m_pInstance);
       	BIOS::DBG::Print("mt= %x, val=%d !\n", (int)&mt, mt.val);

//	for (int i=0; i<10; i++)
//	{
//		BIOS::DBG::Print("Pokus %d !\n", i);
//		BIOS::DelayMs(500);
//	}

	CWnd::Create("CMainWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, BIOS::LCD::LcdHeight), NULL );
	m_wndToolBar.Create( this );
	m_wndGraph.Create( this, WsVisible | WsNoActivate );
	m_wndSignalGraph.Create( this, WsNoActivate );

	m_wndMenuInput.Create( this, WsVisible );
	m_wndMenuCursor.Create( this, WsHidden );
	m_wndMenuMeas.Create( this, WsHidden );
	m_wndMenuMath.Create( this, WsHidden );
	m_wndMenuSettings.Create( this, WsHidden );
	m_wndMenuDisplay.Create( this, WsHidden );
	m_wndMenuGenerator.Create( this, WsHidden );
	m_wndMenuTools.Create( this, WsHidden );
	m_wndZoomBar.Create( this, &m_wndGraph );
	m_wndLReferences.Create( this, WsVisible );
	m_wndTReferences.Create( this, WsVisible );

	m_wndToolBar.SetFocus();
	//m_wndMenuInput.m_itmCH1.SetFocus();
	//m_wndMenuGenerator.m_itmBpm.SetFocus();
	//OnMessage( &m_wndToolBar, ToWord('f', 'c'), 0 ); // force update

	SetTimer(1000);
}

/*virtual*/ void CMainWnd::OnTimer()
{
	static FILEINFO fbase;
	static bool bInit = TRUE;
	FILEINFO f;

	char* buf = (char*)BIOS::DSK::GetSharedBuffer();

	if ( bInit )
	{
		if ( BIOS::DSK::Open(&f, (si8*)"SDK     BIN", BIOS::DSK::IoRead) )
		{
			fbase = f;
			BIOS::DBG::Print("SDK File found!\n");

			memset(buf, 0, FILEINFO::SectorSize);       	
			strcpy(buf, "ANS=(text, msg) Ready... Type 'REQ=your expression'");
			_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf ) );
//			fbase.nMode = BIOS::DSK::IoRead;
			bInit = FALSE;
		} else
			return;
/*
		{
			_ASSERT_VALID( BIOS::DSK::Open(&f, (si8*)"TEST2   TXT", BIOS::DSK::IoWrite) );
			fbase = f;
			fbase.nMode = BIOS::DSK::IoRead;

			memset(buf, 0, FILEINFO::SectorSize);       	
			strcpy(buf, "ANS=Ready... Type 'REQ=your expression'");
			_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf ) );

			for ( int i=0; i<127; i++ )
			{
							memset(buf, 0, FILEINFO::SectorSize);       	
							strcpy(buf, "Dummy (sector begin)");

      				_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf ) );
							// calling this corrupts the shared buffer
			}

			_ASSERT_VALID( BIOS::DSK::Close(&f) ); // allocate 64kB
			BIOS::DelayMs(250);
			bInit = 0;
			return;
		}
		//fbase = f;
//		bInit = 0;
*/
	}	

	buf[0] = 0;
	f = fbase;
	_ASSERT_VALID( BIOS::DSK::Read(&f, (ui8*)buf) );
//	_ASSERT_VALID( BIOS::DSK::Close(&f) );
		
	if ( memcmp(buf, "REQ=", 4) == 0 )
	{
		char buf2[512];
		strcpy(buf2, buf);
		char first[512];

		// _ASSERT_VALID( BIOS::DSK::Open(&f, (si8*)"TEST1   TXT", BIOS::DSK::IoWrite) );
		// Opening file for writing is actually the same as for reading in case that it exists
		f = fbase;
		f.nMode = BIOS::DSK::IoWrite;


//		BIOS::DBG::Print("Evaluating[%s]", buf2);
		CSdkEval e;
		CSdkEval::CEvalOperand opResult = e.Eval(buf2+4);     

		memset(buf, 0, FILEINFO::SectorSize);       	
		strcpy(buf, "ANS=?");
		if ( opResult.m_eType == CEval::CEvalOperand::eoVariable )
		{
			opResult = opResult.m_Data.m_pVariable->Get();
		}

		switch ( opResult.m_eType )               
		{
			case CEval::CEvalOperand::eoError: 
				BIOS::DBG::sprintf(buf, "ANS=(text, msg) Error"); 
				BIOS::DBG::Print("Erorr Evaluating '%s'", buf2);
				break;
			case CEval::CEvalOperand::eoFloat: 
				BIOS::DBG::sprintf(buf, "ANS=(text, float->int) %d", opResult.GetInteger()); 
				break;
			case CEval::CEvalOperand::eoInteger: 	
				BIOS::DBG::sprintf(buf, "ANS=(text, si32) %d", opResult.m_Data.m_iData);	 
				break;
			case CEval::CEvalOperand::eoString: 
				opResult.m_Data.m_pString[ opResult.m_Data.m_pData32[1] ] = 0;
				BIOS::DBG::sprintf(buf, "ANS=(text, string) %s", opResult.m_Data.m_pString); 
				break;
			case CEval::CEvalOperand::eoStream:  {
				CEval::CStream* pStream = opResult.m_Data.m_pStream;
				BIOS::DBG::sprintf(buf, "ANS=(binary, len=%d)", pStream->GetSize());
				int nOfs = strlen(buf);
				int i;
				int nPage =0;
				for ( i = 0; i < pStream->GetSize(); i++ )
				{
					buf[nOfs++] = pStream->Get(); 
					if ( nOfs >= FILEINFO::SectorSize )
					{
						if (nPage == 0)
						{
							memcpy(first, (ui8*)buf, sizeof(first));
							memcpy(buf, "____", 4);
							nPage++;
						}
						_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf) );
						memset((ui8*)buf, 0, FILEINFO::SectorSize);
						nOfs = 0;
					}
				}
				buf[nOfs] = 0;
				_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf) );
				//_ASSERT_VALID( BIOS::DSK::Close(&f) );
				f = fbase;
				_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)first) );
				return;
				break; }
			case CEval::CEvalOperand::eoNone: 
				BIOS::DBG::sprintf(buf, "ANS=(text, msg) none");
				break;
			default:
				BIOS::DBG::sprintf(buf, "ANS=(text, msg) unknown");
				break;
		}
//		BIOS::DBG::Print("Result[%s]", buf);

		_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf ) );
		//_ASSERT_VALID( BIOS::DSK::Close(&f) );
		return;
	}
	if ( memcmp(buf, "ANS=", 4) == 0 )
	{
		return;
	}
	bInit = TRUE; 
 	BIOS::DBG::Print("t=%d rd='%s' ", BIOS::GetTick(), buf);
}

/*virtual*/ void CMainWnd::OnPaint()
{
	BIOS::LCD::Clear(RGB565(000000));
}

/*virtual*/ void CMainWnd::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_wndToolBar )
	{
		if ( code == ToWord('f', 'c') )	// focus changed
		{
			m_wndMenuInput.ShowWindow( ( data == 0 ) ? SwShow : SwHide );
			m_wndMenuCursor.ShowWindow( ( data == 1 ) ? SwShow : SwHide );
			m_wndMenuMeas.ShowWindow( ( data == 2 ) ? SwShow : SwHide );
			m_wndMenuMath.ShowWindow( ( data == 3 ) ? SwShow : SwHide );
			m_wndMenuDisplay.ShowWindow( ( data == 4 ) ? SwShow : SwHide );
			m_wndMenuGenerator.ShowWindow( ( data == 5 ) ? SwShow : SwHide );
			m_wndMenuSettings.ShowWindow( ( data == 6 ) ? SwShow : SwHide );
			m_wndMenuTools.ShowWindow( ( data == 7 ) ? SwShow : SwHide );

			m_wndGraph.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndZoomBar.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndLReferences.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndTReferences.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndSignalGraph.ShowWindow( ( data == 5 ) ? SwShow : SwHide );
			Invalidate();
		}
	}
	if (code == ToWord('d', 'g'))
	{
		if ( m_wndGraph.m_dwFlags & WsVisible )
			m_wndGraph.Invalidate();
	}
}

