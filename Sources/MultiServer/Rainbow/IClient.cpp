#include "IClient.h"

// {D38249A9-4565-4336-9F70-1374F1E158E7}
// [DONTRUNG 11/09] bo ban rieng: trung voi dinh nghia do DEFINE_GUID sinh ra ben
// S3Client/NetConnect/NetConnectAgent.cpp (<initguid.h> + Headers/IClient.h). Gia tri y het.

// {9B2F150C-3D59-40db-A70B-E999D383D044}
// [DONTRUNG 11/09] bo ban rieng: trung voi dinh nghia do DEFINE_GUID sinh ra ben
// S3Client/NetConnect/NetConnectAgent.cpp (<initguid.h> + Headers/IClient.h). Gia tri y het.

// [CULLCPU 11/09] SUA LOI LIEN KET ANDROID sau [DONTRUNG 11/09] (phien iOS): tren Android, Rainbow la mot .so RIENG
// (libRainbow.so, nap luc chay bang dlopen) va KHONG chua NetConnectAgent.cpp, nen hai GUID nay khong con dinh nghia
// nao trong .so -> ld: undefined symbol IID_IESClient / IID_IClientFactory. Tren POSIX, DEFINE_GUID khi KHONG co
// INITGUID chi sinh ra LOI KHAI BAO (KPosixCompat.h:1336). Dinh nghia lai o day, chi cho Android, gia tri y het
// ban goc -> iOS (mot nhi phan duy nhat) van khong bi trung ky hieu, Windows khong doi.
// [DONTRUNG-SUA 11/09] Ba nen ba duong, vi Rainbow duoc dong goi khac nhau o moi nen:
//  - iOS  : MOT nhi phan duy nhat, da co dinh nghia tu NetConnectAgent.cpp -> KHONG dinh nghia lai (neu khong: trung ky hieu).
//  - Android: libRainbow.so RIENG, khong chua NetConnectAgent.cpp -> phai co dinh nghia NGOAI (EXTERN_C), neu khong: undefined symbol.
//  - Windows: Rainbow.dll RIENG, cung khong chua NetConnectAgent.cpp -> GIU NGUYEN Y HET ban goc truoc [DONTRUNG 11/09]
//    (hai dong 'static const GUID'), de ban PC dang phat hanh khong doi mot byte nao.
#ifndef JX_APPLE
#ifdef __ANDROID__
#define JX_ANDROID_GUID 1
EXTERN_C const GUID IID_IESClient =
{ 0xd38249a9, 0x4565, 0x4336, { 0x9f, 0x70, 0x13, 0x74, 0xf1, 0xe1, 0x58, 0xe7 } };
EXTERN_C const GUID IID_IClientFactory =
{ 0x9b2f150c, 0x3d59, 0x40db, { 0xa7, 0xb, 0xe9, 0x99, 0xd3, 0x83, 0xd0, 0x44 } };
#else
// {D38249A9-4565-4336-9F70-1374F1E158E7}
static const GUID IID_IESClient = 
{ 0xd38249a9, 0x4565, 0x4336, { 0x9f, 0x70, 0x13, 0x74, 0xf1, 0xe1, 0x58, 0xe7 } };

// {9B2F150C-3D59-40db-A70B-E999D383D044}
static const GUID IID_IClientFactory = 
{ 0x9b2f150c, 0x3d59, 0x40db, { 0xa7, 0xb, 0xe9, 0x99, 0xd3, 0x83, 0xd0, 0x44 } };
#endif
#endif
