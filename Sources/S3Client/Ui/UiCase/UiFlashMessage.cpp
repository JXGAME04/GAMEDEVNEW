/*********************************************************************************
*文件    ：UiFlashMessage
*创建人  ：Fyt(Fan Zhanpeng)
*创建时间：08-01-2003
*功能描述：游戏中的新消息通知窗口
**********************************************************************************/
#include "KWin32.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "UiFlashMessage.h"
#include "../Elem/Wnds.h"
#include "../Elem/UiImage.h"
#include "../UiBase.h"
#include "../../../Engine/Src/Text.h"
#include <time.h>

#define NEWS_MESSAGE_INI "UiFlashMessage.ini"
#define DEFAULT_MESSAGE  "\\Ui\\DefaultMessage.ini"
//最长无消息空闲时间，空闲时间持续超过此值时，就从ini中取消息来显示(单位：毫秒)
#define	MAX_IDLE_TIME	600000	//10分钟
//同一消息两次显示之间的间隔(单位：毫秒)
#define SHOW_INTERVAL 30000
//两次字符滚动之间的时间(单位：毫秒)
#define SCROLL_INTERVAL 250
//一般消息最多滚动出现的次数
#define	MAX_NORMAL_SHOW_TIMES	3

#define NOT_NEED_INSERT	-1

extern iRepresentShell* g_pRepresentShell;
KUiFlashMessage* KUiFlashMessage::m_pSelf = NULL;
BOOL			KUiFlashMessage::ms_bEnableIdleMsg = FALSE;

/*********************************************************************************
*功能：构造
**********************************************************************************/
KUiFlashMessage::KUiFlashMessage()
{
	m_pHead = NULL;
	m_pHandling = NULL;
	m_nCharasVisibleLimitNum = 0;
	m_nVisionWidth = 0;
	m_nFontSize = 8;
	m_nFontHalfWidth[0] = m_nFontHalfWidth[1] = 4;
	srand(IR_GetCurrentTime());
	SetColor(0x00000000); // fully transparent background
}

void KUiFlashMessage::EnableIdleMsg(BOOL bEnable)
{
	if (!bEnable != !ms_bEnableIdleMsg)
	{
		ms_bEnableIdleMsg = bEnable;
		if (m_pSelf)
		{
			if (ms_bEnableIdleMsg)
			{
				m_pSelf->m_uLastShowTime = IR_GetCurrentTime();
				m_pSelf->m_IniFile.Load(DEFAULT_MESSAGE);
			}
			else
			{
				m_pSelf->m_IniFile.Clear();
			}
		}
	}
}

/*********************************************************************************
*功能：增加一条消息在队列中
**********************************************************************************/
bool KUiFlashMessage::AddMessage(KNewsMessage* pMsg, unsigned int uTime, unsigned int uTimeParam)
{
	KNewsMessageNode* pNew = NULL;

	// Check if the content of pMsg->sMsg is the same as the latest content in the message nodes
	if (pMsg)
	{
		KNewsMessageNode* pCurrent = m_pHead;
		while (pCurrent)
		{
			if (strcmp(pCurrent->sMsg, pMsg->sMsg) == 0)
			{
				// If the content matches, do not add the message
				return false;
			}
			pCurrent = pCurrent->pNext;
		}

		pNew = (KNewsMessageNode*)malloc(sizeof(KNewsMessageNode));
		if (pNew)
		{
			*(KNewsMessage*)pNew = *pMsg;
			pNew->uTime = uTime;
			pNew->uStartTime = uTimeParam;
			pNew->pNext = m_pHead;
			m_pHead = pNew;
		}
	}
	return (pNew != NULL);
}

/*********************************************************************************
*功能：把m_pHandling指针所指的消息从链表中分离出来
**********************************************************************************/
KNewsMessageNode* KUiFlashMessage::SeparateMsg()
{
	KNewsMessageNode* pNode = m_pHandling;
	if (m_pHandling)
	{
		if (m_pHandling == m_pHead)
		{
			m_pHead = m_pHead->pNext;
		}
		else
		{
			pNode = m_pHead;
			while (pNode->pNext != m_pHandling)
			{
				pNode = pNode->pNext;
				//_ASSERT(pNode);
			};
			pNode->pNext = m_pHandling->pNext;
			pNode = m_pHandling;
		}
		m_pHandling = NULL;
		pNode->pNext = NULL;
	}
	return pNode;
}

