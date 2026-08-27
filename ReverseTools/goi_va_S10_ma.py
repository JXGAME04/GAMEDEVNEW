# -*- coding: ascii -*-
# [S10-MA] 26/08 dem - chu duyet: server go BONG MA khoi client ngay khi client
# xin danh mot ID khong con trong vung 3x3 (goi s2c_npcremove co san, 5 byte).
import io, sys
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
CRLF = "\r\n"
def L(*a): return CRLF.join(a)
c = io.open(p, "r", encoding="latin-1", newline="").read()
hb = sum(1 for ch in c if ord(ch) > 127)

# neo: dong tim muc tieu (duy nhat) -> chen truoc dong 'if (nNpcIndex > 0)' ngay sau no
a1 = "int nNpcIndex = Player[nIndex].FindAroundNpc((DWORD)ParamZ);"
n = c.count(a1)
if n != 1:
    print("LOI: neo FindAroundNpc khop %d lan" % n); sys.exit(1)
i = c.index(a1)
a2 = "\t\tif (nNpcIndex > 0)"
j = c.find(a2, i)
if j < 0 or j - i > 2000:
    print("LOI: khong thay 'if (nNpcIndex > 0)' sau neo (j=%d)" % j); sys.exit(1)
giua = c[i:j]
if "[S3-TGT-FIND]" not in giua:
    print("LOI: doan giua khong chua S3-TGT-FIND - sai hien truong"); sys.exit(1)

khoi = L(
"#ifdef _SERVER",
"\t\tif (nNpcIndex <= 0 && ParamZ > 0 && Player[nIndex].m_nNetConnectIdx >= 0)",
"\t\t{",
"\t\t\t// [S10-MA 26/08] GO BONG MA: client vua xin danh mot ID KHONG con trong vung",
"\t\t\t// 3x3 quanh no. Nguyen nhan pho bien: goi go s2c_npcremove truoc do bi rot",
"\t\t\t// (ngan sach broadcast MAX_BROADCAST_COUNT=100 nguoi/luot trong dam dong,",
"\t\t\t// KRegion.cpp:1395) => ban sao ma ket trong bang client toi ~55s (bo don",
"\t\t\t// 1000 tick, KNpcSet.cpp:755) va auto dung danh gio ca nua phut (do that",
"\t\t\t// 26/08: dot 28,7s danh id=92666 found=0, 323 ma bi don trong 282 giay).",
"\t\t\t// Server dang cam du thong tin o day: gui lai goi go CO SAN (5 byte) cho",
"\t\t\t// RIENG client nay - client xoa ma ngay, auto doi muc tieu o nhip ke.",
"\t\t\t// ID khong ton tai phia client thi handler ConformIdx tu vut (no-op).",
"\t\t\tNPC_REMOVE_SYNC RemoveCmd;",
"\t\t\tRemoveCmd.ProtocolType = (BYTE)s2c_npcremove;",
"\t\t\tRemoveCmd.ID = (DWORD)ParamZ;",
"\t\t\tg_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&RemoveCmd, sizeof(RemoveCmd));",
"\t\t\tAUTOLOG_IDX(Player[nIndex].m_nIndex, \"[S10-MA] plr=%d go bong ma id=%u khoi client (skill=%d)\", nIndex, (unsigned int)ParamZ, ParamX);",
"\t\t}",
"#endif",
a2)
c = c[:j] + khoi + c[j + len(a2):]
if sum(1 for ch in c if ord(ch) > 127) != hb:
    print("LOI: high-byte doi"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(c)
print("OK [S10-MA] da chen sau S3-TGT-FIND, truoc nhanh nNpcIndex>0")
