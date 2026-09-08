# BÀN GIAO CHO PHIÊN SAU — Băng thông đông người + hai lỗi client (07/09/2026, 18:00)

Tệp này viết cho một phiên **không có ngữ cảnh gì**. Đọc hết mục 1 → 4 trước khi gõ dòng lệnh đầu tiên.
Chi tiết số liệu từng đợt nằm ở `D:\GAMEDEVNEW\BANGIAO_BANGTHONG_DONGNGUOI_0609.md` mục 6 → 8.21 (mục này gọi tắt là **BG-GỐC**).

---

## 1. Trạng thái lúc bàn giao

### 1.1 Bản đang chạy thật

| Tệp | Băm | Lúc | Nội dung |
|---|---|---|---|
| `bin\server\CoreServer.dll` | **6693429f** | 17:04 | DELTA a→l + LNCK + TUKICH + TRANPHAI60 + BHLV/BHWS/BHMAP |
| `bin\client\CoreClient.dll` | **ae11479a** | 17:04 | như trên (phần client) |
| `bin\multiserver\S3Relay.exe` | 99cf217d | 14:14 | **cũ** |
| `bin\client\Game.exe` | (phiên UITOADO giữ) | | không đụng |

### 1.2 Khe `.moi` đang chờ chủ swap

| Khe | Băm | Của ai | Ghi chú |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | **43ba6ef9** | phiên này (vá m) | siêu tập của ae11479a, chỉ thêm 3 nhãn log |
| `bin\multiserver\S3Relay.exe.moi` | 18de36e5 | phiên BH100 | phải swap cùng CoreServer có BHLV |

Khe `CoreServer.dll.moi` **đang trống** (chủ đã swap 6693429f lúc 17:04).

### 1.3 Cấu hình vừa đổi (chờ chủ vào lại game)

`bin\client\config.ini`, mục `[Client]`, thêm dòng `Rep3CacheMB=1500` lúc 17:45.
Bản lưu: `config.ini.truoc_rep3cache`. Xem mục 4.1 để nghiệm thu.

`bin\server\config.ini` **không có mục `[Server]`** → mọi khoá DELTA đang chạy mặc định trong mã.

### 1.4 Git

Nhánh làm việc `delta-0709` trong worktree `D:\GAMEDEVNEW_wt_delta`, đã đẩy lên `origin/main` tới **0ad46e38**.
Cây chạy thật là `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`, **không phải** nơi build.

---

## 2. Đường lối chủ đã chốt (đọc kỹ, đây là ràng buộc)

1. **Không giới hạn phương án.** Chủ nói rõ: bản test chỉ mình chủ chơi, được đổi giao thức, không phải giữ tương thích với client cũ ngoài đời.
2. **Không hy sinh trải nghiệm.** Cấm cắt gói, cấm giảm tầm nhìn, cấm bỏ số sát thương, cấm hạ tần suất đồng bộ để lấy băng thông. Mọi thứ giảm được phải giảm bằng cách **gửi ít byte hơn cho cùng một lượng thông tin**, không phải bằng cách gửi ít thông tin hơn.
3. **Không hiểu thì hỏi chủ**, đừng đoán rồi vá bừa. Riêng chỗ đụng luật chơi thì bắt buộc hỏi.
4. **Đo trước, sửa sau.** Mỗi lần sửa phải kèm bộ đếm để lần kéo log sau đọc là biết đúng hay sai. Đã ba lần bộ đếm chứng minh giả thuyết của tôi sai (xem mục 5.3) — giữ nếp này.

---

## 3. Đã làm gì (chuỗi vá a → m) và kết quả đo

Nguyên tắc chung của cả chuỗi: gói đồng bộ cũ gửi **toàn bộ** trạng thái mỗi lần dù chỉ một trường đổi. Cách chữa là **băm nội dung, chỉ gửi khi đổi thật**, và tách phần đổi nhanh sang gói gọn riêng.

