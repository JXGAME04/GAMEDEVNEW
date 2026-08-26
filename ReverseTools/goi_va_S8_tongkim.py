# -*- coding: utf-8 -*-
# Ap TRON GOI S8 (4 va) vao mot cay nguon bat ky. Dung: python apply_all_s8.py <ROOT>
#   A - nan vi tri ban than khi lech >= 8 o (SyncNpcMinPlayer nhanh 3)
#   B - tra khe yeu cau NPC truoc khi FIX D bo goi (SyncNpc)
#   C - bang NPC day thi khong ghi de Npc[0] va khong xoa yeu cau (SyncNpc)
#   D - TK_GheVongKim doi sang dieu kien hinh hoc (CoreShell)
import os, subprocess, sys

SAFE = r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts\safe_edit.py"
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW"
KPP  = os.path.join(ROOT, r"Sources\Core\Src\KProtocolProcess.cpp")
CS   = os.path.join(ROOT, r"Sources\Core\Src\CoreShell.cpp")
TMP  = os.path.dirname(os.path.abspath(__file__))

def J(x): return "\r\n".join(x)

def run(name, target, old, new):
    fo = os.path.join(TMP, "aa_" + name + "_old.txt"); fn = os.path.join(TMP, "aa_" + name + "_new.txt")
    open(fo, "wb").write(old.encode("ascii")); open(fn, "wb").write(new.encode("ascii"))
    r = subprocess.run([sys.executable, SAFE, target, "--old-file", fo, "--new-file", fn],
                       capture_output=True, text=True)
    print("[%s] %s -> %s" % ("OK " if r.returncode == 0 else "FAIL", name, os.path.basename(target)))
    if r.returncode != 0:
        print(r.stdout); print(r.stderr); sys.exit(1)

# ================= A =================
run("A_nan_vitri", KPP,
J(["	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);"]),
J([
"#ifndef _SERVER",
"	// [S8 26/08] NAN VI TRI BAN THAN KHI LECH QUA LON (chu game: \"chay bay toa do khi",
"	// qua map phe kim bao danh\" + \"chet hoi sinh lai thi hay bi bo qua mua mau tai npc\").",
"	//",
"	// Nhanh thu ba nay = CA HAI region deu da nap. Ban goc CO Y khong nan (du doan phia",
"	// client cho muot), va de xuat \"nan moi lan sync\" da bi phan bien BAC BO hoi 25/08 vi",
"	// gay giat rubber-band ~4 o moi goi. Nhung co mot ca ban goc khong luong: may chu",
"	// DICH CHUYEN nguoi choi TRONG CUNG MOT MAP - hoi sinh Tong Kim (SetTempRevPos ->",
"	// mobinhtk.lua:365/435), Xa Phu doi phe (battle_transprot -> xaphu.lua:50 NewWorld",
"	// cung map), trap nem ra tran. Luc do KNpc::ChangeWorld thay cung map nen goi thang",
"	// KNpc::SetPos (KNpc.cpp:9962), ma SetPos CHI bao cho nguoi xung quanh",
"	// (s2c_npcremove) chu KHONG gui goi vi tri moi cho CHINH nguoi choi. Neu diem den",
"	// lai nam trong khoi region da nap thi ca hai nhanh tren deu khong chay -> client",
"	// GIU NGUYEN cho cu vinh vien:",
"	//   . may auto tinh duong A* tu diem xuat phat sai  -> \"chay bay toa do\";",
"	//   . may Tong Kim do TK_TrongTrai() bang toa do cu  -> khong nhan ra minh dang o hau",
"	//     doanh nen bo qua buoc mua mau o Quan Y, di thang ra trap.",
"	//",
"	// Nguong 8 o (256 mps) lay bang WA_NHAY_XA cua CoreShell.cpp: nhip client 54 ms, chay",
"	// bo nhanh nhat khong qua noi 1 o, nen lech tu 8 o tro len CHI CO THE la bi dich",
"	// chuyen. Do that 25/08 (SYNCME-DRIFT): sai so du doan binh thuong p90 = 83 mps, tuc",
"	// nguong nay cao gap 3 lan dinh nhieu binh thuong -> khong dung toi duong chay muot.",
"	if (nRegion >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)",
"	{",
"		int nMeX = 0, nMeY = 0;",
"		Npc[nNpcIdx].GetMpsPos(&nMeX, &nMeY);",
"		const int nLech = g_GetDistance(nMeX, nMeY, (int)pSync->m_dwMapX, (int)pSync->m_dwMapY);",
"		if (nLech >= 256)",
"		{",
"			AUTOLOG(\"[S8-NAN] lech=%d cl=(%d,%d) sv=(%d,%d) cell cl=(%d,%d) sv=(%d,%d) regcu=%d regmoi=%d doing=%d t=%u\", nLech, nMeX, nMeY, pSync->m_dwMapX, pSync->m_dwMapY, Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, nMapX, nMapY, Npc[nNpcIdx].m_RegionIndex, nRegion, (int)Npc[nNpcIdx].m_Doing, SubWorld[0].m_dwCurrentTime);",
"			SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);",
"			if (nRegion != Npc[nNpcIdx].m_RegionIndex)",
"			{",
"				// doi region: dung dung ham cua engine (no tu RemoveNpc/AddNpc, dat lai",
"				// m_dwRegionID + m_RegionIndex, va nap map neu can) - hai tham so dau la",
"				// REGION ID chu khong phai chi so (KSubWorld.cpp:2386 ban client).",
"				SubWorld[0].NpcChangeRegion(Npc[nNpcIdx].m_dwRegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);",
"			}",
"			Npc[nNpcIdx].m_MapX = nMapX;",
"			Npc[nNpcIdx].m_MapY = nMapY;",
"			Npc[nNpcIdx].m_OffX = pSync->m_wOffX;",
"			Npc[nNpcIdx].m_OffY = pSync->m_wOffY;",
"			if (Npc[nNpcIdx].m_RegionIndex >= 0)",
"				SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);",
"			// bo noi suy dang dang (khong thi nhan vat truot mot duong dai tu cho cu sang)",
"			memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));",
"			Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;",
"			// duong di dang chay duoc tinh tu diem xuat phat cu -> huy de tinh lai",
"			SubWorld[0].StopPath();",
"			return;",
"		}",
"	}",
"#endif",
"	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);",
]))

