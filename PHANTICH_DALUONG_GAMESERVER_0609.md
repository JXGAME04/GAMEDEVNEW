# PHÂN TÍCH: MỘT GAMESERVER CÓ CHẠY ĐƯỢC NHIỀU NHÂN KHÔNG? (06/09/2026 22:30)

Chủ hỏi: *"Có cách nào cho gameserver chạy được nhiều nhân không? Mục tiêu người chơi đông... nhưng không chạy nhiều gameserver, chỉ chạy một gameserver"* và *"máy tôi cấu hình cao, đưa lên máy chủ cấu hình không bằng sẽ chiếm % một nhân cao hơn, phân tích sâu hơn"*.

Công cụ: `tools\sapxep\phan_tich_tai.py` (hồi quy 3.830 kỳ perf), `tools\sapxep\do_mot_nhan.py` (đo sức một nhân để quy đổi máy). Bản sao ở `ReverseTools\lua54\danhgia_0609\`.

## 1. Số đo nền (06/09 22:05, 1.001 trực tuyến đều là bot)

| Hạng mục | Giá trị |
|---|---|
| Máy đo | i7-13700K, 16 nhân vật lý / 24 luồng, 32 GB, điểm một nhân **22,24** |
| GameServer.exe | 60 luồng, **1 luồng làm gần hết** (9 phút 20 s CPU; luồng kế 2 s), RAM 2,5 GB |
| CPU GameServer | **11,5–15,5 % của MỘT nhân** = 0,5 % toàn máy |
| Tick | tb 6,7 ms / ngân sách 55,6 ms (18 tick/giây) = **12 %** |
| Trong tick | SW_ACTIVATE 4,15 ms (quét vùng + NPC), còn lại ~2,5 ms (bot AI, timer, mission, message loop) |
| Đỉnh | TICK max **357 ms**, LUA_CALL max **351 ms** (một lời gọi Lua treo 0,35 giây) |

Các tiến trình khác (đo cùng lúc, đều là tiến trình RIÊNG nên đã dùng nhân khác): Goddess 4,9 %, Bishop 1,1 %, S3Relay 0,5 %, mysqld ~0 %. Rainbow chưa chạy vì bot không cần.

## 2. Hồi quy: chi phí theo đầu người và theo NPC (3.830 kỳ, online 0 → 1.003)

```
TICK(ms) = 0,58 + 0,0057 × số người         (r² = 0,25)   -> +0,57 ms mỗi 100 người
TICK(ms) = 1,50 + 0,0020 × số NPC           (r² = 0,50)   -> +0,20 ms mỗi 100 NPC
SW_ACTIVATE(ms) = 0,45 + 0,0036 × số người                -> +0,36 ms mỗi 100 người
```

Công thức theo **số NPC** đáng tin hơn (r² gấp đôi) vì trong engine này người chơi cũng là một NPC: tổng NPC = người chơi + quái + NPC chức năng. Kiểm chứng: 2.600 NPC → 1,50 + 5,2 = 6,7 ms, đúng bằng số đo.

## 3. Quy đổi sang máy chủ yếu hơn

Trần một nhân = khi tick chạm 55,6 ms. Nên giữ dưới 80 % ngân sách (44,5 ms) cho an toàn.

| Máy chủ so với máy này | 3.000 người | 5.000 người | 8.000 người | Trần 80 % ngân sách |
|---|---|---|---|---|
| Bằng (điểm 22) | 10,7 ms (19 %) | 15,5 ms (28 %) | 25,5 ms (46 %) | ~7.700 người |
| Chậm 25 % (điểm ~16,5) | 14,3 ms (26 %) | 20,7 ms (37 %) | 34,0 ms (61 %) | ~5.700 người |
| Chậm 45 % (điểm ~12) | 19,5 ms (35 %) | 28,2 ms (51 %) | 46,4 ms (83 %) | ~4.200 người |
| Chậm 60 % (điểm ~9) | 26,8 ms (48 %) | 38,8 ms (70 %) | 63,8 ms (**vỡ**) | ~3.000 người |

Cách lấy số thật cho máy chủ đích: chép `do_mot_nhan.py` sang máy đó, chạy `python do_mot_nhan.py`, lấy `điểm máy chủ / 22,24` làm hệ số. Máy chủ thuê phổ thông (Xeon đời cũ, xung 2,2–2,6 GHz) thường rơi vào nhóm chậm 45–60 %.

**Kết luận mục này:** ngay cả trên máy chậm hơn 60 %, một nhân vẫn gánh được khoảng 3.000 người trước khi tick chạm ngưỡng an toàn. Đây là con số suy từ bot phân tán đều; xem mục 4 để biết vì sao thực tế gãy sớm hơn ở chỗ khác.

## 4. Trần THẬT không nằm ở số nhân

Ba trần khác đến trước trần CPU của vòng game:

1. **Trần phát tán 100 người/lượt** (`KRegion.h:16 MAX_BROADCAST_COUNT = 100`). Mỗi sự kiện (di chuyển, ra chiêu) chỉ gửi tới tối đa 100 người quanh đó, có con trỏ xoay chia lượt. Khi 500 người tụ một chỗ (Tống Kim, công thành), người chơi **đã** không thấy đủ nhau — đây chính là lớp lỗi "không thấy địch/chiêu" của 03–04/09. Thêm nhân không sửa được; phải sửa cách chia lượt hoặc gộp gói.
2. **Trần phía client.** Đo 03/09: một client trong đám 258 người nhận ~59.000 gói/giây rồi sập. Client là ứng dụng 32 bit, một luồng vẽ. Đông người là client chết trước máy chủ.
3. **Đỉnh treo 351 ms trong Lua.** Một lời gọi làm cả thế giới khựng 0,35 giây, người chơi thấy giật. Chia nhân không cứu vì công việc đó vẫn nằm trên luồng game.

## 5. Nếu vẫn muốn nhiều nhân trong MỘT tiến trình

Hệ đã dùng nhiều nhân sẵn ở mức tiến trình: GameServer, Goddess, Bishop, Rainbow, S3Relay chạy song song. Chỉ **vòng game** là một luồng. Ba hướng để chia nhân bên trong nó:

| Hướng | Nội dung | Công | Rủi ro | Lợi thật |
|---|---|---|---|---|
| **A. Đẩy việc phụ ra luồng** | A* tìm đường (khung luồng có sẵn nhưng bị chú thích ở `KSubWorld.cpp:2236`), mã hoá + nén gói phát tán, ghi log, autosave (MySQL đã bất đồng bộ) | 3–5 ngày | Thấp | Bỏ đỉnh do việc phụ; hiện chưa phải nút cổ chai nên lợi ít |
| **B. Chia theo bản đồ** | Mỗi map một luồng; vùng/NPC của map độc lập | 2–4 tuần | **Cao** | Nhân đôi/ba trần CPU |
| **C. Nhiều tiến trình chia map** | Kiến trúc gốc của dòng game này | 1–2 tuần | Trung bình | Trần gần như tuyến tính theo nhân — nhưng chủ đã loại |

Vì sao B rủi ro cao: `Npc[]`, `Player[]`, `SubWorld[]` là mảng toàn cục **không có khoá**. Trong chính mã đã có dấu vết một lần thử chia luồng bị bỏ dở, kèm ghi chú tại `KSubWorldSet::MainLoop` rằng không được chạy song song vì lý do đó. Muốn làm phải thêm hàng đợi thông điệp giữa map cho mọi thứ cắt ngang map: chuyển map, chat, tổ đội, bang hội, giao dịch, script gọi hàm C++ đụng người chơi map khác. Lỗi tranh chấp dữ liệu gây sập ngẫu nhiên, khó tái hiện, khó tìm.

## 6. Đề nghị (theo lợi trên công)

1. **Sửa đỉnh 351 ms** — cái người chơi thấy lag ngay bây giờ. Profiler đã bật, chờ bắt được thủ phạm.
2. **Cache tra cấu hình ở đường giết quái** (`G_CFG` / `SKD_CFG` / `DropRate` chiếm ~40 % thời gian Lua). Giảm khoảng 15 % tải Lua, rẻ và an toàn.
3. **Nâng trần phát tán + gộp gói** thay vì chia nhân. Đây mới là thứ chặn "đông người" thật sự, và còn giúp client sống.
4. **Đo trên máy chủ đích**: chạy `do_mot_nhan.py`, lấy hệ số, tra bảng mục 3 để biết trần thật.
5. **Đo với người chơi thật**: bot không có gói mạng vào/ra nên rẻ hơn người thật ở khâu giải mã gói. Khi có 200–500 người thật, xem lại `TICK` và CPU rồi mới quyết chia nhân.
6. Chỉ khi các bước trên xong mà tick vẫn chạm 60–70 % ngân sách thì mới bàn hướng A, rồi B.
