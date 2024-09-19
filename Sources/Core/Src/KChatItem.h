//---------------------------------------------------------------------------
// Sword3 Engine (c) 2020 by KingJX
//
// File:	KChatItem.h
// Date:	2003.07.28
// Code:	Phong Kieu
// Desc:	KChatItem Class
//---------------------------------------------------------------------------
#include "KSubWorldSet.h"

#ifndef _KCHATITEM_H
#define _KCHATITEM_H

#ifndef _SERVER

struct KOneMsgInfo
{
	int			 nLines;				//这条信息占了多少行
	int			 nCharWidth;			//这条信息占了字符宽
	unsigned int uTextBKColor;			//这条信息字显示时的文字衬底的颜色,0为无
	int			 nMaxSize;				//Msg可以容纳的最大信息长度
	int			 nLen;					//信息长度
	char		 Msg[1];				//信息的内容
};

class KChatItem
{
private:
	DWORD		m_cViewItemIdx;
public:
	KChatItem();			
	void				   Init();
	int						DeleteChatItemInfo();
	int						CheckSubStrInText(char* pstart,char* strItemInfo, int nLen, int sublen)
	{

		for(int z=0; z< nLen; z++)
		{
			if(strItemInfo[z] == pstart[0])
			{
				BOOL checkname = TRUE;
				for(int t=0; t< sublen; t++)
				{
					if(strItemInfo[z+t] != pstart[t])
						checkname = FALSE;
				}
				if(checkname == TRUE)
				{
					return z;
				}
			}
		}
		return -1;
	}
};

extern	KChatItem	g_cChatItem;

#endif


#endif