# ================= B =================
run("B_trakhe", KPP,
J([
"#ifndef _SERVER",
"		// [FIX-D 26/08] khong nhan NPC ngoai tam ve vao bang.",
"		if (S6_XaQuaTam(NpcSync->MapX, NpcSync->MapY))",
"		{",
"			g_nS6BoXa++;",
"			return;",
"		}",
"#endif",
]),
J([
"#ifndef _SERVER",
"		// [FIX-D 26/08] khong nhan NPC ngoai tam ve vao bang.",
"		if (S6_XaQuaTam(NpcSync->MapX, NpcSync->MapY))",
"		{",
"			g_nS6BoXa++;",
"			// [S8 26/08] PHAI TRA KHE YEU CAU TRUOC KHI BO GOI.",
"			// Goi SyncNpc day la TRA LOI cho yeu cau ma CHINH client da gui",
"			// (SyncNpcMin thay NPC la -> InsertNpcRequest -> SendClientCmdRequestNpc).",
"			// Khe chi duoc tra o duoi (RemoveNpcRequest) - nam SAU cai return nay, nen",
"			// moi goi bi FIX D bo la RO RI MOT KHE. Be chi co 19 khe dung duoc",
"			// (MAX_NPC_REQUEST 20 nhung KNpcSet.cpp:90 nap chi so 1..19), duong thu hoi",
"			// con lai la quet timeout 100 khung (~5,5 giay). Trong tran Tong Kim hang",
"			// tram nguoi lien tuc bang qua vanh 40 o thi be can khe trong vai giay; luc",
"			// do InsertNpcRequest tra FALSE (KNpcSet.cpp:1372) => client KHONG HOI NPC",
"			// MOI NAO NUA, mu ca NPC TINH dung sat ben (Quan Y cach diem hoi sinh 12 o)",
"			// - dung hai trieu chung chu game bao.",
"			if (NpcSet.IsNpcRequestExist(NpcSync->ID))",
"				NpcSet.RemoveNpcRequest(NpcSync->ID);",
"			return;",
"		}",
"#endif",
]))

