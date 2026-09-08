# BÀN GIAO: BĂNG THÔNG VÀ SỨC CHỨA KHI ĐÔNG NGƯỜI (06/09/2026 23:30)

**Đọc tài liệu này trước khi động vào bất cứ thứ gì liên quan tới phát tán gói, đồng bộ vị trí, hay ý định chia luồng GameServer.** Tài liệu đi kèm: `PHANTICH_DALUONG_GAMESERVER_0609.md` (đo tải và trần một nhân).

Chủ hỏi: *"nâng trần lên đánh đổi điều gì? Tôi nghĩ phần nặng ảnh hưởng tới game lúc đông là đường truyền, hình ảnh, đạn skill, sync vị trí; nếu chia luồng được các phần này thì có thể khắc phục"* rồi yêu cầu *"phân tích sâu 1-2-3-4 một lần nữa, viết rõ để phiên sau đọc và làm theo"*.

---

## 0. KẾT LUẬN TRƯỚC, ĐỂ KHÔNG ĐỌC NHẦM

1. Trần gần nhất khi đông người là **băng thông**, không phải số nhân CPU. Đo được: gói đồng bộ vị trí nặng **99 byte**, van hiện tại cho mỗi client là **1.500 gói/giây**, ra **1,2 Mbps cho mỗi người chơi**. Đường 1 Gbps chỉ gánh nổi khoảng 800 người trong điều kiện đông đúc.
2. **Chia luồng GameServer không cứu được băng thông.** Ba trong bốn thứ chủ nêu (đường truyền, sync vị trí, hình ảnh) không nằm ở chỗ chia luồng giúp được.
3. **Đính chính quan trọng so với những gì tôi nói trước đó:** trần gói **vị trí đang là 500**, còn trần gói **chiến đấu chỉ là 100**. Tôi từng nói ngược. Xem mục 3.
4. Việc rẻ nhất và đúng nhất để làm ngay là **việc số 3** (nâng trần gói chiến đấu). Việc số 1 và 2 giảm băng thông 6 tới 10 lần nhưng phải phát hành client mới.

---

## 1. SỐ ĐO NỀN (06/09 22:00–23:00, 1.001 trực tuyến đều là bot, chỉ ~1 người thật)

Nguồn: `jx_auto_server.log`, bộ đếm `[BC-DEM]` và `[BC-LOAI]` in mỗi 10 giây từ `KRegion.cpp:1660-1684`.

```
[BC-DEM]  10s: goi=2404405 gui=832 cat_vi_het_ngan_sach=0 node_duyet=2589006 ngoai_tam=100 tam=32 han_muc=1500/giay
[BC-LOAI] 10s loai:goi/gui/cat: 75:15787/18/0 77:1541677/574/0 79:555/0/0 80:46789/0/0 82:527/0/0 84:1004/4/0 85:7577/2/0 86:97462/70/0
```

Giải mã mã gói (tra từ `Headers\KProtocolDef.h`, enum chứa `s2c_syncnpcmin`):

| Mã | Tên | Lượt phát mỗi giây | Tỉ lệ | Ghi chú |
|---|---|---|---|---|
| 77 | `s2c_syncnpcmin` | **154.168** | **90,0 %** | Đồng bộ vị trí NPC, 99 byte. Mục tiêu chính. |
| 86 | `s2c_npcrun` | 9.746 | 5,7 % | Lệnh chạy |
| 80 | `s2c_syncobjstate` | 4.679 | 2,7 % | Trạng thái vật thể |
| 75 | `s2c_syncplayermin` | 1.579 | 0,9 % | Ngoại hình người chơi (đã có lọc băm từ 04/09) |
| 85 | `s2c_npcwalk` | 758 | 0,4 % | Lệnh đi |
| 84, 79, 82 | `npcremove`, `objadd`, `objremove` | 208 | 0,1 % | |

**Chú ý quan trọng:** trong lần đo này **không có gói chiến đấu nào** (91 `npchurt`, 92 `npcdeath`, 95 `skillcast` đều bằng 0) vì bot lúc đó chỉ đi lại, không đánh nhau. Khi có đánh nhau thật thì ba mã này xuất hiện và chúng đang bị trần 100. Phiên sau muốn số liệu đúng thì phải đo lúc có chiến đấu.

Tỉ lệ `node_duyet / goi` hiện là **1,08**, nghĩa là mỗi vùng trung bình chỉ có một người. Bot rải đều khắp bản đồ. Khi người tụ một chỗ, tỉ lệ này lên tới 100 hoặc 500 tuỳ trần, và đó là lúc chi phí quét nổ.

---

## 2. CẤU TRÚC GÓI VÀ ĐƯỜNG ĐI (đọc kỹ trước khi sửa)

### 2.1 Gói chính `NPC_NORMAL_SYNC` — 99 byte

Định nghĩa: `Sources\Core\Src\KProtocol.h:505-530`. Tệp đang trong `#pragma pack(push, enter_protocol)` từ dòng 18.

| Trường | Kiểu | Byte | Đổi mỗi tick? |
|---|---|---|---|
| `ProtocolType` | BYTE | 1 | không |
| `ID` | DWORD | 4 | không |
| `MapX`, `MapY` | int ×2 | 8 | **có** |
| `m_fkRegionID` | DWORD | 4 | hiếm |
| `m_fkOffX`, `m_fkOffY` | int ×2 | 8 | **có** |
| `Camp`, `Doing`, `State`, `m_bySeries` | BYTE ×4 | 4 | `Doing`/`State` có, còn lại hiếm |
| `m_nProtectedTime` | int | 4 | hiếm |
| `m_CurrentLife`, `m_CurrentLifeMax`, `m_LifeMax` | int ×3 | 12 | chỉ khi đánh nhau |
| `m_WalkSpeed`, `m_RunSpeed`, `m_ASpeed`, `m_CSpeed` | int ×4 | 16 | rất hiếm |
| `m_CurrentMana`, `m_CurrentManaMax`, `m_ManaMax` | int ×3 | 12 | hiếm |
| `MissionGroup` | int | 4 | rất hiếm |
| `StateInfo[18]` | BYTE ×18 | 18 | hiếm |
| `NpcEnchant` | int | 4 | rất hiếm |
| **Tổng** | | **99** | |

`MAX_SKILL_STATE = 18`, khai ở `Sources\Core\Src\GameDataDef.h:446`.

**Chỗ phát:** `KNpc::NormalSync()` tại `Sources\Core\Src\KNpc.cpp:6792`. Gửi tới vùng hiện tại rồi 8 vùng kề, tổng 9 lượt `BroadCast` cho mỗi lần đồng bộ (dòng 6865 và vòng `for (j = 0; j < 8; j++)` ngay sau).

**Chỗ gọi:** `KRegion::Activate()` tại `Sources\Core\Src\KRegion.cpp:768`. Mỗi vùng mỗi tick chỉ đồng bộ **5 NPC** (`kNpcSyncChunkSize = 5`) theo con trỏ xoay `m_nNpcSyncCursor`. Nghĩa là số lượt đồng bộ **không** tăng theo số NPC trong vùng, chỉ số node phải quét mới tăng.

**Chỗ nhận:** `KProtocolProcess::SyncNpcMin()` tại `Sources\Core\Src\KProtocolProcess.cpp:2224-2469`, đăng ký ở dòng 229.

Đã kiểm bằng công cụ đếm: **mọi trường đều được dùng ít nhất một lần** ngoài dòng ghi log. Nên **không được cắt bừa trường nào**; phải làm theo kiểu hai gói, xem mục 3.1.

### 2.2 Gói phụ đi kèm khi NPC là người chơi

Cũng trong `KNpc::NormalSync()`:

- `PLAYER_NORMAL_SYNC` mã 75, phần ngoại hình. Đã có lọc băm FNV-1a từ 04/09 (`[PS-BO]` trong log), bỏ được khoảng 62 % số lần gửi. Đừng làm lại việc này.
- `NPC_PLAYER_TYPE_NORMAL_SYNC` mã 78, chỉ **29 byte**, định nghĩa ngay sau `NPC_NORMAL_SYNC` trong `KProtocol.h`. Quan trọng: gói này **không phát tán**, chỉ gửi riêng cho chính người chơi đó qua `PackDataToClient` tại `KNpc.cpp:7036`. Vì thế nó không xuất hiện trong `[BC-LOAI]`. **Đây là bằng chứng sẵn có rằng một gói vị trí gọn hoàn toàn khả thi trong engine này.**

### 2.3 Vòng phát tán `KRegion::BroadCast`

Tại `Sources\Core\Src\KRegion.cpp:1560` trở đi. Điểm cần nhớ:

- Tham số `int &nMaxCount` là **ngân sách dùng chung cho cả 9 vùng**, truyền theo tham chiếu.
- Chỉ trừ ngân sách khi **thật sự gửi** cho người có kết nối. Bot (`m_nNetConnectIdx < 0`) không ăn suất. Đây là sửa `[F4 04/09]`, đừng làm hỏng.
- Có con trỏ xoay `m_nBroadCastCursor` để khi số người vượt trần thì mỗi lần phát phục vụ một đoạn khác nhau, tránh người xếp sau thành vô hình.
- Lọc tầm: `|dx| <= 32` và `|dy| <= 32`, chỉnh bằng khoá `[Server] BroadCastTam` trong `config.ini`.
- Van thứ hai: `[Server] BroadCastGoiToiDa` mặc định **1.500** gói vị trí mỗi giây cho mỗi client, biến `g_nBCHanMuc`. Chỉ áp cho các mã mà `BC_LaGoiViTri()` trả đúng, tức 75, 77, 85, 86 (hàm ở `KRegion.cpp:34`).

### 2.4 Ba mức trần hiện hành

Khai ở `Sources\Core\Src\KRegion.h`:

```
dòng 16: #define MAX_BROADCAST_COUNT        100
dòng 18: #define NPC_SYNC_BROADCAST_LIMIT   500
dòng 23: #define NPC_EVENT_BROADCAST_LIMIT  100000
```

Bảng đầy đủ nơi dùng, rút bằng cách quét `KNpc.cpp` theo hàm bao:

| Hàm | Dòng | Trần đang dùng | Loại việc |
|---|---|---|---|
| `SetCurrentCamp` / `SetCamp` / `RestoreCurrentCamp` | 625, 661, 696 | 100 | đổi phe |
| `ProcessState` | 1398, 1458 | 100 | trạng thái |
| `DoDeath` | 1832, 1833 | 100000 | chết, miễn ngân sách |
| `DoHurt` | 2075 | **100** | **trúng đòn** |
| `DoRun` / `DoWalk` / `DoJump` / `DoSit` | 2644, 3380, 6212, 2749 | 100 | di chuyển |
| `DoSkill` | 2936 | **100** | **ra chiêu** |
| `DoPlayerTalk` | 3436 | 100 | chat |
| `CastAutoSkillAt` | 3612 | **100** | **tự đánh** |
| `HS_BroadcastDetonate` | 3647 | 100 | kích nổ Hoa Sơn |
| `SyncDamageInfo` | 3960 | **100** | **số sát thương** |
| `Cast` | 6139 | **100** | **thi triển** |
| `NormalSync` | 6860 | 100 | (nhánh phụ) |
| `NormalSync` | 6865, 7010 | **500** | **đồng bộ vị trí** |
| `BroadCastRevive` | 7065 | 100 | hồi sinh |
| `SendDataToNearRegion` | 9829 | 100 | chung |
| `SetPos` / `ChangeWorld` | 11059, 11157 | 100000 | dịch chuyển, miễn ngân sách |

**Đây là chỗ tôi từng nói sai.** Gói vị trí vốn đã được ưu ái trần 500, còn ra chiêu và sát thương chỉ có 100.

### 2.5 Mã giao thức còn trống

Enum trong `Headers\KProtocolDef.h` có 188 mục, mã cuối là 254 (`s2c_end`). Còn **66 mã trống**, các đoạn liền: **1-9, 15-31, 39-47, 62-63, 221-249**.

Đoạn **221-249** là chỗ tốt nhất để thêm gói mới, vì nằm cuối và không làm dịch mã của bất cứ gói nào đang dùng. **Tuyệt đối không chèn giữa enum**, làm vậy là đổi số hiệu mọi gói phía sau và phá tương thích client theo Cổng 2.

---

## 3. BỐN VIỆC, PHÂN TÍCH SÂU

### 3.1 Việc 1 — Thu nhỏ gói đồng bộ vị trí

**Ý tưởng:** tách `NPC_NORMAL_SYNC` 99 byte thành hai gói. Gói gọn gửi mỗi tick khi chỉ đổi vị trí; gói đầy đủ chỉ gửi khi trường chậm đổi hoặc theo chu kỳ làm mới.

**Thiết kế đề xuất:**

```c
// Headers\KProtocolDef.h  - THEM VAO DOAN 221-249, KHONG chen giua enum
s2c_syncnpcpos = 221,

// Sources\Core\Src\KProtocol.h - dat NGAY SAU NPC_NORMAL_SYNC
typedef struct
{
    BYTE   ProtocolType;   // s2c_syncnpcpos
    DWORD  ID;
    int    MapX;
    int    MapY;
    DWORD  m_fkRegionID;
    int    m_fkOffX;
    int    m_fkOffY;
    BYTE   Doing;
    BYTE   State;
} NPC_POS_SYNC;            // 27 byte
```

**Phía máy chủ**, trong `KNpc::NormalSync()`:

- Giữ bộ nhớ đệm cho mỗi NPC gồm các trường chậm: `Camp`, `m_bySeries`, `m_nProtectedTime`, ba trường máu, bốn tốc độ, ba trường mana, `MissionGroup`, `StateInfo[18]`, `NpcEnchant`.
- Băm chúng bằng FNV-1a, đúng cách `[PS-BO]` đã làm cho gói ngoại hình hôm 04/09. Mã mẫu có sẵn quanh `KNpc.cpp:6990-7010`.
- Băm đổi, hoặc quá 10 giây kể từ lần gửi đầy đủ trước, thì gửi `NPC_NORMAL_SYNC` như cũ. Ngược lại gửi `NPC_POS_SYNC`.
- Mảng đệm nên đặt cạnh `s_adwPSBam` / `s_adwPSLuc` hiện có để cùng kiểu quản lý.

**Phía client**, trong `KProtocolProcess`:

- Đăng ký `ProcessFunc[s2c_syncnpcpos] = &KProtocolProcess::SyncNpcPos;` cạnh dòng 229.
- Viết `SyncNpcPos` bằng cách sao `SyncNpcMin` (dòng 2224-2469) rồi bỏ các đoạn đụng trường chậm. Giữ nguyên toàn bộ logic nắn toạ độ, `S6_XaQuaTam`, `InsertNpcRequest`, `S10-KEO`, `S10-SNAP`, vì đó là các sửa lỗi đã tốn nhiều công.
- Rủi ro chính: NPC lạ chưa biết ID mà chỉ nhận gói gọn thì không đủ dữ liệu để dựng. Xử lý: khi `SearchID` trả 0, gói gọn phải đi đường `InsertNpcRequest` y như gói đầy đủ, hoặc đơn giản hơn là **luôn gửi gói đầy đủ cho NPC mà client chưa từng thấy**. Cách thứ hai an toàn hơn.

**Lợi ước tính:** nếu 90 % lần đồng bộ dùng gói gọn thì băng thông phần này còn `0,9 × 27 + 0,1 × 99 = 34 byte` trung bình, tức **giảm 2,9 lần**. Kèm việc 2 thì giảm tổng 6 tới 10 lần.

**Rủi ro:** đổi giao thức, phải phát hành client mới. Client cũ nhận mã 221 sẽ rơi vào `ProcessFunc` chưa gán. **Phải kiểm hàm dispatch xử lý con trỏ NULL thế nào trước khi làm** — nếu nó gọi thẳng thì client cũ sẽ sập. Cách an toàn: thêm khoá `config.ini [Server] GoiViTriGon = 0` mặc định tắt, chỉ bật sau khi client mới đã phát hành đủ.

**Công:** khoảng 2 ngày, gồm cả kiểm.

**Cách nghiệm thu:** so `[BC-LOAI]` trước và sau, mã 77 phải giảm mạnh và mã 221 tăng lên; tổng byte gửi giảm; vào game xem người khác di chuyển có mượt và có đủ máu, trạng thái, hiệu ứng không.

### 3.2 Việc 2 — Gộp nhiều đối tượng vào một gói

**Ý tưởng:** thay vì mỗi NPC một gói riêng gửi 9 lần, gom tất cả NPC được đồng bộ trong cùng một vùng ở cùng một tick thành **một gói duy nhất** có phần đầu chung và danh sách mục.

```c
s2c_syncnpcposlist = 222,

typedef struct
{
    BYTE   ProtocolType;   // s2c_syncnpcposlist
    BYTE   nCount;         // so muc, toi da 5 (bang kNpcSyncChunkSize)
    // theo sau la nCount muc NPC_POS_ENTRY (26 byte moi muc, bo ProtocolType)
} NPC_POS_LIST_HEAD;
```

**Vì sao đúng chỗ:** `KRegion::Activate` vốn đã đồng bộ **đúng 5 NPC mỗi vùng mỗi tick**. Năm gói riêng lẻ hiện thành 5 lần gọi `BroadCast`, mỗi lần lặp 9 vùng, tức **45 lượt quét**. Gộp lại chỉ còn **9 lượt quét** cho cả 5 NPC. Đây là chỗ giảm chi phí quét tốt nhất, và cũng là thứ trực tiếp hạ tải khi đông người.

**Chỗ sửa:** `KRegion::Activate` tại `KRegion.cpp:760-775`. Thay vì gọi `Npc[nNpcIdx].NormalSync()` cho từng NPC, gom vào một đệm cục bộ rồi phát một lần sau vòng lặp. Cần tách phần dựng dữ liệu ra khỏi `NormalSync` thành một hàm riêng như `KNpc::FillPosSync(NPC_POS_ENTRY*)`.

**Phía client:** một handler đọc `nCount` rồi lặp, mỗi mục xử lý y như `SyncNpcPos`.

**Cẩn thận:** gói phải nhỏ hơn giới hạn đệm mạng. Kiểm `m_nNetworkBufferMaxLen` trong `Sources\MultiServer\Heaven\ServerStage.cpp` quanh dòng 390 trước khi chọn số mục tối đa. Năm mục × 26 byte cộng 2 byte đầu là 132 byte, an toàn.

**Lợi ước tính:** giảm số lượt quét 5 lần và giảm thêm phần đầu gói lặp lại. Cộng với việc 1 thì tổng băng thông giảm khoảng 8 tới 10 lần, và chi phí CPU quét giảm khoảng 5 lần.

**Rủi ro:** cao hơn việc 1 vì đụng vòng lặp nóng nhất của máy chủ. Phải làm **sau** việc 1 và phải có cùng khoá bật tắt.

**Công:** khoảng 2 tới 3 ngày.

### 3.3 Việc 3 — Nâng trần gói chiến đấu, làm được ngay

**Đây là việc rẻ nhất và nên làm trước.** Không đổi giao thức, không cần client mới.

**Vấn đề:** ra chiêu, trúng đòn, số sát thương, tự đánh đều bị trần 100 người mỗi lượt, trong khi đồng bộ vị trí lại được 500. Khi 300 người đánh nhau ở Tống Kim, người chơi **thấy nhau đứng đó nhưng không thấy chiêu và không thấy sát thương**. Đây đúng là triệu chứng chủ báo hôm 03 và 04 tháng 9.

**Cách sửa gọn nhất:** đọc trần từ `config.ini` và tách theo nhóm, thay vì sửa 15 chỗ gọi.

```c
// Sources\Core\Src\KRegion.cpp, dau ham KRegion::BroadCast, canh khoi doc s_nBCTam
// Tran rieng cho goi CHIEN DAU: doc mot lan tu config.ini [Server] BroadCastChienDau (mac dinh 300).
// Chi NANG, khong bao gio ha, de khong lam yeu duong nao dang chay tot.
static int s_nBCChienDau = -1;
if (s_nBCChienDau < 0)
{
    s_nBCChienDau = (int)GetPrivateProfileIntA("Server", "BroadCastChienDau", 300, ".\\config.ini");
    if (s_nBCChienDau < 100)  s_nBCChienDau = 100;
    if (s_nBCChienDau > 1000) s_nBCChienDau = 1000;
}
// nBCLoaiGoi da duoc tinh o tren. Cac ma chien dau: 91 npchurt, 92 npcdeath, 95 skillcast,
// 87 npcattack, 88 npcmagic. Chi nang khi tran hien tai dang thap hon.
if (!bBCBoDuoc && nMaxCount < s_nBCChienDau &&
    (nBCLoaiGoi == 87 || nBCLoaiGoi == 88 || nBCLoaiGoi == 91 || nBCLoaiGoi == 92 || nBCLoaiGoi == 95))
{
    nMaxCount = s_nBCChienDau;
}
```

**Bẫy phải tránh:** `nMaxCount` là tham chiếu dùng chung cho cả 9 vùng. Nếu nâng bên trong hàm thì nó được nâng ở lần gọi đầu và giữ nguyên cho 8 lần sau, đó đúng là điều mong muốn. Nhưng **phải nâng trước vòng quét**, và không được nâng lại ở mỗi vùng, nếu không ngân sách thành vô hạn.

**Đánh đổi, trả lời trực tiếp câu hỏi của chủ:**

| Khoản | Ảnh hưởng khi nâng 100 lên 300 |
|---|---|
| Băng thông | Gói chiến đấu hiện gần như bằng 0 trong lúc không đánh nhau. Khi đánh nhau, phần này tăng tối đa 3 lần nhưng vẫn nhỏ hơn nhiều so với 90 % lưu lượng của gói vị trí. Ước tổng tăng dưới 10 %. |
| CPU máy chủ | Mỗi lượt phát quét thêm node, tăng tuyến tính theo trần. Chỉ nặng ở vùng đông. |
| Client | Nhận thêm gói chiến đấu. Van 1.500 gói mỗi giây **không** chặn loại này vì `BC_LaGoiViTri` chỉ trả đúng cho 75, 77, 85, 86. Đây là rủi ro thật: đám 300 người đánh nhau có thể đẩy client vượt ngưỡng. |
| Người chơi | Thấy đủ chiêu và sát thương của nhau. Đây là cái được. |

**Vì rủi ro client**, nên đặt mặc định 300 chứ không phải 500, và theo dõi `[BC-DEM]` cùng phản hồi người chơi rồi mới nâng tiếp. Khoá trong `config.ini` cho phép chỉnh mà không cần build lại.

