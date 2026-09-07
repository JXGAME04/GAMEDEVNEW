# -*- coding: utf-8 -*-
# (chu thich tieng Viet co dau chi o day; MOI CHUOI CHEN VAO C++ deu ASCII thuan)
# goi_va_matdo_goc_0709.py - vá TẬN GỐC vụ "mất đồ người chơi" (CaiBang 06/09).
# Gốc: m_Hand LỆCH -> InsertEquipment ném Item[m_Hand] (item của NGƯỜI KHÁC) ra đất -> KObj::Release
# giải phóng -> chủ thật mất đồ; entry treo thành "bản ghi ma" trong bản lưu (xem
# PHANTICH_MATDO_CAIBANG_GOC_THAT_0609.md).
#
# 1. KItemList::Hand() tự chữa: chỉ trả chỉ số THẬT SỰ đang ở tay (entry pos_hand); lệch -> 0 + log.
#    Mọi chỗ "nem mon tren tay" (10 chỗ) đều qua Hand() nên được chặn một lượt.
# 2. InsertEquipment: chỉ ném khi Hand() hợp lệ VÀ Remove() xác nhận danh sách này đang giữ món.
# 3. ExchangeItem (server, chống hack): m_Hand không có trong danh sách -> xoá (trước treo tới khi vào lại).
# 4. KObjSet::Add (server): từ chối tạo vật rơi cho chỉ số item RỖNG / ngoài biên.
# 5. AddKIL (nạp DB): trùng chỗ (tay / ô trang bị / dự phòng / mọi lưới) -> nếu CÙNG MỘT MÓN với món
#    đang ở đó = bản ghi KÉP -> bỏ + log (không nhân bản); khác -> cứu sang chỗ trống + log;
#    hết chỗ mới bỏ (có log). Món "đang mặc" không hợp ô (bùa, mũ ở ô giày) cũng đi đường cứu.
#
# Idempotent: chạy lại không đổi gì. Chỉ chèn ASCII, đọc/ghi latin-1 (byte 1:1), kiểm số byte cao.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = os.path.join(ROOT, 'Sources', 'Core', 'Src', 'KItemList.cpp')
HDR = os.path.join(ROOT, 'Sources', 'Core', 'Src', 'KItemList.h')
OBJ = os.path.join(ROOT, 'Sources', 'Core', 'Src', 'KObjSet.cpp')
NL = '\r\n'


def L(*lines):
    return NL.join(lines) + NL


def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()


def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)


def byte_cao(s):
    return sum(1 for c in s if ord(c) >= 128)


class Va(object):
    def __init__(self, p):
        self.p = p
        self.s = doc(p)
        self.cao = byte_cao(self.s)
        self.n = 0
        self.bo = 0

    def thay(self, old, new, dau_moi):
        """thay 'old' (duy nhat) bang 'new'; 'dau_moi' = chuoi chi co trong 'new' de nhan biet da va."""
        if dau_moi in self.s:
            self.bo += 1
            return
        c = self.s.count(old)
        if c != 1:
            raise SystemExit('NEO %r thay %d lan trong %s' % (old[:60], c, self.p))
        self.s = self.s.replace(old, new)
        self.n += 1

    def thay_doan(self, dau, cuoi, new, dau_moi):
        """thay doan [dau, cuoi) (cuoi giu nguyen) bang 'new'."""
        if dau_moi in self.s:
            self.bo += 1
            return
        if self.s.count(dau) != 1:
            raise SystemExit('NEO DAU %r thay %d lan trong %s' % (dau[:60], self.s.count(dau), self.p))
        a = self.s.index(dau)
        b = self.s.find(cuoi, a)
        if b < 0 or b - a > 4000:
            raise SystemExit('NEO CUOI %r khong thay sau NEO DAU trong %s' % (cuoi[:60], self.p))
        self.s = self.s[:a] + new + self.s[b:]
        self.n += 1

    def luu(self):
        assert all(ord(c) < 128 for c in self.s) or True
        if byte_cao(self.s) != self.cao:
            raise SystemExit('SO BYTE CAO DOI trong %s: %d -> %d - KHONG GHI' % (self.p, self.cao, byte_cao(self.s)))
        ghi(self.p, self.s)
        print('%s: va %d hunk, bo qua (da co) %d, byte cao %d giu nguyen' % (os.path.basename(self.p), self.n, self.bo, self.cao))


