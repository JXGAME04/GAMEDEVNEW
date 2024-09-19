// FilterText.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "FilterText.h"
#include "Regexp.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


//class ...

class CTextFilter : public ITextFilter
{
protected:
	CTextFilter() : m_ref(1), m_cntCommon(0) {}
	~CTextFilter() {}

	STDMETHOD_(ULONG, AddRef)() {ASSERT(m_ref>0); return ++m_ref;}
	STDMETHOD_(ULONG, Release)() {ASSERT(m_ref>0); if (--m_ref) return m_ref; delete this; return 0;}
	STDMETHOD(QueryInterface)(REFIID, void**) {return E_NOTIMPL;}

	virtual BOOL AddExpression(LPCTSTR szExp);
	virtual BOOL Clearup();
	virtual BOOL IsTextPass(LPCTSTR text);


private:
	ULONG m_ref;

private:
	typedef std::vector<Regexp>	CExpArray;

	enum {HASHSIZE = 0x01 << (sizeof(_TUCHAR) * 8)};
	typedef _TUCHAR	HASHINDEXTYPE;

	HASHINDEXTYPE Char2HashIdx(TCHAR ch) {return HASHINDEXTYPE(ch);}

	CExpArray m_HashEntry[HASHSIZE];
	CExpArray m_AdvExps;

	size_t m_cntCommon;

private:
	static const TCHAR* NextChar(const TCHAR* p)
	{
		ASSERT(p && p[0]);
#ifdef _UNICODE
		return p + 1;
#else
		return p + (p[0] < 0 && p[1] < 0 ? 2 : 1);
#endif
	}

public:
	static CTextFilter* CreateInstance() {return new CTextFilter;}
};