**Công:** nửa ngày kể cả kiểm. Cần chủ duyệt vì đụng cảm nhận trong game theo Cổng 4.

**Cách nghiệm thu:** vào Tống Kim lúc đông, xem có thấy chiêu của người khác không; `[BC-LOAI]` cột `cat` của mã 91, 92, 95 phải về 0 hoặc gần 0.

### 3.4 Việc 4 — Chia luồng, phân tích đúng chỗ

Chủ nêu bốn phần: đường truyền, hình ảnh, đạn skill, sync vị trí. Phân loại theo chỗ thật sự tốn và chỗ chia luồng giúp được:

| Phần | Nằm ở đâu | Chia luồng máy chủ có giúp? | Cách đúng |
|---|---|---|---|
| Sync vị trí | Máy chủ, 90 % lưu lượng | **Không** | Việc 1 và 2 |
| Đường truyền | Mạng | **Không**, băng thông là băng thông | Việc 1 và 2 |
| Đạn skill | Cả hai phía | Một phần | Giảm gói đạn phía máy chủ; client gộp vẽ |
| Hình ảnh | **Client** | Không liên quan tới máy chủ | Chia luồng trong Game.exe |

**Về phía máy chủ**, chỉ có một khâu đáng tách và nó chưa nặng: `CIOCPServer::PackDataToClient` ở `Sources\MultiServer\Heaven\ServerStage.cpp:390`. Hàm này đã tự khoá hai lớp (`m_csCM` và `pCN->csWriteAction`) nên về lý thuyết gọi được từ luồng khác. Nhưng hiện nó chỉ chép vào đệm, chi phí thấp. **Chỉ tách khi đo được nó thành điểm nóng với người chơi thật.**

Vòng game thì **không tách được**: `Npc[]`, `Player[]`, `SubWorld[]` là mảng toàn cục không khoá. Trong `KSubWorldSet::MainLoop` (`Sources\Core\Src\KSubWorldSet.cpp`) còn nguyên ghi chú của lần thử chia luồng bị bỏ dở, nói rõ lý do. Đừng thử lại nếu chưa giải quyết xong chuyện khoá.

**Về phía client**, đã có sẵn một luồng mạng: `Sources\Engine\Src\KNetThread.cpp:79`. Vòng vẽ nằm ở luồng chính, nhịp điều khiển bằng `g_nPaintFps` khai ở `Sources\S3Client\S3Client.cpp:68`, đọc từ `config.ini [Client] PaintFps`. Muốn tách luồng vẽ thì phải làm trong Represent2 hoặc Represent3, là việc lớn và thuộc cây client. **Không nên đụng trước khi giảm được số gói**, vì giảm gói cũng giảm luôn tải vẽ.

---

## 4. THỨ TỰ LÀM VÀ ĐIỀU KIỆN

| Bước | Việc | Công | Điều kiện tiên quyết | Cần client mới |
|---|---|---|---|---|
| 1 | Việc 3, nâng trần gói chiến đấu | Nửa ngày | Chủ duyệt | Không |
| 2 | Mở cho 50 tới 100 người thật, đo `[BC-DEM]`, `[BC-LOAI]`, `jx_perf_server.log`, `jx_lua_prof.log` | 30 phút | Chủ mở máy chủ | Không |
| 3 | Chạy `tools\sapxep\do_mot_nhan.py` trên máy chủ đích, lấy hệ số quy đổi | 5 phút | Có máy chủ đích | Không |
| 4 | Việc 1, gói vị trí gọn | 2 ngày | Có số liệu bước 2 | **Có** |
| 5 | Việc 2, gộp gói | 2 tới 3 ngày | Việc 1 xong và chạy ổn | **Có** |
| 6 | Việc 4, chia luồng | Chỉ khi bước 2 chứng minh cần | | |

**Không được bỏ bước 2.** Mọi con số trong tài liệu này đo trên bot, mà bot không gửi gói lên và không nhận gói xuống. Toàn bộ chi phí mạng của người chơi thật chưa có trong bất kỳ phép đo nào.

---

## 5. LUẬT CHO PHIÊN SAU

1. **Cổng 2 vẫn áp dụng.** Việc 1 và 2 đổi giao thức. Chỉ thêm mã mới vào đoạn 221-249, tuyệt đối không chèn giữa enum, không đổi thứ tự trường của struct đang dùng.
2. **Mọi thay đổi trần hoặc gói phải có khoá trong `config.ini`** và mặc định giữ hành vi cũ. Bật lên sau khi đã kiểm.
3. **Đo trước khi sửa.** Bộ đếm `[BC-DEM]` và `[BC-LOAI]` đã có sẵn, in mỗi 10 giây. Chụp một mẫu trước và một mẫu sau, đính vào bàn giao.
4. **Đừng làm lại việc đã có.** Lọc băm ngoại hình đã có từ 04/09; bỏ trừ ngân sách cho bot đã có từ 04/09; lọc tầm hai chiều đã có từ 03/09; con trỏ xoay chia lượt đã có.
5. **Đừng tin số đo lấy lúc máy đang build.** Các đỉnh 200 tới 357 mili giây tối 06/09 trùng đúng lúc chạy MSBuild và robocopy trên cùng máy.
6. Công cụ để dùng lại: `tools\sapxep\phan_tich_tai.py` (hồi quy tải theo số người và số NPC), `tools\sapxep\truy_dinh.py` (truy nguồn gây đỉnh tick), `tools\sapxep\do_mot_nhan.py` (đo sức một nhân để quy đổi máy). Bản sao ở `ReverseTools\lua54\danhgia_0609\`.

---

## 6. PHÂN TÍCH TIẾP (06/09 23:40, phiên sau đọc lại bàn giao và đối chiếu mã)

Mọi dòng dưới đây đã đối chiếu với mã trên `origin/main` (cbceded5) và với toàn bộ nhật ký `bin\server\jx_auto_server.log` (30 MB, 105 cửa sổ `[BC-DEM]`). Chưa sửa một dòng mã nào; đây là phần đọc để phiên sau làm đúng.

### 6.1 Sáu chỗ bàn giao nói chưa đúng, phải sửa trước khi làm theo

**1. "Lần đo không có gói chiến đấu" là đọc nhầm dòng log.** `[BC-LOAI]` chỉ in **8 mã NHỎ NHẤT** có phát (`KRegion.cpp:1676`: quá 8 mục thì bỏ, trừ mã có `cat`). Hễ tám mã 75..86 có mặt là mọi mã từ 87 trở lên biến mất khỏi dòng đó. Muốn thấy chiến đấu phải đọc `[BC-TOP]` (8 loại nhiều nhất theo lượt GỬI). Bẫy này đã ghi trong bộ nhớ 04/09 và vẫn mắc lại.

**2. Gói nặng nhất khi đánh nhau không phải 77 mà là 207 `s2c_show_damage`** (số sát thương bay lên, `DAMAGESHOW` 17 byte). Cửa sổ nặng nhất trong toàn bộ nhật ký (`t=1041990127`, pid 63136, một client CaiBang giữa ~250 bot đang đánh nhau):

| Mã | Tên | Gửi trong 10 s | Byte | Cỡ gói |
|---|---|---|---|---|
| 207 | show_damage | **9.403** | 156 KB | 17 |
| 77 | syncnpcmin | 3.634 | **351 KB** | 99 |
| 95 | skillcast | 715 | 14 KB | 21 |
| 148 | castskilldirectly | 692 | 14 KB | 21 |
| 86 | npcrun | 665 | 8 KB | 13 |
| 75 | syncplayermin | 287 | 65 KB | 234 |
| 85 | npcwalk | 148 | 1 KB | 13 |
| 91 | npchurt | 92 | 1 KB | 17 |
| **Tổng** | | **15.863 (1.586/giây)** | **630 KB (61 KB/giây)** | tb 40 |

207 chiếm 59 % số gói, 77 chiếm 56 % số byte. Van 1.500 gói/giây không hề chạm (`bo_vi_tri=0`), trần 100/500 cũng không cắt ai (`cat_vi_het_ngan_sach=0` ở cả 105 cửa sổ).

**3. Mã 87 `npcattack` và 88 `npcmagic` không được phát ở đâu cả** (quét toàn bộ `Core\Src\*.cpp`: không có dòng `ProtocolType = s2c_npcattack` hay `s2c_npcmagic`). Nơi phát gói chiến đấu thật:

| Mã | Nơi phát | Hàm |
|---|---|---|
| 148 castskilldirectly | `KNpc.cpp:1385, 1444, 3605, 6125` | `ProcessState`, `CastAutoSkillAt`, `Cast` |
| 95 skillcast | `KNpc.cpp:2897` | `DoSkill` |
| 91 npchurt | `KNpc.cpp:2057` | `DoHurt` |
| 92 npcdeath | `KNpc.cpp:1817` | `DoDeath`, đã miễn ngân sách |
| 207 show_damage | `KNpc.cpp:3950` | `SyncDamageInfo` |

Đoạn mã mẫu của việc 3 (mục 3.3) kiểm 87/88/91/92/95 nên **bỏ sót 148 và 207**, tức bỏ sót chính hai mã nhiều nhất, và kiểm hai mã chết.

**4. Đoạn mã mẫu việc 3 đặt trong `KRegion::BroadCast` sẽ nâng ngân sách lại ở MỖI vùng.** `nMaxCount` là tham chiếu dùng chung cho 9 lần gọi. Sau khi vùng đầu gửi bớt, điều kiện `nMaxCount < s_nBCChienDau` lại đúng ở vùng thứ hai và được nâng lên 300 lần nữa, tối đa 9 lần. Mục 3.3 tự cảnh báo điều này rồi lại viết đúng cái lỗi đó. Cách đúng: sửa tại **nơi khởi tạo** `int nMaxCount = MAX_BROADCAST_COUNT;` của từng hàm chiến đấu (xem 6.4). Không đụng `BroadCast`.

**5. Client cũ nhận mã 221 không phải "bị bỏ qua" mà là LỆCH LUỒNG.** `KProtocolProcess::ProcessNetMsg` (`KProtocolProcess.cpp:516`) quả có kiểm `ProcessFunc == NULL` và bỏ gói. Nhưng vòng tách gói nằm TRƯỚC nó, ở `S3Client\NetConnect\NetConnectAgent.cpp:345-350`: nhảy tới gói kế bằng `GetProtocolSize(Msg)`. Bảng `g_nProtocolSize[MAX_PROTOCOL_NUM = 200]` (`KProtocol.cpp:9`, `KProtocol.h:23`) là danh sách theo vị trí, **đúng 156 mục cho mã 65..220** (đã đếm bằng script). Mã 221 rơi vào ô 156 = 0, vòng lặp coi là gói dài động và đọc 2 byte kế (phần thấp của `ID`) làm độ dài. Toàn bộ phần còn lại của lần xả đó bị hiểu sai, handler rác được gọi, cùng lớp sập 04/09. **Không được để client cũ nhận dù một gói mới.** Khoá `config.ini` chỉ là công tắc toàn cục, không đủ khi còn người chưa cập nhật. Cổng có sẵn trong mã: `Headers\KProtocolDef.h:5,8` `USE_KPROTOCOL_VERSION 1` + `KPROTOCOL_VERSION 20240430`, Bishop kiểm lúc đăng nhập (`MultiServer\Bishop\GamePlayer.cpp:794`, sai thì `LOGIN_R_INVALID_PROTOCOLVERSION`). Khi phát hành việc 1 hoặc 2 phải **tăng số này** và build lại `Bishop.exe` (và GatewayRobot nếu còn dùng) cùng client và CoreServer. Client cũ sẽ không đăng nhập được, thay vì sập giữa trận.

**6. "1,2 Mbps mỗi người, 1 Gbps gánh 800 người" là cận trên lý thuyết, không phải số đo.** Đỉnh đo được là 61 KB/giây = 0,5 Mbps (bảng trên). Gói vị trí bị chặn tự nhiên bởi thiết kế: mỗi vùng mỗi tick 5 NPC, client nghe 9 vùng, tức tối đa 5 × 9 × 18 = 810 gói 77/giây trước lọc tầm, không phải 1.500. Với 300 người thật đánh nhau, phần sự kiện (207/148/95/91) tăng theo số người trong tầm, ước 70–100 KB/giây mỗi client. 1 Gbps gánh khoảng 1.200–1.700 client như vậy, đường 100 Mbps gánh 120–170. Kết luận "băng thông là trần đầu tiên" vẫn đúng, chỉ con số đổi.

Ghi chú: `Rainbow` là `rainbow.dll`, thư viện kết nối IOCP mà GameServer nạp để nối Goddess/Bishop/Relay (`GameServer\KSOServer.cpp:81, 682`), không phải tiến trình chuyển tiếp cho người chơi. Client nối thẳng vào GameServer (`NetConnectAgent.cpp:202`). Câu "Rainbow chưa chạy vì bot không cần" trong `PHANTICH_DALUONG` mục 1 và 7.4 là đọc nhầm; lưu lượng người chơi đi thẳng vào card mạng của GameServer.

### 6.2 Đường đi thật của một byte tới client (đã đọc mã)

- `PackDataToClient` (`Heaven\ServerStage.cpp:390`) chỉ **chép vào đệm riêng của kết nối**, mỗi lần khoá `m_csCM` toàn cục. Đệm 10 KB (`ServerStage.h:86` `bufferSize = 1024*10`, trừ 32).
- Xả thật ở `GameServer\KSOServer.cpp:3316`: `if (m_nGameLoop & 0x01) SendPackToClient(-1)`, tức **mỗi 2 tick một lần = 9 lần/giây**. Mỗi lần xả thêm một `WORD` độ dài rồi mã hoá `KSG_EncodeBuf` (XOR, không nén). Mọi gói máy chủ → client nằm chờ 0–111 ms (trung bình ~55 ms) trong đệm. Đầy 10 KB thì gửi ngay (`_SendDataEx`, có header riêng), không mất gì.
- Không có nén ở tầng nào. Chi phí TCP/IP khoảng 3 % ở 61 KB/giây. Con số 99 byte cho gói 77 trên đường truyền là đúng.
- Ý nghĩa cho "sync vị trí, đạn skill mượt": xả mỗi tick (bỏ `& 0x01`) cắt trung bình ~28 ms trễ cho mọi thứ, đổi lại gấp đôi số `WSASend` (18/giây/client). Một dòng, nhưng phải đo `[PERF]` trước sau. Không thuộc băng thông.

### 6.3 Gói 207: nên hạn dòng, không nâng trần

`SyncDamageInfo` (`KNpc.cpp:3946`) ép `bBroadCast = true` ở dòng 3957 bất kể tham số, phát tới 100 người trong 9 vùng, rồi **gửi riêng** cho người đánh (3995) và cho nạn nhân (4020-4022). Bản phát tán vì thế chỉ phục vụ **người đứng xem**; người đánh và nạn nhân nhận hai bản (client `SetBlood2` `KNpc.cpp:12074` có 5 khe, số trùng hiện hai lần chồng nhau). Ở 300 người đánh nhau, nâng trần 207 lên 300 nghĩa là mỗi client nhận vài nghìn gói 17 byte mỗi giây chỉ để xem số của người khác.

Đề nghị cho 207:
- **Không** đưa 207 vào nhóm được nâng trần; giữ 100.
- Thêm 207 vào lớp "bỏ được" trong `BC_LaGoiViTri` (`KRegion.cpp:34`) với hạn mức riêng mỗi client, ví dụ `[Server] BroadCastSatThuongToiDa` = 300/giây. Người đánh và nạn nhân đã có bản gửi riêng nên không ai mất số của mình.
- Tuỳ chọn thêm: thu tầm phát 207 cho người xem xuống 16 ô (nửa màn hình).

### 6.4 Việc 3 viết lại cho đúng

- Nhóm nâng: **148, 95, 91** (chiêu, chiêu tự động, trúng đòn). 92 đã miễn ngân sách. 207 xử lý riêng theo 6.3.
- Chỗ sửa: sáu dòng `int nMaxCount = MAX_BROADCAST_COUNT;` tại `KNpc.cpp:1398, 1458, 2936, 3612, 6139` (148/95) và `KNpc.cpp:2075` (91) thành `int nMaxCount = BC_TranChienDau();`. Hàm đặt trong `KRegion.cpp` cạnh `g_nBCHanMuc`, đọc `[Server] BroadCastChienDau` một lần (mặc định 300, kẹp 100..1000), chỉ phía máy chủ.
- Trần này chỉ chạm khi có hơn 100 **người thật** trong tầm 32 ô (bot không ăn suất từ F4). Hôm nay chưa có tình huống đó; đây là chuẩn bị cho ngày đông, không phải sửa triệu chứng hiện tại.
- Nghiệm thu: `[BC-TOP]` (không phải `[BC-LOAI]`) và `[BC-NGUOI]` khi có từ 100 người thật một chỗ.

### 6.5 Việc 1 (gói gọn): ba điều phải thêm vào thiết kế

1. **Máu và nội lực hiện tại đổi liên tục**, cả khi đứng yên chưa đầy máu (hồi máu `KNpc.cpp:1325-1327`) lẫn mỗi đòn khi đánh nhau. Nếu băm chúng vào nhóm "chậm" thì trong trận gần như lần nào cũng phải gửi gói đầy đủ; việc 1 mất tác dụng đúng lúc cần. Gói gọn phải mang `m_CurrentLife` (và `m_CurrentMana` nếu muốn giữ thanh nội lực của người khác): 27 → 31/35 byte. Băm chỉ gồm `Camp, m_bySeries, m_nProtectedTime, LifeMax ×2, 4 tốc độ, ManaMax ×2, MissionGroup, StateInfo[18], NpcEnchant`.
2. **NPC lạ không cần gói đầy đủ.** Đường phát hiện của client (`KProtocolProcess.cpp:2232-2250`) chỉ dùng `ID, MapX, MapY, State & STATE_HIDE` rồi `InsertNpcRequest` → `c2s_requestnpc` → `KNpcSet::SyncNpc` → `KNpc::SendSyncData` gửi 76 `NPC_SYNC` đầy đủ. Không có đường nào khác: đổi vùng (`KSubWorld::PlayerChangeRegion`) không đẩy NPC nào cho client, `KSubWorld::SendSyncData` chỉ gửi `WORLD_SYNC`. Nhánh gắn lại NPC mồ côi (2255-2300) dùng `m_fkRegionID, m_fkOffX/Y, Doing`, đều có trong gói gọn. Vậy `SyncNpcPos` = `SyncNpcMin` bỏ khối gán trường ở cuối; không cần "luôn gửi đầy đủ cho NPC chưa thấy".
3. **Lợi thật nhỏ hơn bàn giao nói.** Trong cửa sổ đỉnh, 77 là 56 % byte; gọn 31 byte cho 90 % lần gửi làm phần 77 giảm ~2,6 lần, **tổng byte mỗi client giảm ~35 %** khi đánh nhau (nhiều hơn ở thành). Con số "6 tới 10 lần" của bàn giao cộng cả việc 2, mà việc 2 chỉ tiết kiệm 1 byte `ProtocolType` mỗi mục (header `WORD` là của cả lần xả, không phải của từng gói). **Việc 2 là việc giảm CPU quét, gần như không giảm băng thông.**

### 6.6 Việc 2 (gộp gói): chi tiết bàn giao chưa nói

- Lọc tầm hiện tính theo vị trí **từng NPC** (`nOX, nOY` truyền vào `BroadCast`). Gộp 5 NPC cách nhau tới 16×32 ô thì phải chọn: lọc "có ít nhất một mục trong tầm" (gửi dư vài mục, client tự bỏ mục ≥ 40 ô qua `S6_XaQuaTam`), hoặc dựng gói riêng cho từng người nhận (đắt). Chọn cách một.
- `BC_LaGoiViTri` phải thêm mã 222; không thì gói gộp không bao giờ bị van bỏ và bị đếm như gói chiến đấu.
- Van `g_anBCGiay` đếm **gói**; gói gộp 5 mục phải cộng 5, không thì hạn mức thực tế tăng 5 lần.
- So sánh trước sau bằng `[BC-TOP]` phải quy ra **mục**, không phải gói.
- Chi phí quét giảm đúng như bàn giao: 45 lượt `BroadCast` mỗi vùng mỗi tick còn 9. Nhưng hôm nay `node_duyet/goi` chỉ 1,08 (bot rải) tới 3 (cửa sổ đỉnh); quét đang rẻ. Việc 2 chỉ đáng khi đo được `SW_ACTIVATE` tăng vì phát tán.

### 6.7 Việc 0 (mới): bỏ gửi lại gói 77 không đổi, chỉ máy chủ, không cần client

Cùng thủ thuật `[PS-BO]` của 04/09 nhưng cho gói 77: băm toàn bộ `NPC_NORMAL_SYNC`; giống lần trước và chưa tới kỳ làm mới thì không phát. Trong lần đo bot rải (mục 1), 77 là 90 % lượt phát và phần lớn NPC đứng yên đầy máu, gói y hệt nhau mỗi lần. Trong Tống Kim ai cũng di chuyển nên gần như không lợi; lợi ở thành, bãi luyện, chợ.

Điều kiện an toàn (đã đọc mã client):
- Kỳ làm mới ≤ 2 giây: client gắn lại NPC mồ côi chỉ khi `m_SyncSignal` ≤ 36 tick (`KNpcSet.cpp:799`), dọn NPC im lặng quá 1.000 tick (`:828`; nhánh 5 giây ở `:848` đã bị chú thích).
- **Người mới bước vào tầm chỉ biết NPC qua gói 77** (6.5 điểm 2), nên phải phát bất kể băm khi có người vừa vào vùng: ghi mốc thời gian trong `KRegion::AddPlayer` (`KRegion.cpp:1244`); trong `NormalSync` nếu vùng hiện tại hoặc 8 vùng kề có mốc dưới 2 giây thì gửi. Thiếu gác này, NPC đứng yên hiện muộn tới 2 giây khi đi vào thành.
- `PS_XoaDauVet` đã móc vào `SendSyncData` (`KNpc.cpp:6592`) cho người chơi; mở rộng cho mọi NPC.
- Công nửa ngày, chỉ `CoreServer.dll`. Nghiệm thu: byte của 77 trong `[BC-TOP]` ở thành giảm mạnh; client không tăng `[S6-BAL]`/`[S6-XOAXA]`.

### 6.8 Nhịp đồng bộ trong vùng đông (cho "sync vị trí")

`KRegion::Activate` (`KRegion.cpp:712`, chunk 5 ở `:718`) đồng bộ 5 NPC mỗi vùng mỗi tick theo con trỏ xoay, nên với N NPC trong vùng mỗi NPC được sync lại mỗi N/5 tick: 100 NPC → 1,1 s; 300 → 3,3 s; 500 → 5,6 s. Di chuyển thật đi bằng lệnh 85/86 (mỗi `DoRun` một lần, trần 100). Ai lỡ lệnh chạy (ngoài trần) thấy người kia đứng im tới 3,3 s rồi nhảy (`[S10-KEO]`). Đó là gốc "đứng đó mà không thấy chạy" trong đám đông, độc lập với băng thông. Cách chữa rẻ: chunk thích nghi `max(5, N/18)` để mỗi NPC được sync không quá 1 giây một lần; đổi lại số gói 77 tăng theo N, nên phải đi kèm việc 0 hoặc việc 1.

Ghi chú nhỏ đã kiểm: vòng chọn 5 chỉ số ở `KRegion.cpp:737` tính `(cursor + k) % N` trước khi duyệt; khi quấn vòng, các chỉ số quấn (0, 1, 2...) đã bị duyệt qua nên bị bỏ tick đó. Vùng 3 NPC chỉ sync 3, 1, 2 lượt luân phiên (2/3). Vô hại ở 18 Hz; ghi để không tưởng là lỗi khác.

### 6.9 Thứ tự làm, cập nhật

| Bước | Việc | Công | Cần client mới | Điều kiện |
|---|---|---|---|---|
| 1 | Việc 3 viết lại (6.4): 148/95/91 lên `BroadCastChienDau`, 207 giữ 100 | nửa ngày | Không | Chủ duyệt (Cổng 4) |
| 2 | 207 vào lớp bỏ được với hạn mức riêng (6.3) | nửa ngày | Không | Cùng bước 1 |
| 3 | Sửa `[BC-LOAI]` in 8 loại NHIỀU NHẤT thay vì 8 mã nhỏ nhất, hoặc chỉ đọc `[BC-TOP]` | 15 phút | Không | |
| 4 | Việc 0 (6.7) | nửa ngày | Không | Có gác người mới vào vùng |
| 5 | Mở 50–100 người thật, đọc `[BC-TOP]`, `[BC-NGUOI]`, `jx_perf_server.log` | 30 phút | Không | Chủ mở |
| 6 | Việc 1 (6.5) + tăng `KPROTOCOL_VERSION` + build Bishop | 2 ngày | **Có** | Sau bước 5 |
| 7 | Việc 2 (6.6) | 2–3 ngày | **Có** | Khi `SW_ACTIVATE` tăng vì phát tán |
| 8 | Xả socket mỗi tick (6.2) | 1 dòng | Không | Đo `[PERF]` trước sau |

### 6.10 Luật bổ sung cho phiên sau

1. Đọc số chiến đấu ở `[BC-TOP]`, không ở `[BC-LOAI]`.
2. Mã chiến đấu thật là 148, 95, 91, 92, 207. 87 và 88 là mã chết.
3. Không bao giờ nâng `nMaxCount` bên trong `KRegion::BroadCast`.
4. Đổi giao thức thì tăng `KPROTOCOL_VERSION` và build lại Bishop; không dựa vào kiểm `ProcessFunc == NULL`.
5. Gói mới phải khai đúng vị trí trong `g_nProtocolSize` (`KProtocol.cpp`), và thêm vào `BC_LaGoiViTri` nếu là gói lặp.

---

## 7. PHÂN TÍCH SÂU HƠN VÀ PHƯƠNG ÁN TỐT NHẤT (07/09 rạng sáng)

Chủ yêu cầu *"phân tích sâu hơn và tìm phương án tốt nhất"*. Phần này thay cho thứ tự ở mục 6.9. Ba việc mới đã làm để có căn cứ: mổ nhị phân **bản Linux tham chiếu** xem nó phát tán thế nào; dựng **mô hình trận 300 người thật** từ số đo; đọc tầng socket hai đầu (Nagle, đệm, xả) và giới hạn phía client.

### 7.1 Bản Linux tham chiếu làm gì (mổ `D:\ServerLinux\server1\jx_linux_y`)

Công cụ: `ReverseTools\linuxbc\{dis4,callees,callers,argscan,fn,raw}.py` (dùng `elfre.py` của phiên quái/đạn 06/09). Bảng địa chỉ đã chốt: bảng lệch 8 vùng kề `POff` chỉ có MỘT bản ở `0x08258040`, được 4 hàm gom người nhận dùng (`0x080E0FC0`, `0x080E1920`, `0x080E1AD0`, `0x080E1C80`); danh sách kết nối toàn cục `0x082E9380`; `g_pServer` `0x082E8CA0`; `NormalSync` `0x080810C0`; `KRegion::Activate` `0x080E2660`; bọc gửi gần vùng `0x0807A870` (46 nơi gọi). Bản dump: `ReverseTools\linuxbc\normalsync_linux.txt`, `region_activate_linux.txt`.

| Hạng mục | Linux `jx_linux_y` | JX1 hiện nay |
|---|---|---|
| Gói đồng bộ NPC định kỳ (mã 77 = 0x4D, cùng số hiệu) | **27 byte**: ID, MPS X/Y, `Doing` + phần trăm máu nén chung một byte, cờ trạng thái 4 bit, phe, hệ, vài byte nữa. Không có offset, không có mã vùng, không tốc độ, không nội lực, không `StateInfo[18]` | **99 byte** |
| Gói ngoại hình người chơi (mã 75 = 0x4B) | **39 byte**, toàn số hiệu (mũ, giáp, vũ khí, ngựa, cấp bậc...) | **234 byte** (GameTitle 64 + TongName 32 + TongTitle 32 + MateName 32 + ...), đã băm bỏ 98 % |
| Nhịp đồng bộ mỗi vùng (`KRegion::Activate`) | **1 NPC mỗi 2 tick**: `2*i == m_nNpcSyncCounter`, đếm mỗi tick, quấn ở 2N → mỗi NPC được sync lại mỗi **2N+1 tick** | **5 NPC mỗi tick** → mỗi NPC mỗi **N/5 tick** |
| Trần người nhận gói đồng bộ | 1.200 (`0x4B0`, coi như không trần) | 500 |
| Trần sự kiện một lần (chạy, chiêu, trúng đòn, đổi phe...) | **100** | 100 |
| Lọc tầm | hộp 32 ô nhưng **một phía** (đúng lỗi JX1 đã sửa 03/09) trong 3×3 vùng | hộp 32 ô hai phía |
| Cách gửi | **gom danh sách kết nối** của 9 vùng vào một mảng rồi MỘT lệnh `PackDataToClients(list, n, buf, size)` (vtable +0x2c) | 9 lần `BroadCast`, mỗi người nhận một `PackDataToClient` + khoá `m_csCM` |
| Chat gần | hình tròn `dx²+dy² ≤ 528` (bán kính ~23 ô), trần 1.200 | không đổi |
| NPC AiMode 8 | không phát tán gì | |

Quy ra một vùng 50 NPC, tính cho một người nhận: Linux gửi 9 gói/giây × 27 byte = **0,24 KB/giây**; JX1 gửi 90 gói/giây × 99 byte = **8,9 KB/giây**, tức **37 lần**. Nhịp mỗi NPC: Linux 5,6 s (50 NPC) / 33 s (300 NPC); JX1 0,55 s / 3,3 s. Bản Linux để việc "thấy người ta chạy" cho lệnh 85/86 (13 byte, phát ngay lúc ra lệnh) và chỉ dùng 77 làm nắn chậm.

Ghi chú lịch sử: chú thích `[24/08]` ở `KRegion.cpp:750` cho biết cơ chế `m_nNpcSyncCounter` gốc của JX1 **chưa bao giờ chạy** (không nơi nào tăng biến), và vòng chunk 5 được thêm sau đó. Nghĩa là nhịp 5 NPC/tick không phải thiết kế gốc; bản Linux có cùng biến và **có tăng** nó mỗi tick.

**Kết luận từ tham chiếu:** gốc băng thông của JX1 nằm ở hai chỗ nhân với nhau: gói 77 to gấp 3,7 lần và nhịp gửi gấp 10 lần. Bản tham chiếu đã chạy hàng nghìn người với gói nhỏ + nhịp thưa + không trần cho gói đồng bộ, trần 100 cho sự kiện.

### 7.2 Những ràng buộc khác đã đọc trong phiên này

- **Client chứa được 800 NPC** (`KNpc.h:23`, nâng từ 256 hôm 03/09) và 128 khe hỏi NPC. Không còn là trần cho 300 người.
- **Màn hình 800×600 mặc định, ô 32×32 px** (`S3Client.cpp:91`, `KSubWorld.cpp:44`): người chơi thấy ±12 ô ngang, ±9 ô dọc. Hộp 32 ô máy chủ đang gửi (65×65 = 4.225 ô) rộng gấp **8 lần** vùng nhìn thấy (25×19 = 475 ô); phần dư là tiền nạp để NPC không "hiện đột ngột" ở mép, nhưng không cần cùng nhịp với phần trên màn hình.
- **Không đầu nào đặt `TCP_NODELAY`** (quét `MultiServer\Common`, `Heaven`, `S3Client\NetConnect`): Nagle bật cả hai chiều. Lệnh client → máy chủ nhỏ (13-21 byte) ra dồn dập bị giữ tới khi có ACK; máy chủ chỉ xả 9 lần/giây nên ACK về chậm, lệnh thứ hai chờ tới ~111 ms. Đây là một gốc "bấm chiêu chậm" độc lập với băng thông.
- **Client dừng nhận không làm vỡ đệm client** (`KSocketClient2.cpp:278`: chỉ `recv` khi được gọi; dữ liệu dồn ở đệm hệ điều hành rồi ép ngược máy chủ). Phía máy chủ không có trần số ghi treo (`m_outstandingWrites` chỉ đếm), một client đứng hình tích ~100 KB/giây trong `CIOBuffer` tới khi ping timeout 60 s (`KSOServer.cpp:3288`) cắt kết nối. Chấp nhận được, nhưng là chỗ đặt "van theo tình trạng socket" sau này.
- `SearchID` phía client duyệt tuyến tính (`KNpcSet.cpp:221`), 800 khe × 3.000 gói/giây vẫn dưới 1 ms mỗi khung. Không phải trần.

### 7.3 Mô hình trận 300 người thật (mỗi client), suy từ số đo cửa sổ đỉnh

Giả định: 300 người đánh nhau trong 3×3 vùng, mỗi người 1 chiêu/giây, chịu 2 đòn/giây, 1 lệnh chạy/giây; gói 77 chạm trần thiết kế 5 × 9 vùng × 18 = 810/giây; cỡ gói lấy từ mã (77: 99, 75: 234, 86: 13, 148/95: 21, 91: 17, 207: 17).

| Loại | Hiện nay | Giai đoạn A (mục 7.5) | Giai đoạn B (mục 7.6) |
|---|---|---|---|
| 77 đồng bộ | 810/s × 99 = 80 KB/s | chunk 3: 486/s × 99 = 48 KB/s | 486/s × 25 = 12 KB/s |
| 75 ngoại hình (làm mới 5 s → 30 s) | 60/s × 234 = 14 KB/s | 10/s × 234 = 2,3 KB/s | 2,3 KB/s |
| 85/86 lệnh chạy | tới 1/3 người: 100/s = 1,3 KB/s | 300/s = 3,9 KB/s | 3,9 KB/s |
| 148/95 chiêu | tới 1/3: 100/s = 2,1 KB/s | 300/s = 6,3 KB/s | 6,3 KB/s |
| 91 trúng đòn | tới 1/3: 200/s = 3,4 KB/s | 600/s = 10,2 KB/s | 10,2 KB/s |
| 207 số sát thương | tới 1/3: 300/s = 5,1 KB/s | hạn mức 300/s = 5,1 KB/s | 5,1 KB/s |
| **Tổng mỗi client** | **~106 KB/s, ~1.570 gói/s, thấy 1/3 chiêu** | **~76 KB/s, ~2.000 gói/s, thấy đủ** | **~40 KB/s, ~2.000 gói/s, thấy đủ** |
| Máy chủ phát cho 300 client | 32 MB/s = 255 Mbps | 23 MB/s = 180 Mbps | 12 MB/s = 96 Mbps |

Đọc bảng: hôm nay người chơi **không thấy 2/3 chiêu và lệnh chạy** nhưng lại tốn nhiều băng thông nhất, vì 77 và 75 chiếm 89 % byte. Giai đoạn A sửa cái nhìn thấy và cắt 30 % byte mà không cần client. Giai đoạn B cắt thêm một nửa. Đường 1 Gbps chịu được cả ba cột; **đường 100 Mbps chỉ chịu được cột B** với 300 người một chỗ.

### 7.4 So sánh các hướng

| # | Hướng | Byte/client | Nhìn thấy | CPU máy chủ | Client mới | Rủi ro | Công | Kết luận |
|---|---|---|---|---|---|---|---|---|
| P1 | Trần sự kiện một lần theo `config.ini`, áp cho mọi nơi dùng `MAX_BROADCAST_COUNT` | +20 % khi đông | **1/3 → đủ** | +, tuyến tính theo trần | Không | Thấp (mặc định 100 = cũ) | nửa ngày | **Làm** |
| P2 | 207 vào lớp bỏ được, hạn mức riêng mỗi client | −, chặn lũ 207 | giữ | 0 | Không | Thấp (người đánh/nạn nhân có bản riêng) | nửa ngày | **Làm** |
| P3 | Băm bỏ 77 trùng (việc 0) + gác người mới vào vùng | −50..90 % ở thành, 0 % khi đánh | giữ | +băm 99 B/lần | Không | Thấp | nửa ngày | **Làm** |
| P4 | Nhịp chunk theo `config.ini` (mặc định 5, thử 3) | −40 % phần 77 | giữ nếu có P1 | − | Không | Trung bình: NPC lỡ lệnh chạy đứng lâu hơn | 1 giờ | **Làm, bật sau P1** |
| P5 | Xả socket mỗi tick + `TCP_NODELAY` hai đầu | 0 | trễ −28..110 ms | +9 WSASend/s/client | Nagle client: cần build Game.exe (không đổi giao thức) | Thấp | nửa ngày | **Làm** |
| P0 | `BroadCastLamMoi` 5 → 30 s + bỏ 75 khỏi lớp bỏ được | −12 KB/s khi 300 người | giữ | 0 | Không | Rất thấp | 0 (chỉ config) | **Làm ngay** |
| P6 | Gói 77 gọn ~25 byte như Linux (việc 1) | **−35..60 %** | giữ | − | **Có** + tăng `KPROTOCOL_VERSION` | Trung bình, có cổng phiên bản | 2-3 ngày | **Giai đoạn B** |
| P7 | Gói 75 gọn 40 byte, tên/danh hiệu theo yêu cầu | −2 KB/s sau P0 | giữ | 0 | Có | Đổi struct đang dùng (Cổng 2) | 2 ngày | **Bỏ**, P0 đã đủ |
| P8 | Gộp 5 NPC một gói (việc 2) | −1 byte/mục | giữ | −80 % quét sync | Có | Cao (vòng nóng) | 2-3 ngày | **Bỏ**: Linux cũng không làm, CPU chưa nghẽn |
| P9 | Chia luồng GameServer | 0 | 0 | ? | Không | Rất cao | tuần | **Bỏ** cho tới khi đo thấy CPU |
| P10 | Gom danh sách + `PackDataToClients` một lệnh như Linux | 0 | 0 | −khoá `m_csCM` theo người nhận | Không | Trung bình (đổi vtable `IServer` → build heaven.dll) | 1-2 ngày | Sau, khi có 500+ người thật |
| P11 | Van theo tình trạng socket (`m_outstandingWrites`) thay van cố định | bảo vệ client chậm | giữ | 0 | Không | Thấp | 1 ngày | Sau bước đo |

### 7.5 PHƯƠNG ÁN TỐT NHẤT: giai đoạn A, chỉ máy chủ, ~2 ngày, mọi thứ có khoá và mặc định giữ hành vi cũ

Thứ tự thi công và nghiệm thu từng bước; **mỗi bước một commit, đặt `.moi` rồi chờ chủ swap** (luật `feedback-day-git-moi-lan-fix`).

**A0. Chỉ sửa config, không build**: `bin\server\config.ini` thêm `[Server] BroadCastLamMoi=30`. Kèm A2 thì bỏ 75 khỏi `BC_LaGoiViTri` để ngoại hình không bao giờ bị van bỏ (30 s nhìn nhầm quần áo là quá lâu). Nghiệm thu: `[PS-BO]` tỉ lệ bỏ ≥ 99 %.

**A1. Trần sự kiện một lần theo config.** `KRegion.h` trong `#ifdef _SERVER`: `#define MAX_BROADCAST_COUNT BC_TranMotLan()`; hàm trong `KRegion.cpp` cạnh `g_nBCHanMuc`, đọc `[Server] BroadCastMotLan` một lần, mặc định 100, kẹp 100..1500. Mọi nơi (20 chỗ `KNpc.cpp`, 4 chỗ `KObj.cpp`, 1 chỗ `KObjSet.cpp`, mặc định của `SendDataToNearRegion`) tự theo. **Ngoại lệ bắt buộc:** `SyncDamageInfo` (`KNpc.cpp:3960`) đổi thành số 100 cố định hoặc `BC_TranSatThuong()`, vì 207 xử lý ở A2. `NPC_SYNC_BROADCAST_LIMIT` 500 giữ nguyên. Chủ đặt `BroadCastMotLan=300` khi mở đông. Nghiệm thu: `[BC-TOP]` cột gửi của 148/95/91/86 tăng theo số người, không có `cat`.

