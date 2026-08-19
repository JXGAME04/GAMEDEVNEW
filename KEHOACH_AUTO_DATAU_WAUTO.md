# KẾ HOẠCH: AUTO DÃ TẨU TRONG WAUTO (auto ngoại)

Ngày: 18/08/2026 · Người viết: phiên Claude (khảo sát 11 mũi đọc mã, ~1,6tr token)
Nguồn sự thật: `D:\GAMEDEVNEW` (client + server script) · UI: `E:\Src_Auto_Ngoai\WAuto`
Tài liệu nền: `AUTO_DATAU_SPEC.md` (đặc tả hệ nhiệm vụ) — **có 1 điểm SAI đã phát hiện, xem §1.**

---

## 1. PHÁT HIỆN QUYẾT ĐỊNH THIẾT KẾ (đã kiểm bằng mắt từng dòng mã)

1. **Client KHÔNG đọc được task value ≥256.** `TASK_VALUE_SYNC.nTaskId` là `BYTE`
   (KProtocol.h:1865-1870; KPlayerTask.cpp:85 gán `nTaskId = nNo` cắt mod 256).
   Mọi id Dã Tẩu (1020-1046, 2419/2420/2797…) không tới client đúng.
   → Spec mục 3 ("client giữ bản sao task") **SAI với id ≥256**.
   → **Không sửa protocol** (server ở máy khác, client cũ của người chơi sẽ desync).
   → Thiết kế: **máy trạng thái điều khiển bằng NỘI DUNG HỘI THOẠI** — client bắt các gói
   sẵn có: `s2c_scriptaction` (câu hỏi+đáp án, KPlayer.cpp:7513 / 7634), `s2c_openquestfinishdlg`
   (KProtocolProcess.cpp:3896), `s2c_openaffairbox` (:4040), `s2c_extendchat` (:4115).
   Text template của server đã trích đủ 100% từng byte TCVN3 (6 loại nhiệm vụ — xem phụ lục
   trong scratchpad khảo sát; nhúng vào code C++ dạng octal escape).

2. **Engine auto ngoại là ExtAuto trong CoreShell.cpp** (không phải KPlayerAuto.cpp —
   fkauto đã bị tắt tick tại KPlayer.cpp:432). Chuỗi: WAuto.exe →(shared mem, `autoData`
   pack(1), 54ms/lần)→ `KMyApp::ExtAutoLoop` S3Client.cpp:789 →
   `OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_*, …)` → case trong CoreShell.cpp:3669+,
   trạng thái ở `KPlayer::m_sExtAuto` (KPlayer.h:112-221, bị memset khi ATYPE_CLEAR).

3. **Rương thao tác ĐƯỢC TỪ XA** — server chỉ kiểm `m_CUnlocked`:
   - chuyển đồ rương↔túi: `c2sdnmbr_exchangeitem` (KProtocolProcess.cpp:4967-5089,
     rương→túi cần !FightMode :5073) hoặc move-item cổ điển (KPlayer.cpp:4914 chỉ chặn khóa);
   - rút tiền: `c2s_storemoney` (KItemList.cpp:1998 chỉ chặn khóa) — **mọi nơi, mọi map**;
   - client LUÔN biết nội dung rương+tiền rương (sync từ login: KPlayer.cpp:8954, s2c_syncitem).
   → "kiểm tra rương 1 2 3 + hành trang" = quét `m_ItemList` local, không cần đi tới rương.
   Mở khóa: `SendClientCPUnlockCmd(atoi(szBoxPass))` (đã có sẵn — ô mật khẩu tab Hậu cần).

4. **Mua đồ nhiệm vụ loại 1 phải qua shop thật** — `c2s_openshop`/AutoBuyItem tuy mua từ xa
   được nhưng ép Particular=0/Series=0 (KBuySell.cpp:494) → không khớp 5 trường.
   Server so khớp 5 trường TUYỆT ĐỐI khi trả (tasklink_head.lua:285-292), mua thành nào
   cũng được (không kiểm nguồn gốc).

