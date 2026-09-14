# -*- coding: latin-1 -*-
# [CAMERA 13/09] Buoc 3 lia/zoom canh: THAM SO THEO MAP (settings\camera_mobile.ini, nhu cameraInit cua game 3D) + zoom troi dan toi muc
# cua map khi vao map + nho zoom nguoi choi (UserData\CameraMobile.ini) + chu bao "Nhin rong NNN %" (JxLia_Ve trong UiShell)
# + chum hai ngon khong con phu thuoc LiaCanh (map tat lia van chum duoc, chum khi do chi zoom khong lia).
# Idempotent, latin-1, neo CRLF (nguon) / LF (ini). Dung: python android/va_nguon_camera_map_1309.py [goc] [--config <ini> ...]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

# Chu tieng Viet co dau (TCVN3) cho dong "Nhin rong NNN %" tren man hinh: ma hoa bang bo cong cu cua skill, KHONG go tay byte.
VN_NHIN_RONG = None
try:
    sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
    from vn_edit import vn as _vn
    VN_NHIN_RONG = _vn(u'Nh\u00ecn r\u1ed9ng')   # "Nhin rong" co dau (escape unicode de tep nay thuan ASCII)
except Exception as e:
    KQ.append('CANH BAO: khong ma hoa duoc TCVN3 (%s) -> giu chu khong dau' % e)

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu, so_lan=1, cao_them=0):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc + cao_them != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d (mong +%d) o %s' % (cao_truoc, cao_sau, cao_them, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

S3 = os.path.join(GOC, 'Sources', 'S3Client')
LH = os.path.join(S3, 'Platform', 'JxLiaCanh.h')
LC = os.path.join(S3, 'Platform', 'JxLiaCanh.cpp')

# ---------- header ----------
va(LH,
   'void JxLia_ZoomDat(int nPhanTram);\t// dat zoom % (100..ZoomToiDa, lam tron ZoomBuoc): Represent3 + vung truy van vat the (SetRepresentAreaSize)' + R,
   'void JxLia_ZoomDat(int nPhanTram);\t// dat zoom % (100..ZoomToiDa, lam tron ZoomBuoc): Represent3 + vung truy van vat the (SetRepresentAreaSize)' + R + R +
   '// [CAMERA 13/09] Tham so theo MAP: settings\\camera_mobile.ini muc [Map_<id>] (id = so thu tu maplist.ini) / [MacDinh]: ZoomMacDinh, ZoomToiDa,' + R +
   '// LiaXaNgang, LiaXaDoc, LiaCanh, ZoomCanh. Vao map / doi map: 1 s sau doc muc, zoom TROI dan (ZoomTocDo %/s) toi zoom nguoi choi da nho' + R +
   '// (UserData\\CameraMobile.ini, [Cham] ZoomNho=1) kep theo ZoomToiDa cua map, chua nho thi ZoomMacDinh cua map.' + R +
   '// Goi cuoi moi khung ve (UiShell::UiPaint): chu "Nhin rong NNN %" 1,5 s sau khi zoom doi ([Cham] ZoomChu=1).' + R +
   'void JxLia_Ve();' + R,
   'void JxLia_Ve();')

