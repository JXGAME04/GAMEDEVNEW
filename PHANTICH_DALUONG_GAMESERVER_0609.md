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

## 7. PHÂN TÍCH SÂU TỪNG ĐỀ NGHỊ (06/09 23:00) — có đính chính thứ tự ưu tiên

Chủ yêu cầu đào sâu mục 6. Sau khi đo thêm, **thứ tự ưu tiên tôi đưa ở mục 6 phải sửa**: việc số 1 đã xong rồi, việc số 2 gần như vô ích, còn việc số 3 mới là thứ duy nhất đáng làm cho mục tiêu đông người.

### 7.1 Đề nghị 1 "sửa đỉnh 351 ms" — ĐÃ XONG, không cần làm nữa

Truy bằng `tools\sapxep\truy_dinh.py` trên 3.731 kỳ perf có hơn 500 người:

| Giai đoạn | TICK trung bình | TICK đỉnh (trung vị) | TICK đỉnh (lớn nhất) | RunTime đỉnh (trung vị) | Tick trễ |
|---|---|---|---|---|---|
| Toàn bộ lịch sử | 7,98 ms | 53,5 ms | 1.997 ms | 50,2 ms | 47.846 (1,19 %) |
| 60 kỳ gần nhất | 6,99 ms | 24,2 ms | 357 ms | 14,4 ms | 26 |
| 20 kỳ gần nhất | 7,09 ms | 22,9 ms | 224,7 ms | 13,6 ms | **1** |

Thủ phạm lịch sử: **87,3 %** số kỳ có đỉnh trên 50 ms là do `SCRIPT_TIME`, tức lời gọi `RunTime` của `timerserver.lua` chạy mỗi phút. Ngày trước nó tốn trung vị 50 ms, p99 216 ms, tệ nhất 881 ms. Log `logs\hethong.log` bây giờ:

```
[PROF] RunTime 13 ms: hd3=7 naplai=4 tongkim=1
[PROF] RunTime 15 ms: hd3=7 naplai=6 cl=1
```

Còn 12–15 ms, tức đã giảm khoảng bốn lần nhờ tối ưu `dofile` theo nội dung hôm 05/09 và các sửa tối nay. Trong 20 phút gần nhất chỉ có **1 tick trễ**.

Các đỉnh 200–357 ms còn sót đều rơi đúng vào 21:23, 21:38, 22:00–22:16, là lúc tôi chạy build MSBuild, robocopy 3.053 tệp và thao tác git trên **cùng một máy**. Đó là nhiễu do máy tôi, không phải máy chủ. Trên máy chủ thật không có việc này.

**Kết luận:** bỏ đề nghị 1. Việc cần làm chỉ là theo dõi `jx_lua_prof.log` khi có người thật.

### 7.2 Đề nghị 2 "cache G_CFG/SKD_CFG" — lợi quá nhỏ, hạ xuống cuối

Số đo: Lua chiếm 0,20–0,41 ms trên tick 6,7–7,1 ms, tức **3–6 % của tick**, mà tick lại chỉ dùng 12 % ngân sách. Vậy Lua tiêu tốn khoảng 0,6 % năng lực một nhân.

Profiler cho thấy `ch_lib` 12,7 % + `lib_sukien` 10,9 % + `droprate_normal` 17,0 % = khoảng 40 % thời gian Lua. Cache tra cấu hình bỏ được nhiều nhất 80 % phần đó:

```
0,3 ms/tick × 40 % × 80 % = 0,10 ms/tick  =  1,4 % của tick  =  0,17 % của một nhân
```

Quy ra sức chứa: trần một nhân nhích từ khoảng 7.700 lên khoảng 7.900 người. **Không đáng một buổi công.** Chỉ nên làm nếu tiện tay khi sửa các tệp đó vì lý do khác, hoặc khi profiler chỉ ra `DropRate` thành điểm nóng thật với người chơi thật (người thật giết quái nhiều hơn bot).

Tôi đính chính: ở mục 6 tôi xếp việc này thứ hai là sai, vì lúc đó tôi chưa quy nó ra phần trăm của một nhân.

### 7.3 Đề nghị 3 "phát tán" — ĐÂY MỚI LÀ TRẦN THẬT, xếp số 1

Đo thật từ `jx_auto_server.log`, bộ đếm `[BC-DEM]` mỗi 10 giây, với 1.001 bot phân tán và 1 người thật:

| Chỉ số | Mỗi 10 giây | Quy ra mỗi giây |
|---|---|---|
| Lượt phát (gọi `KRegion::BroadCast`) | 2.404.405 | **240.000** |
| Node người chơi đã quét | 2.589.006 | **259.000** |
| Gói thật gửi tới client | 832 | 83 |
| Bị cắt vì hết hạn mức 100 | 0 | 0 |

Đọc số này ra ý nghĩa: mỗi sự kiện gọi phát tới 9 vùng (vùng hiện tại cộng 8 vùng kề), nên 240.000 lượt tương ứng khoảng 27.000 sự kiện mỗi giây. Tỉ lệ node quét trên lượt phát hiện là **1,08**, nghĩa là mỗi vùng trung bình chỉ có một người. Bot rải đều nên quét gần như miễn phí, và đó là lý do tick chỉ 6,7 ms.

Khi người chơi tụ một chỗ thì tỉ lệ này là thứ nổ tung. Vòng quét trong `KRegion::BroadCast` duyệt danh sách người của vùng cho tới khi gửi đủ 100 người thật, mỗi node phải đọc `Npc[nNpcIndex].m_MapX/m_MapY`, là truy cập ngẫu nhiên vào mảng lớn nên hay trượt cache:

