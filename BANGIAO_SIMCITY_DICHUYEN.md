# SIMCITY — VÁ "ĐI TUẦN TRA / ĐI THEO LỘ TRÌNH THÌ DÍNH GÓC, LAG"

Commit `bf9f1d72` (nhánh `main`, đã push). Build **Server Release|x64 PASS** (0 lỗi, link xong).
Đã chép sang cây chạy `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\`
(bản cũ lưu ở `CoreServer.dll.bak_20260817_111005`).

---

## 0. TRẢ LỜI CÂU HỎI "BẢN GỐC CÓ HÀM DI CHUYỂN A\* CHO BOT KHÔNG?"

**KHÔNG — và cũng không cần.**

| Nơi tìm | Kết quả | Bằng chứng |
|---|---|---|
| `jx_linux_y` (JX2 gốc, bảng ký hiệu còn nguyên) | Chỉ có `KNpcFindPath` (men tường). **0 ký hiệu** A*/AStar/PathNode/OpenList | quét `.symtab`, 1.661 hàm API |
| `vdk.so` (lớp SimCity native) | `NpcRun` @0x054b0 dịch ngược ra: vài lệnh kiểm rồi `KNpc::RunTo(npc, x<<5, y<<5)`. **Không có A\***. Còn **từ chối** `m_Kind == 1` | capstone x86-32 |
| JX1 `KJXPathFinder.cpp` | **CÓ A\* thật** (fcost/hcost/AddOpenNode, chạy ngược từ đích, giữ cây giữa các lần gọi) — nhưng `KJXPathFinder.h:12` là `#ifndef _SERVER` | đọc trọn 844 dòng |

**Vì sao không nên bật A\* server-side:** một thể hiện = một bản đồ và **một đích tại một thời điểm**;
đổi đích thì `ResetMap()` quét cả `w×h` ô. N bot khác đích trong một khung = N lần quét toàn bảng.
Server có `MAX_SUBWORLD = 1000`. Thêm nữa `CalcFactors()` lúc `Init` là O(w·h·29²) ≈ 1,7×10⁸ với map 20×20 region.
Nguồn dữ liệu vật cản cũng là client-only (`m_cLittleMap.GetBarrierBuf`).

**Quan trọng nhất:** JX2 gốc đi được đúng những chặng thẳng 18–20 ô này bằng **cùng một `GetDir`**.
Nên "thiếu A\*" **không giải thích được** việc JX1 đi không nổi. Gốc rễ nằm chỗ khác — mục 1.

---

## 1. GỐC RỄ THẬT (4 vòng phản biện, đều đọc thẳng mã)

### R1 — Engine chỉ cho **ĐÚNG MỘT** lần né vật cản cho **MỖI lệnh** `SendCommand` ⟵ nguyên nhân chính

`KNpcFindPath.cpp:106-111`
```cpp
m_nFindTimes++;
if (m_nFindTimes > 1) { m_nFindTimes = 0; return 0; }
```
Truy hết các nhánh:
- bước đi được (`:75-80`) chỉ đặt lại `m_nFindState = 0`, **không** động `m_nFindTimes`;
- vào men tường (`:120-121, :137-138, :149-150`) đặt `m_nFindTimer = 0`, **không** động `m_nFindTimes`;
- **mọi** nhánh bỏ cuộc (`:156, :166, :194, :219`) đều có `//m_nFindTimes = 0;` **BỊ COMMENT**;
- **chỉ** nhánh ĐỔI ĐÍCH (`:60-67`) mới xóa sạch.

⇒ Vật cản **thứ hai** của cùng một lệnh → `return 0` → `ServeMove` nhánh `_SERVER`
(`KNpc.cpp:4422-4432`) gọi `DoStand()` → `m_Doing = do_stand` → `OnWalk()` không còn được gọi
→ **bot đứng im vĩnh viễn**.

Đo thật: chặng preset trung vị **19,9 ô (638 MPS)**. Một chặng xuyên phố chắc chắn gặp hơn một vật cản.
Ngân sách men tường cũng bé: `MAX_FIND_TIMER = 30` khung × 5 MPS/khung = **150 MPS = 4,7 ô**.

