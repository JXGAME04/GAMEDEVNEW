# -*- coding: utf-8 -*-
"""goi_va_cay_c_0909.py - [CAY 09/09 c] Cay canh (KIpoTree/KIpotBranch): bo 2 vong duyet CA danh sach la moi lan vat dong di chuyen.

SO DO (DOLUOT 16:0x, ho so tick, luc dong): KIpotBranch::AddPointLeafToList = 44-48 % thoi gian tick.
Co che cu (KScenePlaceC::MoveObject): moi lan NPC/dan doi toa do -> PluckRto (go) + AddLeafPoint (chen lai):
  vong 1 duyet CA danh sach la (hang tram vat dong) chi de tim 'vat duong' (BUILDIN, sap xep theo duong) khop;
  vong 2 duyet lai tu dau de tim cho chen theo y. ~1000 lan/tick x 2 x N nut.
Sua (ket qua Y HET cach cu, co dau do kiem 1/64 lan):
  (1) chi muc vat duong cua tung danh sach (khoa = dia chi con tro dau danh sach, xay lan dau, bo khi chen BUILDIN,
      xoa het khi Clear/Fell) -> vong 1 chi duyet vat duong (vai cai) thay vi ca danh sach.
  (2) KIpoTree::DoiViTri: tinh danh sach dich cua toa do CU va MOI (di xuong cay + khop vat duong qua chi muc);
      neu cung danh sach -> doi cho TAI CHO: giu nguyen neu khoa(truoc) <= y < khoa(sau), khong thi truot vai nut
      (danh sach tang dan theo khoa = y, vat duong = min(y dau, y cuoi) - dung y luat chen cu) va noi lai
      pAheadBrother/pBrother. Khac danh sach / dang co den dong / dang ngoai cay / lui gap vat tinh -> duong cu.
  (3) Dau do: 1/64 lan doi cho, duyet lai tu dau theo cach cu, so (truoc, sau) -> 'lech'. In o [WORLD b] 'cay c:'.
Khong doi hanh vi may chu (_SERVER giu nguyen ma cu).
"""
import io
import sys

NL = "\r\n"
T = "\t"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
TAG = "[CAY 09/09 c]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# ---------------- KIpotBranch.h: khai bao TimDanhSach ----------------
F = D + "Scene/KIpotBranch.h"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    s = rep(s, T + "void AddLineLeafToList(KIpotLeaf*& pFirst, KIpotBuildinObj* pLeaf);" + NL,
            T + "void AddLineLeafToList(KIpotLeaf*& pFirst, KIpotBuildinObj* pLeaf);" + NL +
            "  public:" + NL +
            T + "KIpotLeaf** TimDanhSach(const POINT& p, KIpotBranch*& pNhanh, KIpotLeaf*& pLaCha);" + T + "// " + TAG + " danh sach la ma toa do p se duoc chen vao (di xuong cay + khop vat duong), tra ve chu (nhanh hoac la cha)" + NL +
            "  private:" + NL, "H TimDanhSach")
    ghi(F, s, c0, l0, h0, "KIpotBranch.h")
else:
    print("KIpotBranch.h da co")

