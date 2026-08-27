# -*- coding: ascii -*-
# S11 (26/08 dem) - chu duyet "oke lam di" sau phan bien 3 tac nhan (bo probe tong quat
# vi 2 rui ro chet nguoi; giu cac mieng song sot + dieu kien bat buoc cua phan bien).
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
    print("OK [%s] %s (x%d)" % (note, fname, expect))

CHECK_EXCL = L(
"\t\tif (Player[nPlayerIdx].m_mAutoExcludeNpcID.find(Npc[nIdx].m_dwID) != Player[nPlayerIdx].m_mAutoExcludeNpcID.end())",
"\t\t\tcontinue;\t// [S11] dang bi loai (khong toi duoc / ma) - het han purge se tha")

# ---------- C1: TK_ChonDich ----------
ap("CoreShell.cpp",
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  "\t\tif (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)",
  "\t\t\tcontinue;",
  "\t\tif (Npc[nIdx].m_Kind == kind_player)"),
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  CHECK_EXCL,
  "\t\tif (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)",
  "\t\t\tcontinue;",
  "\t\tif (Npc[nIdx].m_Kind == kind_player)"), "C1 TK_ChonDich")

# ---------- C2: LD_ChonDich ----------
ap("CoreShell.cpp",
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  "\t\tif (Npc[nIdx].m_Kind != kind_player)",
  "\t\t\tcontinue;\t\t// trong san chi co nguoi choi"),
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  CHECK_EXCL,
  "\t\tif (Npc[nIdx].m_Kind != kind_player)",
  "\t\t\tcontinue;\t\t// trong san chi co nguoi choi"), "C2 LD_ChonDich")

# ---------- C3a: HD_ChonDichDai ----------
ap("CoreShell.cpp",
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  "\t\tif (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)",
  "\t\t\tcontinue;",
  "\t\tNpc[nIdx].GetMpsPos(&x, &y);"),
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  CHECK_EXCL,
  "\t\tif (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)",
  "\t\t\tcontinue;",
  "\t\tNpc[nIdx].GetMpsPos(&x, &y);"), "C3a HD_ChonDichDai")

# ---------- C3b: HD_TimQuai ----------
ap("CoreShell.cpp",
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  "\t\tNpc[nIdx].GetMpsPos(&dX, &dY);"),
L("\t\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)",
  "\t\t\tcontinue;",
  CHECK_EXCL,
  "\t\tNpc[nIdx].GetMpsPos(&dX, &dY);"), "C3b HD_TimQuai")

# ---------- C4: FIX-6 - nang han loai 60s + probe hoi server ----------
ap("CoreShell.cpp",
L("\t\t\t\t\t\t\tAUTOLOG(\"[S9-BOMUCTIEU] tgID=%u d=%d tot nhat=%d qua 4s khong gan them -> loai 30s\", Npc[nTGNpcIdx].m_dwID, nDist, s_nS9ApD);",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_mAutoExcludeNpcID[Npc[nTGNpcIdx].m_dwID] = uCurTime + 30000;",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_sExtAuto.uNpcID = 0;",
  "\t\t\t\t\t\t\ts_uS9ApID = 0;",
  "\t\t\t\t\t\t\treturn 0;"),
L("\t\t\t\t\t\t\tAUTOLOG(\"[S9-BOMUCTIEU] tgID=%u d=%d tot nhat=%d qua 4s khong gan them -> loai 60s + hoi server\", Npc[nTGNpcIdx].m_dwID, nDist, s_nS9ApD);",
  "\t\t\t\t\t\t\t// [S11 26/08] 30s -> 60s: han loai phai VUOT chu ky don rac 55s, khong thi",
  "\t\t\t\t\t\t\t// ma duoc chon lai truoc khi bo don kip hot (phan bien: 34s < 55s).",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_mAutoExcludeNpcID[Npc[nTGNpcIdx].m_dwID] = uCurTime + 60000;",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_sExtAuto.uNpcID = 0;",
  "\t\t\t\t\t\t\t// [S11] hoi server ve con vua loai (khuon REQNPC chuan, tu tiet luu 19 khe:",
  "\t\t\t\t\t\t\t// Insert FALSE = bo im lang). Server con no cung map -> tra vi tri that de",
  "\t\t\t\t\t\t\t// sua ban sao; khac map/da xoa -> tra fail va [S11-XOAMA] go ma ngay.",
  "\t\t\t\t\t\t\t// Tan suat tu gioi han <= 1 lan/4s (chi chay trong nhanh loai nay).",
  "\t\t\t\t\t\t\tif (!NpcSet.IsNpcRequestExist(Npc[nTGNpcIdx].m_dwID) && NpcSet.InsertNpcRequest(Npc[nTGNpcIdx].m_dwID))",
  "\t\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\t\tAUTOLOG(\"[S11-DO] hoi server ve tgID=%u\", Npc[nTGNpcIdx].m_dwID);",
  "\t\t\t\t\t\t\t\tSendClientCmdRequestNpc(Npc[nTGNpcIdx].m_dwID);",
  "\t\t\t\t\t\t\t}",
  "\t\t\t\t\t\t\ts_uS9ApID = 0;",
  "\t\t\t\t\t\t\treturn 0;"), "C4 FIX-6 60s + probe")