# ---------- statics ----------
va(LC,
   'static int\t\t\ts_nZoomThuXong = 0;' + R,
   'static int\t\t\ts_nZoomThuXong = 0;' + R +
   '// [CAMERA 13/09] theo map + nho + troi + chu bao' + R +
   'static int\t\t\ts_nZoomToiDaCfg = 150, s_nXaNgangCfg = 60, s_nXaDocCfg = 60, s_nBatCfg = 1, s_nZoomBatCfg = 1;\t// gia tri config.ini (map ghi de len ban dang dung)' + R +
   'static int\t\t\ts_nZoomNho = 1;\t\t// [Cham] ZoomNho: 1 = nho zoom nguoi choi qua UserData\\CameraMobile.ini' + R +
   'static int\t\t\ts_nZoomTocDo = 100;\t// [Cham] ZoomTocDo: %/giay khi troi toi zoom cua map' + R +
   'static int\t\t\ts_nZoomChu = 1;\t\t// [Cham] ZoomChu: 1 = chu "Nhin rong NNN %" 1,5 s khi doi' + R +
   'static int\t\t\ts_nZoomNguoiChoi = 0;\t// zoom nguoi choi tu chum (0 = chua), nho qua tep' + R +
   'static int\t\t\ts_nZoomDich = 0;\t\t// zoom dang troi toi (0 = khong troi)' + R +
   'static float\t\ts_fZoomTroi = 100.f;\t// zoom lien tuc trong luc troi' + R +
   'static int\t\t\ts_nCanDocMap = 1;\t\t// 1 = vao map / doi map, can doc lai muc cua map' + R +
   'static int\t\t\ts_nMapId = -1;\t\t\t// map dang ap' + R +
   'static unsigned int\ts_uZoomChuLuc = 0;\t\t// luc zoom doi gan nhat (chu bao)' + R +
   'static int\t\t\ts_nZoomChuTruoc = 100;\t// zoom lan bao truoc (chi bao khi doi)' + R +
   'static char\t\t\ts_szZoomChu[48] = "";' + R,
   's_nZoomToiDaCfg')
# khai bao truoc: ZoomAp / Camera_DocMap dinh nghia o cuoi tep (khoi ZOOM) nhung JxLia_Nhip goi
va(LC,
   'static char\t\t\ts_szZoomChu[48] = "";' + R,
   'static char\t\t\ts_szZoomChu[48] = "";' + R +
   'static void\t\t\tZoomAp(int nPhanTram);\t// dinh nghia o khoi ZOOM cuoi tep' + R +
   'static void\t\t\tCamera_DocMap();' + R,
   'static void\t\t\tCamera_DocMap();')

# ---------- DocCaiDat: khoa moi + nho zoom ----------
va(LC,
   '\ts_nZoom = s_nZoomMacDinh;' + R,
   '\ts_nZoom = s_nZoomMacDinh;' + R +
   '\ts_nZoomToiDaCfg = s_nZoomToiDa; s_nXaNgangCfg = s_nXaNgang; s_nXaDocCfg = s_nXaDoc; s_nBatCfg = s_nBat; s_nZoomBatCfg = s_nZoomBat;\t// [CAMERA 13/09]' + R +
   '\ts_nZoomNho   = GetPrivateProfileInt("Cham", "ZoomNho", 1, szCfg);' + R +
   '\ts_nZoomTocDo = GetPrivateProfileInt("Cham", "ZoomTocDo", 100, szCfg);' + R +
   '\ts_nZoomChu   = GetPrivateProfileInt("Cham", "ZoomChu", 1, szCfg);' + R +
   '\tif (s_nZoomTocDo < 10) s_nZoomTocDo = 10; if (s_nZoomTocDo > 1000) s_nZoomTocDo = 1000;' + R +
   '\tif (s_nZoomNho)' + R +
   '\t{\t// [CAMERA 13/09] zoom nguoi choi da nho (nhu CameraSave cua game 3D): UserData\\CameraMobile.ini dong "Zoom=NNN"' + R +
   '\t\tchar szNho[MAX_PATH]; char szDong[64]; FILE* pTep;' + R +
   '\t\tGetCurrentDirectory(MAX_PATH, szNho); strcat(szNho, "\\\\UserData\\\\CameraMobile.ini");' + R +
   '\t\tpTep = fopen(szNho, "rt");' + R +
   '\t\tif (pTep)' + R +
   '\t\t{' + R +
   '\t\t\twhile (fgets(szDong, sizeof(szDong), pTep))' + R +
   '\t\t\t\tif (strncmp(szDong, "Zoom=", 5) == 0) s_nZoomNguoiChoi = atoi(szDong + 5);' + R +
   '\t\t\tfclose(pTep);' + R +
   '\t\t\tif (s_nZoomNguoiChoi < 100 || s_nZoomNguoiChoi > 300) s_nZoomNguoiChoi = 0;' + R +
   '\t\t}' + R +
   '\t}' + R +
   '\ts_fZoomTroi = (float)s_nZoom;' + R,
   '"ZoomNho"')