# ---------------- KIpotBranch.cpp ----------------
F = D + "Scene/KIpotBranch.cpp"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    # (a) khoi chi muc + ham phu, sau #define IS_BRANCH
    neo = "#define" + T + "IS_BRANCH(i) (m_uFlag & m_BranchFlag[i])" + NL
    khoi = NL.join([
        "#ifndef _SERVER",
        "#include <map>",
        "#include <vector>",
        "// " + TAG + " chi muc 'vat duong' (BUILDIN sap xep theo duong) cua tung danh sach la; khoa = dia chi con tro dau danh sach.",
        "// Xay lan dau khi can, bo khi chen BUILDIN vao danh sach do, xoa het khi cay Clear/Fell (KIpoTree).",
        "extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon;",
        "unsigned g_uCayTaiCho = 0, g_uCayDoiCho = 0, g_uCayBuoc = 0, g_uCayKhacDs = 0, g_uCayLuiTinh = 0, g_uCayKhacCha = 0, g_uCayNgoai = 0, g_uCayDo = 0, g_uCayLech = 0, g_uCayXayChiMuc = 0;",
        "struct KCayDsDuong { std::vector<KIpotBuildinObj*> aDuong; };",
        "static std::map<KIpotLeaf**, KCayDsDuong> s_CayDuong;",
        "void CayDuongXoaHet() { s_CayDuong.clear(); }",
        "static inline void CayDuongBo(KIpotLeaf** ppFirst) { if (!s_CayDuong.empty()) s_CayDuong.erase(ppFirst); }",
        "static inline bool CayLaDuong(const KIpotLeaf* pL)",
        "{",
        T + "return pL->eLeafType == KIpotLeaf::IPOTL_T_BUILDIN_OBJ && (((const KIpotBuildinObj*)pL)->pBio->Props & SPBIO_P_SORTMANNER_MASK) == SPBIO_P_SORTMANNER_LINE;",
        "}",
        "static const std::vector<KIpotBuildinObj*>& CayDuongLay(KIpotLeaf** ppFirst)",
        "{",
        T + "std::map<KIpotLeaf**, KCayDsDuong>::iterator it = s_CayDuong.find(ppFirst);",
        T + "if (it != s_CayDuong.end()) return it->second.aDuong;",
        T + "KCayDsDuong& d = s_CayDuong[ppFirst]; g_uCayXayChiMuc++;",
        T + "for (KIpotLeaf* pL = *ppFirst; pL; pL = pL->pBrother) if (CayLaDuong(pL)) d.aDuong.push_back((KIpotBuildinObj*)pL);",
        T + "return d.aDuong;",
        "}",
        "// khop vat duong: dung y vong 1 cua AddPointLeafToList (WIDTH_EXPAND 13, HEIGHT_UP_EXPAND 3, HEIGHT_DOWN_EXPAND 128)",
        "static inline bool CayKhopDuong(const POINT& lp, const KIpotBuildinObj* pL, RELATION_ENUM& eRelate)",
        "{",
        T + "const POINT op1 = pL->oPosition, op2 = pL->oEndPos;",
        T + "eRelate = SM_Relation_PointLine(lp, op1, op2);",
        T + "RECT rcArea;",
        T + "if (op1.x < op2.x) { rcArea.left = op1.x - 13; rcArea.right = op2.x + 13; } else { rcArea.left = op2.x - 13; rcArea.right = op1.x + 13; }",
        T + "if (op1.y < op2.y) { rcArea.top = op1.y; rcArea.bottom = op2.y; } else { rcArea.top = op2.y; rcArea.bottom = op1.y; }",
        T + "if (eRelate != RELATION_UP) { rcArea.top -= 3; rcArea.bottom += 128; }",
        T + "return lp.x > rcArea.left && lp.x < rcArea.right && lp.y >= rcArea.top && lp.y <= rcArea.bottom;",
        "}",
        "// khoa sap xep cua mot nut trong danh sach: dung y vong 2 (BUILDIN khong phai POINT = min(y dau, y cuoi); con lai = y)",
        "static inline int CayKhoa(const KIpotLeaf* pL)",
        "{",
        T + "if (pL->eLeafType == KIpotLeaf::IPOTL_T_BUILDIN_OBJ && (((const KIpotBuildinObj*)pL)->pBio->Props & SPBIO_P_SORTMANNER_MASK) != SPBIO_P_SORTMANNER_POINT)",
        T + "{ const int a = pL->oPosition.y, b = ((const KIpotBuildinObj*)pL)->oEndPos.y; return a < b ? a : b; }",
        T + "return pL->oPosition.y;",
        "}",
        "// dau do: duyet lai tu dau theo cach cu (chen truoc nut dau tien co khoa > y) va so voi cho hien tai cua pLeaf",
        "static void CayDoDung(KIpotLeaf* pFirst, KIpotRuntimeObj* pLeaf, int y)",
        "{",
        T + "g_uCayDo++;",
        T + "KIpotLeaf* pTr = NULL; KIpotLeaf* pL = pFirst; bool bThay = false;",
        T + "for (; pL; pL = pL->pBrother) { if (pL == pLeaf) { bThay = true; continue; } if (y < CayKhoa(pL)) break; pTr = pL; }",
        T + "if (!bThay) { for (KIpotLeaf* q = pL; q; q = q->pBrother) if (q == pLeaf) { bThay = true; break; } }",
        T + "if (!bThay || pLeaf->pAheadBrother != pTr || pLeaf->pBrother != pL) g_uCayLech++;",
        "}",
        "// doi cho tai cho trong CUNG danh sach; tra ve false = chua sua gi, di duong cu (PluckRto + AddLeafPoint)",
        "bool CayDoiChoTaiCho(KIpotLeaf*& pFirst, KIpotRuntimeObj* pLeaf, const POINT& oMoi)",
        "{",
        T + "const int y = oMoi.y;",
        T + "KIpotLeaf* pTruoc = pLeaf->pAheadBrother;",
        T + "KIpotLeaf* pSau = pLeaf->pBrother;",
        T + "if (pTruoc == NULL ? (pFirst != pLeaf) : (pTruoc->pBrother != pLeaf)) { g_uCayKhacCha++; return false; }",
        T + "if ((pTruoc == NULL || CayKhoa(pTruoc) <= y) && (pSau == NULL || y < CayKhoa(pSau)))",
        T + "{",
        T*2 + "pLeaf->oPosition = oMoi; g_uCayTaiCho++;",
        T*2 + "if (((g_uCayTaiCho + g_uCayDoiCho) & 63) == 0) CayDoDung(pFirst, pLeaf, y);",
        T*2 + "return true;",
        T + "}",
        T + "if (pSau != NULL && CayKhoa(pSau) <= y)",
        T + "{" + T + "// tien: chen SAU nut cuoi cung co khoa <= y",
        T*2 + "KIpotLeaf* pDich = pSau; g_uCayBuoc++;",
        T*2 + "while (pDich->pBrother && CayKhoa(pDich->pBrother) <= y) { pDich = pDich->pBrother; g_uCayBuoc++; }",
        T*2 + "if (pTruoc) pTruoc->pBrother = pSau; else pFirst = pSau;",
        T*2 + "if (pSau->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ) ((KIpotRuntimeObj*)pSau)->pAheadBrother = pTruoc;",
        T*2 + "pLeaf->pBrother = pDich->pBrother; pLeaf->pAheadBrother = pDich; pDich->pBrother = pLeaf;",
        T*2 + "if (pLeaf->pBrother && pLeaf->pBrother->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ) ((KIpotRuntimeObj*)pLeaf->pBrother)->pAheadBrother = pLeaf;",
        T + "}",
        T + "else",
        T + "{" + T + "// lui (pTruoc != NULL, khoa(pTruoc) > y): chen TRUOC nut xa nhat (lui tu pTruoc) van co khoa > y",
        T*2 + "KIpotLeaf* pDich = pTruoc; g_uCayBuoc++;",
        T*2 + "for (;;)",
        T*2 + "{",
        T*3 + "if (pDich->eLeafType != KIpotLeaf::IPOTL_T_RUNTIME_OBJ) { g_uCayLuiTinh++; return false; }" + T + "// vat tinh khong co con tro nut truoc -> duong cu",
        T*3 + "KIpotLeaf* pTr = ((KIpotRuntimeObj*)pDich)->pAheadBrother;",
        T*3 + "if (pTr == NULL || CayKhoa(pTr) <= y) break;",
        T*3 + "pDich = pTr; g_uCayBuoc++;",
        T*2 + "}",
        T*2 + "KIpotLeaf* pTr = ((KIpotRuntimeObj*)pDich)->pAheadBrother;",
        T*2 + "pTruoc->pBrother = pSau;",
        T*2 + "if (pSau && pSau->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ) ((KIpotRuntimeObj*)pSau)->pAheadBrother = pTruoc;",
        T*2 + "if (pTr) pTr->pBrother = pLeaf; else pFirst = pLeaf;",
        T*2 + "pLeaf->pAheadBrother = pTr; pLeaf->pBrother = pDich; ((KIpotRuntimeObj*)pDich)->pAheadBrother = pLeaf;",
        T + "}",
        T + "pLeaf->oPosition = oMoi; g_uCayDoiCho++;",
        T + "if (((g_uCayTaiCho + g_uCayDoiCho) & 63) == 0) CayDoDung(pFirst, pLeaf, y);",
        T + "return true;",
        "}",
        "KIpotLeaf** KIpotBranch::TimDanhSach(const POINT& p, KIpotBranch*& pNhanh, KIpotLeaf*& pLaCha)",
        "{",
        T + "KIpotBranch* pB = this; int nIndex;",
        T + "for (;;)",
        T + "{",
        T*2 + "const RELATION_ENUM e = SM_Relation_PointLine(p, pB->m_oHeadPoint, pB->m_oEndPoint);",
        T*2 + "nIndex = (e == RELATION_UP) ? 0 : 1;",
        T*2 + "if (pB->m_uFlag & m_BranchFlag[nIndex]) pB = pB->m_pSubBranch[nIndex]; else break;",
        T + "}",
        T + "KIpotLeaf** pp = &pB->m_pLeafs[nIndex];",
        T + "pNhanh = pB; pLaCha = NULL;",
        T + "for (int nSau = 0; *pp != NULL && nSau < 64; nSau++)",
        T + "{",
        T*2 + "const std::vector<KIpotBuildinObj*>& aDuong = CayDuongLay(pp);",
        T*2 + "KIpotBuildinObj* pKhop = NULL; RELATION_ENUM eKhop = RELATION_UP, e;",
        T*2 + "for (size_t u = 0; u < aDuong.size(); u++)",
        T*2 + "{",
        T*3 + "if (!CayKhopDuong(p, aDuong[u], e)) continue;",
        T*3 + "pKhop = aDuong[u]; eKhop = e;",
        T*3 + "if (e == RELATION_UP) break;",
        T*2 + "}",
        T*2 + "if (pKhop == NULL) break;",
        T*2 + "pNhanh = NULL; pLaCha = pKhop;",
        T*2 + "pp = (eKhop == RELATION_DOWN) ? &pKhop->pRChild : &pKhop->pLChild;",
        T + "}",
        T + "return pp;",
        "}",
        "#endif",
        "",
    ])
    s = rep(s, neo, neo + khoi, "C khoi")
    # (b) AddPointLeafToList: bo chi muc khi chen BUILDIN
    neo = T + "_ASSERT(pLeaf);" + NL + T + "if (pFirst == NULL)" + NL + T + "{" + NL + T*2 + "pFirst = pLeaf;" + NL + T*2 + "if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)"
    s = rep(s, neo, T + "_ASSERT(pLeaf);" + NL + "#ifndef _SERVER" + NL + T + "if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_BUILDIN_OBJ) CayDuongBo(&pFirst);" + T + "// " + TAG + NL + "#endif" + NL + neo[len(T + "_ASSERT(pLeaf);" + NL):], "C AddPointLeafToList bo chi muc")
    # (c) vong 1 -> duyet chi muc
    old = NL.join([
        T + "for(pL = pFirst; pL; pL = pL->pBrother)",
        T + "{",
        "#ifndef _SERVER",
        T*2 + "g_uCayDuyet1++;" + T + "// [CAY 09/09 do]",
        "#endif",
        T*2 + "if (pL->eLeafType != KIpotLeaf::IPOTL_T_BUILDIN_OBJ)",
        T*3 + "continue;",
        T*2 + "if ((((KIpotBuildinObj*)pL)->pBio->Props & SPBIO_P_SORTMANNER_MASK) != SPBIO_P_SORTMANNER_LINE)",
        T*3 + "continue;",
        "",
    ])
    new = NL.join([
        "#ifndef _SERVER",
        T + "const std::vector<KIpotBuildinObj*>& aDuong = CayDuongLay(&pFirst);" + T + "// " + TAG + " chi duyet vat duong (chi muc), khong duyet ca danh sach",
        T + "for (size_t uD = 0; uD < aDuong.size(); uD++)",
        T + "{",
        T*2 + "pL = aDuong[uD]; g_uCayDuyet1++;",
        "#else",
        T + "for(pL = pFirst; pL; pL = pL->pBrother)",
        T + "{",
        T*2 + "if (pL->eLeafType != KIpotLeaf::IPOTL_T_BUILDIN_OBJ)",
        T*3 + "continue;",
        T*2 + "if ((((KIpotBuildinObj*)pL)->pBio->Props & SPBIO_P_SORTMANNER_MASK) != SPBIO_P_SORTMANNER_LINE)",
        T*3 + "continue;",
        "#endif",
        "",
    ])
    s = rep(s, old, new, "C vong 1")
    # (d) AddLineLeafToList: bo chi muc
    neo = "void KIpotBranch::AddLineLeafToList(KIpotLeaf*& pFirst, KIpotBuildinObj* pLeaf)" + NL + "{" + NL + T + "_ASSERT(pLeaf);" + NL
    s = rep(s, neo, neo + "#ifndef _SERVER" + NL + T + "CayDuongBo(&pFirst);" + T + "// " + TAG + NL + "#endif" + NL, "C AddLineLeafToList")
    ghi(F, s, c0, l0, h0, "KIpotBranch.cpp")