# ---------- C5: RequestNpcFail go ma ----------
ap("KProtocolProcess.cpp",
L("void KProtocolProcess::RequestNpcFail(BYTE* pMsg)",
  "{",
  "\tNPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;",
  "",
  "\tif (NpcSet.IsNpcRequestExist(pNpcSync->ID))",
  "\t\tNpcSet.RemoveNpcRequest(pNpcSync->ID);\t",
  "}"),
L("void KProtocolProcess::RequestNpcFail(BYTE* pMsg)",
  "{",
  "\tNPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;",
  "",
  "\tif (NpcSet.IsNpcRequestExist(pNpcSync->ID))",
  "\t\tNpcSet.RemoveNpcRequest(pNpcSync->ID);\t",
  "#ifndef _SERVER",
  "\t// [S11-XOAMA 26/08] Server vua XAC NHAN 'ID nay khong con' - truoc day handler chi tra",
  "\t// khe yeu cau va DE NGUYEN ban sao ma trong bang => probe vo dung voi ma pho bien nhat",
  "\t// (NPC da bi xoa han: 323 ma bi bo don 55s hot trong 282 giay). Go luon ban sao.",
  "\t// DecRef co GAC do_death/do_revive theo dung khuon bo don CheckBalance - khuon go cua",
  "\t// NetCommandRemoveNpc DecRef vo dieu kien se lam am bang dem BYTE tham chieu o (tran",
  "\t// 255 = ket o vinh vien) neu xac vua nhan goi chet giua luc hoi va luc fail ve.",
  "\t{",
  "\t\tint nIdxMa = NpcSet.SearchID(pNpcSync->ID);",
  "\t\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdxMa) && !Npc[nIdxMa].m_bClientOnly && Npc[nIdxMa].m_Kind != kind_partner)",
  "\t\t{",
  "\t\t\tAUTOLOG(\"[S11-XOAMA] npc=%u idx=%d kind=%u doing=%d reg=%d life=%d -> go ma theo xac nhan server t=%u\", pNpcSync->ID, nIdxMa, Npc[nIdxMa].m_Kind, (int)Npc[nIdxMa].m_Doing, Npc[nIdxMa].m_RegionIndex, Npc[nIdxMa].m_CurrentLife, SubWorld[0].m_dwCurrentTime);",
  "\t\t\tif (Npc[nIdxMa].m_RegionIndex >= 0)",
  "\t\t\t{",
  "\t\t\t\tSubWorld[0].m_Region[Npc[nIdxMa].m_RegionIndex].RemoveNpc(nIdxMa);",
  "\t\t\t\tif (Npc[nIdxMa].m_Doing != do_death && Npc[nIdxMa].m_Doing != do_revive)",
  "\t\t\t\t\tSubWorld[0].m_Region[Npc[nIdxMa].m_RegionIndex].DecRef(Npc[nIdxMa].m_MapX, Npc[nIdxMa].m_MapY, obj_npc);",
  "\t\t\t}",
  "\t\t\tNpcSet.Remove(nIdxMa);",
  "\t\t}",
  "\t}",
  "#endif",
  "}"), "C5 RequestNpcFail go ma")

