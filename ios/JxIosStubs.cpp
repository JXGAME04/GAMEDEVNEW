//---------------------------------------------------------------------------
// [IOS 11/09] Ban RONG cho cac ham cua BANG DO NHIP / HIEU NANG.
//
// Ban Android cai chung o Platform/JxPerfHudAndroid.cpp, nhung tep do doc /proc va /sys
// (CPU, RAM, pin, nhiet) - iOS khong co. Sau dot [JXMOBILE 11/09] cac cho GOI chung da
// chuyen sang rao JX_MOBILE nen ban iOS cung goi toi, vi vay phai co dinh nghia.
//
// Bang do chi de GO LOI; de rong o day la dung y: khong do gi, khong ve gi, khong ton gi.
// Khi nao can bang do cho iOS thi viet ban dung API cua Apple (task_info, ProcessInfo.thermalState)
// - xem PHANTICH_IOS_LOTRINH_1109.md muc 3.5.
//---------------------------------------------------------------------------
void JxDoNhip_Vong(void)                                   {}
void JxDoNhip_KhungVe(int nCatNgang)                       { (void)nCatNgang; }
void JxNhip_DatMuc(int nMuc)                               { (void)nMuc; }
void JxNhip_ChuMuc(int nMuc, char* sz, int n)              { (void)nMuc; if (sz && n > 0) sz[0] = 0; }
void JxNhip_VeNen(int nX, int nY, int nRong, int nCao)     { (void)nX; (void)nY; (void)nRong; (void)nCao; }