| Vá | Nội dung | Kết quả đo |
|---|---|---|
| a | Gói vị trí gọn mã **221** (`NPC_POS_SYNC`, nay 28 byte) thay gói đầy đủ 98 byte; băm delta; hello `c2s_deltahello = 175` để client cũ không nhận mã lạ | 221 chiếm 46 % byte, client 24,8 KB/s |
| b | Đẩy đệm mỗi tick, tắt Nagle phía máy chủ | — |
| c | Vòng bất tử băm 0/1 (máy chủ trừ mỗi tick nên trước đó luôn "đổi") | gói 77 từ 17 % → 10,5 % |
| d | Cờ chiến đấu đi theo bit `STATE_FIGHTMODE` của gói 77/221, bỏ khỏi băm gói 75 | 75 từ 15 % → 12,1 % |
| e | Bộ đếm chẩn đoán nhóm trường của gói 75 | chỉ ra 90 % là "nhiều nhóm cùng lúc" |
| f | Ngựa + tốc độ đi/chạy vào gói 221 (25 → 28 byte), bỏ khỏi băm 75 | lần phát lại 75 do lên/xuống ngựa **32.649 → 131** |
| g | Số sát thương gọn mã **222** (`DAMAGESHOW_GON` 13 byte, bỏ `dwLauncher`) cho người xem; hai người trong cuộc bị loại khỏi phát vùng vì đã nhận gói riêng (trước đó nhận trùng 2 gói mỗi đòn); hello v2; kỳ làm mới 75 lên 300 s | 207 từ 25 % → 8,3 % |
| h | **Sửa hồi quy của d/g**: client `SyncNpcMin` loại trừ chính mình khi áp `STATE_FIGHTMODE` → cờ chiến đấu của mình kẹt tới 300 s (phù về thành vẫn "đánh skill được") | phiên TUKICH đo giúp, đã hết |
| i | Kỳ làm mới gói đầy đủ 10 → 60 s; làm mới 75 dàn đều theo `m_Index % 128` giây; đếm XOR từng bit cờ | 75 → 7,4 %; **bot=0** (bot không còn gây phát lại 75) |
| j | **Nằm bẹp**: chặn gói đồng bộ đầy đủ đặt chính nhân vật về `do_death` (điều kiện chặn cũ là tautology luôn đúng) + nhãn `[S7-NAMBEP]` | nhãn sai điều kiện, xem mục 4.2 |
| k | Đếm nhóm trường của **băm chậm**; sửa `[PS-BO]` in phần trăm âm do tràn int | chỉ đích danh: `StateInfo` 74,7 %, chỉ số tối đa 15,2 %, tốc độ **0** |
| l | Gói trạng thái gọn mã **223** (`NPC_STATE_SYNC` 39 byte = mã + ID + `StateInfo[18]` + 4 chỉ số tối đa) thay gói đầy đủ 98 byte; hello v3 | quyết định phát gói đầy đủ **4.886 → 711 mỗi 10 s (−85 %)**; gói 77 biến mất khỏi bảng byte |
| m | Nhãn `[S7-SAUHOISINH]` (+1/+3/+6 s sau mỗi lần hồi sinh) và `[S7-NAMBEP-LAU]` (tư thế chết > 10 s) | chờ nghiệm thu |

**Thành phần byte tới client sau vá l** (7,7 phút, chỗ thưa người): 221 gọn 61,3 % · gói ngoại hình 75 12,1 % · số sát thương 222 8,5 % · chiêu 148 5,3 % · trạng thái 223 5,0 % · lệnh chạy 86 4,2 %.
Máy chủ 8,5 ms mỗi nhịp (15 % ngân sách 55 ms), 0 lỗi luồng, 0 giật, không sập.

**Ba mã gói mới, nhớ kỹ vị trí bảng cỡ** (`Sources/Core/Src/KProtocol.cpp`):