// 把pNode所指的消息加到链表末端！
void KUiFlashMessage::AddToTail(KNewsMessageNode* pNode)
{
	if (pNode)
	{
		if (m_pHead)
		{
			KNewsMessageNode* pPrev = m_pHead;
			while (pPrev->pNext)
				pPrev = pPrev->pNext;
			pPrev->pNext = pNode;
		}
		else
		{
			m_pHead = pNode;
		}
	}
}

// 清除所有的消息
void KUiFlashMessage::Clear()
{
	m_IniFile.Clear();
	while (m_pHandling = m_pHead)
	{
		m_pHead = m_pHead->pNext;
		free(m_pHandling);
	};
}

/*********************************************************************************
*功能：打开窗口
**********************************************************************************/
KUiFlashMessage* KUiFlashMessage::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiFlashMessage;
		if (m_pSelf)
		{
			m_pSelf->Initialize();
		}
	}
	if (m_pSelf)
	{
		m_pSelf->m_uLastShowTime = IR_GetCurrentTime();
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}


/*********************************************************************************
*功能：如果窗口正被显示，则返回实例指针
**********************************************************************************/
KUiFlashMessage* KUiFlashMessage::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


/*********************************************************************************
*功能：关闭窗口
**********************************************************************************/
void KUiFlashMessage::CloseWindow(BOOL bDestory)
{
	if (m_pSelf)
	{
		if (bDestory == TRUE)
		{
			m_pSelf->Clear();
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

/*********************************************************************************
*功能：初始化
**********************************************************************************/
void KUiFlashMessage::Initialize()
{
	if (ms_bEnableIdleMsg)
		m_IniFile.Load(DEFAULT_MESSAGE);

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	// determine line height and how many slots fit
	m_nLineHeight = m_nFontSize + 2; // font size + small padding (adjust if desired)
	//if (m_Height <= 0) m_nHeight = m_Height; // keep original members mis-typed? (no change)
	// compute usable content height: total window height minus vertical padding (indentV top+bottom)
	int usableHeight = m_Height - m_nIndentV * 2;
	if (usableHeight <= 0) usableHeight = m_nFontSize + 2;
	m_nNumSlots = usableHeight / m_nLineHeight;
	if (m_nNumSlots < 1) m_nNumSlots = 1;

	m_DisplaySlots.clear();
	m_DisplaySlots.resize(m_nNumSlots);
	for (int i = 0; i < m_nNumSlots; ++i)
	{
		m_DisplaySlots[i].bActive = false;
		m_DisplaySlots[i].pSourceNode = NULL;
	}

	Wnd_AddWindow(this, WL_TOPMOST);
}
extern int SCREEN_WIDTH;
/*********************************************************************************
*功能：载入界面方案
**********************************************************************************/
void KUiFlashMessage::LoadScheme(const char* pszScheme)
{
	if (m_pSelf && pszScheme && pszScheme[0])
	{
		char    szBuf[128];
		sprintf(szBuf, "%s\\%s", pszScheme, NEWS_MESSAGE_INI);
		KIniFile	Ini;
		if (Ini.Load(szBuf))
		{
			if (SCREEN_WIDTH == 1024)
			{
				m_pSelf->Init(&Ini, "Main1024");
			}
			else
			{
				m_pSelf->Init(&Ini, "Main");
			}
			Ini.GetInteger("Main", "IndentH", 0, &m_pSelf->m_nIndentH);
			Ini.GetInteger("Main", "IndentV", 0, &m_pSelf->m_nIndentV);
			Ini.GetInteger("Main", "Font", 0, &m_pSelf->m_nFontSize);
			if (m_pSelf->m_nFontSize < 8)
				m_pSelf->m_nFontSize = 8;
			m_pSelf->m_nFontHalfWidth[0] = (m_pSelf->m_nFontSize + 1) / 2;
			m_pSelf->m_nFontHalfWidth[1] = m_pSelf->m_nFontSize / 2;;
			m_pSelf->m_nVisionWidth = m_pSelf->m_Width - m_pSelf->m_nIndentH * 2;
			m_pSelf->m_nCharasVisibleLimitNum =
				(m_pSelf->m_nVisionWidth * 2) / m_pSelf->m_nFontSize;
			Ini.GetString("Main", "TextColor", "0,0,0", szBuf, sizeof(szBuf));
			m_pSelf->m_uTextColor = GetColor(szBuf);
			Ini.GetString("Main", "TextBorderColor", "0,0,0", szBuf, sizeof(szBuf));
			m_pSelf->m_uTextBorderColor = GetColor(szBuf);
			Ini.GetInteger("Main", "MaxIdleTime", MAX_IDLE_TIME, (int*)&m_pSelf->m_uMaxIdleTime);
			Ini.GetInteger("Main", "ShowInterval", SHOW_INTERVAL, (int*)&m_pSelf->m_uShowInterval);
			Ini.GetInteger("Main", "ScrollInterval", SCROLL_INTERVAL, (int*)&m_pSelf->m_uScrollInterval);
			Ini.GetInteger("Main", "DisplayDuration", 5000, (int*)&m_pSelf->m_uDisplayDuration);
		}
	}
}

/*********************************************************************************
*功能：两个字 - 穿透！！！
**********************************************************************************/
int KUiFlashMessage::PtInWindow(int x, int y)
{
	return 0;
}

//新闻消息真的来了
void KUiFlashMessage::MessageArrival(KNewsMessage* pMsg, SYSTEMTIME* pTime)
{
	unsigned int uTime;
	if (m_pSelf && pMsg &&
		pMsg->nMsgLen > 0 && pMsg->nMsgLen <= sizeof(pMsg->sMsg))
	{
		switch (pMsg->nType)
		{
		case NEWSMESSAGE_NORMAL://一般消息
			m_pSelf->AddMessage(pMsg, 0, 0);
			break;
		case NEWSMESSAGE_COUNTING://倒计（秒）数消息
			if (pTime && pTime->wSecond)
				uTime = pTime->wSecond * 1000;
			else
				uTime = 3000;
			m_pSelf->AddMessage(pMsg, uTime, IR_GetCurrentTime());
			break;
		case NEWSMESSAGE_TIMEEND: //定时消息
			if (pTime)
			{
				SYSTEMTIME Local;
				SystemTimeToTzSpecificLocalTime(NULL, pTime, &Local);
				tm		TimeBuf;
				TimeBuf.tm_isdst = 0;
				TimeBuf.tm_year = Local.wYear - 1900;
				TimeBuf.tm_mon = Local.wMonth - 1;
				TimeBuf.tm_mday = Local.wDay;
				TimeBuf.tm_hour = Local.wHour;
				TimeBuf.tm_min = Local.wMinute;
				TimeBuf.tm_sec = Local.wSecond;
				TimeBuf.tm_wday = Local.wDayOfWeek;
				TimeBuf.tm_yday = 0;
				uTime = mktime(&TimeBuf);
				if (uTime == (unsigned int)(time_t)-1)
					uTime = 0;
			}
			else
				uTime = 0;
			if (uTime)
				m_pSelf->AddMessage(pMsg, uTime, 0);
			else
			{
				pMsg->nType = NEWSMESSAGE_NORMAL;
				m_pSelf->AddMessage(pMsg, 0, 0);
				pMsg->nType = NEWSMESSAGE_TIMEEND;
			}
			break;
		}
	}
}

/*********************************************************************************
*功能：显示消息，并在显示完成后对消息进行进一步处理
**********************************************************************************/
void KUiFlashMessage::PaintWindow()
{
	if (!g_pRepresentShell)
		return;


	KOutputTextParam Param;
	Param.Color = m_uTextColor;
	Param.BorderColor = m_uTextBorderColor;
	Param.nSkipLine = 0;
	Param.nNumLine = 1;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
	Param.nVertAlign = 0;
	Param.bPicPackInSingleLine = true;

	for (int i = 0; i < m_nNumSlots; ++i)
	{
		DisplaySlot& slot = m_DisplaySlots[i];
		if (!slot.bActive) continue;

		Param.nX = slot.nTextPosX;
		Param.nY = m_nAbsoluteTop + m_nIndentV + i * m_nLineHeight;

		// compute the render start and length:
		int renderStart = slot.bStationary ? 0 : slot.nCharIndex;
		int renderLen = slot.CurrentMsg.nMsgLen - renderStart;
		if (renderLen <= 0) continue;

		int nLen = 1;
		char szTemp[512];
		strncpy(szTemp, slot.CurrentMsg.sMsg, sizeof(szTemp) - 1);
		nLen = TEncodeText(szTemp, slot.CurrentMsg.nMsgLen);

		g_pRepresentShell->OutputRichText(m_nFontSize, &Param,
			szTemp, nLen, m_nVisionWidth);
	}
}



/*********************************************************************************
*功能：在队列中，寻找是否有符合显示条件的消息，使指针m_pHandling指向找到的消息
**********************************************************************************/
bool KUiFlashMessage::PickAMessage()
{
	m_pHandling = NULL;
	unsigned int	uCurrentTime = IR_GetCurrentTime();

	if (m_pHead == NULL)
	{
		if (ms_bEnableIdleMsg && uCurrentTime > m_uLastShowTime + m_uMaxIdleTime)
		{	//如果过了时间了，还没有新消息到，就随机在ini中选一条消息，插入队列中
			PickFromIni();
			if (m_pHandling = m_pHead)
				m_pHandling->uShowTimes++;
			//ASSERT(m_pHandling->nType == NEWSMESSAGE_NORMAL);
			m_uLastShowTime = uCurrentTime;	//对付从ini中也不能取到消息的情况
		}
	}
	else
	{
		KNewsMessageNode* pNode = m_pHead;
		while (pNode)
		{
			bool bTobeFree = false;
			if (pNode->nType == NEWSMESSAGE_NORMAL)
			{
				m_pHandling = pNode;
				if ((++pNode->uShowTimes) > MAX_NORMAL_SHOW_TIMES)
					bTobeFree = true;
			}
			else if (pNode->nType == NEWSMESSAGE_COUNTING)
			{
				m_pHandling = pNode;
				if (uCurrentTime - pNode->uStartTime >= pNode->uTime)
					bTobeFree = true;
			}
			else if (uCurrentTime > pNode->uLastShowTime + m_uShowInterval)
			{
				m_pHandling = pNode;
				if ((unsigned int)time(NULL) >= pNode->uTime)
					bTobeFree = true;	//限时消息到时了，消灭它
				else
					pNode->uLastShowTime = uCurrentTime;
			}
			else
			{
				pNode = pNode->pNext;
				continue;
			}
			if (bTobeFree == false)
				break;
			pNode = pNode->pNext;
			free(SeparateMsg());
		};
	}
	return (m_pHandling != NULL);
}

/*********************************************************************************
*功能：在Ini文件中随机抽取一条消息插入队列
**********************************************************************************/
void KUiFlashMessage::PickFromIni()
{
	int         i, nValue;
	char        szNum[16];
	KNewsMessage	Msg;

	m_IniFile.GetInteger("Main", "Count", 0, &nValue);
	if (nValue > 0)
	{
		i = rand() % nValue;

		m_IniFile.GetString("Main", itoa(i, szNum, 10), "", Msg.sMsg, sizeof(Msg.sMsg));
		Msg.nMsgLen = TEncodeText(Msg.sMsg, strlen(Msg.sMsg));
		if (Msg.nMsgLen > 0)
		{
			Msg.nType = NEWSMESSAGE_NORMAL;
			MessageArrival(&Msg, NULL);
		}
	}
}

// Convert a queue node into a display slot (copy message, setup slot state)
// Convert a queue node into a display slot (copy message, setup slot state)
void KUiFlashMessage::ConvertMsgToSlot(KNewsMessageNode* pNode, int idx)
{
	// safety
	if (!pNode || idx < 0 || idx >= m_nNumSlots) return;

	DisplaySlot& slot = m_DisplaySlots[idx];

	slot.CurrentMsg.nMsgLen = 0;
	slot.nInsertPlace = NOT_NEED_INSERT;
	slot.pSourceNode = pNode;

	if (pNode->nType == NEWSMESSAGE_COUNTING)
	{
		slot.nInsertPlace = TFindSpecialCtrlInEncodedText(pNode->sMsg,
			pNode->nMsgLen, 0, '%');

		if (slot.nInsertPlace < 0 ||
			slot.nInsertPlace > pNode->nMsgLen - 2 ||
			pNode->sMsg[slot.nInsertPlace + 1] != 'd')
		{
			slot.nInsertPlace = NOT_NEED_INSERT;
		}
	}

	if (slot.nInsertPlace == NOT_NEED_INSERT)
	{
		memcpy(slot.CurrentMsg.sMsg, pNode->sMsg, pNode->nMsgLen);
		slot.CurrentMsg.nMsgLen = pNode->nMsgLen;
	}
	// else, will be filled per-frame by MakeCountingMsgForSlot()

	// --- IMPORTANT CHANGE: make slot display stationary immediately (no right->left scroll) ---
	slot.bActive = true;
	slot.bJustIncoming = false; // no entering animation
	slot.bStationary = true;    // mark as stationary so PaintWindow renders full text
	slot.uDisplayStartTime = IR_GetCurrentTime(); // start display timer now
	slot.nCharIndex = 0;
	slot.nHalfIndex = 0;
	slot.nSkipTimes = 0;
	slot.nTextPosX = m_nAbsoluteLeft + m_nIndentH; // align to left indent
	slot.nLineLen = slot.CurrentMsg.nMsgLen;
	slot.uLastScrollTime = IR_GetCurrentTime(); // keep a sensible value
}


// Make counting message for a particular slot (returns false if expired)
bool KUiFlashMessage::MakeCountingMsgForSlot(int idx)
{
	if (idx < 0 || idx >= m_nNumSlots) return false;
	DisplaySlot& slot = m_DisplaySlots[idx];
	if (!slot.bActive || !slot.pSourceNode) return false;

	KNewsMessageNode* pNode = slot.pSourceNode;
	unsigned int uCurrent = IR_GetCurrentTime();
	if (uCurrent - pNode->uStartTime >= pNode->uTime)
		return false;

	char sBuffer[sizeof(pNode->sMsg) + 8];
	memcpy(sBuffer, pNode->sMsg, slot.nInsertPlace);
	int outLen = slot.nInsertPlace;
	int nNumLen = sprintf(sBuffer + outLen, "%d",
		(pNode->uTime - uCurrent + pNode->uStartTime + 999) / 1000);
	outLen += nNumLen;
	int insertRest = slot.nInsertPlace + 2;
	if (pNode->nMsgLen > insertRest)
	{
		memcpy(sBuffer + outLen, pNode->sMsg + insertRest,
			pNode->nMsgLen - insertRest);
		outLen += pNode->nMsgLen - insertRest;
	}

	// pad to keep legacy behavior
	nNumLen = 5 - nNumLen;
	if (nNumLen > 0)
	{
		memset(sBuffer + outLen, 0, nNumLen);
		outLen += nNumLen;
	}

	if (outLen > (int)sizeof(slot.CurrentMsg.sMsg))
	{
		outLen = TSplitEncodedString(sBuffer, outLen,
			sizeof(slot.CurrentMsg.sMsg), true);
	}
	memcpy(slot.CurrentMsg.sMsg, sBuffer, outLen);
	slot.CurrentMsg.nMsgLen = outLen;
	return true;
}

// Reset a single slot (called when a new msg is assigned or when Pick puts node there)
void KUiFlashMessage::ResetSlot(int idx)
{
	if (idx < 0 || idx >= m_nNumSlots) return;
	DisplaySlot& slot = m_DisplaySlots[idx];
	if (!slot.bActive || !slot.pSourceNode) { slot.bActive = false; return; }

	slot.uLastScrollTime = IR_GetCurrentTime();
	slot.nSkipTimes = 0;

	// No entering scroll: display immediately as stationary
	slot.bJustIncoming = false;
	slot.bStationary = true;
	slot.uDisplayStartTime = IR_GetCurrentTime();

	slot.nCharIndex = 0;
	slot.nHalfIndex = 0;
	slot.nTextPosX = m_nAbsoluteLeft + m_nIndentH;

	if (slot.nInsertPlace != NOT_NEED_INSERT)
	{
		// prepare the counting text content immediately
		MakeCountingMsgForSlot(idx);
	}
}



// Scroll logic for a single slot.
// returns true when this slot has finished displaying and should be recycled.
bool KUiFlashMessage::ScrollSlot(int idx)
{
	// With stationary display we don't perform horizontal moving.
	// Keep this function to support counting messages refresh and to indicate
	// when a counting message has expired (so Breathe can recycle the slot).
	if (idx < 0 || idx >= m_nNumSlots) return true;
	DisplaySlot& slot = m_DisplaySlots[idx];
	if (!slot.bActive || !slot.pSourceNode) return true;

	// If counting message, refresh display text; if expired -> finished.
	if (slot.nInsertPlace != NOT_NEED_INSERT)
	{
		if (MakeCountingMsgForSlot(idx) == false)
			return true; // counting expired -> finished
	}

	// Since we're not scrolling horizontally, we simply keep the slot displayed.
	// Breathe() will handle expiration based on uDisplayStartTime and m_uDisplayDuration.
	return false; // not finished yet
}


// Try to assign messages from the queue to free slots (old messages first)
// Return true if assigned any message.
bool KUiFlashMessage::PickMessagesForSlots()
{
	unsigned int uCurrentTime = IR_GetCurrentTime();
	bool anyAssigned = false;

	// if queue empty and idle behaviour
	if (m_pHead == NULL)
	{
		if (ms_bEnableIdleMsg && uCurrentTime > m_uLastShowTime + m_uMaxIdleTime)
		{
			PickFromIni();
			m_uLastShowTime = uCurrentTime;
		}
		return false;
	}

	// For each slot, if it's free, try to find an eligible message from queue (oldest first)
	for (int s = 0; s < m_nNumSlots; ++s)
	{
		if (m_DisplaySlots[s].bActive)
			continue;

		KNewsMessageNode* pNode = m_pHead;
		KNewsMessageNode* prev = NULL;

		while (pNode)
		{
			bool bEligible = false;
			bool bShouldBeFreed = false;

			// Decide eligibility, but DO NOT mutate pNode->uShowTimes yet.
			if (pNode->nType == NEWSMESSAGE_NORMAL)
			{
				// normal messages are eligible
				bEligible = true;
				// check if they would exceed max shows if shown now
				if ((unsigned int)(pNode->uShowTimes + 1) > (unsigned int)MAX_NORMAL_SHOW_TIMES)
					bShouldBeFreed = true;
			}
			else if (pNode->nType == NEWSMESSAGE_COUNTING)
			{
				// counting messages eligible only if not expired
				if (uCurrentTime - pNode->uStartTime < pNode->uTime)
					bEligible = true;
				else
					bShouldBeFreed = true; // expired, free it
			}
			else // time-end (NEWSMESSAGE_TIMEEND)
			{
				if (uCurrentTime > pNode->uLastShowTime + m_uShowInterval)
				{
					// only eligible when interval passed
					if ((unsigned int)time(NULL) >= pNode->uTime)
						bShouldBeFreed = true; // time passed -> free
					else
					{
						bEligible = true;
						// update last-show time so we don't pick it again too soon
						pNode->uLastShowTime = uCurrentTime;
					}
				}
			}

			if (bShouldBeFreed)
			{
				// Remove pNode from list and free it, then continue scanning from next
				KNewsMessageNode* next = pNode->pNext;
				if (prev)
					prev->pNext = next;
				else
					m_pHead = next;
				free(pNode);
				pNode = next;
				continue;
			}

			if (!bEligible)
			{
				// move on
				prev = pNode;
				pNode = pNode->pNext;
				continue;
			}

			// At this point pNode is eligible and should be assigned to slot 's'.
			// Now mutate uShowTimes (for normal).
			if (pNode->nType == NEWSMESSAGE_NORMAL)
				pNode->uShowTimes++;

			// Detach chosen (pNode) from the list using 'prev'
			KNewsMessageNode* chosen = pNode;
			KNewsMessageNode* next = pNode->pNext;
			if (prev)
				prev->pNext = next;
			else
				m_pHead = next;
			chosen->pNext = NULL;

			// Assign to slot s
			ConvertMsgToSlot(chosen, s);
			ResetSlot(s);
			anyAssigned = true;

			// Done with this slot, break to outer loop (we will start searching from head for next slot)
			break;
		} // end while pNode

	} // end for slots

	return anyAssigned;
}

/*********************************************************************************
*功能：保持活动，更新消息信息
**********************************************************************************/
void KUiFlashMessage::Breathe()
{
	unsigned int now = IR_GetCurrentTime();

	for (int i = 0; i < m_nNumSlots; ++i)
	{
		DisplaySlot& slot = m_DisplaySlots[i];
		if (!slot.bActive) continue;

		if (slot.bStationary)
		{
			// Counting messages still update (MakeCountingMsgForSlot called in Paint/Scroll).
			// Finish when display duration elapsed OR counting expired.
			if (slot.nInsertPlace != NOT_NEED_INSERT)
			{
				// counting messages were updated in ScrollSlot; if expired, it would return true there.
				// but double-check here: if counting expired, free now.
				KNewsMessageNode* pNode = slot.pSourceNode;
				if (pNode)
				{
					unsigned int uCurrent = IR_GetCurrentTime();
					if (uCurrent - pNode->uStartTime >= pNode->uTime)
					{
						// finished
						if (slot.pSourceNode) AddToTail(slot.pSourceNode);
						slot.bActive = false;
						slot.pSourceNode = NULL;
						continue;
					}
				}
			}
			// check stationary timeout
			if (now >= slot.uDisplayStartTime + m_uDisplayDuration)
			{
				if (slot.pSourceNode) AddToTail(slot.pSourceNode);
				slot.bActive = false;
				slot.pSourceNode = NULL;
				continue;
			}
			// else keep showing
			continue;
		}

		// non-stationary slot: scroll frames based on per-slot timer
		if (IR_IsTimePassed(m_uScrollInterval, slot.uLastScrollTime))
		{
			bool finished = ScrollSlot(i);
			slot.uLastScrollTime = now;
			if (finished)
			{
				if (slot.pSourceNode)
				{
					AddToTail(slot.pSourceNode);
				}
				slot.bActive = false;
				slot.pSourceNode = NULL;
			}
		}
	}

	// fill free slots if possible
	PickMessagesForSlots();
}

void KUiFlashMessage::ConvertMsg()
{
	//_ASSERT(m_pHandling);
	//填充当前显示消息的内容
	m_CurrentMsg.nMsgLen = 0;

	m_nInsertPlace = NOT_NEED_INSERT;
	if (m_pHandling->nType == NEWSMESSAGE_COUNTING)
	{
		m_nInsertPlace = TFindSpecialCtrlInEncodedText(m_pHandling->sMsg,
			m_pHandling->nMsgLen, 0, '%');

		if (m_nInsertPlace < 0 &&
			m_nInsertPlace > m_pHandling->nMsgLen - 2)
		{
			m_nInsertPlace = NOT_NEED_INSERT;
		}
		else if (m_pHandling->sMsg[m_nInsertPlace + 1] != 'd')
		{
			m_nInsertPlace = NOT_NEED_INSERT;
		}
	}

	if (m_nInsertPlace == NOT_NEED_INSERT)
	{
		memcpy(m_CurrentMsg.sMsg, m_pHandling->sMsg, m_pHandling->nMsgLen);
		m_CurrentMsg.nMsgLen = m_pHandling->nMsgLen;
	}
}

bool KUiFlashMessage::MakeCountingMsg()
{
	//_ASSERT(m_pHandling &&
	//	m_pHandling->nType == NEWSMESSAGE_COUNTING &&
	//	m_nInsertPlace != NOT_NEED_INSERT);
	unsigned int uCurrent = IR_GetCurrentTime();
	if (uCurrent - m_pHandling->uStartTime >= m_pHandling->uTime)
		return false;

	char	sBuffer[sizeof(m_pHandling->sMsg) + 8];
	memcpy(sBuffer, m_pHandling->sMsg, m_nInsertPlace);
	m_CurrentMsg.nMsgLen = m_nInsertPlace;
	int nNumLen = sprintf(sBuffer + m_nInsertPlace, "%d",
		(m_pHandling->uTime - uCurrent + m_pHandling->uStartTime + 999) / 1000);
	m_CurrentMsg.nMsgLen += nNumLen;
	m_nInsertPlace += 2;
	if (m_pHandling->nMsgLen > m_nInsertPlace)
	{
		memcpy(sBuffer + m_CurrentMsg.nMsgLen,
			m_pHandling->sMsg + m_nInsertPlace,
			m_pHandling->nMsgLen - m_nInsertPlace);
		m_CurrentMsg.nMsgLen += m_pHandling->nMsgLen - m_nInsertPlace;
	}
	m_nInsertPlace -= 2;

	//此段特殊的额外处理是为了...
	{
		//_ASSERT(nNumLen > 5);
		nNumLen = 5 - nNumLen;
		if (nNumLen > 0)
		{
			memset(sBuffer + m_CurrentMsg.nMsgLen, 0, nNumLen);
			m_CurrentMsg.nMsgLen += nNumLen;
		}
	}

	if (m_CurrentMsg.nMsgLen > sizeof(m_CurrentMsg.sMsg))
	{
		m_CurrentMsg.nMsgLen = TSplitEncodedString(sBuffer, m_CurrentMsg.nMsgLen,
			sizeof(m_CurrentMsg.sMsg), true);
	}
	memcpy(m_CurrentMsg.sMsg, sBuffer, m_CurrentMsg.nMsgLen);
	return true;
}

/*********************************************************************************
*功能：重置当前所显示消息的显示状态，重置计时器
**********************************************************************************/
void KUiFlashMessage::Reset()
{
	if (m_pHandling == NULL)
		return;

	m_uLastShowTime = m_uLastScrollTime = IR_GetCurrentTime();

	ConvertMsg();
	m_bJustIncoming = true;
	m_nCharIndex = 0;
	m_nHalfIndex = 0;
	m_nTextPosX = m_nAbsoluteLeft + m_nIndentH + m_nVisionWidth;
}

bool KUiFlashMessage::ScrollMsg()
{
	static int snSkipTimes = 0;
	if (snSkipTimes > 0)
	{
		m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
		m_nHalfIndex ^= 1;
		snSkipTimes--;
		if (snSkipTimes == 0 && m_bJustIncoming == false)
		{
			m_nTextPosX = m_nAbsoluteLeft + m_nIndentH;
		}
		return false;
	}
	//_ASSERT(m_pHandling);
	if (m_nInsertPlace != NOT_NEED_INSERT)
	{
		if (MakeCountingMsg() == false)
			return true;
	}
	int	nPos, nLen;
	if (m_bJustIncoming)
	{
		nLen = 1;
		if (m_nCharIndex < m_CurrentMsg.nMsgLen)
		{
			nPos = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg + m_nCharIndex,
				m_CurrentMsg.nMsgLen - m_nCharIndex, nLen, false, m_nFontSize);
			m_nCharIndex += nPos;
		}
		if (nLen)
		{
			m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
			m_nHalfIndex ^= 1;
			snSkipTimes = nLen - 1;
		}
		if (m_nTextPosX < m_nAbsoluteLeft + m_nIndentH)
		{
			m_nTextPosX = m_nAbsoluteLeft + m_nIndentH;
			m_bJustIncoming = false;
			nLen = 1;
			m_nCharIndex = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg,
				m_CurrentMsg.nMsgLen, nLen, false, m_nFontSize);
			if (m_nCharIndex >= m_CurrentMsg.nMsgLen)
			{
				snSkipTimes = 0;
				return true;
			}
		}
	}
	else
	{
		nLen = 1;
		nPos = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg + m_nCharIndex,
			m_CurrentMsg.nMsgLen - m_nCharIndex, nLen, false, m_nFontSize);
		m_nCharIndex += nPos;
		if (nLen > 1)
		{
			m_nTextPosX += nLen / 2 * m_nFontSize;
			if ((nLen % 2) == 0)
			{
				m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
				m_nHalfIndex ^= 1;
			}
			snSkipTimes = nLen - 1;
		}
		if (m_nCharIndex >= m_CurrentMsg.nMsgLen)
		{
			snSkipTimes = 0;
			return true;
		}
	}
	return false;
}