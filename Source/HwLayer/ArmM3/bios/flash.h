//#include "flash/flash.h"
//#include "flash/helper.h"
#include "flash/all.h"

// requires 2kb !
LinearFlashing* g_pLinearFlashing = (LinearFlashing*)g_ADCMem;


/*static*/ bool BIOS::MEMORY::PageWrite(int nPage, const ui8* pBuffer)
{
	return flashPageWrite((flashpage_t)nPage, (const flashdata_t*) pBuffer) 
		== FLASH_RETURN_SUCCESS ? true : false;
}

/*static*/ bool BIOS::MEMORY::PageRead(int nPage, ui8* pBuffer)
{
	return flashPageRead((flashpage_t)nPage, (flashdata_t*) pBuffer);
}

/*static*/ bool BIOS::MEMORY::PageErase(int nPage)
{
	return flashPageErase(nPage) == FLASH_RETURN_SUCCESS;
}

/*static*/ void BIOS::MEMORY::LinearStart()
{
	// TODO: for some reason the variable gets corrupted after first flashing (or uninitialized)
	g_pLinearFlashing = (LinearFlashing*)g_ADCMem;
  //BIOS::DBG::Print("FL0 (%08x, %08x)", g_pLinearFlashing, g_ADCMem);
	memset( g_pLinearFlashing, 0, sizeof(LinearFlashing) );
	linearFlashProgramStart(g_pLinearFlashing);
}

/*static*/ bool BIOS::MEMORY::LinearFinish()
{
	bool bOk = linearFlashProgramFinish(g_pLinearFlashing);
	memset( g_pLinearFlashing, 0, sizeof(LinearFlashing) );
	return bOk;
}

/*static*/ bool BIOS::MEMORY::LinearProgram( ui32 nAddress, unsigned char* pData, int nLength )
{
	linearFlashProgram(g_pLinearFlashing, nAddress, (flashdata_t*)pData, nLength);
	return true;
}
