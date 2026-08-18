//---------------------------------------------------------------------------
// taobot_bdb.cpp - nhan ban nhan vat mau thanh N tai khoan bot, ghi THANG vao
//                  Berkeley DB "roledb" cua Goddess.
//
// VI SAO PHAI VIET BANG C++ CHU KHONG PHAI PYTHON:
//   roledb co MOT CHI MUC PHU theo ten tai khoan (IDBRoleServer.cpp:60
//   db_table->addIndex(get_account)). Berkeley DB chi tu bao tri chi muc do khi ghi
//   QUA cung mot handle da associate. Ghi thang vao tep bang thu vien khac se de chi
//   muc lech, va GetRoleListOfAccount (IDBRoleServer.cpp:584) se KHONG THAY bot.
//   Nen tool nay dung lai dung lop ZDBTable va dung ham chi muc get_account y het
//   Goddess.
//
// BAT BUOC:
//   * Goddess PHAI DUNG HAN truoc khi chay (BDB env mo doc-ghi, hai tien trinh cung
//     ghi la hong du lieu).
//   * Chay voi thu muc hien hanh = bin\multiserver  (ZDBTable lay getcwd() + "\database",
//     xem DBTable.cpp:15-19).
//
// AN TOAN:
//   * Mac dinh CHAY THU, khong ghi gi. Phai them --apply moi ghi.
//   * Tu choi neu ten nhan vat dinh tao DA TON TAI (khoa chinh cua roledb la TEN
//     NHAN VAT - IDBRoleServer.cpp:576).
//   * Tu choi neu nhan vat mau DA CO PHAI: he bot chi tu vao phai khi bot CHUA co phai
//     (KBotManager.cpp:2891). Clone tu nhan vat da co phai = bot dung im mai.
//---------------------------------------------------------------------------
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#include "DBTable.h"
#include "S3DBInterface.h"

//--------------------------------------------------------------------- chi muc
// PHAI y het Goddess (IDBRoleServer.cpp:26-35), neu khong chi muc se khac dinh dang.
int get_account(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey)
{
	memset(ikey, 0, sizeof(DBT));
	TRoleData *pRoleData = (TRoleData *)pdata->data;
	ikey->data = pRoleData->BaseInfo.caccname;
	ikey->size = (u_int32_t)(strlen(pRoleData->BaseInfo.caccname) + 1);
	return 0;
}

//--------------------------------------------------------------------- con tro
// ZDBTable::closeCursor (DBTable.h:47-56) chi free(key/data) roi delete cursor -
// no KHONG goi c_close tren cursor->dbcp. Moi lan search() ma khong duyet het bang
// next() se de lai MOT con tro BDB treo; BDB canh bao "Closing a primary DB while a
// secondary DB has active cursors is unsafe" va nhieu lan se can khoa.
// Ham nay dong dbcp truoc roi moi tha bo nho.
static void DongConTro(ZDBTable *pTab, ZCursor *c)
{
	if (!c) return;
	if (c->dbcp) { c->dbcp->c_close(c->dbcp); c->dbcp = NULL; }
	pTab->closeCursor(c);
}

//--------------------------------------------------------------------- ten bot
static const char *HO[] = {
	"Tran", "Le", "Nguyen", "Pham", "Hoang", "Vo", "Dang", "Bui", "Do", "Ngo",
	"Duong", "Ly", "Ho", "Truong", "Dinh", "Mai", "Phan", "Vu", "Cao", "Ha"
};
static const char *TEN[] = {
	"Phong", "Vu", "Long", "Ha", "Minh", "Tuan", "Khoa", "Nam", "Lam", "Hai",
	"Anh", "Bao", "Thang", "Quan", "Hung", "Son", "Trung", "Kiet", "Dat", "Vinh",
	"Nhat", "Duy", "Tai", "Loc", "Phuc", "Thinh", "Khang", "Hieu", "Toan", "Nghia"
};
#define N_HO   (sizeof(HO)  / sizeof(HO[0]))
#define N_TEN  (sizeof(TEN) / sizeof(TEN[0]))