5. **Xa Phu godatau = option 5** (1-based; gửi index 4) trong menu 9 mục (xaphu.lua:19),
   miễn phí, server tự chở đúng map khi task 1021==4 (station.lua:337-393, commit 61e834b5).

6. **Tìm NPC Dã Tẩu bằng template**: client có `Npc[].m_NpcSettingIdx == 108` +
   `m_Kind == kind_dialoger(3)` (KNpc.h:449, KProtocolProcess.cpp:1817). Tên hiển thị
   TCVN3 `44 B7 20 54 C8 75` ("Dã Tẩu"). Tọa độ NPC 10 thành đã có (spec §1, đơn vị ô ×32).

7. **Thông điệp tiến độ loại 4** đến qua `Msg2Player` → `s2c_extendchat`/`chat_channelchat`,
   sender "Hệ Thống" (`48 D6 20 54 68 E8 6E 67`), channelid -1; số đếm X hiện tại parse được
   giữa literal `tæng céng` và ` tÊm.` (bắt tại KProtocolProcess.cpp:4128-4134).

8. **Cửa sổ 3 rương**: nType≤4 = KUiDaTau [Exp/Money/Random], nType>4 = KUiDaTau1
   [Point/Lucky/Item]; bấm bằng `SendUiCmdScript(3|4, "finish_*")` (KProtocol.cpp:695),
   server whitelist KProtocolProcess.cpp:5818-5833. **SelectAward_Cancel = nút Lucky**
   (seasonnpc.lua:1243-1244) → muốn tích lượt hủy thì chọn Lucky.

9. **Nộp đồ give-box**: bắt `S2C_GIVE_BOX.Value2` (tên hàm callback) khi mở; đặt đồ =
   2 gói `c2s_playermoveitem` (nhặt nguồn / thả `pos_affairitem`), xong
   `SendUiCmdScript(1, <Value2>)`. Trả loại 4/5/6 không cần give-box (server tự kiểm).

10. **Item Phúc Duyên Lộ** 6/1/121·122·123 (+10/+20/+50) có trong bảng item nhưng
    script `fuyuanlu_*.lua` **thiếu ở cây D** (có thể có trên server sống) → chiến thuật
    "đứng tại NPC: dùng 1 item → thử trả → chưa được thì dùng tiếp" (không cần baseline,
    tự phát hiện item chết → chuyển xử lý kẹt). Danh vọng: KHÔNG có item (đã grep toàn cây).
    Item "hoàn thành nhanh nhiệm vụ" **KHÔNG TỒN TẠI** (chứng minh bằng vắng mặt) →
    tùy chọn của người chơi chỉ còn: treo / hủy (3 kiểu hủy).

11. **Hủy nhiệm vụ** (Task_CancelConfirm sea:357-383): còn lượt hủy → hộp 2 option
    (câu chứa "bạn còn…cơ hội") chọn [1]=hủy bằng lượt (GIỮ chuỗi); hết lượt → hộp 3 option:
    [1]=hủy thường (RESET chuỗi, +1 đếm 1036, >2 lần đầu chuỗi = phạt 10 phút),
    [2]=dùng 100 mảnh SHXT, [3]=thôi. **Mọi kiểu hủy đều +1 đếm 2797 → mất mốc-40.**

12. **Đường về từ map nhiệm vụ 21/122/75/225/226/227** (không có Xa Phu ở đó):
    dùng Thổ Địa Phù (item_townportal, `AutoUseItem`) — kiểm chi tiết khi code;
    dự phòng `c2s_aibacktotown` mode A (KPlayer.cpp:6561, cần m_sPortalPos còn hạn).
    8 map thành thị thì NPC Dã Tẩu ở ngay đó, không phải đi.
    **Trả nhiệm vụ ở BẤT KỲ NPC Dã Tẩu nào** (trạng thái nằm trên nhân vật).

## 2. THAY ĐỔI DỮ LIỆU — `autoData` (ipc_shared.h, THÊM CUỐI struct, cả 2 bản)

