# ĐẶC TẢ VIẾT AUTO DÃ TẨU (tasklink) — cho phiên viết bot sau

Ngày chốt: 17/08/2026 · Hệ đã chạy thật trên server test `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`
Nguồn sự thật: `D:\GAMEDEVNEW` (cây build DUY NHẤT) · Tài liệu nền: `BANGIAO_DATAU_TASKLINK.md` (mục 6b-6e), `DANHSACH_DATAU_PORT.md`, `PHANTICH_DATAU_LINUX_GOC.md`
**Mọi số liệu dưới đây đã kiểm bằng mã/byte trong đợt port — auto viết theo đây, KHÔNG cần đọc lại script.**

---

## 0. TÓM TẮT CHO NGƯỜI VIẾT AUTO

Vòng lặp auto: **gặp NPC → nhận nhiệm vụ → làm theo loại (đọc task 1021) → về trả → chọn thưởng ở cửa sổ 3 rương → lặp**. Trần 40 nhiệm vụ/ngày. Chiến lược tối ưu: **KHÔNG BAO GIỜ hủy nhiệm vụ trong ngày** (đủ 40 + 0 hủy = 100tr exp + 5 Bảo rương thần bí + mốc mỗi-10 = 3 Boss Triệu Hoán Phù ×4 lần). Trạng thái đọc hoàn toàn từ **task value** (server sync về client từng biến qua `s2c_taskvalue` mỗi lần đổi — KPlayerTask.cpp:83-88).

## 1. NPC DÃ TẨU — 10 VỊ TRÍ

Template **108**, NameID **59** (tên "Dã Tẩu"), script `\script\global\seasonnpc.lua`. Minimap có marker sẵn.

| Map | Tên | X | Y |
|---|---|---|---|
| 1 | Phượng Tường | 1620 | 3089 |
| 11 | Thành Đô | 3154 | 5067 |
| 20 | Giang Tân Thôn | 3537 | 6231 |
| 37 | Biện Kinh | 1736 | 3101 |
| 53 | Ba Lăng Huyện | 1626 | 3172 |
| 78 | Tương Dương | 1595 | 3288 |
| 80 | Dương Châu | 1745 | 2967 |
| 121 | Long Môn Trấn | 1960 | 4501 |
| 162 | Đại Lý | 1651 | 3226 |
| 176 | Lâm An | 1562 | 2979 |

(Tọa độ theo đơn vị ô ×32 khi AddNpcNew; auto tìm NPC theo template 108 quanh tọa độ là chắc nhất.)

## 2. MÁY TRẠNG THÁI HỘI THOẠI (điều khiển bằng task **1028** = course)

Gói thoại: chuẩn `UI_SELECTDIALOG` (PLAYER_SCRIPTACTION_SYNC). Client chọn đáp án theo **INDEX** (1-based, thứ tự cố định như liệt kê). Auto nên match index; text chỉ để kiểm tra chéo.

| course | Màn hiện ra | Option auto cần chọn |
|---|---|---|
| **0** (chưa nhập môn) | 3 nút: [1] "Được thôi!..." (nhận) · [2] "...nội dung ra sao" (info) · [3] "Ta bận rồi" (thoát) | **[1]** → `Task_Confirm` → giao nhiệm vụ đầu + mở màn Task_MainDialog |
| **1** (đang làm) | Task_MainDialog 4 nút: [1] chờ · [2] "ta đã hoàn thành..., xin kiểm tra" (TRẢ) · [3] hủy · [4] info | Chưa xong → đóng; xong → **[2]** `Task_Accept` |
| **2** (đã nộp, chưa nhận thưởng) | **Nhảy thẳng** cửa sổ 3 rương (mục 5) — chống mất thưởng khi thoát game | chọn rương |
| **3** (đã lãnh) | 2 nút: [1] "Đương nhiên, cho ta biết nhiệm vụ tiếp" · [2] nghỉ | **[1]** → `Task_TaskProcess` → nhiệm vụ mới, course về 1 |