# ------------------------------------------------------------------ KItemList.h
h = Va(HDR)
h.thay(
    '\tint\t\t\tHand() { return m_Hand; };' + NL,
    L('\tint\t\t\tHand();\t\t// [MATDO 07/09] tu chua m_Hand lech - xem KItemList.cpp',
      '\tBOOL\t\tCungMotMon(int nIdx1, int nIdx2);\t\t// [MATDO 07/09] hai KItem = MOT mon (ban ghi kep)',
      '\tBOOL\t\tMonKepTrongLuoi(int nRoom, int nIdx, int nX, int nY);\t// [MATDO 07/09]',
      '\tint\t\t\tCuuMonKhiNap(int i, int nIdx, int& nPlace, int& nX, int& nY, int nRoomUuTien, const char* szLyDo);\t// [MATDO 07/09]'),
    'CuuMonKhiNap(int i')
h.luu()

# ------------------------------------------------------------------ KItemList.cpp
c = Va(CPP)

HELPERS = L(
    '// ===================== [MATDO 07/09] chong mat do / nhan ban do =====================',
    '// Goc vu mat do 06/09 (PHANTICH_MATDO_CAIBANG_GOC_THAT_0609.md): m_Hand LECH = tro vao chi so',
    '// khong con la entry pos_hand cua danh sach nay (Equip cu khong xoa m_Hand; chi so da duoc',
    '// giai phong roi cap lai cho nguoi khac). InsertEquipment nem Item[m_Hand] ra dat -> KObj::Release',
    '// giai phong -> CHU THAT mat do; entry cua chu that treo -> ban luu ghi "ban ghi ma".',
    'static const char* KIL_Ten(int nPlayerIdx)',
    '{',
    '\tif (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)',
    '\t\treturn Player[nPlayerIdx].m_PlayerName;',
    '\treturn "?";',
    '}',
    '',
    '// Ghi logs\\hethong.log the [MATDO] (g_DebugLog cua Engine chi gui cua so debug - may chu that KHONG thay).',
    'static void KIL_Log(const char* szFmt, ...)',
    '{',
    '\tchar sz[1024];',
    '\tva_list va;',
    '\tva_start(va, szFmt);',
    '\tint n = _vsnprintf(sz, sizeof(sz) - 1, szFmt, va);',
    '\tva_end(va);',
    '\tif (n < 0)',
    '\t\tn = (int)sizeof(sz) - 1;',
    '\tsz[n] = 0;',
    '\tg_GhiLogHeThong("MATDO", sz);',
    '\tg_DebugLog("[MATDO] %s", sz);',
    '}',
    '',
    '// Hand(): chi tra chi so THAT SU dang o tay; lech thi tu chua ve 0 + ghi log. Moi noi lay mon',
    '// tren tay de nem/xu ly deu qua ham nay -> chan mot luot ca 10 cho "nem mon tren tay".',
    'int KItemList::Hand()',
    '{',
    '\tif (m_Hand)',
    '\t{',
    '\t\tint nList = FindSame(m_Hand);',
    '\t\tif (!nList || m_Items[nList].nPlace != pos_hand)',
    '\t\t{',
    '\t\t\tKIL_Log("%s: m_Hand=%d LECH (entry=%d place=%d) -> xoa", KIL_Ten(m_PlayerIdx), m_Hand, nList, nList ? m_Items[nList].nPlace : -1);',
    '\t\t\tm_Hand = 0;',
    '\t\t}',
    '\t}',
    '\treturn m_Hand;',
    '}',
    '',
    '// Hai KItem la MOT mon (ban ghi KEP trong ban luu): cung toan bo thuoc tinh sinh + seed + so chong + do ben.',
    'BOOL KItemList::CungMotMon(int nIdx1, int nIdx2)',
    '{',
    '\tif (nIdx1 <= 0 || nIdx2 <= 0 || nIdx1 >= MAX_ITEM || nIdx2 >= MAX_ITEM)',
    '\t\treturn FALSE;',
    '\tKItem& a = Item[nIdx1];',
    '\tKItem& b = Item[nIdx2];',
    '\tif (a.GetGenre() != b.GetGenre() || a.GetDetailType() != b.GetDetailType()',
    '\t || a.GetParticular() != b.GetParticular() || a.GetLevel() != b.GetLevel()',
    '\t || a.GetSeries() != b.GetSeries() || a.GetGoldId() != b.GetGoldId()',
    '\t || a.GetNature() != b.GetNature() || a.GetStackNum() != b.GetStackNum()',
    '\t || a.GetDurability() != b.GetDurability())',
    '\t\treturn FALSE;',
    '\tKItemGeneratorParam* pa = a.GetItemParam();',
    '\tKItemGeneratorParam* pb = b.GetItemParam();',
    '\tif (pa->uRandomSeed != pb->uRandomSeed || pa->nVersion != pb->nVersion || pa->nLuck != pb->nLuck)',
    '\t\treturn FALSE;',
    '\tif (memcmp(pa->nGeneratorLevel, pb->nGeneratorLevel, sizeof(pa->nGeneratorLevel)) != 0)',
    '\t\treturn FALSE;',
    '\treturn TRUE;',
    '}',
    '',
    '// O luoi (nX,nY) cua room dang co CHINH mon nay (ban ghi kep) -> TRUE + log; nguoi goi bo ban ghi.',
    'BOOL KItemList::MonKepTrongLuoi(int nRoom, int nIdx, int nX, int nY)',
    '{',
    '\tif (nRoom < 0 || nRoom >= room_num)',
    '\t\treturn FALSE;',
    '\tconst int nO = m_Room[nRoom].FindItem(nX, nY);',
    '\tif (nO > 0 && CungMotMon(nIdx, nO))',
    '\t{',
    '\t\tKIL_Log("%s: ban ghi KEP trong luoi room %d o (%d,%d) (item %d = %d) -> bo", KIL_Ten(m_PlayerIdx), nRoom, nX, nY, nIdx, nO);',
    '\t\treturn TRUE;',
    '\t}',
    '\treturn FALSE;',
    '}',
    '',
    '// Cuu mon khi nap DB ma cho cu bi chiem / khong hop: thu room uu tien (chinh container do), roi',
    '// hanh trang, roi ruong. Dat entry i va tra place/x/y moi qua tham chieu de AddKIL dung tiep.',
    '// Tra 0 = het cho -> nguoi goi bo mon (da co log, khong con "mat im lang").',
    'int KItemList::CuuMonKhiNap(int i, int nIdx, int& nPlace, int& nX, int& nY, int nRoomUuTien, const char* szLyDo)',
    '{',
    '\tstatic const int aMacDinh[2] = { room_equipment, room_repository };',
    '\tconst int nW = Item[nIdx].GetWidth(), nH = Item[nIdx].GetHeight();',
    '\tint aThu[3];',
    '\tint nThu = 0;',
    '\tif (nRoomUuTien >= 0 && nRoomUuTien < room_num)',
    '\t\taThu[nThu++] = nRoomUuTien;',
    '\tfor (int k = 0; k < 2; k++)',
    '\t\tif (aMacDinh[k] != nRoomUuTien)',
    '\t\t\taThu[nThu++] = aMacDinh[k];',
    '\tfor (int t = 0; t < nThu; t++)',
    '\t{',
    '\t\tconst int nRoom = aThu[t];',
    '\t\tint nPos;',
    '\t\tswitch (nRoom)',
    '\t\t{',
    '\t\tcase room_equipment:\tnPos = pos_equiproom;\t\tbreak;',
    '\t\tcase room_repository:\tnPos = pos_repositoryroom;\tbreak;',
    '\t\tcase room_exbox1:\t\tnPos = pos_exbox1room;\t\tbreak;',
    '\t\tcase room_exbox2:\t\tnPos = pos_exbox2room;\t\tbreak;',
    '\t\tcase room_exbox3:\t\tnPos = pos_exbox3room;\t\tbreak;',
    '\t\tcase room_equipmentex:\tnPos = pos_equiproomex;\t\tbreak;',
    '\t\tdefault:\t\t\t\tcontinue;',
    '\t\t}',
    '\t\tPOINT pt;',
    '\t\tif (!m_Room[nRoom].FindRoom(nW, nH, &pt))',
    '\t\t\tcontinue;',
    '\t\tif (!m_Room[nRoom].PlaceItem((int)pt.x, (int)pt.y, nIdx, nW, nH))',
    '\t\t\tcontinue;',
    '\t\tKIL_Log("%s: CUU mon %s (genre %d dt %d pt %d cap %d seed %u) tu %s (place %d,%d,%d) -> place %d (%d,%d)",',
    '\t\t\tKIL_Ten(m_PlayerIdx), Item[nIdx].GetName(), Item[nIdx].GetGenre(), Item[nIdx].GetDetailType(),',
    '\t\t\tItem[nIdx].GetParticular(), Item[nIdx].GetLevel(), Item[nIdx].GetItemParam()->uRandomSeed,',
    '\t\t\tszLyDo, nPlace, nX, nY, nPos, (int)pt.x, (int)pt.y);',
    '\t\tnPlace = nPos;',
    '\t\tnX = (int)pt.x;',
    '\t\tnY = (int)pt.y;',
    '\t\tm_Items[i].nPlace = nPos;',
    '\t\tm_Items[i].nX = nX;',
    '\t\tm_Items[i].nY = nY;',
    '\t\treturn 1;',
    '\t}',
    '\tKIL_Log("%s: BO mon %s (genre %d dt %d pt %d cap %d seed %u) tu %s (place %d,%d,%d): het cho cuu",',
    '\t\tKIL_Ten(m_PlayerIdx), Item[nIdx].GetName(), Item[nIdx].GetGenre(), Item[nIdx].GetDetailType(),',
    '\t\tItem[nIdx].GetParticular(), Item[nIdx].GetLevel(), Item[nIdx].GetItemParam()->uRandomSeed,',
    '\t\tszLyDo, nPlace, nX, nY);',
    '\treturn 0;',
    '}',
    '')

