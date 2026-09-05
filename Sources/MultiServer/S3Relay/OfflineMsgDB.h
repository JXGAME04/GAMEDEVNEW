//////////////////////////////////////////////////////////////////////////////
// OfflineMsgDB.h  (S3Relay)  [HAOHUU 04/09]
//
// LOI NHAN MAT CHO NGUOI CHOI NGOAI TUYEN -- port tu S3Relay Linux (s3relay_y):
//   KOfflineMsgStore / KQuery_OfflineMsg, bang MySQL
//   OfflineMsg(ID, Receiver, Sender, Msg, LastModify)
//   - nguoi nhan khong truc tuyen -> luu; khi ho dang nhap -> giao het roi xoa
//   - toi da 10 loi nhan / nguoi nhan   (ma may 0x80e65dd: cmp [ebp-20h],9 ; jg bo)
//   - qua 30 ngay thi xoa                (ma may 0x80e5c29: now - 0x278d00 giay)
// Ket noi MySQL doc o DataBase.ini [relaydb] (cung kho relay_kv). Bang tu tao
// (CREATE TABLE IF NOT EXISTS) luc khoi dong relay. Cau hinh o relay_friendcfg.ini:
//   [offlinemsg]
//   enable         = 1      ; 0 = tat, relay chay nhu cu
//   maxperreceiver = 10     ; mac dinh = ban Linux
//   keepdays       = 30     ; mac dinh = ban Linux
//////////////////////////////////////////////////////////////////////////////
#ifndef OFFLINEMSGDB_H
#define OFFLINEMSGDB_H

#define OFFMSG_MAX_LOAD		64		// tran so loi nhan doc mot lan (>= maxperreceiver)
#define OFFMSG_TEXT_MAX		255		// CHAT_SOMEONECHAT_SYNC.sentlen la BYTE
#define OFFMSG_NAME_MAX		32		// _NAME_LEN

struct OFFMSG_REC
{
	char	sender[OFFMSG_NAME_MAX];
	char	text[OFFMSG_TEXT_MAX + 1];
	int		textlen;
	int		year, mon, day, hour, minute;	// gio may chu MySQL luc luu
};

BOOL	OfflineMsg_Init();		// doc cau hinh, noi MySQL, tao bang, xoa loi nhan het han. Loi -> tu tat tinh nang
void	OfflineMsg_Uninit();
BOOL	OfflineMsg_Enabled();
int		OfflineMsg_Store(const char* receiver, const char* sender, const void* msg, int len);	// 1 = da luu
int		OfflineMsg_Load(const char* receiver, OFFMSG_REC* out, int max);						// so ban ghi doc duoc
BOOL	OfflineMsg_Clear(const char* receiver);												// xoa het cua receiver

#endif