Chú ý: nếu đã hủy lậu >2 lần đầu chuỗi, NPC giận **10890 tick (~10 phút)** (task 1036==10, mốc thời gian task 1029, `GetGameTime()` = tick 18/giây).

## 3. BẢNG TASK ID — TOÀN BỘ TRẠNG THÁI AUTO CẦN ĐỌC

Server sync từng biến về client ngay khi đổi (auto đọc memory client hoặc bắt gói `s2c_taskvalue`).

| Task | Nghĩa | Decode |
|---|---|---|
| **1028** | course 0/1/2/3 | số nguyên |
| **1021** | LOẠI nhiệm vụ hiện tại 1..6 | số nguyên |
| **1030** | SỐ DÒNG trong bảng loại (tra settings) | 1-based theo bảng dữ liệu |
| **1020** | trạng thái chuỗi, 4 byte | b1=lần trong link (0-19) · b2=link (1-20) · b3=vòng · b4=lượt hủy còn |
| 1035 | số "đơn vị link" đã xong trong link | |
| 1044 | tổng nhiệm vụ (cache) | = (links×20)+lần+vòng×400 |
| **1031** | (loại 4) map id phải đến | |
| **1032** | (loại 4) b1=loại cuộn (1=Địa Đồ 205, 2=Mật Chỉ 212) · b2=số tấm cần | GetByte |
| **1025** | (loại 4) số tấm ĐÃ nhặt | so với b2 của 1032 |
| **1027** | mảnh Sơn Hà Xã Tắc đang có (loại 6 + tiền tệ hủy 100 mảnh) | |
| 1026 | (loại 5) mốc chỉ số lúc nhận (danh vọng/PK/TK) | hiệu = hiện tại − 1026 |
| 1033/2574/1034 | (loại 5-exp) level + exp/1e5 + exp dư lúc nhận | |
| 1038–1043 | 6 ô thoại ngẫu nhiên (chỉ hiển thị) | |
| **2419 / 2420** | ngày (yymmdd) / SỐ NHIỆM VỤ ĐÃ LÀM HÔM NAY (trần 40) | |
| **2797** | số lần hủy hôm nay | mốc-40 đòi ==0 |
| 1036 / 1029 | đếm hủy lậu / mốc tick phạt | 1036==10 → đang bị phạt |
| 1045 / 1046 | cờ tái nhập hủy / bản sao lượt hủy (chống hack) | auto KHÔNG đụng |
| 1037 | seed khóa bộ 3 thưởng | mở lại cửa sổ = vẫn 3 ô cũ |
| 2690 | mốc-10 đã phát (chống trùng) | |

## 4. SÁU LOẠI NHIỆM VỤ — THUẬT TOÁN CHO AUTO