else:
    print("KIpotBranch.cpp da co")

# ---------------- KIpoTree.h ----------------
F = D + "Scene/KIpoTree.h"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    s = rep(s, T + "void PluckRto(KIpotRuntimeObj* pLeaf);" + NL,
            T + "void PluckRto(KIpotRuntimeObj* pLeaf);" + NL +
            T + "bool DoiViTri(KIpotRuntimeObj* pLeaf, const POINT& oMoi);" + T + "// " + TAG + " doi toa do vat dong tai cho neu van cung danh sach la; false = chua sua gi, goi duong cu" + NL, "H DoiViTri")
    ghi(F, s, c0, l0, h0, "KIpoTree.h")
else:
    print("KIpoTree.h da co")

# ---------------- KIpoTree.cpp ----------------
F = D + "Scene/KIpoTree.cpp"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    s = rep(s, T + "m_DefaultBranch.Clear();" + NL + T + "RemoveRtoGroupWithPermanentLeaf();",
            T + "m_DefaultBranch.Clear();" + NL + "#ifndef _SERVER" + NL + T + "{ extern void CayDuongXoaHet(); CayDuongXoaHet(); }" + T + "// " + TAG + NL + "#endif" + NL + T + "RemoveRtoGroupWithPermanentLeaf();", "C Clear")
    s = rep(s, T + "m_DefaultBranch.RemoveAllRtoLeafs(&m_PermanentLeaf);" + NL + T + "m_DefaultBranch.Clear();" + NL,
            T + "m_DefaultBranch.RemoveAllRtoLeafs(&m_PermanentLeaf);" + NL + T + "m_DefaultBranch.Clear();" + NL + "#ifndef _SERVER" + NL + T + "{ extern void CayDuongXoaHet(); CayDuongXoaHet(); }" + T + "// " + TAG + NL + "#endif" + NL, "C Fell")
    neo = "void KIpoTree::PluckRto(KIpotRuntimeObj* pLeaf)" + NL + "{"
    ham = NL.join([
        "#ifndef _SERVER",
        "// " + TAG + " doi toa do vat dong TAI CHO khi danh sach la dich (toa do moi) trung danh sach hien tai (toa do cu);",
        "// khong Pluck + chen lai (2 vong duyet ca danh sach ~1000 lan/tick luc dong). false = chua sua gi -> duong cu.",
        "extern bool CayDoiChoTaiCho(KIpotLeaf*& pFirst, KIpotRuntimeObj* pLeaf, const POINT& oMoi);",
        "extern unsigned g_uCayKhacDs, g_uCayNgoai, g_uCayKhacCha;",
        "bool KIpoTree::DoiViTri(KIpotRuntimeObj* pLeaf, const POINT& oMoi)",
        "{",
        T + "if (m_nDenDong > 0) return false;" + T + "// dang co den dong (BanDoSang=1): duong cu xoa den + tao lai den o cho moi",
        T + "if (pLeaf->pParentBranch == NULL && pLeaf->pParentLeaf == NULL) { g_uCayNgoai++; return false; }" + T + "// dang ngoai cay (m_PermanentLeaf)",
        T + "KIpotBranch* pGoc = m_pMainBranch ? m_pMainBranch : &m_DefaultBranch;",
        T + "KIpotBranch *pNhanhCu = NULL, *pNhanhMoi = NULL; KIpotLeaf *pChaCu = NULL, *pChaMoi = NULL;",
        T + "KIpotLeaf** ppCu = pGoc->TimDanhSach(pLeaf->oPosition, pNhanhCu, pChaCu);",
        T + "if (pNhanhCu != pLeaf->pParentBranch || pChaCu != pLeaf->pParentLeaf) { g_uCayKhacCha++; return false; }",
        T + "KIpotLeaf** ppMoi = pGoc->TimDanhSach(oMoi, pNhanhMoi, pChaMoi);",
        T + "if (ppMoi != ppCu) { g_uCayKhacDs++; return false; }",
        T + "return CayDoiChoTaiCho(*ppMoi, pLeaf, oMoi);",
        "}",
        "#endif",
        "",
    ])
    s = rep(s, neo, ham + neo, "C DoiViTri")
    ghi(F, s, c0, l0, h0, "KIpoTree.cpp")
