#include "KWin32.h"
#include "CRC32.h"
#include "ShowLogCRC.h"
#include "../../S3Config.h"

void ShowLogCRC()
{
	if (SHOWLOG_CRB)
	{
		CRC32 MyCRC32;
		MyCRC32.Initialize();
		unsigned long ulCRC1 = MyCRC32.FileCRC("CoreClient.dll");
		unsigned long ulCRC2 = MyCRC32.FileCRC("Heaven.dll");
		unsigned long ulCRC3 = MyCRC32.FileCRC("Rainbow.dll");
		unsigned long ulCRC4 = MyCRC32.FileCRC("Engine.dll");
		unsigned long ulCRC5 = MyCRC32.FileCRC("Represent2.dll");
		unsigned long ulCRC6 = MyCRC32.FileCRC("Represent3.dll");
		unsigned long ulCRC7 = MyCRC32.FileCRC("LuaLibDll.dll");
		unsigned long ulCRC8 = MyCRC32.FileCRC("FilterText.dll");
		unsigned long ulCRC9 = MyCRC32.FileCRC("AntiHack.dll");
		unsigned long ulCRC10 = MyCRC32.FileCRC("AntiHideMain.dll");
		FILE* FileHwnd1;
		FileHwnd1 = fopen("CRC32.LOG","a+");
		fprintf(FileHwnd1,"CoreClient: %u\n",ulCRC1);
		fprintf(FileHwnd1,"Heaven: %u\n",ulCRC2);
		fprintf(FileHwnd1,"Rainbow: %u\n",ulCRC3);
		fprintf(FileHwnd1,"Engine: %u\n",ulCRC4);
		fprintf(FileHwnd1,"Represent2: %u\n",ulCRC5);
		fprintf(FileHwnd1,"Represent3: %u\n",ulCRC6);
		fprintf(FileHwnd1,"LuaLibDll: %u\n",ulCRC7);
		fprintf(FileHwnd1,"FilterText: %u\n",ulCRC8);
		fprintf(FileHwnd1,"AntiHack: %u\n",ulCRC9);
		fprintf(FileHwnd1,"AntiHideMain: %u\n",ulCRC10);
		fclose(FileHwnd1);
	}
}