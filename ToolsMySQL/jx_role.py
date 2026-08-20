# -*- coding: ascii -*-
"""
jx_role.py -- Giai ma blob TRoleData cua JX1 va bo doc file .bak cua Goddess.

Bo cuc da duoc KIEM CHUNG THUC NGHIEM tren 1003/1003 ban ghi cua roledb song:
    dwDataLen == len(blob)          1003/1003
    key.rstrip(NUL) == szName       1003/1003
    (dwDataLen-4-dwItemOffset)/233 == nItemCount
    (dwItemOffset-dwTaskOffset)/8  == nTaskCount
=> TDBItemData=233, TDBSkillData=TDBTaskData=8, va dwDataLen DA BAO GOM 4 byte CRC cuoi.

Nguon bo cuc: D:\\GAMEDEVNEW\\Lib\\S3DBInterface.h (581 dong, #pragma pack(push,1) dong 15).
KHONG dung D:\\GAMEDEVNEW\\Headers\\S3DBInterface.h - do la header CHET (szName[100], khong pack).
"""

import struct
import zlib

BASEINFO_SIZE = 701
HDR_SIZE = 745          # sizeof(TRoleData) truoc pBuffer
ITEM_SIZE = 233
SKILL_SIZE = 8
TASK_SIZE = 8
MAX_DATALEN = 327680    # tran cung trong IDBRoleServer.cpp

# --- offset trong TRoleBaseInfo (pack 1) ---
O_dwId = 0
O_szName = 4
O_bSex = 36
O_szAlias = 37
O_caccname = 69
O_nFirstSect = 101
O_nSect = 102
O_cFightMode = 103
O_cUseRevive = 104
O_cIsExchange = 105
O_cPkStatus = 106
O_irevivalid = 123
O_irevivalx = 127
O_irevivaly = 131
O_ientergameid = 135
O_ientergamex = 139
O_ientergamey = 143
O_cpartnercode = 147
O_isavemoney = 179
O_imoney = 183
O_ifiveprop = 187
O_iteam = 191
O_ifightlevel = 195
O_fightexp = 199        # double 8 byte
O_ileadlevel = 207
O_ileadexp = 211
O_iliveexp = 215
O_ipower = 219
O_iagility = 223
O_iouter = 227
O_iinside = 231
O_iluck = 235
O_imaxlife = 239
O_icurlife = 251
O_ipkvalue = 263
O_nSectStat = 289
O_nWorldStat = 293
O_nKillPeopleNumber = 297
O_nForbiddenFlag = 301
O_dwTongID = 305
O_szStringduphong1 = 413        # ten ban doi
O_szStringduphong2 = 445        # KINH MACH - 8 byte NHI PHAN, co the chua byte 0

# --- offset trong TRoleData sau BaseInfo ---
O_dwVersion = 701
O_bBaseNeedUpdate = 705
O_nFightSkillCount = 706
O_nLiveSkillCount = 708
O_nStateSkillCount = 710
O_nTaskCount = 712
O_nItemCount = 713
O_nFriendCount = 715
O_dwTaskOffset = 717
O_dwLSkillOffset = 721
O_dwFSkillOffset = 725
O_dwSSkillOffset = 729
O_dwItemOffset = 733
O_dwFriendOffset = 737
O_dwDataLen = 741


def _cstr(buf, off, maxlen):
    raw = buf[off:off + maxlen]
    i = raw.find(b'\x00')
    return raw if i < 0 else raw[:i]


class RoleBlobError(Exception):
    pass