# ---------- C6: SyncNpc client - toa do khong thuoc map hien tai ----------
ap("KProtocolProcess.cpp",
L("\tAUTOLOG_EVERY(1000, \"SYNCNPC-REGION-BAD npc=%u mps=(%d,%d) region=%d kind=%d set=%d t=%u\", NpcSync->ID, NpcSync->MapX, NpcSync->MapY, nRegion, (int)NpcSync->m_btKind, NpcSync->NpcSettingIdx, SubWorld[0].m_dwCurrentTime);",
  "",
  "\tif (nRegion == -1)",
  "\t\treturn;"),
L("\tAUTOLOG_EVERY(1000, \"SYNCNPC-REGION-BAD npc=%u mps=(%d,%d) region=%d kind=%d set=%d t=%u\", NpcSync->ID, NpcSync->MapX, NpcSync->MapY, nRegion, (int)NpcSync->m_btKind, NpcSync->NpcSettingIdx, SubWorld[0].m_dwCurrentTime);",
  "",
  "\tif (nRegion == -1)",
  "\t{",
  "#ifndef _SERVER",
  "\t\t// [S11-MAPLA 26/08] Toa do goi tra loi KHONG thuoc cua so map hien tai (tra loi",
  "\t\t// xuyen map / goi qua cu). Truoc day return im lang: (a) GIU nguyen ban sao ma,",
  "\t\t// (b) khe yeu cau REQNPC ket 100 tick moi tu hoi. Go ban sao (DecRef co gac nhu",
  "\t\t// [S11-XOAMA]) + tra khe ngay.",
  "\t\tint nIdxLa = NpcSet.SearchID(NpcSync->ID);",
  "\t\tif (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdxLa) && !Npc[nIdxLa].m_bClientOnly && Npc[nIdxLa].m_Kind != kind_partner)",
  "\t\t{",
  "\t\t\tAUTOLOG(\"[S11-MAPLA] npc=%u idx=%d doing=%d reg=%d mps=(%d,%d) ngoai map -> go t=%u\", NpcSync->ID, nIdxLa, (int)Npc[nIdxLa].m_Doing, Npc[nIdxLa].m_RegionIndex, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);",
  "\t\t\tif (Npc[nIdxLa].m_RegionIndex >= 0)",
  "\t\t\t{",
  "\t\t\t\tSubWorld[0].m_Region[Npc[nIdxLa].m_RegionIndex].RemoveNpc(nIdxLa);",
  "\t\t\t\tif (Npc[nIdxLa].m_Doing != do_death && Npc[nIdxLa].m_Doing != do_revive)",
  "\t\t\t\t\tSubWorld[0].m_Region[Npc[nIdxLa].m_RegionIndex].DecRef(Npc[nIdxLa].m_MapX, Npc[nIdxLa].m_MapY, obj_npc);",
  "\t\t\t}",
  "\t\t\tNpcSet.Remove(nIdxLa);",
  "\t\t}",
  "\t\tif (NpcSet.IsNpcRequestExist(NpcSync->ID))",
  "\t\t\tNpcSet.RemoveNpcRequest(NpcSync->ID);",
  "#endif",
  "\t\treturn;",
  "\t}"), "C6 SyncNpc map la")

# ---------- S1: NpcRequestCommand - gac xuyen map ----------
ap("KProtocolProcess.cpp",
L("void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)",
  "{",
  "\tNPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;",
  "\tNpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);",
  "}"),
L("void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)",
  "{",
  "\tNPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;",
  "#ifdef _SERVER",
  "\t// [S11-DOIMAP 26/08] KNpcSet::SyncNpc tim ID TOAN CUC moi map va goi NPC_SYNC khong",
  "\t// mang ma map => nguoi choi da roi map (ve thanh) van duoc tra ve TOA DO MAP KHAC,",
  "\t// client dien giai tren map hien tai thanh vi tri bay va lam ma BAT TU (moi lan tra",
  "\t// loi lai refresh dong ho don). Khac map thi tra FAIL de client go ban sao.",
  "\tint nS11Me = Player[nIndex].m_nIndex;",
  "\tif (nS11Me > 0 && nS11Me < MAX_NPC)",
  "\t{",
  "\t\tint nS11Found = NpcSet.SearchID(pNpcRequestSync->ID);",
  "\t\tif (nS11Found > 0 && Npc[nS11Found].m_SubWorldIndex != Npc[nS11Me].m_SubWorldIndex)",
  "\t\t{",
  "\t\t\tNPC_REQUEST_FAIL S11Fail;",
  "\t\t\tS11Fail.ProtocolType = (BYTE)s2c_requestnpcfail;",
  "\t\t\tS11Fail.ID = pNpcRequestSync->ID;",
  "\t\t\tg_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&S11Fail, sizeof(S11Fail));",
  "\t\t\tAUTOLOG_IDX(nS11Me, \"[S11-DOIMAP] plr=%d hoi id=%u dang o map khac (sw=%d vs %d) -> tra fail\", nIndex, pNpcRequestSync->ID, Npc[nS11Found].m_SubWorldIndex, Npc[nS11Me].m_SubWorldIndex);",
  "\t\t\treturn;",
  "\t\t}",
  "\t}",
  "#endif",
  "\tNpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);",
  "}"), "S1 gac xuyen map")

