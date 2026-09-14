# -*- coding: latin-1 -*-
# [HAOQUANG 14/09] Vong hao quang duoi chan + cong tac bat/tat cac hieu ung (chu 14/09: "hay lam va co nut tat mo khi can tat hoac mo"
# sau muc 10.2 mo game 3D Kiem Vong Giang Ho: halo_npc_purple/gold/pink + halo_boss_red o sys_foot quai tinh anh / boss):
#   Core/Src/KNpc.cpp  (chi JX_MOBILE): vong duoi chan QUAI theo m_Type (BOSS_STATE) = anh trang spr\haoquang\vongquai.spr / vongboss.spr
#                       (android/anh_haoquang_vong.py) nhan mau; vong duoi chan NHAN VAT CUA MINH theo pham chat trang bi cao nhat dang mac
#                       (vongtronvang / vongtrontim / vongtronxanh.spr co san cua JX1). Ve ngay truoc than trong KNpc::Paint.
#   Core/Src/KObj.cpp  (chi JX_MOBILE): cong tac tong "Sang vat roi" cho cot sang + loe (VATROI 14/09).
#   S3Client/Ui/UiCase/UiOptions2.h/.cpp (chi JX_MOBILE): 3 cong tac moi o Cai dat > Toi uu: Hao quang quai / Hao quang trang bi /
#                       Sang vat roi (luu uiautoconfig.ini [Options2], mac dinh BAT). MAX_TOGGLE_BTN_COUNT 7 -> 10, bo cuc ini:
#                       android/sinh_uioptions2_haoquang.py.
# Idempotent, latin-1, neo CRLF (nguon co chu GBK/TCVN3 -> giu byte cao). Dung: python android/va_nguon_haoquang_1409.py [goc]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d o %s' % (cao_truoc, cao_sau, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

KN = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KNpc.cpp')
KO = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KObj.cpp')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')