Bảng dữ liệu tra theo `task 1030` (dòng) tại `settings\task\` (test local: đọc thẳng file server; bản phát hành: nhúng bảng vào auto). Trọng số loại theo link: **loại 4 nhiều nhất (~30-35%)**, loại 2 tăng dần theo link, loại 1 chỉ link thấp.

### Loại 1 — MUA VẬT PHẨM (`tasklink_buygoods.txt`, 35 dòng)
- Dòng cho (Genre, Detail, Particular, Level, GoodsFive=hệ) + tên tiệm/item (TaskInfo/TaskInfo1 — chỉ là lời thoại).
- **PHÁT HIỆN QUAN TRỌNG CHO AUTO: các tiệm JX1 dùng bảng bán CHUNG toàn server — KHÔNG cần đến đúng thành trong lời thoại.** Mua tại MỘT thành duy nhất đủ cho mọi dòng:
  - Vũ khí (574 kiếm Kim, 578 đao Thổ, 576/586 côn Thủy/Mộc, 581/582 thương/thích Mộc, 584 thích Hỏa): **tiệm Thợ Rèn** bất kỳ → menu giao dịch vũ khí (Sale 14/17).
  - Yêu đái 575, trúc/hộ uyển 577/579/580/585, giày 587: **tiệm Tạp Hóa** bất kỳ (Sale 11 = dòng 12).
  - Ngựa cấp 3: **Bôn Ngựa** (5 goods mặc định 388/398/408/418/428 theo thành + 572 Bạch-Thủy ở Phượng Tường, 573 Hắc-Mộc ở Đại Lý — 2 con này đã thêm vào dòng 49/50).
- Trả: chọn nút [2] → **GiveItemUI** (hộp give-box): đặt ĐÚNG 1 món vào ô, OK. Server so khớp tuyệt đối 5 trường (genre, detail, particular, hệ, level) — món bị TIÊU HỦY.

### Loại 2 — TÌM VẬT PHẨM (`tasklink_findgoods.txt`, 526 dòng) — nộp mất đồ
- 5 nhóm trang bị: nhẫn 0/3/0, bội 0/4/0-1, liên 0/9/0-1 (mọi level 1-10 đều có trong bảng item JX1) + huyền tinh 6/1/146 (level 1-6).
- Cột MagicEnName: `n` (371 dòng — chỉ cần đúng 5 trường cơ bản; chú ý ô `n` ở GoodsFive/Level nghĩa là bỏ qua tiêu chí đó) hoặc **mã thuộc tính 85-110** (id trùng enum JX1: 85 sinh lực, 89 nội lực, 93 thể lực, 96 hồi thể lực, 97 sức mạnh, 98 thân pháp, 99 ngoại công, 101-105 kháng độc/hỏa/lôi/băng) + khoảng [MinValue, MaxValue] trên `nValue[0]`.
- Auto: giữ **kho đồ nộp** (nhặt đồ xanh trang sức khi farm; đồ trắng mua tiệm cho dòng `n`). Không có đồ → 2 lựa chọn đều tốn kém: hủy (phá mốc-40) hoặc treo. Khuyến nghị auto: ưu tiên tích lũy trang sức xanh mọi hệ trong lúc farm loại 4.

### Loại 3 — KHOE VẬT PHẨM (`tasklink_showgoods.txt`, 45 dòng) — **KHÔNG mất đồ**
- Chỉ magic 85-110 + min-max. Auto giữ 1 bộ "đồ khoe" chuẩn (mỗi thuộc tính 1 món giá trị cao) là trả được mãi mãi. Rẻ nhất trong 6 loại.

### Loại 4 — ĐỊA ĐỒ CHỈ / MẬT CHỈ (`tasklink_findmaps.txt`, 28 dòng) — XƯƠNG SỐNG
- Đọc 1031 (map), 1032.b1 (1=cuộn 205 từ QUÁI THƯỜNG ~0,1-0,7%/con; 2=cuộn 212 CHỈ TỪ BOSS XANH ~0,8-4,7%/boss), 1032.b2 (số cần: 5/8/15 loại 1; 1/2/3 loại 2), 1025 (đã có).
- 14 map: 8 thành/thôn lv10 (1,11,37,53,78,80,162,176) · 21,122 lv40 · 75,225,226,227 lv90.
- **Nhặt là TỰ CỘNG + tự hủy cuộn (không vào túi), có thông báo "được X tấm"** — hook C++ lo hết; auto chỉ cần giết + nhặt (WAuto auto-loot sẵn có dùng được). Cuộn về TÚI chỉ khi nhặt bằng bản DLL cũ — click phải cũng cộng.
- Nhặt trong TỔ ĐỘI: cả đội được cộng mỗi tấm (auto multibox: chỉ cần 1 nhân vật nhặt).
- Cuộn nhặt khi KHÔNG làm nhiệm vụ/sai map → +1 mảnh SHXT (1027) — vẫn có ích (loại 6 + hủy).
- Đủ số → về NPC trả ([2]) — không cần give-box, server tự kiểm 1025.

### Loại 5 — NÂNG CHỈ SỐ (`tasklink_upground.txt`, 45 dòng)
- NumericType từ dòng bảng: 2=exp (hiệu exp từ 1033/2574/1034; 5k→50tr) · 3=danh vọng (hiệu so 1026; 1-100) · 4=phúc duyên (GetFuYuan; 1-100) · 5=PK 1-10 (**cẩn thận: PK JX1 kẹp 0..10 — PK nền cao là bất khả thi, chủ game đã chấp nhận giữ nguyên**) · 6=Tống Kim (task 38, cộng CUỐI trận TK; 100-10000).
- ⚠️ Danh vọng/phúc duyên là TIỀN TỆ shop — auto **cấm tiêu** 2 thứ này khi đang làm loại 5 (hiệu số âm là kẹt).
- Auto đơn giản nhất: type 2 cày quái; type 3/4/5/6 nếu vượt khả năng → dùng lượt hủy TÍCH LŨY TỪ THƯỞNG (SelectAward_Cancel — hủy bằng lượt này không tăng 2797? — CÓ tăng LIMITCancelCount → vẫn phá mốc-40. Trade-off: hoặc treo nhiệm vụ sang hôm sau).

### Loại 6 — SƠN HÀ XÃ TẮC (`tasklink_worldmaps.txt`, 11 dòng)
- Cần Num mảnh (1→5000!) so task 1027, trả là TRỪ đúng Num. Mảnh đến từ cuộn sai-map + item 439 (+100)/2523 (+1000) (script `shanhe-canpian.lua` đã port). Dòng Num lớn trúng phải cân nhắc hủy/treo.

## 5. CỬA SỔ THƯỞNG 3 RƯƠNG (sau mỗi lần trả)

- Server bốc 3 ô KHÁC LOẠI từ {Tiền 20%, Exp 33%, Vật phẩm 34%, Đổi-lần-nữa 8%, Lượt-hủy 5%}, **khóa seed** (đóng mở lại = y nguyên).
- Gói `s2c_openquestfinishdlg` (m_szNotice[64] + nType): **nType ≤ 4 → KUiDaTau** (nút Exp / Money / Random) · **nType > 4 → KUiDaTau1** (nút Point / Lucky / Item).
- Client bấm nút → `GOI_ADD_UI_CMD_SCRIPT` với cmd **3** (funcs `finish_exp`/`finish_money`/`quest_random`) hoặc cmd **4** (`finish_point`/`finish_lucky`/`finish_item`) — auto có thể gọi thẳng OperationRequest này thay vì click pixel.
- Ánh xạ nút→thưởng thật do server giữ (tối đa 1 nút icon xấp xỉ). Chiến lược auto gợi ý: ưu tiên **Item > Exp > Money**; lấy **Lucky (lượt hủy)** khi kho lượt hủy < 2. Sau khi bấm 1 nút, whitelist server tự xóa (không bấm được nút thứ 2).
- **Túi phải ≥ 5 ô trống trước khi trả nhiệm vụ** (mySG từ chối nếu <5; mốc-10 cần ≥3; đủ-40 nên chừa ≥6).

## 6. MỐC THƯỞNG (auto nên tối ưu quanh đây)

| Mốc | Điều kiện | Nhận |
|---|---|---|
| Mỗi 10 nhiệm vụ (tổng tích lũy, task 1044 % 10) | tự động khi trả | **3× Boss Triệu Hoán Phù (6/1/1023)** |
| 30/ngày | nNum−nCancel == 30 | +30tr exp |
| **40/ngày** | đủ 40 VÀ **2797 == 0** (không hủy) | **+100tr exp + 5× Bảo rương thần bí (6/1/2383)** |
| Mỗi nhiệm vụ | — | cống hiến bang +floor(giá trị/8) nếu có bang |
| Bảng vật phẩm ngẫu nhiên | ô "Vật phẩm" | có **Huyền Thiên Chùy 2357** (~5%/lần bốc) |

**Rương 2383**: click phải, cần **6× chìa 2357** + 4 ô trống → 1 món: Lễ bao 30% / Đại Lực 25% / Phi Tốc 25% / Quẻ Huy Hoàng-7-ngày 10% / TTL 5% / Cẩm nang 3% / TTL đặc biệt ×3 2%. Auto: gom chìa, mở khi đủ.

## 7. BỀ MẶT GÓI TIN / API CLIENT (tham chiếu mã)

| Việc | Cơ chế | Nguồn |
|---|---|---|
| Mở thoại NPC | c2s DialogNpc (nNpcId) → server ExecuteScript "main" | KPlayer.cpp `DialogNpc` ~8000 |
| Nhận thoại | s2c PLAYER_SCRIPTACTION_SYNC, UI_SELECTDIALOG, content = "câu\|đáp1\|đáp2..." (client tách '\|') | ScriptFuns LuaSelectUI :555 |
| Chọn đáp án | c2s answer index → server chạy m_szTaskAnswerFun[i] | KPlayer.cpp :6740+ |
| Đọc trạng thái | s2c_taskvalue (mỗi SetTask server-side) — client giữ bản sao task | KPlayerTask.cpp:83-88 |
| Cửa sổ rương | s2c_openquestfinishdlg {szNotice[64], bType} | ScriptFuns :9650 (đã vá strncpy) |
| Bấm rương | c2s PLAYER_UI_CMD_SCRIPT {nType=3/4, szFunc} — whitelist 6 tên | KProtocolProcess.cpp:5797-5807 |
| Give-box (loại 1/2/3) | GiveItemUI → client đặt item pos_affairitem → c2s UI_CMD case 1 | KProtocolProcess UiCommandScript case 1 |
| Nhặt cuộn | c2s pickup thường — server tự xử (hook ServerPickUpItem) | KPlayer.cpp:4767+ |

## 8. CẠM BẪY CHO AUTO (đã đổ máu mới biết — ĐỪNG lặp lại)

1. **Không hủy nhiệm vụ** nếu muốn mốc-40 (2797 phải =0 cả ngày). Hủy lậu >2 lần đầu chuỗi = NPC nghỉ chơi ~10 phút.
2. Túi <5 ô khi trả = server từ chối trao vật phẩm (course vẫn 2 — nói chuyện lại được, không mất thưởng, nhưng auto phải dọn túi).
3. Cửa sổ rương mở lại vẫn 3 ô cũ (seed) — auto crash giữa chừng cứ gặp lại NPC.
4. Cuộn 205 rơi từ quái thường; **212 CHỈ boss xanh** — auto nhiệm vụ Mật Chỉ phải có logic săn boss (map 75 đã cân AutoGoldenNpc=2000 như map khác).
5. JX1 **không có A\* server**, NPC tính là TƯỜNG — di chuyển xa = chuỗi lệnh ngắn (kinh nghiệm WAuto sẵn có).
6. Đổi map làm nhiệm vụ loại 4: **dùng NPC Xa Phu, mục "Đến nơi làm nhiệm vụ dã tẩu" (`godatau`) — ĐÃ nối hệ mới 17/08 (commit `61e834b5`)**: đang có nhiệm vụ loại 4 (task 1021==4) là chở thẳng tới map 1031 đúng tọa độ bảng `TL_MAPTRAPINDEX`, miễn phí, tự bật trạng thái chiến đấu. Auto chỉ cần: gặp Xa Phu → chọn mục đó → tới nơi. (Thổ Địa Phù là phương án phụ.)
7. Mua đồ loại 1: mua ở bất kỳ thành nào (bảng bán chung) — đừng tốn thời gian chạy đúng thành theo lời thoại.
8. Client phải là bản có `settings\buysell.txt` mới (đã đồng bộ 16/08) — client cũ không nhìn thấy hàng nhiệm vụ.
9. Server side đã có log đầy đủ: `[DaTau ruong 2383]` (WriteLog), ScriptError.log — debug auto nhìn đó.
10. Nhặt trong tổ: mỗi cuộn cộng cho CẢ đội — multibox chỉ cần 1 con nhặt (nhưng mỗi con phải TỰ nhận nhiệm vụ ở NPC).

## 9. VIỆC CÒN MỞ PHÍA SERVER (không chặn viết auto)

- Kinh tế chìa ~53% nhu cầu 5 rương/ngày — chủ game có thể tăng dòng chìa (auto không phụ thuộc).
- Loại 5-PK giữ nguyên trần 10 (chủ game chốt) — auto cứ dùng lượt hủy/treo khi kẹt.
- Nguồn đồ-có-magic cho loại 2 hiếm — auto nên nhặt-giữ trang sức xanh từ sớm.
