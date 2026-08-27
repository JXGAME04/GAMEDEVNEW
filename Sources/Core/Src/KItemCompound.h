//---------------------------------------------------------------------------
// KItemCompound.h - HE LO REN ([hop thanh] / compound)
//
// VIET MOI HOAN TOAN, dich nguoc 100% tu ban Linux (JX2/Kiem The):
//     D:\ServerLinux\server1\script\item\compound\*.lua
//     D:\ServerLinux\server1\script\item\itemvalue\*.lua
//     D:\ServerLinux\server1\settings\item\compoundscript.txt
//     D:\ServerLinux\server1\settings\item\itemvaluescript.txt
//
// KHONG tai dung khoi ma lo ren cu (da chet) cua du an JX1.
//
// Mo hinh cua ban goc:
//   - moi thao tac lo ren la mot COMPOUND_TYPE (0..8) tro toi mot kich ban Lua.
//   - kich ban do hien thuc ham Compound(matBatBuoc, matTuChon, bXemTruoc)
//     va tra ve (chi so vat pham sinh ra, ma ket qua).
//   - lua chon san pham dua tren GIA TRI VAT PHAM (item value): tong gia tri
//     nguyen lieu quyet dinh xac suat ra tung san pham dich.
//---------------------------------------------------------------------------
#ifndef KItemCompoundH
#define KItemCompoundH

#ifdef _SERVER

#include <vector>

//---------------------------------------------------------------------------
// Ma ket qua - PHAI khop 1-1 voi compound_header.lua (ban Linux, dong 30-39).
// Ban goc ghi ro: "以下各操作结果值必须和程序中GameDataDef.h里
// FOUNDRY_RESULT_TYPE枚举值一一对应" = cac gia tri nay phai trung khit voi
// enum FOUNDRY_RESULT_TYPE ben C++.
//---------------------------------------------------------------------------
enum FOUNDRY_RESULT_TYPE
{
	FOUNDRY_RESULT_UNKNOWN			= -1,	// loi khong ro
	FOUNDRY_RESULT_SUCCEED			= 0,	// thanh cong
	FOUNDRY_RESULT_FAIL				= 1,	// that bai (do xac suat)
	FOUNDRY_RESULT_NO_MONEY			= 2,	// khong du tien
	FOUNDRY_RESULT_LEVEL_FULL		= 3,	// nguyen lieu da o cap cao nhat
	FOUNDRY_RESULT_LACK_RESOURCE	= 4,	// thieu nguyen lieu
	FOUNDRY_RESULT_LEVEL_ERROR		= 5,	// sai cap nguyen lieu
	FOUNDRY_RESULT_MAGIC_ERROR		= 6,	// sai thuoc tinh phep thuat
	FOUNDRY_RESULT_SERIES_ERROR		= 7,	// sai ngu hanh
	FOUNDRY_RESULT_RULE_ERROR		= 8,	// nguyen lieu khong dung luat
};

//---------------------------------------------------------------------------
// COMPOUND_TYPE - tra tu settings\item\compoundscript.txt.
// Ban Linux dung dung 9 dong (0..8); giu nguyen y nghia.
//---------------------------------------------------------------------------
enum COMPOUND_TYPE
{
	COMPOUND_XUANJING			= 0,	// hop thanh Huyen Tinh
	COMPOUND_ORE_UPGRADE_1		= 1,	// nang cap khoang (the 1)
	COMPOUND_ORE_UPGRADE_2		= 2,	// nang cap khoang (the 2)
	COMPOUND_MAGIC_DISTILL		= 3,	// chiet xuat thuoc tinh
	COMPOUND_EQUIP_COMPOUND		= 4,	// DUC TRANG BI TIM (ep do tim)
	COMPOUND_EQUIP_ENCHASE		= 5,	// kham nam
	COMPOUND_ATLAS				= 6,	// DO PHO HOANG KIM (ep HK mon phai)
	COMPOUND_FANTASYGOLD		= 7,	// nang cap Huyen Kim
	COMPOUND_FANTASYGOLD_ESSENCE= 8,	// nang cap Huyen Kim Chi Tinh
	COMPOUND_TYPE_COUNT			= 9,
};

//---------------------------------------------------------------------------
// Pham chat vat pham theo QUY UOC BAN LINUX.
// JX1 luu khai niem nay o m_CommonAttrib.nItemNature nhung DANH SO KHAC:
//     Linux 0 (thuong)        <-> JX1 NATURE_NORMAL = 0
//     Linux 1 (hoang kim)     <-> JX1 NATURE_GOLD   = 2
//     Linux 2 (kham nam duoc) <-> JX1 NATURE_VIOLET = 1
// Moi cho trao doi voi Lua deu phai di qua hai ham doi duoi day.
//---------------------------------------------------------------------------
enum ITEM_QUALITY_LINUX
{
	ITEMQUALITY_NORMAL		= 0,
	ITEMQUALITY_GOLD		= 1,	// hoang kim
	ITEMQUALITY_ENCHASABLE	= 2,	// kham nam duoc (do tim)
	ITEMQUALITY_PLATINA		= 4,	// bach kim
};