| Mã | Cấu trúc | Ô trong bảng cỡ client | Cổng hello |
|---|---|---|---|
| 221 `s2c_syncnpcpos` | `NPC_POS_SYNC` 28 B | 156 | v1 |
| 222 `s2c_showdamagegon` | `DAMAGESHOW_GON` 13 B | 157 | v2 |
| 223 `s2c_syncnpcstate` | `NPC_STATE_SYNC` 39 B | 158 | v3 |
| 175 `c2s_deltahello` | `C2S_DELTA_HELLO` | ô 110 phía máy chủ | — |

**Luật cổng hello**: máy chủ chỉ phát mã mới khi **mọi** client đang nối đã báo phiên bản đủ (`NS_SoClientCu`, `NS_SoClientCu2`, `NS_SoClientCu3` trong `KNpc.cpp`). Nhờ vậy swap lệch một bên không bao giờ hỏng luồng. Thêm mã mới thì phải nâng phiên bản hello và thêm một hàm đếm tương tự.

---

## 4. Việc đang treo — làm theo thứ tự này

### 4.1 Nghiệm thu `Rep3CacheMB=1500` (ưu tiên cao nhất, chỉ cần chủ vào lại game)

**Bệnh:** đang đánh nhau mất hết hình ảnh kỹ năng, chỉ còn động tác đánh.
**Đã chẩn:** không phải mạng. Gói chiêu 148/95 vẫn tới 1.000–1.700 gói mỗi 10 giây. Bộ nhớ ảnh của client dính 508–511 MB trên ngân sách 512 MB suốt trận; khi vượt ngân sách, `TextureResMgr::CheckBalance` bỏ tới 8 khung ảnh của **mọi tài nguyên nghỉ quá 1 giây** (bình thường 10 giây). Hiệu ứng chiêu dùng thưa nên bị bỏ rồi nạp lại không kịp; ảnh nhân vật vẽ mỗi khung nên không bao giờ bị bỏ.
**Trần 512 MB nằm ở** `Sources/Represent/Represent3/TextureResMgr.cpp:97`, khoá ini đọc **sau** cái kẹp nên ghi đè được.

**Kiểm sau khi chủ vào lại game:**

```bash
grep "REP3\] cache texture" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_rep3.log" | tail -2
```

Phải thấy `ngan sach 1500 MB`. Sau đó xem dòng thống kê 30 giây: `texture ... MB` phải **rời khỏi trần** (dưới 1.500 rõ rệt) và số mục bỏ mỗi kỳ phải giảm mạnh. Nếu hiệu ứng vẫn mất mà cache không còn đầy thì bệnh nằm chỗ khác, quay lại đo `LoadImage FAIL` và luồng nạp.

**Việc nên làm tiếp:** nâng luôn cái kẹp trong mã (ví dụ theo VRAM trống thay vì hằng số 512), để máy khác không phải sửa ini. Cần build client.

### 4.2 Nghiệm thu vá m rồi chốt lỗi nằm bẹp

**Bệnh:** chết, về thành rồi vẫn nằm dưới nền đất.
**Đã loại trừ:** máy chủ hồi sinh đúng 0,70–0,78 giây mỗi lần; client 15/15 lần đứng dậy trong 0,2 giây theo nhãn cũ; lớp vẽ không kẹt quá 3 nhịp; không có ảnh nhân vật nào nạp hỏng.
**Đã sửa một gốc thật (vá j):** trong `KProtocolProcess.cpp` hàm `SyncNpc`, điều kiện `if (m_Doing != do_death || m_Doing != do_revive)` **luôn đúng** (một giá trị không thể vừa bằng hai thứ), nên gói đồng bộ đầy đủ luôn áp trạng thái máy chủ cho **cả chính nhân vật**, kể cả trạng thái chết. Nay đã chặn cho chính mình và ghi `[S7-NAMBEP-CHAN]` mỗi lần chặn.
**Nhãn cũ hỏng:** điều kiện "còn máu mà vẫn tư thế chết" không bao giờ đúng, vì khi kẹt thì client cũng giữ máu bằng 0 (`NetCommandDeath` đặt 0). Mười một lần chết không ghi dòng nào.

**Đọc log sau khi swap `43ba6ef9`:**

```bash
grep -a "S7-SAUHOISINH\|S7-NAMBEP" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_auto.log" | tail -30
```

