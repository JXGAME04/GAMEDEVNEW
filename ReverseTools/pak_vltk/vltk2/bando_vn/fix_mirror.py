# -*- coding: utf-8 -*-
"""Guong git clientdata_jx2 phai la BAN SAO DUNG BYTE cua cay song: doi ten 2 tep SPR trong guong
sang dang cp1252 giong tren dia client, them README giai thich, chep them cong cu vao bando_vn."""
import os, sys, shutil, hashlib
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
MIRROR = r"D:\GAMEDEVNEW\clientdata_jx2"
CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
TOOLS = r"D:\GAMEDEVNEW\ReverseTools\pak_vltk\vltk2\bando_vn"
HERE = os.path.dirname(os.path.abspath(__file__))

def gbk2disk(s): return s.encode("gbk").decode("cp1252")
def md5(p): return hashlib.md5(open(p, "rb").read()).hexdigest()

d = os.path.join(MIRROR, "Spr", "Ui3", gbk2disk("小地图"))
for gbkname in ("剑侠大地图2_vn.spr", "打怪向导山洞图_vn.spr"):
    src = os.path.join(d, gbkname)            # ten Unicode that (ghi nham o buoc truoc)
    dst = os.path.join(d, gbk2disk(gbkname))  # ten dang cp1252 nhu tren dia
    if os.path.exists(src):
        os.replace(src, dst)
    live = os.path.join(CLIENT, "Spr", "Ui3", gbk2disk("小地图"), gbk2disk(gbkname))
    print("  %-22s guong=%s  md5 khop cay song=%s" % (gbkname, os.path.basename(dst), md5(dst) == md5(live)))

readme = os.path.join(MIRROR, "Spr", "README.md")
open(readme, "w", encoding="utf-8").write(u"""# clientdata_jx2\\Spr — tài nguyên đặt trên ĐĨA của client (đè lên pak)

Cây này là **bản sao đúng byte** của `E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE\\bin\\client\\Spr\\`.
Client là ứng dụng ANSI (CreateFileA, codepage 1252) mở đường dẫn ghi bằng **byte GBK**
(ví dụ `\\Spr\\Ui3\\小地图\\剑侠大地图2_vn.spr`), nên trên đĩa Windows lưu tên = byte GBK đọc theo cp1252:

| Tên GBK thật | Tên trên đĩa (cp1252) |
|---|---|
| `小地图` | `Ð¡µØÍ¼` |
| `剑侠大地图2_vn.spr` | `½£ÏÀ´óµØÍ¼2_vn.spr` |
| `打怪向导山洞图_vn.spr` | `´ò¹ÖÏòµ¼É½¶´Í¼_vn.spr` |

**Đừng đổi tên cho "đẹp"** — đổi là client không tìm thấy nữa. Chép nguyên thư mục này đè lên
`bin\\client\\Spr\\` là khôi phục được. `KPakFile::Open` chế độ 0 = đọc ĐĨA trước, pak sau
(`Sources\\Engine\\Src\\KPakFile.cpp:241`).

## [BANDO20 06/09] Bản đồ tiếng Việt lấy từ client VLTK 2.0
2 tệp `_vn` trên = ảnh 752×576 rút từ `update.pak` của 2.0 (uid `43841ADC` / `88D08A38`), dùng bởi
`settings\\MapList.ini` khoá `WorldMapImage=` / `CaveMapImage=`. Lùi: đổi 2 khoá về tên không có `_vn`
(bản chữ Hán vẫn nằm trong pak). Công cụ + 2 bản kiểu khác: `ReverseTools\\pak_vltk\\vltk2\\bando_vn\\`.
""")
print("  README:", readme)

for f in ("apply_bando.py", "preview_final.py", "fix_mirror.py", "cp_check.py"):
    shutil.copyfile(os.path.join(HERE, f), os.path.join(TOOLS, f))
print("  cong cu -> bando_vn:", ", ".join(("apply_bando.py", "preview_final.py", "fix_mirror.py", "cp_check.py")))
print("XONG")