int  g_QualityLinuxToNature(int nQuality);	// Linux -> JX1 nItemNature
int  g_QualityNatureToLinux(int nNature);	// JX1 nItemNature -> Linux

//---------------------------------------------------------------------------
// Mot dong cua itemvaluescript.txt.
//
// Cot de TRONG = khop moi gia tri. Bo nhi phan Linux thu bang SAU DAU:
//   0x081539A1..0x081539D1  test esi,esi / js   -> chi so AM la "bat ky".
// Vi vay o rong duoc quy ve -1, va phep thu la  (nX >= 0 && nX != giaTri).
//---------------------------------------------------------------------------
#define KVR_ANY		(-1)

struct KItemValueRule
{
	int		nQuality;
	int		nGenre;
	int		nDetailType;
	int		nParticular;
	int		nLevel;
	int		nSeries;
	bool	bHasFixedValue;		// cot ITEMVALUE co so cung
	double	dFixedValue;
	char	szScript[256];		// cot ITEMVALUE_SCRIPT (rong = khong co)
};

//---------------------------------------------------------------------------
// KFoundryResDemand - DIEU KIEN NGUYEN LIEU, doc tu settings\item\foundryresdemand.ini
//
// Vi sao phai co lop nay: kich ban Lua cua ban goc CO Y khong kiem nguyen lieu.
// Vi du xuanjing_compound.lua chi co "function verifySrcItems() return
// RESULT_SUCCEED end" - khong mot phep kiem nao. Toan bo viec loc nam o C++,
// chay TRUOC khi goi Lua. Thieu lop nay thi he lo ren nhan bat ky vat pham gi.
//
// Do tu nhi phan Linux (jx_linux_y):
//   0x0814F050  ham nap: lap "ResScheme_%d", roi 21 khoa theo bang 0x0825EC20
//   0x0814EEC0  doc mot tiet doan "%s_%d": Quality/Genre/DetailType/PtcType/
//               Stackable, MOI TRUONG MAC DINH -1 = KHONG KIEM (ky tu dai dien)
//   0x0814ECC0  phep so khop: 4 truong voi -1 la dai dien, KEM mot chot chan
//               vat pham DANG XEP CHONG (0x0814ED29..0x0814ED6E)
//   0x0825EBC0  bang 9 ham kiem, moi COMPOUND_TYPE mot ham
//
// KHAC BAN GOC MOT DIEM (co chu y): ban goc doi chieu theo O CO DINH cua cua so
// lo ren (item[0] phai la trang bi, item[1] phai la Huyen Tinh...). JX1 nhan
// nguyen lieu qua hop trao doi nen KHONG co thu tu o. Vi vay o day doi:
//     - moi khoa bat buoc cua thao tac phai co it nhat mot nguyen lieu khop
//     - va moi nguyen lieu phai khop it nhat mot khoa cua thao tac
// Hieu qua tuong duong, va chat hon o cho no loai ca nguyen lieu thua.
//---------------------------------------------------------------------------
enum FOUNDRY_RES_KEY				// DUNG thu tu 21 khoa cua ban goc
{
	FRK_CompoundCryolite_Ring = 0,
	FRK_CompoundCryolite_Necklace,
	FRK_CompoundCryolite_Pendant,
	FRK_UpgradeCryolite,
	FRK_UpgradePropMine,
	FRK_Distill_Equip,
	FRK_Distill_Cryolite,
	FRK_Distill_OrgMine,
	FRK_Distill_EnhanceItem,
	FRK_Forge_Equip,
	FRK_Forge_Cryolite,
	FRK_Enchase_Equip,
	FRK_Enchase_Cryolite,
	FRK_Enchase_PropMine,
	FRK_Enchase_EnhanceItem,
	FRK_CompoundGold_Cryolite,
	FRK_CompoundGold_Atlas,
	FRK_CompoundGold_Material,
	FRK_CompoundGold_EnhanceItem,
	FRK_UpgradeFantasyGold,
	FRK_UpgradeFantasyGoldEssence,
	FRK_COUNT,
};

#define FRD_BOQUA	(-1)			// truong vang mat trong ini = khong kiem