ADDKIL_HEAD = 'int KItemList::AddKIL(int nIdx, int nPlace, int nX, int nY, BOOL bInit, BOOL bBreak)' + NL + '{' + NL
c.thay(ADDKIL_HEAD, HELPERS + ADDKIL_HEAD, 'int KItemList::CuuMonKhiNap(')

# pos_hand
c.thay(
    L('\tcase pos_hand:', '\t\tif (m_Hand)', '\t\t\treturn 0;', '\t\tm_Items[i].nPlace = pos_hand;'),
    L('\tcase pos_hand:',
      '\t\tif (m_Hand)',
      '\t\t{',
      '\t\t\tif (!bInit)\t// [MATDO 07/09] luc CHAY giu hanh vi cu (nguoi goi tu xu ly); chi NAP DB (bInit) moi khu kep / cuu',
      '\t\t\t\treturn 0;',
      '\t\t\t// [MATDO 07/09] ban luu co HAI mon "tren tay" (m_Hand lech luc luu): kep -> bo + log; khac -> cuu.',
      '\t\t\tif (CungMotMon(nIdx, m_Hand))',
      '\t\t\t{',
      '\t\t\t\tKIL_Log("%s: ban ghi KEP tren tay (item %d = %d) -> bo", KIL_Ten(m_PlayerIdx), nIdx, m_Hand);',
      '\t\t\t\treturn 0;',
      '\t\t\t}',
      '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, -1, "tren tay"))',
      '\t\t\t\treturn 0;',
      '\t\t\tbreak;',
      '\t\t}',
      '\t\tm_Items[i].nPlace = pos_hand;'),
    '"tren tay"')