```c
// == Da Tau (them 18/08/2026, phai o CUOI struct, truoc constructor) ==
int  bDaTau;        // bat/tat tong
int  bDTType[6];    // bat/tat tung loai 1..6
int  nDTSkipMode;   // loai tat/ket: 0=treo (bo qua, ngung DT) 1=huy nhiem vu
int  nDTCancelMode; // cach huy: 0=chi luot huy thuong(giu chuoi), 1=cho phep huy thuong(reset chuoi), 2=uu tien 100 manh SHXT
int  nDTReward1;    // cua so Exp/Money/Random: 0/1/2
int  nDTReward2;    // cua so Point/Lucky/Item: 0/1/2 (Lucky = tich luot huy)
int  bDTUseBox;     // cho phep lay do/tien tu ruong (dung szBoxPass tab Hau can)
int  bDTTrainAfter; // du 40 /treo -> tha may cho auto thuong (ve map luyen cong theo tab Di chuyen/Hau can)
int  nDTWDMoney;    // (van luong) rut tu ruong khi thieu tien mua do
```
= 14 int = 56 byte. `.dat` cũ vẫn nạp được (LoadRoleData đọc thiếu → phần đuôi = 0 = tắt).
Khởi tạo 0 trong constructor. **Rebuild cả 3: WAuto.exe + Game.exe + CoreClient.dll.**

## 3. MÁY TRẠNG THÁI PHÍA GAME (CoreShell.cpp, case `ATYPE_DATAU` mới)

- Enum `ATYPE_DATAU` thêm CUỐI `AUTOOPERATION_INDEX` (CoreShell.h:58).
- Gọi từ `ExtAutoLoop` TRƯỚC khối `bReturn` (S3Client.cpp:1053); khi máy DT đang
  "cầm lái" (cờ `nDTEngaged`) thì ExtAutoLoop **bỏ qua** `ATYPE_MOVE` + `ATYPE_RETURN`
  (tránh giành quyền di chuyển), các ATYPE khác (hồi máu, buff, nhặt, đánh) giữ nguyên.
- Trạng thái thêm CUỐI `struct ExtAuto` (KPlayer.h:221, POD, memset-an toàn).
- Capture hội thoại: struct toàn cục `g_sDTCap` (CoreShell.cpp) + extern ở
  KPlayer.cpp / KProtocolProcess.cpp; mỗi kênh có seq tăng dần: SELECTDIALOG (KPlayer.cpp:7513),
  TALK (KPlayer.cpp:7636), QUESTFINISH (KProtocolProcess.cpp:3901), AFFAIRBOX (:4055),
  SYSMSG "Hệ Thống" (:4128-4134, lọc tiền tố "B¹n nhËn"/"§¹i hiÖp").
- Bảng nhúng `KDaTauTables.h` **SINH TỰ ĐỘNG** bằng python từ 7 file tasklink_*.txt
  (script `D:\GAMEDEVNEW\ReverseTools\gen_datau_tables.py`): buygoods 35 dòng (5 trường +
  tên shop/item TCVN3), findgoods 526 dòng, showgoods 45 dòng, map name→id (TL_MAPTRAPINDEX,
  14 map loại 4 + tọa độ đến), tọa độ NPC Dã Tẩu 10 thành.