# ================= C =================
run("C_bangday", KPP,
J([
"		Npc[nIdx].m_dwID = NpcSync->ID;",
"		Npc[nIdx].m_Kind = NpcSync->m_btKind;",
"		Npc[nIdx].m_Height = 0;",
"		",
"		if (NpcSet.IsNpcRequestExist(NpcSync->ID))",
"			NpcSet.RemoveNpcRequest(NpcSync->ID);",
]),
J([
"#ifndef _SERVER",
"		// [S8 26/08] BANG 256 KHE DA DAY (AddNpcSet2 -> AddNpcSet1 -> FindFree() = 0).",
"		// Ban goc van chay tiep va ghi Npc[0].m_dwID/m_Kind - ma Npc[0] la O TRONG",
"		// dung lam gia tri \"khong tim thay\" cua ca he (NpcSet.SearchID / FindFree /",
"		// DT_FindNpcName deu coi 0 la khong co). Ghi de vao do vua lam ban o trong,",
"		// vua XOA LUON yeu cau dang cho (RemoveNpcRequest ngay duoi) nen client se",
"		// KHONG BAO GIO hoi lai con NPC do - voi NPC DUNG YEN (Quan Y, Xa Phu, NPC",
"		// bao danh) thi may chu it khi phat lai NormalSync, nghia la mat han.",
"		// Dung: bo goi, GIU nguyen yeu cau de khe tu het han (~5,5 giay) roi hoi lai.",
"		if (!nIdx)",
"			return;",
"#endif",
"		Npc[nIdx].m_dwID = NpcSync->ID;",
"		Npc[nIdx].m_Kind = NpcSync->m_btKind;",
"		Npc[nIdx].m_Height = 0;",
"		",
"		if (NpcSet.IsNpcRequestExist(NpcSync->ID))",
"			NpcSet.RemoveNpcRequest(NpcSync->ID);",
]))

# ================= D =================
run("D_vongkim", CS,
J([
"static int TK_GheVongKim(int nPlayerIdx, int nX, int nY, int nDx, int nDy, UINT uCurTime)",
"{",
"	if (TK_ThayDuoc(nX, nY, nDx, nDy))",
"		return 0;",
"	const int wx = TK_O((int)g_TKVongKim.x);",
"	const int wy = TK_O((int)g_TKVongKim.y);",
"	if (g_GetDistance(nX, nY, wx, wy) <= TK_O(3))",
"		return 0;\t// dung sat diem vong ma van chua \"thay\" (lech ban do?) - di thang",
"	DT_WalkTo(nPlayerIdx, wx, wy, 96, uCurTime);",
"	return 1;",
"}",
]),
J([
"// Goc hoc chua NPC bao danh Kim: o phia TAY mang tuong va phia BAC hanh lang.",
"// So do tu ban do that (ReverseTools/tk_luoi_client_vs_server.py, ca hai luoi).",
"#define TK_KIM_HOC_X\t1562",
"#define TK_KIM_HOC_Y\t3082",
"static int TK_GheVongKim(int nPlayerIdx, int nX, int nY, int nDx, int nDy, UINT uCurTime)",
"{",
"	// (26/08 v2) BO dieu kien TK_ThayDuoc. Do ra ngay 26/08: nguon that cua",
"	// SubWorld::TestBarrier phia client la KScenePlaceC::GetObstacleInfo, ma ham do",
"	// chi tra loi trong CUA SO 3x3 region quanh nguoi choi (INSIDE_AREA(...,1) -",
"	// Scene/KScenePlaceC.cpp:1726-1739); ra ngoai cua so no tra thang Obstacle_Normal,",
"	// tuc \"co tuong\", DU CHO DO LA DAT TRONG. Region rong 16 o (RWPP_AREGION_WIDTH",
"	// 512 mps) nen moi diem cach hon ~16-31 o deu bi bao la bi chan => \"chua nhin",
"	// thay\" luon dung, dieu kien vo nghia. Do la ly do ban va 26/08 sang khong het loi.",
"	// Thay bang HINH HOC TAT DINH: NPC bao danh Kim nam trong mot HOC mo ve phia NAM,",
"	// loi ra vao DUY NHAT la hanh lang y = 3083..3084 (A* tung o tren ca luoi client",
"	// lan server deu di qua day). Chi can hoi minh va dich co CUNG PHIA hay khong.",
"	const int nHocMe   = (nX <= TK_O(TK_KIM_HOC_X) && nY <= TK_O(TK_KIM_HOC_Y));",
"	const int nHocDich = (nDx <= TK_O(TK_KIM_HOC_X) && nDy <= TK_O(TK_KIM_HOC_Y));",
"	if (nHocMe == nHocDich)",
"		return 0;\t\t// cung phia mang tuong - di thang",
"	const int wx = TK_O((int)g_TKVongKim.x);",
"	const int wy = TK_O((int)g_TKVongKim.y);",
"	if (g_GetDistance(nX, nY, wx, wy) <= TK_O(3))",
"		return 0;\t\t// da toi cua hanh lang - buoc sau di thang vao",
"	DT_WalkTo(nPlayerIdx, wx, wy, 96, uCurTime);",
"	return 1;",
"}",
]))
print("XONG GOI S8 tren %s" % ROOT)