# pos_equip: thay ca khoi H3 cu (tu dong chu thich [MATDO 06/09] O da co mon ... toi truoc m_Items[i].nPlace = pos_equip;)
c.thay_doan(
    '\t\t// [MATDO 06/09] O da co mon (ban luu co HAI mon cung mot o trang bi - xem Equip):' + NL,
    '\t\tm_Items[i].nPlace = pos_equip;' + NL,
    L('\t\t// [MATDO 07/09] O da co mon, HOAC mon khong hop o (bua "dang mac", mu o o giay... = ban ghi MA do entry',
      '\t\t// treo chi so - xem PHANTICH_MATDO_CAIBANG_GOC_THAT_0609.md). Cung mon voi mon dang o o = ban ghi KEP',
      '\t\t// -> bo + log (khong nhan ban); khac -> cuu vao hanh trang/ruong + log; het cho moi bo (co log).',
      '\t\tif (m_EquipItem[nX] || (bInit && (item_equip != Item[nIdx].GetGenre() || !Fit(nIdx, nX))))',
      '\t\t{',
      '\t\t\tif (!bInit)\t// luc CHAY: giu hanh vi cu',
      '\t\t\t\treturn 0;',
      '\t\t\tif (m_EquipItem[nX] && CungMotMon(nIdx, m_EquipItem[nX]))',
      '\t\t\t{',
      '\t\t\t\tKIL_Log("%s: ban ghi KEP o trang bi %d (item %d = %d) -> bo", KIL_Ten(m_PlayerIdx), nX, nIdx, m_EquipItem[nX]);',
      '\t\t\t\treturn 0;',
      '\t\t\t}',
      '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, -1, "o trang bi"))',
      '\t\t\t\treturn 0;',
      '\t\t\tbreak;',
      '\t\t}'),
    '"o trang bi"')