**A2. 207 có hạn mức riêng.** `KRegion.cpp`: `BC_LaGoiSatThuong(n) → n == 207`; mảng `g_anBCGiay207[MAX_PLAYER]` reset mỗi giây như `g_anBCGiay`; khoá `[Server] BroadCastSatThuongToiDa` mặc định 300, kẹp 50..5000; vượt thì bỏ và đếm `bo_sat_thuong=` trong `[BC-DEM]`. Người đánh và nạn nhân vẫn nhận qua hai lệnh gửi riêng ở `KNpc.cpp:3995` và `4020-4022`. Nghiệm thu: `[BC-NGUOI]` tổng gói/giây của client đông không vượt ~2.500.

**A3. Băm bỏ 77 trùng.** Trong `KNpc::NormalSync` sau khi dựng `NpcSync`: `s_adwNSBam[MAX_NPC]`, `s_adwNSLuc[MAX_NPC]` (mỗi mảng 392 KB, `#ifdef _SERVER`), băm FNV-1a như `PS_Bam`; giống lần trước và chưa tới `[Server] DongBoLamMoi` (mặc định 2 s, kẹp 1..5) **và** không có người mới vào vùng thì `return` trước khi phát. Gác người mới: `KRegion` thêm `DWORD m_dwLucCoNguoiVao`, ghi `GetTickCount()` trong `KRegion::AddPlayer` (`KRegion.cpp:1244`); `NormalSync` kiểm `CURREGION` + 8 vùng kề, mốc dưới 2 s thì phát bất kể băm. `PS_XoaDauVet` trong `SendSyncData` (`KNpc.cpp:6592`) mở rộng thành xoá cả hai dấu vết cho mọi NPC. Log `[NS-BO]` 10 s như `[PS-BO]`. Nghiệm thu: đứng trong thành, `[BC-TOP]` byte 77 giảm ≥ 70 %; đi từ ngoài vào thành NPC hiện ngay; client không tăng `[S6-BAL]`, `[S6-XOAXA]`, `[S10-KEO]`.

**A3b (tuỳ chọn, cùng commit).** Trong `KRegion::Activate` đếm ngân sách chunk theo **lần phát thật** (`NormalSync` trả về có phát hay không): NPC đứng yên không tốn suất, NPC di chuyển được sync dày hơn với cùng số gói.

**A4. Nhịp chunk theo config.** `kNpcSyncChunkSize` thành `static int` đọc `[Server] DongBoMoiTick` (mặc định 5, kẹp 1..20), mảng `aSyncIdx[20]`. Chỉ đổi giá trị sau khi A1 đã bật 300 (để lệnh chạy tới đủ người). Nghiệm thu khi thử 3: `[S10-KEO]`/`[S10-SNAP]` mỗi phút không tăng quá 2 lần so với trước.

**A5. Trễ xả và Nagle.** (a) `GameServer\KSOServer.cpp:3316` bỏ `& 0x01` để xả mỗi tick (đây là `GameServer.exe`, không phải `CoreServer.dll`; `ChayGameServer.bat` hiện chỉ thay `CoreServer.dll` và `heaven.dll`, phải thêm `GameServer.exe.moi` hoặc chủ chép tay). (b) `MultiServer\Common\SocketServer.cpp`: sau `accept`, `setsockopt(TCP_NODELAY, 1)` → build lại `heaven.dll` như 04/09. (c) `KSocketClient2::ConnectTo`: cùng `setsockopt` → build `Game.exe`, không đổi giao thức nên phát dần được. Nghiệm thu: đo thời gian bấm chiêu → thấy chiêu bằng `jx_paint`/log client trước sau; `[PERF]` MainLoop không đổi.

**A6. Sửa `[BC-LOAI]`** in 8 loại nhiều nhất theo `goi` (hoặc bỏ dòng này, chỉ giữ `[BC-TOP]`).

**Kết quả kỳ vọng sau A** (cột giữa bảng 7.3): thấy đủ chiêu và lệnh chạy ở 300 người, byte mỗi client −30 %, trễ −28 ms, và ở thành/bãi luyện gói 77 giảm 70-90 %. Không đổi một byte nào trên đường truyền, client cũ chạy bình thường.

### 7.6 Giai đoạn B: gói 77 gọn theo bản Linux (khi cần 100 Mbps hoặc trên 400 người một chỗ)

Thiết kế gói (mã 221, đoạn trống 221-249, khai đúng vị trí trong `g_nProtocolSize` `KProtocol.cpp` ngay sau `s2c_scriptdata`):

```c
typedef struct { BYTE ProtocolType; DWORD ID; int MapX; int MapY;
                 BYTE Doing; BYTE State; BYTE Camp; BYTE m_bySeries;
                 int m_CurrentLife; int m_CurrentLifeMax; } NPC_POS_SYNC;   // 25 byte
```

