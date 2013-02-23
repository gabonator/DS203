#define ENABLE_MODULE_BALLS

#if defined(ADD_MODULE) && defined(ENABLE_MODULE_BALLS)
	ADD_MODULE( "Balls", CWndBalls )
#endif

#if !defined(__BALLS_H__) && defined(ENABLE_MODULE_BALLS)
#define __BALLS_H__
#include <Source/Framework/Wnd.h>

class CBallGenerator 
{
public:
	int w, h, cx, cy, gx, gy, sx, sy, lc, lg, ls;

	CBallGenerator(int _w, int _h)
	{
		w = _w;
		h = _h;
		// 0 = left, 256 = right, 128 = center
		cx = (w*128)>>8;	// center		
		cy = (w*128)>>8;    
		gx = (w*153)>>8;	// glow
		gy = (h*179)>>8;
		sx = (w*179)>>8;  // shadow
		sy = (h*205)>>8;
		lc = (60*w)>>7;		
		lg = (70*w)>>7;
		ls = (50*w)>>7;
	}

	void Render(ui16* pPixels, ui32 nColor)
	{
		Render( pPixels, GetColorR(nColor), GetColorG(nColor), GetColorB(nColor), 230);
	}
		
	void Render(ui16 *pPixels, int r, int g, int b, int a)
	{
		// color, light, shadow
		// r, g, b, a
		si16 pMaterial[10] =
			{r, 240-r, -60,
			 g, 240-g, -60,
			 b, 240-b, -60,
			 a};

		render( pPixels, pMaterial );
	}

private:
	void render(ui16 *pPixels, si16* mat)
	{
  	int x, y;

    for (y=0; y<h; y++)
      for (x=0; x<w; x++)
    	{
    		int a = (x-cx)*(x-cx) + (y-cy)*(y-cy);

    		a = _sqrt( a *256*256/lc/lc );

    		if (a>255)
    			a = 0;
  			else
  			{
  	  		a = 255-a;
  	  		a *= 5;	
  	  		if (a>255)
    				a = 255;
  			}

    		int c = (x-gx)*(x-gx) + (y-gy)*(y-gy);
    		c = _sqrt( c *256*256/lg/lg );
    		c = c*c/256;
    		if (c>255)
    			c = 255;		

    		int s = (x-sx)*(x-sx) + (y-sy)*(y-sy);
    		s = _sqrt( s *256*256/ls/ls);
    		if (s>255)
    			s = 0;
  			else
  	  		s = 255-s;

        int r = mat[0*3+0] + ((mat[0*3+1]*c)>>8) + ((mat[0*3+2]*s)>>8);
        int g = mat[1*3+0] + ((mat[1*3+1]*c)>>8) + ((mat[1*3+2]*s)>>8);
        int b = mat[2*3+0] + ((mat[2*3+1]*c)>>8) + ((mat[2*3+2]*s)>>8);

		if ( mat[10] != 255 )
			a = (a*mat[9]) >> 8;

  			r = range(r);
  			g = range(g);
  			b = range(b);
/*
  			// flatten
  			if (d0==255)
  			{
//  				r=255; g=0; b=255; d=255;
//					r = 0; g = 0; b = 0;
					pPixels++;
					continue;
  			} else*/
  			{
					ui16 pixOrig = *pPixels;
					int _r = Get565R(pixOrig);
					int _g = Get565G(pixOrig);
					int _b = Get565B(pixOrig);
					int dr = r-_r;
					int dg = g-_g;
					int db = b-_b;

  				_r += (dr * a) >> 8;
  				_g += (dg * a) >> 8;
  				_b += (db * a) >> 8;
					*pPixels++ = RGB565RGB(_r, _g, _b);
  			}

     	}
	}

	int range(int v)
	{
		if ( v < 0 )
			return 0;
		if ( v > 255 )
			return 255;
		return v;
	}

  int _sqrt(int a)
  {
      int ret=0;
      int s;
      int ret_sq=-a-1;
      for(s=30; s>=0; s-=2){
          int b;
          ret+= ret;
          b=ret_sq + ((2*ret+1)<<s);
          if(b<0){
              ret_sq=b;
              ret++;
          }
      }
      return ret;
  }
};

class CWndBalls : public CWnd
{
	enum {
		BallSize = 32
	};

public:
	CBallGenerator	m_Gen;
	ui16 pBuffer[BallSize*BallSize];
	bool bRedraw;

	CWndBalls() : m_Gen(BallSize, BallSize)
	{
		bRedraw = true;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndBalls", dwFlags | CWnd::WsTick | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}
	
	virtual void OnPaint()
	{
		if ( bRedraw )
		{
			CRect rc1( m_rcClient );
			CRect rc2( m_rcClient );
			CRect rc3( m_rcClient );
			rc1.bottom = m_rcClient.Height() / 3;
			rc2.top = rc1.bottom;
			rc2.bottom = m_rcClient.Height() * 2 / 3;
			rc3.top = rc2.bottom;
			BIOS::LCD::Bar( rc1, RGB565(ffffff) );
			BIOS::LCD::Bar( rc2, RGB565(000000) );
			BIOS::LCD::Bar( rc3, RGB565(0000ff) );
			bRedraw = false;
		}
		int nX = Random() % (m_rcClient.Width()-m_Gen.w) + m_rcClient.left;
		int nY = Random() % (m_rcClient.Height()-m_Gen.h) + m_rcClient.top;
		int nR = Random();

		ui32 nColor = RGB32( (nR&3) * 85, ((nR>>2&3)) * 85, ((nR>>4&3)) * 85 );

		CRect rcImage( nX, nY, nX + m_Gen.w, nY + m_Gen.h );

		BIOS::LCD::GetImage( rcImage, pBuffer );	
		m_Gen.Render( pBuffer, nColor );
		BIOS::LCD::PutImage( rcImage, pBuffer );
	}

	void OnTick()	
	{
		Invalidate();
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('t', 'i') )
		{
			// OnTick
			OnTick();
			return;
		}
		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			bRedraw = true;
			/*
			const char* message = "Ahoj, toto je experimentalny zapis do suboru!";
			FILEINFO f;
			BIOS::DSK::Open( &f, "TEST    TXT", BIOS::DSK::IoWrite );

			ui8* pSector = (ui8*)BIOS::DSK::GetSharedBuffer();
			strcpy((char*)pSector, message);

			BIOS::DSK::Write( &f, pSector );
			BIOS::DSK::Close( &f, strlen(message) );*/
			return;
		}
	}

private:
	unsigned int Random()
	{
		// our initial starting seed is 5323
		static unsigned int nSeed = 5323;
		static unsigned int nX = 0;

		// Take the current seed and generate a new value from it
		// Due to our use of large constants and overflow, it would be
		// very hard for someone to predict what the next number is
		// going to be from the previous one.
		nSeed = (8253729 * nSeed + 2396403);
		nSeed += nX++;

		// Take the seed and return a value between 0 and 32767
		return nSeed & 32767;
	}
};
#endif