# ======================= KNpc.cpp: khoi ham phu truoc KNpc::Paint + goi truoc m_DataRes.Draw =======================
KHOI = (
    '#ifdef JX_MOBILE' + R +
    '// [HAOQUANG 14/09] Vong hao quang duoi chan (chu 14/09: "hay lam va co nut tat mo khi can tat hoac mo" sau muc 10.2 mo game 3D Kiem Vong' + R +
    '// Giang Ho: halo_npc_purple/gold/pink + halo_boss_red = vong phap tran phang o sys_foot cua quai tinh anh / boss, song thuong truc):' + R +
    '//  - QUAI: m_Type (BOSS_STATE, may chu day qua NpcSync.NpcEnchant - cung truong to mau ten quai o PaintInfo): boss_blue -> vongquai.spr' + R +
    '//    xanh duong, boss_muter -> vongquai.spr xanh la; boss_gold -> vongboss.spr hoang kim, boss_event -> tim, boss_war -> do. Anh TRANG' + R +
    '//    (android/anh_haoquang_vong.py) nhan mau bang IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST, quay theo thoi gian (70 ms/khung).' + R +
    '//  - TRANG BI (chi nhan vat cua MINH - client khong biet do nguoi khac dang mac, khong doi giao thuc): pham chat cao nhat dang mac' + R +
    '//    >= [HaoQuang] TrangBiTu (3 hoang kim) -> vong hao quang co san cua JX1: hoang kim vongtronvang.spr, do tim vongtrontim.spr,' + R +
    '//    bach kim vongtronxanh.spr (30 khung, giu mau goc).' + R +
    '//  Ve ngay truoc than NPC (KNpc::Paint -> m_DataRes.Draw) tai diem dat chan (REF_SPOT, neo = tam vong). Cong tac Cai dat > Toi uu' + R +
    '//  (UiOptions2) qua JxHaoQuang_DatBat; config.ini [HaoQuang] Alpha / TrangBiTu / Thu (1 = moi quai thuong cung co vong xanh, 2 = ca minh' + R +
    '//  - de thu may ao). Khong doi lop KNpc, khong doi giao thuc; PC y het.' + R +
    'static int         s_nHqDoc = 0, s_nHqQuai = 1, s_nHqTrangBi = 1, s_nHqAlpha = 220, s_nHqTrangBiTu = 3, s_nHqThu = 0, s_nHqBao = 0;' + R +
    'static KRUImage    s_HqAnh[5];' + R +
    'static const char* s_HqTen[5] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
    '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr" };' + R +
    'static const int   s_HqKhung[5] = { 12, 16, 30, 30, 30 };' + R +
    'static const int   s_HqMs[5]    = { 70, 70, 50, 50, 80 };' + R + R +
    'extern "C" void JxHaoQuang_DatBat(int nQuai, int nTrangBi)' + R +
    '{' + R +
    '\ts_nHqQuai    = nQuai ? 1 : 0;' + R +
    '\ts_nHqTrangBi = nTrangBi ? 1 : 0;' + R +
    '}' + R + R +
    'static void HaoQuang_DocCfg()' + R +
    '{' + R +
    '\tchar szCfg[MAX_PATH];' + R +
    '\tif (s_nHqDoc)' + R +
    '\t\treturn;' + R +
    '\ts_nHqDoc = 1;' + R +
    '\tGetCurrentDirectory(MAX_PATH, szCfg); strcat(szCfg, "\\\\Config.ini");' + R +
    '\ts_nHqAlpha     = GetPrivateProfileInt("HaoQuang", "Alpha", 220, szCfg);' + R +
    '\ts_nHqTrangBiTu = GetPrivateProfileInt("HaoQuang", "TrangBiTu", 3, szCfg);' + R +
    '\ts_nHqThu       = GetPrivateProfileInt("HaoQuang", "Thu", 0, szCfg);' + R +
    '\tif (s_nHqAlpha < 30) s_nHqAlpha = 30;' + R +
    '\tif (s_nHqAlpha > 255) s_nHqAlpha = 255;' + R +
    '\tg_DebugLog("[HAOQUANG] quai=%d trang bi=%d alpha=%d trang bi tu=%d thu=%d", s_nHqQuai, s_nHqTrangBi, s_nHqAlpha, s_nHqTrangBiTu, s_nHqThu);' + R +
    '}' + R + R +
    '// ve mot vong tai diem dat chan (x, y): nAnh 0..4; dwMau 0 = giu mau goc (anh mau san), khac 0 = nhan mau (anh trang) + alpha cau hinh' + R +
    'static void HaoQuang_VeVong(int nAnh, DWORD dwMau, int x, int y)' + R +
    '{' + R +
    '\tKRUImage& a = s_HqAnh[nAnh];' + R +
    '\tif (a.szImage[0] == 0)' + R +
    '\t{' + R +
    '\t\tmemset(&a, 0, sizeof(a));' + R +
    '\t\ta.nType = ISI_T_SPR;' + R +
    '\t\tstrcpy(a.szImage, s_HqTen[nAnh]);' + R +
    '\t\ta.uImage = 0;' + R +
    '\t\ta.nISPosition = IMAGE_IS_POSITION_INIT;' + R +
    '\t\ta.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;' + R +
    '\t}' + R +
    '\ta.nFrame = (int)(((unsigned int)GetTickCount() / (unsigned int)s_HqMs[nAnh]) % (unsigned int)s_HqKhung[nAnh]);' + R +
    '\ta.oPosition.nX = x; a.oPosition.nY = y; a.oPosition.nZ = 0;' + R +
    '\ta.bRenderStyle = dwMau ? IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST : IMAGE_RENDER_STYLE_ALPHA;' + R +
    '\ta.Color.Color_dw = ((DWORD)(dwMau ? s_nHqAlpha : 255) << 24) | (dwMau & 0x00ffffff);' + R +
    '\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE, 0);' + R +
    '\tif (!s_nHqBao)' + R +
    '\t{' + R +
    '\t\ts_nHqBao = 1;' + R +
    '\t\tg_DebugLog("[HAOQUANG] ve vong dau tien: anh %d mau %06X tai %d,%d", nAnh, (unsigned int)(dwMau & 0xffffff), x, y);' + R +
    '\t}' + R +
    '}' + R + R +
    '// goi trong KNpc::Paint ngay truoc m_DataRes.Draw (than); p = NPC dang ve' + R +
    'static void HaoQuang_Ve(KNpc* p)' + R +
    '{' + R +
    '\tint x = 0, y = 0, nAnh = -1, i, nMau = 0, nIdx, c;' + R +
    '\tDWORD dwMau = 0;' + R +
    '\tHaoQuang_DocCfg();' + R +
    '\tif (p->m_Doing == do_death || p->m_Doing == do_revive || p->m_CurrentLife <= 0)' + R +
    '\t\treturn;' + R +
    '\tif (p->m_Kind == kind_normal || p->m_Kind == kind_dialoger)' + R +
    '\t{' + R +
    '\t\tif (!s_nHqQuai)' + R +
    '\t\t\treturn;' + R +
    '\t\tswitch (p->m_Type)' + R +
    '\t\t{' + R +
    '\t\tcase boss_blue:  nAnh = 0; dwMau = 0x6E78FF; break;\t// quai xanh (tinh anh) - cung mau ten 0x6569d7, sang hon chut' + R +
    '\t\tcase boss_muter: nAnh = 0; dwMau = 0x3CDC3C; break;' + R +
    '\t\tcase boss_gold:  nAnh = 1; dwMau = 0xFFD94E; break;\t// boss hoang kim' + R +
    '\t\tcase boss_event: nAnh = 1; dwMau = 0xE65AFF; break;\t// boss su kien (tim)' + R +
    '\t\tcase boss_war:   nAnh = 1; dwMau = 0xFF3C28; break;\t// boss chien truong (do)' + R +
    '\t\tdefault:' + R +
    '\t\t\tif (s_nHqThu && p->m_Kind == kind_normal) { nAnh = 0; dwMau = 0x6E78FF; }' + R +
    '\t\t\tbreak;' + R +
    '\t\t}' + R +
    '\t}' + R +
    '\telse if (p->m_Kind == kind_player && p->m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)' + R +
    '\t{' + R +
    '\t\tif (!s_nHqTrangBi)' + R +
    '\t\t\treturn;' + R +
    '\t\tfor (i = 0; i < itempart_num; i++)' + R +
    '\t\t{' + R +
    '\t\t\tnIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);' + R +
    '\t\t\tif (nIdx <= 0)' + R +
    '\t\t\t\tcontinue;' + R +
    '\t\t\tc = Item[nIdx].GetColorItem();' + R +
    '\t\t\tif (c != broken_item && c > nMau)' + R +
    '\t\t\t\tnMau = c;' + R +
    '\t\t}' + R +
    '\t\tif (s_nHqThu >= 2 && nMau < gold_item)' + R +
    '\t\t\tnMau = gold_item;' + R +
    '\t\tif (nMau < s_nHqTrangBiTu || nMau < gold_item)' + R +
    '\t\t\treturn;' + R +
    '\t\tnAnh = (nMau == gold_item) ? 2 : (nMau == purple_item) ? 3 : (nMau == platinum_item) ? 4 : -1;' + R +
    '\t}' + R +
    '\tif (nAnh < 0)' + R +
    '\t\treturn;' + R +
    '\tp->GetNpcRes()->GetPos(&x, &y);' + R +
    '\tHaoQuang_VeVong(nAnh, dwMau, x, y);' + R +
    '}' + R +
    '#endif' + R)

