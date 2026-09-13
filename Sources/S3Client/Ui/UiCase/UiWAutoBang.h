// [ANDROID 11/09 WAUTO B2] BANG dieu khien cho trang noi dung cua khung WAuto trong game - SINH BANG MAY (android/sinh_bocuc_wauto.py)
// tu WAutoUI/WAuto.rc + SaveRoleData + ipc_shared.h. KHONG SUA TAY: chay lai bo sinh. Chuoi: TCVN3.
// Doc boi UiWAutoTrang.cpp (chi Android).
#ifndef UiWAutoBang_H
#define UiWAutoBang_H
#ifdef JX_MOBILE

#define WA_MUC_TICK	1
#define WA_MUC_NHAP	2
#define WA_MUC_CHON	3
#define WA_MUC_NHAN	4
#define WA_MUC_NUT	5
#define WA_MUC_DSACH	6
#define WA_MUC_SOLIEU	7	// o chu dien luc chay (so lieu nhan vat / toa do dang dat) - nChiSo = WA_SL_*

// [WAUTO 12/09] loc danh sach chieu: game tra MOT danh sach phang (GetAllSkillByType), WAuto.exe che thanh 7
// danh sach con (WAuto.cpp:3057-3097) -> loc lai o day cho dung, khong thi chon duoc chieu sai khe.
#define WA_LC_KHONG	0
#define WA_LC_K	1	// moi chieu chu dong TRU vong sang (SkillKAr)
#define WA_LC_S	2	// buff phe ta: (bState || nStyle==2) && bAlly && !bAura (SkillSAr)
#define WA_LC_SE	3	// trang thai len dich: bState && !bAlly (SkillSEAr)
#define WA_LC_BP	4	// chieu danh: nStyle <= 1 && !bAlly (SkillBAr / SkillPAr)
#define WA_LC_A	5	// vong sang: bAura (SkillAAr)

// [WAUTO 12/09] ma so lieu dien luc chay (WA_MUC_SOLIEU)
#define WA_SL_SINHLUC	1
#define WA_SL_NOILUC	2
#define WA_SL_THELUC	3
#define WA_SL_BANDO	4
#define WA_SL_TOADO	5
#define WA_SL_DANGCAP	6
#define WA_SL_KINHNGHIEM	7
#define WA_SL_MAP_AP	8	// ban do da dat cho may di chuyen (autoData.szMoveMap)
#define WA_SL_DIEM_X	9
#define WA_SL_DIEM_Y	10

// [WAUTO 12/09] ma viec (nut / hop chon / danh sach co logic rieng) - khop VIEC trong sinh_bang_wauto.py
#define WA_V_KHONG	0
#define WA_V_LAY_BANDO	1
#define WA_V_LAY_DIEM	2
#define WA_V_THEO_SAU	3
#define WA_V_DS_TOADO	4
#define WA_V_DS_LOC	5
#define WA_V_DS_KHONGNHAT	6
#define WA_V_DS_TODOI	7
#define WA_V_DS_NGUHANH	8
#define WA_V_UUTIEN_GAN	9
#define WA_V_DS_LIENDAU	10
#define WA_V_TK_RUONG	11
#define WA_V_ST_BOSS	12
#define WA_V_UUTIEN_NGU	13
#define WA_KIEU_INT	0
#define WA_KIEU_SHORT	1
#define WA_KIEU_UINT	2
#define WA_KIEU_CHUOI	3
#define WA_KIEU_KHONG	4
#define WA_NGUON_TINH	0
#define WA_NGUON_CHIEU	1
#define WA_NGUON_KHAC	2
#define WA_SIZEOF_AUTODATA	7644

struct WAUiMuc
{
	unsigned char	nLoai;		// WA_MUC_*
	unsigned char	nKhe;		// khe trong kho widget cung loai (Tick%d / Nhap%d / Chon%d / Nut%d trong ini)
	unsigned char	nKheNhan;	// khe nhan chu di kem (o tick), 255 = khong
	unsigned char	nChiSo;		// chi so trong mang (bTKGio[4]...)
	int				nOff;		// offset trong autoData (-1 = khong noi)
	unsigned char	nKieu;		// WA_KIEU_*
	unsigned short	nCo;		// co truong (byte) - chuoi: do dai toi da
	const char* const*	pLuaChon;	// hop chon: cac dong (TCVN3), NULL neu nguon dong
	unsigned char	nLuaChon;
	unsigned char	nNguon;		// WA_NGUON_*
	unsigned char	nLocChieu;	// WA_LC_* (hop chon nguon CHIEU)
	unsigned char	nViec;		// WA_V_*
	const char*		szIdc;		// ten IDC goc ben WAuto.exe (de doi chieu / nhat ky / tim ghi chu)
};
struct WAUiTab { const WAUiMuc* pMuc; int nMuc; const char* szIni; int nHang; int nCot; };