- Bỏ `m_fkRegionID`, `m_fkOffX/Y`: client tính từ MPS bằng `Mps2Map`, đúng hai dòng đang bị chú thích `//nOffX` và `//SubWorld[0].m_Region[nRegion].m_RegionID` (`KProtocolProcess.cpp:2295, 2298, 2328, 2334`). Bản Linux làm vậy.
- Mang máu hiện tại vì nó đổi liên tục (mục 6.5). Băm nhóm chậm gồm phần còn lại của `NPC_NORMAL_SYNC`; băm đổi hoặc quá 10 s thì gửi gói 77 đầy đủ như cũ.
- Client: `SyncNpcPos` = `SyncNpcMin` bỏ khối gán trường cuối; mọi nhánh S6/S10 giữ nguyên.
- **Cổng phiên bản:** `Headers\KProtocolDef.h:8` `KPROTOCOL_VERSION 20240430 → 20260907`; build `bin\multiserver\Bishop.exe` (`MultiServer\Bishop`, kiểm `GamePlayer.cpp:794`), `Game.exe` + `CoreClient.dll`, `CoreServer.dll`. Client cũ bị từ chối lúc đăng nhập, chạy `ChoiGame.bat` là cập nhật. Khoá `[Server] GoiViTriGon` mặc định 0, bật sau khi Bishop mới chạy.
- Kỳ vọng: 77 từ 99 còn ~30 byte trung bình, tổng mỗi client ~40 KB/giây (cột phải bảng 7.3).

### 7.7 Rủi ro và cách lùi

| Rủi ro | Dấu hiệu | Lùi |
|---|---|---|
| A1 làm client ngập khi 300 người | `[BC-NGUOI]` > 3.000 gói/s, client tụt fps | `BroadCastMotLan=100` trong config, không cần build |
| A3 làm NPC hiện muộn | người chơi báo "vào thành trống", `[NS-BO]` cao ở vùng có người mới | `DongBoLamMoi=1` hoặc tắt băm bằng `DongBoLamMoi=0` (thêm nhánh 0 = tắt) |
| A4 làm người lỡ lệnh chạy đứng lâu | `[S10-KEO]` tăng | `DongBoMoiTick=5` |
| A5 xả mỗi tick tăng CPU | `[PERF]` MainLoop tăng > 1 ms | giữ `& 0x01` |
| B gửi gói mới cho client cũ | sập client như 04/09 | không thể xảy ra khi Bishop đã tăng phiên bản; vẫn giữ khoá `GoiViTriGon` để tắt |

### 7.8 Việc phiên sau làm ngay, theo thứ tự

1. A0 (config) và A6 (log) trước, để mọi số đo sau đó đọc được đúng.
2. A1 + A2 một nhánh, một commit, build `CoreServer.dll` từ `origin/main`, đặt `.moi`, chờ chủ. Xin chủ đặt `BroadCastMotLan=300` khi có ≥ 100 người thật.
3. A3 (+A3b) nhánh riêng, đo `[NS-BO]` ở thành và ở Tống Kim.
4. A4 và A5 sau khi A1-A3 chạy ổn một buổi có người thật.
5. Đo với 50-100 người thật (`[BC-TOP]`, `[BC-NGUOI]`, `[PERF]`, `jx_paint`), rồi mới quyết giai đoạn B theo đường truyền của máy chủ đích: 1 Gbps → B không vội; 100 Mbps → B bắt buộc trước khi mở 300 người.
6. Không làm P7, P8, P9 khi chưa có số đo cho thấy CPU hoặc quét vùng nghẽn.

---

## 8. THI CÔNG (07/09 00:20–01:10): chủ chốt "bản test, không giới hạn trải nghiệm" → làm trọn gói, chờ swap

Chủ 07/09: *"phân tích chọn phương án tốt nhất mà không giới hạn mất trải nghiệm của game... bản này là bản test nên chỉ có tôi test... cứ làm thoải mái không giới hạn phương án hay phụ thuộc vào người chơi"*. Vì vậy bỏ mọi ý "hạn dòng/cắt bớt" ở mục 7; giữ lại đúng những gì **giảm byte và trễ mà không bớt thứ người chơi thấy**, và đổi giao thức luôn (chỉ một client).

Nhánh `delta-0709` (worktree `D:\GAMEDEVNEW_wt_delta`), gốc `origin/main` 68d7591b (đã gồm MATDO + AI710L), commit **1d4e70c5** + **ff6c229e**; bộ vá áp lại được: `ReverseTools\goi_va_delta_0709.py`, `ReverseTools\goi_va_delta2_hello_0709.py`.

### 8.1 Những gì đã đổi (mã)

| Chỗ | Đổi | Khoá `config.ini [Server]` (mặc định = giá trị mới) |
|---|---|---|
| `KRegion.h/.cpp` | `MAX_BROADCAST_COUNT` → `BC_TranMotLan()`, `NPC_SYNC_BROADCAST_LIMIT` → `BC_TranDongBo()` | `BroadCastMotLan=100000`, `BroadCastDongBo=100000` (= không cắt ai; về cũ: 100 / 500) |
| `KRegion.cpp` | van vị trí mỗi client 1.500 → 5.000; `BC_LaGoiViTri` bỏ 75 (không bao giờ bỏ ngoại hình), thêm 221; `[BC-LOAI]` in 8 loại NHIỀU NHẤT | `BroadCastGoiToiDa=5000` |
| `KRegion::Activate` | chunk theo config, đếm theo **lần phát thật**, cửa sổ liên tục từ con trỏ (hết quấn vòng) | `DongBoMoiTick=10` (cũ 5, kẹp 1..40) |
| `KRegion::AddPlayer` | ghi `m_dwLucCoNguoiVao`; NormalSync phát bất kể băm khi có người vừa vào 9 vùng | |
| `KNpc::NormalSync` | tách `NS_DungGoi`; băm toàn gói + băm nhóm chậm → **bỏ qua / gói GỌN 221 (25 byte) / gói 77 đầy đủ**; trả về có phát hay không; log `[NS-BO]` | `DongBoLamMoi=2000` ms (0 = như cũ), `DongBoLamMoiDay=10000` ms, `DongBoGoiGon=1` |
| `KNpc::SendSyncData` | đẩy riêng một gói 77 đầy đủ ngay sau `NPC_SYNC` cho client vừa biết NPC | |
| `KNpc.cpp` 75 | làm mới ngoại hình 5 → 30 s | `BroadCastLamMoi=30` |
| `KProtocolDef.h`, `KProtocol.h/.cpp` | `s2c_syncnpcpos = 221` + `NPC_POS_SYNC` 25 byte (ô 156 bảng client); `c2s_deltahello = 175` + `C2S_DELTA_HELLO` 2 byte (ô 110 bảng máy chủ) | |
| `KProtocolProcess.cpp` client | `SyncNpcPos`: dựng lại `NPC_NORMAL_SYNC` từ gói gọn + trường chậm trong `Npc[]` rồi giao `SyncNpcMin` (giữ nguyên S6/S10); `SyncEnd` gửi hello | |
| `KProtocolProcess.cpp` máy chủ | `DeltaHello` đặt `g_abyDeltaHello[]`; `KPlayerSet::Add` xoá cờ; `NS_SoClientCu()` đếm client chưa hello mỗi giây → **> 0 thì không phát gói gọn cho ai** | |
| `KSOServer.cpp` (GameServer.exe) | xả socket mỗi tick (cũ mỗi 2 tick) | |
| `SocketServer.cpp` (heaven.dll) | `TCP_NODELAY` khi accept | |

Không đụng: gói 75 (struct giữ nguyên), gói sự kiện (chỉ bỏ trần), client `Game.exe`, `Rainbow.dll` (Nagle phía client chưa tắt: cần build `Rainbow.dll` Win32, để sau).

### 8.2 Vì sao an toàn khi swap lệch (máy chủ mới + client cũ hoặc ngược lại)

- Máy chủ mới chỉ phát mã 221 khi **mọi** client thật đang nối đã gửi `c2s_deltahello`; client cũ không gửi → `NS_SoClientCu() > 0` → chỉ gói 77 đầy đủ như cũ (vẫn được lợi bỏ qua NPC không đổi). Không có nguy cơ lệch luồng ở `NetConnectAgent.cpp:345` (mục 6.1.5).
- Client mới + máy chủ cũ: máy chủ cũ không có handler 175 → `ProcessFunc` NULL → bỏ qua (`KProtocolProcess::ProcessNetMsg` phía máy chủ kiểm NULL); client không bao giờ nhận 221. Bình thường.
- Muốn tắt hẳn cơ chế mới mà không build: `DongBoLamMoi=0` (phát như cũ, không bỏ qua, không gói gọn), `BroadCastMotLan=100`, `BroadCastDongBo=500`, `DongBoMoiTick=5`, `BroadCastLamMoi=5`.

### 8.3 Nhị phân đã build và đặt `.moi`

| Tệp | md5 | Kích thước | Đặt ở | Ghi chú |
|---|---|---|---|---|
| `CoreServer.dll` | **4b89f185** | 18.478.080 | `bin\server\CoreServer.dll.moi` | thay bản a509a089 của phiên MATDO (đã kiểm superset: 0 chuỗi thiếu; bản cũ giữ tên `CoreServer.dll.moi.matdo_a509a089_0037`) |
| `CoreClient.dll` | **5c359b16** | 2.610.176 | `bin\client\CoreClient.dll.moi` | thay bản 4e25e8e4 (giữ `CoreClient.dll.moi.matdo_4e25e8e4_0037`) |
| `heaven.dll` | **096fdeb2** | 1.124.864 | `bin\server\heaven.dll.moi` | TCP_NODELAY; `Common.lib` x64 build lại rồi mới link |
| `GameServer.exe` | **b98d9f31** | 1.987.584 | `bin\server\GameServer.exe.moi` | xả mỗi tick. **`ChayGameServer.bat` chưa biết tệp này**: thêm dòng `call :capnhat GameServer.exe` ngay sau `call :capnhat heaven.dll` (bản lưu `.truoc_0709` đã có), hoặc chép tay |

Swap: tắt GameServer → `ChayGameServer.bat`; thoát game → `ChoiGame.bat`. Thứ tự tuỳ ý (8.2).

### 8.4 Nghiệm thu (đọc `bin\server\jx_auto_server.log`)

1. `[NS-BO] dong bo theo thay doi: bo=… gon=… day=… (lam moi 2000 ms, day du 10000 ms, gon=1, client cu=0)` — `client cu` phải về **0** sau khi client mới đăng nhập; đứng trong thành `bo` phải chiếm đa số; đi lại thì `gon` chiếm đa số, `day` chỉ khi có buff/máu tối đa đổi hoặc 10 s một lần.
2. `[BC-TOP]` phải thấy mã **221** thay chỗ 77 về số lượt; byte của 77 giảm mạnh.
3. `[BC-LOAI]` giờ in đúng 8 loại nhiều nhất (thấy 148/91/207 khi đánh nhau).
4. Vào Tống Kim: mọi chiêu/lệnh chạy của bot phải thấy đủ (trần đã bỏ), `[BC-DEM] cat_vi_het_ngan_sach=0`, `bo_vi_tri` nhỏ.
5. Client: đi từ ngoài vào thành NPC hiện ngay (gác người mới vào vùng); không tăng `[S6-BAL]`, `[S6-XOAXA]`, `[S10-KEO]` so với trước; buff/máu người khác vẫn đúng (gói đầy đủ khi thay đổi + SendSyncData).
6. `[PERF]` MainLoop không tăng quá 1 ms sau khi xả mỗi tick.

### 8.5 Chưa làm / để sau

- Nagle phía client (`Rainbow.dll` Win32, `ClientStage.cpp`): cần build Rainbow và thêm `Rainbow.dll` vào `ChoiGame.bat`.
- `ChayGameServer.bat` chưa được sửa (bị chặn quyền), chủ thêm một dòng như 8.3.
- Chưa tăng `KPROTOCOL_VERSION` (chỉ một client, và đã có bắt tay hello nên không cần).
- Gom danh sách + `PackDataToClients` một lệnh (CPU), van theo tình trạng socket: chờ đo với người thật.

### 8.6 ĐÃ CHẠY THẬT 00:56:55 (chủ chạy `ChayGameServer.bat` và `ChoiGame.bat`), số đo 10 giây đầu

`bin\server\CoreServer.dll` = 4b89f185, `heaven.dll` = 096fdeb2, `GameServer.exe` = b98d9f31 (tiến trình 54884 khởi động 00:56:55, sau khi tệp đã thay), `Game.exe` mới 00:57:02 với `CoreClient.dll` 5c359b16. Không có sập, `hethong.log` boot bình thường.

| Dòng log (pid 54884) | Giá trị | Ý nghĩa |
|---|---|---|
| `[NS-BO]` | `bo=716 gon=2491 day=112 ... client cu=0` | client mới đã chào; 75 % lượt là gói gọn, 22 % bỏ qua (NPC không đổi), 3 % gói đầy đủ |
| `[BC-TOP]` | `221:gui=1155,28KB` vs `77:gui=20,1KB` | gói gọn thay gần hết gói 77: 1.155 gói chỉ 28 KB (~25 byte/gói); trước đây 874 gói 77 = 84 KB |
| `[BC-DEM]` | `han_muc=5000/giay bo_vi_tri=0 cat_vi_het_ngan_sach=0` | van mới, không cắt gì |
| `[PS-BO]` | `99% bo, lam moi 30 giay` | ngoại hình 30 s |

Lưu ý: lúc đo bot vừa đăng nhập lại sau restart nên `goi` mới ~3.000/10 s; xem lại `[NS-BO]`/`[BC-TOP]` khi 1.000 bot đã vào đủ và khi có Tống Kim. Nếu còn một cửa sổ `Game.exe` cũ (mở 23:21 hôm trước, CoreClient cũ) đăng nhập vào, `client cu` sẽ > 0 và máy chủ tự ngừng gói gọn cho tới khi cửa sổ đó thoát; đó là chủ ý, không phải lỗi.

### 8.7 Phân tích nhật ký 13 phút đầu sau swap (00:57–01:10, 1.000 bot vào đủ lúc 01:09, Tống Kim bật, chủ đứng trong đám đông)

Nguồn: `jx_auto_server.log` pid 54884 (77 cửa sổ 10 s), `jx_perf_server.log`, `logs\hethong.log`, client `jx_paint.log`, `jx_crash.log`. Công cụ: `ReverseTools\linuxbc\..` không dùng; script phiên này `ptich_log_delta.py` (scratchpad).

**Ổn định:** không dòng lỗi nào cho pid 54884; `hethong.log` boot bình thường, `RunTime` 14–38 ms; client không sập (`jx_crash.log` chỉ có dòng khởi động 00:57:02), **0 lần `Net Msg Error`** (client tách gói 221 đúng), `[SUM]` 124 lượt vẽ/giây, spikes 2 (15 phút trước swap: 6), max 116 ms (trước 143 ms).

**Máy chủ, cùng 1.000 bot:**

| Chỉ số | Trước swap (pid 67968, 11 cửa sổ cuối) | Sau swap (pid 54884, từ 150 s) |
|---|---|---|
| Lượt `BroadCast` mỗi 10 s | 2,4–2,6 triệu | 1,1–1,6 triệu (**−40 %**) |
| `NormalSync` mỗi giây | 28.900 | 30.700 (chunk 10 thay 5, nhưng 48 % bị bỏ vì NPC không đổi) |
| Quyết định cộng dồn 852 s | | bỏ 48 %, gọn 49 %, đầy đủ 3 % |
| TICK trung bình / p95 / % ngân sách | 6,7 ms / 9 ms / 12 % | 6,4–7,0 ms / 9 ms / 12 % (**không tăng** dù băm 99 byte mỗi NPC và xả mỗi tick) |
| `cat_vi_het_ngan_sach`, `bo_vi_tri` | 0, 0 | 0, 0 (`han_muc=5000`) |
| `client cu` | | 0 ở cả 86 dòng `[NS-BO]` |

**Client của chủ trong đám đông (CaiBang):** trung bình 19 KB/giây, đỉnh **43,6 KB/giây ở 1.640 gói/giây**. Cửa sổ đỉnh trước swap cùng cỡ gói (1.586 gói/giây) là 61,5 KB/giây. Cùng mức ~1.600 gói/giây: **−29 % byte**, trong khi số lần đồng bộ vị trí nhận được tăng gần gấp đôi (678/giây gồm 606 gói gọn + 72 đầy đủ, trước 363/giây) và mọi chiêu/lệnh chạy đều tới (trần đã bỏ).

Thành phần byte của client trong Tống Kim (trung bình 77 cửa sổ, KB mỗi 10 s): 221 gọn 84 · 77 đầy đủ 38 · 75 ngoại hình 29 · 207 số sát thương 14 · 148 chiêu 9 · 86 chạy 7 · 95 chiêu 3. Đỉnh 207 lên 103 KB/10 s (6.261 gói) khi đánh dồn: đúng như dự đoán, số sát thương là gói nhiều nhất khi đánh nhau, và chủ chọn không cắt.

**Phát hiện cần chỉnh (đã làm ngay, commit `[DELTA 07/09 c]`):** gói 77 đầy đủ chỉ 6 % số lượt nhưng 20–30 % byte đồng bộ, và trong Tống Kim có cửa sổ 77 chiếm 139 KB > 221 (72 KB). Nguyên nhân: `m_nProtectedTime` (vòng bất tử sau hồi sinh) nằm trong nhóm chậm nhưng máy chủ trừ nó **mỗi tick** (`KNpc.cpp:1627`), nên NPC đang bất tử phát gói đầy đủ ở mọi lần đồng bộ; bot Tống Kim hồi sinh liên tục. Client không tự đếm lùi và chỉ cần biết bật/tắt (`CoreShell.cpp:8965`), nên băm quy về 0/1: gói đầy đủ chỉ đi khi bật và khi hết. Kèm theo: `[NS-BO]` in số trong 10 s thay vì cộng dồn.

**Chưa cần chỉnh:** 75 ngoại hình 29 KB/10 s trong Tống Kim là do đổi thật (cờ chiến đấu, lên xuống ngựa, hồi sinh), không phải làm mới 30 s. 207 là lựa chọn của chủ. `node_duyet` tăng (4,5–6,2 triệu/10 s) vì chủ đứng giữa đám đông, CPU không đổi.

**Bản vá c đã build:** `bin\server\CoreServer.dll.moi` = **4b524b2f** (18.478.080, 01:15), chỉ máy chủ, client giữ nguyên 5c359b16. Chờ chủ tắt GameServer → `ChayGameServer.bat`. Nghiệm thu: trong Tống Kim, dòng `[NS-BO] 10s` có `day` nhỏ hơn hẳn `gon` (kỳ vọng dưới 3 %), `[BC-TOP]` byte của 77 giảm so với cửa sổ 360 s ở 8.7.

### 8.8 Trận Tống Kim trọn vẹn 01:00–01:30 trên bản DELTA (chủ: "để hết trận lấy log phân tích tiếp, tìm phương án fix tiếp")

Nguồn: `jx_auto_server.log.1` (tệp đã xoay, 67 MB) + `jx_auto_server.log`, pid 54884, **181 cửa sổ 10 s = 30,0 phút**, t = 1048800419..1050600598; `jx_perf_server.log`; client `jx_paint.log`, `jx_crash.log`. Script: `ptich_tran_tk.py` (scratchpad; đọc cả hai tệp, tách theo pid, lọc theo t). Bản chạy suốt trận: CoreServer 4b89f185 (vá c 4b524b2f chưa swap), 1.000 bot, Tống Kim trận 500, chủ (CaiBang) đứng trong đám đông.

**Ổn định cả trận:** máy chủ TICK 6,4–7,0 ms (12 % ngân sách), `tre` ≤ 1/phút, không dòng lỗi; client không sập, 0 `Net Msg Error`, 4 spike (trận 22:50 hôm trước 9 spike), thời gian vẽ mỗi lượt 2,1 ms (trước 1,44 ms: nhận gấp đôi lần đồng bộ và đủ chiêu nên vẽ nhiều hơn, vẫn ~124 lượt/giây).

**Client của chủ, phân bố theo 181 cửa sổ:**

| | gói/giây | KB/giây |
|---|---|---|
| trung vị | 818 | 23,7 |
| p90 | 1.545 | 40,8 |
| p95 | 1.640 | 43,3 |
| tối đa | 2.196 | 51,3 |
| trung bình | 864 | 24,8 |

Trận 22:50 hôm trước (bản cũ, cùng bot, cùng chỗ) chỉ còn 8 cửa sổ đỉnh trong nhật ký cũ: 1.586 / 1.490 / 1.483 gói/giây ở 61,5 / 60 / 51 KB/giây. Cùng ~1.500–1.600 gói/giây thì bản DELTA là 40–43 KB/giây, **giảm 29–33 %**, trong khi số lần đồng bộ vị trí nhận được gấp đôi và mọi chiêu/lệnh chạy đều tới.

**Thành phần byte tới client cả trận (44,1 MB / 30 phút):**

| Mã | Gói | MB | Tỉ lệ | Cỡ thật |
|---|---|---|---|---|
| 221 vị trí gọn | 829.356 | 20,2 | **46 %** | 25 B |
| 77 vị trí đầy đủ | 79.546 | 7,6 | **17 %** | 98 B |
| 75 ngoại hình | 29.916 | 6,7 | **15 %** | 231 B |
| 207 số sát thương | 281.345 | 4,6 | 10 % | 17 B |
| 148 chiêu | 104.882 | 2,1 | 5 % | 20 B |
| 86 lệnh chạy | 147.886 | 1,8 | 4 % | 12 B |
| 95 chiêu | 48.234 | 0,9 | 2 % | 20 B |
| 85 lệnh đi | 23.049 | 0,2 | 0,5 % | 10 B |

Quyết định đồng bộ cả trận (cộng dồn `[NS-BO]`): bỏ qua 25,1 triệu (44 %), gọn 29,5 triệu (52 %), đầy đủ 1,77 triệu (3 %). Lượt `BroadCast` 1,4–1,7 triệu/10 s (trước 2,4–2,6 triệu), `node_duyet` 4,5–7 triệu/10 s (chủ giữa đám đông), không cắt, van không bỏ.

**Ba phát hiện dẫn tới phương án sửa tiếp:**