va(KN,
   R + 'void KNpc::Paint()' + R + '{' + R,
   R + KHOI + 'void KNpc::Paint()' + R + '{' + R,
   '[HAOQUANG 14/09] Vong hao quang duoi chan')

va(KN,
   '\tm_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, FALSE, bPaintBody);' + R,
   '#ifdef JX_MOBILE' + R +
   '\tif (bPaintBody)' + R +
   '\t\tHaoQuang_Ve(this);\t// [HAOQUANG 14/09] vong hao quang duoi chan, ve truoc bong + than' + R +
   '#endif' + R +
   '\tm_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, FALSE, bPaintBody);' + R,
   '[HAOQUANG 14/09] vong hao quang duoi chan, ve truoc bong')


# sua cay da va ban dau (dong trong thua sau #endif -> kiem --pc HONG); cay moi thi neo khong co, bo qua
_s = doc(KN)
if '#endif' + R + R + 'void KNpc::Paint()' in _s and '[HAOQUANG 14/09] Vong hao quang duoi chan' in _s:
    va(KN, '#endif' + R + R + 'void KNpc::Paint()', '#endif' + R + 'void KNpc::Paint()', '#endif' + R + 'void KNpc::Paint()')

# ======================= KObj.cpp: cong tac tong "Sang vat roi" =======================
va(KO,
   'static KRUImage     s_VrCot, s_VrLoe;' + R,
   'static KRUImage     s_VrCot, s_VrLoe;' + R +
   'static int          s_nVrBat = 1;\t// [HAOQUANG 14/09] cong tac Cai dat > Toi uu "Sang vat roi" (UiOptions2 -> JxVatRoi_DatBat)' + R +
   'extern "C" void JxVatRoi_DatBat(int nBat) { s_nVrBat = nBat ? 1 : 0; }' + R,
   '[HAOQUANG 14/09] cong tac Cai dat > Toi uu "Sang vat roi"')

