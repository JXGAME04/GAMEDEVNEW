# clientdata_jx2\Spr — tài nguyên đặt trên ĐĨA của client (đè lên pak)

Cây này là **bản sao đúng byte** của `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Spr\`.
Client là ứng dụng ANSI (CreateFileA, codepage 1252) mở đường dẫn ghi bằng **byte GBK**
(ví dụ `\Spr\Ui3\小地图\剑侠大地图2_vn.spr`), nên trên đĩa Windows lưu tên = byte GBK đọc theo cp1252:

| Tên GBK thật | Tên trên đĩa (cp1252) |
|---|---|
| `小地图` | `Ð¡µØÍ¼` |
| `剑侠大地图2_vn.spr` | `½£ÏÀ´óµØÍ¼2_vn.spr` |
| `打怪向导山洞图_vn.spr` | `´ò¹ÖÏòµ¼É½¶´Í¼_vn.spr` |

**Đừng đổi tên cho "đẹp"** — đổi là client không tìm thấy nữa. Chép nguyên thư mục này đè lên
`bin\client\Spr\` là khôi phục được. `KPakFile::Open` chế độ 0 = đọc ĐĨA trước, pak sau
(`Sources\Engine\Src\KPakFile.cpp:241`).

## [BANDO20 06/09] Bản đồ tiếng Việt lấy từ client VLTK 2.0
2 tệp `_vn` trên = ảnh 752×576 rút từ `update.pak` của 2.0 (uid `43841ADC` / `88D08A38`), dùng bởi
`settings\MapList.ini` khoá `WorldMapImage=` / `CaveMapImage=`. Lùi: đổi 2 khoá về tên không có `_vn`
(bản chữ Hán vẫn nằm trong pak). Công cụ + 2 bản kiểu khác: `ReverseTools\pak_vltk\vltk2\bando_vn\`.
