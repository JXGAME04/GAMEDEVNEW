# -*- coding: ascii -*-
# S12 (27/08) - chua "bung 8 o" cua CHINH NGUOI CHOI. Chu duyet; qua phan bien 3 tac nhan.
# Do that: 8/9 cu bung = server TELEPORT (SetPos cung map khong bao chinh client) roi TU DAT
# nguoi choi chay ~1100 mps theo script; lenh dat-di bi client vut (ConformIdx loai self).
# [TELE-server]: SetPos gui ngay goi tu-sync s2c_syncnpcminplayer cho rieng chu nhan vat.
# [THEO-client]: sau moi cu S8-NAN, mo cua so 3000ms cho phep ap lenh run/walk cho CHINH MINH
#   (co gac diet echo: dang co duong click A* hoac vua gui lenh cua minh thi KHONG ap).
# Mieng [KEO] bi HOAN theo phan bien (2 rui ro chet nguoi, chi nham 1/9 cu).
import io, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
CRLF = "\r\n"
def L(*a): return CRLF.join(a)
ok = True

def ap(fname, old, new, note, expect=1):
    global ok
    p = ROOT + "\\" + fname
    c = io.open(p, "r", encoding="latin-1", newline="").read()
    hb = sum(1 for ch in c if ord(ch) > 127)
    n = c.count(old)
    if n != expect:
        print("LOI [%s] %s: neo khop %d lan (can %d)" % (note, fname, n, expect)); ok = False; return
    c = c.replace(old, new)
    if sum(1 for ch in c if ord(ch) > 127) != hb:
        print("LOI [%s] %s: high-byte doi" % (note, fname)); ok = False; return
    io.open(p, "w", encoding="latin-1", newline="").write(c)
    print("OK [%s] %s" % (note, fname))

# ================= TELE: KNpc.cpp cuoi SetPos =================
ap("KNpc.cpp",
L("\tDoStand();",
  "\tm_ProcessAI = 1;",
  "\tm_ProcessState = 1;",
  "\treturn 1;",
  "}"),
L("\tDoStand();",
  "\tm_ProcessAI = 1;",
  "\tm_ProcessState = 1;",
  "\t// [S12-TELE 27/08] SetPos CUNG MAP von KHONG bao cho chinh nguoi choi (chi phat",
  "\t// s2c_npcremove cho nguoi xung quanh) - client mu, chi phat hien qua vong xoay",
  "\t// tu-sync 5 NPC/tick/region (p50 564ms) => chuoi [S8-NAN] bung 4 cu lien tiep sau",
  "\t// moi lan script teleport + dat di (do that 26/08: 8/9 cu bung la loai nay).",
  "\t// Gui NGAY mot goi tu-sync CO SAN (s2c_syncnpcminplayer, 27 byte) cho rieng chu",
  "\t// nhan vat: client snap MOT lan dung luc dich chuyen that (hop mat nguoi choi).",
  "\t// memset bat buoc: khuon goc de m_byDoing/MapID/m_nEquipCount la rac stack.",
  "\t// Gac m_nPlayerIdx > 0: bot SimCity co m_nPlayerIdx = 0; bot thuong co",
  "\t// m_nNetConnectIdx = -1 (PackDataToClient tu chan nhung khoi ton call).",
  "\tif (IsPlayer() && !m_btSimCityBot && m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER &&",
  "\t\tPlayer[m_nPlayerIdx].m_nNetConnectIdx >= 0)",
  "\t{",
  "\t\tNPC_PLAYER_TYPE_NORMAL_SYNC sTeleSync;",
  "\t\tmemset(&sTeleSync, 0, sizeof(sTeleSync));",
  "\t\tsTeleSync.ProtocolType = s2c_syncnpcminplayer;",
  "\t\tsTeleSync.m_dwNpcID = m_dwID;",
  "\t\tint nTeleX = 0, nTeleY = 0;",
  "\t\tGetMpsPos(&nTeleX, &nTeleY);",
  "\t\tsTeleSync.m_dwMapX = nTeleX;",
  "\t\tsTeleSync.m_dwMapY = nTeleY;",
  "\t\tsTeleSync.m_wOffX = m_OffX;",
  "\t\tsTeleSync.m_wOffY = m_OffY;",
  "\t\tg_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sTeleSync, sizeof(sTeleSync));",
  "\t\tg_DebugLog(\"[S12-TELE]%d:%s setpos cung map -> bao chinh chu (%d,%d)\", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, nTeleX, nTeleY);",
  "\t}",
  "\treturn 1;",
  "}"), "TELE SetPos bao chinh chu")