va(KO,
   '\tif (!s_nVrCotSang || nDropState == 1 || nColorID < s_nVrCotSangTu || nColorID == 2 || nColorID >= 6)' + R,
   '\tif (!s_nVrBat || !s_nVrCotSang || nDropState == 1 || nColorID < s_nVrCotSangTu || nColorID == 2 || nColorID >= 6)\t// [HAOQUANG 14/09] s_nVrBat' + R,
   '!s_nVrBat || !s_nVrCotSang')

va(KO,
   '\tif (!s_nVrLoe || nIndex <= 0 || nIndex >= MAX_OBJECT || !s_uVrLoeLuc[nIndex])' + R,
   '\tif (!s_nVrBat || !s_nVrLoe || nIndex <= 0 || nIndex >= MAX_OBJECT || !s_uVrLoeLuc[nIndex])\t// [HAOQUANG 14/09] s_nVrBat' + R,
   '!s_nVrBat || !s_nVrLoe')

# ======================= UiOptions2.h =======================
va(OH,
   '\tOPTION_I_VENHANH,\t\t// lia ve nhanh' + R,
   '\tOPTION_I_VENHANH,\t\t// lia ve nhanh' + R +
   '\tOPTION_I_HQQUAI,\t\t// [HAOQUANG 14/09] vong hao quang duoi chan quai tinh anh / boss (Core/Src/KNpc.cpp)' + R +
   '\tOPTION_I_HQTRANGBI,\t\t// vong hao quang trang bi cua minh' + R +
   '\tOPTION_I_SANGVATROI,\t// cot sang + loe vat pham roi (Core/Src/KObj.cpp, VATROI 14/09)' + R,
   '[HAOQUANG 14/09] vong hao quang duoi chan quai tinh anh / boss (Core/Src/KNpc.cpp)')

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] = OPTION_INDEX_COUNT2 (4 cu + lia / nhin rong / ve nhanh): 4 hang tu Top=101 (ini lop ghi de)',
   '#define MAX_TOGGLE_BTN_COUNT 10\t// [HAOQUANG 14/09] 7 -> 10 (+ hao quang quai / hao quang trang bi / sang vat roi): 5 hang tu Top=70, cach 31 (android/sinh_uioptions2_haoquang.py); [CAMERA 13/09 TUYCHON] = OPTION_INDEX_COUNT2',
   '[HAOQUANG 14/09] 7 -> 10')

# ======================= UiOptions2.cpp =======================
va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] lop KUiOptions2 co 7 nut (header tra lai 4 cho KUiOptions) - dat SAU include cuoi',
   '#define MAX_TOGGLE_BTN_COUNT 10\t// [HAOQUANG 14/09] 10 nut; [CAMERA 13/09 TUYCHON] lop KUiOptions2 (header tra lai 4 cho KUiOptions) - dat SAU include cuoi',
   '[HAOQUANG 14/09] 10 nut')

