# -*- coding: utf-8 -*-
"""BAN DONG HANH - G1 buoc 1: rut 5 ban do partner tu pak ra tep roi.

Nguon (do o PHANTICH_BANDONGHANH_2708.md muc 5.3):
    512 云中道 / 513 百花谷 / 514 山贼洞 / 515 紫霞岭  ->  E:\\jx1m_cdn\\data\\image2.pak
    539 长歌门禁地                                   ->  bin\\client\\data\\update03.pak
Dich:
    bin\\client\\maps\\<duong dan nhu MapList>\\v_YYY\\XXX_Region_C.dat  + <ten>.wor + <ten>24.jpg
    bin\\server\\maps\\...                     \\v_YYY\\XXX_Region_S.dat + <ten>.wor + <ten>24.jpg
    (KRegion::LoadObject chi mo _Region_S; doan OBSTACLE hai ban mot dinh dang - da
     chung minh dot Viem De. .fp tu sinh lan chay dau.)

Ten GBK viet duoi dang byte latin-1, khong go tay ky tu Trung.
Chay lai duoc nhieu lan (idempotent).
"""
import os
import struct
import sys
import zlib
import importlib.util

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
import ucl  # noqa: E402

spec = importlib.util.spec_from_file_location(
    "pak_id", r"D:\GAMEDEVNEW\ReverseTools\viemde\pak_id.py")
pak_id = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pak_id)
fid = pak_id.file_name_to_id

SEP = "\\"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# ---- ten GBK (byte latin-1) ----
DACTHU  = "\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8"                                  # 特殊用地
NHIEMVU = "\xc8\xce\xce\xf1\xd3\xc3\xb5\xd8"                                  # 任务用地
GIAODUC = "\xcd\xac\xb0\xe9\xbd\xcc\xd3\xfd\xc8\xce\xce\xf1"                  # 同伴教育任务
KICHTINH= "\xcd\xac\xb0\xe9\xbe\xe7\xc7\xe9\xc8\xce\xce\xf1"                  # 同伴剧情任务
VANTRUNG= "\xd4\xc6\xd6\xd0\xb5\xc0"                                          # 云中道
BACHHOA = "\xb0\xd9\xbb\xa8\xb9\xc8"                                          # 百花谷
SONTAC  = "\xc9\xbd\xd4\xf4\xb6\xb4"                                          # 山贼洞
TUHA    = "\xd7\xcf\xcf\xbc\xc1\xeb"                                          # 紫霞岭
CAMDIA  = "\xb3\xa4\xb8\xe8\xc3\xc5\xbd\xfb\xb5\xd8"                          # 长歌门禁地

IMAGE2   = r"E:\jx1m_cdn\data\image2.pak"
UPDATE03 = CLI + r"\data\update03.pak"

# (map id, thu muc cha tinh tu maps\, ten map, pak nguon)
MAPS = [
    (512, DACTHU + SEP + NHIEMVU + SEP + GIAODUC,  VANTRUNG, IMAGE2),
    (513, DACTHU + SEP + NHIEMVU + SEP + KICHTINH, BACHHOA,  IMAGE2),
    (514, DACTHU + SEP + NHIEMVU + SEP + KICHTINH, SONTAC,   IMAGE2),
    (515, DACTHU + SEP + NHIEMVU + SEP + KICHTINH, TUHA,     IMAGE2),
    (539, DACTHU,                                  CAMDIA,   UPDATE03),
]

OBSTACLE_LEN = 16 * 32 * 4  # 2048
REGION_RANGE = range(60, 171)


def load_pak(path):
    """Index pak DUNG 16 byte/entry (XPackIndexInfo: uId,uOffset,lSize,lCompressSizeFlag).

    BAY 27/08: ban dau doc buoc 12 byte -> chi khop ~1/4 entry (uid chi thang hang
    khi i*12 chia het 16). Moi phep do pak truoc do deu UNDERCOUNT."""
    f = open(path, "rb")
    sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
    assert sig == 0x4B434150, path  # 'PACK'
    f.seek(ioff)
    raw = f.read(count * 16)
    idx = {}
    for i in range(count):
        u, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
        idx[u] = (off, size, cf)
    return f, idx