Pha (nDTPhase):
```
DTP_OFF → (bDaTau && dữ kiện đủ) → DTP_GOTONPC
DTP_GOTONPC : không ở 1 trong 10 thành → nếu đang có việc dở ở map nv → DTP_FARM;
              ngược lại báo trạng thái + treo. Ở thành: FindPath tới tọa độ NPC 108,
              tới gần (<128) → DialogNpc → DTP_TALK
DTP_TALK    : đợi capture (timeout 3s retry, 5 lần → treo). Phân loại theo text:
              intro(3 opt "Được thôi…")→chọn 0 · course3(2 opt "Đương nhiên…")→đọc số
              lượt còn lại trong câu, chọn 0 · course1(4 opt "ta đã hoàn thành…")→parse
              template 6 loại → DTP_EXEC · "40 lần là đủ"→DTP_DONE · phạt→đợi 10' ·
              cửa sổ thưởng→DTP_REWARD
DTP_EXEC    : loại tắt (bDTType[i]==0) → DTP_SKIP. Ngược lại:
  T1 mua    : tìm 5-trường trong túi → có → DTP_TURNIN; trong rương (bDTUseBox) → rút → TURNIN;
              chưa có → kiểm tiền (thiếu → rút nDTWDMoney vạn) → FindPath tới shop theo Detail
              (0=thợ rèn/vũ khí · 5,6,8=tạp hóa · 10=bán ngựa; tọa độ g_ShopStation/quét NPC
              kind_dialoger theo tên) → DialogNpc → chọn option chứa "giao dịch" (thử lần
              lượt) → shop mở → quét GDI_TRADE_NPC_ITEM khớp 5 trường → SendClientCmdBuy →
              TURNIN. Không thấy hàng (vd ngựa sai thành) → DTP_SKIP.
  T2 tìm    : quét túi + 4 room rương theo luật dòng (5-trường, wildcard 'n', hoặc
              G/D/P + magic id + [min,max] trên nValue[0]) → rương thì rút → TURNIN; không có → SKIP.
  T3 khoe   : như T2 nhưng không mất đồ.
  T4 địa đồ : đang ở map đích? chưa → tới Xa Phu (g_MoveStation) → DialogNpc → chọn index 4
              ("Đến nơi làm nhiệm vụ dã tẩu") → tới map (server bật FightMode) → DTP_FARM:
              thả cho ATYPE_FIGHT/PICKUP chạy quanh tọa độ đến (neo nTempX/Y), đếm X từ
              sysmsg; X≥Num → về (map thành: đi bộ tới NPC; map hoang: Thổ Địa Phù /
              aibacktotown) → TURNIN (không cần give-box).
  T5 chỉ số : parse tên chỉ số: exp→DTP_FARM trên map luyện công CỦA NGƯỜI CHƠI (thả cho
              auto thường tự đi map + đánh; theo dõi delta exp client) đủ → về TURNIN ·
              phúc duyên→đứng tại NPC: dùng Phúc Duyên Lộ (túi/rương, AutoUseItem) → thử
              trả → lặp; 2 lần dùng mà FuYuan không tăng (đọc Npc[].nFuYuan) → SKIP ·
              danh vọng/PK/Tống Kim→thử trả 1 lần (nhỡ đã đủ) → fail → SKIP.
  T6 SHXT   : thử trả 1 lần → fail ("chưa thu thập đủ mảnh") → SKIP.
DTP_TURNIN  : về NPC → main dialog → chọn 1 ("ta đã hoàn thành…") → T1/2/3: đợi give-box
              → kiểm ≥5 ô trống (thiếu → báo + treo) → 2 gói move-item vào pos_affairitem
              → SendUiCmdScript(1, Value2) → đợi kết quả: cửa sổ thưởng = thành công;
              Say "chưa đạt đúng yêu cầu" = fail → SKIP/quay lại EXEC.
DTP_REWARD  : nType≤4 → finish_exp|money|random theo nDTReward1; ngược lại
              finish_point|lucky|item theo nDTReward2 → nói chuyện lại → course 3 → lặp.
DTP_SKIP    : nDTSkipMode==0 → DTP_DONE(treo). ==1 → chọn option 3 (hủy) → hộp xác nhận:
              2 opt (còn lượt) → chọn 0 · 3 opt (hết lượt) → theo nDTCancelMode:
              0→thoát (treo) · 1→chọn 0 (hủy thường, qua thêm 1 hộp xác nhận) · 2→chọn 1
              (100 mảnh; fail "có đúng đã mang 100 mảnh" → mode 1 hay treo tùy nDTCancelMode).
DTP_DONE    : bDTTrainAfter → máy DT nhả lái (nDTEngaged=0) → auto thường tự về map luyện
              công theo cấu hình sẵn của người chơi; sang ngày mới (đổi ngày local) → reset.
```
Mọi bước: 1 hành động/tick + deadline `uDTNext` (600–1500ms), đếm retry, quá 5 lần → treo +
báo trạng thái. Báo trạng thái qua `ChannelMessageArrival` "[DaTau] …" (ASCII, không dấu).

