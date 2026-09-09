# -*- coding: utf-8 -*-
"""goi_va_vung_doluot_0909.py - [VUNG 09/09] + [DOLUOT 09/09] (client), theo so do tran TK 14:28-14:29:

[WORLD-TICK] 35 tick >= 20 ms trong 30 s: (1) vong DAN 20-35 ms/tick khi 500-1100 dan; (2) MOT NPC 13-30 ms o pha PC/ST.
AutoLog da loai tru bang dau thoi gian (0-36 dong/50 ms truoc tick nang). Nap dong bo sprite loai tru ([REP3-NAP] 5-7 lan/30 s).

(1) VUNG: KRegion::FindNpc (KRegion.h, inline, dung chung) DUYET TUYEN TINH ca m_NpcList cua vung cho MOI o hoi
    (sau khi m_pNpcRef[o] != 0). Dan: CheckNearestCollision hoi 9 o, ProcessCollision hoi (2R+1)^2 o => ~1000 dan x 9 o x
    130 NPC/vung = ~1,2 trieu lan doc Npc[] (cache miss) moi tick. Sua (CHI client): chi muc o -> NPC xay lai khi danh sach/o
    NPC doi (phien ban m_uNpcDoi tang o AddNpc/RemoveNpc/AddRef/DecRef obj_npc; Close/Init), giu DUNG THU TU m_NpcList
    (counting sort on dinh) => ket qua y het duyet. May chu giu nguyen duong duyet. Bo do (PaintLog=1): moi 64 lan hoi
    so voi duyet cu -> dem lech [WORLD b] 'vung: xay N so N lech N'.
(2) DOLUOT: khong doan nua - lay mau con tro lenh luong chinh moi ~1 ms tren luong rieng (SuspendThread/GetThreadContext),
    gan pha (1 = tick the gioi, 2 = ve) + so thu tu; tick/khung ve >= 20 ms duoc danh dau -> gom mau -> moi 30 s in
    [DOLUOT] top 12 ham (dbghelp SymFromAddr, kem module+RVA de tra offline). Cong tac [Client] DoLuot=1 (mac dinh 0).
"""
import io
import sys

NL = "\r\n"
T = "\t"
CORE = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
F_RH = CORE + "KRegion.h"
F_RC = CORE + "KRegion.cpp"
F_SW = CORE + "KSubWorld.cpp"
F_SWS = CORE + "KSubWorldSet.cpp"
TAG_V = "[VUNG 09/09]"
TAG_D = "[DOLUOT 09/09]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


def rep_sau(s, moc, old, new, ten):
    """thay lan xuat hien DAU TIEN cua old SAU vi tri cua moc (moc phai duy nhat)"""
    if s.count(moc) != 1:
        print("FAIL moc %s: %d" % (ten, s.count(moc))); sys.exit(1)
    a = s.find(moc)
    p = s.find(old, a)
    if p < 0:
        print("FAIL neo %s: khong thay sau moc" % ten); sys.exit(1)
    return s[:p] + new + s[p + len(old):]