def read_entry(f, off, size, cf):
    """XPACK_METHOD: byte cao cua cf = kieu nen (0 none, 1 UCL, 2 bzip2);
    3 byte thap = kich thuoc sau nen. lSize = kich thuoc goc."""
    cs = cf & 0xFFFFFF
    fl = cf >> 24                   # 0x01 UCL, 0x02 bzip2, 0x20 UCL 'Pak VNG'
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    blob = f.read(cs if cs else size)
    if fl == 0 or cs in (0, size):
        return blob[:size] if size > 0 else blob
    if fl == 1:                      # TYPE_UCL
        return ucl.nrv2b_decompress_8(blob, size)
    if fl == 2:                      # TYPE_BZIP2
        import bz2
        return bz2.decompress(blob)
    return zlib.decompress(blob)


def check_region(data):
    """tra do dai doan OBSTACLE (index 1) trong tep combin, None neu hong."""
    if len(data) < 12:
        return None
    n = struct.unpack_from("<I", data, 0)[0]
    if n == 0 or n > 64:
        return None
    # KCombinFileSection = {uOffset, uLength}; OBSTACLE = section DAU TIEN
    # (SceneDataDef.h:28 REGION_OBSTACLE_FILE_INDEX = 0) -> nam ngay offset 4
    try:
        off, ln = struct.unpack_from("<II", data, 4)
        return ln
    except Exception:
        return None


def save(dst, data):
    if os.path.isfile(dst) and os.path.getsize(dst) == len(data):
        return 0
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "wb") as f:
        f.write(data)
    return 1


def main():
    paks = {}
    tong_cli = tong_srv = 0
    for mapid, parent, name, pakpath in MAPS:
        if pakpath not in paks:
            paks[pakpath] = load_pak(pakpath)
        f, idx = paks[pakpath]
        base_in_pak = SEP + "maps" + SEP + parent + SEP + name          # \maps\...\<ten>
        print("=== map %d (%s) tu %s ===" % (
            mapid, name.encode("latin-1").decode("gbk"), os.path.basename(pakpath)))

        viet = 0
        n_reg = n_obs = 0
        # 1) region
        for y in REGION_RANGE:
            for x in REGION_RANGE:
                rel = base_in_pak + SEP + ("v_%03d" % y) + SEP + ("%03d_Region_C.dat" % x)
                u = fid(rel)
                if u not in idx:
                    continue
                data = read_entry(f, *idx[u])
                ln = check_region(data)
                n_reg += 1
                if ln == OBSTACLE_LEN:
                    n_obs += 1
                sub = parent + SEP + name + SEP + ("v_%03d" % y)
                viet += save(CLI + SEP + "maps" + SEP + sub + SEP + ("%03d_Region_C.dat" % x), data)
                viet += save(SRV + SEP + "maps" + SEP + sub + SEP + ("%03d_Region_S.dat" % x), data)
        # 2) wor + jpg (nam CANH thu muc map, ten <ten>.wor / <ten>24.jpg)
        for suffix in (".wor", "24.jpg"):
            rel = SEP + "maps" + SEP + parent + SEP + name + suffix
            u = fid(rel)
            if u in idx:
                data = read_entry(f, *idx[u])
                viet += save(CLI + SEP + "maps" + SEP + parent + SEP + name + suffix, data)
                viet += save(SRV + SEP + "maps" + SEP + parent + SEP + name + suffix, data)
                print("   %s: %d byte" % (suffix, len(data)))
            else:
                print("   !!! THIEU %s trong pak" % suffix)
        print("   region: %d (co vat can dung 2048B: %d) | tep ghi moi: %d" % (n_reg, n_obs, viet))
        tong_cli += n_reg
    for f, _ in paks.values():
        f.close()
    print("XONG. Tong region:", tong_cli)


if __name__ == "__main__":
    main()
