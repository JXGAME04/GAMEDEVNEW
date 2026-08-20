//////////////////////////////////////////////////////////////////////////////
// KGameKV.h -- Dua du lieu tinh nang dang TEP len MySQL ma KHONG phai viet lai
//              bo tuan tu hoa cua tung tinh nang.
//
// VAN DE: Cong Thanh Chien (jx2citywar.txt), xep hang (jx2ladder.txt), lien minh
// (jx2league.txt), bang xep hang (StatData.dat) va ~10 tep .lua deu co bo doc/ghi
// rieng, viet tay, da chay nhieu nam. Viet lai tung cai la rui ro cao ma khong
// them gia tri gi -- van de cua chung KHONG phai dinh dang, ma la:
//   * tep nam tren mot may, mat dia la mat het;
//   * duong ghi cua Lua (`openfile(f,"w+")` trong script\lib\lib_ham.lua) CAT
//     TRANG roi moi ghi -- mat dien giua chung la mat CA BANG;
//   * khong co lich su, khong quay lui duoc.
//
// CACH LAM: giu nguyen bo doc/ghi cu, chi GAN THEM hai moc:
//   * Ngay SAU khi ghi tep thanh cong  -> PutFile()  (BAT DONG BO, khong chan game)
//   * Luc KHOI DONG truoc khi doc tep  -> GetFileToDisk() (dong bo, mot lan)
//
// Ket qua: MySQL la nguon su that va la ban sao; tep tren dia tro thanh ban dem
// cuc bo. Mat tep -> lan khoi dong sau tu dung lai tu MySQL. Ghi hong -> con
// lich su trong game_kv_history de quay lui.
//////////////////////////////////////////////////////////////////////////////

#ifndef KGAMEKV_H
#define KGAMEKV_H

#ifdef _SERVER

namespace KGameKV
{
    // Ghi mot chuoi byte bat ky vao game_kv. bAsync = true thi day vao hang doi
    // ghi nen (dung trong vong lap game); false thi ghi ngay (luc khoi dong).
    bool Put(const char* szNs, const char* szKey,
             const void* pData, int nLen, bool bAsync = true);

    // Doc ra bo dem cua ben goi. Tra ve so byte doc duoc, -1 neu khong co/loi.
    int  Get(const char* szNs, const char* szKey, void* pBuf, int nCap);

    bool Del(const char* szNs, const char* szKey);

    // Doc TOAN BO mot tep tren dia roi ghi vao game_kv.
    // Goi ngay SAU khi tep da duoc ghi thanh cong.
    bool PutFile(const char* szNs, const char* szKey, const char* szPath,
                 bool bAsync = true);

    // Nguoc lai: lay tu game_kv ghi ra tep. Ghi qua .tmp roi MoveFileEx nen
    // khong bao gio de lai tep cut do.
    // bChiKhiThieu = true: chi ghi khi tep chua ton tai (khoi phuc sau mat dia).
    bool GetFileToDisk(const char* szNs, const char* szKey, const char* szPath,
                       bool bChiKhiThieu = true);

    // Ghi mot dong nhat ky vao game_log. LUON bat dong bo -- day la duong nong
    // nhat, moi ngay hang chuc nghin dong.
    bool Log(const char* szLoai, const char* szRole, const void* pNoiDung, int nLen);
    bool LogStr(const char* szLoai, const char* szRole, const char* szNoiDung);
}

// Lay mot lenh quan tri CHUA XU LY (cu nhat) trong bang admin_command roi
// danh dau da xu ly, trong MOT giao dich. Thay cho viec doc tep
// dulieu/username_kick.txt roi xoa trang -- cach cu mat lenh neu web ghi
// dung luc script dang xoa.
// Tra true neu co lenh; szArg nhan tham so.
bool KGameKV_LayLenhQuanTri(const char* szCmd, char* szArg, int nCap);

#endif // _SERVER
#endif // KGAMEKV_H
