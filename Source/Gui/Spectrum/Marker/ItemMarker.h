#ifndef __MENUITEMSPECMARKER_H__
#define __MENUITEMSPECMARKER_H__

class CItemSpecMarker : public CWndMenuItem
{

public:
	virtual void Create(CWnd *pParent) 
	{
		CWndMenuItem::Create( NULL, RGB565(404040), 2, pParent);
	}

	virtual void OnPaint()
	{
		bool bEnabled = Settings.Spec.MarkerSource != CSettings::Spectrum::_Off;
		ui16 clr = bEnabled ? RGB565(000000) : RGB565(808080);
	
		CWndMenuItem::OnPaint();
		
		int x = m_rcClient.left + 10 + MarginLeft;
		int y = m_rcClient.top;

		char* strFreq = CUtils::FormatFrequency(Settings.Spec.fMarkerX);
		BIOS::LCD::Print( x, y, clr, RGBTRANS, strFreq );
		y += 16;

		char* strAmpl = CUtils::FormatVoltage(Settings.Spec.fMarkerY);
		BIOS::LCD::Print( x, y, clr, RGBTRANS, strAmpl );
	}
};

#endif