# ---------- S2: KRegion.h - hang so moi ----------
ap("KRegion.h",
L("// Tran cho duong dong bo dinh ky (NormalSync). Ban goc = 100; chu game chon 500.",
  "#define\tNPC_SYNC_BROADCAST_LIMIT\t500"),
L("// Tran cho duong dong bo dinh ky (NormalSync). Ban goc = 100; chu game chon 500.",
  "#define\tNPC_SYNC_BROADCAST_LIMIT\t500",
  "// [S11 26/08] Goi su kien MOT LAN (chet s2c_npcdeath, go s2c_npcremove): hiem (vai",
  "// cu/giay ca tran) nhung MAT goi la client giu MA - danh vao khong khi, xac 0 mau",
  "// van di, bung ve trai. Gia tri > so nguoi toi da nghia la KHONG CAT AI; chi phi",
  "// thuc bi chan boi so node trong danh sach region, khong phai boi so nay.",
  "#define\tNPC_EVENT_BROADCAST_LIMIT\t100000"), "S2 hang so")

# ---------- S3: DoDeath mien ngan sach ----------
ap("KNpc.cpp",
L("\t//",
  "\tint nMaxCount = MAX_BROADCAST_COUNT;",
  "\tCURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);",
  "\tint i;"),
L("\t//",
  "\t// [S11 26/08] goi CHET mien ngan sach (xem chu thich NPC_EVENT_BROADCAST_LIMIT).",
  "\tint nMaxCount = NPC_EVENT_BROADCAST_LIMIT;",
  "\tCURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);",
  "\tint i;"), "S3 DoDeath")

# ---------- S4: KNpcSet::Remove mien ngan sach ----------
ap("KNpcSet.cpp",
L("\t\tint nMaxCount = MAX_BROADCAST_COUNT;",
  "\t\tSubWorld[nSubWorld].m_Region[nRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);"),
L("\t\t// [S11 26/08] goi GO mien ngan sach (xem chu thich NPC_EVENT_BROADCAST_LIMIT).",
  "\t\tint nMaxCount = NPC_EVENT_BROADCAST_LIMIT;",
  "\t\tSubWorld[nSubWorld].m_Region[nRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);"), "S4 KNpcSet::Remove")

# ---------- S5: SendDataToNearRegion them tham so nLimit ----------
ap("KNpc.h",
"\tvoid\t\t\t\tSendDataToNearRegion(void* pBuffer, DWORD dwSize);",
"\tvoid\t\t\t\tSendDataToNearRegion(void* pBuffer, DWORD dwSize, int nLimit = -1);\t// [S11] nLimit>0 = tran rieng (goi chet/go)", "S5a KNpc.h")

ap("KNpc.cpp",
L("void\tKNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize)",
  "{"),
L("void\tKNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize, int nLimit)",
  "{"), "S5b chu ky")

ap("KNpc.cpp",
L("\tint nMaxCount = MAX_BROADCAST_COUNT;",
  "\tSubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX, m_MapY);"),
L("\t// [S11] nLimit>0 = goi su kien mot-lan (chet/go) mien ngan sach; mac dinh giu 100.",
  "\tint nMaxCount = (nLimit > 0) ? nLimit : MAX_BROADCAST_COUNT;",
  "\tSubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX, m_MapY);"), "S5c than ham")

# 2 call-site go khi SetPos/ChangeWorld (dem dung 2)
ap("KNpc.cpp",
"\t\tSendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));",
"\t\tSendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC), NPC_EVENT_BROADCAST_LIMIT);\t// [S11] go khong cat ai",
"S5d call-site go", expect=2)

sys.exit(0 if ok else 1)