Mỗi lần chết phải có đúng 3 dòng `[S7-SAUHOISINH]` ở +1 s, +3 s, +6 s. Đọc theo bảng này:

| Dòng +3 s cho thấy | Nghĩa là | Hướng sửa |
|---|---|---|
| `doing=1 cdoing=1 resdoing=1` | đứng bình thường | không phải lần này, chờ lần sau |
| `doing=10` (chết) hoặc `cdoing=8` | **logic client** kẹt ở trạng thái chết | tìm ai đặt lại; nếu có dòng `[S7-NAMBEP-CHAN]` thì chính gói đồng bộ đầy đủ là thủ phạm và vá j đã chặn đúng |
| `doing=1 cdoing=1` nhưng `resdoing=8` hoặc `resaction=36` | **lớp vẽ** không đổi tư thế | soi `KNpcRes::SetAction`; nghi ảnh SPR của tư thế đứng không nạp được → thêm nhãn in tên tệp SPR đang vẽ |
| có `[S7-NAMBEP-LAU]` | kẹt trên 10 giây, chắc chắn là bệnh | đọc luôn ba trường `doing/cdoing/resdoing` trong dòng đó |

`do_death = 10`, `do_revive = 21`, `do_stand = 1`; `cdo_stand = 1`, `cdo_death = 8`.

### 4.3 Chuẩn bị cho 500 người thật (việc lớn nhất còn lại)

**Điều phải nhớ trước tiên:** hôm nay **chỉ có một kết nối thật**. Một nghìn bot chạy trong lòng máy chủ, `m_nNetConnectIdx = -1`, nên **đường gửi chưa bao giờ bị thử**. Máy chủ phát tán 175.000 lượt mỗi giây nhưng chỉ gửi thật 1.421 lần mỗi giây.

Ngoại suy 500 người cùng một chiến trường: byte và số gói **mỗi người** không đổi, nhưng tổng thành 80–240 Mbps và 400.000–1.500.000 lần gọi hàm gửi mỗi giây, tức khoảng 83.000 lần trong một nhịp 55 ms.

**Chỗ vỡ đầu tiên không phải đường truyền mà là một ổ khoá chung.** `CIOCPServer::PackDataToClient` (`Sources/MultiServer/Heaven/ServerStage.cpp:390`) lấy **hai** khoá mỗi gói: `m_csCM` (khoá chung toàn bảng client) rồi `csWriteAction` (khoá riêng client). 83.000 lần mỗi nhịp nhân 150–500 ns là 12–42 ms, cộng 8,5 ms hiện tại thì vượt ngân sách 55 ms. Hàm đẩy đệm mỗi nhịp cũng giữ đúng khoá chung đó trong khi duyệt cả 500 client.

**Ba bước theo thứ tự:**

1. **Phép thử nhân bản đường gửi** (`[Server] MoPhongNhanBan=500`, mặc định 0). Mỗi lần gửi thì chép thêm N−1 lần vào bộ đệm rác và đếm byte, để đo đúng chi phí CPU và băng thông của 500 người mà không cần 500 máy. Rẻ nhất, cho số chắc chắn, không đụng luật chơi.
2. **Bỏ khoá chung khỏi đường nóng**: khoá riêng từng client là đủ cho bộ đệm ghi. Đây là đòn bẩy CPU lớn nhất. Đụng lõi mạng nên phải soi kỹ ai còn dùng `m_csCM` (danh sách client, lúc thêm/bớt kết nối) và thử kỹ.
3. **Thưa theo khoảng cách khi vùng đông**: quá 16 ô thì hạ nhịp đồng bộ vị trí còn một nửa, chỉ bật khi vùng đông người. Gói 221 đang chiếm 61 % byte nên đây là chỗ duy nhất còn nhiều mỡ. **Đụng trải nghiệm nên phải hỏi chủ trước.**