static const char* const s_LuaChon_0_0[] = { "Kªnh phô cËn", "Kªnh thÕ giíi" };
static const WAUiMuc s_Muc_0[] = {
	{ 7, 1, 255, 1, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_NL" },
	{ 7, 4, 255, 2, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_NM" },
	{ 7, 5, 255, 4, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_MN" },
	{ 7, 7, 255, 3, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_NS" },
	{ 7, 8, 255, 5, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_MC" },
	{ 7, 10, 255, 6, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_NLV" },
	{ 7, 12, 255, 7, -1, 4, 0, NULL, 0, 0, 0, 0, "IDC_STRING_0_NE" },
	{ 1, 0, 13, 0, 80, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_ETP" },
	{ 1, 1, 14, 0, 84, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_ENW" },
	{ 1, 2, 15, 0, 88, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_ETM" },
	{ 2, 0, 255, 0, 92, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_0_ETH" },
	{ 2, 1, 255, 0, 96, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_0_ETM" },
	{ 1, 3, 18, 0, 100, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_CH" },
	{ 3, 0, 18, 0, 104, 0, 4, s_LuaChon_0_0, 2, 0, 0, 0, "IDC_COMBO_0_CH" },
	{ 2, 2, 255, 0, 108, 3, 80, NULL, 0, 0, 0, 0, "IDC_EDITOR_0_CH" },
	{ 1, 4, 19, 0, 7532, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_NTS" },
	{ 1, 5, 20, 0, 7536, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_0_MIS" },
	{ 2, 3, 255, 0, 7540, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_0_MIS" },
};
static const char* const s_LuaChon_1_0[] = { "HÕt chç 1 «", "HÕt chç 4 « (2x2)", "HÕt chç 6 « (2x3)", "HÕt chç 8 « (2x4)" };
static const WAUiMuc s_Muc_1[] = {
	{ 1, 0, 3, 0, 0, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_PL" },
	{ 2, 0, 255, 0, 8, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PL1" },
	{ 2, 1, 255, 0, 12, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PL2" },
	{ 2, 2, 255, 0, 16, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PL3" },
	{ 1, 1, 4, 0, 4, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_PM" },
	{ 2, 3, 255, 0, 20, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PM1" },
	{ 2, 4, 255, 0, 24, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PM2" },
	{ 2, 5, 255, 0, 28, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_PM3" },
	{ 1, 2, 5, 0, 32, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPL" },
	{ 2, 6, 255, 0, 60, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_TPL" },
	{ 1, 3, 6, 0, 36, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPM" },
	{ 2, 7, 255, 0, 64, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_TPM" },
	{ 1, 4, 7, 0, 40, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPNL" },
	{ 1, 5, 8, 0, 44, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPNM" },
	{ 1, 6, 9, 0, 48, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPFI" },
	{ 1, 7, 10, 0, 52, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPFM" },
	{ 2, 8, 255, 0, 68, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_TPNFM" },
	{ 3, 0, 255, 0, 76, 0, 4, s_LuaChon_1_0, 4, 0, 0, 0, "IDC_COMBO_1_TPFI" },
	{ 1, 8, 12, 0, 56, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_TPDI" },
	{ 2, 9, 255, 0, 72, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_TPNDI" },
	{ 1, 9, 13, 0, 188, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UL" },
	{ 1, 10, 14, 0, 212, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UO" },
	{ 1, 11, 15, 0, 192, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UP" },
	{ 1, 12, 16, 0, 196, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UE" },
	{ 1, 13, 17, 0, 200, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_US" },
	{ 1, 14, 18, 0, 204, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UB" },
	{ 2, 10, 255, 0, 216, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_1_UB" },
	{ 1, 15, 19, 0, 208, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UBT" },
	{ 1, 16, 20, 0, 376, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UBCL" },
	{ 1, 17, 21, 0, 380, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_UBCLC" },
	{ 1, 18, 22, 0, 400, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_1_RV" },
};
static const char* const s_LuaChon_2_0[] = { "Tù ®éng", "Lªn ngùa", "Xuèng ngùa" };
static const char* const s_LuaChon_2_1[] = { "Kh«ng thiÕt lËp", "Tr¸nh boss", "¦u tiªn boss", "ChØ ®¸nh boss" };
static const char* const s_LuaChon_2_2[] = { "§¸nh tr¶", "Thæ ®Þa phï", "Tho¸t game" };
static const char* const s_LuaChon_2_3[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_4[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_5[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_6[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_7[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_8[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_9[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_10[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_2_11[] = { "Kh«ng thiÕt lËp" };
static const WAUiMuc s_Muc_2[] = {
	{ 1, 0, 0, 0, 220, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_2_F" },
	{ 2, 0, 255, 0, 236, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_VS" },
	{ 3, 0, 0, 0, 248, 0, 4, s_LuaChon_2_0, 3, 0, 0, 0, "IDC_COMBO_2_F" },
	{ 1, 1, 1, 0, 224, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_2_N" },
	{ 2, 1, 255, 0, 240, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_NV" },
	{ 3, 1, 1, 0, 252, 0, 4, s_LuaChon_2_1, 4, 0, 0, 0, "IDC_COMBO_2_N" },
	{ 1, 2, 2, 0, 228, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_2_FR" },
	{ 2, 2, 255, 0, 244, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_FRV" },
	{ 3, 2, 2, 0, 256, 0, 4, s_LuaChon_2_2, 3, 0, 0, 0, "IDC_COMBO_2_FR" },
	{ 1, 3, 3, 0, 232, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_2_GB" },
	{ 3, 3, 4, 0, 260, 0, 4, s_LuaChon_2_3, 1, 1, 2, 0, "IDC_COMBO_2_SP1" },
	{ 3, 4, 5, 0, 264, 0, 4, s_LuaChon_2_4, 1, 1, 2, 0, "IDC_COMBO_2_SP2" },
	{ 3, 5, 6, 0, 268, 0, 4, s_LuaChon_2_5, 1, 1, 2, 0, "IDC_COMBO_2_SP3" },
	{ 3, 6, 7, 0, 280, 0, 4, s_LuaChon_2_6, 1, 1, 4, 0, "IDC_COMBO_2_SB" },
	{ 2, 3, 255, 0, 304, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_SLS" },
	{ 3, 7, 8, 0, 296, 0, 4, s_LuaChon_2_7, 1, 1, 4, 0, "IDC_COMBO_2_SLS" },
	{ 2, 4, 255, 0, 308, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_SMS" },
	{ 3, 8, 9, 0, 300, 0, 4, s_LuaChon_2_8, 1, 1, 4, 0, "IDC_COMBO_2_SMS" },
	{ 3, 9, 10, 0, 288, 0, 4, s_LuaChon_2_9, 1, 1, 5, 0, "IDC_COMBO_2_SA1" },
	{ 3, 10, 11, 0, 292, 0, 4, s_LuaChon_2_10, 1, 1, 5, 0, "IDC_COMBO_2_SA2" },
	{ 2, 5, 255, 0, 316, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_2_SC" },
	{ 3, 11, 13, 0, 312, 0, 4, s_LuaChon_2_11, 1, 1, 4, 0, "IDC_COMBO_2_SC" },
};
static const WAUiMuc s_Muc_3[] = {
	{ 1, 0, 0, 0, 6520, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_FL" },
	{ 3, 0, 0, 0, 6588, 3, 32, NULL, 0, 2, 0, 3, "IDC_COMBO_3_FL" },
	{ 2, 0, 255, 0, 6540, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_3_FL" },
	{ 7, 3, 255, 8, 6556, 3, 32, NULL, 0, 0, 0, 0, "IDC_STRING_3_CM" },
	{ 5, 0, 255, 0, -1, 4, 0, NULL, 0, 2, 0, 1, "IDC_BTN_3_MG" },
	{ 1, 1, 4, 0, 6524, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_PT" },
	{ 7, 5, 255, 9, 6548, 0, 4, NULL, 0, 0, 0, 0, "IDC_STRING_3_PTX" },
	{ 7, 6, 255, 10, 6552, 0, 4, NULL, 0, 0, 0, 0, "IDC_STRING_3_PTY" },
	{ 5, 1, 255, 0, -1, 4, 0, NULL, 0, 2, 0, 2, "IDC_BTN_3_PT" },
	{ 1, 2, 7, 0, 6528, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_CD" },
	{ 6, 2, 8, 0, -1, 4, 0, NULL, 0, 0, 0, 4, "IDC_LIST_3_CD" },
	{ 1, 3, 9, 0, 6532, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_UH" },
	{ 1, 4, 10, 0, 6536, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_MT" },
	{ 1, 5, 11, 0, 6816, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_3_EN" },
};
static const char* const s_LuaChon_4_0[] = { "NhÆt tÊt c¶", "TiÒn vµ ®Æc phÈm", "TiÒn, ®Æc phÈm, ®å mµu", "TiÒn vµ ®å 1 « mµu", "TiÒn vµ ®å 1-4 « mµu", "ChØ nhÆt tiÒn" };
static const WAUiMuc s_Muc_4[] = {
	{ 1, 0, 0, 0, 404, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_PI" },
	{ 1, 1, 1, 0, 408, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_PIF" },
	{ 2, 0, 255, 0, 412, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_4_PIV" },
	{ 1, 2, 2, 0, 416, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_IC" },
	{ 3, 0, 3, 0, 420, 0, 4, s_LuaChon_4_0, 6, 0, 0, 0, "IDC_COMBO_4_TY" },
	{ 1, 3, 4, 0, 424, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_NOP" },
	{ 5, 0, 255, 0, -1, 4, 0, NULL, 0, 2, 0, 6, "IDC_BTN_4_NOP" },
	{ 1, 4, 5, 0, 5232, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_FT" },
	{ 6, 1, 6, 0, -1, 4, 0, NULL, 0, 0, 0, 5, "IDC_LIST_4_FT" },
	{ 1, 5, 7, 0, 5560, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_PRZ" },
	{ 2, 1, 255, 0, 5564, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_4_PRZ" },
	{ 1, 6, 8, 0, 5568, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_LV" },
	{ 2, 2, 255, 0, 5572, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_4_LV" },
	{ 1, 7, 9, 0, 5576, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_RG" },
	{ 2, 3, 255, 0, 5580, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_4_RG" },
	{ 1, 8, 10, 0, 5584, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_ARI" },
	{ 1, 9, 11, 0, 5588, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_4_ARB" },
};
static const char* const s_LuaChon_5_0[] = { "B¸n gi÷ läc ®å", "B¸n hÕt ®å" };
static const char* const s_LuaChon_5_1[] = { "Kim S¸ng D­îc (tiÓu)", "Kim S¸ng D­îc (trung)", "Kim S¸ng D­îc (®¹i)", "Håi Thiªn ®¬n", "Cöu ChuyÓn Hoµn Hån ®¬n", "Thõa Tiªn MËt (tiÓu)", "Thõa Tiªn MËt (trung)", "Thõa Tiªn MËt (®¹i)", "ThÊt KhiÕu Bæ t©m ®¬n", "Ngò Hoa Ngäc Lé Hoµn" };
static const char* const s_LuaChon_5_2[] = { "Ng­ng ThÇn ®¬n (tiÓu)", "Ng­ng ThÇn ®¬n (trung)", "Ng­ng ThÇn ®¬n (®¹i)", "§¹i Bæ t¸n", "¤ ®Çu Hoµn thÇn ®¬n", "Thõa Tiªn MËt (tiÓu)", "Thõa Tiªn MËt (trung)", "Thõa Tiªn MËt (®¹i)", "ThÊt KhiÕu Bæ t©m ®¬n", "Ngò Hoa Ngäc Lé Hoµn" };
static const char* const s_LuaChon_5_3[] = { "Thanh Ng­u Gi¶i ®éc t¸n", "Ho¾c H­¬ng ChÝnh khÝ hoµn", "Tam ThuËn t¸n", "Ng©n KiÒu Khø ®éc ®¬n", "Long §µm Hãa §éc ®¬n" };
static const char* const s_LuaChon_5_4[] = { "CÊt r­¬ng 1", "CÊt r­¬ng 1,2", "CÊt r­¬ng 1,2,3" };
static const char* const s_LuaChon_5_5[] = { "Quay l¹i ®iÓm cò", "N¬i ®· ®i qua 1", "N¬i ®· ®i qua 2", "N¬i ®· ®i qua 3", "§Õn n¬i nhiÖm vô D· TÈu" };
static const char* const s_LuaChon_5_6[] = { "H¾c Sa ®éng (90)", "Tr­êng B¹ch S¬n B¾c (90)", "Tr­êng B¹ch S¬n Nam (90)", "Kháa Lang ®éng (90)", "Sa M¹c Mª Cung 1 (90)", "Sa M¹c Mª Cung 2 (90)", "Sa M¹c Mª Cung 3 (90)", "Phong L¨ng §é (90)", "M¹c Cao QuËt (90)", "D­îc V­¬ng §éng tÇng 4 (90)", "TiÕn Cóc ®éng (90)", "C¸n Viªn ®éng (90)", "TuyÕt B¸o ®éng tÇng 8 (90)", "Sa M¹c §Þa BiÓu (80)", "Thanh Khª §éng (80)", "Ch©n nói Tr­êng B¹ch (80)", "L­ìng Thñy §éng (80)", "L©m Du Quan (70)", "L·o Hæ §éng (70)", "TÇn L¨ng tÇng 2 (70)", "T­¬ng D­¬ng MËt §¹o (60)", "Hoµnh S¬n Ph¸i (60)", "Thiªn TÇm Th¸p tÇng 3 (60)", "NghiÖt Long §éng (50)", "Thiªn TÇm Th¸p (50)", "Thanh Thµnh S¬n (40)", "§iÓm Th­¬ng S¬n (40)", "Vò Di S¬n (30)", "Thæ PhØ §éng (30)", "KiÕm C¸c T©y Nam (20)", "TÇn L¨ng tÇng 1 (20)" };
static const WAUiMuc s_Muc_5[] = {
	{ 1, 0, 0, 0, 6396, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_R" },
	{ 1, 1, 1, 0, 6400, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_S" },
	{ 3, 0, 1, 0, 6408, 0, 4, s_LuaChon_5_0, 2, 0, 0, 0, "IDC_COMBO_5_S" },
	{ 1, 2, 2, 0, 6404, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_SH" },
	{ 1, 3, 3, 0, 6412, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_RP" },
	{ 1, 4, 4, 0, 6416, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_RPF" },
	{ 1, 5, 5, 0, 6420, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_WD" },
	{ 2, 0, 255, 0, 6424, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_WD" },
	{ 2, 1, 255, 0, 6468, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_PW" },
	{ 1, 6, 7, 0, 6444, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_BL" },
	{ 3, 1, 7, 0, 6448, 0, 4, s_LuaChon_5_1, 10, 0, 0, 0, "IDC_COMBO_5_BL" },
	{ 2, 2, 255, 0, 6468, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_BL" },
	{ 1, 7, 8, 0, 6452, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_BM" },
	{ 3, 2, 8, 0, 6456, 0, 4, s_LuaChon_5_2, 10, 0, 0, 0, "IDC_COMBO_5_BM" },
	{ 2, 3, 255, 0, 6472, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_BM" },
	{ 1, 8, 9, 0, 6460, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_BP" },
	{ 3, 3, 9, 0, 6464, 0, 4, s_LuaChon_5_3, 5, 0, 0, 0, "IDC_COMBO_5_BP" },
	{ 2, 4, 255, 0, 6476, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_BP" },
	{ 1, 9, 10, 0, 6480, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_BTP" },
	{ 2, 5, 255, 0, 6496, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_BTP" },
	{ 1, 10, 11, 0, 6484, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_SM" },
	{ 2, 6, 255, 0, 6500, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_5_SM" },
	{ 1, 11, 13, 0, 6504, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_SI" },
	{ 3, 4, 13, 0, 6508, 0, 4, s_LuaChon_5_4, 3, 0, 0, 0, "IDC_COMBO_5_SI" },
	{ 1, 12, 14, 0, 6488, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_GS" },
	{ 3, 5, 14, 0, 6512, 0, 4, s_LuaChon_5_5, 5, 0, 0, 0, "IDC_COMBO_5_GS" },
	{ 1, 13, 15, 0, 6492, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_5_GM" },
	{ 3, 6, 15, 0, 6516, 0, 4, s_LuaChon_5_6, 31, 0, 0, 0, "IDC_COMBO_5_GM" },
};
static const char* const s_LuaChon_6_0[] = { "Kh«ng tù mêi nhãm", "Mêi nhãm tÊt c¶" };
static const char* const s_LuaChon_6_1[] = { "Kh«ng tù vµo nhãm", "NhËn vµo tÊt c¶ nhãm" };
static const WAUiMuc s_Muc_6[] = {
	{ 3, 0, 255, 0, 5592, 0, 4, s_LuaChon_6_0, 2, 0, 0, 0, "IDC_COMBO_6_TI" },
	{ 3, 1, 255, 0, 5596, 0, 4, s_LuaChon_6_1, 2, 0, 0, 0, "IDC_COMBO_6_TJ" },
	{ 1, 0, 0, 0, 5600, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_6_TL" },
	{ 6, 0, 1, 0, -1, 4, 0, NULL, 0, 0, 0, 7, "IDC_LIST_6_NE" },
	{ 1, 1, 2, 0, 6376, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_6_LEA" },
	{ 2, 0, 255, 0, 6380, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_6_LN" },
	{ 2, 1, 255, 0, 6384, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_6_LM" },
	{ 1, 2, 5, 0, 6388, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_6_RM" },
	{ 2, 2, 255, 0, 6392, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_6_RM" },
};
static const char* const s_LuaChon_7_0[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_7_1[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_7_2[] = { "Kh«ng thiÕt lËp" };
static const WAUiMuc s_Muc_7[] = {
	{ 1, 0, 0, 0, 320, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_F" },
	{ 2, 0, 255, 0, 332, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_7_VS" },
	{ 1, 1, 2, 0, 336, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_N" },
	{ 2, 1, 255, 0, 340, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_7_N" },
	{ 1, 2, 3, 0, 344, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_FL" },
	{ 1, 3, 4, 0, 360, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_DV" },
	{ 1, 4, 5, 0, 348, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_FP" },
	{ 1, 5, 6, 0, 352, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_FM" },
	{ 1, 6, 7, 0, 356, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_H" },
	{ 1, 7, 9, 0, 384, 0, 4, NULL, 0, 0, 0, 9, "IDC_CHECKBOX_7_PYD" },
	{ 1, 8, 10, 0, 384, 0, 4, NULL, 0, 0, 0, 13, "IDC_CHECKBOX_7_PYS" },
	{ 5, 0, 255, 0, -1, 4, 0, NULL, 0, 2, 0, 8, "IDC_BTN_7_PYS" },
	{ 3, 0, 11, 0, 364, 0, 4, s_LuaChon_7_0, 1, 1, 3, 0, "IDC_COMBO_7_CS1" },
	{ 3, 1, 12, 0, 368, 0, 4, s_LuaChon_7_1, 1, 1, 3, 0, "IDC_COMBO_7_CS2" },
	{ 3, 2, 13, 0, 372, 0, 4, s_LuaChon_7_2, 1, 1, 3, 0, "IDC_COMBO_7_CS3" },
	{ 1, 9, 14, 0, 7640, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_7_KHIEN" },
};
static const char* const s_LuaChon_8_0[] = { "Treo (t¹m dõng)", "Hñy nhiÖm vô" };
static const char* const s_LuaChon_8_1[] = { "ChØ dïng l­ît hñy", "Cho phÐp hñy th­êng (reset chuçi)", "¦u tiªn 100 m¶nh SHXT" };
static const char* const s_LuaChon_8_2[] = { "Kinh nghiÖm", "B¹c", "NgÉu nhiªn" };
static const char* const s_LuaChon_8_3[] = { "§iÓm", "May m¾n (l­ît hñy)", "VËt phÈm" };
static const WAUiMuc s_Muc_8[] = {
	{ 1, 0, 0, 0, 6820, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_ON" },
	{ 1, 1, 1, 0, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T1" },
	{ 1, 2, 2, 1, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T2" },
	{ 1, 3, 3, 2, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T3" },
	{ 1, 4, 4, 3, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T4" },
	{ 1, 5, 5, 4, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T5" },
	{ 1, 6, 6, 5, 6824, 0, 24, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_T6" },
	{ 3, 0, 7, 0, 6848, 0, 4, s_LuaChon_8_0, 2, 0, 0, 0, "IDC_COMBO_8_SKIP" },
	{ 3, 1, 8, 0, 6852, 0, 4, s_LuaChon_8_1, 3, 0, 0, 0, "IDC_COMBO_8_CANCEL" },
	{ 3, 2, 9, 0, 6856, 0, 4, s_LuaChon_8_2, 3, 0, 0, 0, "IDC_COMBO_8_RW1" },
	{ 3, 3, 10, 0, 6860, 0, 4, s_LuaChon_8_3, 3, 0, 0, 0, "IDC_COMBO_8_RW2" },
	{ 1, 7, 11, 0, 6864, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_BOX" },
	{ 2, 0, 255, 0, 6872, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_8_WD" },
	{ 1, 8, 13, 0, 6876, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_LB" },
	{ 1, 9, 14, 0, 6880, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_MS" },
	{ 2, 1, 255, 0, 6884, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_8_MM" },
	{ 1, 10, 16, 0, 6868, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_8_TRAIN" },
};
static const char* const s_LuaChon_9_0[] = { "Phe Tèng", "Phe Kim", "Tù c©n b»ng (bªn Ýt ng­êi)" };
static const char* const s_LuaChon_9_1[] = { "Mua nhanh (®Çy tói)", "Mua theo sè l­îng", "Kh«ng mua" };
static const char* const s_LuaChon_9_2[] = { "TÊt c¶ thuèc ®ang cã", "ChØ thuèc t¨ng c«ng", "ChØ thuèc phßng thñ", "T¨ng c«ng + m¸u/tèc ®é" };
static const char* const s_LuaChon_9_3[] = { "§Þch gÇn nhÊt", "HiÖu óy / Phã T­íng / §¹i T­íng" };
static const char* const s_LuaChon_9_4[] = { "Ph­îng T­êng Phñ", "Thµnh §« Phñ", "§¹i Lý", "BiÖn Kinh Phñ", "T­¬ng D­¬ng Phñ", "D­¬ng Ch©u Phñ", "L©m An Phñ" };
static const char* const s_LuaChon_9_5[] = { "Trung t©m", "§«ng", "T©y", "Nam", "B¾c", "GÇn nhÊt (tù chän)" };
static const WAUiMuc s_Muc_9[] = {
	{ 1, 0, 0, 0, 6888, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_ON" },
	{ 1, 1, 2, 0, 6892, 0, 16, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_G1" },
	{ 1, 2, 3, 1, 6892, 0, 16, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_G2" },
	{ 1, 3, 4, 2, 6892, 0, 16, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_G3" },
	{ 1, 4, 5, 3, 6892, 0, 16, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_G4" },
	{ 2, 0, 255, 0, 6908, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_9_SOM" },
	{ 2, 1, 255, 0, 6912, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_9_LECH" },
	{ 3, 0, 8, 0, 6916, 0, 4, s_LuaChon_9_0, 3, 0, 0, 0, "IDC_COMBO_9_PHE" },
	{ 3, 1, 9, 0, 6920, 0, 4, s_LuaChon_9_1, 3, 0, 0, 0, "IDC_COMBO_9_MUA" },
	{ 2, 2, 255, 0, 6924, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_9_SB" },
	{ 1, 5, 11, 0, 6928, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_TH" },
	{ 3, 2, 12, 0, 6932, 0, 4, s_LuaChon_9_2, 4, 0, 0, 0, "IDC_COMBO_9_THS" },
	{ 3, 3, 13, 0, 6936, 0, 4, s_LuaChon_9_3, 2, 0, 0, 0, "IDC_COMBO_9_UT" },
	{ 3, 4, 14, 0, 7384, 0, 4, s_LuaChon_9_4, 7, 0, 0, 0, "IDC_COMBO_9_VE" },
	{ 1, 6, 15, 0, 7628, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_RUONG" },
	{ 3, 5, 16, 0, 7632, 0, 4, s_LuaChon_9_5, 6, 0, 0, 11, "IDC_COMBO_9_RH" },
	{ 1, 7, 17, 0, 7636, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_9_RCAT" },
};
static const char* const s_LuaChon_10_0[] = { "Ph­îng T­êng Phñ", "Thµnh §« Phñ", "§¹i Lý", "BiÖn Kinh Phñ", "T­¬ng D­¬ng Phñ", "D­¬ng Ch©u Phñ", "L©m An Phñ", "Thµnh ®ang ®øng (kh«ng tù ®i)" };
static const char* const s_LuaChon_10_1[] = { "Ph­îng T­êng Phñ", "Thµnh §« Phñ", "§¹i Lý", "BiÖn Kinh Phñ", "T­¬ng D­¬ng Phñ", "D­¬ng Ch©u Phñ", "L©m An Phñ", "Trë l¹i thµnh lóc ®i" };
static const WAUiMuc s_Muc_10[] = {
	{ 1, 0, 0, 0, 6944, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_ON" },
	{ 1, 1, 1, 0, 6948, 0, 8, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_K1" },
	{ 2, 0, 255, 0, 6956, 0, 8, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_G1" },
	{ 2, 1, 255, 0, 6964, 0, 8, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_P1" },
	{ 1, 2, 3, 1, 6948, 0, 8, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_K2" },
	{ 2, 2, 255, 1, 6956, 0, 8, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_G2" },
	{ 2, 3, 255, 1, 6964, 0, 8, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_P2" },
	{ 2, 4, 255, 0, 6972, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_LUOT" },
	{ 2, 5, 255, 0, 6976, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_PL" },
	{ 2, 6, 255, 0, 6980, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_SOM" },
	{ 2, 7, 255, 0, 6984, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_LECH" },
	{ 3, 0, 9, 0, 7032, 0, 4, s_LuaChon_10_0, 8, 0, 0, 0, "IDC_COMBO_10_BD" },
	{ 1, 3, 10, 0, 6988, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_LAP" },
	{ 2, 8, 255, 0, 6992, 3, 20, NULL, 0, 0, 0, 0, "IDC_EDITOR_10_TEN" },
	{ 1, 4, 12, 0, 7028, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_PT" },
	{ 6, 0, 13, 0, -1, 4, 0, NULL, 0, 0, 0, 10, "IDC_LIST_10_PT" },
	{ 1, 5, 14, 0, 7012, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_CAT" },
	{ 1, 6, 15, 0, 7016, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_10_TH" },
	{ 3, 1, 16, 0, 7020, 0, 4, s_LuaChon_10_1, 8, 0, 0, 0, "IDC_COMBO_10_VE" },
};
static const char* const s_LuaChon_11_0[] = { "§øng ¨n exp (an toµn)", "Tranh lµm L«i Chñ (x3 exp)" };
static const char* const s_LuaChon_11_1[] = { "Tù chän (®µi 2 - nÐ ®µi chñ)", "§µi 1 (®µi chñ - cã loa)", "§µi 2", "§µi 3", "§µi 4", "§µi 5" };
static const WAUiMuc s_Muc_11[] = {
	{ 1, 0, 0, 0, 7296, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_12_BN" },
	{ 2, 0, 255, 0, 7300, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_BNG" },
	{ 2, 1, 255, 0, 7304, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_BNP" },
	{ 2, 2, 255, 0, 7308, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_CAY" },
	{ 3, 0, 4, 0, 7312, 0, 4, s_LuaChon_11_0, 2, 0, 0, 0, "IDC_COMBO_12_MODE" },
	{ 3, 1, 5, 0, 7316, 0, 4, s_LuaChon_11_1, 6, 0, 0, 0, "IDC_COMBO_12_DAI" },
	{ 1, 1, 6, 0, 7320, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_12_BUFF" },
	{ 1, 2, 7, 0, 7324, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_12_BC" },
	{ 2, 3, 255, 0, 7328, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_BCG" },
	{ 2, 4, 255, 0, 7332, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_BCP" },
	{ 2, 5, 255, 0, 7336, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_LECH" },
	{ 1, 3, 11, 0, 7340, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_12_TS" },
	{ 2, 6, 255, 0, 7344, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_TSG" },
	{ 2, 7, 255, 0, 7348, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_TSP" },
	{ 2, 8, 255, 0, 7352, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_12_TSL" },
};
static const char* const s_LuaChon_12_0[] = { "LÇn l­ît 20 con", "NgÉu nhiªn", "Cè ®Þnh 1 con" };
static const char* const s_LuaChon_12_1[] = { "Gi¶ Giíi Nh©n", "TrÞnh Cöu NhËt", "Chu Së B¸", "Trang Minh Trung", "Cam ChÝnh C«", "Vò NhÊt ThÕ", "D­¬ng Phong DËt", "Hµ Sinh Vong", "T¨ng ChØ ãan", "VÖ Biªn Thµnh", "Cè Thñ §»ng", "Gia C¸t Kinh Hång", "Phan Ng¹t Nh¹n", "Liªn H×nh Th¸i", "Bµo TriÖt s¬n", "V¹n Hå Tinh", "Trö Thiªn MÉn", "§o¹n L¨ng NguyÖt", "T¶ DËt Minh", "NhËm Th­¬ng Khung" };
static const WAUiMuc s_Muc_12[] = {
	{ 1, 0, 0, 0, 7356, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_13_ON" },
	{ 2, 0, 255, 0, 7360, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_13_LUOT" },
	{ 3, 0, 2, 0, 7364, 0, 4, s_LuaChon_12_0, 3, 0, 0, 0, "IDC_COMBO_13_CHON" },
	{ 3, 1, 3, 0, 7368, 0, 4, s_LuaChon_12_1, 20, 0, 0, 12, "IDC_COMBO_13_BOSS" },
	{ 2, 1, 255, 0, 7380, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_13_NGHI" },
	{ 1, 1, 5, 0, 7372, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_13_GHEP" },
	{ 1, 2, 6, 0, 7376, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_13_CHO" },
};
static const char* const s_LuaChon_13_0[] = { "Kh«ng thiÕt lËp" };
static const char* const s_LuaChon_13_1[] = { "1 lÇn mçi môc tiªu míi", "LÆp theo thêi gian", "B¾n khi cßn xa, ¸p s¸t th× ng­ng", "§ñ tÇng Né / ¢m LuËt th× b¾n" };
static const WAUiMuc s_Muc_13[] = {
	{ 1, 0, 0, 0, 7388, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_14_ON" },
	{ 3, 0, 3, 0, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S1" },
	{ 2, 0, 255, 0, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D1" },
	{ 3, 1, 4, 1, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S2" },
	{ 2, 1, 255, 1, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D2" },
	{ 3, 2, 5, 2, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S3" },
	{ 2, 2, 255, 2, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D3" },
	{ 3, 3, 6, 3, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S4" },
	{ 2, 3, 255, 3, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D4" },
	{ 3, 4, 7, 4, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S5" },
	{ 2, 4, 255, 4, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D5" },
	{ 3, 5, 8, 5, 7392, 0, 24, NULL, 0, 1, 1, 0, "IDC_COMBO_14_S6" },
	{ 2, 5, 255, 5, 7416, 0, 24, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_D6" },
	{ 1, 1, 9, 0, 7468, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_14_NOUT" },
	{ 1, 2, 10, 0, 7440, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_14_TC" },
	{ 3, 6, 11, 0, 7444, 0, 4, s_LuaChon_13_0, 1, 1, 1, 0, "IDC_COMBO_14_TCS" },
	{ 3, 7, 12, 0, 7448, 0, 4, s_LuaChon_13_1, 4, 0, 0, 0, "IDC_COMBO_14_TCK" },
	{ 2, 6, 255, 0, 7452, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_TCM" },
	{ 2, 7, 255, 0, 7456, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_TCD" },
	{ 2, 8, 255, 0, 7460, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_14_TCH" },
	{ 1, 3, 16, 0, 7464, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_14_TCP" },
};
static const char* const s_LuaChon_14_0[] = { "Ph­îng T­êng Phñ", "Thµnh §« Phñ", "§¹i Lý", "BiÖn Kinh Phñ", "T­¬ng D­¬ng Phñ", "D­¬ng Ch©u Phñ", "L©m An Phñ", "Thµnh ®ang ®øng (Xa Phu gÇn nhÊt)" };
static const char* const s_LuaChon_14_1[] = { "Tù theo bang héi", "Bªn c«ng (cã lÖnh bµi)", "Bªn thñ (cã lÖnh bµi)" };
static const char* const s_LuaChon_14_2[] = { "Ph¸ cæng råi h¹ Long trô", "ChØ ®¸nh ng­êi (tuÇn tra)" };
static const char* const s_LuaChon_14_3[] = { "Canh Long trô, chiÕm l¹i nÕu mÊt", "ChØ ®¸nh ng­êi (tuÇn tra)" };
static const char* const s_LuaChon_14_4[] = { "Ph­îng T­êng Phñ", "Thµnh §« Phñ", "§¹i Lý", "BiÖn Kinh Phñ", "T­¬ng D­¬ng Phñ", "D­¬ng Ch©u Phñ", "L©m An Phñ", "§iÓm l­u (Rêi khái ®Êu tr­êng)" };
static const char* const s_LuaChon_14_5[] = { "CÊp 1", "CÊp 2", "CÊp 3", "CÊp 4", "CÊp 5" };
static const WAUiMuc s_Muc_14[] = {
	{ 1, 0, 0, 0, 7472, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_15_ON" },
	{ 2, 0, 255, 0, 7476, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_GIO" },
	{ 2, 1, 255, 0, 7480, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_PHUT" },
	{ 2, 2, 255, 0, 7484, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_LECH" },
	{ 2, 3, 255, 0, 7488, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_SOM" },
	{ 2, 4, 255, 0, 7492, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_CUA" },
	{ 3, 0, 6, 0, 7496, 0, 4, s_LuaChon_14_0, 8, 0, 0, 0, "IDC_COMBO_15_BD" },
	{ 3, 1, 7, 0, 7500, 0, 4, s_LuaChon_14_1, 3, 0, 0, 0, "IDC_COMBO_15_PHE" },
	{ 3, 2, 8, 0, 7504, 0, 4, s_LuaChon_14_2, 2, 0, 0, 0, "IDC_COMBO_15_CONG" },
	{ 3, 3, 9, 0, 7508, 0, 4, s_LuaChon_14_3, 2, 0, 0, 0, "IDC_COMBO_15_THU" },
	{ 3, 4, 10, 0, 7512, 0, 4, s_LuaChon_14_4, 8, 0, 0, 0, "IDC_COMBO_15_VE" },
	{ 1, 1, 11, 0, 7516, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_15_LOA" },
	{ 1, 2, 12, 0, 7520, 0, 4, NULL, 0, 0, 0, 0, "IDC_CHECKBOX_15_MUA" },
	{ 2, 5, 255, 0, 7524, 0, 4, NULL, 0, 0, 0, 0, "IDC_EDITOR_15_SB" },
	{ 3, 5, 255, 0, 7528, 0, 4, s_LuaChon_14_5, 5, 0, 0, 0, "IDC_COMBO_15_CAP" },
};
static const WAUiTab s_WAUiTab[15] = {
	{ s_Muc_0, 18, "UiWAuto_Tab0.ini", 12, 2 },
	{ s_Muc_1, 31, "UiWAuto_Tab1.ini", 14, 2 },
	{ s_Muc_2, 22, "UiWAuto_Tab2.ini", 13, 2 },
	{ s_Muc_3, 14, "UiWAuto_Tab3.ini", 8, 2 },
	{ s_Muc_4, 17, "UiWAuto_Tab4.ini", 8, 2 },
	{ s_Muc_5, 28, "UiWAuto_Tab5.ini", 11, 2 },
	{ s_Muc_6, 9, "UiWAuto_Tab6.ini", 5, 2 },
	{ s_Muc_7, 16, "UiWAuto_Tab7.ini", 9, 2 },
	{ s_Muc_8, 17, "UiWAuto_Tab8.ini", 14, 2 },
	{ s_Muc_9, 17, "UiWAuto_Tab9.ini", 14, 2 },
	{ s_Muc_10, 19, "UiWAuto_Tab10.ini", 14, 2 },
	{ s_Muc_11, 15, "UiWAuto_Tab11.ini", 12, 2 },
	{ s_Muc_12, 7, "UiWAuto_Tab12.ini", 7, 2 },
	{ s_Muc_13, 21, "UiWAuto_Tab13.ini", 16, 2 },
	{ s_Muc_14, 15, "UiWAuto_Tab14.ini", 11, 2 },
};

// [WAUTO 12/09] g_MagicTable cua WAuto.cpp: dong thuoc tinh cho bang LOC khi nhat do (nFtMagic[i][0] = nId)
struct WAMagic { const char* szTen; short nId; };
#define WA_SO_MAGIC	40
static const WAMagic s_WAMagic[WA_SO_MAGIC] = {
	{ "Sinh lùc tèi ®a", 85 },
	{ "Néi lùc tèi ®a", 89 },
	{ "ThÓ lùc tèi ®a", 93 },
	{ "Phßng thñ vËt lý", 104 },
	{ "Kh¸ng ®éc", 101 },
	{ "Kh¸ng b¨ng", 105 },
	{ "Kh¸ng háa", 102 },
	{ "Kh¸ng l«i", 103 },
	{ "May m¾n", 135 },
	{ "§é chÝnh x¸c ®iÓm", 166 },
	{ "Tèc ®é ®¸nh - ngo¹i c«ng", 115 },
	{ "Bá qua nÐ tr¸nh", 58 },
	{ "Kh«ng thÓ ph¸ hñy", 43 },
	{ "Tèc ®é di chuyÓn", 111 },
	{ "Ph¶n ®ßn cËn chiÕn", 117 },
	{ "Kh¸ng tÊt c¶", 114 },
	{ "Hót sinh lùc", 136 },
	{ "Hót néi lùc", 137 },
	{ "Phôc håi sinh lùc", 88 },
	{ "Phôc håi néi lùc", 92 },
	{ "Phôc håi thÓ lùc", 96 },
	{ "Søc m¹nh", 97 },
	{ "Sinh khÝ", 99 },
	{ "Th©n ph¸p", 98 },
	{ "S¸t th­¬ng vËt lý-ngo¹i ®iÓm", 121 },
	{ "§éc s¸t ngo¹i c«ng", 125 },
	{ "B¨ng s¸t ngo¹i c«ng", 123 },
	{ "Háa s¸t ngo¹i c«ng", 122 },
	{ "L«i s¸t ngo¹i c«ng", 124 },
	{ "S¸t th­¬ng vËt lý-ngo¹i %", 126 },
	{ "ChuyÓn hãa thµnh néi lùc", 134 },
	{ "S¸t th­¬ng vËt lý - néi c«ng", 168 },
	{ "B¨ng s¸t - néi c«ng", 169 },
	{ "Háa s¸t - néi c«ng", 170 },
	{ "L«i s¸t - néi c«ng", 171 },
	{ "§éc s¸t - néi c«ng", 172 },
	{ "Thêi gian phôc håi", 113 },
	{ "Thêi gian cho¸ng", 110 },
	{ "Thêi gian lµm chËm", 106 },
	{ "Thêi gian tróng ®éc", 108 },
};

// [WAUTO 12/09] s_aTKRCo cua WAuto.cpp: thanh nao CO huong ruong nao (hang = thu tu o "Het tran ve";
// cot = 0 Trung tam / 1 Dong / 2 Tay / 3 Nam / 4 Bac). Hop "Ruong cua" chi liet ke huong CO THAT cua thanh do,
// gia tri luu la MA HUONG chu khong phai chi so dong; dong cuoi luon la "Gan nhat (tu chon)" = 5.
#define WA_TKR_THANH	7
#define WA_TKR_HUONG	5
static const unsigned char s_WATKRuongCo[WA_TKR_THANH][WA_TKR_HUONG] = {
	{ 1, 1, 1, 1, 1 },
	{ 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 1 },
	{ 1, 1, 1, 0, 1 },
	{ 1, 1, 1, 1, 1 },
	{ 1, 1, 1, 1, 1 },
	{ 0, 1, 0, 1, 1 },
};

#endif // JX_MOBILE
#endif