1. **Gói 77 đầy đủ vẫn 17 % byte** (44 gói/giây tới client) dù chỉ 6 % số lượt; có cửa sổ 77 nhiều hơn 221 (phút 8,0: 1.526 gói = 147 KB; phút 28,5: 1.322 gói = 127 KB). Vá c (vòng bất tử băm 0/1) đã build nhưng chưa swap; kỳ vọng còn dưới 5 %.
2. **Gói 75 ngoại hình 15 % byte** (16,6 gói/giây × 231 byte) dù làm mới 30 s. Nguyên nhân duy nhất đổi liên tục trong trận là bit `0x02` cờ chiến đấu trong `m_btSomeFlag` (bot đánh/nghỉ/hồi sinh, `KPlayer.cpp:7002-7007`, `KPlayerBot.cpp:12119`); client chỉ biết cờ này qua gói 75 (`KProtocolProcess.cpp:3035`). **Vá d:** cờ chiến đấu đi theo bit `STATE_FIGHTMODE 0x40` của `State` trong gói 77/221 (client `SyncNpcMin` áp cho `kind_player` khác mình), bỏ bit `0x02` khỏi băm gói 75; thêm đếm `[PS-BO] doi: chi_co_chien_dau / co_khac_hoac_ngua / khac` để đối chiếu. Kỳ vọng 75 còn dưới 3 %.
3. **207 số sát thương 10 %** trung bình nhưng tới 40 % ở cửa sổ đánh dồn (12.707 gói = 210 KB/10 s). Là lựa chọn của chủ (không cắt); nếu sau này cần thì gói cho người xem có thể bỏ `dwLauncher` (17 → 13 byte) vì client chỉ dùng người nhận, hoặc thu tầm cho người xem. Chưa làm.

**Việc ngoài lề phát hiện qua `jx_perf_server.log`:** trong khoảng 22:50–01:05 có **ba** chuỗi `[PERF]` cùng ghi vào tệp này (giây lệch :13/:12/:01 rồi :16/:17/:50), tất cả báo `online=1001`; chuỗi ":01→:50" là GameServer thật (khởi động lại 23:22 và 00:56, `online=1` lúc mới lên). Chuỗi ":13" **bão hoà suốt trận 22:52–23:21** (TICK 47–62 ms, 99,9 % ngân sách, 350–440 tick trễ/phút, tick 960–1.279/phút) nhưng đó không phải vòng game thật (`[BC-VUNG]` cùng lúc ghi 18 tick/giây). Chưa biết hai tiến trình kia là gì; vá d thêm `pid=` vào đầu dòng `[PERF]` để lần sau đọc ra ngay.

**Sau vá d, mức kỳ vọng cho client trong Tống Kim:** 221 ~11 KB/s + 207 ~3 KB/s + 148/95/86 ~3 KB/s + 77 ~1 KB/s + 75 ~0,5 KB/s ≈ **19 KB/s trung bình, ~35 KB/s đỉnh** (từ 24,8 / 51). Phần còn lại là gói gọn 25 byte × 460/giây, tức là "giá" của việc đồng bộ vị trí dày gấp đôi; muốn giảm nữa chỉ còn cách hạ `DongBoMoiTick` (10 → 6 giảm ~40 % phần 221 nhưng NPC lỡ lệnh chạy chờ lâu hơn), là đánh đổi trải nghiệm nên để chủ quyết.

**Vá c + d đã build (commit a8a6bfcc, 5b390cd8), chờ swap CẢ HAI:** `bin\server\CoreServer.dll.moi` = **6246967d** (18.479.104, thay bản c 4b524b2f, giữ tên `.moi.delta_c_4b524b2f_0115`), `bin\client\CoreClient.dll.moi` = **9cb92330** (2.610.176). Máy chủ mới + client cũ vẫn chạy (client cũ bỏ qua bit 0x40, chỉ mất tính năng cờ chiến đấu theo gói vị trí; gói 75 vẫn mang cờ khi được gửi). Nghiệm thu trận Tống Kim kế: `[PS-BO] ... doi: chi_co_chien_dau=` phải chiếm đa số các lần đổi trước đây và 75 tụt dưới 3 % byte; `[NS-BO] 10s` cột `day` dưới 3 %; `[PERF]` có `pid=` để nhận diện hai tiến trình lạ.

**Cập nhật 03:28:** phiên SK120 đã thay khe máy chủ bằng bản của họ **1a33f617** (build từ `origin/main` 33b1dfa4 = gồm trọn DELTA c+d, đã kiểm 0 chuỗi thiếu; bản 6246967d giữ tên `.moi.delta_6246967d_0142`). Client build lại từ `origin/main` e85303e2 (gồm cả `KMissle.cpp`/`KNpc.cpp` của SK120): `bin\client\CoreClient.dll.moi` = **5f86a7f7**. Cặp chờ swap hiện tại: **máy chủ 1a33f617 + client 5f86a7f7**, chạy cả hai bat.

### 8.9 So sánh bản c+d (máy chủ 1a33f617 + client 5f86a7f7, chạy từ 07:28) với trận 01:00 (bản 4b89f185)

Chủ chạy cả hai bat lúc 07:28; trận Tống Kim mới bắt đầu 07:31. Số dưới đây lấy 13,5 phút đầu (82 cửa sổ, 3 phút đầu bot còn đang vào), so với 30 phút của trận 01:00. Chưa hết trận, chỉ so xu hướng; số liệu trọn trận đo lại sau 08:01.

| Chỉ số | Trận 01:00 (bản a) | 07:31 (bản c+d, 13,5 phút) |
|---|---|---|
| 77 đầy đủ, tỉ lệ byte | 17 % | **10,5 %** |
| `day/(gon+day)` (NS-BO) | 5,7 % | **2,7 %** |
| 75 ngoại hình, tỉ lệ byte | 15 % | **12,1 %** (chưa giảm như kỳ vọng) |
| 221 gọn | 46 % | 54 % |
| 207 số sát thương | 10 % | 14 % (đánh dồn gần chủ nhiều hơn) |
| Client trung bình | 24,8 KB/s | 12,3 KB/s (gồm 3 phút vắng) |
| Client đỉnh | 51,3 KB/s @ 2.196 gói/s | 52,6 KB/s @ 2.185 gói/s (cửa sổ 207 + 221 dồn) |
| TICK máy chủ | 6,7 ms | 6,5 ms, `tre` 1 |
| Client | 4 spike / 30 phút | 10 spike, 9 trong 5 phút đầu (nạp map), 0 `Net Msg Error` |
| `client cu` | 0 | 0 |

**Đọc:** vá c làm gói 77 đầy đủ giảm gần một nửa như tính. Vá d đúng nhưng chưa đủ: `[PS-BO] doi: chi_co_chien_dau=10.782 co_khac_hoac_ngua=5.924 khac=41.321` — cờ chiến đấu chỉ là 19 % số lần gói 75 phải phát lại, **71 % là trường khác** chưa xác định. Nghi ngờ theo mã: bốn trường tốc độ (`WalkSpeed/RunSpeed/AttackSpeed/CastSpeed` = tốc độ HIỆN TẠI, đổi mỗi lần trúng buff/giảm tốc trong trận) hoặc `RankInWorld/Repute` (xếp hạng tính lại mỗi phút). Đỉnh băng thông không đổi vì cửa sổ đỉnh do 207 + 221 quyết định, đúng như dự báo mục 8.8.

**Vá e (chỉ máy chủ, chẩn đoán):** băm gói 75 với từng nhóm trường xoá trắng (tốc độ / rank-danh hiệu / chỉ số / trang bị-ngựa / tên-chuỗi-cờ) → `[PS-BO] ... nhom: toc_do= rank= chi_so= trang_bi= ten= nhieu=`. Sau trận kế đọc dòng này là biết trường nào; cách sửa tương ứng đã sẵn: tốc độ hiện tại → đưa 2 byte vào gói gọn (27 byte, đúng bằng Linux) và bỏ khỏi băm 75; RankInWorld/Repute → bỏ khỏi băm 75 và chỉ gửi ở kỳ làm mới 30 s (client không cần tức thời).

**Bí ẩn `[PERF]` đã hết:** từ 07:28 chỉ còn một tiến trình ghi (`pid=54436`). Hai chuỗi lạ đêm qua không còn; nếu tái xuất thì `pid=` sẽ chỉ ra ngay.

**Vá e đã build:** `bin\server\CoreServer.dll.moi` = **bdc8ae53** (18.480.640, từ `origin/main` aa38685a ⊇ SK120 33b1dfa4), chỉ máy chủ; client giữ 5f86a7f7. Swap sau khi trận 07:31 kết thúc (~08:01), rồi đọc `[PS-BO] ... nhom:` ở trận kế.

**Cập nhật 07:55:** phiên SKEXP thay cả hai khe bằng bản build từ `origin/main` 217202f0 (⊇ vá e b4e6277c, đã kiểm đủ dấu hiệu DELTA): **`CoreServer.dll.moi` = ce462519, `CoreClient.dll.moi` = e7c70880** (không đổi giao thức; bản e bdc8ae53 giữ tên `.moi.delta_e_bdc8ae53_0749`). Cặp chờ swap hiện tại là cặp này, chạy cả hai bat sau trận 07:31.

**Cập nhật 09:58:** phiên TOCDO thay khe: **`CoreServer.dll.moi` = e37ab486, `CoreClient.dll.moi` = ab99660c** (build từ `origin/main` ffffade4 ⊇ vá e, đã kiểm đủ dấu hiệu DELTA; cặp SKEXP giữ tên `.moi.tocdo_*`). Live vẫn 1a33f617 + 5f86a7f7 từ 07:28, tức vá e chưa chạy; chạy cả hai bat để có chẩn đoán gói 75 ở trận kế.

### 8.10 Trận 10:11 trên bản có vá e (live db5d064f/563c127e từ 10:08): thủ phạm gói 75 và vá f

Chủ swap 10:08 (bản MATDO b c37a0b1b, gồm TOCDO + SKEXP + DELTA a–e); trận Tống Kim thứ ba của ngày bắt đầu 10:11. Nhật ký trận 07:31 đã bị xoay mất (chỉ còn 13,5 phút đầu ở mục 8.9). Số dưới đây là 11 phút đầu trận 10:11 (67 cửa sổ).

| Chỉ số | Trận 01:00 (a) | 07:31 (c+d) | 10:11 (c+d+e) |
|---|---|---|---|
| 77 đầy đủ, % byte | 17 | 10,5 | 9,9 |
| `day/(gon+day)` | 5,7 % | 2,7 % | 2,6 % |
| 75 ngoại hình, % byte | 15 | 12,1 | 11,8 |
| 221 gọn | 46 | 54 | 61 |
| Client trung bình / đỉnh | 24,8 / 51,3 KB/s | 12,3 / 52,6 | 8,5 / 42,5 |
| TICK | 6,7 ms | 6,5 | 6,5 |
| Client giật | 4 | 10 (9 lúc nạp) | 10 (10 lúc nạp), 0 `Net Msg Error` |

**Chẩn đoán vá e trả lời dứt khoát** (`[PS-BO]` cộng dồn 11 phút, 38.500 lần gói 75 phải phát lại): chỉ cờ chiến đấu 7.514 (đã loại nhờ vá d), chỉ cờ khác/ngựa 2.972, còn lại 35.481 chia theo nhóm: tốc độ 2.118 · rank 347 · chỉ số 0 · trang bị 10 · tên/chuỗi 334 · **nhiều nhóm cùng lúc 32.649 (90 %)**. Cái gì đổi hai nhóm trong một lần? **Lên/xuống ngựa**: `KPlayer::CheckRideHorse` (`KPlayer.cpp:10478`) đổi `m_bRideHorse` (→ `HorseType`, nhóm trang bị) và gỡ/áp thuộc tính ngựa (→ `RunSpeed`, nhóm tốc độ) cùng lúc; bot Tống Kim lên ngựa để chạy xa và xuống ngựa để đánh (`KPlayerBot.cpp:3928, 6743`), mỗi bot vài chục giây một lần. Client chỉ biết ngựa và tốc độ hiện tại qua gói 75 (`KProtocolProcess.cpp:2994`, `:3008`), nên mỗi lần lên/xuống ngựa là 231 byte tới mọi người trong tầm.

**Vá f (commit aa1c78cc):** `NPC_POS_SYNC` thêm `HorseType, WalkSpeed, RunSpeed` (25 → 28 byte, bản Linux 27); máy chủ gom ba trường này vào băm gói gọn và bỏ `HorseType/WalkSpeed/RunSpeed/AttackSpeed/CastSpeed` khỏi băm gói 75 (tốc độ đánh/ra chiêu client đã nhận qua gói 77 `m_ASpeed/m_CSpeed`); khi phải gửi gói 77 đầy đủ mà ngựa/tốc độ vừa đổi thì gửi thêm một gói gọn (`[NS-BO] gon_them=`). Client `SyncNpcPos` áp ngựa + tốc độ cho người chơi khác y hệt `SyncPlayerMin`. Kỳ vọng: gói 75 còn dưới 2 % byte, tổng byte client −10 %, và cái nhìn thấy không đổi (ngựa hiện đúng lúc, tốc độ chạy đúng).

**Vá f đã build (từ `origin/main` b666656b + vá f = a6d62a3c):** `bin\server\CoreServer.dll.moi` = **bc6b2b80** (18.481.664), `bin\client\CoreClient.dll.moi` = **9916d00f** (2.611.712); đổi cỡ gói 221 (25 → 28) nên phải swap CẢ HAI (máy chủ mới chỉ phát 221 khi client đã chào, nên lệch vẫn không sập). Nghiệm thu trận kế: `[PS-BO]` cột `nhieu` gần như đứng yên, gói 75 dưới 2 % byte trong `[BC-TOP]`; `[NS-BO] 10s` có `gon_them=`; bot lên/xuống ngựa vẫn hiện đúng trên client.

**Cập nhật 10:42:** phiên BANGSAT2 thay khe bằng bản từ `origin/main` 34b58589 (⊇ vá f a6d62a3c + Chiến Lệnh 10a55613 + BANGSAT2): **`CoreServer.dll.moi` = 8008e14d, `CoreClient.dll.moi` = 7bcf5119** (đã kiểm có `gon_them`, đủ dấu hiệu DELTA; bản f giữ tên `.moi.delta_f_*_1028`). Kèm `Game.exe.moi` 610a1b63 của phiên Chiến Lệnh. Swap cả ba bằng hai bat.

### 8.11 Trận 11:12 trên bản có vá f (live e8130f46 từ 11:09 + client 7bcf5119 từ 10:43): kết quả và bước tiếp

58 cửa sổ 10 s (11:09–11:19, trận từ 11:12, chủ giữa đám đánh dồn). Không sập, 0 `Net Msg Error`, TICK 6,5 ms (một nhịp 414 ms lúc 11:21 là script, `tre` 2/11 phút).

| Chỉ số | 01:00 (a) | 10:11 (a–e) | 11:12 (a–f) |
|---|---|---|---|
| Gói 75 ngoại hình, % byte | 15 | 11,8 | **7,3** |
| Gói 75, số lần phát (máy chủ) | – | 51.700 / 11 phút | **19.800 / 10 phút** (−62 %) |
| `[PS-BO] nhom nhieu` (lên/xuống ngựa) | – | 32.649 | **131** |
| Gói 77 đầy đủ, % byte | 17 | 9,9 | **6,9** |
| 221 gọn | 46 | 61 | 53,6 |
| 207 số sát thương | 10 | 8,5 | **25** (đánh dồn: một cửa sổ 13.229 gói = 219 KB) |
| Client trung bình / đỉnh | 24,8 / 51,3 KB/s | 8,5 / 42,5 | 17,6 / **64,7** @ 3.337 gói/s |
| `gon_them` | – | – | ~700 / 10 s |

**Đọc:** vá f đúng thủ phạm: số lần gói 75 phát lại vì lên/xuống ngựa từ 32.649 xuống 131. Phần còn lại của 75 (19.800 lần/10 phút ≈ 1.000 bot ÷ 30 s) **chính là kỳ làm mới 30 giây**, không còn là thay đổi thật; các nguyên nhân thật chỉ còn rank 678, tên/cấp 530, cờ khác 3.897 (bit PK/ngủ/bang), khác 1.342.

**Bước tiếp (theo lợi/công):**

1. **Chỉ config, không build:** `config.ini [Server] BroadCastLamMoi=300` (kỳ làm mới ngoại hình 30 → 300 s). An toàn vì gói 75 không còn nằm trong lớp bị van bỏ và người mới thấy NPC đã nhận ngoại hình đầy đủ qua `PLAYER_SYNC` lúc hỏi NPC. Kỳ vọng 75 còn ~1 % byte. Chủ thêm dòng vào `[Server]` (không có dấu cách trước `=`).
2. **207 giờ là gói lớn thứ hai khi đánh dồn (25 %, đỉnh 219 KB/10 s).** Cách giảm không mất gì: bản phát tán cho người xem bỏ `dwLauncher` (client `s2cShowDamage` chỉ dùng người nhận, số, loại, chí mạng; `dwLauncher` chỉ để ghi log), 17 → 13 byte (−24 % phần 207, ≈ −6 % tổng lúc đánh dồn); người đánh/nạn nhân vẫn nhận bản đầy đủ riêng. Đổi giao thức nhỏ (mã mới, gated hello). Làm khi chủ muốn.
3. `gon_them` ~700/10 s (gói gọn kèm sau gói đầy đủ) là chi phí nhỏ (25 B), giữ.
4. Đỉnh 64,7 KB/s @ 3.337 gói/s là "giá" của không cắt số sát thương khi 250 bot đánh dồn quanh chủ; client vẫn 124 lượt vẽ/giây, 2,2 ms mỗi lượt.

### 8.12 Vá g: kỳ làm mới gói 75 lên 300 s + gói 207 gọn cho người xem + hết nhận 207 hai lần

**Từ trận 11:12:** gói 75 còn 7,3 % và 19,8k lần/10 phút = đúng kỳ làm mới 30 s của 1.000 bot. Kiểm `bin\server\config.ini`: **không có mục `[Server]`** nên mọi khoá DELTA đang chạy mặc định; và mã kẹp `BroadCastLamMoi` ≤ 60 s (`KNpc.cpp` chỗ đọc khoá) nên chỉ sửa config không tới 300 được. Vá mã: mặc định 300 s, kẹp 3.600 s (config vẫn ghi đè được).

**Gói 207 (25 % khi đánh dồn), đọc `KNpc::SyncDamageInfo` (KNpc.cpp:3963):** phát 9 vùng bản 17 byte tới mọi người trong tầm 32 ô, gồm cả người đánh và người bị đánh, rồi **gửi riêng lại** cho người đánh và người bị đánh. Nghĩa là hai người trong cuộc nhận mỗi đòn **hai gói trùng nhau** (client `SetBlood2` vẽ hai số chồng khít cùng chỗ cùng lúc nên không ai thấy). Client `s2cShowDamage` (KProtocolProcess.cpp:4621) chỉ dùng người nhận, số, loại, chí mạng; `dwLauncher` chỉ vào dòng log (dòng `AddInfo` dùng nó đã bị chú thích từ trước).

**Vá g (script `ReverseTools/goi_va_delta7_207gon_lammoi_0709.py`, 8 tệp):**

1. `DAMAGESHOW_GON` 13 byte = `DAMAGESHOW` bỏ `dwLauncher` (thứ tự trường giữ nguyên), mã mới `s2c_showdamagegon = 222`, ô 157 bảng cỡ client; client dựng lại `DAMAGESHOW` với `dwLauncher = 0` rồi đi chung `s2cShowDamage`.
2. Phát vùng dùng bản gọn khi mọi client đang nối đã báo hello **phiên bản 2** (`NS_SoClientCu2`, đếm lại mỗi giây); `KRegion::BroadCast` thêm hai tham số mặc định `nBoNguoi1/2 = -1` để **loại hai người trong cuộc** khỏi phát vùng; họ vẫn nhận gói riêng đầy đủ 17 byte như cũ → mỗi đòn còn đúng một gói.
3. Hello `byPhienBan = 2`; máy chủ lưu phiên bản (máy chủ cũ ghi 1 với mọi giá trị nên không sao). Ma trận: client cũ + máy chủ mới → 207 đầy đủ; client mới + máy chủ cũ → không phát 222. Không thể lệch luồng.
4. Đếm `[DMG-GON] gon= day= (client chua bao phien ban 2: n)` mỗi 20.000 đòn để đối chiếu.

**Kỳ vọng trận kế:** 75 từ 7,3 % xuống ~0,8 %; byte 207 tới người xem −24 %, người trong cuộc bớt một gói 17 byte mỗi đòn gây/nhận; tổng byte client ≈ −10 % trong trận, đỉnh 64,7 → ~58 KB/s; không đổi gì nhìn thấy.

**Đã đặt 11:38 (chờ chủ chạy CẢ HAI bat):** `CoreServer.dll.moi` **1d06f7f0** (build từ origin/main be72d6b6 = DELTA a–g + LNCK + TUKICH + TRANPHAI60; bản LNCK a71d3305 đổi tên `.moi.lnck_a71d3305`) và `CoreClient.dll.moi` **1ca74e6e** (⊇ TUKICH 15bd934d, đổi tên `.moi.tukich_15bd934d`). Thứ tự swap không quan trọng: máy chủ mới + client cũ → 207 đầy đủ; client mới + máy chủ cũ → không phát 222. Sau trận kế: đọc `[DMG-GON] gon= day=` (gon phải ≈ 100 %), `[PS-BO] gui=` (kỳ vọng ≈ 2.000/10 phút thay vì 19.800), tỉ lệ byte 207 của client.

### 8.13 Hồi quy của vá d/g: cờ chiến đấu của CHÍNH MÌNH kẹt trên client (phù về thành vẫn "đánh skill được"), vá h

**Phiên TUKICH (wauto-58) đo và chuyển giao, tôi kiểm lại mã, đúng.** Chứng cứ của họ: cùng mốc thời gian, cùng chiêu 372, client ghi `[E4_SKILL_IN] fight=1` còn máy chủ ghi `[S2-NETSKILL-IN] fight=0`, lệch kéo dài ≥ 7 giây. Chiều ngược lại cũng dính: ra khỏi thành client kẹt `fight=0`, không đánh được.

**Gốc = hai mảnh của DELTA gặp nhau:** vá d bỏ bit `0x02` (cờ chiến đấu) khỏi băm gói 75 (`KNpc.cpp` `sPSBam.m_btSomeFlag &= ~0x02`) với lý do cờ đã đi theo `STATE_FIGHTMODE` của gói 77/221; nhưng client `SyncNpcMin` (KProtocolProcess.cpp:2509) lại **loại trừ chính mình** khi áp cờ đó. Với chính mình không còn đường nào: 75 không phát lại (bit đã bỏ khỏi băm), 77/221 thì client tự bỏ qua; chỉ còn kỳ làm mới 75, mà vá g vừa nâng 30 → 300 s nên client giữ cờ cũ tới 5 phút (tự khỏi khi trường khác của gói 75 đổi, nên lúc có lúc không).

