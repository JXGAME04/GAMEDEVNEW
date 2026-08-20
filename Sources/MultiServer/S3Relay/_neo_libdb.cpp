//////////////////////////////////////////////////////////////////////////////
// _neo_libdb.cpp -- TEP NEO, chi de GIU THU VIEN Berkeley DB trong lien ket.
//
// VI SAO CAN: S3Relay.vcxproj dat RuntimeLibrary = MultiThreaded (/MT) NHUNG lai
// IgnoreSpecificDefaultLibraries = libcmt.lib;msvcrt.lib;msvcrtd.lib. Nghia la
// C runtime KHONG den tu chi thi mac dinh cua trinh bien dich, ma den KEM THEO
// libdb181sd.lib (thu vien Berkeley DB tinh). Neu khong con doan ma nao goi
// Berkeley DB, trinh lien ket khong rut gi tu libdb -> khong co CRT ->
// 1.688 loi "unresolved external symbol operator new".
//
// Tep nay goi mot ham vo hai cua Berkeley DB de trinh lien ket VAN phai rut
// libdb vao, giu nguyen Y HET moi truong lien ket cu. Nho vay co the thay
// DBTable.cpp bang ban MySQL ma KHONG phai dong vao cau hinh CRT --
// dung loai thay doi da tung gay hong heap trong du an nay.
//
// Ham nay KHONG BAO GIO duoc goi luc chay.
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "db.h"

extern "C" const char* _NeoGiuLibDb()
{
    int a = 0, b = 0, c = 0;
    return db_version(&a, &b, &c);
}
