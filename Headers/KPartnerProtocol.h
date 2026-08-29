#ifndef KPARTNERPROTOCOL_H
#define KPARTNERPROTOCOL_H

//---------------------------------------------------------------------------
// [BDH-G4] He BAN DONG HANH - goi lenh tu cua so / thanh nhanh client.
// Tach tep rieng y het KDiceProtocol.h (doc ghi chu dau tep do: hai ban
// KProtocol.h dung chung include guard nen KHONG duoc nhet struct vao do).
//---------------------------------------------------------------------------

#pragma pack(push, enter_partner_protocol)
#pragma pack(1)

#define PARTNER_OP_NAME_LEN     32

// btOp trong goi c2s_partnerop
enum PARTNER_OP
{
    PARTNER_OP_CALLOUT     = 1,     // goi ra / thu ve (toggle)
    PARTNER_OP_SELECT      = 2,     // nParam = so con 1..3
    PARTNER_OP_TALK        = 3,     // mo doi thoai partner_talk.lua
    PARTNER_OP_ATTACK      = 4,     // che do chu dong cong kich
    PARTNER_OP_FOLLOW      = 5,     // che do chi di theo
    PARTNER_OP_RENAME      = 6,     // szName = ten moi (<=16 byte)
    PARTNER_OP_FORGETSKILL = 7,     // (mo qua doi thoai - nhu TALK)
    PARTNER_OP_DELETE      = 8,     // (mo qua doi thoai - nhu TALK)
    PARTNER_OP_PETSYS      = 30,    // [PETSYS] nParam = PET_OPERATION_* (1..7)
};

typedef struct
{
    BYTE    ProtocolType;           // c2s_partnerop
    BYTE    btOp;                   // xem PARTNER_OP
    int     nParam;
    char    szName[PARTNER_OP_NAME_LEN];
} PARTNER_OP_DATA;

#pragma pack(pop, enter_partner_protocol)

#endif  // KPARTNERPROTOCOL_H