va(OC,
   'void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);\t// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)' + R,
   'void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);\t// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)' + R +
   'extern "C" void JxHaoQuang_DatBat(int nQuai, int nTrangBi);\t// [HAOQUANG 14/09] Core/Src/KNpc.cpp: vong hao quang quai / trang bi' + R +
   'extern "C" void JxVatRoi_DatBat(int nBat);\t\t\t\t\t// Core/Src/KObj.cpp: cot sang + loe vat pham roi' + R,
   '[HAOQUANG 14/09] Core/Src/KNpc.cpp: vong hao quang quai / trang bi')

va(OC,
   '\t"LiaCanh", "NhinRong", "LiaVeNhanh",\t// [CAMERA 13/09 TUYCHON] luu UiCommon.ini [Options2]' + R,
   '\t"LiaCanh", "NhinRong", "LiaVeNhanh",\t// [CAMERA 13/09 TUYCHON] luu UiCommon.ini [Options2]' + R +
   '\t"HaoQuangQuai", "HaoQuangTrangBi", "SangVatRoi",\t// [HAOQUANG 14/09]' + R,
   '"HaoQuangQuai", "HaoQuangTrangBi", "SangVatRoi"')

va(OC,
   '\t\tJxLia_DatTuyChon(m_ToggleItemList[OPTION_I_LIA].bEnable, m_ToggleItemList[OPTION_I_NHINRONG].bEnable, m_ToggleItemList[OPTION_I_VENHANH].bEnable);' + R +
   '\t\tbreak;' + R,
   '\t\tJxLia_DatTuyChon(m_ToggleItemList[OPTION_I_LIA].bEnable, m_ToggleItemList[OPTION_I_NHINRONG].bEnable, m_ToggleItemList[OPTION_I_VENHANH].bEnable);' + R +
   '\t\tbreak;' + R +
   '\tcase OPTION_I_HQQUAI:' + R +
   '\tcase OPTION_I_HQTRANGBI:\t// [HAOQUANG 14/09] ap ngay' + R +
   '\t\tJxHaoQuang_DatBat(m_ToggleItemList[OPTION_I_HQQUAI].bEnable, m_ToggleItemList[OPTION_I_HQTRANGBI].bEnable);' + R +
   '\t\tbreak;' + R +
   '\tcase OPTION_I_SANGVATROI:' + R +
   '\t\tJxVatRoi_DatBat(bEnable);' + R +
   '\t\tbreak;' + R,
   'case OPTION_I_HQTRANGBI:\t// [HAOQUANG 14/09] ap ngay')

va(OC,
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_NHINRONG], true, &bOptionsEnable[OPTION_I_NHINRONG]);' + R,
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_NHINRONG], true, &bOptionsEnable[OPTION_I_NHINRONG]);' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_HQQUAI], true, &bOptionsEnable[OPTION_I_HQQUAI]);\t// [HAOQUANG 14/09] mac dinh BAT' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_HQTRANGBI], true, &bOptionsEnable[OPTION_I_HQTRANGBI]);' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_SANGVATROI], true, &bOptionsEnable[OPTION_I_SANGVATROI]);' + R,
   'ls_ToggleOptionName2[OPTION_I_HQQUAI], true')

va(OC,
   '\t\tJxLia_DatTuyChon(bOptionsEnable[OPTION_I_LIA], bOptionsEnable[OPTION_I_NHINRONG], bOptionsEnable[OPTION_I_VENHANH]);\t// [CAMERA 13/09 TUYCHON] luc vao the gioi + moi lan nap lai' + R,
   '\t\tJxLia_DatTuyChon(bOptionsEnable[OPTION_I_LIA], bOptionsEnable[OPTION_I_NHINRONG], bOptionsEnable[OPTION_I_VENHANH]);\t// [CAMERA 13/09 TUYCHON] luc vao the gioi + moi lan nap lai' + R +
   '\t\tJxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI], bOptionsEnable[OPTION_I_HQTRANGBI]);\t// [HAOQUANG 14/09]' + R +
   '\t\tJxVatRoi_DatBat(bOptionsEnable[OPTION_I_SANGVATROI]);' + R,
   'JxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI]')


