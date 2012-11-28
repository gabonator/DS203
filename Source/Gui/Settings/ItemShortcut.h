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
		/*if ( ( nKey & BIOS::KEY::KeyLeft ) || ( nKey & BIOS::KEY::KeyRight ) )
        {
            CItemProvider::OnKey( nKey ); // this will update the variable
            Settings.Save(); // save new value, again this approach is not good idea
            return;
        }*/
		CItemProvider::OnKey( nKey );
	}
};

#endif