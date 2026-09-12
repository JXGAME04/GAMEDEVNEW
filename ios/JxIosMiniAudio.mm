//---------------------------------------------------------------------------
// [IOS-AM 11/09] Phan CAI DAT cua miniaudio cho Apple, tach rieng ra mot don vi bien dich.
//
// Vi sao phai tach: tren iOS, backend Core Audio cua miniaudio include
// <AVFoundation/AVFoundation.h> (de dat phien am thanh), keo theo <objc/objc.h>.
// objc.h khai "typedef bool BOOL", trong khi lop dem Win32 cua game
// (Sources/Engine/Src/Platform/KPosixCompat.h:175) khai "typedef int BOOL".
// Hai khai bao nay dung ten khac kieu -> bien dich hong ngay tu header cua he thong.
// Cung ly do da phai tach ios/JxIosDuongDan.mm truoc day.
//
// Cach lam: tep NAY chi dinh nghia MINIAUDIO_IMPLEMENTATION roi include miniaudio.h,
// TUYET DOI khong include header nao cua game. KSoundMa.cpp van include miniaudio.h nhung
// KHONG dinh nghia MINIAUDIO_IMPLEMENTATION khi o Apple, nen chi lay khai bao - phan khai bao
// khong dung toi AVFoundation.
//
// Danh sach dinh nghia duoi day PHAI GIONG HET trong KSoundMa.cpp: chung quyet dinh
// cau truc cua kieu du lieu miniaudio, lech mot cai la hai don vi bien dich hieu khac nhau.
//---------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_FLAC
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_AAUDIO
#define MA_ENABLE_OPENSL
#define MA_ENABLE_NULL
#define MA_ENABLE_COREAUDIO
#include "../ThirdParty/miniaudio/miniaudio.h"
