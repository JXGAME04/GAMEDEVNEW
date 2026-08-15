/*****************************************************************************************
//	Copyright : VLUS 2025
//	Author	:   X
//	CreateTime:	2025-10-8
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/wndpage.h"
#include "../Elem/WndImagePart.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../elem/WndObjContainer.h"
#include "../../../core/src/gamedatadef.h"

#define LIVE_SKILL_COUNT			10
#define	FIGHT_SKILL_COUNT			50
#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define FIGHT_SKILL_UI_MAX			50
#define	FIGHT_SKILL_SUB_PAGE_COUNT	1

class KUiFightSkillSubPageNew : public KWndPage
{
public:
	KUiFightSkillSubPageNew();
	void	Initialize(/*int nSubPageIndex*/);				//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新单个技能
	void	UpdateData(KUiSkillData* pSkills);			//更新数据
	void	UpdateFaction(int faction);
private:
	int		   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	 PaintWindow();								//绘制窗口
	void	 OnSkillPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
private:
//	int			m_nSubPagIndex;
	int			m_nRemainSkillPoint;
	KWndObjectBox	m_FightSkills[FIGHT_SKILL_UI_MAX];
	KWndButton	m_btnFightSkills[FIGHT_SKILL_UI_MAX];
//	KUiSkillData	m_Skills[FIGHT_SKILL_COUNT_PER_PAGE];
	struct
	{
		int		nFont;
		SIZE	Offset;
		unsigned int Color;
	}			m_SkillTextParam;
	int m_nFaction;
};

class KUiFightSkillSubNew : public KWndPageSet
{
public:	
	void	Initialize();								//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateData();								//更新数据
	void	UpdateFaction(int faction);
private:
	KUiFightSkillSubPageNew	m_SubPages[FIGHT_SKILL_SUB_PAGE_COUNT];
	KWndButton				m_SubPageBtn[FIGHT_SKILL_SUB_PAGE_COUNT];
};

class KUiFightSkillNew : public KWndPage
{
public:
	void	Initialize();								//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateData();								//更新数据	
	void	UpdateFaction(int faction);
	int		m_nFaction;
private:
	KWndText32			m_RemainSkillPoint;
	KUiFightSkillSubNew	m_InternalPad;
};

class KUiLiveSkillNew : public KWndPage
{
public:
	KUiLiveSkillNew();
	void	Initialize();								//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateBaseData();							//更新技能公共数据
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateData();								//更新数据
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	PaintWindow();								//绘制窗口
private:
	KWndText32		m_RemainSkillPoint;
	int				m_nRemainSkillPoint;

	KWndObjectBox	m_LiveSkill[LIVE_SKILL_COUNT];
//	KUiSkillData	m_Skills[LIVE_SKILL_COUNT];
	struct
	{
		int		nFont;
		SIZE	Offset;
		unsigned int Color;
	}			m_SkillTextParam;
};

class KUiSkillsNew : public KWndPageSet
{
public:
	static KUiSkillsNew* OpenWindow();							//打开窗口，返回唯一的一个类对象实例
	static KUiSkillsNew* GetIfVisible();						//如果窗口正被显示，则返回实例指针
	static void		 CloseWindow(bool bDestroy);			//关闭窗口，同时可以选则是否删除对象实例
	static void		 LoadScheme(const char* pScheme);		//载入界面方案
	static void		 UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateFightRemainPoint(int nPoint);				//更新战斗技能升级点数
	void	UpdateLiveBaseData();							//更新生活技能公共数据
	void	UpdateFaction(int faction);

private:
	KUiSkillsNew(){}
	~KUiSkillsNew() {}
	void	 Initialize();							//初始化
	int		   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
private:
	static KUiSkillsNew* m_pSelf;
private:
	KUiFightSkillNew	m_FightSkillPad;
	KUiLiveSkillNew	m_LiveSkillPad;
	KWndButton		m_FightSkillPadBtn;
	KWndButton		m_LiveSkillPadBtn;
	KWndButton		m_Close;
};