def parse(blob, strict=True):
    """Giai ma blob -> dict cac truong dan xuat. KHONG sua doi blob."""
    n = len(blob)
    if n < HDR_SIZE:
        raise RoleBlobError("blob %d byte < header %d" % (n, HDR_SIZE))
    u = lambda fmt, off: struct.unpack_from(fmt, blob, off)[0]

    r = {}
    r['data_len'] = u('<i', O_dwDataLen)
    r['role_name'] = _cstr(blob, O_szName, 32)
    r['acc_name'] = _cstr(blob, O_caccname, 32)
    r['alias'] = _cstr(blob, O_szAlias, 32)
    r['dwId'] = u('<I', O_dwId)
    r['sex'] = blob[O_bSex]
    r['first_sect'] = blob[O_nFirstSect]
    r['sect'] = blob[O_nSect]
    r['use_revive'] = blob[O_cUseRevive]
    r['pk_status'] = blob[O_cPkStatus]
    r['revival_id'] = u('<i', O_irevivalid)
    r['revival_x'] = u('<i', O_irevivalx)
    r['revival_y'] = u('<i', O_irevivaly)
    r['enter_game_id'] = u('<i', O_ientergameid)
    r['enter_game_x'] = u('<i', O_ientergamex)
    r['enter_game_y'] = u('<i', O_ientergamey)
    r['save_money'] = u('<i', O_isavemoney)
    r['money'] = u('<i', O_imoney)
    r['series'] = u('<i', O_ifiveprop)
    r['level'] = u('<i', O_ifightlevel)
    r['fight_exp'] = u('<d', O_fightexp)
    r['lead_level'] = u('<i', O_ileadlevel)
    r['live_exp'] = u('<i', O_iliveexp)
    r['cur_life'] = u('<i', O_icurlife)
    r['max_life'] = u('<i', O_imaxlife)
    r['pk_value'] = u('<i', O_ipkvalue)
    r['sect_stat'] = u('<i', O_nSectStat)
    r['world_stat'] = u('<i', O_nWorldStat)
    r['kill_num'] = u('<i', O_nKillPeopleNumber)
    r['forbidden'] = u('<i', O_nForbiddenFlag)
    r['tong_id'] = u('<I', O_dwTongID)
    r['partner'] = _cstr(blob, O_szStringduphong1, 32)
    r['meridian'] = blob[O_szStringduphong2:O_szStringduphong2 + 8]  # nhi phan tho

    r['version'] = u('<I', O_dwVersion)
    r['n_fight_skill'] = u('<h', O_nFightSkillCount)
    r['n_live_skill'] = u('<h', O_nLiveSkillCount)
    r['n_state_skill'] = u('<h', O_nStateSkillCount)
    r['n_task'] = blob[O_nTaskCount]
    r['n_item'] = u('<h', O_nItemCount)
    r['n_friend'] = u('<h', O_nFriendCount)
    r['off_task'] = u('<i', O_dwTaskOffset)
    r['off_lskill'] = u('<i', O_dwLSkillOffset)
    r['off_fskill'] = u('<i', O_dwFSkillOffset)
    r['off_sskill'] = u('<i', O_dwSSkillOffset)
    r['off_item'] = u('<i', O_dwItemOffset)
    r['off_friend'] = u('<i', O_dwFriendOffset)

    # ---- CO HAI DANG BAN GHI, PHAI TU NHAN DIEN (da kiem chung bang du lieu that) ----
    # Dang A -- da tung duoc GameServer luu: KSOServer.cpp lam dwDataLen += 4 roi noi
    #           4 byte CRC32 vao cuoi. => vung vat pham = [off_item, data_len-4)
    # Dang B -- nhan vat MOI TAO, chua tung duoc luu: PlayerCreator.cpp:354 dat
    #           dwDataLen = (BYTE*)pItemData - pData, tuc KHONG co CRC.
    #           => vung vat pham = [off_item, data_len)
    # Do that tren kho production: 1816 dang A, 3 dang B (CayTien1, THIEULAMCUIBAP,
    # TinheBanDam -- deu la nhan vat cap 1, 1002 byte). Neu ep cung "luon co CRC" thi
    # 3 nguoi choi that BI LOAI IM LANG.
    # Khong the nham lan: neu (X-4) va X cung chia het 233 thi 4 chia het 233 -- vo ly.
    body_a = r['data_len'] - 4 - r['off_item']
    body_b = r['data_len'] - r['off_item']
    if r['off_item'] > 0 and body_a >= 0 and body_a % ITEM_SIZE == 0:
        r['has_crc'] = True
        r['n_item_derived'] = body_a // ITEM_SIZE
    elif r['off_item'] > 0 and body_b >= 0 and body_b % ITEM_SIZE == 0:
        r['has_crc'] = False
        r['n_item_derived'] = body_b // ITEM_SIZE
    else:
        r['has_crc'] = None          # khong xac dinh duoc -> ban ghi hong
        r['n_item_derived'] = -1

    if r['has_crc']:
        r['crc_stored'] = struct.unpack_from('<I', blob, r['data_len'] - 4)[0]
        r['crc_calc'] = zlib.crc32(blob[:r['data_len'] - 4]) & 0xFFFFFFFF
        r['crc_ok'] = (r['crc_stored'] == r['crc_calc'])
    else:
        r['crc_stored'] = 0
        r['crc_calc'] = 0
        r['crc_ok'] = False

    r['n_task_derived'] = (r['off_item'] - r['off_task']) // TASK_SIZE \
        if (r['off_item'] and r['off_task']) else 0

    if strict:
        if r['data_len'] != n:
            raise RoleBlobError("dwDataLen=%d != len(blob)=%d" % (r['data_len'], n))
        if r['has_crc'] is None:
            raise RoleBlobError(
                "vung vat pham khong chia het %d byte (off_item=%d data_len=%d)"
                % (ITEM_SIZE, r['off_item'], r['data_len']))
        # kiem chuoi offset lien tuc: FSkill -> SSkill -> Task -> Item
        if r['off_fskill'] and r['off_fskill'] != HDR_SIZE:
            raise RoleBlobError("dwFSkillOffset=%d != %d" % (r['off_fskill'], HDR_SIZE))
        if r['off_fskill'] + r['n_fight_skill'] * SKILL_SIZE != r['off_sskill']:
            raise RoleBlobError("chuoi offset dut giua FightSkill va StateSkill")
        if r['off_sskill'] + r['n_state_skill'] * SKILL_SIZE != r['off_task']:
            raise RoleBlobError("chuoi offset dut giua StateSkill va Task")
        if r['off_task'] + r['n_task_derived'] * TASK_SIZE != r['off_item']:
            raise RoleBlobError("chuoi offset dut giua Task va Item")
    return r