**Vá h (client, một điều kiện, không đụng giao thức, không mất băng thông đã tiết kiệm):** bỏ vế "khác chính mình" ở KProtocolProcess.cpp:2509 → `if (Npc[nIdx].m_Kind == kind_player)`. An toàn vì máy chủ đã nhét cờ này cho mọi NPC kể cả chính mình (`NS_DungGoi`), gói gọn 221 mang `State` và `SyncNpcPos` chuyển tiếp sang `SyncNpcMin` cho cả chính mình (chỉ loại chính mình ở phần ngựa/tốc độ), và client không bao giờ tự ghi `m_FightMode` cho chính mình (KPlayer.cpp:6837/6866/6953/7031/7036 đều trong `#ifdef _SERVER`, mở ở 6808 và 6990). Cách khác (đưa bit 0x02 trở lại băm 75) sẽ mất lại phần tiết kiệm vì bot Tống Kim đổi cờ liên tục (`chi_co_chien_dau=135.596` trong 30 phút), không chọn. Script `ReverseTools/goi_va_delta8_fightmode_chinhminh_0709.py`.

**Kiểm sau swap:** vào thành bằng Thổ Địa Phù rồi bấm chiêu: client không còn cho mua chiêu; `findstr /C:"[E4_SKILL_IN] npc=1 " jx_auto.log` phải ra `fight=0` khớp `[S2-NETSKILL-IN] fight=0`. Ra khỏi thành bật chiến đấu phải đánh được ngay (cờ về theo 221 trong ≤ vài tick).

**Tình trạng lúc 15:06:** máy chủ live 11f83405 (14:35, có vá g: `[DMG-GON] gon=1.658.820 day=0`, `lam moi moi 300 giay`), client live 52a55d89 (TUKICH b, 14:43, có hello 2). Vá h chỉ cần swap client.

**Đã đặt 15:12 (chỉ client, chờ chủ chạy ChoiGame.bat):** `CoreClient.dll.moi` **716ab775** = build từ origin/main 7caf7cd0 + vá h (⊇ client live 52a55d89 TUKICH b, kiểm chuỗi missing = 0; commit 6a7ca54f sau đó chỉ là báo cáo, không đổi mã). Máy chủ không cần swap.

### 8.14 Đo sau khi vá g chạy thật (14:35 → 15:10, chủ vào đám đông ~400 NPC từ 14:57) và vá i

Nguồn: pid 27800 (máy chủ 11f83405 từ 14:35, có vá g), 210 cửa sổ 10 s = 34,8 phút; đoạn đánh nhau 90 cửa sổ (15 phút cuối). Client 52a55d89 (14:43) rồi 716ab775 (15:09, vá h). Không sập, không dòng lỗi, `[DMG-GON] gon=2,18 triệu day=0`, `lam moi moi 300 giay`.

**Mật độ khác trận 11:12** (chủ đứng chỗ ~400 NPC trong tầm 32 ô, gói 77 410/10 s ≈ số NPC trong tầm ÷ 10 s; trận 11:12 chỉ ~125/10 s) nên so theo tỉ lệ và theo "mỗi NPC", không so KB tuyệt đối.

| Chỉ số (đoạn đánh nhau) | 11:12 (a–f) | 15:00 (a–g) |
|---|---|---|
| 221 gọn | 53,6 % | 52,4 % (4.911 gói/10 s) |
| 207 → 222 số sát thương | 25 % (17 B) | **8,3 %** (222 13 B, 1.697 gói/10 s; 207 đầy đủ không còn xuất hiện trong top) |
| 77 đầy đủ | 6,9 % | 15,4 % (410 gói/10 s = **kỳ làm mới 10 s** × số NPC trong tầm) |
| 75 ngoại hình | 7,3 % | 11,2 % (127 gói/10 s; **cụm**: hai cửa sổ 778 và 1.024 gói = 177 / 234 KB) |
| Lần phát 75 máy chủ | 19,8k / 10 phút | 6,7k / 10 phút (−66 %) |
| Client TB / đỉnh | 17,6 / 64,7 KB/s | 26,2 / 62,0 KB/s @ 1.724 gói/s (mật độ gấp đôi) |
| TICK | 6,5 ms | 8,0 ms (14,5 %), 15:13 lên 12,6 ms / 87 ms max (đầu đợt mới) |

**Đọc:**

1. **207 gọn đúng như tính:** gon = 100 %, số sát thương còn 8,3 % dù chủ đứng giữa đám đánh dồn.
2. **Gói 77 giờ là mục lớn thứ hai (15,4 %)** và toàn bộ là kỳ làm mới đầy đủ 10 giây (`DongBoLamMoiDay`, NS-BO `day/(gon+day)` = 2,7 % máy chủ-wide nhưng tới client ở chỗ 400 NPC là 40 KB/10 s). Kỳ này chỉ là lưới an toàn: mọi trường chậm nằm trong băm chậm (đổi là phát đầy đủ ngay), NPC mới thấy thì client hỏi `c2s_requestnpc` → `SendSyncData` gửi đầy đủ; client không xoá NPC theo tuổi đồng bộ (chỉ theo S6 khoảng cách). → **vá i: 10 → 60 s** (−80 % gói 77 ≈ −12 % byte trong trận).
3. **Gói 75 phát theo cụm:** bot vào tầm cùng lúc thì 300 s sau làm mới cùng lúc → cửa sổ đỉnh 62 KB/s có 234 KB là 75 (38 %). → **vá i: dàn đều** + (m_Index % 128) giây cho từng NPC (đỉnh kỳ vọng ~45 KB/s).
4. **`co_khac_hoac_ngua = 14.508` trong 35 phút (62 % số lần phát 75)** chưa rõ bit nào: bot chỉ có bit 0x08 (tên bang) có thể đổi, người thật có 0x01/0x10 (PK) và 0x04 (ngủ); mà nhóm `ten` chỉ 1.540 nên số liệu tự mâu thuẫn. → **vá i: đếm XOR từng bit + tách bot/người** trong `[PS-BO] ... | bit: 01= 02= 04= 08= 10= 20= | bot= nguoi=`. Lần kéo log sau đọc dòng này là biết.
5. **Vá h (cờ chiến đấu chính mình):** client 716ab775 chạy từ 15:11:56; `[E4_SKILL_IN] npc=1 fight=1` khớp `[S2-NETSKILL-IN] fight=1` ở mọi dòng cuối; chưa có mẫu Thổ Địa Phù về thành để chốt, chủ thử theo mục 8.13.

**Vá i (chỉ máy chủ, script `ReverseTools/goi_va_delta9_dandeu75_lammoiday_0709.py`):** (1) `DongBoLamMoiDay` mặc định 60.000 ms, kẹp 600.000; (2) làm mới 75: `+ (m_Index % 128) * 1000` ms; (3) đếm bit cờ. Kỳ vọng trận kế ở cùng mật độ: 77 15 → ~3 %, 75 11 → ~6 % (còn phần bit chưa rõ), tổng −15 %, đỉnh 62 → ~45 KB/s.

**Đã đặt 15:21 (chỉ máy chủ, chờ chủ chạy ChayGameServer.bat):** `CoreServer.dll.moi` **b9b4cb4a** = origin/main 822e5857 (⊇ live 11f83405, kiểm chuỗi missing = 0). Sau trận kế đọc: `[PS-BO] ... | bit: 01= 02= 04= 08= 10= 20= | bot= nguoi=` (bit nào đổi), tỉ lệ byte 77 (kỳ vọng ~3 %), cửa sổ đỉnh không còn cụm 75.

### 8.15 Chu bao "chet ve thanh roi van nam bep duoi dat" (15:48): do 15 lan chet va va j

**Nhat ky noi gi** (client 716ab775 + may chu b9b4cb4a, 15 lan chet trong mot phien):

| Tang | Ket qua |
|---|---|
| May chu | chet -> ve thanh 0,70-0,78 s moi lan, khong lan nao `[S7-REV-NUOT]` |
| Logic client | 15/15 lan `[S7-REV-CLI2] doing=1 cdoing=1` (dung), mau ve day sau ~1,3 s |
| Lop ve | `[S9-VE]` ngay sau khi dung con `resdoing=8 resaction=36` (tu the chet) nhung `[S9-KET]` = **0** ca nhat ky, tuc lop ve duoi kip trong <= 3 nhip (~0,17 s) |
| Anh SPR | 0 dong `LoadImage FAIL` cho bo phan nhan vat trong phien (24.000 dong FAIL deu la thu muc giao dien Ui3) |

Nen **nhan hien co khong bat duoc** trieu chung chu thay: theo may do, 15/15 lan deu dung day trong 0,2 s.

**Nhung doc ma thi co mot loi that, dung hinh dang trieu chung** (`KProtocolProcess.cpp:2198`, ham `SyncNpc` xu ly goi dong bo DAY DU):

```
if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive) // need check later -- spe 03/05/27
    Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, ...);
```

Dieu kien nay **luon dung** (mot gia tri khong the vua bang `do_death` vua bang `do_revive`), tuc y dinh chan cua tac gia goc chua bao gio chay. He qua: goi dong bo day du **luon** ap `Doing` cua may chu, ke ca `do_death` va ke ca cho CHINH NHAN VAT. Nhat ky 07/09 co hai dong `SYNCNPC-SETPOS npc=92526 idx=1 doing=10 life=0` (nhan nay lay mau 1 giay/dong nen thuc te nhieu hon nhieu) - lan nay chung toi dung luc dang chet nen vo hai. Mot goi nhu vay toi **sau** khi da hoi sinh se dat nhan vat nam xuong trong khi may chu van coi la dang dung: dung "ve thanh roi van nam bep", va chi het khi nguoi choi tu di.

**Va j (chi client, `ReverseTools/goi_va_nambep_j_0709.py`):**

1. **Chan:** goi dong bo day du khong duoc dat CHINH NHAN VAT ve `do_death`/`do_revive` khi minh dang khong o hai trang thai do. Chet va hoi sinh cua chinh minh chi den tu goi rieng `s2c_npcdeath` va `s2c_playerrevive`. NPC khac khong doi. Moi lan chan ghi `[S7-NAMBEP-CHAN]` kem trang thai, nen lan sau doc nhat ky la biet day co phai goc that khong.
2. **Nhan bat tai tran `[S7-NAMBEP]`:** chinh minh con mau (`m_CurrentLife > 0`) ma logic hoac lop ve van o tu the chet lien tuc >= 0,5 s thi ghi moi 2 giay day du `doing/cdoing/resdoing/resaction/frame/life/o`, va ghi `[S7-NAMBEP-HET]` khi dung day. Neu lan toi van nam ma khong co dong `CHAN` nao thi loi nam o tang ve (Represent3), va so lieu trong dong nay chi thang tang nao.

Chu choi tiep vai tran roi keo nhat ky: co `[S7-NAMBEP-CHAN]` = da chan dung goc; co `[S7-NAMBEP]` ma khong co `CHAN` = con tang khac, doc `resdoing` de biet la lop ve hay logic.

**Da dat 16:14 (chi client, cho chu chay ChoiGame.bat):** `CoreClient.dll.moi` **a6a9e29d** = origin/main a21c0362 + va j (chua ca HC-CAT b51b6e31 cua phien khac, ban do doi ten `.moi.hccat_b51b6e31`; kiem chuoi: du 7/7 chuoi tinh nang cua ho). May chu khong can swap.

### 8.16 Va i chay that (may chu b9b4cb4a tu 15:33): so lieu 41,7 phut va muc tieu ke tiep

| Chi so | 15:00 (va g) | 16:15 (va i) |
|---|---|---|
| 75 ngoai hinh | 11,2 % | **7,4 %** (73 goi/10 s, het phat theo cum) |
| 77 day du | 15,4 % | 16,4 % (382 goi/10 s) |
| 221 gon | 52,4 % | 49,6 % |
| 222 so sat thuong | 8,3 % | 14,3 % (danh don nhieu hon) |
| Client TB / dinh | 26,2 / 62,0 KB/s | **22,9 / 58,9 KB/s** |
| day/(gon+day) | 2,6 % | 1,9 % |
| TICK | 8,0 ms | 8,7 ms (15,7 %) |

**Bo dem bit tra loi cau hoi treo tu muc 8.14** (`[PS-BO] ... | bit: 01=831 02=0 04=0 08=6 10=831 20=0 40=0 80=0 | bot=0 nguoi=27302`):

1. **bot = 0**: sau va d/f/i, bot khong con gay mot lan phat lai goi ngoai hinh nao. Toan bo phat lai la cua nguoi that.
2. Bit doi that su: `0x01` va `0x10` (cap co trang thai PK, luon doi cung nhau) 831 lan; `0x08` (co bang) 6 lan; co chien dau `0x02` = 0 (va d con nguyen tac dung); ngu `0x04` = 0.
3. Nhom `rank` (RankInWorld/Repute/FuYuan/PKValue/ReBorn) 1.115 lan - nhieu nhat trong cac nhom "cham".

**Ky lam moi day du 60 s KHONG ha duoc goi 77** (16,4 %): `day/(gon+day)` chi 1,9 %, tuc phan lon goi 77 khong phai do ky lam moi ma do **bam cham doi**. Cac truong con lai trong bam cham co the doi lien tuc khi danh nhau: `m_ASpeed`/`m_CSpeed` (toc do danh/ra chieu, doi theo buff), `m_btMenuState`, `NpcEnchant`, `Camp/CurrentCamp`. Buoc sau (khi khe .moi may chu ranh): dem tung nhom truong cua bam cham y nhu da lam voi goi 75, roi dua nhung truong doi nhanh xuong goi gon 221 hoac bo khoi bam cham.

**Loi hien thi nho trong nhat ky:** `[PS-BO] ... (-24% bo)` am la do `s_nPSBo * 100` tran so nguyen 32 bit khi bo > 21 trieu; chi la dong log, khong anh huong chay. Sua kem lan va sau.

### 8.17 Va k: dem nhom truong cua BAM CHAM (tim vi sao goi 77 van 16,4 %)

Muc 8.16 cho thay ky lam moi day du 60 s khong ha duoc goi 77 vi `day/(gon+day)` chi 1,9 % - goi day du la do **bam cham doi**, khong phai do ky lam moi. Bam cham = ca goi `NPC_NORMAL_SYNC` tru toa do/vung/Doing/State/mau/noi luc, va `m_nProtectedTime` quy ve 0/1. Nhung truong con lai co the doi lien tuc khi danh nhau, dac biet `m_WalkSpeed/m_RunSpeed/m_ASpeed/m_CSpeed` (buff toc do) va `StateInfo` (trang thai ky nang).

**Va k (chi may chu, khong doi hanh vi, script `ReverseTools/goi_va_delta11_nhom_bamcham_0709.py`):** bam lai voi tung nhom xoa trang, nhom nao khong doi thi doi nam trong nhom do - y het cach da dung cho goi 75 o va e. Nam nhom: toc do (Walk/Run/A/CSpeed) - trang thai ky nang (`StateInfo`) - chi so toi da (LifeMax/ManaMax) - phe/loai (Camp/Series/NpcEnchant/MissionGroup) - vong bat tu. Ket qua vao dong `[NS-BO] ... | bam cham doi o nhom: toc_do= trang_thai= chi_so_max= phe_loai= bat_tu= nhieu=`, dem lai moi 10 giay.

Khac va i mot diem quan trong: **moc so sanh cua tung nhom duoc luu DUNG LUC luu `s_adwNSBamCham` (lan phat day du truoc)**, khong luu moi tick. Bo dem bit cua va i lam moc theo tick nen so bit (1.668) khong khop so lan phat lai (24.806) - lan nay khong bi vay.

Kem theo: sua dong `[PS-BO]` in `-24% bo` do `bo * 100` tran so nguyen 32 bit khi bo vuot 21 trieu (tinh bang `__int64`).

**Chua dat vao khe `.moi`:** khe may chu dang giu ban `1ed4d726` cua phien BH100 (cap bang theo kinh nghiem) va commit cua ho **chua len origin/main**, nen toi khong the build sieu tap. Va k da push len origin/main; dat `.moi` sau khi chu swap ban cua ho, hoac sau khi ho push va toi build gop.

**Da dat 16:22:** `CoreServer.dll.moi` **b1308b4c** = origin/main 552bab7e (gom ca BHLV 40eb50c1 cua phien BH100 - ho da push nen build gop duoc; ban 1ed4d726 doi ten `.moi.bhlv_1ed4d726`). Kiem chuoi: co du `TONG_GetLevelExpNeed`, `tong_setting.ini`, `TONG_GetExpLevel`. `S3Relay.exe.moi` 26d7df5d cua ho giu nguyen, phai swap cung. Sau tran ke doc `[NS-BO] ... | bam cham doi o nhom:` de biet truong nao lam goi 77 phat lai.

### 8.18 Va l: goi TRANG THAI gon 223 - tra loi cua bo dem va k

**Bo dem nhom cua va k (b1308b4c chay that tu 16:21), 20 cua so = 200 giay:**

| Nhom truong lam BAM CHAM doi | So lan | Ti le |
|---|---|---|
| Trang thai ky nang (`StateInfo[18]`) | 50.527 | **74,7 %** |
| Chi so toi da (LifeMax/ManaMax) | 10.287 | 15,2 % |
| Nhieu nhom cung luc | 4.799 | 7,1 % |
| Vong bat tu | 1.988 | 2,9 % |
| Phe/he/phu phep | 6 | 0,0 % |
| **Toc do (Walk/Run/A/CSpeed)** | **0** | **0,0 %** |

Nghi ngo o muc 8.16 ve toc do buff **SAI**: toc do khong doi lan nao. Thu pham la trang thai ky nang - dung nhu Tong Kim: bot dong buff/trung debuff lien tuc, moi lan doi mot byte trong `StateInfo` la phat ca goi `NPC_NORMAL_SYNC` 98 byte du toa do khong doi.

**Va l (`ReverseTools/goi_va_delta12_goitrangthai_0709.py`, ca hai ben):**

1. Goi moi `s2c_syncnpcstate = 223`, cau truc `NPC_STATE_SYNC` **39 byte** = ma + ID + `StateInfo[18]` + 4 chi so toi da (o 158 bang co goi client).
2. May chu bo hai nhom nay khoi bam cham, giu bam rieng; khi chung doi ma dang gui goi GON thi gui them goi 223; khi gui goi DAY DU thi thoi (goi day du da mang san). Dem `[NS-TT] 10s goi trang thai gon (223): n lan`.
3. Hello len **phien ban 3**; chi bat khi MOI client dang noi deu bao 3 (`NS_SoClientCu3`). Swap mot ben khong sao: may chu moi + client cu thi giu nguyen cach cu; client moi + may chu cu thi khong ai phat 223.

**Ky vong:** 90 % so lan phat goi 77 chuyen tu 98 byte xuong 28 + 39 = 67 byte (goi gon + goi trang thai), tuc goi 77 tu 16,4 % xuong con vai phan tram va tong byte giam khoang 7 %. Muon huong loi phai swap **ca hai** ban.

**Da dat 17:05 (CA HAI, chu chay ca hai bat):** `CoreServer.dll.moi` **6693429f** + `CoreClient.dll.moi` **ae11479a** (origin/main + va l; ban BHWS 55c9c3fd cua phien BH100 doi ten `.moi.bhws_55c9c3fd`, da kiem co du `TONG_GetTongMap`, `TONG_GetLevelExpNeed`, `tong_setting.ini`). `S3Relay.exe.moi` 18de36e5 cua ho giu nguyen, swap cung. Sau tran ke doc `[NS-TT]` va ti le byte goi 77.

### 8.19 Va l chay that (ca hai ban swap 17:04, client vao lai 17:12): goi day du gan nhu bien mat

**So lieu may chu (dem quyet dinh, khong phu thuoc chu dung o dau):**

| Chi so /10 s | Va k (16:21-17:04) | Va l (17:05-17:21) |
|---|---|---|
| `day` (quyet dinh phat goi DAY DU 98 byte) | 4.372-4.886 | **584-843** (-83 %) |
| `day/(gon+day)` | 1,9 % | **0,4 %** |
| `gon_them` | 640-1.000 | 45 |
| Goi trang thai 223 phat | - | ~2.800 lan |
| Bam cham doi o nhom | trang_thai 74,7 % / chi_so_max 15,2 % | **trang_thai 0 / chi_so_max 0** (da roi khoi bam cham dung nhu thiet ke), con toc_do 24, bat_tu 242, phe_loai 15 |
| `client chua bao phien ban 3` | - | 0 (cong hello hoat dong) |

**Byte toi client cua chu (47 cua so, 7,7 phut):** 221 gon 61,3 % - 75 ngoai hinh 12,1 % - 222 so sat thuong 8,5 % - 148 chieu 5,3 % - **223 trang thai 5,0 %** - 86 lenh chay 4,2 % - 95 chieu 3,0 %. **Goi 77 day du khong con trong bang** (truoc do 16,4 %, dung thu ba); ca doan chi thay mot cua so co 63 goi 77.

**Thanh that ve phep do:** doan nay chu o cho thua nguoi hon doan truoc (BroadCast 1,09 trieu/10 s so voi 1,75 trieu; goi sat thuong 656 so voi 1.697/10 s) nen **khong so thang** trung binh KB/s duoc (10,1 / dinh 40,2 KB/s so voi 22,9 / 58,9 truoc do). Con so chac chan la hai dong dau bang tren: quyet dinh phat goi day du giam 83 % va ti le goi day du con 0,4 %, ca hai deu la dem tren toan bo 1.000 bot nen khong lech vi chu dung dau.

**Suc khoe:** TICK 8,5 ms (15 %), client 1.250 luot ve/10 s, 0 `Net Msg Error`, 0 giat, khong sap. Nhan `[S7-NAMBEP]` cua va j chua ghi dong nao nhung moi co 1 lan chet sau 17:12 - chua du de ket luan ve loi nam bep.

**Con lai gi:** goi 221 gon dang la 61 %, do la "gia" cua dong bo vi tri day gap doi (`DongBoMoiTick=10`), khong cat neu khong muon mat muot. Goi 75 ngoai hinh 12,1 % (nhung tuyet doi chi 11,8 KB/10 s) - phan con lai la cap co PK va nhom rank cua nguoi that, da do o muc 8.16.

### 8.20 Neu 500 NGUOI THAT cung danh thi sao? (chu hoi 17:30) - ngoai suy tu so do that

