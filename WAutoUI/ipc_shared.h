
#ifndef IPC_SHARED_H
#define IPC_SHARED_H
//save old padding
#pragma pack(push, enter_nopadding)
//start no padding
#pragma	pack(1)
#define SHARED_SIZE	512*128

#define defSKILLNUMGET 72
enum PROTGAMEID
{
	PRG_REPLYRECVED = 1,
	PRG_MAINSYNC,
	PRG_REPISHIDE,
	PRG_ONOFFPK,
	PRG_AUTOONOFF,
	PRG_OPENNOPICK,
	PRG_TEAMNAMELIST,
};

enum PROTTOOLID
{
	PRT_CONNECT = 1,
	PRT_GAMELOOP,
	PRT_HIDEGAME,
	PRT_ISHIDE,
	PRT_TICKSTART,
	PRT_RETONOFPK,
	PRT_RETAUTOONOF,
	PRT_GETITEMNAME,
	PRT_GETTEAMAROUND,
	PRT_ACTAUTOLG,
	PRT_QUITGAME,	
};

struct SharedState
{
	int CmdID;
	unsigned int Size;
};

struct autoCoord
{
	int x;
	int y;
};

struct IPCSkillInfo
{
	char	szName[40];
	short	nId;
	short	nStyle;
	unsigned char	bLR;
	unsigned char	bAlly;
	unsigned char	bAura;
	unsigned char	bState;
};

struct IPCMainSync : public SharedState
{
	char szPassword[64];
	char szAccount[32];
	char szName[32];
	char szMap[32];
	int nMapId;
	int nNpcIdx;
	unsigned int dwPID;
	int life;
	int mana;
	int lifemax;
	int manamax;
	int stamina;
	int staminamax;
	int mapx;
	int mapy;
	int level;
	__int64 curexp;
	__int64 fullexp;
	int skillnum;
	short nSelServer1;
	short nSelServer2;
	IPCSkillInfo skill[defSKILLNUMGET];
	// (24/08 r2) dong trang thai auto dang lam gi (TCVN3) - hien o chan WAuto.
	// PHAI o CUOI struct; WAuto kiem Size >= sizeof truoc khi doc truong nay.
	char szHoatDong[48];
};