**→ Bản vá:** không bao giờ phát thẳng node xa nữa. Phát **từng bước 128 MPS (4 ô)**, mỗi bước
qua `TestBarrier` trước khi phát. Mỗi bước = một đích mới = **nạp lại tín dụng né**.
Hằng số `SC_STEP_MPS` trong `KSimCity.cpp` — tăng thì ít gói tin hơn nhưng dễ kẹt hơn.

### R2 — JX1 tính **NPC LÀ TƯỜNG**; JX2 gốc tắt lớp đó ⟵ khuếch đại R1, **CHƯA SỬA**

`KSubWorld.cpp:1471-1476` (nhánh `_SERVER`): bước sang ô mới thì gọi
`GetBarrierMin(..., TRUE)` → `KRegion.cpp:983-987`: `if (bCheckNpc && m_pNpcRef[...] > 0) return Obstacle_JumpFly`.
`KNpcSet.cpp:524`: **mọi** NPC khi sinh ra đều `AddRef(obj_npc)` — kể cả NPC bán hàng đứng yên.

JX2 gốc có công tắc `IsCheckNpcBarrier=0` (`settings/gamesetting.ini:13`).
`grep -rn "IsCheckNpcBarrier" D:\GAMEDEVNEW\Sources\` ⇒ **0 kết quả** — JX1 không có.

⇒ Lộ trình được vẽ trong thế giới NPC-trong-suốt nay chạy giữa rừng tường NPC.

**Chưa vá, chờ ý kiến chủ game.** Cách vá gọn nhất nếu muốn (chỉ ảnh hưởng bot, người chơi thật
không đụng tới): thêm nạp chồng `KSubWorld::TestBarrierMin(..., BOOL bCheckNpc)` rồi để
`KNpcFindPath::CheckBarrier` truyền `!Npc[m_NpcIdx].m_btSimCityBot`.
Bản vá R1 đã trung hòa phần lớn tác hại của R2 (mỗi 4 ô một tín dụng), nên có thể không cần.

---

## 2. CÁC LỖI KHÁC ĐÃ VÁ CÙNG ĐỢT

| Mã | Lỗi | Đo được | Đã vá |
|---|---|---|---|
| **B1** | `nLeg = 0` vô điều kiện ⇒ bắn bot tới node **đầu tiên theo thứ tự tệp** | cách **1009 ô** (`1_phuongtuong`), **1155 ô** (`37_bienkinh`) | bám node **gần nhất**; xa hơn 32 ô thì từ chối và báo GM số ô |
| **B2** | `dx*dx+dy*dy` kiểu `int` **tràn** ⇒ "tới đích" GIẢ ⇒ tiến chặng **mỗi tick** ⇒ 18 gói `s2c_npcwalk`/giây/bot | `183_ngudoc` dist²=**2,642e9**, `154_thuyyen`=**2,019e9** (INT_MAX=2,147e9) | dùng `__int64` |
| **B5** | `SC_PatrolBox` dựng 4 góc `cx±nHalf` **không kiểm** `TestBarrier` | trong thành gần như chắc chắn có góc trong nhà | thử thu nhỏ dần bán kính, đòi ≥2 góc đi được, không đủ thì báo "chỗ này quá chật" |
| **B3** | node trùng/quá gần | **59 chặng** ngắn hơn bán kính tới đích, có node **dài 0** | lọc lúc nạp preset |
| **B4** | `LoadPreset` không khử trùng lặp | 64 lần bấm menu là cạn `SC_MAX_ROUTE` | tra theo (tệp, tên tuyến), trùng thì trả lại rid cũ |

**Cổng chặn mới trong `SC_Breathe`:**
- **region NGUỘI** (`KRegion::IsActive`) — trước đây đọc `do_stand` ở region nguội thành "engine bỏ cuộc",
  bỏ vắng vài phút là bộ đếm chặng chạy vòng quanh lộ trình dù bot chưa hề nhúc nhích.
- **bất động hợp lệ**: `m_StunState` / `m_FreezeState` / `m_FrozenAction` (nuốt im lặng mọi `do_walk`,
  `KNpc.cpp:4647-4661`) / `m_CurrentWalkSpeed<=0` / `m_bExchangeServer`.
- **kẹt kiểu B**: `do_walk` mà đứng yên — `ServeMove` khôi phục vị trí ở region −1 (`KNpc.cpp:4498-4507`)
  và **giữ nguyên** `do_walk`. Bắt bằng bộ đếm `nStuck`.
- **bỏ cuộc thì bám lại node gần nhất**, KHÔNG tiến chặng mù quáng (đo thật: **13.984/13.984** chặng
  preset đều là cạnh đồ thị `_nodes.txt`, nên chặng kế chỉ đi được khi xuất phát từ node k).
  Bỏ cuộc liên tiếp 3 lần thì **dừng hẳn bot**.

---

## 3. GĐ3 — BOT NÓI CHUYỆN (đã đăng ký `SC_LoadChat` / `SC_ChatChance` / `SC_ClearChat`)

🔴 **BOT PHẢI CÓ TÊN.** Nhánh sentinel của `KNpc::Load` (`KNpc.cpp:5046-5072`) **không ghi** `Name`,
`Init()` **không xóa** `Name`, `FindFree()` trả ô **tái sử dụng** ⇒ bot mang tên chủ cũ của ô.
Client tìm người nói bóng thoại **THEO TÊN** và lấy match **ĐẦU TIÊN** (`CoreShell.cpp:8458-8473`)
⇒ tên rỗng = không vẽ bóng; tên trùng = bóng thoại **nhảy sang quái hoặc người chơi thật**.
→ C luôn đặt tên (`SC%d` nếu Lua không truyền), Lua cấp tên ngẫu nhiên từ bảng `SC_HO`/`SC_TEN`.

Các vá khác: không nói khi đã chết; **lọc câu > 64 byte ngay lúc NẠP** (đệm 68 byte cho câu 65..67
lọt vào kho rồi bị chặn lúc nói = bot câm không rõ lý do); nhịp **rải đều** theo chỉ số NPC thay vì
một nhịp chung (chống dồn cục sau lag spike vì bộ đếm nhịp server là "cộng bù"); trộn chỉ số NPC vào
`g_Random` vì LCG rác ở bit thấp (chu kỳ đúng 512 = phát lại danh sách).

---

## 4. TEST

Vào game bằng tài khoản GM, dùng **Lệnh Bài Admin** → **"SimCity - bot gia lap"**.

1. **Đứng giữa đường trong thành** (Phượng Tường / Tương Dương / Biện Kinh / Lâm An).
2. `Sinh 5 bot tai cho toi dung` → phải thấy 5 nhân vật mang tên kiểu `TranPhong1`.
3. `Di theo lo trinh ban do nay` → chọn tuyến.
   - Nếu báo *"Bot cách tuyến … tới N ô (tối đa 32)"* → **đó là B1 đang chặn đúng**: đứng gần tuyến hơn rồi sinh lại.
4. `Bat driver di chuyen` → bot phải đi liên tục, **không đứng chết ở góc nhà**.
5. `Cho bot vua sinh di tuan tra (vuong)` — nếu báo *"chỗ này quá chật"* là B5 đang chặn đúng, ra chỗ rộng hơn.
6. `Bat bot noi chuyen` → sau ~25 giây mỗi bot bắt đầu có bóng thoại; kiểm tên trên bóng thoại
   **đúng là tên bot**, không nhảy sang NPC/người chơi khác.
7. Đứng **xa** bot vài màn hình rồi quay lại: bot phải ở gần chỗ cũ (region nguội thì không đi) —
   trước đây bộ đếm chặng chạy vòng quanh lộ trình dù bot đứng im.

**Cần restart GameServer** để nạp `CoreServer.dll` mới.

---

## 5. CÒN LẠI

- **R2** (NPC là tường) — chờ quyết định.
- Giai đoạn chiến đấu (bot đánh nhau), Tống Kim, thú cưng/kéo xe.
- HỞ #3: hút máu/hút mana từ bot (`KNpc.cpp:3704-3730`) — phải bịt **trước** giai đoạn chiến đấu.
- `SC_MAX_CHAT = 512` nên chỉ 512/1690 câu `general` được nạp (lấy 512 dòng đầu theo thứ tự tệp).
- Client chỉ giữ `MAX_NPC = 256` NPC — bot ngoài 256 sẽ không hiện bóng thoại (vẫn tốn băng thông server).
