//---------------------------------------------------------------------------
//  CrashLog.h - bat loi sap game va ghi ra jx_crash.log
//
//  Muc dich: khi game crash (nhat la o map dong nguoi) thi biet CHINH XAC
//  no chet o dau - module nao, lech bao nhieu, ngan xep goi ham ra sao -
//  thay vi phai doan.
//
//  Cai dat: goi CrashLog_Install() cang som cang tot trong WinMain.
//  Ket qua: tep jx_crash.log nam canh Game.exe, GHI THEM moi lan crash.
//---------------------------------------------------------------------------
#ifndef CRASHLOG_H
#define CRASHLOG_H

// Cai bo bat loi (goi 1 lan, dau WinMain).
void CrashLog_Install();

// Ghi mot dong moc vao jx_crash.log de biet game dang lam gi luc chet.
// Dung o cac cho quan trong: doi map, vao map dong... Rat re (chi khi bat).
void CrashLog_Mark(const char* pszWhat, int nValue);

#endif