# ================================================================ KRegion.h
s, c0, l0, h0 = doc(F_RH)
if TAG_V not in s:
    # (a) khai bao: FindNpcDuyet + thanh vien chi muc (client)
    old = T + "int" + T*3 + "FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx = 0);"
    new = old + NL + T + "int" + T*3 + "FindNpcDuyet(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx);" + T + "// " + TAG_V + " duong duyet danh sach cu (may chu dung; client dung de do lech)"
    s = rep(s, old, new, "RH khai bao")
    old = "#ifndef _SERVER" + NL + T + "void" + T*2 + "Paint();" + NL + "#endif"
    new = NL.join([
        "#ifndef _SERVER",
        T + "void" + T*2 + "Paint();",
        T + "// " + TAG_V + " chi muc o -> NPC cho FindNpc: dan (CheckNearestCollision 9 o, ProcessCollision (2R+1)^2 o) hoi hang nghin",
        T + "// o moi tick, moi lan duyet ca m_NpcList (130 NPC) = 1,2 trieu lan doc Npc[] / tick (do [WORLD-TICK] 14:28: dan 20-35 ms).",
        T + "// Xay lai khi m_uNpcDoi != m_uNpcChiMuc (NPC vao/ra vung, doi o). Thu tu trong o = thu tu m_NpcList => y het duyet.",
        T + "unsigned" + T + "m_uNpcDoi;" + T*2 + "// tang o AddNpc/RemoveNpc/AddRef/DecRef(obj_npc)/Init/Close",
        T + "unsigned" + T + "m_uNpcChiMuc;" + T + "// phien ban chi muc dang giu",
        T + "int" + T*2 + "m_nChiMucO;" + T*2 + "// so o + 1 cua m_pChiMucODau (de biet khi kich thuoc vung doi)",
        T + "int*" + T*2 + "m_pChiMucODau;" + T + "// [so o + 1]: o k giu m_pChiMucNpc[ODau[k] .. ODau[k+1])",
        T + "int*" + T*2 + "m_pChiMucKe;" + T*2 + "// [so o] con tro dien tam khi xay",
        T + "int*" + T*2 + "m_pChiMucNpc;" + T*2 + "// [so NPC] chi so NPC, theo o roi theo thu tu danh sach",
        T + "int" + T*2 + "m_nChiMucNpcCap;",
        T + "void" + T*2 + "XayChiMucNpc();",
        "#endif",
    ])
    s = rep(s, old, new, "RH thanh vien")
    # (b) FindNpc moi (client: chi muc) + doi ten than cu thanh FindNpcDuyet
    old = "inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx)" + NL + "{"
    new = NL.join([
        "#ifndef _SERVER",
        "extern int g_nCorePaintLog;",
        "extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;" + T + "// " + TAG_V + " bo do: so lan hoi, so lan chi muc khac duyet, so lan xay",
        "#endif",
        "inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx)",
        "{",
        "#ifndef _SERVER",
        T + "// " + TAG_V + " client: tra chi muc o thay vi duyet ca danh sach (cung thu tu, cung phep chon)",
        T + "if (nMapX < 0 || nMapY < 0 || nMapX >= m_nWidth || nMapY >= m_nHeight)",
        T*2 + "return 0;",
        T + "if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)",
        T*2 + "return 0;",
        T + "if (m_uNpcChiMuc != m_uNpcDoi || !m_pChiMucODau)",
        T*2 + "XayChiMucNpc();",
        T + "if (m_pChiMucODau)",
        T + "{",
        T*2 + "const int nO = nMapY * m_nWidth + nMapX;",
        T*2 + "int nFallback = 0, nKq = 0;",
        T*2 + "bool bXong = false;",
        T*2 + "for (int k = m_pChiMucODau[nO]; k < m_pChiMucODau[nO + 1] && !bXong; k++)",
        T*2 + "{",
        T*3 + "const int nIdx = m_pChiMucNpc[k];",
        T*3 + "if (nIdx <= 0 || nIdx >= MAX_NPC || Npc[nIdx].m_MapX != nMapX || Npc[nIdx].m_MapY != nMapY)",
        T*4 + "continue;" + T + "// phong ho: NPC doi o ma khong qua AddRef/DecRef (chi muc cu) -> bo qua nhu m_pNpcRef",
        T*3 + "if (NpcSet.GetRelation(nNpcIdx, nIdx) & nRelation)",
        T*3 + "{",
        T*4 + "if (nPreferIdx <= 0) { nKq = nIdx; bXong = true; break; }" + T + "// con dau tien theo thu tu danh sach",
        T*4 + "if (nIdx == nPreferIdx) { nKq = nIdx; bXong = true; break; }",
        T*4 + "if (nFallback == 0)",
        T*5 + "nFallback = nIdx;",
        T*4 + "else if ((Npc[nFallback].m_Doing == do_death || Npc[nFallback].m_Doing == do_revive) &&",
        T*5 + "Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)",
        T*5 + "nFallback = nIdx;",
        T*3 + "}",
        T*2 + "}",
        T*2 + "if (!bXong) nKq = nFallback;",
        T*2 + "if (g_nCorePaintLog > 0)" + T + "// bo do: moi 64 lan so voi duyet cu",
        T*2 + "{",
        T*3 + "if ((++g_uVungSo & 63) == 0 && FindNpcDuyet(nMapX, nMapY, nNpcIdx, nRelation, nPreferIdx) != nKq)",
        T*4 + "g_uVungLech++;",
        T*2 + "}",
        T*2 + "return nKq;",
        T + "}",
        "#endif",
        T + "return FindNpcDuyet(nMapX, nMapY, nNpcIdx, nRelation, nPreferIdx);",
        "}",
        "inline int KRegion::FindNpcDuyet(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx)",
        "{",
    ])
    s = rep(s, old, new, "RH FindNpc")
    ghi(F_RH, s, c0, l0, h0, "KRegion.h")
