# -*- coding: ascii -*-
# S10 (26/08 dem) - "SERVER GUI DICH THAT" - chu duyet sau phan bien doi khang 3 tac nhan.
# M1 (server): SendSyncData gui kem s2c_npcrun/walk mang m_DesX/m_DesY that.
# M2 (client): XOA khoi doan dich S9-DICH/S9-LUI (nguon sinh "quay dau").
# M3 (client): luoi hep S10-KEO/S10-GAC/S10-SNAP - chi tac dong ban sao DANG DUNG.
import io, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
CRLF = "\r\n"
def L(*a): return CRLF.join(a)
ok = True

def doc(f):
    return io.open(ROOT + "\\" + f, "r", encoding="latin-1", newline="").read()

def ghi(f, c, hb):
    if sum(1 for ch in c if ord(ch) > 127) != hb:
        print("LOI %s: high-byte doi" % f); return False
    io.open(ROOT + "\\" + f, "w", encoding="latin-1", newline="").write(c)
    return True

# ============ M1: KNpc.cpp - gui dich that cuoi SendSyncData ============
c = doc("KNpc.cpp"); hb = sum(1 for ch in c if ord(ch) > 127)
old = L('\t\t\tprintf("Packing player sync data failed...\\n");',
        "\t\t\treturn FALSE;",
        "\t\t}",
        "\t}",
        "\treturn bRet;")
new = L('\t\t\tprintf("Packing player sync data failed...\\n");',
        "\t\t\treturn FALSE;",
        "\t\t}",
        "\t}",
        "\t// [S10-M1 26/08] TRAM LO HONG GOC (co tu ban 2003, comment 'need check later -- spe",
        "\t// 03/05/27' o KProtocolProcess.cpp:1987): goi NPC_SYNC (ADD) khong mang dich den, nen",
        "\t// client thay NPC dang chay giua chang la tu che lenh 'chay toi cho dang dung' - ban",
        "\t// sao mu dich cho toi chang ke tiep (do that: 10% khoang cho > 2,2s; ban sao tu quay",
        "\t// dau gap 6,1 lan server). Gui kem MOT goi lenh CO SAN (s2c_npcrun/s2c_npcwalk, 13",
        "\t// byte, client co handler tu truoc - khong doi protocol) mang DICH THAT m_DesX/m_DesY",
        "\t// cho rieng client nay. Cung hang doi FIFO per-client (ServerStage.cpp:396+) nen luon",
        "\t// toi SAU goi ADD va de len lenh tu che (khe lenh mot cho - ke ghi sau thang).",
        "\t// GAC SONG COT m_DesX>0: DoSkill muon m_DesX=-1/m_DesY=CHI-SO-KHE lam tham so ma khong",
        "\t// doi m_Doing (KNpc.cpp:2592+2649) - thieu gac la ban sao chay ve goc map.",
        "\t// Khong gui cho chinh chu nhan vat (client cung se vut qua ConformIdx) do sach log.",
        "\t// Ket qua goi phu KHONG duoc dong vao bRet: chuoi login theo buoc (KPlayerDBFuns.cpp:46+)",
        "\t// doc gia tri tra ve cua ham nay de quyet dinh buoc nap nhan vat.",
        "\tif (bRet && (m_Doing == do_run || m_Doing == do_walk) && m_DesX > 0 && m_DesY > 0 &&",
        "\t\t!(IsPlayer() && Player[m_nPlayerIdx].m_nNetConnectIdx == nClient))",
        "\t{",
        "\t\tNPC_RUN_SYNC RunCmd;",
        "\t\tRunCmd.ProtocolType = (BYTE)((m_Doing == do_walk) ? s2c_npcwalk : s2c_npcrun);",
        "\t\tRunCmd.ID = m_dwID;",
        "\t\tRunCmd.nMpsX = m_DesX;",
        "\t\tRunCmd.nMpsY = m_DesY;",
        "\t\tg_pServer->PackDataToClient(nClient, (BYTE*)&RunCmd, sizeof(RunCmd));",
        "\t\tg_DebugLog(\"[S10-M1]%d:%s goi dich that (%d,%d) doing=%d cho client %d\", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, m_DesX, m_DesY, (int)m_Doing, nClient);",
        "\t}",
        "\treturn bRet;")
n = c.count(old)
if n != 1:
    print("LOI M1: neo khop %d lan" % n); ok = False
else:
    c = c.replace(old, new)
    if ghi("KNpc.cpp", c, hb): print("OK M1 (server gui dich that trong SendSyncData)")
    else: ok = False

# ============ M2+M3: KProtocolProcess.cpp - thay khoi S9 bang khoi S10 ============
c = doc("KProtocolProcess.cpp"); hb = sum(1 for ch in c if ord(ch) > 127)
# ranh dau: '#ifndef _SERVER' ngay truoc chu thich [FIX-2 26/08]
start_marker = "#ifndef _SERVER" + CRLF + "\t\t// [FIX-2 26/08] GOC cua 'truot toi - lui'"
# ranh cuoi: '#endif' + CRLF ngay truoc nhanh nan m_nNeedFixPos
end_marker = "#endif" + CRLF + "\t\tif (Npc[nIdx].m_nNeedFixPos > 0 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)"
i = c.find(start_marker)
j = c.find(end_marker)
if i < 0 or j < 0 or j < i:
    print("LOI M2: khong tim thay ranh khoi S9 (i=%d j=%d)" % (i, j)); ok = False
