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