# ======================= [HAOQUANG 14/09 b] go loi: doc cau hinh VatRoi ngay dau KObj::Draw + Thu=3 hao quang moi NPC =======================
# Truoc: VatRoi_DocCfg chi chay trong VatRoi_VeDuoi, ma VeDuoi chi duoc goi khi da co vat pham -> [VatRoi] Thu=2 (cot sang moi vat the)
# khong bao gio bat truoc khi co vat roi dau tien. Nay doc mot lan ngay dau hook (sau lan dau chi la 1 phep so sanh).
va(KO,
   '#ifdef JX_MOBILE' + R +
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money) || s_nVrThu >= 2)' + R,
   '#ifdef JX_MOBILE' + R +
   '\tVatRoi_DocCfg();\t// [HAOQUANG 14/09 b] doc [VatRoi] truoc khi xet Thu (truoc day chi doc luc ve vat pham dau tien -> Thu=2 khong bat duoc)' + R +
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money) || s_nVrThu >= 2)' + R,
   '[HAOQUANG 14/09 b] doc [VatRoi] truoc khi xet Thu')

va(KN,
   '\t\t\tif (s_nHqThu && p->m_Kind == kind_normal) { nAnh = 0; dwMau = 0x6E78FF; }' + R,
   '\t\t\tif (s_nHqThu && (p->m_Kind == kind_normal || s_nHqThu >= 3)) { nAnh = 0; dwMau = 0x6E78FF; }\t// [HAOQUANG 14/09 b] Thu=3: ca NPC thoai (thu trong thanh)' + R,
   '[HAOQUANG 14/09 b] Thu=3')


# [HAOQUANG 14/09 c] NPC thoai trong thanh khong co mau tren client (sync gon: m_CurrentLife = 0) -> bo dieu kien mau, chi xet chet/hoi sinh
va(KN,
   '\tif (p->m_Doing == do_death || p->m_Doing == do_revive || p->m_CurrentLife <= 0)' + R,
   '\tif (p->m_Doing == do_death || p->m_Doing == do_revive)\t// [HAOQUANG 14/09 c] khong xet m_CurrentLife (NPC thoai = 0 tren client)' + R,
   '[HAOQUANG 14/09 c] khong xet m_CurrentLife')


# [HAOQUANG 14/09 d] go loi: bao lan ve dau tien cua TUNG anh + bao 1 lan/5 s quyet dinh cho NPC (kind, type, anh) khi Thu > 0
va(KN,
   '\tif (!s_nHqBao)' + R + '\t{' + R + '\t\ts_nHqBao = 1;' + R,
   '\tif (!(s_nHqBao & (1 << nAnh)))' + R + '\t{' + R + '\t\ts_nHqBao |= (1 << nAnh);' + R,
   's_nHqBao |= (1 << nAnh)')
va(KN,
   '\tif (nAnh < 0)' + R + '\t\treturn;' + R + '\tp->GetNpcRes()->GetPos(&x, &y);' + R,
   '\tif (s_nHqThu && p->m_Kind != kind_player)' + R +
   '\t{' + R +
   '\t\tstatic unsigned int s_uHqBaoLuc = 0;' + R +
   '\t\tif ((unsigned int)GetTickCount() - s_uHqBaoLuc > 5000)' + R +
   '\t\t{' + R +
   '\t\t\ts_uHqBaoLuc = (unsigned int)GetTickCount();' + R +
   '\t\t\tg_DebugLog("[HAOQUANG] npc %d kind %u type %d doing %d -> anh %d", p->m_Index, (unsigned int)p->m_Kind, p->m_Type, (int)p->m_Doing, nAnh);' + R +
   '\t\t}' + R +
   '\t}' + R +
   '\tif (nAnh < 0)' + R + '\t\treturn;' + R + '\tp->GetNpcRes()->GetPos(&x, &y);' + R,
   '[HAOQUANG] npc %d kind %u type %d doing %d -> anh %d')

print('\n'.join(KQ))