else:
    print("KIpoTree.cpp da co")

# ---------------- KScenePlaceC.cpp: MoveObject ----------------
F = D + "Scene/KScenePlaceC.cpp"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    old = NL.join([
        T*3 + "EnterCriticalSection(&m_ProcessCritical);",
        T*3 + "m_ObjectsTree.PluckRto(pLeaf);",
        T*3 + "pLeaf->oPosition.x = x;",
        T*3 + "pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;",
        T*3 + "m_ObjectsTree.AddLeafPoint(pLeaf);",
        T*3 + "LeaveCriticalSection(&m_ProcessCritical);",
        "",
    ])
    new = NL.join([
        T*3 + "EnterCriticalSection(&m_ProcessCritical);",
        "#ifndef _SERVER",
        T*3 + "POINT oMoi; oMoi.x = x; oMoi.y = y + POINT_LEAF_Y_ADJUST_VALUE;" + T + "// " + TAG + " doi cho tai cho neu cung danh sach la",
        T*3 + "if (!m_ObjectsTree.DoiViTri(pLeaf, oMoi))",
        T*3 + "{",
        "#endif",
        T*3 + "m_ObjectsTree.PluckRto(pLeaf);",
        T*3 + "pLeaf->oPosition.x = x;",
        T*3 + "pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;",
        T*3 + "m_ObjectsTree.AddLeafPoint(pLeaf);",
        "#ifndef _SERVER",
        T*3 + "}",
        "#endif",
        T*3 + "LeaveCriticalSection(&m_ProcessCritical);",
        "",
    ])
    s = rep(s, old, new, "C MoveObject")
    ghi(F, s, c0, l0, h0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")

# ---------------- KSubWorldSet.cpp: in [WORLD b] ----------------
F = D + "KSubWorldSet.cpp"
s, c0, l0, h0 = doc(F)
if TAG not in s:
    s = rep(s, "extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon; extern unsigned g_uAmNap; extern double g_dAmNapMs;",
            "extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon; extern unsigned g_uAmNap; extern double g_dAmNapMs; extern unsigned g_uCayTaiCho, g_uCayDoiCho, g_uCayBuoc, g_uCayKhacDs, g_uCayLuiTinh, g_uCayKhacCha, g_uCayNgoai, g_uCayDo, g_uCayLech, g_uCayXayChiMuc; /* " + TAG + " */",
            "S extern")
    s = rep(s, "khop %u con %u | am thanh %u lan %.2f ms",
            "khop %u con %u | cay c: tai cho %u doi cho %u buoc %u | duong cu: khac ds %u lui tinh %u khac cha %u ngoai %u | do %u lech %u | chi muc xay %u | am thanh %u lan %.2f ms",
            "S fmt")
    s = rep(s, "g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon, g_uAmNap, g_dAmNapMs);",
            "g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon, g_uCayTaiCho, g_uCayDoiCho, g_uCayBuoc, g_uCayKhacDs, g_uCayLuiTinh, g_uCayKhacCha, g_uCayNgoai, g_uCayDo, g_uCayLech, g_uCayXayChiMuc, g_uAmNap, g_dAmNapMs);",
            "S args")
    s = rep(s, "g_uCayChen = g_uCayDuyet1 = g_uCayDuyet2 = g_uCayKhop = g_uCayCon = 0;",
            "g_uCayChen = g_uCayDuyet1 = g_uCayDuyet2 = g_uCayKhop = g_uCayCon = 0; g_uCayTaiCho = g_uCayDoiCho = g_uCayBuoc = g_uCayKhacDs = g_uCayLuiTinh = g_uCayKhacCha = g_uCayNgoai = g_uCayDo = g_uCayLech = g_uCayXayChiMuc = 0;",
            "S reset")
    ghi(F, s, c0, l0, h0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")
print("XONG " + TAG)