// Ten nhan vat PHAI duy nhat: no vua la khoa chinh cua roledb, vua la thu ma
// NpcSet::SearchName tra ve khop DAU TIEN (PM, moi bang hoi, theo-sau-theo-ten).
static void SinhTen(char *szOut, int nSize, int nIdx)
{
	_snprintf(szOut, nSize, "%s%s%d", HO[rand() % N_HO], TEN[rand() % N_TEN], nIdx);
	szOut[nSize - 1] = 0;
}

//--------------------------------------------------------------------- tro giup
static void InHuongDan()
{
	printf(
		"taobot_bdb - nhan ban nhan vat mau thanh N tai khoan bot (Berkeley DB roledb)\n"
		"\n"
		"  taobot_bdb --mau <taikhoan> [--tu 1] [--den 1000] [--he-deu] [--apply]\n"
		"  taobot_bdb --xem <taikhoan>          xem thong tin nhan vat cua mot tai khoan\n"
		"  taobot_bdb --go --tu 1 --den 1000 [--apply]   go cac bot da tao\n"
		"\n"
		"  --mau <tk>   tai khoan co nhan vat dung lam khuon (vd: hinodl)\n"
		"  --tu/--den   dai TEN TAI KHOAN bot, mac dinh 1..1000\n"
		"  --he-deu     rai deu ngu hanh 0..4 (Kim/Moc/Thuy/Hoa/Tho) - NEN BAT\n"
		"  --apply      THAT SU ghi. Khong co = chay thu, khong dong vao du lieu\n"
		"\n"
		"BAT BUOC: dung han Goddess truoc khi chay, va chay tu thu muc bin\\multiserver\n");
}