struct autoData
{
	int		bCheckiLife;
	int		bCheckiMana;
	int		nIlifeCell1;
	int		nIlifeCell2;
	int		nIlifeCell3;
	int		nImanaCell1;
	int		nImanaCell2;
	int		nImanaCell3;
	int		bCheckTPLife;
	int		bCheckTPMana;
	int		bCheckTPLifeGone;
	int		bCheckTPManaGone;
	int		bCheckTPIBox;
	int		bCheckTPMoney;
	int		bCheckTPIDmg;
	int		nTPLife;
	int		nTPMana;
	int		nTPMoney;
	int		nTPDmgItem;
	int		nTPiboxSel;
	int		bOutWhenTP;
	int		bOutWhenDis;
	int		bOutTimer;
	int		nHour;
	int		nMinute;
	int		bChat;
	int		nChatChann;
	char	szChat[80];
	int		bEatLifeFull;
	int		bEatPoison;
	int		bEatExp;
	int		bEatSkill;
	int		bUseBuff;
	int		bPTBuff;
	int		bOpenBag;
	int		nUseBuffVal;
	int		bFight;
	int		bApproach;
	int		bFightBack;
	int		bSkipGoldboss;
	int		nVision;
	int		nNearDist;
	int		nFBVision;
	int		nSelFHorse;
	int		nSelBoss;
	int		nSelFBack;
	int		nSkillIdSP1;
	int		nSkillIdSP2;
	int		nSkillIdSP3;
	int		nSkillIdL;
	int		nSkillIdR;
	int		nSkillIdB;
	int		nSkillIdP;
	int		nSkillIdA1;
	int		nSkillIdA2;
	int		nSkillIdLS;
	int		nSkillIdMS;
	int		nSLSPerc;
	int		nSMSPerc;
	int		nSkillIdC;
	int		nSkillCSec;
	int		bOnPK;
	int		bUseFKey;
	UINT	uFKey;
	int		nPKVision;
	int		bPKAppr;
	int		nPKNearDist;
	int		bPKFollowTG;
	int		bPKPlayer;
	int		bPKNpc;
	int		bPKDownHorse;
	int		bDrawVision;
	int		nSkillIdCS1;
	int		nSkillIdCS2;
	int		nSkillIdCS3;
	int		bCLBuff;
	int		bCLBuffCamp;
	int		nPriority;
	short	nSerPy[6];
	int		bRevive;
	int		bPickUp;
	int		bFollowPick;
	int		nPickVision;
	int		bCityPick;
	int		nPickType;
	int		bNoPick;
	int		nNOPCount;
	char	szNOPName[60][80];
	int		bFilter;
	int		nFtMaCount;
	int		nFtMagic[40][2];
	int		bPrize;
	int		nPrize;
	int		bLevel;
	int		nLevel;
	int		bSaveRing;
	int		nSRLevel;
	int		bArrangeI;
	int		bArrangeB;
	int		nSelInvitePt;
	int		nSelJoinPt;
	int		bJoinPtByList;
	int		nIJPtCount;
	char	szIJPtName[24][32];
	int		bLeavePt;
	int		nLeavePtMem;
	int		nLeavePtMin;
	int		bRemovePt;
	int		nRemovePtMin;
	int		bReturn;
	int		bSellItem;
	int		bSellHorse;
	int		nSelSell;
	int		bRepair;
	int		bFRepair;
	int		bWithdraw;
	int		nWDMoney;
	char	szBoxPass[16];
	int		bBuyLife;
	int		nBuyLifeSel;
	int		bBuyMana;
	int		nBuyManaSel;
	int		bBuyPois;
	int		nBuyPoisSel;
	int		nBLNum;
	int		nBMNum;
	int		nBPNum;
	int		bBuyTP;
	int		bHoldMoney;
	int		bGoStation;
	int		bGoMap;
	int		nBTPNum;
	int		nHoldMoneyNum;
	int		bSaveItem;
	int		nSelStore;
	int		nSelStation;
	int		nSelMap;
	int		bMoveFollow;
	int		bAroundPoint;
	int		bMoveCoord;
	int		bMoveUpHorse;
	int		bMoveKillMons;
	int		nFollowDist;
	int		nMoveMapId;
	int		nPointX;
	int		nPointY;
	char	szMoveMap[32];
	char	szFollName[32];
	int		nCoordCount;
	autoCoord sMoveCoord[24];
	int		bEncircle;
	// == Da Tau (18/08/2026) - PHAI o cuoi struct, truoc constructor ==
	int		bDaTau;			// bat/tat auto Da Tau
	int		bDTType[6];		// bat/tat tung loai nhiem vu 1..6
	int		nDTSkipMode;	// loai tat/ket: 0=treo (ngung DT) 1=huy nhiem vu
	int		nDTCancelMode;	// cach huy: 0=chi luot huy thuong 1=cho phep huy thuong(reset chuoi) 2=uu tien 100 manh SHXT
	int		nDTReward1;		// cua so Exp/Money/Random: 0/1/2
	int		nDTReward2;		// cua so Point/Lucky/Item: 0/1/2 (Lucky=tich luot huy)
	int		bDTUseBox;		// cho phep lay do/tien tu ruong (dung szBoxPass)
	int		bDTTrainAfter;	// xong/treo -> tha may cho auto thuong ve map luyen cong
	int		nDTWDMoney;		// (van luong) rut tu ruong khi thieu tien mua do
	int		bDTLenhBai;		// (20/08) loai tat: dung Lenh bai hoan thanh (6,1,4818) neu co
	int		bDTMuaSap;		// (20/08) T2 thieu do: di xem sap nguoi ban de mua (nhay 10 thanh)
	int		nDTMaxMua;		// (20/08) gia mua o sap toi da (van luong)
	// == Tong Kim (24/08/2026) - PHAI o cuoi struct, truoc constructor ==
	// (file APdata\<ID nhan vat>.dat ghi NGUYEN struct: them truong o giua la nat
	//  cau hinh cua moi nguoi choi cu - xem ky uc wauto-luu-cau-hinh-apdata)
	int		bTongKim;		// bat/tat auto Tong Kim
	int		bTKGio[4];		// bat tung khung gio (13h23 / 17h50 / 20h50 / 22h50)
	int		nTKSom;			// di som may phut truoc gio mo tran
	int		nTKLech;		// gio may chu lech gio may nay bao nhieu PHUT (co the am)
	int		nTKPhe;			// 0 = Tong, 1 = Kim, 2 = tu can bang theo quan so
	int		nTKMuaMau;		// 0 = mua nhanh (day tui), 1 = mua so luong, 2 = khong mua
	int		nTKSoBinh;		// so binh thuoc khi nTKMuaMau == 1
	int		bTKThuoc;		// tu an thuoc hoat dong Tong Kim (mua o Cua hang diem TK)
	int		nTKThuocSel;	// 0 = tat ca, 1 = chi tang cong, 2 = chi phong thu, 3 = cong + mau/toc
	int		nTKUuTien;		// 0 = dich gan nhat, 1 = uu tien Hieu Uy / Pho Tuong / Dai Tuong
	int		bTKVeCho;		// het tran nho Xa Phu dua ve dung cho luc nay
	// == Lien dau / WLLS (24/08/2026) - PHAI o cuoi struct, truoc constructor ==
	int		bLienDau;		// bat/tat auto Lien dau
	int		bLDKhung[2];	// bat khung gio 1 / 2 (mac dinh 18h va 20h)
	int		nLDGio[2];		// gio mo khung 1 / 2
	int		nLDPhut[2];		// phut mo khung 1 / 2
	int		nLDLuot;		// so luot moi khung (goc 4)
	int		nLDPhutLuot;	// so phut moi luot (goc 15 - doi duoc o wlls_config.lua)
	int		nLDSom;			// toi som may phut truoc gio mo
	int		nLDLech;		// gio may chu lech gio may nay bao nhieu PHUT (co the am)
	int		bLDTuLap;		// tu lap chien doi neu chua co
	char	szLDTen[20];	// ten chien doi (de trong = lay ten nhan vat)
	int		bLDCatDoCam;	// tu cat do CAM (WLLS_FORBID_ITEM) vao ruong truoc khi bao danh
	int		bLDNhanThuong;	// cuoi mua tu nhan thuong xep hang + danh hieu
	int		nLDVeThanh;		// het gio ra Xa phu ve thanh nao (0..6); 7 = o lai hoi truong
	int		bLDPhuVe;		// dung phu ve thanh khi dang o map khong co Su gia
	// == Lien dau r2 (24/08/2026 toi) - PHAI o cuoi struct, truoc constructor ==
	int		bLDParty;		// Song dau: tu to doi 2 nguoi voi ten trong danh sach
	int		nLDThanhBD;		// bao danh o thanh nao (0..6 theo g_LDVeMap); 7 = thanh dang dung
	int		nLDPtCount;		// so ten trong danh sach ban dien
	char	szLDPtName[8][32];	// danh sach ung vien to doi (chi moi 1 nguoi dau tien gap)
	// == Hoat dong Bach Nhan + Bang Chien (24/08/2026 dem) - PHAI o cuoi struct ==
	int		bHDBachNhan;	// bat auto Bach Nhan Loi Dai (cay exp hang ngay)
	int		nHDBNGio;		// gio vao (theo gio MAY CHU + nHDLech)
	int		nHDBNPhut;
	int		nHDBNCay;		// cay bao nhieu phut; 0 = toi khi het 50 luot exp
	int		nHDBNMode;		// 0 = dung an exp (an toan), 1 = tranh lam Loi Chu
	int		nHDBNDai;		// 0 = tu chon (dai 2), 1..5 = dai cu the
	int		bHDBNBuff;		// tu di an buff Co Thu x2
	int		bHDBangChien;	// bat auto Bang Chien (Vo Lam De Nhat Bang)
	int		nHDBCGio;		// gio di (mac dinh 20:25 - cua vao that 20:30-21:30)
	int		nHDBCPhut;
	int		nHDLech;		// gio may chu lech gio may nay (PHUT, am duoc) - chung ca tab
	autoData()
	{
		bCheckiLife = 0;
		bCheckiMana = 0;
		nIlifeCell1 = 0;
		nIlifeCell2 = 0;
		nIlifeCell3 = 0;
		nImanaCell1 = 0;
		nImanaCell2 = 0;
		nImanaCell3 = 0;
		bCheckTPLife = 0;
		bCheckTPMana = 0;
		bCheckTPLifeGone = 0;
		bCheckTPManaGone = 0;
		bCheckTPIBox = 0;
		bCheckTPMoney = 0;
		bCheckTPIDmg = 0;
		nTPLife = 0;
		nTPMana = 0;
		nTPMoney = 0;
		nTPDmgItem = 0;
		nTPiboxSel = 0;
		bOutWhenTP = 0;
		bOutWhenDis = 0;
		bOutTimer = 0;
		nHour = 0;
		nMinute = 0;
		bChat = 0;
		nChatChann = 0;
		szChat[0] = 0;
		bEatLifeFull = 0;
		bEatPoison = 0;
		bEatExp = 0;
		bEatSkill = 0;
		bUseBuff = 0;
		bPTBuff = 0;
		bOpenBag = 0;
		nUseBuffVal = 0;
		bFight = 0;
		bApproach = 0;
		bFightBack = 0;
		bSkipGoldboss = 0;
		nVision = 0;
		nNearDist = 0;
		nFBVision = 0;
		nSelFHorse = 0;
		nSelBoss = 0;
		nSelFBack = 0;
		nSkillIdSP1 = 0;
		nSkillIdSP2 = 0;
		nSkillIdSP3 = 0;
		nSkillIdL = 0;
		nSkillIdR = 0;
		nSkillIdB = 0;
		nSkillIdP = 0;
		nSkillIdA1 = 0;
		nSkillIdA2 = 0;
		nSkillIdLS = 0;
		nSkillIdMS = 0;
		nSLSPerc = 0;
		nSMSPerc = 0;
		nSkillIdC = 0;
		nSkillCSec = 0;
		bOnPK = 0;
		bUseFKey = 0;
		uFKey = 0;
		nPKVision = 0;
		bPKAppr = 0;
		nPKNearDist = 0;
		bPKFollowTG = 0;
		bPKPlayer = 0;
		bPKNpc = 0;
		bPKDownHorse = 0;
		bDrawVision = 0;
		nSkillIdCS1 = 0;
		nSkillIdCS2 = 0;
		nSkillIdCS3 = 0;
		bCLBuff = 0;
		bCLBuffCamp = 0;
		nPriority = 0;
		nSerPy[0] = 0;
		nSerPy[1] = 1;
		nSerPy[2] = 2;
		nSerPy[3] = 3;
		nSerPy[4] = 4;
		nSerPy[5] = 0;
		bRevive = 0;
		bPickUp = 0;
		bFollowPick = 0;
		nPickVision = 0;
		bCityPick = 0;
		nPickType = 0;
		bNoPick = 0;
		nNOPCount = 0;
		bFilter = 0;
		nFtMaCount = 0;
		bPrize = 0;
		nPrize = 0;
		bLevel = 0;
		nLevel = 0;
		bSaveRing = 0;
		nSRLevel = 0;
		bArrangeI = 0;
		bArrangeB = 0;
		nSelInvitePt = 0;
		nSelJoinPt = 0;
		bJoinPtByList = 0;
		nIJPtCount = 0;
		bLeavePt = 0;
		nLeavePtMem = 0;
		nLeavePtMin = 0;
		bRemovePt = 0;
		nRemovePtMin = 0;
		bReturn = 0;
		bSellItem = 0;
		bSellHorse = 0;
		nSelSell = 0;
		bRepair = 0;
		bFRepair = 0;
		bWithdraw = 0;
		nWDMoney = 0;
		szBoxPass[0] = 0;
		bBuyLife = 0;
		nBuyLifeSel = 0;
		bBuyMana = 0;
		nBuyManaSel = 0;
		bBuyPois = 0;
		nBuyPoisSel = 0;
		nBLNum = 0;
		nBMNum = 0;
		nBPNum = 0;
		bBuyTP = 0;
		bHoldMoney = 0;
		bGoStation = 0;
		bGoMap = 0;
		nBTPNum = 0;
		nHoldMoneyNum = 0;
		bSaveItem = 0;
		nSelStore = 0;
		nSelStation = 0;
		nSelMap = 0;
		bMoveFollow = 0;
		bAroundPoint = 0;
		bMoveCoord = 0;
		bMoveUpHorse = 0;
		bMoveKillMons = 0;
		nFollowDist = 0;
		nMoveMapId = 0;
		nPointX = 0;
		nPointY = 0;
		szMoveMap[0] = 0;
		szFollName[0] = 0;
		nCoordCount = 0;
		bEncircle = 0;
		bDaTau = 0;
		for (int dti = 0; dti < 6; ++dti)
			bDTType[dti] = 0;
		nDTSkipMode = 0;
		nDTCancelMode = 0;
		nDTReward1 = 0;
		nDTReward2 = 0;
		bDTUseBox = 0;
		bDTTrainAfter = 0;
		nDTWDMoney = 0;
		bDTLenhBai = 0;
		bDTMuaSap = 0;
		nDTMaxMua = 0;
		bTongKim = 0;
		for (int tki = 0; tki < 4; ++tki)
			bTKGio[tki] = 0;
		nTKSom = 0;
		nTKLech = 0;
		nTKPhe = 0;
		nTKMuaMau = 0;
		nTKSoBinh = 0;
		bTKThuoc = 0;
		nTKThuocSel = 0;
		nTKUuTien = 0;
		bTKVeCho = 0;
		bLienDau = 0;
		for (int ldi = 0; ldi < 2; ++ldi)
		{
			bLDKhung[ldi] = 0;
			nLDGio[ldi] = 0;
			nLDPhut[ldi] = 0;
		}
		nLDLuot = 0;
		nLDPhutLuot = 0;
		nLDSom = 0;
		nLDLech = 0;
		bLDTuLap = 0;
		szLDTen[0] = 0;
		bLDCatDoCam = 0;
		bLDNhanThuong = 0;
		nLDVeThanh = 0;
		bLDPhuVe = 0;
		bLDParty = 0;
		nLDThanhBD = 7;
		nLDPtCount = 0;
		szLDPtName[0][0] = 0;
		bHDBachNhan = 0;
		nHDBNGio = 12;
		nHDBNPhut = 5;
		nHDBNCay = 0;
		nHDBNMode = 0;
		nHDBNDai = 0;
		bHDBNBuff = 1;
		bHDBangChien = 0;
		nHDBCGio = 20;
		nHDBCPhut = 25;
		nHDLech = 0;
	}
};

struct IPCGameLoop : public SharedState
{
	autoData setting;
};

struct IPCHideGame : public SharedState
{
	BOOL bHide;
};

struct IPCAutoLogin : public SharedState
{
	char szPassword[64];
	char szAccount[32];
	char szName[32];
	UINT dwID;
	short nSelSvGroup;
	short nSelServer;
};
//restore old padding
#pragma pack(pop, enter_nopadding)
#endif