## 4. UI WAUTO — TAB 9 "Dã Tẩu"

- `Resource.h`: chèn khối ID mới 393..~412 (đặt `IDC_*_8_*`), nâng `IDC_INDEX_END`,
  dời các ID popup phía sau (+n). Thêm `IDC_TABBTN_9` (hàng 3, y=96).
- `WAuto.rc` (UTF-16 LE!): khối `//tab 8`: 1 checkbox tổng, 6 checkbox loại, 2 static+combo
  (cách xử lý loại tắt/kẹt, cách hủy), 2 static+combo (thưởng Exp-window, thưởng Point-window),
  checkbox rương, checkbox luyện công, static+edit tiền rút (vạn).
- `WAuto.cpp` (UTF-16 LE): ShowTab nhánh 8 · WM_COMMAND (BN_CLICKED/CBN_SELCHANGE/EN_KILLFOCUS
  case mới) · SaveRoleData đọc → autoData · UpdateUI ngược lại · WM_INITDIALOG đổ combo ·
  LoadRoleData mặc định (bDTType tất cả =1, nDTReward1=0 exp, nDTReward2=2 item,
  nDTWDMoney=50, bDTUseBox=1, skip=treo) · CheckRadioButton bound TABBTN_9.
- Sửa 2 file UTF-16 bằng script python (đọc/ghi utf-16, giữ BOM+CRLF), kiểm bằng byte-diff.

## 5. FILE ĐỤNG TỚI & CÁCH SỬA AN TOÀN

| File | Encoding | Cách sửa |
|---|---|---|
| E:\Src_Auto_Ngoai\WAuto\WAuto\ipc_shared.h · Resource.h | ASCII | Edit thường |
| E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.cpp · WAuto.rc | UTF-16 LE BOM | python utf-16 |
| D:\GAMEDEVNEW\Sources\Core\Src\ipc_shared.h | ASCII | Edit thường |
| CoreShell.h · CoreShell.cpp · KPlayer.h · KPlayer.cpp · KProtocolProcess.cpp · S3Client.cpp | TCVN3/GBK | safe_edit.py |
| Core\Src\KDaTauTables.h (MỚI, sinh tự động) | ASCII+octal | Write |
| ReverseTools\gen_datau_tables.py (MỚI) | ASCII | Write |

D↔E: CoreShell.* / KPlayer.h / KItemList.h / KProtocol.cpp / S3Client.cpp / ipc_shared.h
GIỐNG NHAU → sau khi sửa D, chép kết quả sang E được. **KPlayer.cpp / KProtocolProcess.cpp /
KProtocol.h KHÁC (D có phần bot/SimCity/size_t mà E chưa có) → áp TỪNG HUNK riêng sang E.**

## 6. BUILD & KIỂM CHỨNG