def recalc_crc(blob):
    """Tra ve blob MOI voi 4 byte CRC32 cuoi duoc tinh lai. Khong sua blob goc."""
    if len(blob) < 4:
        raise RoleBlobError("blob qua ngan")
    body = blob[:-4]
    return body + struct.pack('<I', zlib.crc32(body) & 0xFFFFFFFF)


def iter_items(blob):
    """Sinh (seq, ilocal, raw233) cho tung vat pham. ilocal = INVENTORY_ROOM."""
    r = parse(blob, strict=False)
    off = r['off_item']
    cnt = int(r['n_item_derived'])
    for i in range(cnt):
        o = off + i * ITEM_SIZE
        if o + ITEM_SIZE > len(blob) - 4:
            break
        yield i, blob[o], blob[o:o + ITEM_SIZE]


# ---------------------------------------------------------------- .bak reader
def read_bak(path, size_t=4, verbose=False):
    """Doc file .bak cua Goddess voi BA PHEP KIEM CHUNG CHEO bat buoc.

    Khuon: [size_t keysize][key][size_t datasize][data] lap lai.
    Ba phep kiem (theo canh bao R9): keysize<=32 VA dwDataLen==datasize
    VA key.rstrip(NUL)==BaseInfo.szName.
    Tra ve (good, rejected) trong do good=[(key,blob)], rejected=[(offset,ly_do)].
    """
    raw = open(path, 'rb').read()
    n = len(raw)
    fmt = '<I' if size_t == 4 else '<Q'
    pos = 0
    good = []
    rejected = []
    while pos + size_t <= n:
        start = pos
        ksz = struct.unpack_from(fmt, raw, pos)[0]
        pos += size_t
        if ksz == 0 or ksz > 32 or pos + ksz + size_t > n:
            rejected.append((start, "keysize=%d khong hop le" % ksz))
            # truot 1 byte de tim lai dong bo
            pos = start + 1
            continue
        key = raw[pos:pos + ksz]
        pos += ksz
        dsz = struct.unpack_from(fmt, raw, pos)[0]
        pos += size_t
        if dsz < HDR_SIZE or dsz > MAX_DATALEN or pos + dsz > n:
            rejected.append((start, "datasize=%d khong hop le" % dsz))
            pos = start + 1
            continue
        blob = raw[pos:pos + dsz]
        pos += dsz
        try:
            dl = struct.unpack_from('<i', blob, O_dwDataLen)[0]
        except Exception:
            rejected.append((start, "blob qua ngan de doc dwDataLen"))
            pos = start + 1
            continue
        if dl != dsz:
            rejected.append((start, "dwDataLen=%d != datasize=%d" % (dl, dsz)))
            pos = start + 1
            continue
        name = _cstr(blob, O_szName, 32)
        if key.rstrip(b'\x00') != name:
            rejected.append((start, "key=%r != szName=%r" % (key, name)))
            pos = start + 1
            continue
        good.append((key, blob))
    if verbose:
        print("read_bak: %d ban ghi hop le, %d diem bi tu choi" % (len(good), len(rejected)))
    return good, rejected
