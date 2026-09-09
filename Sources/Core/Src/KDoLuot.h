#ifndef KDoLuotH
#define KDoLuotH
// [DOLUOT 09/09 c] Bo lay mau con tro lenh luong chinh (dinh nghia trong KSubWorld.cpp). [Client] DoLuot=1, DoLuotNguong=ms (0 = moi tick/khung).
// Dung: { DoLuotPham p(1); ... } quanh tick the gioi (KSubWorld::Activate) va DoLuotPham p(2) quanh ve (KCoreShell::DrawGameSpace).
#ifndef _SERVER
LONG DoLuotBatDau(int nPha, LARGE_INTEGER* pLi0);	// tra so thu tu (0 = tat)
void DoLuotKetThuc(int nPha, LONG lSeq, const LARGE_INTEGER& li0);
struct DoLuotPham
{
	LARGE_INTEGER m_li0; int m_nPha; LONG m_lSeq;
	DoLuotPham(int nPha) : m_nPha(nPha) { m_lSeq = DoLuotBatDau(nPha, &m_li0); }
	~DoLuotPham() { if (m_lSeq) DoLuotKetThuc(m_nPha, m_lSeq, m_li0); }
};
#endif
#endif
