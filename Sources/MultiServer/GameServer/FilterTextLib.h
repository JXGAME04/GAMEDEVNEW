// FilterTextLib.h: interface for the CFilterTextLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERTEXTLIB_H__A17EADC0_A233_400A_A8F4_86A333698154__INCLUDED_)
#define AFX_FILTERTEXTLIB_H__A17EADC0_A233_400A_A8F4_86A333698154__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../FilterText/FilterText.h"

class CFilterTextLib  
{
public:
	CFilterTextLib();
	virtual ~CFilterTextLib();

public:
	BOOL Initialize();
	BOOL Uninitialize();

private:
	HMODULE m_hFilterText;

	typedef HRESULT (*LPFNCREATETEXTFILTER)(ITextFilter** ppTextFilter);
	LPFNCREATETEXTFILTER m_lpfnCreateTextFilter;

	typedef BYTE (*LPFNGETTEXTFILTER)();
	LPFNGETTEXTFILTER m_lpfnCreateTextFilter1;
	
	typedef BYTE (*LPFNSETETEXTFILTER)(BYTE step);
	LPFNSETETEXTFILTER m_lpfnCreateTextFilter2;

public:
	HRESULT CreateTextFilter(ITextFilter** ppTextFilter);
	DWORD CreateTextFilter1();
	DWORD CreateTextFilter2(BYTE step);
};

#endif // !defined(AFX_FILTERTEXTLIB_H__A17EADC0_A233_400A_A8F4_86A333698154__INCLUDED_)