# ================= THEO 1: bien cua so + ham gac (dau file, sau g_nS9SvMeX) =================
ap("KProtocolProcess.cpp",
L("int g_nS9SvMeX = 0;",
  "int g_nS9SvMeY = 0;"),
L("int g_nS9SvMeX = 0;",
  "int g_nS9SvMeY = 0;",
  "",
  "#ifndef _SERVER",
  "// [S12-THEO 27/08] Cua so 'nghe lenh cho CHINH MINH': binh thuong client vut moi lenh",
  "// run/walk server phat cho ban than (ConformIdx loai self) - dung cho choi tay, nhung",
  "// khi SCRIPT teleport + TU DAT nguoi choi di (Tong Kim: ~1100 mps @400 mps/s) thi client",
  "// mu hoan toan => chuoi [S8-NAN] bung 4 cu lien tiep. Sau moi cu S8-NAN mo cua so 3000ms",
  "// cho phep ap lenh self => doan dat-di hien thi thanh chay muot.",
  "// DIET ECHO (phan bien bat buoc): khi nguoi choi DANG tu di (co duong click A* hoac vua",
  "// gui lenh cua minh trong ~278ms) thi lenh self chinh la echo click cua minh doi ve -",
  "// KHONG ap. Dong ho dung timeGetTime (ms, chiu wrap); CAM dung m_dwCurrentTime (la FRAME",
  "// ~18/s va bi gan lai theo server moi SyncWorld).",
  "DWORD g_uS12CuaSoSelf = 0;",
  "static BOOL S12_ChoPhepSelf(int nIdx)",
  "{",
  "\tif (nIdx <= 0 || nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)",
  "\t\treturn FALSE;",
  "\tif (g_uS12CuaSoSelf == 0 || (DWORD)(timeGetTime() - g_uS12CuaSoSelf) >= 3000)",
  "\t\treturn FALSE;",
  "\tint nS12Tx = 0, nS12Ty = 0;",
  "\tif (SubWorld[0].HaveTarget(nS12Tx, nS12Ty))",
  "\t\treturn FALSE;\t// dang co duong click A* cua nguoi choi - lenh self la echo",
  "\tif (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < defMAX_PLAYER_SEND_MOVE_FRAME)",
  "\t\treturn FALSE;\t// vua gui lenh di chuyen cua minh ~278ms truoc - echo",
  "\treturn TRUE;",
  "}",
  "#endif"), "THEO bien + ham gac")

# ================= THEO 2: NetCommandRun ap self =================
ap("KProtocolProcess.cpp",
L("\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))",
  "\t{",
  "\t\tAUTOLOG_EVERY(300, \"NET-RUN npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u\", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);",
  "\t\tNpc[nIdx].SendCommand(do_run, MapX, MapY);",
  "\t\tNpc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;",
  "\t}"),
L("\tBOOL bS12Self = FALSE;",
  "#ifndef _SERVER",
  "\tbS12Self = S12_ChoPhepSelf(nIdx);",
  "#endif",
  "\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)",
  "\t{",
  "\t\tAUTOLOG_EVERY(300, \"NET-RUN npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u\", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);",
  "\t\tNpc[nIdx].SendCommand(do_run, MapX, MapY);",
  "\t\tNpc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;",
  "#ifndef _SERVER",
  "\t\tif (bS12Self)",
  "\t\t{",
  "\t\t\tAUTOLOG(\"[S12-THEO] ap lenh run cho CHINH MINH dich=(%d,%d) t=%u\", (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);",
  "\t\t\tg_uS12CuaSoSelf = timeGetTime();\t// lam tuoi cua so cho lo trinh dai hon 3s",
  "\t\t}",
  "#endif",
  "\t}"), "THEO NetCommandRun")

# ================= THEO 3: NetCommandWalk ap self =================
ap("KProtocolProcess.cpp",
L("\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))",
  "\t{",
  "\t\tAUTOLOG_EVERY(300, \"NET-WALK npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u\", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);",
  "\t\tNpc[nIdx].SendCommand(do_walk, MapX, MapY);",
  "\t\tNpc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;",
  "\t}"),
L("\tBOOL bS12Self = FALSE;",
  "#ifndef _SERVER",
  "\tbS12Self = S12_ChoPhepSelf(nIdx);",
  "#endif",
  "\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)",
  "\t{",
  "\t\tAUTOLOG_EVERY(300, \"NET-WALK npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u\", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);",
  "\t\tNpc[nIdx].SendCommand(do_walk, MapX, MapY);",
  "\t\tNpc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;",
  "#ifndef _SERVER",
  "\t\tif (bS12Self)",
  "\t\t{",
  "\t\t\tAUTOLOG(\"[S12-THEO] ap lenh walk cho CHINH MINH dich=(%d,%d) t=%u\", (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);",
  "\t\t\tg_uS12CuaSoSelf = timeGetTime();",
  "\t\t}",
  "#endif",
  "\t}"), "THEO NetCommandWalk")

# ================= THEO 4: mo cua so tai khoi S8-NAN =================
ap("KProtocolProcess.cpp",
L("\t\t\tAUTOLOG(\"[S8-NAN] lech=%d cl=(%d,%d) sv=(%d,%d) cell cl=(%d,%d) sv=(%d,%d) regcu=%d regmoi=%d doing=%d t=%u\","),
L("\t\t\t// [S12-THEO 27/08] vua bi nan lon = nhieu kha nang server dang dieu khien minh",
  "\t\t\t// (teleport/dat di): mo cua so 3000ms nghe lenh run/walk cho chinh minh.",
  "\t\t\tg_uS12CuaSoSelf = timeGetTime();",
  "\t\t\tAUTOLOG(\"[S8-NAN] lech=%d cl=(%d,%d) sv=(%d,%d) cell cl=(%d,%d) sv=(%d,%d) regcu=%d regmoi=%d doing=%d t=%u\","), "THEO mo cua so o S8-NAN")

# ================= THEO 5: reset cua so khi doi map =================
ap("KProtocolProcess.cpp",
L("\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetProcessAI(TRUE);"),
L("\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetProcessAI(TRUE);",
  "\t\tg_uS12CuaSoSelf = 0;\t// [S12-THEO] cua so khong duoc song sot qua doi map (lenh ton dong se thi hanh muon)"), "THEO reset khi doi map")

sys.exit(0 if ok else 1)