//--------------------------------------------------------------------- main
int main(int argc, char *argv[])
{
	const char *szMau  = NULL;
	const char *szXem  = NULL;
	int  nTu = 1, nDen = 1000;
	bool bApply = false, bHeDeu = false, bGo = false;

	for (int i = 1; i < argc; i++)
	{
		if      (!strcmp(argv[i], "--mau")   && i + 1 < argc) szMau = argv[++i];
		else if (!strcmp(argv[i], "--xem")   && i + 1 < argc) szXem = argv[++i];
		else if (!strcmp(argv[i], "--tu")    && i + 1 < argc) nTu   = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--den")   && i + 1 < argc) nDen  = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--apply"))  bApply = true;
		else if (!strcmp(argv[i], "--he-deu")) bHeDeu = true;
		else if (!strcmp(argv[i], "--go"))     bGo    = true;
		else { InHuongDan(); return 1; }
	}
	if (!szMau && !szXem && !bGo) { InHuongDan(); return 1; }

	setvbuf(stdout, NULL, _IONBF, 0);   // khong dem: sap giua chung van thay duoc dong cuoi

	char szCwd[512];
	_getcwd(szCwd, sizeof(szCwd));
	printf("Thu muc hien hanh : %s\n", szCwd);
	printf("   => moi truong BDB se la: %s\\database\n", szCwd);
	printf("   (phai la bin\\multiserver, neu khong se tao nham mot roledb RONG)\n\n");

	srand((unsigned)time(NULL));

	// BAY DA DINH MOT LAN: ZDBTable::ZDBTable chi dat index_number = 0 BEN TRONG nhanh
	// mo moi truong THANH CONG (DBTable.cpp:32-38). Neu db_env_create/dbenv->open that bai
	// thi index_number giu nguyen RAC, va addIndex() ngay sau do ghi vao
	// get_index_funcs[rac] -> hong heap (0xC0000374) truoc khi kip in loi.
	// Vi dbenv la protected, ta do gian tiep: tu mo thu moi truong y het truoc da.
	{
		char szEnv[512];
		_getcwd(szEnv, sizeof(szEnv));
		strcat(szEnv, "\\database");
		DB_ENV *pTest = NULL;
		int ret = db_env_create(&pTest, 0);
		if (ret) { printf("LOI: db_env_create that bai (%d)\n", ret); return 2; }
		pTest->set_errpfx(pTest, "kiemtra");
		pTest->set_lg_regionmax(pTest, 512 * 1024);
		pTest->set_lg_max(pTest, 16 * 1024 * 1024);
		pTest->set_lg_bsize(pTest, 2 * 1024 * 1024);
		pTest->set_cachesize(pTest, 0, 64 * 1024 * 1024, 1);
		ret = pTest->open(pTest, szEnv,
		                  DB_CREATE | DB_INIT_LOG | DB_INIT_LOCK | DB_INIT_MPOOL |
		                  DB_INIT_TXN | DB_RECOVER | DB_THREAD | DB_PRIVATE, 0);
		if (ret)
		{
			printf("LOI: khong mo duoc moi truong BDB tai\n   %s\n   ma loi %d (%s)\n",
			       szEnv, ret, db_strerror(ret));
			printf("   Kiem: Goddess da dung han chua? Dung thu muc bin\\multiserver chua?\n");
			pTest->close(pTest, 0);
			return 2;
		}
		pTest->close(pTest, 0);
		printf("Moi truong BDB: mo thu OK (%s)\n\n", szEnv);
	}

	ZDBTable *pTab = new ZDBTable("database", "roledb");
	pTab->addIndex(get_account);
	if (!pTab->open())
	{
		printf("LOI: khong mo duoc bang roledb (moi truong thi OK).\n");
		delete pTab;
		return 2;
	}
	printf("Mo roledb: OK\n");

	//------------------------------------------------------------- XEM
	if (szXem)
	{
		ZCursor *c = pTab->search(szXem, (int)strlen(szXem) + 1, 0);
		if (!c) { printf("Khong tim thay nhan vat nao cua tai khoan %s\n", szXem); }
		else
		{
			// QUY UOC CON TRO CUA ZDBTable (doc DBTable.cpp:222-240):
			//   _next() TU free(cursor->data), va khi het du lieu no c_close + DELETE CURSOR
			//   roi moi tra false. => sau mot next() tra false, con tro DA CHET.
			//   Goi closeCursor luc do la giai phong hai lan -> hong heap (0xC0000374).
			//   Goddess cung theo quy uoc nay: IDBRoleServer.cpp:635 "if(!next(cursor)) break;"
			//   va KHONG he goi closeCursor.
			do {
				TRoleData *p = (TRoleData *)c->data;
				// ipassrole -> m_nChestPW (KPlayerDBFuns.cpp:382). Gia tri 214519 la MIN:
				// KPlayer::LaunchPlayer (KPlayer.cpp:6768) se g_pServer->Release() roi dong
				// 6776 goi PackDataToClient tren con tro NULL -> giet tang mang may chu.
				printf("  ten=%-20s taikhoan=%-16s cap=%-4d nguhanh=%d phai=%d/%d "
				       "ipassrole=%d%s len=%d\n",
				       p->BaseInfo.szName, p->BaseInfo.caccname, p->BaseInfo.ifightlevel,
				       p->BaseInfo.ifiveprop, p->BaseInfo.nSect, p->BaseInfo.nFirstSect,
				       p->BaseInfo.ipassrole,
				       (p->BaseInfo.ipassrole == 214519) ? "  <<< MIN 214519 !!!" : "",
				       p->dwDataLen);
			} while (pTab->next(c));   // KHONG closeCursor sau vong nay
		}
		pTab->close();
		delete pTab;
		return 0;
	}

	//------------------------------------------------------------- GO
	if (bGo)
	{
		int nXoa = 0;
		for (int i = nTu; i <= nDen; i++)
		{
			char szAcc[32];
			_snprintf(szAcc, sizeof(szAcc), "%d", i);
			szAcc[sizeof(szAcc) - 1] = 0;
			ZCursor *c = pTab->search(szAcc, (int)strlen(szAcc) + 1, 0);
			if (!c) continue;
			// gom ten truoc roi moi xoa: remove() ngay giua vong duyet se lam hong con tro
			char szTens[8][100];
			int  nTen = 0;
			do {
				TRoleData *p = (TRoleData *)c->data;
				if (nTen < 8)
				{
					strncpy(szTens[nTen], p->BaseInfo.szName, 99);
					szTens[nTen][99] = 0;
					nTen++;
				}
			} while (pTab->next(c));   // KHONG closeCursor (xem quy uoc o tren)
			for (int k = 0; k < nTen; k++)
			{
				if (bApply)
					pTab->remove(szTens[k], (int)strlen(szTens[k]) + 1);
				nXoa++;
			}
		}
		printf("%s %d nhan vat bot trong dai %d..%d\n",
		       bApply ? "DA XOA" : "(chay thu) se xoa", nXoa, nTu, nDen);
		if (bApply) pTab->commit();
		pTab->close();
		delete pTab;
		return 0;
	}

	//------------------------------------------------------------- NHAN BAN
	ZCursor *cMau = pTab->search(szMau, (int)strlen(szMau) + 1, 0);
	if (!cMau)
	{
		printf("LOI: tai khoan mau \"%s\" khong co nhan vat nao trong roledb.\n", szMau);
		pTab->close(); delete pTab; return 3;
	}

	int nLenMau = cMau->size;
	char *pBlob = (char *)malloc(nLenMau);
	memcpy(pBlob, cMau->data, nLenMau);
	TRoleData *pMau = (TRoleData *)pBlob;

	printf("Nhan vat mau : ten=%s  taikhoan=%s  cap=%d  nguhanh=%d  phai=%d/%d  blob=%d byte\n",
	       pMau->BaseInfo.szName, pMau->BaseInfo.caccname, pMau->BaseInfo.ifightlevel,
	       pMau->BaseInfo.ifiveprop, pMau->BaseInfo.nSect, pMau->BaseInfo.nFirstSect, nLenMau);
	DongConTro(pTab, cMau);

	// Chot dieu kien: mau PHAI chua vao phai.
	// KBotManager.cpp:2888-2896 - bot chi tu vao phai khi GetCurFactionNo() < 0.
	// Clone tu nhan vat DA co phai => bot khong bao gio tu vao phai, dung im mai.
	bool bCoPhai = (pMau->BaseInfo.nSect != 0xFF) || (pMau->BaseInfo.nFirstSect != 0xFF);
	if (bCoPhai && (pMau->BaseInfo.nSect < 10 || pMau->BaseInfo.nFirstSect < 10))
	{
		printf("\nCANH BAO: nhan vat mau CO VE DA VAO PHAI (nSect=%d nFirstSect=%d).\n",
		       pMau->BaseInfo.nSect, pMau->BaseInfo.nFirstSect);
		printf("          Tool se XOA PHAI tren cac ban sao (dat 0xFF = chua vao phai)\n");
		printf("          de bot tu vao phai duoc o cap 10.\n");
	}

	int nTong = nDen - nTu + 1;
	printf("\nKE HOACH: tao %d nhan vat bot cho tai khoan \"%d\"..\"%d\"\n", nTong, nTu, nDen);
	printf("          ngu hanh: %s\n", bHeDeu ? "rai deu 0..4" : "giu nguyen cua mau");
	printf("          phai    : xoa (0xFF) de bot tu vao phai o cap 10\n");
	printf("          che do  : %s\n\n", bApply ? "GHI THAT" : "CHAY THU (khong ghi gi)");

	if (!bApply)
	{
		printf("Chay thu xong. Them --apply de lam that.\n");
		free(pBlob); pTab->close(); delete pTab; return 0;
	}

	char *pNew = (char *)malloc(nLenMau);
	int nOk = 0, nTrung = 0;
	for (int i = nTu; i <= nDen; i++)
	{
		char szAcc[32], szTen[64];
		_snprintf(szAcc, sizeof(szAcc), "%d", i);
		szAcc[sizeof(szAcc) - 1] = 0;

		// ten duy nhat: thu toi 40 lan roi bo cuoc bang ten xac dinh
		bool bDat = false;
		for (int t = 0; t < 40 && !bDat; t++)
		{
			SinhTen(szTen, sizeof(szTen), i);
			ZCursor *cc = pTab->search_key(szTen, (int)strlen(szTen) + 1);
			if (!cc) bDat = true; else DongConTro(pTab, cc);
		}
		if (!bDat)
		{
			_snprintf(szTen, sizeof(szTen), "Bot%d", i);
			szTen[sizeof(szTen) - 1] = 0;
			ZCursor *cc = pTab->search_key(szTen, (int)strlen(szTen) + 1);
			if (cc) { DongConTro(pTab, cc); nTrung++; continue; }
		}

		memcpy(pNew, pBlob, nLenMau);
		TRoleData *p = (TRoleData *)pNew;

		memset(p->BaseInfo.szName, 0, sizeof(p->BaseInfo.szName));
		strncpy(p->BaseInfo.szName, szTen, sizeof(p->BaseInfo.szName) - 1);
		memset(p->BaseInfo.caccname, 0, sizeof(p->BaseInfo.caccname));
		strncpy(p->BaseInfo.caccname, szAcc, sizeof(p->BaseInfo.caccname) - 1);

		if (bHeDeu)
			p->BaseInfo.ifiveprop = (i - nTu) % 5;   // series_metal..series_earth (GameDataDef.h:468-472)

		// ---- GIOI TINH ----
		// Truoc day KHONG he ghi bSex nen ca 1000 bot chep nguyen gioi tinh cua nhan vat mau
		// (hinodl = nam) => sinh ra toan nam, va nam thi khong dung de vao hai phai he Thuy.
		//
		// Y NGHIA TRUONG (da doc tan dong, khong doan): KPlayerDBFuns.cpp:238-246
		//   bSex khac 0 -> PLAYER_FEMALE_NPCTEMPLATEID (-2) = NU
		//   bSex bang 0 -> PLAYER_MALE_NPCTEMPLATEID   (-1) = NAM
		// Mau nhan vat chon CHI theo bSex nen doi mot truong nay la doi luon hinh dang;
		// khong phai sua them truong ngoai hinh nao khac.
		//
		// LUAT: he THUY (ifiveprop == 2) BAT BUOC NU - hai phai cua he do la Nga My va Thuy
		// Yen, ca hai deu chi nhan nu. Cac he con lai boc ngau nhien nam/nu.
		if (p->BaseInfo.ifiveprop == 2)
			p->BaseInfo.bSex = 1;                 // he Thuy: chi co nu
		else
			p->BaseInfo.bSex = (BYTE)(rand() & 1); // cac he khac: nam / nu deu duoc

		// 0xFF -> (char)-1 => GetCurFactionNo() < 0 => du dieu kien tu vao phai
		p->BaseInfo.nSect      = 0xFF;
		p->BaseInfo.nFirstSect = 0xFF;

		if (pTab->add(p->BaseInfo.szName, (int)strlen(p->BaseInfo.szName) + 1, pNew, nLenMau))
			nOk++;

		if ((nOk % 100) == 0 && nOk) printf("   ... da tao %d/%d\n", nOk, nTong);
	}

	pTab->commit();
	pTab->close();
	free(pNew);
	free(pBlob);
	delete pTab;

	printf("\nXONG: tao duoc %d/%d nhan vat bot", nOk, nTong);
	if (nTrung) printf(" (%d bi bo vi trung ten)", nTrung);
	printf(".\nBat lai Goddess roi goi bot trong game.\n");
	return 0;
}