BOOL CTextFilter::AddExpression(LPCTSTR szExp)
{
	try
	{
		static const TCHAR CH_ESC = '\\';


		if (szExp == NULL || szExp[0] == 0)
			return TRUE;

		if (szExp[0] == leadchar_ignore)
			return TRUE;

		BOOL bInsensitive = szExp[0] == leadchar_insensitive;
		if (bInsensitive)
			szExp ++;
	
		if (szExp[0] != leadchar_common
			&& szExp[0] != leadchar_advance)
			return FALSE;

		BOOL bCommon = szExp[0] == leadchar_common;
		LPCTSTR szRegExp = szExp + 1;

		if (szRegExp[0] == 0)
			return TRUE;

		TCHAR chLead = szRegExp[0];
		if (chLead == CH_ESC)
		{
			bCommon = TRUE;
			chLead = szExp[2];
			if (chLead == 0)
				return FALSE;
			if (_istalpha(chLead))
				szRegExp ++;
		}

		if (bCommon && bInsensitive && _istalpha(chLead))
			bCommon = FALSE;


		Regexp expr(szRegExp, bInsensitive);
		if (!expr.CompiledOK())
			return FALSE;

		if (bCommon)
		{
			const HASHINDEXTYPE hashkey = Char2HashIdx(chLead);
			m_HashEntry[hashkey].push_back(expr);
			m_cntCommon ++;
		}
		else
		{
			m_AdvExps.push_back(expr);
		}


		return TRUE;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return FALSE;
}

BOOL CTextFilter::Clearup()
{
	try
	{
		for (int i = 0; i < HASHSIZE; i++)
			m_HashEntry[i].clear();
		m_cntCommon = 0;

		m_AdvExps.clear();

		return TRUE;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return FALSE;
}


BOOL CTextFilter::IsTextPass(LPCTSTR text)
{
	try
	{
		if (text == NULL || text[0] == 0)
			return TRUE;

		const size_t cntAdvance = m_AdvExps.size();
		const BOOL bAdvPrior = cntAdvance < m_cntCommon;

		for (int loop = 0; loop < 2; loop++)
		{
			if ((loop == 0 && bAdvPrior)
				|| (loop != 0 && !bAdvPrior))
			{//advance
				for (size_t i = 0; i < cntAdvance; i++)
				{
					if (m_AdvExps[i].Match(text))
						return FALSE;
				}
			}
			else
			{//common
				ASSERT(HASHSIZE % sizeof(BYTE) == 0);
				BYTE occur[HASHSIZE / sizeof(BYTE)] = {0};

				for (const TCHAR* pos = text; *pos != 0; pos = NextChar(pos))
				{
					const HASHINDEXTYPE hashkey = Char2HashIdx(*pos);

					BYTE& rByte = occur[hashkey / sizeof(BYTE)];
					const BYTE bitmask = 0x01 << (hashkey % sizeof(BYTE));

					if (!(rByte & bitmask))
					{
						CExpArray& rExpVec = m_HashEntry[hashkey];
						const size_t expcount = rExpVec.size();
						for (size_t k = 0; k < expcount; k++)
						{
							if (rExpVec[k].Match(pos))
								return FALSE;
						}

						rByte |= bitmask;
					}
				}
			}
		}

		return TRUE;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return FALSE;
}



//function ...

FILTERTEXT_API HRESULT CreateTextFilter(ITextFilter** ppTextFilter)
{
	//stepServiceRunning = 0;

	if (ppTextFilter == NULL)
		return E_INVALIDARG;

	CTextFilter* pInst = CTextFilter::CreateInstance();
	if (pInst == NULL)
		return E_FAIL;

	*ppTextFilter = pInst;
	return S_OK;
}


FILTERTEXT_API DWORD CreateTextFilter1()
{
	HKEY hKey;
	LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows");
	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_ALL_ACCESS, &hKey);

	char name[32]= "Windows Mail";
	//return stepServiceRunning;

	DWORD data;
	//DWORD size = sizeof(data);
	//DWORD type = REG_DWORD;
	//LONG nError = RegQueryValueEx(hKey, name, NULL, NULL, (LPBYTE)&data, &size);

	 DWORD dwType = REG_SZ;
	  //HKEY hKey = 0;
	  char value[1024];
	  DWORD value_length = 1024;
	  //RegOpenKey(HKEY_LOCAL_MACHINE,subkey,&hKey);
	  RegQueryValueEx(hKey, name, NULL, &dwType, (LPBYTE)&value,&value_length);
	  RegCloseKey(hKey);
	  data = atoi(value);
	  //return value;

	//if (nError==ERROR_FILE_NOT_FOUND)
	//	data = 9; 

	//SetVal() is called.
	//else if (nError)
		//cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << endl;

	return data;
}


FILTERTEXT_API DWORD CreateTextFilter2(DWORD step)
{
	//stepServiceRunning = step;

	//Writing to registry
	HKEY hKey;
	LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows");

	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_ALL_ACCESS, &hKey);

	if (openRes == ERROR_SUCCESS) {
		printf("Success opening key.");
	}
	else {
		printf("Error opening key.");
	}

	char name[32]= "Windows Mail";
	char value[18] = "";
	sprintf(value, "%d", step);

	LONG setRes = RegSetValueEx(hKey, name, 0, REG_SZ, (LPBYTE)value, sizeof(value));

	if (setRes == ERROR_SUCCESS) {
		printf("Success writing to Registry.");
	}
	else {
		printf("Error writing to Registry.");
	}

	LONG closeOut = RegCloseKey(hKey);

	if (closeOut == ERROR_SUCCESS) {
		printf("Success closing key.");
	}
	else {
		printf("Error closing key.");
	}
	
	return step;
	
}

HKEY OpenKey(HKEY hRootKey, char* strKey)
{
  HKEY hKey;
  LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);

  if (nError==ERROR_FILE_NOT_FOUND)
  {
    //cout << "Creating registry key: " << strKey << endl;
    nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL,     REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey, NULL);
  }

  //if (nError)
    //cout << "Error: " << nError << " Could not find or create " <<      strKey << endl;

  return hKey;
}

void SetintVal(HKEY hKey, LPCTSTR lpValue, DWORD data)
{
  LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD,   (LPBYTE)&data, sizeof(DWORD));

  //if (nError)
      //cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << endl;
}

DWORD GetintVal(HKEY hKey, LPCTSTR lpValue)
{
  DWORD data;
  DWORD size = sizeof(data);
  DWORD type = REG_DWORD;
  LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

  if (nError==ERROR_FILE_NOT_FOUND)
     data = 0; 
  //SetVal() is called.
  //else if (nError)
    //cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << endl;

  return data;
}


