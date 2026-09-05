/*****************************************************************************************
//	界面--新闻消息窗口
//	Copyright : Kingsoft 2003
//	Author	:   Fyt(Fan Zhanpeng)
//	CreateTime:	2003-08-01
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _UIFLASHMESSAGE_H
#define _UIFLASHMESSAGE_H    1

#include "KEngine.h"
#include "KIniFile.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShadow.h"
#include "../../../Core/Src/GameDataDef.h"
#include "UiNewsMessage.h"
#include <vector>

class KUiFlashMessage : protected KWndShadow
{
public:
	//----界面面板统一的接口函数----
	static KUiFlashMessage* OpenWindow();						// 打开窗口，返回唯一的一个类对象实例
	static KUiFlashMessage* GetIfVisible();                     //如果窗口正被显示，则返回实例指针
	static void				CloseWindow(BOOL bDestory = FALSE);	// 关闭窗口
	static void				LoadScheme(const char* pszScheme);	// 载入界面方案
	static void				MessageArrival(KNewsMessage* pMsg, SYSTEMTIME* pTime);
	static void				EnableIdleMsg(BOOL bEnable);

private:
	static KUiFlashMessage* m_pSelf;
	static BOOL				ms_bEnableIdleMsg;
private:
	KUiFlashMessage();
	~KUiFlashMessage() {}
	void	Initialize();
	void	LoadScheme(KIniFile* pIni);			// 载入界面方案
	virtual void	Breathe();					// 我要活动，处理消息队列-____-|||b阿
	virtual int		PtInWindow(int x, int y);	// 穿透！！！
	virtual void	PaintWindow();

	bool	AddMessage(KNewsMessage* pMsg, unsigned int uTime, unsigned int uTimeParam = 0);
	// 在消息队列-__-||b的最前面增加一条消息-__-||b
	KNewsMessageNode* SeparateMsg();			// 把m_pHandling所指的消息从链表分离出来！！！！！
	void	AddToTail(KNewsMessageNode* pNode);	// 把pNode所指的消息加到链表末端！
	bool	PickAMessage();						// 在队列中，寻找是否有符合显示条件的消息，并且负责删除过时消息
	void	Reset();							// 重置消息的显示状态
	bool	ScrollMsg();						// 滚动当前消息
	void	PickFromIni();						// 在Ini文件中随机抽取一条消息插入队列
	void	Clear();							// 清除所有的消息
	void	ConvertMsg();						// 转换消息到最终显示内容
	bool	MakeCountingMsg();					// 生成倒计时消息的显示内容

private:

	KNewsMessageNode* m_pHead;			// 消息链表中的头一个消息
	KNewsMessageNode* m_pHandling;		// 正在显示/处理的新闻消息

	KIniFile		m_IniFile;				// 纪录Ini文件中要抽出来显示的一些消息

	int				m_nIndentH;				// 前缀和显示消息部份分隔多少
	int				m_nIndentV;				// 文字相对于窗口顶部的向下缩近（单位：像素点）
	int				m_nFontSize;			// 字体大小
	unsigned int	m_uTextColor;			// 前景字符的颜色
	unsigned int	m_uTextBorderColor;		// 文字边缘颜色
	int				m_nVisionWidth;			// 显示消息的空间的像素宽度
	int				m_nCharasVisibleLimitNum;// 最多可的字符的限制
	int				m_nFontHalfWidth[2];	// 字符的两个一半宽度

	unsigned int	m_uMaxIdleTime;			// 最长无消息空闲时间
	unsigned int	m_uShowInterval;		// 同一个消息两次显示之间的间隔
	unsigned int	m_uScrollInterval;		// 字符滚动的时间间隔

	unsigned int	m_uLastShowTime;		// 上一次显示消息的时候
	KNewsMessage	m_CurrentMsg;			// 当前显示的消息内容
	int				m_nLineLen;				// 当前显示消息的总显示字符数长度

	unsigned int	m_uLastScrollTime;		// 上次滚动的时间
	bool	m_bJustIncoming;				// 消息正在进入,(头)还未开始离开显示区域
	int		m_nCharIndex;					// 消息字符的滚动下标
	int		m_nTextPosX;					// 消息显示区域的左上角X坐标(相对)
	int		m_nHalfIndex;					// 字符半宽的索引
	int		m_nInsertPlace;					// 只对倒计数消息有效
	
	// --- New: multiple display slots ---
	struct DisplaySlot
	{
		bool            bActive;            // is this slot showing a message?
		KNewsMessage    CurrentMsg;         // currently displayed message content
		KNewsMessageNode* pSourceNode;      // pointer to the node we took from queue
		unsigned int    uLastScrollTime;    // slot-specific last scroll time
		bool            bJustIncoming;      // true while text still entering from right
		bool            bStationary;        // true while head has hit left and message is paused
		unsigned int    uDisplayStartTime;  // when stationary display started
		int             nCharIndex;
		int             nTextPosX;
		int             nHalfIndex;
		int             nInsertPlace;      // for counting messages
		int             nSkipTimes;        // slot-local skip counter (was static before)
		int             nLineLen;          // total char length for this slot
		// [TKCHAT 05/09] chu da ma hoa + do rong px, tinh MOT lan luc sinh (khong TEncodeText moi khung)
		char            szEnc[512];
		int             nEncLen;
		int             nRongPx;
	};

	std::vector<DisplaySlot> m_DisplaySlots; // dynamic number of slots (computed at Initialize)
	int             m_nNumSlots;             // how many vertical slots we can display
	int             m_nLineHeight;           // computed line height (font + spacing)
	unsigned int    m_uDisplayDuration;   // NEW: how long to show after head hits left (ms)
	// [TKCHAT 04/09c] dong TROI LEN nhu 2.0 (chu: "dong thong bao luc danh chet chay giua man hinh len roi bien mat")
	int             m_nRiseFrom;          // Y tuyet doi noi dong sinh ra (ini RiseFrom; -1 = day cua so)
	int             m_nRiseSpeed;         // toc do troi len, px/giay (ini RiseSpeed)
	int             m_nFadeMs;            // toi dan trong FadeMs cuoi (ini FadeMs; 0 = khong)
	unsigned int    m_uMaxQueueDelay;     // hang doi don qua muc nay (ms) thi bo tin moi (ini MaxQueueDelay)
	unsigned int    m_uNextStartTime;     // thoi diem som nhat dong ke tiep duoc sinh (cach dong truoc dung 1 hang)
	int             m_nMaxLines;          // [TKCHAT 05/09] toi da so dong cung hien (ini MaxLines, mac dinh 3)
	int             m_bEnable;            // [TKCHAT 05/09b] ini Enable (0 = tat hoan toan; chu thu xem con lag khong)
	int     RiseFromY() { return (m_nRiseFrom >= 0) ? m_nRiseFrom : (m_nAbsoluteTop + m_Height - m_nIndentV - m_nLineHeight); }

	// --------------------------------------

	// old m_CurrentMsg removed; use slot.CurrentMsg
	// old state members dedicated to single message removed:
	// m_nLineLen, m_uLastScrollTime, m_bJustIncoming, m_nCharIndex, m_nTextPosX, m_nHalfIndex, m_nInsertPlace

	// helper functions (new)
	void    ResetSlot(int idx);                      // reset state for slot idx
	bool    ScrollSlot(int idx);                     // scroll one slot; returns true when finished
	void    ConvertMsgToSlot(KNewsMessageNode* pNode, int idx); // fill slot.CurrentMsg from pNode
	bool    MakeCountingMsgForSlot(int idx);         // produce counting message for slot idx

	// modified: PickAMessage -> will fill free slots
	bool    PickMessagesForSlots();                  // pick messages for any free slots
	// called when a new message arrives: attempt to preempt the oldest active slot
	bool    PreemptOldestSlot(KNewsMessageNode* pNew);

};

#endif
