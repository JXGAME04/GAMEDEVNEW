# -*- coding: utf-8 -*-
# [S12b 28/08] Va 3 mieng sau nghiem thu S12 TRUOT (BANGIAO_DICHUYEN_GIATLUI_2508.md muc 9.27):
#   V1a: mo cua so S12-THEO ngay tai nhanh dat-lai-mo-coi cua self-sync (lenh dat toi cung
#        mili-giay voi cu ha canh - do that 27+28/08, cua so chi mo o S8-NAN la qua muon)
#   V1b: gac echo so THEM DICH - lenh self co dich lech >64 mps so dich minh vua TU GUI
#        (SendClientCmdRun/Walk) chac chan KHONG phai echo -> cho qua du auto dang bat
#   V2 : S6-VANH: (a) GetMapDis == VOID_DIS (self chua co region) -> KHONG go (dong quy uoc
#        S6_XaQuaTam "nhan het"); (b) nguong go 40->42 tao dai tre 2 o so nguong nhan-lai 40
#   V3 : [S12-TELE] g_DebugLog (chi ra DebugWin, khong file) -> AUTOLOG (jx_auto_server.log)
# Chay lai duoc nhieu lan (idempotent). Ap SAU chuoi: goi_va_S10_dichthat -> S10_ma ->
# S11_chongma -> S12_bung8o.
import io, sys
D = r"D:\GAMEDEVNEW\Sources\Core\Src"

def sua(path, hunks):
    p = D + "\\" + path
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    truoc = sum(1 for c in s if ord(c) > 127)
    n = 0
    crlf = "\r\n" in s
    for ten, cu, moi in hunks:
        if crlf:
            cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
        if moi in s:
            print("  [=] %s: %s da ap tu truoc" % (path, ten)); continue
        if cu not in s:
            print("LOI: %s: khong tim thay neo cho %s" % (path, ten)); sys.exit(1)
        if s.count(cu) != 1:
            print("LOI: %s: neo %s khop %d cho (can 1)" % (path, ten, s.count(cu))); sys.exit(1)
        s = s.replace(cu, moi); n += 1
        print("  [+] %s: %s" % (path, ten))
    sau = sum(1 for c in s if ord(c) > 127)
    if truoc != sau:
        print("LOI: %s: high-byte doi %d -> %d" % (path, truoc, sau)); sys.exit(1)
    if n:
        io.open(p, "w", encoding="latin-1", newline="").write(s)
    return n

tong = 0

# ============ KProtocolProcess.cpp ============
tong += sua("KProtocolProcess.cpp", [
("H1 bien dich-tu-gui",
 'DWORD g_uS12CuaSoSelf = 0;\n',
 'DWORD g_uS12CuaSoSelf = 0;\n'
 '// [S12b 28/08] dich move CHINH MINH vua tu gui (ghi tai SendClientCmdRun/Walk,\n'
 '// KProtocol.cpp): dung phan biet echo (dich trung) voi lenh DAT-DI cua server (dich la).\n'
 'int g_nS12TuGuiX = 0, g_nS12TuGuiY = 0;\n'),

("H2 chu ky + bypass dich-la",
 'static BOOL S12_ChoPhepSelf(int nIdx)\n'
 '{\n'
 '\tif (nIdx <= 0 || nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)\n'
 '\t\treturn FALSE;\n'
 '\tif (g_uS12CuaSoSelf == 0 || (DWORD)(timeGetTime() - g_uS12CuaSoSelf) >= 3000)\n'
 '\t\treturn FALSE;\n',
 'static BOOL S12_ChoPhepSelf(int nIdx, int nDichX, int nDichY)\n'
 '{\n'
 '\tif (nIdx <= 0 || nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)\n'
 '\t\treturn FALSE;\n'
 '\tif (g_uS12CuaSoSelf == 0 || (DWORD)(timeGetTime() - g_uS12CuaSoSelf) >= 3000)\n'
 '\t\treturn FALSE;\n'
 '\t// [S12b 28/08] Dich lech >64 mps (2 o) so dich minh vua TU GUI = chac chan KHONG phai\n'
 '\t// echo cua minh -> cho ap luon, khong xet 2 gac duoi (do that 27/08: auto bat lam\n'
 '\t// HaveTarget/SendMoveFrames chan sach ca lenh dat that; 42 lenh echo that thi van bi\n'
 '\t// chan dung nho nhanh trung-dich roi xuong gac cu).\n'
 '\tif (g_nS12TuGuiX != 0 || g_nS12TuGuiY != 0)\n'
 '\t{\n'
 '\t\tint nS12Lx = nDichX - g_nS12TuGuiX; if (nS12Lx < 0) nS12Lx = -nS12Lx;\n'
 '\t\tint nS12Ly = nDichY - g_nS12TuGuiY; if (nS12Ly < 0) nS12Ly = -nS12Ly;\n'
 '\t\tif (nS12Lx > 64 || nS12Ly > 64)\n'
 '\t\t\treturn TRUE;\n'
 '\t}\n'),

("H3a callsite run",
 '\tbS12Self = S12_ChoPhepSelf(nIdx);\n'
 '#endif\n'
 '\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)\n'
 '\t{\n'
 '\t\tAUTOLOG_EVERY(300, "NET-RUN',
 '\tbS12Self = S12_ChoPhepSelf(nIdx, (int)MapX, (int)MapY);\n'
 '#endif\n'
 '\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)\n'
 '\t{\n'
 '\t\tAUTOLOG_EVERY(300, "NET-RUN'),

("H3b callsite walk",
 '\tbS12Self = S12_ChoPhepSelf(nIdx);\n'
 '#endif\n'
 '\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)\n'
 '\t{\n'
 '\t\tAUTOLOG_EVERY(300, "NET-WALK',
 '\tbS12Self = S12_ChoPhepSelf(nIdx, (int)MapX, (int)MapY);\n'
 '#endif\n'
 '\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)\n'
 '\t{\n'
 '\t\tAUTOLOG_EVERY(300, "NET-WALK'),

("H4 mo cua so tai dat-lai-mo-coi",
 '\t\tmemset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));\n'
 '\t\tNpc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;\n'
 '\t\treturn;\n',
 '\t\tmemset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));\n'
 '\t\tNpc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;\n'
 '#ifndef _SERVER\n'
 '\t\t// [S12b 28/08] Vua duoc DAT LAI tu mo coi = server vua dich chuyen minh (teleport/\n'
 '\t\t// vao map). Do that 27+28/08: lenh DAT-DI cua script toi CUNG MILI-GIAY voi cu sync\n'
 '\t\t// nay; cua so truoc day chi mo o nhanh S8-NAN (486ms sau) nen lenh bi vut -> thang\n'
 '\t\t// bung 8 o moi ~0,6s (11 cu / 5 cu). Mo cua so ngay tai day; gac echo giu nguyen.\n'
 '\t\tg_uS12CuaSoSelf = timeGetTime();\n'
 '\t\tAUTOLOG("[S12-CUA] mo cua so theo-lenh tai dat-lai sv=(%d,%d) t=%u", (int)pSync->m_dwMapX, (int)pSync->m_dwMapY, SubWorld[0].m_dwCurrentTime);\n'
 '#endif\n'
 '\t\treturn;\n'),
])