va(LC,
   '\tg_DebugLog("[ZOOM] chum hai ngon: bat=%d toi da=%d%% buoc=%d%% mac dinh=%d%% thu=%d%%", s_nZoomBat, s_nZoomToiDa, s_nZoomBuoc, s_nZoomMacDinh, s_nZoomThu);' + R,
   '\tg_DebugLog("[ZOOM] chum hai ngon: bat=%d toi da=%d%% buoc=%d%% mac dinh=%d%% thu=%d%%", s_nZoomBat, s_nZoomToiDa, s_nZoomBuoc, s_nZoomMacDinh, s_nZoomThu);' + R +
   '\tg_DebugLog("[CAMERA] theo map: nho=%d (zoom nguoi choi da nho %d%%) toc do troi=%d%%/s chu=%d", s_nZoomNho, s_nZoomNguoiChoi, s_nZoomTocDo, s_nZoomChu);' + R,
   '[CAMERA] theo map: nho')

# ---------- ham noi bo: ghi nho (dat truoc khoi Rep3TheGioi) ----------
va(LC,
   '// [ZOOM 13/09] Represent3 (libRepresent3.so / Represent3.dll) qua GetProcAddress nhu Wnds.cpp JxUi_TheGioi: libmain.so khong link Represent3;' + R,
   '// [CAMERA 13/09] ghi zoom nguoi choi: UserData\\CameraMobile.ini (nhu KyNangMobile.ini: GetCurrentDirectory + duong tuong doi)' + R +
   'static void Camera_GhiNho()' + R +
   '{' + R +
   '\tchar szNho[MAX_PATH]; FILE* pTep;' + R + R +
   '\tif (!s_nZoomNho || s_nZoomNguoiChoi <= 0)' + R +
   '\t\treturn;' + R +
   '\tGetCurrentDirectory(MAX_PATH, szNho); strcat(szNho, "\\\\UserData\\\\CameraMobile.ini");' + R +
   '\tpTep = fopen(szNho, "wt");' + R +
   '\tif (!pTep)' + R +
   '\t\treturn;' + R +
   '\tfprintf(pTep, "; [CAMERA 13/09] zoom nguoi choi tu chum (%%); xoa tep = ve ZoomMacDinh cua map\\nZoom=%d\\n", s_nZoomNguoiChoi);' + R +
   '\tfclose(pTep);' + R +
   '}' + R + R +
   '// [ZOOM 13/09] Represent3 (libRepresent3.so / Represent3.dll) qua GetProcAddress nhu Wnds.cpp JxUi_TheGioi: libmain.so khong link Represent3;' + R,
   'static void Camera_GhiNho()')

# ---------- diem tren ban do: dung chung lia mot ngon / chum hai ngon (chum khong can LiaCanh) ----------
va(LC,
   'bool JxLia_DuocBatDau(int x, int y)' + R + '{' + R + '\tDocCaiDat();' + R +
   '\tif (!s_nBat || !g_pCoreShell || !TrongGame())' + R + '\t\treturn false;' + R,
   '// [CAMERA 13/09] diem (x,y) nam tren ban do (khong dinh giao dien / can / nut ky nang) - dung chung cho lia mot ngon va chum hai ngon' + R +
   'static bool TrenBanDo(int x, int y)' + R + '{' + R +
   '\tif (!g_pCoreShell || !TrongGame())' + R + '\t\treturn false;' + R,
   'static bool TrenBanDo(int x, int y)')
va(LC,
   '\tif (JxKyNang_TrungNut(x, y) > 0)' + R + '\t\treturn false;' + R + '\treturn true;' + R + '}' + R,
   '\tif (JxKyNang_TrungNut(x, y) > 0)' + R + '\t\treturn false;' + R + '\treturn true;' + R + '}' + R + R +
   'bool JxLia_DuocBatDau(int x, int y)' + R + '{' + R + '\tDocCaiDat();' + R +
   '\treturn s_nBat && TrenBanDo(x, y);' + R + '}' + R,
   '\treturn s_nBat && TrenBanDo(x, y);')
