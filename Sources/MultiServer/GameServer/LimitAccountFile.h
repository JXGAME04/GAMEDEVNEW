// LimitAccountFile.h: interface for the LimitAccountFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIMITACCOUNTFILE_H__D0F5F8AB_175E_4D21_A8F0_B4FACD2709DA__INCLUDED_)
#define AFX_LIMITACCOUNTFILE_H__D0F5F8AB_175E_4D21_A8F0_B4FACD2709DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CLimitAccountFile  
{
public:
	CLimitAccountFile();
	virtual ~CLimitAccountFile();

public:
	BOOL Initialize(const std::string& root, size_t threshold);
	BOOL Uninitialize();

	ULONG BeginBatch();
	ULONG EndBatch();
	BOOL IsBatched();
	void	KPIGetExePath( LPSTR lpExePath, DWORD dwSize );
	int		GetCountHWID_Login(char* sHWID);
	int		GetCountHWID_Logout(char* sHWID);
	int		Replace_Line(const char *oldline, const char* newline);

	size_t TraceLog(const void* pData, size_t size);

private:
	enum {
		CH_SEPARATOR	= '\\',
		CH_EXTSPLIT		= '~',
	};

	std::string m_strRoot;
	std::string c_strfile;
	size_t m_threshold;

	HANDLE m_hFile;
	ULONG m_lBatch;

private:
	BOOL PrepareFile();
	static BOOL OpenUniDirectory(const std::string& dirname, std::string* pNewdir);
	static HANDLE CreateUniFile(const std::string& filename, std::string* pNewfile);
};

#endif // !defined(AFX_LIMITACCOUNTFILE_H__D0F5F8AB_175E_4D21_A8F0_B4FACD2709DA__INCLUDED_)
