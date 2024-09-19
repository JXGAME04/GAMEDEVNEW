/* -------------------------------------------------------------------------
//	文件名		：	kautoai.h
//	创建者		：	luobaohang
//	创建时间	：	2007-11-3 12:24:53
//	功能描述	：	自动挂机AI
//
// -----------------------------------------------------------------------*/
#ifndef __KAUTOAI_H__
#define __KAUTOAI_H__

#include <vector>
#include <string>

// -------------------------------------------------------------------------
#define DIS_MOVE  500	//判断能直接走的距离

class AutoAi;
//自动寻路类
class KAutoMove
{
public:
	KAutoMove() { m_CurDir = -1; }
	INT AutoMove();	//以当前方向随机寻路移动
	INT	AutoMoveTo(INT nX, INT nY);	//向目标点寻路移动
	VOID Reset(){ m_CurDir = -1; };
protected:
	//向某一方向寻路并Move，返回移动距离及方向
	INT FindPathAndMove(INT &nDir8, INT minStep = -1);
	//检查直线能走的最远距离
	inline INT	CheckDirectMoveDis(KNpc *pNpc, INT nDir8, INT nSpeed);
	INT m_CurDir;
};

// -------------------------------------------------------------------------

#endif /* __KAUTOAI_H__ */