| Người tụ trong một vùng | Node quét mỗi lượt phát | So với hiện nay |
|---|---|---|
| 1 (hiện nay, bot rải đều) | 1,08 | 1× |
| 100 | ~100 | 90× |
| 300 | ~100 (dừng khi đủ 100 gửi) | 90× |
| 500 | ~100 | 90× |

Điểm quan trọng: trần 100 (`KRegion.h:16 MAX_BROADCAST_COUNT`) **giới hạn chi phí quét lẫn chi phí gửi**, nên máy chủ không chết vì phát tán. Cái chết là **người chơi không thấy nhau**: khi 500 người ở cùng chỗ, mỗi sự kiện chỉ tới được 100 người, con trỏ xoay `m_nBroadCastCursor` chia lượt nên ai cũng thấy chập chờn. Đây đúng là lớp lỗi "không thấy địch, không thấy chiêu" hôm 03–04/09.

Van thứ hai là hạn mức `BroadCastGoiToiDa` mặc định **1.500 gói vị trí mỗi giây cho mỗi client** trong `config.ini`. Nó bảo vệ client 32 bit khỏi bị ngập, vì đo hôm 03/09 cho thấy 59.000 gói mỗi giây làm client sập.

Ba hướng sửa, xếp theo lợi trên công:

1. **Gộp gói vị trí.** Thay vì mỗi NPC di chuyển gửi một gói riêng, gom các thay đổi trong một tick của cùng một vùng thành một gói nhiều mục. Giảm số gói khoảng 5 tới 10 lần, cho phép nâng trần 100 lên 300–500 mà client vẫn sống. Cần đổi giao thức nên phải sửa cả client, và đây là thay đổi có rủi ro theo Cổng 2 của quy trình.
2. **Nâng trần theo loại gói.** Gói chiến đấu như ra chiêu, sát thương, chết quan trọng hơn gói vị trí. Cho gói chiến đấu trần cao hơn hẳn, gói vị trí giữ trần thấp. Không đổi giao thức, chỉ đổi tham số theo `nBCLoaiGoi` vốn đã có sẵn trong mã. Rẻ và an toàn nhất.
3. **Lọc theo tầm nhìn thật.** Hiện lọc theo ô vuông 32 ô. Có thể thu hẹp động khi mật độ cao: đông thì chỉ gửi cho người trong 16 ô. Người chơi mất tầm nhìn xa nhưng thấy rõ người quanh mình.

### 7.4 Đề nghị 4 và 5 "đo trên máy đích và với người thật" — vẫn giữ, là điều kiện tiên quyết

Hiện tại chưa có người chơi thật nào đáng kể: `[BC-DEM]` cho thấy chỉ 83 tới 124 gói gửi thật mỗi giây, tức khoảng 1 người. Tiến trình `Rainbow` thậm chí chưa chạy. Nghĩa là **toàn bộ chi phí mạng của người chơi thật chưa hề nằm trong bất kỳ số đo nào ở trên**.

Người chơi thật thêm ba khoản mà bot không có: giải mã gói lệnh gửi lên, mã hoá và đóng gói gửi xuống trong `PackDataToClient`, và tiến trình `Rainbow` chuyển tiếp. Hai khoản đầu nằm trên luồng game.

Vì vậy con số "một nhân gánh 3.000 người trên máy chậm 60 %" ở mục 3 là **cận trên lạc quan**. Muốn biết thật thì phải đo với người thật, không có đường tắt.

### 7.5 Đề nghị 6 "chia nhân" — phân tích sâu hướng A

Nếu vẫn muốn chuẩn bị cho đa luồng, hướng A là hướng duy nhất đáng cân nhắc bây giờ, và nên chọn đúng phần việc:

| Việc | Có tách được không | Lý do |
|---|---|---|
| Mã hoá và nén gói phát tán | **Được** | `PackDataToClient` chỉ ghi vào đệm của từng kết nối, không đụng `Npc[]`. Đây cũng là phần sẽ phình khi đông người thật. |
| A* tìm đường | **Được** | Lưới đường đi chỉ đọc; khung luồng đã có sẵn nhưng bị chú thích ở `KSubWorld.cpp:2236`. Hàng đợi yêu cầu, trả kết quả ở tick sau. |
| Ghi log | Được | Đã có `PB_LogNgoai` ghi thẳng; chuyển sang hàng đợi một luồng. |
| Quét vùng, AI, script | **Không** | Đụng `Npc[]`, `Player[]` toàn cục không khoá. |

Lợi dự kiến của hướng A ngay bây giờ gần bằng không, vì cả ba việc trên cộng lại chưa tới 1 ms mỗi tick khi chưa có người thật. Lợi thật chỉ xuất hiện khi có vài trăm người thật, lúc đó phần mã hoá gói mới nặng. **Nên hoãn tới khi đo được.**

### 7.6 Thứ tự đề nghị SAU KHI đào sâu

1. **Nâng trần phát tán theo loại gói** (mục 7.3 cách 2). Rẻ, không đổi giao thức, sửa đúng thứ đang chặn đông người. Nửa ngày.
2. **Mở máy chủ cho người thật vào, dù chỉ 50 tới 100 người**, rồi đọc `[BC-DEM]`, `jx_perf_server.log` và `jx_lua_prof.log`. Đây là dữ liệu còn thiếu duy nhất và không thể thay thế bằng suy luận.
3. **Chạy `do_mot_nhan.py` trên máy chủ đích** để có hệ số quy đổi thật.
4. Gộp gói vị trí (mục 7.3 cách 1) nếu bước 2 cho thấy client vẫn ngập.
5. Cache `G_CFG` khi tiện tay.
6. Hướng A chia luồng, chỉ khi bước 2 cho thấy mã hoá gói đã thành điểm nóng.