# pos_equipback
c.thay(
    L('\t\tif (m_AltEquipmentItem[nX])', '\t\t\treturn 0;', '\t\tm_Items[i].nPlace = pos_equipback;'),
    L('\t\tif (m_AltEquipmentItem[nX] || (bInit && (item_equip != Item[nIdx].GetGenre() || !Fit(nIdx, nX))))',
      '\t\t{',
      '\t\t\tif (!bInit)\t// luc CHAY: giu hanh vi cu',
      '\t\t\t\treturn 0;',
      '\t\t\t// [MATDO 07/09] nhu pos_equip: kep -> bo + log; khac -> cuu vao hanh trang/ruong.',
      '\t\t\tif (m_AltEquipmentItem[nX] && CungMotMon(nIdx, m_AltEquipmentItem[nX]))',
      '\t\t\t{',
      '\t\t\t\tKIL_Log("%s: ban ghi KEP o du phong %d (item %d = %d) -> bo", KIL_Ten(m_PlayerIdx), nX, nIdx, m_AltEquipmentItem[nX]);',
      '\t\t\t\treturn 0;',
      '\t\t\t}',
      '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, -1, "o du phong"))',
      '\t\t\t\treturn 0;',
      '\t\t\tbreak;',
      '\t\t}',
      '\t\tm_Items[i].nPlace = pos_equipback;'),
    '"o du phong"')