**Không còn là vấn đề:** trần người nhận mỗi lượt phát đã là 100.000 (`BroadCastDongBo`, chưa lần nào cắt), van 5.000 gói mỗi giây mỗi client chưa chạm, `MaxPlayer=1500` trong `GameServer_cfg.ini`. 500 người **tản khắp thế giới** thì không có vấn đề gì, vì phát tán tính theo vùng.

### 4.4 Tách map chiến trường ra một nhân CPU riêng — chủ hỏi 17:50, trả lời: không an toàn

Vòng chính (`KSubWorldSet::MainLoop`) duyệt tuần tự từng map trong một luồng. Ngay trong mã đã có ghi chú rằng lần trước **đã bỏ** vòng đa luồng vì `SubWorld[i].Activate()` dùng chung mảng `Npc[]` và `Player[]` toàn cục **không có khoá**. Đạn, tổ đội, bang hội, giao dịch và script đều xuyên map nên một luồng riêng cho chiến trường vẫn ghi vào cùng mảng đó. Đó là loại lỗi hỏng dữ liệu ngẫu nhiên, không tái hiện được.

Thêm nữa, CPU **chưa phải** chỗ nghẽn: mỗi nhịp 8,5–9,1 ms trên ngân sách 55 ms. Chỗ vỡ khi đông người là đường gửi (mục 4.3), tách map không chạm tới nó.

Nếu chủ vẫn muốn song song hoá thì lộ trình an toàn là: đưa **đường gửi** sang luồng riêng trước (tách bạch, không đụng trạng thái game), sau đó mới tính tới việc song song hoá một pha thuần tính toán trong tick.

### 4.5 Mỡ còn lại, theo thứ tự

| Mục | Tỉ lệ byte | Ghi chú |
|---|---|---|
| Gói 221 vị trí gọn | 61 % | Giá của đồng bộ vị trí dày. Chỉ giảm được bằng đánh đổi, chủ phải quyết |
| Gói 75 ngoại hình | 12 % | Còn lại là cặp cờ PK (0x01/0x10 đổi cùng nhau, 831 lần) và nhóm rank/danh vọng (1.115 lần) của người thật. Cách xử giống vá l: tách sang gói gọn hoặc chỉ gửi ở kỳ làm mới |
| Gói 222 số sát thương | 8,5 % | Đã gọn 13 byte, hết cách trừ khi bớt thông tin |
| Gói 148/95 chiêu | 8 % | Chưa soi |
| Gói 223 trạng thái | 5 % | Vừa thêm, đang thay chỗ của gói 98 byte |

---

## 5. Cách làm việc (bắt buộc theo)

### 5.1 Sửa mã nguồn JX1 — luật sống còn

Nguồn game là **TCVN3 một byte**, có tệp bọc UTF-8. **Cấm dùng Edit/Write thẳng lên nguồn game** — sẽ nát toàn bộ tiếng Việt trong tệp.

Cách duy nhất: viết một script Python vá theo mẫu `ReverseTools/goi_va_delta*.py`, trong đó:

- đọc và ghi bằng `io.open(p, encoding="latin-1", newline="")`;
- mọi neo và mọi đoạn chèn chỉ dùng **ASCII**, tiếng Việt trong chú thích thì viết không dấu;
- đổi `\n` thành `\r\n` trước khi so khớp (mọi tệp đều CRLF);
- đếm số byte cao trước và sau, khác nhau là hỏng, không được ghi;
- kiểm tra số lần khớp đúng bằng số mong đợi rồi mới thay.

Sau khi vá, chạy `python "C:\Users\nguye\.claude\skills\swordonline-dev\scripts\check_encoding.py" <tệp>` và kiểm `bare-LF=0`.

Script vá dùng xong thì chép vào `ReverseTools/` và commit, để phiên sau tái áp được.

### 5.2 Build và đặt bản swap

Máy chủ:

```bash
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe" "D:/GAMEDEVNEW_wt_delta/Sources/Core/Core.vcxproj" "/p:Configuration=Server Release" "/p:Platform=x64" "/p:SolutionDir=D:/GAMEDEVNEW_wt_delta/" "/p:PostBuildEventUseInBuild=false" /m:8 /nologo /v:m /t:Build
```