else:
    print("KRegion.h da co")

# ================================================================ KRegion.cpp
s, c0, l0, h0 = doc(F_RC)
if TAG_V not in s:
    # ctor
    old = T + "m_pMslRef" + T*2 + "= NULL;"
    new = old + NL + "#ifndef _SERVER" + NL + T + "m_uNpcDoi = 1; m_uNpcChiMuc = 0; m_nChiMucO = 0; m_pChiMucODau = NULL; m_pChiMucKe = NULL; m_pChiMucNpc = NULL; m_nChiMucNpcCap = 0;" + T + "// " + TAG_V + NL + "#endif"
    s = rep(s, old, new, "RC ctor")
    # dtor: khoi if (m_pMslRef) DAU TIEN sau 'KRegion::~KRegion()'
    old = T + "if (m_pMslRef)" + NL + T*2 + "delete [] m_pMslRef;"
    new = old + NL + "#ifndef _SERVER" + NL + T + "delete [] m_pChiMucODau; delete [] m_pChiMucKe; delete [] m_pChiMucNpc;" + T + "// " + TAG_V + NL + "#endif"
    s = rep_sau(s, "KRegion::~KRegion()", old, new, "RC dtor")
    # Init: doi phien ban
    old = T + "ZeroMemory(m_pNpcRef, nWidth * nHeight);"
    new = old + NL + "#ifndef _SERVER" + NL + T + "m_uNpcDoi++;" + T + "// " + TAG_V + " kich thuoc/du lieu vung doi -> xay lai chi muc" + NL + "#endif"
    s = rep(s, old, new, "RC Init")
    # AddRef / DecRef: case obj_npc DAU TIEN sau moi dau ham
    old = T + "case obj_npc:" + NL + T*2 + "pBuffer = m_pNpcRef;" + NL + T*2 + "break;"
    new = T + "case obj_npc:" + NL + T*2 + "pBuffer = m_pNpcRef;" + NL + "#ifndef _SERVER" + NL + T*2 + "m_uNpcDoi++;" + T + "// " + TAG_V + NL + "#endif" + NL + T*2 + "break;"
    s = rep_sau(s, "BOOL KRegion::AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)", old, new, "RC AddRef")
    s = rep_sau(s, "BOOL KRegion::DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)", old, new, "RC DecRef")
    # AddNpc / RemoveNpc
    old = T*3 + "m_NpcList.AddTail(&Npc[nIdx].m_Node);"
    new = "#ifndef _SERVER" + NL + T*3 + "m_uNpcDoi++;" + T + "// " + TAG_V + NL + "#endif" + NL + old
    s = rep(s, old, new, "RC AddNpc")
    old = T*2 + "Npc[nIdx].m_Node.Remove();"
    new = "#ifndef _SERVER" + NL + T*2 + "m_uNpcDoi++;" + T + "// " + TAG_V + NL + "#endif" + NL + old
    s = rep(s, old, new, "RC RemoveNpc")
    # XayChiMucNpc + bo dem, dat truoc AddNpc
    old = "void KRegion::AddNpc(int nIdx)"
    new = NL.join([
        "#ifndef _SERVER",
        "unsigned g_uVungSo = 0, g_uVungLech = 0, g_uVungXay = 0;" + T + "// " + TAG_V + " bo do (in o [WORLD b])",
        "// " + TAG_V + " Xay chi muc o -> NPC: counting sort ON DINH theo thu tu m_NpcList (con dau tien trong danh sach van la",
        "// con dau tien trong o). Chi chay khi phien ban doi (vai lan/tick luc dong, O(so NPC + so o)).",
        "void KRegion::XayChiMucNpc()",
        "{",
        T + "const int nO = m_nWidth * m_nHeight;",
        T + "m_uNpcChiMuc = m_uNpcDoi;",
        T + "if (nO <= 0 || !m_pNpcRef)",
        T + "{",
        T*2 + "delete [] m_pChiMucODau; m_pChiMucODau = NULL; m_nChiMucO = 0;",
        T*2 + "return;",
        T + "}",
        T + "if (!m_pChiMucODau || m_nChiMucO != nO + 1)",
        T + "{",
        T*2 + "delete [] m_pChiMucODau; delete [] m_pChiMucKe;",
        T*2 + "m_pChiMucODau = new int[nO + 1]; m_pChiMucKe = new int[nO]; m_nChiMucO = nO + 1;",
        T + "}",
        T + "int nSo = 0;",
        T + "KIndexNode* pNode = (KIndexNode*)m_NpcList.GetHead();",
        T + "while (pNode) { nSo++; pNode = (KIndexNode*)pNode->GetNext(); }",
        T + "if (nSo > m_nChiMucNpcCap)",
        T + "{",
        T*2 + "delete [] m_pChiMucNpc; m_nChiMucNpcCap = nSo + 64; m_pChiMucNpc = new int[m_nChiMucNpcCap];",
        T + "}",
        T + "memset(m_pChiMucODau, 0, sizeof(int) * (nO + 1));",
        T + "for (pNode = (KIndexNode*)m_NpcList.GetHead(); pNode; pNode = (KIndexNode*)pNode->GetNext())",
        T + "{",
        T*2 + "const int i = pNode->m_nIndex;",
        T*2 + "if (i <= 0 || i >= MAX_NPC) continue;",
        T*2 + "const int x = Npc[i].m_MapX, y = Npc[i].m_MapY;",
        T*2 + "if (x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight) continue;",
        T*2 + "m_pChiMucODau[y * m_nWidth + x + 1]++;",
        T + "}",
        T + "for (int k = 0; k < nO; k++)",
        T + "{",
        T*2 + "m_pChiMucODau[k + 1] += m_pChiMucODau[k];",
        T*2 + "m_pChiMucKe[k] = m_pChiMucODau[k];",
        T + "}",
        T + "for (pNode = (KIndexNode*)m_NpcList.GetHead(); pNode; pNode = (KIndexNode*)pNode->GetNext())",
        T + "{",
        T*2 + "const int i = pNode->m_nIndex;",
        T*2 + "if (i <= 0 || i >= MAX_NPC) continue;",
        T*2 + "const int x = Npc[i].m_MapX, y = Npc[i].m_MapY;",
        T*2 + "if (x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight) continue;",
        T*2 + "m_pChiMucNpc[m_pChiMucKe[y * m_nWidth + x]++] = i;",
        T + "}",
        T + "g_uVungXay++;",
        "}",
        "#endif",
        "",
        old,
    ])
    s = rep(s, old, new, "RC XayChiMucNpc")
    ghi(F_RC, s, c0, l0, h0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ================================================================ KSubWorldSet.cpp: in bo do [WORLD b]
s, c0, l0, h0 = doc(F_SWS)
if TAG_V not in s:
    old = T*2 + "extern double g_dKhacMs[3]; extern unsigned g_uKhacSo[3];" + T + "// [WORLD 09/09 c]"
    new = old + NL + T*2 + "extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;" + T + "// " + TAG_V
    s = rep(s, old, new, "SWS extern")
    old = "| nguoi choi %.2f ms/tick\\n\","
    new = "| nguoi choi %.2f ms/tick | vung: xay %u, hoi %u, lech %u\\n\","
    s = rep(s, old, new, "SWS fmt")
    old = T*4 + "(double)g_uKhacSo[0] / g_uWorldTick, g_dKhacMs[0] / g_uWorldTick, (double)g_uKhacSo[1] / g_uWorldTick, g_dKhacMs[1] / g_uWorldTick, g_dKhacMs[2] / g_uWorldTick);"
    new = T*4 + "(double)g_uKhacSo[0] / g_uWorldTick, g_dKhacMs[0] / g_uWorldTick, (double)g_uKhacSo[1] / g_uWorldTick, g_dKhacMs[1] / g_uWorldTick, g_dKhacMs[2] / g_uWorldTick," + NL + \
          T*4 + "g_uVungXay, g_uVungSo, g_uVungLech);"
    s = rep(s, old, new, "SWS args")
    old = T*2 + "g_dKhacMs[0] = g_dKhacMs[1] = g_dKhacMs[2] = 0.0; g_uKhacSo[0] = g_uKhacSo[1] = g_uKhacSo[2] = 0;" + T + "// [WORLD 09/09 c]"
    new = old + NL + T*2 + "g_uVungSo = 0; g_uVungLech = 0; g_uVungXay = 0;" + T + "// " + TAG_V
    s = rep(s, old, new, "SWS reset")
    ghi(F_SWS, s, c0, l0, h0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

# ================================================================ KSubWorld.cpp: [DOLUOT] bo lay mau + moc tick/ve
s, c0, l0, h0 = doc(F_SW)
if TAG_D not in s:
    old = "void WorldTickXong(double dQuet)" + NL + "{"
    new = NL.join([
        "// " + TAG_D + " BO LAY MAU con tro lenh luong chinh ([Client] DoLuot=1; mac dinh 0 = khong tao luong, khong ton gi).",
        "// Luong rieng moi ~1 ms: SuspendThread/GetThreadContext luong chinh -> ghi (EIP, pha, so thu tu) vao vong 16384 mau",
        "// (chi khi luong chinh dang o pha 1 = tick the gioi hay 2 = ve). Luong chinh danh dau tick/khung >= 20 ms (DoLuotPham).",
        "// Luong lay mau gom mau cua cac tick/khung nang (tre >= 200 ms de moc kip), moi 30 s in [DOLUOT] top 12 dia chi:",
        "// module+RVA (tra offline bang pdb) + ten ham qua dbghelp (SymFromAddr, pdb ghi trong DLL). Khong doan nua.",
        "#include <process.h>",
        "#include <dbghelp.h>",
        "static int            g_nDoLuot = -1;" + T + "// -1 chua doc ini",
        "static volatile LONG  g_lDoLuotPha = 0;" + T + "// 0 ngoai, 1 tick, 2 ve",
        "static volatile LONG  g_lDoLuotSeq = 0;",
        "struct DoLuotMau { DWORD_PTR uEip; LONG lPha; LONG lSeq; DWORD dwLuc; };",
        "#define DOLUOT_MAU 16384",
        "#define DOLUOT_NANG 1024",
        "static DoLuotMau      g_aDoLuotMau[DOLUOT_MAU];",
        "static volatile LONG  g_lDoLuotGhi = 0;" + T + "// so mau da ghi (vong)",
        "static volatile LONG  g_aDoLuotNang[DOLUOT_NANG];" + T + "// vong so thu tu tick/khung nang (>= 20 ms)",
        "static volatile LONG  g_aDoLuotNangPha[DOLUOT_NANG];",
        "static HANDLE         g_hDoLuotChinh = NULL;",
        "struct DoLuotDem { DWORD_PTR uEip; unsigned uSo; };",
        "static void DoLuotIn(FILE* pLog, int nPha, DoLuotDem* aDem, int nDem, unsigned uMau, unsigned uLan);",
        "static unsigned __stdcall DoLuotLuong(void*)",
        "{",
        T + "static DoLuotDem aDem[2][4096]; static int nDem[2] = { 0, 0 }; static unsigned uMau[2] = { 0, 0 }, uLan[2] = { 0, 0 };",
        T + "static LONG aLanDem[2][DOLUOT_NANG];" + T + "// seq da dem lan nang (khoi dem 2 lan)",
        T + "LONG lDaGom = 0; DWORD dwIn = GetTickCount();",
        T + "for (;;)",
        T + "{",
        T*2 + "Sleep(1);",
        T*2 + "if (g_lDoLuotPha)",
        T*2 + "{",
        T*3 + "CONTEXT c; memset(&c, 0, sizeof(c)); c.ContextFlags = CONTEXT_CONTROL;",
        T*3 + "if (SuspendThread(g_hDoLuotChinh) != (DWORD)-1)",
        T*3 + "{",
        T*4 + "const BOOL bOk = GetThreadContext(g_hDoLuotChinh, &c);",
        T*4 + "ResumeThread(g_hDoLuotChinh);",
        T*4 + "if (bOk)",
        T*4 + "{",
        T*5 + "const LONG i = InterlockedIncrement(&g_lDoLuotGhi);",
        T*5 + "DoLuotMau& m = g_aDoLuotMau[i & (DOLUOT_MAU - 1)];",
        "#if defined(_M_X64) || defined(_M_AMD64)",
        T*5 + "m.uEip = (DWORD_PTR)c.Rip;",
        "#else",
        T*5 + "m.uEip = (DWORD_PTR)c.Eip;",
        "#endif",
        T*5 + "m.lPha = g_lDoLuotPha; m.lSeq = g_lDoLuotSeq; m.dwLuc = GetTickCount();",
        T*4 + "}",
        T*3 + "}",
        T*2 + "}",
        T*2 + "// gom mau cu hon 200 ms (tick/khung da ket thuc, da danh dau nang hay chua)",
        T*2 + "const LONG lGhi = g_lDoLuotGhi; const DWORD dwNow = GetTickCount();",
        T*2 + "while (lDaGom < lGhi && lGhi - lDaGom < DOLUOT_MAU)",
        T*2 + "{",
        T*3 + "const DoLuotMau& m = g_aDoLuotMau[(lDaGom + 1) & (DOLUOT_MAU - 1)];",
        T*3 + "if ((DWORD)(dwNow - m.dwLuc) < 200) break;",
        T*3 + "lDaGom++;",
        T*3 + "const int p = (m.lPha == 1) ? 0 : ((m.lPha == 2) ? 1 : -1);",
        T*3 + "if (p < 0) continue;",
        T*3 + "if (g_aDoLuotNang[m.lSeq & (DOLUOT_NANG - 1)] != m.lSeq) continue;" + T + "// khong nang",
        T*3 + "if (aLanDem[p][m.lSeq & (DOLUOT_NANG - 1)] != m.lSeq) { aLanDem[p][m.lSeq & (DOLUOT_NANG - 1)] = m.lSeq; uLan[p]++; }",
        T*3 + "uMau[p]++;",
        T*3 + "int k = 0; for (; k < nDem[p]; k++) if (aDem[p][k].uEip == m.uEip) { aDem[p][k].uSo++; break; }",
        T*3 + "if (k == nDem[p] && nDem[p] < 4096) { aDem[p][k].uEip = m.uEip; aDem[p][k].uSo = 1; nDem[p]++; }",
        T*2 + "}",
        T*2 + "if ((DWORD)(dwNow - dwIn) >= 30000)",
        T*2 + "{",
        T*3 + "dwIn = dwNow;",
        T*3 + "FILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        T*3 + "if (pLog)",
        T*3 + "{",
        T*4 + "for (int p = 0; p < 2; p++) if (uMau[p]) DoLuotIn(pLog, p + 1, aDem[p], nDem[p], uMau[p], uLan[p]);",
        T*4 + "fclose(pLog);",
        T*3 + "}",
        T*3 + "nDem[0] = nDem[1] = 0; uMau[0] = uMau[1] = 0; uLan[0] = uLan[1] = 0;",
        T*2 + "}",
        T + "}",
        T + "return 0;",
        "}",
        "typedef DWORD (WINAPI *PFN_DoLuotSymSetOptions)(DWORD);",
        "typedef BOOL  (WINAPI *PFN_DoLuotSymInitialize)(HANDLE, PCSTR, BOOL);",
        "typedef BOOL  (WINAPI *PFN_DoLuotSymFromAddr)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);",
        "static void DoLuotIn(FILE* pLog, int nPha, DoLuotDem* aDem, int nDem, unsigned uMau, unsigned uLan)",
        "{",
        T + "static PFN_DoLuotSymFromAddr s_pfnTu = NULL; static int s_nThu = 0;",
        T + "if (!s_pfnTu && s_nThu == 0)" + T + "// dbghelp: nap 1 lan tren luong nay (khong dung o luong chinh)",
        T + "{",
        T*2 + "s_nThu = 1;",
        T*2 + "HMODULE h = LoadLibraryA(\"dbghelp.dll\");",
        T*2 + "if (h)",
        T*2 + "{",
        T*3 + "PFN_DoLuotSymSetOptions pOpt = (PFN_DoLuotSymSetOptions)GetProcAddress(h, \"SymSetOptions\");",
        T*3 + "PFN_DoLuotSymInitialize pInit = (PFN_DoLuotSymInitialize)GetProcAddress(h, \"SymInitialize\");",
        T*3 + "if (pOpt) pOpt(0x00000002 | 0x00000004);" + T + "// SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS",
        T*3 + "if (pInit && pInit(GetCurrentProcess(), NULL, TRUE))",
        T*4 + "s_pfnTu = (PFN_DoLuotSymFromAddr)GetProcAddress(h, \"SymFromAddr\");",
        T*2 + "}",
        T + "}",
        T + "// sap xep giam dan theo so mau (chon dan 12)",
        T + "fprintf(pLog, \"[DOLUOT] t=%u pha %s: %u lan nang, %u mau:\", (unsigned)GetTickCount(), nPha == 1 ? \"TICK\" : \"VE\", uLan, uMau);",
        T + "for (int r = 0; r < 12 && r < nDem; r++)",
        T + "{",
        T*2 + "int nMax = r;",
        T*2 + "for (int k = r + 1; k < nDem; k++) if (aDem[k].uSo > aDem[nMax].uSo) nMax = k;",
        T*2 + "if (nMax != r) { DoLuotDem t = aDem[r]; aDem[r] = aDem[nMax]; aDem[nMax] = t; }",
        T*2 + "char szMod[MAX_PATH] = \"?\"; DWORD_PTR uRva = aDem[r].uEip; HMODULE hMod = NULL;",
        T*2 + "if (GetModuleHandleExA(0x00000004 | 0x00000002, (LPCSTR)aDem[r].uEip, &hMod) && hMod)" + T + "// FROM_ADDRESS | UNCHANGED_REFCOUNT",
        T*2 + "{",
        T*3 + "char szDuong[MAX_PATH]; if (GetModuleFileNameA(hMod, szDuong, MAX_PATH)) { const char* p = strrchr(szDuong, '\\\\'); strncpy(szMod, p ? p + 1 : szDuong, MAX_PATH - 1); szMod[MAX_PATH - 1] = 0; }",
        T*3 + "uRva = aDem[r].uEip - (DWORD_PTR)hMod;",
        T*2 + "}",
        T*2 + "char szTen[256] = \"\"; DWORD64 uLech = 0;",
        T*2 + "if (s_pfnTu)",
        T*2 + "{",
        T*3 + "char aBuf[sizeof(SYMBOL_INFO) + 200]; SYMBOL_INFO* pSym = (SYMBOL_INFO*)aBuf; memset(aBuf, 0, sizeof(aBuf));",
        T*3 + "pSym->SizeOfStruct = sizeof(SYMBOL_INFO); pSym->MaxNameLen = 199;",
        T*3 + "if (s_pfnTu(GetCurrentProcess(), (DWORD64)aDem[r].uEip, &uLech, pSym)) { strncpy(szTen, pSym->Name, 255); szTen[255] = 0; }",
        T*2 + "}",
        T*2 + "fprintf(pLog, \" | %.1f%% %s+%X %s+%u\", aDem[r].uSo * 100.0 / uMau, szMod, (unsigned)uRva, szTen[0] ? szTen : \"?\", (unsigned)uLech);",
        T + "}",
        T + "fprintf(pLog, \"\\n\");",
        "}",
        "// Luong chinh: RAII quanh tick (KSubWorld::Activate) va ve (KSubWorld::Paint)",
        "struct DoLuotPham",
        "{",
        T + "LARGE_INTEGER m_li0; int m_nPha; LONG m_lSeq;",
        T + "DoLuotPham(int nPha) : m_nPha(0), m_lSeq(0)",
        T + "{",
        T*2 + "if (g_nDoLuot < 0)",
        T*2 + "{",
        T*3 + "g_nDoLuot = GetPrivateProfileIntA(\"Client\", \"DoLuot\", 0, \".\\\\config.ini\") ? 1 : 0;",
        T*3 + "if (g_nDoLuot)",
        T*3 + "{",
        T*4 + "if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_hDoLuotChinh, THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, 0))",
        T*5 + "g_nDoLuot = 0;",
        T*4 + "else",
        T*4 + "{",
        T*5 + "unsigned uTid = 0; HANDLE h = (HANDLE)_beginthreadex(NULL, 0, DoLuotLuong, NULL, 0, &uTid);",
        T*5 + "if (!h) g_nDoLuot = 0; else CloseHandle(h);",
        T*4 + "}",
        T*3 + "}",
        T*2 + "}",
        T*2 + "if (!g_nDoLuot) return;",
        T*2 + "m_nPha = nPha; m_lSeq = InterlockedIncrement(&g_lDoLuotSeq);",
        T*2 + "QueryPerformanceCounter(&m_li0);",
        T*2 + "InterlockedExchange(&g_lDoLuotPha, (LONG)nPha);",
        T + "}",
        T + "~DoLuotPham()",
        T + "{",
        T*2 + "if (!m_nPha) return;",
        T*2 + "InterlockedExchange(&g_lDoLuotPha, 0);",
        T*2 + "LARGE_INTEGER li1, f; QueryPerformanceCounter(&li1); QueryPerformanceFrequency(&f);",
        T*2 + "const double dMs = (double)(li1.QuadPart - m_li0.QuadPart) * 1000.0 / (double)f.QuadPart;",
        T*2 + "if (dMs >= 20.0) { g_aDoLuotNangPha[m_lSeq & (DOLUOT_NANG - 1)] = m_nPha; InterlockedExchange(&g_aDoLuotNang[m_lSeq & (DOLUOT_NANG - 1)], m_lSeq); }",
        T + "}",
        "};",
        "",
        old,
    ])
    s = rep(s, old, new, "SW bo lay mau")
    # moc tick: sau SetCurrentTime trong Activate
    old = T + "g_ScenePlace.SetCurrentTime(m_dwCurrentTime);" + NL + T + "LARGE_INTEGER liW0, liW1, liW2;" + T + "// [WORLD 08/09 a]"
    new = T + "g_ScenePlace.SetCurrentTime(m_dwCurrentTime);" + NL + T + "DoLuotPham doLuotTick(1);" + T + "// " + TAG_D + " tick the gioi" + NL + T + "LARGE_INTEGER liW0, liW1, liW2;" + T + "// [WORLD 08/09 a]"
    s = rep(s, old, new, "SW moc tick")
    old = "void KSubWorld::Paint()" + NL + "{" + NL + T + "if(m_uPaintTime > timeGetTime())"
    new = "void KSubWorld::Paint()" + NL + "{" + NL + T + "DoLuotPham doLuotVe(2);" + T + "// " + TAG_D + " ve the gioi" + NL + T + "if(m_uPaintTime > timeGetTime())"
    s = rep(s, old, new, "SW moc ve")
    ghi(F_SW, s, c0, l0, h0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")
print("XONG VUNG + DOLUOT")