1. Sửa xong grep lại từng file (luật bàn giao #4).
2. Build kiểm sớm ở D: `Core.vcxproj` ClientRelease Win32 (KHÔNG VcpkgEnableManifest) +
   `S3Client.vcxproj` Release Win32 (`/p:VcpkgEnableManifest=false`) — build thẳng vcxproj.
3. Build WAuto: `WAuto.vcxproj` Release Win32 (v143).
4. Phản biện: workflow review đa lens trên toàn bộ diff, verify từng finding trước khi sửa.
5. Áp sang E (hunk riêng cho 3 file lệch) → build ở E đúng cấu hình bảng CRT
   (CoreClient=ClientRelease, Game.exe=Release|Win32) — **đóng game trước** (post-build tự
   chép vào bin\client, file khóa = fail lặng); kiểm dấu thời gian + `re_pe_crt.py` +
   grep chuỗi "[DaTau]" trong CoreClient.dll.
6. Commit + push D (từng nhóm việc), cập nhật memory.

## 6b. PHẢN BIỆN ĐỐI KHÁNG (30 finding, mỗi cái verify độc lập) — ĐÃ SỬA

- **4 CONFIRMED (đều = thiếu watchdog gây soft-lock, KHÔNG lỗi bộ nhớ/dữ liệu/protocol):**
  - DT-1 (HIGH): hội thoại NPC lạ không khớp marker → kẹt vô hạn vì chốt `nDTRetry>4`
    bị reset bởi new-dialog. **Sửa:** thêm `nDTUnknown` (save-restore quanh phân loại;
    chỉ tăng ở nhánh unknown, giữ 0 ở mọi nhánh nhận-dạng); >6 lần → treo 5'.
  - DT-2 (HIGH): Xa Phu không chuyển map → vòng GOXAFU↔XAFUTALK_DONE vô hạn.
    **Sửa:** `nDTXaFuTry`, >5 lần → DT_Skip; reset khi vào FARM.
  - DT-3 (MEDIUM): FARM loại 4 không watchdog. **Sửa:** `uDTFarmStall`, reset khi số cuộn
    TĂNG (`n > nDTProg`), 20' không tiến → DT_Skip.
  - DT-4 (LOW): FARM loại 5-exp không watchdog. **Sửa:** dùng chung `uDTFarmStall`,
    reset khi exp tăng, 20' không tiến → treo 15'.
- **UNCERTAIN DT-1 (cải thiện):** `DT_Has=strstr` khớp tên ngắn là substring của tên dài
  (vd "Giới Chỉ (Kim)" ⊂ "Hoàng Ngọc Giới Chỉ (Kim)"). **Sửa:** `DT_HasName` yêu cầu ký tự
  ngay trước tên là `>` (mọi tên item/tiệm/map đều bọc trong `<color=yellow>...`).
- **UNCERTAIN DT-2 (ghi chú, không sửa):** 2/526 dòng findgoods có typo hệ trong TÊN gốc
  (row 27 nFive=1 nhưng tên "(Kim)"; row 47 nFive=3 nhưng "(Thủy)") — lỗi DỮ LIỆU bảng gốc,
  không phải code; server cũng đọc nFive từ cùng bảng nên nộp đồ vẫn theo nFive; nếu khớp
  nhầm thì server fail (không mất đồ) rồi thử candidate kế.
- 3 field watchdog thêm vào `struct ExtAuto` (engine-side, KHÔNG đụng IPC autoData 6876B):
  `nDTUnknown, nDTXaFuTry, uDTFarmStall`.
- Các FALSE_POSITIVE xác nhận thiết kế đúng: give-box 1 gói khớp UI gốc; box→bag/rút tiền
  gate `m_CUnlocked`; không spam gói; không mất đồ (server không tiêu hủy khi trả sai);
  gate MOVE/FIGHT/RETURN không regression khi bDaTau=0; ID tab-8 contiguous < IDC_INDEX_END;
  nút tab 9 cùng nhóm radio; combo Save/Load đúng index.

## 7. GIỚI HẠN PHIÊN BẢN 1 (nói rõ với người chơi)

- Nhiệm vụ mua NGỰA sai thành (hàng ngựa khác nhau theo thành) → xử lý như "kẹt" (skip).
- Loại 5 PK / Tống Kim / danh vọng: không tự cày — thử trả rồi skip.
- Đứng ngoài 10 thành khi bật auto: máy chỉ chạy khi ở 1 trong 10 thành có NPC (hoặc đang
  dở việc ở map nhiệm vụ); ngoài ra báo "[DaTau] hay dung o thanh co Da Tau".
- Sau crash giữa nhiệm vụ exp: mất baseline → cày lại đủ delta yêu cầu (xấu nhất gấp đôi).