va(LC,
   '\treturn JxLia_DuocBatDau(x1, y1) && JxLia_DuocBatDau(x2, y2);' + R,
   '\treturn TrenBanDo(x1, y1) && TrenBanDo(x2, y2);\t// [CAMERA 13/09] chum khong can LiaCanh (map tat lia van chum duoc)' + R,
   'TrenBanDo(x1, y1) && TrenBanDo(x2, y2)')
va(LC,
   '\tJxLia_BatDau((x1 + x2) / 2, (y1 + y2) / 2);\t// lia theo tam hai ngon (dang lech thi giu lech, neo lai tu tam)' + R,
   '\tif (s_nBat)\t// [CAMERA 13/09] LiaCanh=0 thi chum chi zoom, khong lia' + R +
   '\t\tJxLia_BatDau((x1 + x2) / 2, (y1 + y2) / 2);\t// lia theo tam hai ngon (dang lech thi giu lech, neo lai tu tam)' + R,
   'LiaCanh=0 thi chum chi zoom, khong lia')
va(LC,
   '\tJxLia_Keo((x1 + x2) / 2, (y1 + y2) / 2);' + R + '}' + R,
   '\tif (s_nBat)' + R + '\t\tJxLia_Keo((x1 + x2) / 2, (y1 + y2) / 2);' + R + '}' + R,
   '\tif (s_nBat)' + R + '\t\tJxLia_Keo(')
va(LC,
   '\tJxLia_Nha();' + R + '}' + R + R + '#endif // JX_MOBILE',
   '\tif (s_nBat)' + R + '\t\tJxLia_Nha();' + R + '}' + R + R + '#endif // JX_MOBILE',
   '\tif (s_nBat)' + R + '\t\tJxLia_Nha();')

