# -*- coding: utf-8 -*-
r"""Bo chuyen Unicode -> TCVN3 (bang ma bytes) + TU KIEM voi chuoi that trong repo.

Luat an toan khi dung cho menu/thoai JX1:
 - CAM '|' (phan cach option cua goi UI_SELECTDIALOG, KPlayer.cpp:7683/7717)
 - CAM '/' trong phan NHAN (strstr '/' dau tien, ScriptFuns.cpp:716)
 - Chu co dau (byte >0x80) KHONG duoc dung NGAY TRUOC '<' va KHONG o CUOI chuoi
   (TEncodeText nuot - Text.cpp:468/470) -> tu them space hy sinh.
"""
import io

M = {
 'à':0xB5,'ả':0xB6,'ã':0xB7,'á':0xB8,'ạ':0xB9,
 'ă':0xA8,'ằ':0xBB,'ẳ':0xBC,'ẵ':0xBD,'ắ':0xBE,'ặ':0xC6,
 'â':0xA9,'ầ':0xC7,'ẩ':0xC8,'ẫ':0xC9,'ấ':0xCA,'ậ':0xCB,
 'đ':0xAE,'Đ':0xA7,
 'è':0xCC,'ẻ':0xCE,'ẽ':0xCF,'é':0xD0,'ẹ':0xD1,
 'ê':0xAA,'ề':0xD2,'ể':0xD3,'ễ':0xD4,'ế':0xD5,'ệ':0xD6,
 'ì':0xD7,'ỉ':0xD8,'ĩ':0xDC,'í':0xDD,'ị':0xDE,
 'ò':0xDF,'ỏ':0xE1,'õ':0xE2,'ó':0xE3,'ọ':0xE4,
 'ô':0xAB,'ồ':0xE5,'ổ':0xE6,'ỗ':0xE7,'ố':0xE8,'ộ':0xE9,
 'ơ':0xAC,'ờ':0xEA,'ở':0xEB,'ỡ':0xEC,'ớ':0xED,'ợ':0xEE,
 'ù':0xEF,'ủ':0xF1,'ũ':0xF2,'ú':0xF3,'ụ':0xF4,
 'ư':0xAD,'ừ':0xF5,'ử':0xF6,'ữ':0xF7,'ứ':0xF8,'ự':0xF9,
 'ỳ':0xFA,'ỷ':0xFB,'ỹ':0xFC,'ý':0xFD,'ỵ':0xFE,
}
# chu HOA nguyen am co dau: game dung ma thuong (font .VnTime ve duoc) - map ve thuong
for k in list(M.keys()):
    if k != 'Đ':
        M[k.upper()] = M[k]

def tcvn3(u):
    """unicode -> bytes TCVN3"""
    out = bytearray()
    for ch in u:
        o = ord(ch)
        if ch in M: out.append(M[ch])
        elif o < 0x80: out.append(o)
        else: raise ValueError("khong co trong bang TCVN3: %r" % ch)
    return bytes(out)

def antoan_nhan(u):
    """chuyen + ap luat an toan: khong '|', khong '/' (nhan), khong dau o cuoi/truoc '<'"""
    assert '|' not in u, "CAM '|' trong chuoi menu"
    b = tcvn3(u)
    # chu dau ngay truoc '<'
    out = bytearray()
    for i, c in enumerate(b):
        if c > 0x80 and i + 1 < len(b) and b[i+1:i+2] == b'<':
            out.append(c); out.append(0x20)
        else:
            out.append(c)
    if out and out[-1] > 0x80:
        out.append(0x20)
    return bytes(out)

if __name__ == "__main__":
    # TU KIEM voi chuoi THAT trong repo
    src = io.open(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\task\tollgate\messenger\messenger_turerukou.lua", 'rb').read()
    tests = [
        ("Tiếp tục nhiệm vụ", src),
        ("Rời khỏi khu vực",  src),
        ("Kết thúc đối thoại", io.open(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua",'rb').read()),
        ("Săn Boss Sát Thủ", io.open(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\ui\uitasklist.ini",'rb').read()),
    ]
    ok = 0
    for u, blob in tests:
        b = tcvn3(u)
        r = "KHOP" if b in blob else "*** KHONG THAY ***"
        if b in blob: ok += 1
        print("%-22s -> %-24r %s" % (u, b, r))
    print("tu kiem: %d/%d" % (ok, len(tests)))