else:
    khoi_cu = c[i:j + len("#endif" + CRLF)]
    # kiem tra khoi cu dung la khoi S9 (chua du 3 dau hieu, khong lem sang ma khac)
    if ("[S9-DICH]" not in khoi_cu) or ("[S9-LUI]" not in khoi_cu) or ("s_uS9CuID" not in khoi_cu):
        print("LOI M2: khoi cat ra khong chua du dau hieu S9"); ok = False
    elif "m_nNeedFixPos > 0" in khoi_cu or "g_nS9SvMeX" in khoi_cu or "S8-NAN" in khoi_cu:
        print("LOI M2: khoi cat ra LEM sang ma phai giu"); ok = False
    else:
        khoi_moi = L(
"#ifndef _SERVER",
"\t\t// [S10 26/08] Khoi doan dich S9 cu (r1/r2/r3) DA XOA - no doan dich tu vi tri",
"\t\t// server cu 0,3-1s nen chinh no sinh 'quay dau' (do that: ban sao tu dao chieu",
"\t\t// gap 6,1 lan server tren cung NPC). Tu nay DICH THAT do server gui: moi chang",
"\t\t// qua broadcast s2c_npcrun/walk (KNpc::DoRun:2341), va ngay luc ADD qua mieng",
"\t\t// [S10-M1] cuoi KNpc::SendSyncData. O day chi con LUOI AN TOAN HEP cho ban sao",
"\t\t// DANG DUNG - dang dung thi khong co huong nen KHONG THE quay dau:",
"\t\t// 1) [S10-KEO] ban sao dung ma server bao dang chay, lech >= 2 o (lenh chang bi",
"\t\t//    nuot/mat - ngan sach broadcast 100 nguoi/luot trong dam dong): cho chay bu",
"\t\t//    toi vi tri server. Can tren 12 o (xa hon la dich chuyen that - cam phat",
"\t\t//    lenh chay xuyen ban do). Gac khe lenh: m_Command dang co gi thi NHUONG,",
"\t\t//    de khong de len DICH THAT vua toi cung dot goi (ProcCommand tick sau moi",
"\t\t//    thi hanh nen m_Doing chua kip doi - race mot-khe co that).",
"\t\t// 2) [S10-SNAP] ca hai ben cung dung ma lech >= 2 o (diem dung cuoi bi lech do",
"\t\t//    mat lenh chang cuoi - khong co nhanh nay thi lech ton tai VINH VIEN vi",
"\t\t//    nhanh nan duoi chi chay khi m_nNeedFixPos>0): ghi de toa do theo dung",
"\t\t//    khuon nhanh nan. Hai ben cung dung => vo hinh voi nguoi choi.",
"\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&",
"\t\t\tNpc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion &&",
"\t\t\tNpc[nIdx].m_Doing == do_stand)",
"\t\t{",
"\t\t\tint nS10X = 0, nS10Y = 0;",
"\t\t\tNpc[nIdx].GetMpsPos(&nS10X, &nS10Y);",
"\t\t\tint nS10D = g_GetDistance(nS10X, nS10Y, NpcSync->MapX, NpcSync->MapY);",
"\t\t\tif (nS10D >= 64)",
"\t\t\t{",
"\t\t\t\tif ((NpcSync->Doing == do_run || NpcSync->Doing == do_walk) && nS10D < 32 * 12)",
"\t\t\t\t{",
"\t\t\t\t\tif (Npc[nIdx].m_Command.CmdKind == do_none)",
"\t\t\t\t\t{",
"\t\t\t\t\t\tAUTOLOG(\"[S10-KEO] npc=%u idx=%d lech=%d doingsv=%d -> chay bu toi (%d,%d) t=%u\", NpcSync->ID, nIdx, nS10D, (int)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\t\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY);",
"\t\t\t\t\t}",
"\t\t\t\t\telse",
"\t\t\t\t\t{",
"\t\t\t\t\t\tAUTOLOG(\"[S10-GAC] npc=%u idx=%d lech=%d khe lenh co cmd=%d -> nhuong dich that t=%u\", NpcSync->ID, nIdx, nS10D, (int)Npc[nIdx].m_Command.CmdKind, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\t\t}",
"\t\t\t\t}",
"\t\t\t\telse if (NpcSync->Doing == do_stand)",
"\t\t\t\t{",
"\t\t\t\t\tAUTOLOG(\"[S10-SNAP] npc=%u idx=%d lech=%d hai ben cung dung -> nan toa do t=%u\", NpcSync->ID, nIdx, nS10D, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\t\tSubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);",
"\t\t\t\t\tNpc[nIdx].m_MapX = nMapX;",
"\t\t\t\t\tNpc[nIdx].m_MapY = nMapY;",
"\t\t\t\t\tNpc[nIdx].m_OffX = NpcSync->m_fkOffX;",
"\t\t\t\t\tNpc[nIdx].m_OffY = NpcSync->m_fkOffY;",
"\t\t\t\t\tSubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);",
"\t\t\t\t\tNpc[nIdx].m_nNeedFixPos = 0;",
"\t\t\t\t}",
"\t\t\t}",
"\t\t}",
"#endif")
        c = c[:i] + khoi_moi + CRLF + c[j + len("#endif" + CRLF):]
        # khoi cu bien mat han?
        if "[S9-DICH]" in c or "[S9-LUI]" in c or "s_uS9CuID" in c:
            print("LOI M2: dau hieu S9 van con sau khi thay"); ok = False
        elif "g_nS9SvMeX" not in c:
            print("LOI M2: g_nS9SvMeX bien mat (CoreShell se khong link duoc)"); ok = False
        elif ghi("KProtocolProcess.cpp", c, hb):
            print("OK M2+M3 (xoa khoi S9 %d dong, thay bang luoi S10)" % khoi_cu.count(CRLF))
        else: ok = False

sys.exit(0 if ok else 1)