**Diem mau chot: hom nay chi co MOT ket noi that.** 1.000 bot chay trong long may chu, `m_nNetConnectIdx = -1`, nen **duong GUI chua bao gio bi thu**. Do that: `[BC-DEM]` 175.000 lan phat/giay, 500.000 node duyet/giay, nhung chi **1.421 lan gui that/giay** (mot client).

**Ngoai suy** (500 nguoi cung mot chien truong Tong Kim, mat do quanh moi nguoi bang muc do that hom nay ~250 thuc the):

| | 1 nguoi (do that) | 500 nguoi (ngoai suy) |
|---|---|---|
| Byte moi client | 10-23 KB/s, dinh 40-65 | khong doi (day la thanh qua cua DELTA) |
| Goi moi client | 400-900/s, dinh 1.500-3.300 | khong doi |
| **Byte ra tong** | 10-23 KB/s | **10 MB/s TB, dinh 30 MB/s = 80-240 Mbps** |
| **So lan `PackDataToClient`/giay** | 1.421 | **400.000 TB, dinh 1,5 trieu** |
| Trong MOT nhip 55 ms | 79 lan | **~83.000 lan** |
| Lan phat / node duyet | 175k / 500k moi giay | gan nhu khong doi (do so NPC quyet dinh) |

**Cai vo dau tien khong phai bang thong ma la MOT O KHOA CHUNG.** `CIOCPServer::PackDataToClient` (`MultiServer/Heaven/ServerStage.cpp:390`) lay **hai** khoa moi goi: `m_csCM` (khoa CHUNG toan bo bang client) roi `csWriteAction` (khoa rieng client). Voi 83.000 lan trong mot nhip: neu moi lan ton 150 ns thi mat 12 ms/nhip, con neu bi tranh chap voi luong IOCP nhan goi (500 ns) thi 42 ms - cong voi 8,5 ms hien tai la **vuot ngan sach 55 ms**, tuc tre nhip, giat, nguoi vo hinh. `SendPackToClient(-1)` moi nhip cung giu chinh khoa chung do trong khi duyet ca 500 client va goi `WSASend` cho tung nguoi.

**Thu tu vo:** (1) o khoa chung trong duong gui; (2) duong truyen ra 80-240 Mbps (may 100 Mbps la nghen, 1 Gbps thi du); (3) may cua nguoi choi yeu (400-3.000 goi/giay/nguoi - may chu game chay 125 khung/giay nhung may nguoi choi thuong yeu hon).

**Khong con la van de:** tran nguoi nhan moi lan phat da la 100.000 (`BroadCastDongBo`, do `cat_vi_het_ngan_sach=0`); van 5.000 goi/giay moi client chua bao gio cham; `MaxPlayer=1500` trong `GameServer_cfg.ini`. Neu 500 nguoi **tan ra khap the gioi** thay vi don mot cho thi khong co van de gi ca - phat tan tinh theo vung.

**DELTA da mua duoc bao nhieu:** truoc hom nay moi client an 24,8 KB/s TB (dinh 51-65) voi so lan dong bo vi tri chi bang MOT NUA. Cung tran 500 nguoi do, ban cu can khoang 2,5 lan bang thong va 2 lan so goi -> ~600 Mbps va ~3 trieu lan gui/giay. Noi cach khac: 500 nguoi tu **khong the** thanh **sat nguong**.

**Muon chac thi do, dung doan** (chua lam, cho chu quyet):

1. **Phep thu nhan ban duong gui** (`[Server] MoPhongNhanBan=500`, mac dinh 0): moi lan `PackDataToClient` thi chep them 499 lan vao bo dem rac va dem byte. Do dung chi phi CPU + byte cua 500 client ma khong can 500 may. Re nhat, ket qua chac chan nhat.
2. **Bo khoa chung `m_csCM` khoi duong nong** (chi giu khoa rieng tung client). Don bay CPU lon nhat, nhung dung vao loi mang nen phai lam can than va thu ky.
3. **Thua theo khoang cach khi dong**: qua 16 o thi ha nhip dong bo vi tri con mot nua (goi 221 dang chiem 61 % byte). Chi bat khi vung dong nguoi nen luc it nguoi khong mat gi.
4. **Chia chien truong cho nhieu GameServer** (da co san `GameServer1_cfg.ini`/`GameServer2_cfg.ini`, `MaxPlayer=290` moi ban).

### 8.21 Chu bao 17:40: (a) danh nhau mat het hinh anh ky nang, (b) chet ve thanh van nam duoi dat

**(a) Mat hinh anh ky nang - do duoc, la LOP VE CUA CLIENT, khong phai mang.** Goi chieu van toi day du:
may chu gui 148 (chieu) 1.029-1.696 goi/10 s va 95 (chieu) 906-1.076 goi/10 s cho client cua chu trong dung
khoang do. Nhung `jx_rep3.log` cho thay bo nho anh **dinh o tran suot tran**:

| | |
|---|---|
| Cache texture | **508-511 MB / ngan sach 512 MB** (khong luc nao roi khoi tran) |
| Ve khung nay | 92-200 MB (mot khung dong nguoi cham toi 200 MB anh) |
| So muc trong cache | 1.955-1.997 (bi ep giu quanh muc do) |
| Bo (giai phong) | 900-2.600 muc moi 30 giay, trong khi nap chi 40-90 |

`TextureResMgr::CheckBalance` khi VUOT ngan sach chuyen sang che do manh tay: moi luot bo toi 8 khung anh cua
**moi tai nguyen nghi qua 1 giay** (binh thuong la 10 giay). Hieu ung chieu dung thua - moi lan dung cach nhau
hon 1 giay - nen bi bo lien tuc roi phai nap lai; dong nguoi thi nap khong kip => **mat hieu ung, chi con dong
tac danh** (anh nhan vat ve moi khung nen khong bao gio bi bo). Ngan sach bi **kep cung 512 MB** trong
`TextureResMgr.cpp:97` du may co RAM 32 GB va VRAM con trong 3.580 MB.

**Sua (chi cau hinh, khong build):** them `Rep3CacheMB=1500` vao muc `[Client]` cua `bin\client\config.ini`
(khoa nay doc SAU cai kep 512 nen co hieu luc). Da them 17:45, ban luu `config.ini.truoc_rep3cache`. Chu thoat
game vao lai la co hieu luc; kiem bang dong `[REP3] cache texture: ... -> ngan sach 1500 MB` va sau do cache
khong con dinh o tran.

**(b) Nam bep: nhan cua va j SAI DIEU KIEN.** 11 lan chet trong nhat ky 17:00-17:40 khong ghi mot dong
`[S7-NAMBEP]` nao, vi dieu kien la "con mau ma van o tu the chet" - ma khi ket o trang thai chet thi client
cung dang giu `m_CurrentLife = 0` (`NetCommandDeath` dat 0), nen cong "con mau" chan mat bao dong.

**Va m (chi client, chi them log, `ReverseTools/goi_va_nambep_m_0709.py`):**

1. `[S7-SAUHOISINH]` - moc chac chan nhat: sau MOI lan chinh minh nhan goi hoi sinh, ghi trang thai day du o
   **+1 s, +3 s, +6 s** (`doing/cdoing/resdoing/resaction/frame/life/reg/o`). Ba dong moi lan chet: nam bep hay
   khong deu thay ro, khong con phu thuoc dieu kien doan truoc.
2. `[S7-NAMBEP-LAU]` - o tu the chet qua **10 giay lien tuc**, KHONG can con mau; chet that luon duoc hoi sinh
   trong ~1 giay (tu dong 5 giay) nen qua 10 giay chac chan la ket. Ghi lai moi 5 giay + mot dong khi dung day.

**Da dat 17:47:** `CoreClient.dll.moi` **43ba6ef9** (⊇ ae11479a dang chay, chi them 3 nhan log). Chi swap client.

### 8.22 [GUI] Heaven.dll: bỏ khoá chung khỏi đường gửi + bộ đếm + phép thử nhân bản (19:00–19:25, phiên tiếp theo)

**Trạng thái lúc vào phiên (19:01):** `CoreClient.dll` 43ba6ef9 (vá m) đã live từ 17:48. `CoreServer.dll` live là **7361e2dd** = bản
XEPHANG build từ origin/main sau 02eb802d (⊇ DELTA l — kiểm đủ chuỗi `NS-BO/NS-TT/PS-BO/DMG-GON/BC-DEM/BC-LOAI` trong nhị phân),
swap 18:05 cùng `GameServer.exe` d07555ad; `bin\multiserver\Goddess.exe.moi` (XEPHANG) chờ chủ đổi tay. Chưa có lần chết nào sau
swap client (0 dòng `[S7-*]`) → vá m chưa có số. Git: delta-0709 = origin/main (fast-forward), không phải merge tay.

**Rep3 (mục 4.1 bàn giao gọn) — trần đã hết kẹp:** `[REP3] cache texture ... ngan sach 1500 MB` từ 17:48 và 18:58. Từ 18:58 tới
19:21 cache texture 238 → 402 MB, **đỉnh 685 MB** (trước kẹt 508–511/512) trong Tống Kim trận 500. Số mục bỏ ~31/s đều
(`bo` là cộng dồn: 36.376 → 39.184 trong 90 s). Đọc lại `TextureResMgr::CheckBalance` (bản [REP3 03/09 SOC], Represent3.dll live
04/09 12:17 build sau commit 695db480 03/09): chỉ **một** chế độ — mỗi lượt bỏ đúng **1 khung** của **1** tài nguyên nghỉ > 10 s; khi
**vượt ngân sách** thì ngoài nhịp định kỳ, **mỗi lần nạp ảnh** gọi thêm CheckBalance (dòng 457) → tại trần, hiệu ứng nghỉ > 10 s bị bào
khung nhanh gấp bội rồi nạp lại không kịp. (Mô tả "8 khung / nghỉ 1 s" ở 8.21 là chế độ `bOver` CŨ đã bị bỏ trong commit 03/09.)
Còn phải chờ chủ xác nhận có còn mất hiệu ứng khi đánh không.

**Việc làm: 4.3 bước 1 + bước 2, gộp trong Heaven.dll** (commit **9538e72c** origin/main, script tái áp
`ReverseTools/goi_va_gui1_khoarieng_nhanban_0709.py`, chỉ `Sources/MultiServer/Heaven/ServerStage.cpp/.h`, thuần ASCII CRLF, không đổi
giao diện `IServer` nên **không cần build GameServer.exe**):

1. **Bảng node cố định** `m_ppNode[]`: cấp trong ctor, không bao giờ đổi (node chỉ huỷ ở dtor) → tra node không cần khoá.
2. **Đường gửi** (`PackDataToClient`, `SendPackToClient`, `SendData`) chỉ giữ `csWriteAction`; **đường nhận** (`GetPackFromClient`,
   `ReadCompleted`, `ProcessCommand`, `ProcessMessage`) chỉ giữ `csReadAction`. `pSocket` được đọc **và dùng** dưới khoá riêng.
3. Nơi **đặt** `pSocket` (`_HelperAddClient`: khoá mã + xoá đệm trước, `pSocket` sau cùng) và nơi **tha** (`_HelperDelClient`,
   `ShutdownClient`) lấy `m_csCM` + **cả hai** khoá riêng → không luồng nào còn giữ socket lúc `SAFE_RELEASE`. Thứ tự khoá toàn cục
   `m_csCM < csWriteAction < csReadAction`, không chỗ nào đảo chiều (`ProcessCommand` lấy lại `csReadAction` từ `GetPackFromClient` là
   đệ quy, CRITICAL_SECTION cho phép).
4. `SendPackToClient(-1)`: chụp danh sách client dưới `m_csCM` (vài µs, ≤ 1.510 phần tử) rồi xả từng client; chỉ `Allocate()` khi
   thật sự có dữ liệu (trước: cấp + tha một bộ đệm cho MỌI client mỗi nhịp dù rỗng). `ReadCompleted` với `dwIndex = -1` (kết nối bị
   từ chối vì hết chỗ) không còn chèn mục NULL vào `std::map`.
5. **Bộ đếm `[GUI-DO]`** mỗi 10 s vào `bin\server\jx_gui_server.log` (mở-ghi-đóng từng dòng, không giữ handle → không cản xoay tệp
   của Core; ~500 B/10 s): `goi` (số PackDataToClient tới client thật), `dem day gui ngay`, `byte`, `t_goi` (ms cộng dồn), `xa` (số nhịp),
   `client_xa`, `byte_xa` (KB/s), `t_xa`, `Write N lan, X us/lan` (chỉ là PostQueuedCompletionStatus — WSASend chạy ở luồng IOCP nên
   luồng chính đo được gần trọn), `moi nhip: goi/xa (max)`, `khoa_rieng`, `client`. Dòng khởi động in `client toi da`, `dem ghi`.
6. **Phép thử nhân bản** `[Server] MoPhongNhanBan=N` (**đọc lại mỗi 10 s — đổi số rồi lưu là có hiệu lực, không cần khởi động lại**, kẹp
   0..2000): mỗi lần gửi thật làm thêm N−1 lần **y hệt** vào node giả (khoá chung nếu chế độ cũ → tra node → khoá riêng → kiểm đệm đầy →
   memcpy; đệm đầy thì `Allocate` + đầu gói + mã hoá KSG + `Release`), mỗi nhịp xả N−1 node giả như thật, chỉ **không WSASend**.
   `[GUI-NB]` in `goi/t`, `xa/byte/t`, `moi nhip goi (max) + xa (max)` và **`duong gui uoc tinh moi nhip X ms (that A + gia B)`**.
   Bộ nhớ N=500: 499 × 16 KB ≈ 8 MB.
7. **Cổng lùi** `[Server] GuiKhoaRieng=0` (đọc lúc khởi động) → mọi đường nóng lấy lại `m_csCM` trước khoá riêng, đúng thứ tự cũ.

**Bản và cấu hình:** `bin\server\heaven.dll.moi` **9fc84e88** (1.145.856 B, live 096fdeb2 từ 00:50; `chk2` thiếu 0 chuỗi, thêm 8 chuỗi
GUI). `bat` `ChayGameServer.bat` đã sẵn `call :capnhat heaven.dll` → swap cùng lượt với Goddess (đổi tay) lần khởi động tới.
`bin\server\config.ini` đã thêm mục `[Server]` với `GuiKhoaRieng=1`, `MoPhongNhanBan=0` (bản lưu `config.ini.truoc_gui`); các khoá
DELTA khác vẫn mặc định trong mã.

**Số nền ngay trước swap (19:22, Tống Kim trận 500, chủ CaiBang đứng chỗ thưa):** TICK 9,17 ms TB / max 24,7 / p95 12 (16,5 %
ngân sách); BroadCast 2,25–2,42 triệu/10 s, node duyệt 2,9–3,2 triệu/10 s, gửi thật 266–2.760/10 s; client nhận 2–6 KB/s.

**Cách đo sau swap (theo thứ tự):**

```bash
grep -a "GUI-DO" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/jx_gui_server.log" | tail -5
```

1. Dòng khởi động phải có `GuiKhoaRieng=1`. Khi chủ đánh: ghi lại `moi nhip: goi X ms + xa Y ms` — đó là chi phí đường gửi thật hôm nay
   (dự kiến dưới 0,2 ms/nhịp).
2. Lúc chủ đang ở đám đông, sửa `MoPhongNhanBan=500` trong `bin\server\config.ini`, chờ 10–20 s, đọc dòng `[GUI-NB]`; lấy
   **`duong gui uoc tinh moi nhip`** và `max`. Xong đặt lại `0`. Đọc theo bảng:

| `duong gui uoc tinh moi nhip` | Nghĩa | Bước tiếp |
|---|---|---|
| < 15 ms (cộng TICK 9 ms vẫn < 55) | luồng chính gánh được 500 người | không cần luồng gửi riêng; còn lại là băng thông ra 80–240 Mbps |
| 15–30 ms | sát ngưỡng khi có đỉnh | đưa phần **xả** (Allocate + mã hoá + Write) sang luồng riêng, phần `goi` (memcpy) giữ luồng chính |
| > 30 ms hoặc `max` > 40 | vượt | luồng gửi riêng **và** hỏi chủ về thưa đồng bộ vị trí theo khoảng cách (bước 3) |

3. Kiểm hồi quy khoá riêng: client vào/ra vài lần, `grep -a -c "Net Msg Error" jx_auto.log` phải là 0, `jx_crash.log` không có sập,
   `GameServer` console không in `Socket is closed`/`Unexpected exception`. Nếu nghi ngờ khoá riêng: `GuiKhoaRieng=0` + khởi động lại
   là về đúng hành vi cũ, không cần đổi nhị phân.

