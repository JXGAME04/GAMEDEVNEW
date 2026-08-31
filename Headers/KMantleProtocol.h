#ifndef KMANTLEPROTOCOL_H
#define KMANTLEPROTOCOL_H

//---------------------------------------------------------------------------
// [PFSYNC 31/08] He PHI PHONG - goi dong bo m_nPfPack (sao/lo/da kham) ve client.
//
// Vi sao can: ITEM_SYNC khong mang m_nPfPack nen phia client GetStarLevel /
// GetMaxStoneNum / GetStoneId deu tra 0 -> PF_StarPrefix ("N sao ") va
// PF_AppendDesc (bang mo ta lo kham, KItem.cpp:216-256) cam hoan toan, du
// thuoc tinh VAN an vao nhan vat (server tu ap qua PF_ModifyStoneAttrib).
//
// Vi sao tach tep rieng: trong cay co HAI tep KProtocol.h dung chung include
// guard KPROTOCOL_H (ban that Sources\Core\Src + ban ton dong Headers) - xem
// ghi chu dau Headers\KDiceProtocol.h. Cau truc goi moi phai nam o tep co
// guard rieng va duoc #include tu CA HAI ban.
//
// Chi gui cho vat pham co du lieu phi phong (m_nPfPack[0..2] khac 0) nen vat
// pham thuong khong ton them mot byte nao tren duong truyen.
//---------------------------------------------------------------------------

#pragma pack(push, enter_mantle_protocol)
#pragma pack(1)

typedef struct
{
	BYTE	ProtocolType;	// s2c_syncpfpack
	DWORD	m_dwID;			// dwID vat pham (nhu ITEM_SYNC_MAGIC)
	int		m_nPfPack[4];	// xem chu thich KItem.h:156-160 (sao/chuc phuc/lo | ma da | cap lo | lan dot pha)
} ITEM_SYNC_PFPACK;

#pragma pack(pop, enter_mantle_protocol)

#endif	// KMANTLEPROTOCOL_H
