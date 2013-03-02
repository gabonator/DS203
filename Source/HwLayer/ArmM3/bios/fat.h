#include "fatfs/all.h"

FIL g_file;
DIR g_directory;
FATFS g_fatfs;

BIOS::FAT::EResult Result(FRESULT r)
{
	switch (r)
	{
    	case FR_OK:	return BIOS::FAT::EOk;
    	case FR_DISK_ERR: return BIOS::FAT::EDiskError;
		case FR_INT_ERR: return BIOS::FAT::EIntError;
		case FR_NO_FILE: return BIOS::FAT::ENoFile;
		case FR_NO_PATH: return BIOS::FAT::ENoPath;
		default:
			return BIOS::FAT::EIntError;
	}
	return BIOS::FAT::EIntError;
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::Init()
{
    FRESULT r = f_mount(0, &g_fatfs);  
	return Result(r);
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::Open(const char* strName, ui8 nIoMode)
{
	if ( nIoMode == BIOS::DSK::IoWrite )
	{
	    FRESULT r = f_open(&g_file, strName, FA_WRITE | FA_CREATE_ALWAYS);
		return Result(r);
	}
	if ( nIoMode == BIOS::DSK::IoRead )
	{
	    FRESULT r = f_open(&g_file, strName, FA_READ | FA_OPEN_EXISTING);
		return Result(r);
	}
    return BIOS::FAT::EIntError;
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::Read(ui8* pSectorData)
{
    ui32 rcount;
    FRESULT r = f_read(&g_file, pSectorData, 512, &rcount);
    
    return Result(r);
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::Write(ui8* pSectorData)
{
    ui32 wcount;
    FRESULT r = f_write(&g_file, pSectorData, 512, &wcount);
    
    if (wcount != 512)
        return EDiskFull; // Disk is full
    
    return Result(r);

}

/*static*/ BIOS::FAT::EResult BIOS::FAT::Close(int nSize /*= -1*/)
{
	FRESULT r;
	if ( nSize != -1 )
    {
	    FRESULT r = f_lseek(&g_file, nSize);
    	if (r != FR_OK)
    	    return Result(r);
		f_truncate(&g_file);
    	if (r != FR_OK)
    	    return Result(r);
	} 
    r = f_close(&g_file);
    return Result(r);
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::OpenDir(char* strPath)
{
	FRESULT r = f_opendir(&g_directory, (char*)strPath);
	return Result(r);
}

/*static*/ BIOS::FAT::EResult BIOS::FAT::FindNext(TFindFile* pFile)
{
	FILINFO* pFileInfo = (FILINFO*)pFile;

	FRESULT r = f_readdir(&g_directory, pFileInfo);
	if ( pFileInfo->fname[0] == 0)
		return BIOS::FAT::ENoFile;

	return Result(r);	
} 

/*static*/ ui32 BIOS::FAT::GetFileSize()
{
	return g_file.fsize;
}