# ============ KProtocol.cpp ============
tong += sua("KProtocol.cpp", [
("H5 ghi dich tu gui run",
 'void SendClientCmdRun(int nX, int nY)\n'
 '{\n'
 '\tNPC_RUN_COMMAND\tNetCommand;\n'
 '\t\n'
 '\tNetCommand.ProtocolType = (BYTE)c2s_npcrun;\n',
 '// [S12b 28/08] xem KProtocolProcess.cpp: phan biet echo voi lenh dat-di cua server.\n'
 'extern int g_nS12TuGuiX, g_nS12TuGuiY;\n'
 'void SendClientCmdRun(int nX, int nY)\n'
 '{\n'
 '\tNPC_RUN_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY;\n'
 '\tNetCommand.ProtocolType = (BYTE)c2s_npcrun;\n'),
("H5b ghi dich tu gui walk",
 'void SendClientCmdWalk(int nX, int nY)\n'
 '{\n'
 '\tNPC_WALK_COMMAND\tNetCommand;\n'
 '\t\n'
 '\tNetCommand.ProtocolType = (BYTE)c2s_npcwalk;\n',
 'void SendClientCmdWalk(int nX, int nY)\n'
 '{\n'
 '\tNPC_WALK_COMMAND\tNetCommand;\n'
 '\tg_nS12TuGuiX = nX; g_nS12TuGuiY = nY;\n'
 '\tNetCommand.ProtocolType = (BYTE)c2s_npcwalk;\n'),
])

# ============ KNpc.cpp ============
tong += sua("KNpc.cpp", [
("H6 VANH: VOID_DIS + dai tre 2 o",
 '\t#define\tMAX_SYNC_RANGE\t40\n'
 '\tif (!IsPlayer() && (GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= MAX_SYNC_RANGE\n'
 '\t\t|| GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= MAX_SYNC_RANGE))\n',
 '\t#define\tMAX_SYNC_RANGE\t40\n'
 '\t// [S12b 28/08] (a) GetMapDis tra VOID_DIS (0x7FFFFFFF) khi CHINH MINH chua duoc dat vao\n'
 '\t// region (vua LoadMap/teleport) - truoc day VOID_DIS >= 40 nen go SACH moi NPC moi frame\n'
 '\t// trong khi S6_XaQuaTam cung dieu kien lai "nhan het" -> flap go-gan ~9Hz (do 27+28/08:\n'
 '\t// 179-208 chu ky <500ms/phien). Self chua dat -> DUNG go, dong quy uoc voi S6_XaQuaTam.\n'
 '\t// (b) Nguong go +2 o so nguong nhan-lai 40 cua S6_XaQuaTam: hai thuoc do (o-nguyen vs\n'
 '\t// mps, per-truc) lech nhau <1 o tai bien -> NPC nam dung vanh 40 bi go-gan moi tick.\n'
 '\tint nS6VDisX = GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex);\n'
 '\tint nS6VDisY = GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex);\n'
 '\tif (!IsPlayer() && nS6VDisX != 0x7FFFFFFF && nS6VDisY != 0x7FFFFFFF\n'
 '\t\t&& (nS6VDisX >= MAX_SYNC_RANGE + 2 || nS6VDisY >= MAX_SYNC_RANGE + 2))\n'),

("H7 S12-TELE ra AUTOLOG",
 '\t\tg_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sTeleSync, sizeof(sTeleSync));\n'
 '\t\tg_DebugLog("[S12-TELE]%d:%s setpos cung map -> bao chinh chu (%d,%d)", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, nTeleX, nTeleY);\n',
 '\t\tg_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sTeleSync, sizeof(sTeleSync));\n'
 '\t\t// [S12b 28/08] g_DebugLog chi ban WM_COPYDATA sang DebugWin (khong ghi file) ->\n'
 '\t\t// khong the nghiem thu. Doi sang AUTOLOG de dem duoc trong jx_auto_server.log.\n'
 '\t\tAUTOLOG("[S12-TELE] %s setpos cung map -> bao chinh chu (%d,%d) t=%u", Name, nTeleX, nTeleY, SubWorld[m_SubWorldIndex].m_dwCurrentTime);\n'),
])

print("Tong hunk ap: %d" % tong)