**Bẫy phiên này:** (a) Git Bash đổi `/p:` của MSBuild thành đường dẫn (`MSB1008 Only one project`) → dùng `-p:`/`-t:` và
`MSYS_NO_PATHCONV=1`; (b) `Heaven.vcxproj` Release|x64 có post-build `copy x64\release\heaven.dll ..\..\..\..in\server\` — vào `bin`
của **cây build**, không phải cây live, nhưng vẫn tắt bằng `-p:PostBuildEventUseInBuild=false`; (c) Heaven link
`Lib\release64\common.lib` (Common post-build chép sang), Common chưa đổi từ 00:31 nên không cần build lại.

### 8.23 Phép thử nhân bản ×500 chạy THẬT (20:27:21–20:29:01, Tống Kim, heaven.dll 9fc84e88 live từ 20:23)

Chủ khởi động lại cụm 20:23 (`ChayGameServer.bat` nuốt `heaven.dll.moi`; Goddess **không** đổi tay nên bảng xếp hạng vẫn trống).
Tiến trình mới pid 53112, `[GUI-DO] khoi dong ... GuiKhoaRieng=1`, client nối bình thường, 0 `Net Msg Error`. Một tiến trình nền canh
`[GUI-DO] goi ≥ 2.500/10 s` (chủ đã vào đám đông) rồi tự đặt `MoPhongNhanBan=500` trong 100 s và trả về 0 — kích 20:27:21 khi `goi=6.433`.

| Cửa sổ 10 s | Client thật nhận | Nhân bản ×500: `goi` / t | `xa` / byte / t | Ước tính **mỗi nhịp** (max goi + max xa) |
|---|---|---|---|---|
| 20:27:21 (bắt đầu) | 268 gói, 0,6 KB/s | 133.732 / 2,3 ms | 89.820 / 2,8 MB (277 KB/s) / 9,3 ms | 0,072 ms |
| +10 s | 1.173, 4,4 KB/s | 585.327 / 9,5 ms | 89.820 / 22,6 MB / 13,6 ms | 0,135 ms |
| +20 s | 9.358, 50,6 KB/s | 4,67 triệu / 76 ms | 91.317 / 274 MB (26,7 MB/s) / 38,5 ms | 0,648 ms (1,74 + 0,62) |
| **+30 s** | **17.253, 66,1 KB/s** | **8,61 triệu / 129 ms** | 89.820 / 338 MB (**33 MB/s = 264 Mbps**) / 46 ms | **0,987 ms (1,88 + 0,73)** |
| +40 s | 8.616, 45,6 KB/s | 4,30 triệu / 67 ms | 90.319 / 238 MB / 37 ms | 0,584 ms (1,73 + 0,53) |
| +50 s | 5.702, 18,8 KB/s | 2,85 triệu / 43 ms | 90.818 / 106 MB / 23 ms | 0,377 ms (2,63 + 0,29) |
| +60 s | 7.017, 44,5 KB/s | 3,50 triệu / 57 ms | 89.820 / 227 MB / 36 ms | 0,526 ms (0,79 + 0,54) |

`xa = 89.820 = 499 × 180` nhịp: mọi node giả đều có dữ liệu mỗi nhịp, đúng như client thật (`client_xa = 180`). Gói trung bình 39 byte,
mỗi lần chép + khoá ≈ 15 ns.

**Máy chủ trong lúc thử:** TICK 7,61 ms TB (kỳ 20:27:31, max 90,3 ms một lần do SW_ACTIVATE 25,9 + LUA_CALL 19,7 + BAUCUA 12,5 —
không phải đường gửi, đường gửi max 2,6 ms), kỳ 20:28:31 8,57 ms max 23; kỳ 20:29:31 sau khi tắt 8,24 ms max 21. Tức **TICK không đổi**
khi có hay không nhân bản. Đường gửi thật (không nhân bản): 0,008–0,012 ms/nhịp, `Write` 3,3–4,7 µs/lần.

**Kết luận:**

1. Chi phí luồng chính của đường gửi cho **500 client** ở mật độ đỉnh hôm nay ≈ **1 ms/nhịp, đỉnh 2,6 ms** — dưới 5 % ngân sách 55 ms.
   Sau khi bỏ khoá chung, **luồng chính gánh được 500 người**; **không cần** đưa đường gửi sang luồng riêng, càng không cần tách map ra
   nhân CPU riêng (4.4).
2. Giới hạn của phép thử: 499 lần chép chạy gộp trong một vòng nên cache nóng hơn thực tế (bộ đệm ghi 500 × 10 KB = 5 MB vẫn nằm trong
   L3), và không đo WSASend ở luồng IOCP (500 × 18 lần/s = 9.000 WSASend/s, ~10–20 µs mỗi lần trên luồng worker ≈ 10–20 % một nhân,
   không chạm luồng chính). Kể cả nhân 3 cho cache lạnh vẫn < 3 ms/nhịp.
3. **Giới hạn còn lại là băng thông ra:** 66 KB/s/người ở đỉnh × 500 = **33 MB/s = 264 Mbps**; trung bình 20–45 KB/s → 80–180 Mbps.
   Đường 1 Gbps đủ; đường 100 Mbps thì phải thưa đồng bộ vị trí (gói 221 chiếm 61 % byte) — câu hỏi 1 cho chủ chỉ cần trả lời khi biết
   máy chủ thật đặt ở đường mạng bao nhiêu.
4. `MoPhongNhanBan` đã trả về 0 lúc 20:29:01 (`doi MoPhongNhanBan 500 -> 0`). 499 node giả (10.208 byte/node ≈ 5 MB) vẫn được giữ tới
   khi tắt máy chủ, không tốn CPU khi N = 0.

**Nằm bẹp (4.2) — lần chết đầu tiên có nhãn, 20:27:3x:** `[S7-CHET-CLI] doing=9 cdoing=7` → `[S7-REV-CLI]` sau 0,72 s → `DoStand`
→ `[S7-SAUHOISINH]` +1/+3/+6 s đều `doing=1 cdoing=1 resdoing=1 resaction=3`, khung 19/45 → 10/45 → 19/45 (hoạt ảnh đứng đang chạy),
`[S7-NAMBEP-CHAN]` = 0, `[S7-NAMBEP-LAU]` = 0. Lần này **không** nằm bẹp và gói đồng bộ đầy đủ cũng không thử áp trạng thái chết
lên chính mình. Cần thêm vài lần chết nữa mới kết luận được vá j đã chữa hay bệnh chỉ thỉnh thoảng.

**Rep3:** đỉnh 303 MB trong trận này (ngân sách 1500), `bo` cộng dồn 9.597 sau 13 phút ≈ 12/s. Chờ chủ xác nhận hiệu ứng.

### 8.24 Số đo TRỌN MỘT TRẬN Tống Kim (20:27:0x–20:57:0x, 30,2 phút, 182 cửa sổ 10 s) — chủ yêu cầu số chính xác, không kết luận vội

Điều kiện: máy chủ pid 53112 (`heaven.dll` 9fc84e88 khoá riêng, `GuiKhoaRieng=1`), 1.000 bot + chủ (CaiBang), Tống Kim trận 500,
`MoPhongNhanBan=500` bật 20:27:21–20:29:01 và 20:39:52–20:57:19 (**113/182 cửa sổ = 18,8 phút**), tắt lúc 20:57:19. Cửa sổ trận lấy theo
`t=1118820000..1120640000` (thư thưởng Tống Kim lúc 20:57:00). Log nguồn: `jx_gui_server.log`, `jx_auto_server.log` (pid 53112, chưa xoay
trong trận), `jx_perf_server.log`, client `jx_auto.log(.1)`, `jx_paint.log`, `jx_rep3.log`.

#### 8.24.1 Đường gửi thật tới client của chủ (1 kết nối), 182 cửa sổ

| Đại lượng | TB | p95 | max | Tổng cả trận |
|---|---|---|---|---|
| Gói `PackDataToClient` tới client | 778 gói/s | 1.990 gói/s | 2.691 gói/s | 1.416.403 gói |
| Byte (trước đầu gói 2 B/lần xả) | 36,8 KB/s | 66,1 KB/s | 74,1 KB/s | 70.951.007 B = 67,7 MB |
| Thời gian `PackDataToClient` mỗi nhịp | 0,003 ms | — | 0,109 ms | — |
| Thời gian `SendPackToClient` mỗi nhịp (xả 1 client) | 0,007 ms | — | 0,280 ms | — |
| `pSocket->Write` (đưa vào hàng đợi IOCP) | 4,4 µs/lần | — | 5,9 µs/lần | 180 lần/10 s = mỗi nhịp |
| Đệm 10.208 B đầy → gửi ngay giữa nhịp | — | — | 72 lần/10 s (cửa sổ 2.690 gói/s) | 233 lần |

Đối chiếu bộ đếm phát tán `[BC-DEM]`/`[BC-NGUOI]` (chỉ gói **phát vùng**, không tính gói gửi thẳng): gửi thật 891.734 gói (TB 489/s,
max 2.009/s), 12,8 KB/s TB, 59,5 KB/s max. Chênh với `[GUI-DO]` (1.416.403) là gói gửi thẳng (sát thương của chính mình, vật phẩm, chat...).

#### 8.24.2 Nhân bản ×500 (499 client giả), 113 cửa sổ — CPU luồng chính và byte ra

| Đại lượng | TB | Trung vị | p95 | max |
|---|---|---|---|---|
| **Đường gửi mỗi nhịp** (goi + xả, chia đều 180 nhịp/10 s) | **0,545 ms** | 0,473 ms | **1,262 ms** | **1,593 ms** |
| Nhịp nặng nhất trong cửa sổ (max goi + max xả) | 1,370 ms | — | 2,661 ms | **10,339 ms** (một lần, 20:46:4x) |
| Byte ra mô phỏng (499 client giả, kể đầu gói) | 17.923 KB/s = **147 Mbps** | — | 33.533 KB/s = **275 Mbps** | 46.744 KB/s = **383 Mbps** |
| Cửa sổ có đường gửi > 1 ms/nhịp | 9 / 113 | | | |
| Số lần chép (memcpy + khoá riêng) | — | — | — | max 12.425.599 / 10 s (cửa sổ 20:49:5x) |

Hai cửa sổ nặng nhất: 20:49:2x–20:49:5x (client thật 2.490 gói/s, 70,9 KB/s → ×500: 1,579 và 1,593 ms/nhịp, 46,7 và 35,9 MB/s) và
20:52:5x–20:53:2x (1,318 → 1,501 ms/nhịp, 33–35 MB/s). Tốc độ chép: 129 ms cho 8,61 triệu lần ≈ 15 ns/lần (gói TB 39 B).

#### 8.24.3 Nhịp máy chủ `[PERF] TICK` từng phút

| Khoảng | TICK TB | p95 | max từng phút |
|---|---|---|---|
| 20:29–20:39 trận này, **không** nhân bản | 8,24–9,30 ms | 11–12 ms | 19,4–53,9 ms |
| 20:40–20:57 trận này, **có** nhân bản ×500 | 8,52–10,05 ms | 11–13 ms | 19,4–33,0 ms (235 ms một lần lúc 20:57:32 = kết trận, phát thưởng) |
| 19:04–19:30 trận trước, heaven **cũ** 096fdeb2 | 8,50–9,70 ms | 11–13 ms | 20,5–45,3 ms |
| 20:27:31 phút đầu trận | 7,61 ms | 11 ms | 90,3 ms (SW_ACTIVATE 25,9 + LUA_CALL 19,7 + BAUCUA 12,5) |

SW_ACTIVATE TB 6,0–7,2 ms; LUA_CALL 0,3–0,7 ms. Không có phút nào `tre` > 1 nhịp.

#### 8.24.4 Phát tán và quyết định đồng bộ (toàn máy chủ, 182 cửa sổ)

| | Tổng cả trận | TB / 10 s | max / 10 s |
|---|---|---|---|
| Lượt phát (`BroadCast`) | 276.528.329 | 1.519.386 | 1.975.148 |
| Node duyệt | — | 6.160.039 | 8.937.432 |
| Cắt vì hết ngân sách / bỏ vị trí | 0 / 0 | | |
| Quyết định đồng bộ: bỏ / gọn / đầy đủ / gọn thêm | 23.177.951 / 30.637.572 / 145.068 / 5.972 | 168.338 gọn, 797 đầy đủ | |
| `day/(gon+day)` | **0,47 %** | | |
| Gói trạng thái 223 | 552.028 | 3.033 | |
| Số sát thương phát gọn 222 (cộng dồn từ 20:23) | 2.441.082 gọn / 0 đầy đủ | | |
| Gói ngoại hình 75 (cộng dồn từ 20:23) | gửi 28.161 / bỏ 27.232.954 (99,9 %) | | |

Thành phần byte tới client (`[BC-TOP]`, 182 cửa sổ): **221: 66,7 %** · 75: 10,7 % · 222: 7,6 % · 148: 3,9 % · 95: 3,6 % · 86: 3,4 % ·
223: 3,1 % · 77: 0,8 % · 85: 0,2 %.

#### 8.24.5 Client của chủ

| | Số đo |
|---|---|
| Chết / hồi sinh | **9 lần**, hồi sinh sau 0,71–0,77 s; `[S7-SAUHOISINH]` +3 s và +6 s **9/9 lần đứng** (`doing=1 cdoing=1 resdoing=1`); 4 dòng +1 s `cdoing=2 resdoing=2` = đang chạy (chủ bấm đi ngay) |
| `[S7-NAMBEP-CHAN]` / `[S7-NAMBEP-LAU]` | 0 / 0 |
| `Net Msg Error` / sập | 0 / 0 |
| Vẽ (`jx_paint` `[SUM]`) | 225.156 lượt vẽ (124/s), **5 khung giật** cả trận, khung lâu nhất 125 ms, cross 513 |
| Bộ nhớ ảnh (`jx_rep3`) | texture 159 → 419 MB (ngân sách 1.500), 60 mẫu; `LoadImage FAIL` **0** trong trận; bỏ 54.894 mục / 30 phút ≈ 30/s; fps TB 55–63 |
| Đồng bộ NPC (`[S6-*]`) | SYNC 20.066, CMD 3.292, GANNHANH 1.774, ME 1.698, ATK 719, ADD 415 |

#### 8.24.6 Đọc số (chỉ những gì số cho phép nói)

1. Với đúng mật độ trận này, đường gửi cho 500 client tốn của luồng chính **0,545 ms/nhịp trung bình, 1,262 ms ở p95 cửa sổ, 1,593 ms ở cửa
   sổ nặng nhất, và 10,3 ms ở đúng một nhịp** trong 18,8 phút đo. Ngân sách nhịp 55 ms, TICK đang 8–10 ms. Phần này **không mô phỏng**
   WSASend (chạy ở luồng IOCP worker, 500 × 18 = 9.000 lần/s) và tranh chấp khoá riêng từ 500 client gửi lên; 499 lần chép chạy gộp nên
   cache nóng hơn thực tế. Nhân 3 cho hai điều đó vẫn dưới 5 ms/nhịp.
2. Byte ra của máy chủ ở mật độ này với 500 người: **147 Mbps trung bình, 275 Mbps ở p95, 383 Mbps đỉnh 10 s** — đây là phép nhân
   thẳng số đo của một client, không phải giả định. Đường 1 Gbps chịu được; đường 100–200 Mbps thì không.
3. TICK khi có và không có nhân bản chênh dưới 0,8 ms, cùng dải với trận 19:00 trên heaven cũ → bỏ khoá chung **không làm chậm** gì với
   một client, và phần nhân bản đúng bằng số `[GUI-NB]` đo được.
4. Nằm bẹp: 9 lần chết, 0 lần tái hiện trên client 43ba6ef9; không có lần nào gói đồng bộ đầy đủ bị chặn (`NAMBEP-CHAN` = 0) nên chưa
   thể nói vá j là gốc — chỉ nói được là **chưa tái hiện**.
5. Mất hiệu ứng: `LoadImage FAIL` = 0, cache 159–419 MB trên 1.500 → về phía số liệu không còn dấu hiệu; cần chủ xác nhận bằng mắt.

### 8.25 Trận Tống Kim 04:21–04:51 (08/09) trên TOÀN BỘ bản mới, so với trận 20:27 (07/09) — chủ yêu cầu "kéo log so sánh"

Chủ nói "trận 3h27 sáng"; theo `[GUI-DO]` lúc 03:27 client chỉ nhận 100–250 gói/s (không có trận), trận thật của sáng nay là **04:21–04:51**
(timer `tongkim=1` 04:21, thư thưởng 04:51). Máy chủ pid 67684 khởi động 04:15 (`CoreServer.dll` **27e5415e** [FX-SV] + `heaven.dll`
9fc84e88 khoá riêng), client pid 47564 khởi động 04:10 (`CoreClient.dll` **19731ad4** [FX], `Represent3.dll` **f4c10a85** trần cache
1024 + `fx:`; chưa có Rep3Ex). 1.000 bot, Tống Kim trận 500, chủ CaiBang.

**Mất dữ liệu:** `jx_auto.log` client xoay vòng 64 MB **mỗi ~27 phút** (`[S6-SYNC]` 385.596 dòng/27 phút = 238 dòng/giây, chiếm 68 %; rồi
`DATAU-GATE` 25k, `MOVE-RESET` 25k, `S6-ME` 24k, `S6-CMD` 14k), máy chủ mỗi ~40 phút (`SPICK-WORLD/RECV/FAR` 37k mỗi nhãn, `SPICK-BELONG`
25k, `SPICK-BAG` 23k, rồi 14 nhãn `E4_*/E3_*` mỗi nhãn 5–13k). Tới 08:47 cả hai tệp chỉ còn từ 07:29/08:20 → **`[FX]`, `[FX-SV]`, `[NS-BO]`,
`[S7-*]` của trận này không còn**. Còn lại: `jx_gui_server.log` (đường gửi), `jx_perf_server.log`, `jx_rep3.log`, `jx_paint.log`, `hethong.log`.

| Đại lượng | 20:27 07/09 (heaven cũ, CoreClient vá m) | **04:21 08/09 (tất cả bản mới)** |
|---|---|---|
| Gói tới client TB / p95 / max | 778 / 1.990 / 2.691 gói/s | **1.064 / — / 4.134 gói/s** (trận đông hơn) |
| Byte tới client TB / max | 36,8 / 74,1 KB/s | **43,5 / 102,7 KB/s** |
| Đường gửi mỗi nhịp TB (goi + xả) | 0,010 ms | 0,011 ms; nhịp nặng nhất goi 0,767 + xả 1,374 ms |
| `Write` µs TB / max | 4,4 / 5,9 | 4,7 / 33,4 |
| Đệm đầy gửi ngay | 233 lần | 1.125 lần (cửa sổ 66 lần khi 2.930 gói/s) |
| TICK từng phút | 8,2–10,05 ms, max 54 | **7,9–9,05 ms, max 48** trong trận; **04:21 13,6 ms (max 128, trễ 17 nhịp)** và **04:25 18,8 ms (p95 45, max 98,6, trễ 27 nhịp)** = SW_ACTIVATE 123 / 68,8 ms lúc 500 bot vào và dàn trận |
| Client RAM riêng | 656 MB max (phiên 20:23) | 373 → **761 MB**, texture 135 → **735 MB**; hồi quy trong trận `RAM = 246 + 0,76 × (texture+raw)` |
| Vẽ | 124 lượt/s, 5 khung giật, max 125 ms | 124 lượt/s, **10 khung giật** (3 lần ≥ 100 ms, max 133), 7/10 do logic > paint |
| fps TB thấp nhất | 55 | 58 |
| `LoadImage FAIL` | 0 | 0 |
| Lớp vẽ `fx:` | — | **tex_null 0, tao_hong 0, khung_khong_tex 0** → không bỏ hình nào; giải mã đồng bộ 107.449 khung = 3,4 s cả trận = **1,8 ms mỗi giây** (~0,03 ms/khung vẽ) |
| `anh_null` | — | **37,6 triệu** = ~311 lần/khung vẽ: 200 sprite biểu cảm `\spr\Ui3\表情\140–339.spr` **không có trên đĩa lẫn pak** (thư mục GBK "表情" = biểu cảm), UI vẫn xin mỗi khung |
| Chết / nằm bẹp, `[FX]`, `[FX-SV]`, `[NS-BO]` | 9 chết, 0 nằm bẹp | **mất log** |

**Đọc số:** trận nặng hơn 37 % về gói và 39 % về byte tới client mà đường gửi vẫn ~0,01 ms/nhịp, TICK giữ 8–9 ms, lớp vẽ không bỏ hình,
fps không tụt → các phần tối ưu (khoá riêng Heaven, DELTA, trần cache 1024, bộ đếm) **không gây hồi quy**. Hai điểm mới cần làm:
1. **Hạ tần suất nhãn tràn log** ở cả hai bên (client `[S6-SYNC]`/`S6-ME`/`S6-CMD`/`MOVE-RESET`/`DATAU-GATE`, máy chủ `SPICK-*`) về
   `AUTOLOG_EVERY(1000)`, nếu không mọi bộ đếm 10 s của một trận 30 phút sẽ mất nửa đầu. Không đổi hành vi game.
2. **200 sprite biểu cảm thiếu** (`spr\Ui3\表情\140–339.spr`): hoặc bổ sung tệp, hoặc UI đừng xin mỗi khung; hiện tốn 311 lần tra
   cache mỗi khung vẽ (nhỏ nhưng vô ích) và biểu cảm không hiện.
Hai nhịp nặng 04:21/04:25 (SW_ACTIVATE tới 123 ms) là lúc 500 bot vào và dàn trận, không thuộc đường gửi — nếu chủ thấy giật lúc bắt đầu
trận thì đó là chỗ để soi (`KRegion::Activate` lúc nạp 500 NPC cùng nhịp).

### 8.26 Trận Tống Kim 09:22 08/09 — trận đầu tiên đo được `[FX]` trọn vẹn; bể đạn client 500 → 3000; D3D9Ex không giảm RAM

Máy chủ pid 22664 (CoreServer **352b207f**, hạ tần suất log), client pid 27684 (09:11–09:35, CoreClient 1fddde04, Represent3 72f81e3e
`Rep3Ex=1`) rồi pid 8916 (09:35–, CoreClient **b670be43** bể đạn 3000, `Rep3Ex=0`). Log **không còn bị xoay** (client 26 MB/19 phút).
Trận 09:22 → 09:42 (20 phút, 122 cửa sổ máy chủ), số **cả trận**.

**Hiệu ứng — máy chủ vs client trong 32 ô quanh chủ (`[FX-SV]` / `[FX]`):**

| | Máy chủ (gần người thật) | Client cũ pid 27684 (12,3 phút) | Client mới pid 8916 (6,2 phút) |
|---|---|---|---|
| Chiêu bắt đầu / bắn | gần chủ 57.501 / **54.049 (94,0 %)**; toàn máy chủ 374.451 / 327.243 (87,4 %) | rx95 36.999 → start 35.494 → **fire 32.234 (90,8 % start)** | 14.462 → 13.226 → **11.890 (89,9 %)** |
| Mất trên đường diễn hoạt (start − fire) | ngắt vì trúng đòn 1.125 | 3.260, giải thích được 1.115 (bo_tgt 115 + hurt<60 % 426 + chết 574) → **2.145 chưa rõ** | 1.336, giải thích được 496 (122 + 142 + 202 + huy_sync 30) → **840 chưa rõ** |
| Chiêu của chủ / gói 95 không có NPC | — | fail 0 / noidx 128 | fail 0 / noidx 44 |
| Gói 148 bắn thẳng | — | 54.651 rx = 54.651 cast, fail 0 | 29.485 = 29.485, fail 0 |
| **Đạn không tạo được (`add_full`)** | — | **36.127** (đỉnh 2.853/10 s): zone 17.316, circle 8.706, wall 4.401, spread 3.902, line 1.381, ext 741; ngoài vùng nạp 320 | **0** (bể 3000, mức dùng cao nhất **1.593**); ngoài vùng 5 |
| Đạn chết sớm: mất người phóng | — | 168.593 (đếm theo nhịp, phóng to) | **3.262 viên** = ô NPC trống 3.088 + mồ côi 174 + ô bị NPC khác dùng 0 (client gỡ NPC 6,9 phút: DEL máy chủ 2.532, ORPHAN 2.771, XOAXA 136; bảng NPC đỉnh 375/800) |
| Đạn mất mục tiêu bám (bay thẳng, không mất hình) | — | 52.065 | 25.566 |
| buff hết 6 ô/loại | — | 203 | 173 |

Đọc: **bể đạn 500 là gốc lớn nhất của "mất hiệu ứng khi đông"** — 36.127 lần tạo đạn hỏng trong 12 phút (máy chủ có 20.000 ô nên sát thương
vẫn tính, chỉ client không thấy); nới 3000 → 0 hỏng, cần thật tới ~1.600 ô. Tầng diễn hoạt client ra được ~90–91 % chiêu so với máy chủ 94 %
→ client mất thêm ~3–4 %, mới giải thích được 1/3 (mục tiêu mồ côi, trúng đòn trước 60 %, chết, gói đồng bộ đè); phần còn lại **chưa có bộ đếm**
(nghi: NPC bị gỡ khỏi bảng client giữa lúc thi triển — `ownerlost trong` 3.060 cho thấy gỡ NPC ~8/s; `nTotalFrame` lệch/`=1`). Đạn mất
người phóng do **ô NPC trống** (server gỡ NPC chết / XOAXA > 40 ô / ORPHAN cuộn vùng) — cần tách tiếp theo loại gỡ.

**Đường gửi / máy chủ / client:**

| | Số |
|---|---|
| Client nặng nhất nhận (`[GUI-DO]`) | TB **1.440 gói/s**, max 3.142; TB 50,4 KB/s, max 112,3 (trận nặng nhất tới nay); `[NS-BO]` gói đầy 0,4 % |
| Đường gửi mỗi nhịp | goi TB 0,010 ms (max 9,2 trong phút trễ), xả TB 0,014 (max 5,8); Write TB 8,9 µs max 39,8; 405 lần đệm đầy gửi ngay |
| Thành phần byte | 221 58,7 % · 222 14,1 % · 75 7,4 % · 148 5,5 % · 223 5,3 % · 86 4,7 % · 95 3,3 % |
| TICK máy chủ | 09:11–09:16 và 09:29→ **8 ms**; **09:17–09:28 (trừ 09:24): 20–31 ms, p95 50–77, trễ 38–145 nhịp/phút** = pha 500 bot giao chiến dày (bot.log: BotTrap 13k, BotTK 8k, Bot chết/hồi sinh 8k trong 12 phút); `[BC-DEM]`/`[NS-BO]` không đổi giữa phút nặng và phút thường → không phải đường gửi/đồng bộ, là AI + đạn bot trong `SW_ACTIVATE` (19–22 ms), cần đo pha con |
| Client mới (6,9 phút) | 9 lần chết / 18 hồi sinh, 0 nằm bẹp, 0 lỗi mạng; fps TB 62–63; `tex_null 0 tao_hong 0 khung_khong_tex 0`; `anh_null` 165–170k/30 s = 200 sprite biểu cảm thiếu tệp (mục 8.24) |
| RAM client | pid 27684 (Ex BẬT): RAM = 150 + 0,89 × texture; pid 8916 (Ex TẮT): 177 + 0,95 × texture, sau trận 688 MB với texture 571 + raw 55 MB → **D3D9Ex không bỏ được bản sao driver** (BANGIAO_RAM_CLIENT_0809.md 3.2) |

**Việc tiếp theo đã chốt từ số này:** (1) giữ bể 3000 (đã live 09:35); (2) thêm bộ đếm "NPC bị gỡ giữa thi triển" + "tới hết diễn hoạt mà chưa
bắn" để đóng nốt ~3 % chưa giải thích (2.145 + 840 chiêu); (3) tách `ownerlost trong` theo loại gỡ (DEL của server / XOAXA / ORPHAN) rồi quyết có giữ đạn bay
tiếp khi người phóng chỉ rời bảng client; (4) đo pha con trong `SW_ACTIVATE` cho pha bot giao chiến; (5) RAM: texture bảng màu 2 B/px.

---

> **PHIEN SAU DOC TRUOC:** `D:\GAMEDEVNEW\BANGIAO_PHIEN_SAU_BANGTHONG_0709.md` — ban giao gon: trang thai hien tai, duong loi chu chot, chuoi va a-m, viec dang treo theo thu tu, cach do, cach chung khe .moi, bay da dinh, ban do ma. Tep nay (8.x) la so lieu chi tiet tung dot de tra cuu.
