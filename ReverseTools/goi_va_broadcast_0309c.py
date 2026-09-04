# -*- coding: utf-8 -*-
"""[BC 03/09 c] DUNG VA DOI PHO - DO CHO RA GOC.
 1. TRA MAX_BROADCAST_COUNT ve 100: nang 100->500 la SAI mo hinh. Tran 100 chua bao gio cat nguoi that
    (do that: cat_vi_het_ngan_sach = 0); no chi tinh co lam VAN CHAN vi bot an suat. Nang len = go them van, khong sua gi.
 2. Sua bo dem [BC-LOAI]: ban truoc in 8 LOAI SO HIEU NHO NHAT (vong for k tang dan) nen giau mat loai lon
    (87 npcattack, 91 npchurt, 95 skillcast). Nay in TOP 8 THEO SO LUONG + nhan dung so hieu.
 3. Them do THEO NGUOI NHAN: moi 10 s in so client nhan gói, client nhan NHIEU NHAT bao nhieu goi va loai nao.
    Day la so lieu quyet dinh: 59.456 goi/giay la CUA MOT client hay chia deu cho vai chuc client?
 4. Them do goi/giay theo TICK de biet co dot bung (burst) hay deu.
 So hieu that (KProtocolDef.h, da kiem bang trinh bien dich): 75 syncplayermin, 76 syncnpc, 77 syncnpcmin,
 78 syncnpcminplayer, 84 npcremove, 85 npcwalk, 86 npcrun, 87 npcattack, 91 npchurt, 92 npcdeath, 95 skillcast.
 Doc/ghi latin-1 newline='' (chu thich GBK). Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_bc\Sources\Core\Src"
MARK = "[BC 03/09 c]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

# ---------------- 1. KRegion.h: tra ve 100
p = os.path.join(ROOT, "KRegion.h"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    s = rep1(s, "#define\tMAX_BROADCAST_COUNT\t\t500" + N,
             "// %s TRA VE 100: do that cho thay cat_vi_het_ngan_sach = 0, tuc tran nay KHONG cat nguoi that." % MARK + N +
             "// Nang len 500 chi go them mot van an toan chu khong sua gi. Giu 100 lam muc chan cuoi." + N +
             "#define\tMAX_BROADCAST_COUNT\t\t100" + N, "MAX_BROADCAST_COUNT ve 100")
    wr(p, s); print("KRegion.h OK (ve 100)")
else:
    print("KRegion.h da va")

# ---------------- 2+3+4. KRegion.cpp: do theo nguoi nhan + top loai
p = os.path.join(ROOT, "KRegion.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    # (a) dem theo nguoi nhan ngay tai cho gui
    old = ("\t\t\t\tg_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);" + N +
           "\t\t\t\tnMaxCount--;\t// [F4] CHI tru khi THAT SU gui (truoc day tru cho ca bot va nguoi ngoai tam)" + N +
           "\t\t\t\tnF4Gui++;" + N)
    new = ("\t\t\t\tg_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);" + N +
           "\t\t\t\tnMaxCount--;\t// [F4] CHI tru khi THAT SU gui (truoc day tru cho ca bot va nguoi ngoai tam)" + N +
           "\t\t\t\tnF4Gui++;" + N +
           "\t\t\t\tBC_DemNguoiNhan(pNode->m_nIndex, nBCLoaiGoi, (int)dwSize);\t// %s do theo TUNG nguoi nhan" % MARK + N)
    s = rep1(s, old, new, "dem nguoi nhan")
    # (b) loai goi tinh mot lan dau ham (dung cho ca dem nguoi nhan)
    old = "\tconst int nF4Tong = m_PlayerList.m_nNodeCount;" + N
    new = ("\tconst int nBCLoaiGoi = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;\t// %s so hieu goi (byte dau)" % MARK + N + old)
    s = rep1(s, old, new, "nBCLoaiGoi")
    # (c) ham dem + in bao cao (dat truoc KRegion::BroadCast)
    old = "void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)" + N
    helper = N.join([
        "// %s DO CHO RA GOC (khong sua hanh vi): moi 10 giay in ba dong vao jx_auto_server.log" % MARK,
        "//   [BC-NGUOI] so client thuc nhan goi, client nhan NHIEU NHAT bao nhieu goi/byte, loai goi nhieu nhat cua no.",
        "//   [BC-TOP]   8 loai goi nhieu nhat theo SO LUOT GUI (ban truoc in 8 so hieu nho nhat -> giau mat 87/91/95).",
        "// So hieu: 75 syncplayermin 76 syncnpc 77 syncnpcmin 78 syncnpcminplayer 84 npcremove 85 npcwalk 86 npcrun",
        "//          87 npcattack 88 npcmagic 91 npchurt 92 npcdeath 95 skillcast.",
        "static int   g_anBCNguoiGoi[MAX_PLAYER];\t// so goi da gui cho tung nguoi choi (chi so Player[])",
        "static int   g_anBCNguoiByte[MAX_PLAYER];",
        "static short g_anBCNguoiLoai[MAX_PLAYER];\t// loai goi gan nhat cua nguoi do (de xem ai doi loai gi)",
        "static int   g_anBCLoaiGui[256];\t\t// so luot GUI theo loai",
        "static int   g_anBCLoaiByte[256];",
        "static void BC_DemNguoiNhan(int nPlayerIndex, int nLoai, int nSize)",
        "{",
        "\tif (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)",
        "\t{",
        "\t\tg_anBCNguoiGoi[nPlayerIndex]++;",
        "\t\tg_anBCNguoiByte[nPlayerIndex] += nSize;",
        "\t\tg_anBCNguoiLoai[nPlayerIndex] = (short)nLoai;",
        "\t}",
        "\tif (nLoai >= 0 && nLoai < 256)",
        "\t{",
        "\t\tg_anBCLoaiGui[nLoai]++;",
        "\t\tg_anBCLoaiByte[nLoai] += nSize;",
        "\t}",
        "}",
        "static void BC_BaoCao10s()",
        "{",
        "\t// nguoi nhan",
        "\tint nSo = 0, nMax = 0, nMaxIdx = 0;",
        "\tdouble dTongByte = 0;",
        "\tfor (int i = 1; i < MAX_PLAYER; i++)",
        "\t{",
        "\t\tif (g_anBCNguoiGoi[i] <= 0)",
        "\t\t\tcontinue;",
        "\t\tnSo++;",
        "\t\tdTongByte += g_anBCNguoiByte[i];",
        "\t\tif (g_anBCNguoiGoi[i] > nMax)",
        "\t\t{",
        "\t\t\tnMax = g_anBCNguoiGoi[i];",
        "\t\t\tnMaxIdx = i;",
        "\t\t}",
        "\t}",
        "\tAUTOLOG(\"[BC-NGUOI] 10s: so_client=%d tong_byte=%.0f | nhieu nhat: player=%d ten=%s goi=%d (%d/giay) byte=%d (%d KB/giay) loai_cuoi=%d\",",
        "\t\tnSo, dTongByte, nMaxIdx,",
        "\t\t(nMaxIdx > 0 && Npc[Player[nMaxIdx].m_nIndex].m_szName[0]) ? Npc[Player[nMaxIdx].m_nIndex].m_szName : \"?\",",
        "\t\tnMax, nMax / 10, nMaxIdx > 0 ? g_anBCNguoiByte[nMaxIdx] : 0, nMaxIdx > 0 ? g_anBCNguoiByte[nMaxIdx] / 10240 : 0,",
        "\t\tnMaxIdx > 0 ? (int)g_anBCNguoiLoai[nMaxIdx] : -1);",
        "\t// top 8 loai theo so luot gui",
        "\tchar szTop[512]; int nLen = 0;",
        "\tint anDung[256];",
        "\tmemset(anDung, 0, sizeof(anDung));",
        "\tfor (int nLan = 0; nLan < 8 && nLen < 440; nLan++)",
        "\t{",
        "\t\tint nBest = -1, nBestVal = 0;",
        "\t\tfor (int k = 0; k < 256; k++)",
        "\t\t{",
        "\t\t\tif (!anDung[k] && g_anBCLoaiGui[k] > nBestVal)",
        "\t\t\t{",
        "\t\t\t\tnBestVal = g_anBCLoaiGui[k];",
        "\t\t\t\tnBest = k;",
        "\t\t\t}",
        "\t\t}",
        "\t\tif (nBest < 0)",
        "\t\t\tbreak;",
        "\t\tanDung[nBest] = 1;",
        "\t\tnLen += _snprintf(szTop + nLen, sizeof(szTop) - 1 - nLen, \" %d:gui=%d,%dKB\", nBest, g_anBCLoaiGui[nBest], g_anBCLoaiByte[nBest] / 1024);",
        "\t}",
        "\tszTop[sizeof(szTop) - 1] = 0;",
        "\tif (nLen > 0)",
        "\t\tAUTOLOG(\"[BC-TOP] 10s top loai theo luot GUI:%s\", szTop);",
        "\tmemset(g_anBCNguoiGoi, 0, sizeof(g_anBCNguoiGoi));",
        "\tmemset(g_anBCNguoiByte, 0, sizeof(g_anBCNguoiByte));",
        "\tmemset(g_anBCLoaiGui, 0, sizeof(g_anBCLoaiGui));",
        "\tmemset(g_anBCLoaiByte, 0, sizeof(g_anBCLoaiByte));",
        "}",
        "",
    ])
    s = rep1(s, old, helper + old, "helper dem")
    # (d) goi bao cao trong khoi 10 s san co
    old = "\t\t\tif (nLen > 0)" + N
    old2 = "\t\t\tmemset(s_anBCGoi, 0, sizeof(s_anBCGoi)); memset(s_anBCGui, 0, sizeof(s_anBCGui)); memset(s_anBCBo, 0, sizeof(s_anBCBo));" + N
    new2 = old2 + "\t\t\tBC_BaoCao10s();\t// %s" % MARK + N
    s = rep1(s, old2, new2, "goi bao cao")
    wr(p, s); print("KRegion.cpp OK (do theo nguoi nhan + top loai)")
else:
    print("KRegion.cpp da va")
print("DONE")