# ---------- JxLia_ZoomDat: tach ZoomAp (ap that) + ZoomDat (nguoi choi dat) + Camera_DocMap ----------
va(LC,
   'void JxLia_ZoomDat(int nPhanTram)' + R +
   '{' + R +
   '\tDocCaiDat();' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\tif (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   '\tnPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\ts_nZoom = nPhanTram;' + R +
   '\tif (s_nZoomDaAp == s_nZoom || !g_pCoreShell)' + R +
   '\t\treturn;' + R +
   '\ts_nZoomDaAp = s_nZoom;' + R,
   '// [CAMERA 13/09] ap zoom that (Represent3 + vung truy van); ZoomDat = nguoi choi dat (huy troi, nho lai), troi theo map goi thang ZoomAp' + R +
   'static void ZoomAp(int nPhanTram)' + R +
   '{' + R +
   '\tDocCaiDat();' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\tif (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   '\tnPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\ts_nZoom = nPhanTram;' + R +
   '\tif (s_nZoomDaAp == s_nZoom || !g_pCoreShell)' + R +
   '\t\treturn;' + R +
   '\ts_nZoomDaAp = s_nZoom;' + R +
   '\tif (s_nZoom != s_nZoomChuTruoc)' + R +
   '\t{\t// chu bao "Nhin rong NNN %" (JxLia_Ve) 1,5 s' + R +
   '\t\ts_nZoomChuTruoc = s_nZoom;' + R +
   '\t\ts_uZoomChuLuc = (unsigned int)GetTickCount();' + R +
   '\t\tsprintf(s_szZoomChu, "Nhin rong %d%%", s_nZoom);' + R +
   '\t}' + R,
   'static void ZoomAp(int nPhanTram)')
va(LC,
   '\tif (s_nNhatKy || s_nZoomThu)' + R +
   '\t\tg_DebugLog("[ZOOM] zoom %d%% (Rep3 %d, vung truy van %dx%d)", s_nZoom, Rep3TheGioi(5, 0), SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);' + R +
   '}' + R,
   '\tif (s_nNhatKy || s_nZoomThu)' + R +
   '\t\tg_DebugLog("[ZOOM] zoom %d%% (Rep3 %d, vung truy van %dx%d)", s_nZoom, Rep3TheGioi(5, 0), SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);' + R +
   '}' + R + R +
   'void JxLia_ZoomDat(int nPhanTram)' + R +
   '{\t// nguoi choi dat (chum): ap ngay, huy troi, nho lai' + R +
   '\tZoomAp(nPhanTram);' + R +
   '\ts_nZoomDich = 0;' + R +
   '\ts_fZoomTroi = (float)s_nZoom;' + R +
   '\ts_nZoomNguoiChoi = s_nZoom;' + R +
   '}' + R + R +
   '// [CAMERA 13/09] doc muc cua map dang dung (settings\\camera_mobile.ini) roi dat zoom dich' + R +
   'static void Camera_DocMap()' + R +
   '{' + R +
   '\tKUiSceneTimeInfoOften o; char szTep[MAX_PATH]; char szMuc[32]; int nZoomMD, nDich;' + R + R +
   '\tif (!g_pCoreShell)' + R +
   '\t\treturn;' + R +
   '\tmemset(&o, 0, sizeof(o));' + R +
   '\tg_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO_OFTEN, (KUPARAM)&o, 0);' + R +
   '\tif (o.nSceneId <= 0)' + R +
   '\t\treturn;\t// chua co map: thu lai nhip sau' + R +
   '\ts_nCanDocMap = 0;' + R +
   '\ts_nMapId = o.nSceneId;' + R +
   '\tGetCurrentDirectory(MAX_PATH, szTep); strcat(szTep, "\\\\settings\\\\camera_mobile.ini");' + R +
   '\tsprintf(szMuc, "Map_%d", o.nSceneId);' + R +
   '\t// [MacDinh] ghi de config.ini; [Map_<id>] ghi de [MacDinh]' + R +
   '\tnZoomMD      = GetPrivateProfileInt(szMuc, "ZoomMacDinh", GetPrivateProfileInt("MacDinh", "ZoomMacDinh", s_nZoomMacDinh, szTep), szTep);' + R +
   '\ts_nZoomToiDa = GetPrivateProfileInt(szMuc, "ZoomToiDa",   GetPrivateProfileInt("MacDinh", "ZoomToiDa",   s_nZoomToiDaCfg, szTep), szTep);' + R +
   '\ts_nXaNgang   = GetPrivateProfileInt(szMuc, "LiaXaNgang",  GetPrivateProfileInt("MacDinh", "LiaXaNgang",  s_nXaNgangCfg, szTep), szTep);' + R +
   '\ts_nXaDoc     = GetPrivateProfileInt(szMuc, "LiaXaDoc",    GetPrivateProfileInt("MacDinh", "LiaXaDoc",    s_nXaDocCfg, szTep), szTep);' + R +
   '\ts_nBat       = GetPrivateProfileInt(szMuc, "LiaCanh",     GetPrivateProfileInt("MacDinh", "LiaCanh",     s_nBatCfg, szTep), szTep);' + R +
   '\ts_nZoomBat   = GetPrivateProfileInt(szMuc, "ZoomCanh",    GetPrivateProfileInt("MacDinh", "ZoomCanh",    s_nZoomBatCfg, szTep), szTep);' + R +
   '\tif (s_nZoomToiDa < 100) s_nZoomToiDa = 100; if (s_nZoomToiDa > 300) s_nZoomToiDa = 300;' + R +
   '\tif (s_nXaNgang < 0) s_nXaNgang = 0; if (s_nXaNgang > 150) s_nXaNgang = 150;' + R +
   '\tif (s_nXaDoc < 0) s_nXaDoc = 0; if (s_nXaDoc > 150) s_nXaDoc = 150;' + R +
   '\t// zoom dich: ZoomThu (go loi) > zoom nguoi choi da nho > ZoomMacDinh cua map; kep theo ZoomToiDa cua map; map tat zoom -> 100' + R +
   '\tif (s_nZoomThu > 100 && !s_nZoomThuXong) { nDich = s_nZoomThu; s_nZoomThuXong = 1; }' + R +
   '\telse if (s_nZoomNho && s_nZoomNguoiChoi > 0) nDich = s_nZoomNguoiChoi;' + R +
   '\telse nDich = nZoomMD;' + R +
   '\tif (!s_nZoomBat) nDich = 100;' + R +
   '\tif (nDich < 100) nDich = 100;' + R +
   '\tif (nDich > s_nZoomToiDa) nDich = s_nZoomToiDa;' + R +
   '\ts_nZoomDich = nDich;' + R +
   '\ts_fZoomTroi = (float)s_nZoom;' + R +
   '\tif (s_nZoomDaAp < 0)' + R +
   '\t\tZoomAp(s_nZoom);\t// doi map: ap lai vung truy van (Represent3 van giu zoom) truoc khi troi' + R +
   '\tg_DebugLog("[CAMERA] map %d (%s): zoom mac dinh %d%%, toi da %d%%, lia %d%%x%d%%, lia=%d zoom=%d | dang %d%% -> troi toi %d%% (nguoi choi nho %d%%)",' + R +
   '\t\to.nSceneId, o.szSceneName, nZoomMD, s_nZoomToiDa, s_nXaNgang, s_nXaDoc, s_nBat, s_nZoomBat, s_nZoom, nDich, s_nZoomNguoiChoi);' + R +
   '}' + R,
   'static void Camera_DocMap()')

# ---------- JxLia_Nhip: bo thoat som khi LiaCanh=0 (van phai doc map + troi zoom), thay khoi ZoomThu 3 s bang doc map + troi ----------
va(LC,
   '\tDocCaiDat();' + R +
   '\tif (!s_nBat)' + R +
   '\t\treturn;' + R +
   '\tuNay = (unsigned int)GetTickCount();' + R +
   '\tdt = s_uNhipTruoc ? (float)(uNay - s_uNhipTruoc) / 1000.f : 0.f;' + R +
   '\tif (dt > 0.1f) dt = 0.1f;\t// khung dung lau (nap map) khong nhay mot cuc' + R +
   '\ts_uNhipTruoc = uNay;' + R +
   '\tif (!TrongGame())' + R +
   '\t{' + R +
   '\t\tif (s_nTrangThai != LIA_KHONG || s_nDaBatCo)' + R +
   '\t\t\tKetThuc("roi the gioi");' + R +
   '\t\ts_nCoGoc = 0;' + R +
   '\t\ts_uVaoGameLuc = 0;' + R +
   '\t\treturn;' + R +
   '\t}' + R +
   '\t// [ZOOM 13/09] zoom mac dinh / ZoomThu (go loi): ap sau khi vao the gioi 3 s (Represent3 va Core da san sang)' + R +
   '\tif (!s_uVaoGameLuc)' + R +
   '\t\ts_uVaoGameLuc = uNay;' + R +
   '\telse if (uNay - s_uVaoGameLuc >= 3000 && !s_nZoomThuXong)' + R +
   '\t{' + R +
   '\t\ts_nZoomThuXong = 1;' + R +
   '\t\tif (s_nZoomThu > 100) { g_DebugLog("[ZOOM] ZoomThu: tu dat %d%%", s_nZoomThu); JxLia_ZoomDat(s_nZoomThu); }' + R +
   '\t\telse if (s_nZoom != 100 || s_nZoomDaAp != s_nZoom) JxLia_ZoomDat(s_nZoom);' + R +
   '\t}' + R,
   '\tDocCaiDat();' + R +
   '\tuNay = (unsigned int)GetTickCount();' + R +
   '\tdt = s_uNhipTruoc ? (float)(uNay - s_uNhipTruoc) / 1000.f : 0.f;' + R +
   '\tif (dt > 0.1f) dt = 0.1f;\t// khung dung lau (nap map) khong nhay mot cuc' + R +
   '\ts_uNhipTruoc = uNay;' + R +
   '\tif (!TrongGame())' + R +
   '\t{' + R +
   '\t\tif (s_nTrangThai != LIA_KHONG || s_nDaBatCo)' + R +
   '\t\t\tKetThuc("roi the gioi");' + R +
   '\t\ts_nCoGoc = 0;' + R +
   '\t\ts_uVaoGameLuc = 0;' + R +
   '\t\treturn;' + R +
   '\t}' + R +
   '\t// [CAMERA 13/09] vao the gioi / doi map: 1 s sau doc muc cua map (settings\\camera_mobile.ini) roi TROI zoom toi dich theo ZoomTocDo' + R +
   '\tif (!s_uVaoGameLuc)' + R +
   '\t\ts_uVaoGameLuc = uNay;' + R +
   '\telse if (uNay - s_uVaoGameLuc >= 1000 && s_nCanDocMap)' + R +
   '\t\tCamera_DocMap();' + R +
   '\tif (s_nZoomDich > 0 && !s_nChum)' + R +
   '\t{' + R +
   '\t\tfloat fBuoc = (float)s_nZoomTocDo * dt;' + R +
   '\t\tif (s_fZoomTroi < (float)s_nZoomDich) { s_fZoomTroi += fBuoc; if (s_fZoomTroi > (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R +
   '\t\telse { s_fZoomTroi -= fBuoc; if (s_fZoomTroi < (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R +
   '\t\tZoomAp((int)(s_fZoomTroi + 0.5f));' + R +
   '\t\tif ((int)(s_fZoomTroi + 0.5f) == s_nZoomDich)' + R +
   '\t\t{' + R +
   '\t\t\tZoomAp(s_nZoomDich);' + R +
   '\t\t\ts_nZoomDich = 0;' + R +
   '\t\t}' + R +
   '\t}' + R +
   '\tif (!s_nBat)' + R +
   '\t\treturn;\t// [CAMERA 13/09] LiaCanh=0 (config hay map): van doc map + troi zoom o tren, khong lia' + R,
   '[CAMERA 13/09] vao the gioi / doi map')

# ---------- doi map -> doc lai; chum nha -> nho ----------
va(LC,
   '\ts_nZoomThuXong = 0; s_nZoomDaAp = -1;\t// [ZOOM 13/09] doi map: 3 s sau ap lai zoom (vung truy van vat the) - Represent3 van giu zoom' + R,
   '\ts_nZoomDaAp = -1; s_nCanDocMap = 1; s_nZoomDich = 0;\t// [CAMERA 13/09] doi map: 1 s sau doc muc cua map, ap lai vung truy van, troi toi zoom dich' + R,
   's_nCanDocMap = 1; s_nZoomDich = 0;')
va(LC,
   '\ts_nChum = 0;' + R +
   '\tif (s_nNhatKy)' + R +
   '\t\tg_DebugLog("[ZOOM] chum nha: zoom=%d%%", s_nZoom);' + R,
   '\ts_nChum = 0;' + R +
   '\tCamera_GhiNho();\t// [CAMERA 13/09] nho zoom nguoi choi' + R +
   '\tif (s_nNhatKy)' + R +
   '\t\tg_DebugLog("[ZOOM] chum nha: zoom=%d%%", s_nZoom);' + R,
   'Camera_GhiNho();\t// [CAMERA 13/09]')

# ---------- JxLia_Ve (chu bao) ----------
va(LC,
   'bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2)' + R,
   '// [CAMERA 13/09] chu "Nhin rong NNN %" 1,5 s sau khi zoom doi - giua man, 1/3 tren (nhu KyNang_VeChu: OutputText toa do man hinh, co 16)' + R +
   'void JxLia_Ve()' + R +
   '{' + R +
   '\tunsigned int uNay = (unsigned int)GetTickCount();' + R +
   '\tint nDai;' + R + R +
   '\tif (!s_nZoomChu || !s_uZoomChuLuc || uNay - s_uZoomChuLuc > 1500 || g_pRepresentShell == NULL || !TrongGame())' + R +
   '\t\treturn;' + R +
   '\tnDai = (int)strlen(s_szZoomChu);' + R +
   '\tif (nDai <= 0)' + R +
   '\t\treturn;' + R +
   '\tg_pRepresentShell->OutputText(16, s_szZoomChu, nDai, SCREEN_WIDTH / 2 - nDai * 4, SCREEN_HEIGHT / 3, 0xFFFFE28A, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);' + R +
   '}' + R + R +
   'bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2)' + R,
   'void JxLia_Ve()')

# [CAMERA 13/09 b] chu bao: co 12 + vien den nhu PerfHud_Chu (JxPerfHudAndroid.cpp), y = 1/5 man (duoi hang icon, tren nhan dau nhan vat)
# - ban dau dat 1/3 man co 16: trung nhan ten tren dau nhan vat o giua man (anh may ao 22:06), khong nhin thay
va(LC,
   '\tg_pRepresentShell->OutputText(16, s_szZoomChu, nDai, SCREEN_WIDTH / 2 - nDai * 4, SCREEN_HEIGHT / 3, 0xFFFFE28A, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);' + R,
   '\tg_pRepresentShell->OutputText(12, s_szZoomChu, nDai, SCREEN_WIDTH / 2 - nDai * 3, SCREEN_HEIGHT / 5, 0xFFFFD040, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);\t// [CAMERA 13/09 b] nhu PerfHud_Chu: co 12, vang, vien den' + R,
   '[CAMERA 13/09 b] nhu PerfHud_Chu')

# includes / extern cho OutputText
va(LC,
   '#include "../../Core/src/GameDataDef.h"\t// KSceneMapInfo' + R,
   '#include "../../Core/src/GameDataDef.h"\t// KSceneMapInfo, KUiSceneTimeInfoOften' + R +
   '#include "../../Represent/iRepresent/iRepresentShell.h"\t// [CAMERA 13/09] OutputText' + R,
   'iRepresentShell.h"\t// [CAMERA 13/09]')
va(LC,
   'extern iCoreShell*\tg_pCoreShell;' + R,
   'extern iCoreShell*\tg_pCoreShell;' + R + 'extern iRepresentShell*\tg_pRepresentShell;\t// [CAMERA 13/09]' + R,
   'extern iRepresentShell*\tg_pRepresentShell;\t// [CAMERA 13/09]')

# chu co dau (TCVN3) cho dong bao - CUOI CUNG, sau khi da chen chuoi khong dau
if VN_NHIN_RONG:
    va(LC, '"Nhin rong %d%%"', '"' + VN_NHIN_RONG + ' %d%%"', VN_NHIN_RONG + ' %d%%',
       cao_them=sum(1 for c in VN_NHIN_RONG if ord(c) >= 0x80))

# ---------- UiShell.cpp: ve chu ----------
US = os.path.join(S3, 'Ui', 'UiShell.cpp')
va(US,
   '#include "../Platform/JxCanDieuKhien.h"\t// [ANDROID 09/09 CAN] can dieu khien ao' + R,
   '#include "../Platform/JxCanDieuKhien.h"\t// [ANDROID 09/09 CAN] can dieu khien ao' + R +
   '#include "../Platform/JxLiaCanh.h"\t// [CAMERA 13/09] chu bao muc zoom' + R,
   'JxLiaCanh.h"\t// [CAMERA 13/09]')
va(US,
   '\tJxCan_Ve();\t// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung' + R,
   '\tJxLia_Ve();\t// [CAMERA 13/09] chu "Nhin rong NNN %" 1,5 s khi zoom doi' + R +
   '\tJxCan_Ve();\t// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung' + R,
   'JxLia_Ve();\t// [CAMERA 13/09]')

# ---------- config.ini: khoa moi sau ZoomThu ----------
KHOI = [
    '; [CAMERA 13/09] theo map: settings\\camera_mobile.ini ([Map_<id>] / [MacDinh]); ZoomNho=1 nho zoom nguoi choi qua UserData\\CameraMobile.ini;',
    ';   ZoomTocDo = %/giay khi troi toi zoom cua map luc vao map; ZoomChu=1 hien chu "Nhin rong NNN %" 1,5 s khi doi',
    'ZoomNho=1',
    'ZoomTocDo=100',
    'ZoomChu=1',
]
def va_config(p):
    s = doc(p)
    if 'ZoomNho=' in s:
        KQ.append('bo qua (da co): %s : ZoomNho' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    neo = 'ZoomThu=0' + nl
    if s.count(neo) != 1:
        raise SystemExit('LOI: khong thay ZoomThu=0 duy nhat trong %s' % p)
    ghi(p, s.replace(neo, neo + nl.join(KHOI) + nl))
    KQ.append('da va: %s : khoi [Cham] ZoomNho/ZoomTocDo/ZoomChu' % p)
va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p): va_config(p)
        else: KQ.append('khong co tep: %s' % p)
print('\n'.join(KQ))
