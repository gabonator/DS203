#ifndef __ITEMSHORTCUT_H__
#define __ITEMSHORTCUT_H__

class CItemShortcut : public CItemProvider
{
public:
	void Create( CWnd* pParent, CValueProvider*	pProvider, const char* pszId)
	{
		CItemProvider::Create( pProvider, pszId, RGB565(808080), pParent );
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft || BIOS::KEY::KeyRight)
		{
			Settings.Save();
		}
		CItemProvider::OnKey( nKey );
	}
};

#endif