Client thì đổi thành `Client Release` và `Win32`. Kết quả nằm ở `Sources/Core/x64/ServerRelease/CoreServer.dll` và `Sources/Core/ClientRelease/CoreClient.dll`.

**Trước khi build luôn `git fetch origin` rồi `git merge origin/main`**, vì có 3–5 phiên khác cùng sửa cây này.

Đặt bản swap là chép đè thành `<tên>.moi` trong `bin\server` hoặc `bin\client`; chủ chạy `ChayGameServer.bat` / `ChoiGame.bat` để nuốt.

### 5.3 Chung khe `.moi` với phiên khác — đã va chạm nhiều lần

Trước khi đè một `.moi` không phải của mình:

```bash
python "<scratchpad>/chk2.py" <bản live> <bản .moi của họ> <bản mình vừa build>
```

Dòng `missing in mine` phải bằng 0 (bỏ qua chuỗi rác nhị phân kiểu `L;%xy>iH`). Nếu thiếu chuỗi tính năng thật thì **chưa được đè**: fetch lại origin/main, build lại, kiểm lại.

Rồi đổi tên bản của họ (`CoreServer.dll.moi.bhws_55c9c3fd` chẳng hạn), đặt bản mình vào, và **nhắn cho phiên đó** bằng `mcp__ccd_session_mgmt__send_message`, nói rõ băm mới, đã gộp commit nào của họ, và tệp nào họ phải swap kèm.

Các phiên đang cùng làm hôm nay: bang hội (BH100/BHLV/BHWS + S3Relay), tự kích chuột (TUKICH), WAuto bán rác (HC-CAT), giao diện toạ độ (UITOADO).

### 5.4 Kéo log và đo

Công cụ chính trong scratchpad của phiên:

- `ptich_tran_tk.py <pid> <t0> <t1>` — đọc cả `jx_auto_server.log.1` và `jx_auto_server.log`, chia cửa sổ 10 giây, in bảng thành phần byte theo mã gói, đỉnh KB/s, quyết định đồng bộ, và 12 kỳ `[PERF]` gần nhất. Truyền `0 0` để lấy toàn bộ theo pid.
- `chk2.py` — so chuỗi ba tệp nhị phân, dùng cho luật siêu tập ở 5.3.

Nơi đọc:

| Tệp | Ở đâu | Đọc gì |
|---|---|---|
| `jx_auto_server.log` (+ `.log.1`) | `bin\server` | mọi nhãn máy chủ, xoay vòng ở 64 MB |
| `jx_perf_server.log` | `bin\server` | `[PERF]` nhịp tick, có `pid=` để tách tiến trình |
| `logs\hethong.log` | `bin\server` | mốc Tống Kim, `[BOTAUTO]` |
| `jx_auto.log` (+ `.log.1`) | `bin\client` | nhãn client, xoay vòng ở 64 MB |
| `jx_paint.log` | `bin\client` | `[SUM]` lượt vẽ, giật |
| `jx_rep3.log` | `bin\client` | bộ nhớ ảnh, `LoadImage FAIL` |
| `jx_crash.log` | `bin\client` | mốc khởi động, sập |

Nhãn quan trọng:

| Nhãn | Ý nghĩa |
|---|---|
| `[NS-BO]` | quyết định đồng bộ mỗi 10 giây: bỏ / gọn / đầy đủ / gọn thêm, kèm nhóm băm chậm nào đổi |
| `[NS-TT]` | số gói trạng thái 223 phát mỗi 10 giây |
| `[PS-BO]` | gói ngoại hình 75: gửi/bỏ, nguyên nhân đổi theo nhóm và theo bit cờ, tách bot/người |
| `[DMG-GON]` | số sát thương phát gọn hay đầy đủ |
| `[BC-DEM]` | lượt phát tán, gửi thật, node duyệt, cắt vì hết ngân sách |
| `[BC-LOAI]` | phân tích theo mã gói: phát / gửi / cắt |
| `[S7-*]` | luồng chết và hồi sinh, hai bên |
| `[S9-VE]` / `[S9-KET]` | lớp vẽ của chính nhân vật |
| `[S6-*]` | đồng bộ NPC phía client, nạp map, NPC mồ côi |