struct KResDemand					// mot tiet doan [Ten_i] cua ini
{
	int		nQuality;
	int		nGenre;
	int		nDetailType;
	int		nPtcType;
	int		nStackable;
};

class KFoundryResDemand
{
public:
	KFoundryResDemand();

	BOOL	Init(int nScheme = 1);
	void	Clear();
	BOOL	IsReady() const { return m_bReady; }

	// Tra ve FOUNDRY_RESULT_SUCCEED neu bo nguyen lieu hop le,
	// FOUNDRY_RESULT_LACK_RESOURCE neu thieu, FOUNDRY_RESULT_RULE_ERROR neu
	// co vat pham khong duoc phep.
	int		Check(int nCompoundType, const int* pnItem, int nCount) const;

	// Kiem O TU CHON (o "tang ty le"). Ban goc khong kiem o may chu - day la
	// cho CHAT HON co y, vi JX1 khong co cua so lo ren de loc phia client.
	int		CheckTuChon(int nCompoundType, const int* pnItem, int nCount) const;

	// So O NGUYEN LIEU CHINH toi da cua mot thao tac. Doc thang tu bang
	// 0x08257F80 cua ban goc: {3,3,3,3,2,3,8,3,3}. Thieu chan nay thi nguoi
	// choi nhet duoc toi COMPOUND_MAX_MATERIAL (32) mon de bom gia tri.
	static int	LaySoO(int nCompoundType);

private:
	BOOL	KhopMotKhoa(int nItemIdx, int nKey) const;
	static BOOL KhopTietDoan(const KResDemand& D, int nItemIdx);

private:
	BOOL					m_bReady;
	std::vector<KResDemand>	m_aryKhoa[FRK_COUNT];	// tiet doan duoc chap nhan
	BOOL					m_bCoKhoa[FRK_COUNT];	// khoa co mat trong so do
};

extern KFoundryResDemand	g_FoundryResDemand;

//---------------------------------------------------------------------------
class KItemCompound
{
public:
	KItemCompound();
	~KItemCompound();

	BOOL	Init();
	void	Clear();
	BOOL	IsReady() const { return m_bReady; }

	// Duong vao chinh: chay mot thao tac lo ren.
	//   nPlayerIdx      chi so nguoi choi
	//   nCompoundType   0..8
	//   pnNec/nNecCount mang chi so vat pham nguyen lieu BAT BUOC
	//   pnAlt/nAltCount mang chi so vat pham nguyen lieu TU CHON
	//   bPreview        1 = chi xem truoc ty le, khong tieu nguyen lieu
	//   pnResultItemIdx (ra) chi so vat pham sinh ra, -1 neu khong co
	// Tra ve: FOUNDRY_RESULT_TYPE
	int		Compound(int nPlayerIdx, int nCompoundType,
					 const int* pnNec, int nNecCount,
					 const int* pnAlt, int nAltCount,
					 BOOL bPreview, int* pnResultItemIdx);

	// ITEM_CalcItemValue dang 2 doi: tinh gia tri cua MOT VAT PHAM CO THAT.
	double	CalcItemValueByIndex(int nItemIdx, const char* szParam);

	// ITEM_CalcItemValue dang 10 doi: tinh gia tri cua MOT MO TA vat pham.
	//   pnMagLvl : mang MagicLevel (6 phan tu, -1 = o kham nam rong)
	//   pnMagic  : mang phang MagicID+3 gia tri, 24 phan tu (6 x 4); NULL = khong co
	double	CalcItemValueByInfo(int nVer, int nQuality, int nGenre, int nDetailType,
								int nParticular, int nLevel, int nSeries, int nLuck,
								const int* pnMagLvl, int nMagLvlCount,
								const int* pnMagic, int nMagicCount,
								const char* szParam);

	const char* GetCompoundScript(int nCompoundType) const;

private:
	BOOL	LoadCompoundScript();
	BOOL	LoadItemValueScript();
	static BOOL KhopLuat(const KItemValueRule& R, int nQuality, int nGenre,
						 int nDetailType, int nParticular, int nLevel, int nSeries);

private:
	BOOL							m_bReady;
	char							m_szCompoundScript[COMPOUND_TYPE_COUNT][256];
	std::vector<KItemValueRule>		m_vRules;
	int								m_nCalcDepth;	// chan de quy vo tan
};

extern KItemCompound	g_ItemCompound;

// Duong dan bang theo phien ban vat pham: \settings\item\%03d\<ten tep>
void	g_MakeItemFilePath(int nItemVer, const char* szFileName, char* szOut, int nOutSize);

// Phien ban vat pham moi nhat may chu dang dung (ITEM_GetLatestItemVersion).
int		g_GetLatestItemVersion();

#endif	// _SERVER

#endif	// KItemCompoundH