# pos_equiproom: thay khoi H4 cu
c.thay_doan(
    '\t\t// [MATDO 06/09] trung o luoi (hai mon cung toa do trong ban luu) -> TRUOC DAY bo mon im' + NL,
    '\t\tm_Items[i].nPlace = pos_equiproom;' + NL,
    L('\t\t// [MATDO 07/09] trung o luoi: cung mon voi mon dang o o = ban ghi KEP -> bo + log (khong nhan ban);',
      '\t\t// khac -> cuu sang o trong khac (hanh trang, roi ruong) + log; het cho moi bo (co log).',
      '\t\tif (!m_Room[room_equipment].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))',
      '\t\t{',
      '\t\t\tif (!bInit)\t// luc CHAY (nhat do, script...): giu hanh vi cu - nguoi goi tu xu ly that bai',
      '\t\t\t\treturn 0;',
      '\t\t\tif (MonKepTrongLuoi(room_equipment, nIdx, nX, nY))',
      '\t\t\t\treturn 0;',
      '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, room_equipment, "hanh trang"))',
      '\t\t\t\treturn 0;',
      '\t\t\tbreak;',
      '\t\t}'),
    '"hanh trang"))')

# pos_repositoryroom: thay khoi H4 cu
c.thay_doan(
    '\t\t// [MATDO 06/09] nhu tren: trung o trong ruong thi tim o trong khac, khong bo mon.' + NL,
    '\t\tm_Items[i].nPlace = pos_repositoryroom;' + NL,
    L('\t\t// [MATDO 07/09] nhu hanh trang: kep -> bo + log; khac -> cuu (ruong, roi hanh trang) + log.',
      '\t\tif (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))',
      '\t\t{',
      '\t\t\tif (!bInit)\t// luc CHAY: giu hanh vi cu',
      '\t\t\t\treturn 0;',
      '\t\t\tif (MonKepTrongLuoi(room_repository, nIdx, nX, nY))',
      '\t\t\t\treturn 0;',
      '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, room_repository, "ruong"))',
      '\t\t\t\treturn 0;',
      '\t\t\tbreak;',
      '\t\t}'),
    '"ruong"))')

# exbox1..3 + hanh trang mo rong: cung khuon
for room, ten in (('room_exbox1', 'ruong mo rong 1'), ('room_exbox2', 'ruong mo rong 2'),
                  ('room_exbox3', 'ruong mo rong 3'), ('room_equipmentex', 'hanh trang mo rong')):
    old = L('\t\tif (!m_Room[%s].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))' % room,
            '\t\t\treturn 0;')
    new = L('\t\tif (!m_Room[%s].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))' % room,
            '\t\t{',
            '\t\t\tif (!bInit)\t// luc CHAY: giu hanh vi cu',
            '\t\t\t\treturn 0;',
            '\t\t\tif (MonKepTrongLuoi(%s, nIdx, nX, nY))\t// [MATDO 07/09] NAP DB: kep -> bo; khac -> cuu; co log' % room,
            '\t\t\t\treturn 0;',
            '\t\t\tif (!CuuMonKhiNap(i, nIdx, nPlace, nX, nY, %s, "%s"))' % (room, ten),
            '\t\t\t\treturn 0;',
            '\t\t\tbreak;',
            '\t\t}')
    c.thay(old, new, 'CuuMonKhiNap(i, nIdx, nPlace, nX, nY, %s,' % room)

# ExchangeItem (server) chong hack: tu chua m_Hand
c.thay(
    L('\t\tprintf("Hack ExchangeItem m_Hand [%s] [%s]\\n",Player[m_PlayerIdx].m_AccoutName,Player[m_PlayerIdx].m_PlayerName);',
      '\t\treturn;'),
    L('\t\tprintf("Hack ExchangeItem m_Hand [%s] [%s]\\n",Player[m_PlayerIdx].m_AccoutName,Player[m_PlayerIdx].m_PlayerName);',
      '\t\tKIL_Log("%s: m_Hand=%d khong co trong danh sach -> xoa (truoc day treo toi khi vao lai)", KIL_Ten(m_PlayerIdx), m_Hand);',
      '\t\tm_Hand = 0;\t// [MATDO 07/09] tu chua',
      '\t\treturn;'),
    'khong co trong danh sach -> xoa')

