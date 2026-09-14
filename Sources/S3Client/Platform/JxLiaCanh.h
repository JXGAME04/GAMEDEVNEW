//---------------------------------------------------------------------------
// [LIA 13/09] LIA CANH (pan) cho ban mobile - chep cam giac camera cua Kiem Vong Giang Ho (Unity 3D,
// D:\game3gTQ, xem PHANTICH_KIEMVONG_GIANGHO_3D_1309.md muc 3-4):
//   ho: MOT ngon keo tren the gioi = quay camera (giao dien an cham truoc), tha tay cho 1 s roi
//       camera tu troi ve sau lung nhan vat (40 do/s); hai ngon chum = zoom (buoc 2).
//   Ban 2D nay khong quay duoc (sprite 8 huong, nen ve san) -> LIA: dich TIEU DIEM khung nhin theo tay,
//   tha tay cho LiaChoVeMs roi troi ve nhan vat; nhan vat buoc di / cam can / doi map -> ve ngay.
//
// Cach lam: dung co che CO SAN cua JX1 - KScenePlaceC::FollowMapMove + SetMapFocusPositionOffset qua
// g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP / GSMOI_SCENE_MAP_FOCUS_OFFSET). Ban PC
// dung dung cai nay cho Ctrl + chuot phai keo ban do nho (UiMiniMap::MapScroll, 3 s nhay ve). Dich tieu
// diem -> LookAt, vung truy van vat the, nap vung, cham chon NPC deu theo -> KHONG dung phep ve
// (khong lap lai 4 loi zoom 12/09). Bay: khi co FollowWithMap bat, KScenePlaceC::SetFocusPosition chi
// ghi m_OrigFocusPosition (khung KHONG theo nhan vat) -> JxLia_Nhip() ap lai do lech MOI KHUNG.
// Don vi: X the gioi = px khung ve; Y the gioi = 2 x px (KRepresentShell3::LookAt: m_nTop = nY/2 ...).
//
// Cham (KSdlApp): ngon keo qua nguong tren BAN DO (ngoai giao dien, ngoai vung can, ngoai nut ky nang)
// = CHAM_LIA. Cham thuong, giu lau (chuot phai), can dieu khien, nut ky nang, keo tren giao dien: nhu cu.
// Truoc 13/09 keo tren ban do = giu chuot trai roi re (di lien tuc); tat lia bang [Cham] LiaCanh=0 thi ve nhu cu.
//
// Chi bien dich khi JX_MOBILE. PC khong doi.
//---------------------------------------------------------------------------
#ifndef JxLiaCanh_H
#define JxLiaCanh_H
#ifdef JX_MOBILE

// [Cham] LiaCanh (mac dinh 1). 0 = tat han: KSdlApp khong bao gio vao CHAM_LIA.
bool JxLia_Bat();
// (x, y) - toa do KHUNG VE, cho dat ngon - co duoc bat dau lia tu day khong: dang trong the gioi, khong sua
// giao dien, ngoai giao dien, ngoai vung can dieu khien (va khong dang cam can), khong trung nut ky nang.
bool JxLia_DuocBatDau(int x, int y);
// Bat dau keo tu (x, y). Neu dang cho ve / dang ve thi giu do lech hien co va keo tiep.
void JxLia_BatDau(int x, int y);
// Ngon di chuyen toi (x, y): dich tieu diem nguoc chieu tay (keo sang phai = xem phan ben trai).
void JxLia_Keo(int x, int y);
// Nha ngon: cho LiaChoVeMs roi troi ve (nhan vat dang di / cam can -> ve ngay khi LiaVeKhiDi=1).
void JxLia_Nha();
// Goi moi vong lap game (KSdlApp::Run, canh JxCan_Nhip): ap lai do lech, dem cho, troi ve.
void JxLia_Nhip();
// Doi map that (GDCNI_SWITCHING_MAPMODE) / roi the gioi: bo do lech, tat co FollowWithMap.
void JxLia_DatLai();
// 1 = dang lech (dang keo, dang cho ve hoac dang ve).
int  JxLia_DangLia();

// [ZOOM 13/09] Chum hai ngon tren ban do = ZOOM NHIN RONG (the gioi ve vao RT to hon khung roi thu nho - Represent3 Rep3_JxTheGioi
// lenh 4/5, giao dien giu nguyen co) + lia theo tam hai ngon. Hai ngon gan nhau = thay rong hon, xa nhau = ve 1:1. Zoom giu toi khi doi.
bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2);	// [Cham] ZoomCanh=1 va ca hai diem deu duoc lia
void JxLia_ChumBatDau(int x1, int y1, int x2, int y2);
void JxLia_ChumKeo(int x1, int y1, int x2, int y2);
void JxLia_ChumNha();
int  JxLia_ZoomLay();				// % (100 = 1:1)
void JxLia_ZoomDat(int nPhanTram);	// dat zoom % (100..ZoomToiDa, lam tron ZoomBuoc): Represent3 + vung truy van vat the (SetRepresentAreaSize)

#endif // JX_MOBILE
#endif