**Bẫy khi đọc:** so KB/s giữa hai đợt chỉ có nghĩa khi mật độ giống nhau. Dùng `BroadCast/10s` và số gói 222 trong bảng để biết đoạn đó đông hay thưa. Con số không phụ thuộc chỗ đứng là các bộ đếm máy chủ (`day`, `gui`, `bo`).

### 5.5 Bẫy đã dính, đừng dính lại

1. **Bash heredoc nuốt một nửa dấu gạch chéo ngược.** Viết `\\U` thành `\U` làm Python báo lỗi escape. Viết tài liệu hay script có dấu gạch chéo thì dùng công cụ ghi tệp, đừng dùng heredoc.
2. **In tiếng Việt ra console Windows** báo lỗi cp1252. Đặt `export PYTHONIOENCODING=utf-8` trước khi chạy.
3. **Biến toàn cục dùng ở cả hai bản** phải định nghĩa **ngoài** mọi khối `#ifdef _SERVER`, nếu không client báo thiếu ký hiệu lúc liên kết. Đã dính với `g_uS7LucHoiSinh`.
4. **Hàm dùng trước khi định nghĩa** trong cùng tệp phải khai báo trước, kể cả hàm tĩnh.
5. **Thêm mã gói mới** thì phải: khai báo trong `Headers/KProtocolDef.h`, thêm cấu trúc trong `KProtocol.h`, thêm cỡ vào **đúng ô** trong `KProtocol.cpp`, đăng ký hàm xử lý, và nâng phiên bản hello. Thiếu ô bảng cỡ thì client coi là gói độ dài động và lệch luồng ngay.
6. **`day/(gon+day)` thấp không có nghĩa là gói đầy đủ ít byte.** Nó là tỉ lệ quyết định trên toàn bộ NPC; phải nhìn bảng byte tới client mới biết tác động thật.

---

## 6. Bản đồ mã cho các việc trên

| Việc | Tệp và chỗ |
|---|---|
| Quyết định đồng bộ, băm nhanh/chậm, gói 221/223 | `Sources/Core/Src/KNpc.cpp`, hàm `NormalSync` |
| Gói ngoại hình 75 và bộ đếm | cùng tệp, khối `[PS-BO]` |
| Số sát thương và gói 222 | cùng tệp, hàm `SyncDamageInfo` |
| Phát tán theo vùng, van, tầm 32 ô | `Sources/Core/Src/KRegion.cpp`, hàm `BroadCast` |
| Client nhận 221/222/223, hello, cờ chiến đấu | `Sources/Core/Src/KProtocolProcess.cpp` |
| Chết và hồi sinh phía client | cùng tệp, `PlayerRevive`, `NetCommandDeath` |
| Nhãn lớp vẽ và nằm bẹp | `Sources/Core/Src/KNpc.cpp`, hàm `Activate` |
| Bộ nhớ ảnh của client | `Sources/Represent/Represent3/TextureResMgr.cpp` |
| Đường gửi gói, ổ khoá chung | `Sources/MultiServer/Heaven/ServerStage.cpp:390` và `450` |
| Vòng chính, duyệt map | `Sources/Core/Src/KSubWorldSet.cpp`, hàm `MainLoop` |

---

## 7. Câu hỏi đang chờ chủ trả lời

1. Có cho thưa đồng bộ vị trí theo khoảng cách khi vùng đông không (mục 4.3 bước 3)? Đây là chỗ duy nhất còn giảm được nhiều, nhưng đụng trải nghiệm nên tôi không tự làm.
2. Có muốn tôi làm phép thử nhân bản 500 lần trước, hay làm thẳng việc đưa đường gửi sang luồng riêng?
3. Có nâng luôn trần bộ nhớ ảnh trong mã không, hay cứ để mỗi máy tự sửa ini?