# InsertEquipment: chi nem khi Hand() hop le va Remove() xac nhan so huu
c.thay(
    L('\t\tint\tnIndex = m_Hand;', '\t\tif (nIndex)', '\t\t{', '\t\t\tRemove(nIndex);'),
    L('\t\t// [MATDO 07/09] GOC MAT DO 06/09: m_Hand lech -> nem Item[m_Hand] = item cua NGUOI KHAC ra dat,',
      '\t\t// KObj::Release giai phong -> chu that mat do. Chi nem khi Hand() hop le VA Remove() xac nhan',
      '\t\t// danh sach nay dang giu mon do; khong thi bo qua (mon moi van vao tay o duoi).',
      '\t\tint\tnIndex = Hand();',
      '\t\tif (nIndex && Remove(nIndex))',
      '\t\t{'),
    'if (nIndex && Remove(nIndex))')
c.luu()

# ------------------------------------------------------------------ KObjSet.cpp (server)
o = Va(OBJ)
o.thay(
    L('\tif (sItemInfo.m_nItemID > 0)',
      '\t{',
      '\t\tg_DebugLog("[ITEM]Object ItemIndex:%d, ID:%d", sItemInfo.m_nItemID, Item[sItemInfo.m_nItemID].GetID());',
      '\t}'),
    L('\tif (sItemInfo.m_nItemID > 0)',
      '\t{',
      '\t\t// [MATDO 07/09] chi so item RONG (da ItemSet.Remove) hoac ngoai bien -> KHONG tao vat roi: vat roi',
      '\t\t// "loi item" nhat vao thanh entry treo, het han thi KObj::Release giai phong NHAM chi so da tai cap.',
      '\t\tif (sItemInfo.m_nItemID >= MAX_ITEM || Item[sItemInfo.m_nItemID].GetID() == 0)',
      '\t\t{',
      '\t\t\tchar szMatDo[160];',
      '\t\t\t_snprintf(szMatDo, sizeof(szMatDo) - 1, "KObjSet::Add tu choi: item %d rong/ngoai bien (dataid=%d)", sItemInfo.m_nItemID, nDataID);',
      '\t\t\tszMatDo[sizeof(szMatDo) - 1] = 0;',
      '\t\t\tg_GhiLogHeThong("MATDO", szMatDo);\t// logs\\hethong.log (g_DebugLog chi ra cua so debug)',
      '\t\t\tg_DebugLog("[MATDO] %s", szMatDo);',
      '\t\t\treturn -1;',
      '\t\t}',
      '\t\tg_DebugLog("[ITEM]Object ItemIndex:%d, ID:%d", sItemInfo.m_nItemID, Item[sItemInfo.m_nItemID].GetID());',
      '\t}'),
    'KObjSet::Add tu choi')
o.luu()

# ------------------------------------------------------------------ KPlayerDBFuns.cpp (nap DB -> bInit = TRUE)
# Chi duong NAP DB duoc phep khu kep / cuu mon; luc chay (nhat do, script, c2sdnmbr...) AddKIL giu
# hanh vi cu (that bai -> nguoi goi tu xu ly). Do that 07/09 08:05-09:29: 12 lan nhat do cua
# CaiBang bi cuu vi client de xuat o tui da bi chiem -> mon roi vao ruong, doi hanh vi ngoai y dinh.
DB = os.path.join(ROOT, 'Sources', 'Core', 'Src', 'KPlayerDBFuns.cpp')
d = Va(DB)
d.thay(
    '\t\tm_ItemList.AddKIL(nIndex, nLocal, nItemX, nItemY);' + NL,
    L('\t\tm_ItemList.AddKIL(nIndex, nLocal, nItemX, nItemY, TRUE);\t// [MATDO 07/09] bInit = NAP DB: duoc khu ban ghi kep / cuu mon trung o (xem AddKIL)'),
    'nItemX, nItemY, TRUE);')
d.luu()
print('XONG')
