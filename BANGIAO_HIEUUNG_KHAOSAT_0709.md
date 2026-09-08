# KHẢO SÁT: HIỆU ỨNG KỸ NĂNG MẤT KHI ĐÔNG NGƯỜI — client JX1 (07/09/2026, 21:40)

Chủ nói 21:20: *"Khảo sát đã — tôi muốn tối ưu tới mức có thể. Vì không phải là đường truyền mà hình ảnh hiệu ứng nữa."*
Bối cảnh: đường gửi đã đo trọn trận (BG băng thông 8.24), đường truyền 10 Gbps → mạng không còn là việc. Triệu chứng chủ báo 17:40:
*"đánh nhau mất hết hình ảnh kỹ năng, chỉ còn động tác đánh"*. Phiên trước đổ cho cache ảnh kẹt trần 512 MB và nâng `Rep3CacheMB=1500`;
trận 20:27–20:57 cache chỉ 159–419 MB, `LoadImage FAIL` = 0, nhưng **chưa có xác nhận của chủ** là hiệu ứng còn mất hay không.

Khảo sát bằng 3 tác tử đọc mã song song + tự kiểm lại từng dòng dùng để kết luận (đánh dấu ✓ = đã tự đọc; ○ = tác tử báo, chưa tự đọc).

---

## 0. Kết luận ngắn

1. **Không có bộ đếm nào đếm được hiệu ứng bị rớt.** Mọi nhãn hiện có trên đường chiêu/đạn (`[MSL-*]`, `[MIS-*]`, `[E3_*]`, `[E4_*]`,
   `NETSKILL-*`, `CAST-*`) đều là `AUTOLOG_EVERY(1000)` — mỗi giây in tối đa một dòng bất kể có bao nhiêu sự kiện — nên số dòng trong log
   **không phải** số lần. Riêng `[E3_MISSLES_BADLAUNCHER]` còn đặt **trước** điều kiện lỗi nên in cả khi bình thường (`KSkills.cpp:718`) ✓.
2. Trong trận 20:27–20:57 những nguyên nhân **có** đếm được đều **không** xảy ra: bể đạn 500 chưa bao giờ đầy (`[MSL-SET-FULL]` = 0), bảng
   NPC dùng tối đa 232/800, `LoadImage FAIL` = 0, cache 159–419/1.500 MB, 5 khung vẽ giật cả trận.
3. Có **9 chỗ rớt hiệu ứng im lặng** (mục 3) mà log không thấy được, cộng **3 lỗi chắc chắn** trong mã (mục 5). Chỗ khả nghi nhất khi đông
   người: chiêu bị ngắt vì trúng đòn trước khung 60 % (client và máy chủ cùng luật nhưng lệch một nhịp), mục tiêu mồ côi khi cuộn vùng, và
   texture không tạo được thì vẽ bỏ qua im lặng.
4. Theo luật chủ "đo trước, sửa sau": bước đầu là **build client có bộ đếm `[FX]` chính xác** (mục 6.1) chạy một trận, rồi mới sửa. Song song
   có 5 việc sửa chắc chắn, không đụng luật chơi (mục 6.2). Việc lớn cần chủ duyệt: gói "skill đã bắn" từ máy chủ và nạp trước ảnh chiêu (6.3).
5. ~~Cần chủ trả lời 4 câu ở mục 6.4~~ **Chủ trả lời 08/09 04:01: "chưa thấy mất hiệu ứng kỹ năng lại"** sau `Rep3CacheMB=1500`, qua 5 lượt
   Tống Kim, cache đỉnh 768 MB (mục 7.0). Trần 512 MB được coi là gốc theo hiện tượng; đã sửa trong mã 512 → 1024 + kẹp theo VRAM (mục 7.2).
   Bản đo `[FX]` vẫn đáng swap để có số chắc và để bắt các chỗ rớt khác (hurt ngắt, mồ côi, đạn) nếu có.

---

## 1. Số đã có từ trận 20:27–20:57 (client pid 25136, cả `jx_auto.log.1` + `jx_auto.log`)

| Nguyên nhân có nhãn | Đếm trong trận | Ý nghĩa |
|---|---|---|
| `[MSL-SET-FULL]` (bể đạn `MAX_MISSLE=500` hết chỗ, `KMissleSet.cpp:76`) ✓ | **0** (cả log 0) | bể đạn chưa đầy ở mật độ này |
| `[MISSLE-POOL-FULL]` (`KSkills.cpp:1246`) | 0 trong trận, 3 cả log | tên sai: `Add` trả −1 vì `Mps2Map` ra vùng chưa nạp (`m_nRegionId < 0`, `KMissleSet.cpp:85`) ✓, không phải hết bể |
| `[S6-ADD] dung=` | max **232/800** | bảng NPC client còn xa trần |
| `NETSKILL-NOIDX` (người phóng không có trong bảng) | 0 | |
| `[S6-ORPHAN]` (NPC bị gỡ vì ô vùng bị tái dùng khi cuộn map, `KRegion.cpp:1938`) ✓ | **275** | mỗi lần cuộn vùng, mọi NPC vùng đó mất `m_RegionIndex` → chiêu nhắm chúng rớt (mục 3.2), đạn của chúng chết (`MIS-ACT-NOLAUNCHER`) |
| `[S6-XOAXA]` (xoá NPC > 40 ô) / `[S6-ORPHAN-BACK]` / `SYNCMIN-REQNPC` | 34 / 32 / 61 | |
| `LoadImage FAIL` (`jx_rep3.log`) | **0** trong trận (26.600 cả tệp, từ trước) | |
| cache texture / `bo` / fps | 159–419 MB, ~30 mục/s, 55–63 | không kẹt trần, không thiếu ảnh |
| `[SUM]` `jx_paint.log` | 225.156 lượt vẽ, 5 khung giật, lâu nhất 125 ms | vẽ không nghẽn |

Nhãn rate-limit (không đếm được): `MIS-ACT-NOLAUNCHER` 93 dòng, `MSL-TGT-LOST` 93, `E3_CAST_ENTRY` 163, `E3_MISSLE_BORN` 156, `MSL-FLY-START` 93.

Bộ nhớ client: `RAM riêng` max **816 MB** cả ngày, `VRAM còn` min 3.405 MB. `Game.exe` **không có LARGEADDRESSAWARE** (đọc PE: characteristics
0x0102) → trần 2 GB không gian địa chỉ; lúc 17:2x cache kẹt 510/512 thì RAM riêng 552–565 MB, VRAM còn 3.58 GB → **không** phải hết bộ nhớ.

---

## 2. Đường đi của một chiêu của NPC khác trên client (đã tự đọc)

```
MÁY CHỦ  KNpc::DoSkill (KNpc.cpp:3018)  phát s2c_skillcast = 95 (NPC_SKILL_SYNC 25 B) cho cả vùng + 8 vùng kề
         ... 10 nhịp sau: KNpc::OnSkill (3403) → IsReachFrame(60) → KSkill::Cast → đạn máy chủ → trúng → 222 số sát thương
CLIENT   NetCommandSkill (KProtocolProcess.cpp:998): tìm nIdx người phóng, nếu MapX==-1 thì MapY là ID mục tiêu → phải có
         và m_RegionIndex >= 0 (:1027-1031) → Npc[nIdx].SendCommand(do_skill) (:1046)
   nhịp  KNpc::DoOrdinSkill: nTotalFrame = m_CastFrame*100/(castSpeed+100) (3258-3262; CastFrame=18 từ settings\...\BaseValue.ini
         cả client lẫn máy chủ ✓; castSpeed client lấy từ gói đồng bộ đầy đủ m_CSpeed, KProtocolProcess.cpp:2550) → m_Doing = do_magic
   nhịp  KNpc::OnSkill (3403): WaitForFrame() tăng khung; CHỈ khi nCurrentFrame == nTotalFrame*60/100 (IsReachFrame 6440, so BẰNG)
         → nếu m_DesX==-1 mà mục tiêu <=0 hoặc Npc[m_DesY].m_RegionIndex<0 → bỏ (3417-3421, KHÔNG nhãn)
         → pSkill->Cast(m_Index, m_DesX, m_DesY) (3430) → CastMissles → CastZone/Line/Wall/Circle/Spread → MissleSet.Add
         → KRegion::AddMissle → hiệu ứng ra đời; KMissle::Activate mỗi nhịp; KMissle::Paint → KMissleRes::Draw → Represent3
   Gói 148 s2c_castskilldirectly (cùng cấu trúc): "bắn ngay không cần diễn hoạt" — từ CastAutoSkillAt (KNpc.cpp:3723, tự phóng
         nhắm mục tiêu), Cast(id,lv) của script/AI (6302), kỹ năng kèm theo (1492, 1551). Client s2cDirectlyCastSkill (:3469)
         → Cast thẳng, không qua diễn hoạt. Máy chủ gửi 148 nhiều ngang 95 (trận 20:27: 254 vs 230 gói/10 s tới client).
```

Trúng đòn: máy chủ `KNpc::DoHurt` (2152-2174 ✓) — sau khe xác suất `g_RandPercent(nHurtI*3/4 + 60 − giảm/4)` và `IgnoreNegativeState` —
đặt `m_Doing = do_hurt`, `nCurrentFrame = 0` **ở cả hai bên** (đoạn chung ngoài `#ifdef`), rồi phát `s2c_npchurt` → client `NetCommandHurt`
(:792) làm y hệt. Tức trúng đòn **ngắt thi triển** ở cả máy chủ lẫn client (luật JX1), chiêu chưa tới khung 60 % thì **không bắn, không sát
thương, không hiệu ứng** — chỉ còn diễn hoạt dở → đúng chữ "chỉ còn động tác đánh" của chủ, và về nguyên tắc là đúng luật. Lệch chỉ xảy ra
trong **một nhịp**: gói hurt và lượt `OnSkill` của cùng nhịp xử lý theo thứ tự khác nhau ở hai bên → client huỷ nhưng máy chủ đã bắn (mất
hiệu ứng nhưng có sát thương) hoặc ngược lại. Với 250 NPC đánh nhau, đòn tới mỗi nhịp nên cửa sổ này không nhỏ — **phải đếm mới biết**.

---

## 3. Chín chỗ rớt hiệu ứng im lặng (không nhãn hoặc nhãn không đếm được)

| # | Chỗ | Điều kiện | Đã kiểm | Khả năng khi đông |
|---|---|---|---|---|
| 3.1 | `KNpc::OnSkill` không bao giờ tới `Cast` | `DoHurt`/lệnh khác đặt lại khung trước khung 60 %; máy chủ có thể đã bắn (lệch nhịp) | ✓ | **cao** |
| 3.2 | `OnSkill` 3417-3421 bỏ vì mục tiêu | `m_DesX==-1` và mục tiêu ≤ 0 hoặc `m_RegionIndex < 0` (mồ côi do cuộn vùng — 275 lần/trận) | ✓ | **cao** |
| 3.3 | `MissleSet.Add` trả −1 ở 5 kiểu cast | `CastLine :1369 continue`, `CastExtractiveLineMissle :1464`, `CastWall :1592`, `CastCircle :1719`, `CastSpread :1890` — chỉ `CastZone` có nhãn (và nhãn nói sai nguyên nhân) | ○ (đã kiểm `Add` ✓) | trung bình: −1 vì vùng chưa nạp; bể 500 đầy chưa xảy ra |
| 3.4 | Người phóng không có trong bảng client | `NetCommandSkill :1035` `nIdx <= 0` → return | ✓ | thấp (0 lần) |
| 3.5 | `nTotalFrame == 1` (tốc độ thi triển rất cao) | `WaitForFrame` đúng ngay nhịp đầu → `DoStand` trước khi `IsReachFrame(60)` = khung 0 khớp | ○ | thấp, chỉ NPC cực nhanh |
| 3.6 | Texture không tạo được → vẽ bỏ qua | `CreateTexture16Bit` void, lỗi `goto error`; `PrepareFrameData` vẫn `return true` (`TextureRes.cpp:593-597`); `DrawSpriteAlpha` `if(!pTex) continue` (`KRepresentShell3.cpp:2976-2979`); `m_nTexMemUsed +=` **trước** khi tạo (`:649`) nên khung hỏng vừa phồng cache ảo vừa giải mã lại mỗi khung | ✓ | thấp hôm nay (VRAM/RAM dư), nhưng **im lặng tuyệt đối** |
| 3.7 | Ảnh nạp hỏng thì 10 s mới thử lại | `REP3_RELOAD_COOLDOWN 10000` (`TextureResMgr.cpp:17`, `:386`) | ✓ | thấp (FAIL = 0) |
| 3.8 | Buff trên thân: 6 ô mỗi loại | `m_cStateSpr[18]` = 6 đầu/6 thân/6 chân; buff thứ 7 cùng loại **bỏ qua không nhãn** (`KNpcRes.cpp:1342`) | ✓ | trung bình (đông người nhiều buff) |
| 3.9 | Cây cảnh chỉ giữ vật thể trong ±1 vùng | `KScenePlaceC.cpp:963/1007` `INSIDE_AREA(ri,1)` — ngoài thì lá bị bỏ tới khi đạn quay lại | ✓ | thấp (ngoài màn hình) |

Khác: `Npc[]` client 800 (`KNpc.h:23`), `MAX_NPC_REQUEST 128`, `Missle[]` 500 client / 20.000 máy chủ (`KMissle.h:8-10`), một `CastZone`
cấp `ChildSkillNum²` viên (`KSkills.cpp:1224`) — 500 là trần thật khi mật độ cao hơn hôm nay.

---

## 4. Lớp vẽ Represent3 (đã tự đọc các dòng nêu)

- **Nạp ảnh đồng bộ trên luồng vẽ**, không có luồng nạp, không hàng đợi, không ngân sách thời gian. `GetImage` (`TextureResMgr.cpp:362-475`)
  giữ `m_ImageProcessLock` suốt: tìm → `LoadImage` (quét mọi .pak tuyến tính `KPakList.cpp:53-65`, giải nén UCL cả tệp) → `PrepareFrameData`
  từng khung khi cần (`SprGetFrame` + RLE→8888 + `CreateTexture` SYSTEMMEM + `LockRect` + `UpdateTexture` sang POOL_DEFAULT,
  `TextureRes.cpp:609-704`). Mỗi hiệu ứng mới xuất hiện = vài chục µs tới ~1 ms mỗi khung trên luồng vẽ → đó là nguồn 5 khung giật.
- **Thải ảnh** `CheckBalance` (`:48-85`): mỗi lượt bỏ đúng **1 khung** của **1** tài nguyên nghỉ > 10 s và không vẽ khung này; lượt định kỳ
  mỗi 25 ms **chỉ khi fps TB ≥ 25** (`KRepresentShell3.cpp:2569`); khi **vượt ngân sách** thì mỗi lần nạp ảnh gọi thêm (`:456-466`) với hai
  tật: điều kiện `tmCur <= m_tmLastCheckBalance` đúng khi bằng nhau nên cùng một mili giây gọi bao nhiêu lần bỏ bấy nhiêu, và
  `m_uTexCacheMemUsed` chỉ tính lại ở `EndProfile` nên trạng thái "vượt" kéo dài cả khung → tại trần, hiệu ứng nghỉ > 10 s bị bào hết khung
  rồi giải mã lại đồng bộ. Đây là chuyện đã xảy ra lúc 17:xx (bỏ 36–73 mục/s so với 30/s bình thường).
- Ngân sách: RAM/16 kẹp 60..512 MB (`:95-97`), `[Client] Rep3CacheMB` ghi đè (`:104`). Với tiến trình 32-bit không LAA, mỗi MB texture
  POOL_DEFAULT kéo ~0,66 MB RAM tiến trình (chú thích `:98-103`) → 1.500 MB cache ≈ 1,2–1,4 GB RAM riêng, **sát trần 2 GB**.
- `Rep3Log` mở-đóng tệp mỗi dòng (`KRepresentShell3.cpp:68-83`) — chỉ khi có sự kiện, không đáng kể.
- "raw spr" = byte RLE chưa giải nén của mọi khung của SPR nén cả tệp, **không có ngân sách riêng**, chỉ thả khi cả node bị xoá.

---

## 5. Ba lỗi chắc chắn trong mã (sửa được ngay, không đụng luật chơi)

1. **Tuỳ chọn "Giảm skill" chết.** Bảng `KUiOptions2` (`S3Client\Ui\UiCase\UiOptions2.cpp:24-31`, ini `[Options2] GiamSkill` trong
   `UserData\<acc>\UiAutoConfig.ini`) → `CoreShell.cpp:22980` ghi `Option.SetLow(LowEstMissle, ...)` nhưng **mọi** chỗ đọc dùng `LowMissle`
   (`KMissle.cpp:1120/1756/1778`, `KMissleRes.cpp:128`, `KNpcRes.cpp:387/533/684/716`) ✓ → không bao giờ giảm. Đây là đòn bẩy sẵn có cho
   máy yếu mà đang vô hiệu. (`GiamMap` cũng đặt nhầm `LowEstPlayer` = ẩn người chơi khác, không phải map.)
2. **`PrepareFrameData` trả `true` dù tạo texture hỏng** + cộng `m_nTexMemUsed` trước khi tạo (mục 3.6) → hiệu ứng biến mất im lặng và cache
   ảo phồng. Sửa: trả `false` khi `texInfo[0].pTexture == NULL` sau khi tạo, chỉ cộng bộ nhớ sau khi tạo thành công, và ghi `Rep3Log` một lần.
3. **Nhãn sai chỗ / sai tên**: `[E3_MISSLES_BADLAUNCHER]` in trước điều kiện; `[MISSLE-POOL-FULL]` báo "hết bể" khi thật ra vùng chưa nạp.

---

## 6. Đề xuất, theo thứ tự

### 6.1 Đo trước (build CoreClient.dll, chỉ client, chờ khe `.moi` trống — hiện trống)

Bộ đếm **chính xác** (cộng dồn, in `[FX] 10s:` một dòng mỗi 10 s như `[GUI-DO]`), tách NPC khác / chính mình:

| Đếm | Chỗ |
|---|---|
| chiêu nhận (95, 148) / bắt đầu diễn hoạt (`SendCommand(do_skill)`) | `NetCommandSkill`, `s2cDirectlyCastSkill` |
| tới khung 60 % và `Cast` được gọi / `Cast` trả FALSE | `OnSkill` |
| huỷ trước 60 %: vì `do_hurt` / vì lệnh khác (đi, đứng, gói đồng bộ) / vì chết | `DoHurt`, `SendCommand` khi `m_Doing==do_magic && nCurrentFrame < 60%` |
| bỏ vì mục tiêu ≤ 0 / mồ côi | `OnSkill :3417-3421` |
| `MissleSet.Add` −1: hết bể / vùng chưa nạp, theo kiểu cast | 5 chỗ ở `KSkills.cpp` + `Add` |
| đạn chết sớm: mất người phóng / mất mục tiêu | `KMissle::Activate :474/:500` (thay `AUTOLOG_EVERY` bằng đếm) |
| vẽ bỏ qua vì texture NULL / `GetImage` NULL; số khung phải giải mã đồng bộ và tổng ms giải mã mỗi khung vẽ | `DrawSpriteAlpha`, `GetImage`, `CreateTexture16Bit` |
| buff bỏ vì hết 6 ô | `KNpcRes.cpp:1342` |

Kèm mốc so sánh với máy chủ: máy chủ đếm `Cast` thật (đã bắn) của NPC trong tầm nhìn của chủ → hiệu số "máy chủ bắn − client hiện" là con
số chủ hỏi.

### 6.2 Sửa chắc chắn (cùng build 6.1, không đụng luật chơi)

1. Nối `GiamSkill` → `LowMissle` (và đặt tên lại `GiamMap`), giữ mặc định tắt.
2. `PrepareFrameData`/`CreateTexture16Bit` báo lỗi thật (mục 5.2).
3. Buff trên thân: 6 → 12 ô mỗi loại (`MAX_SKILL_STATE` là cỡ gói 223 nên **không** đổi; chỉ đổi mảng vẽ `m_cStateSpr` phía client nếu tách được).
4. Sửa hai nhãn sai (mục 5.3).
5. Nhánh vượt ngân sách của `CheckBalance`: bỏ `tmCur <=`, trừ `m_uTexCacheMemUsed` khi thả khung.

### 6.3 Tối ưu lớn (cần chủ duyệt vì đụng giao thức / kiến trúc)

| Việc | Được gì | Công |
|---|---|---|
| **Gói "skill đã bắn"** từ máy chủ ngay tại `Cast` (khung 60 %) cho người xem: client tạo hiệu ứng khi nhận, bỏ cổng 60 % cục bộ cho NPC khác | hiệu ứng xuất hiện **đúng và chỉ khi** máy chủ bắn; hết lệch nhịp hurt; ~12 B/chiêu, 250 NPC × 1/s ≈ 3 KB/s | 1 buổi, hello v4 |
| **Nạp trước ảnh chiêu** khi nhận 95: giải mã sẵn các khung đạn của skill đó trước khung 60 % | hết giật lúc hiệu ứng xuất hiện, khung đầu không thiếu | nửa buổi |
| **Luồng giải mã**: RLE→8888 ở luồng phụ, luồng vẽ chỉ `CreateTexture`/`UpdateTexture` | loại hẳn giải mã đồng bộ khỏi luồng vẽ | 1–2 ngày, cần khoá cẩn thận |
| **LARGEADDRESSAWARE** cho Game.exe | +2 GB không gian địa chỉ trên Windows 64-bit; cache 1.500 MB mới thật sự an toàn | linker flag + rà mã dùng bit dấu con trỏ |
| Nâng kẹp 512 trong mã theo VRAM còn và LAA | máy khác không phải sửa ini | nhỏ, sau LAA |

### 6.4 Câu hỏi cho chủ (chưa trả lời thì chưa nên sửa lớn)

1. Sau `Rep3CacheMB=1500` (từ 17:48), trận 20:27 **còn** mất hiệu ứng không?
2. Mất hiệu ứng của **chính mình** (bấm chiêu, thấy vung tay nhưng không thấy chiêu) hay của **người khác/bot**, hay cả hai?
3. "Mất" là **không hiện từ đầu**, hay **hiện rồi tắt giữa chừng**, hay **hiện trễ**?
4. Cho build bộ đếm `[FX]` + 5 việc sửa chắc chắn (6.2) vào một `CoreClient.dll.moi` ngay không?

---

## 7. BẢN ĐO ĐÃ BUILD (23:40) — ba khe `.moi` chờ swap

Chủ nói 21:30: *"đang test local nên chưa đo được máy chủ"* → máy chủ chạy cùng máy, bộ đếm phía máy chủ nằm trong CoreServer.dll, đo local là đủ.
Đã làm 6.1 + 3 sửa chắc chắn của 6.2 (không đổi luật chơi, không đổi giao thức), commit `[FX 07/09]` trên origin/main,
script tái áp `ReverseTools/goi_va_fx1_core_bodem_0709.py` (Core) và `goi_va_fx2_rep3_bodem_0709.py` (Represent3).

| Khe | Băm | Bao trùm bản live | Bat tự đổi |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **27e5415e** | ⊇ 7361e2dd (XEPHANG + DELTA), chỉ thêm `[FX-SV]`, `[MISSLE-ADD-FAIL]` | `ChayGameServer.bat` |
| `bin\client\CoreClient.dll.moi` | **19731ad4** | ⊇ 43ba6ef9 (vá m), thêm `[FX]`, `[MISSLE-ADD-FAIL]`, KLadder LECH CO (XEPHANG) | `ChoiGame.bat` |
| `bin\client\Represent3.dll.moi` | **f4c10a85** (thay 3728dfec) | ⊇ c4474ed5 (04/09 12:17, cùng nguồn 18e717ae), thêm `fx:` vào dòng 30 s **+ trần cache 512 → 1024 MB, kẹp ≤ ½ VRAM còn (mục 7.2)** | `ChoiGame.bat` |

### 7.0 Chủ xác nhận 08/09 04:01: *"chưa thấy mất hiệu ứng kỹ năng lại"*

Từ 17:48 07/09 (`Rep3CacheMB=1500`) tới 04:00 08/09 chủ đánh **5 lượt Tống Kim** (thư thưởng 20:57, 21:20, 21:38, 22:15, 23:20, 23:44),
cache texture đỉnh **768 MB**, RAM riêng đỉnh 789 MB, VRAM còn ≥ 3.322 MB, 0 lần chết sau 20:57, `LoadImage FAIL` đúng 200 dòng và
đều là ảnh UI `\spr\Ui3\<thư mục GBK>\140–339.spr` lúc khởi động 20:23 (thiếu tệp, không phải hiệu ứng). Trước đó (17:xx) cache kẹt 508–511/512.
→ Bằng chứng theo hiện tượng: **trần 512 MB là gốc** (cache kẹt trần → nhánh vượt ngân sách bào hết khung hiệu ứng nghỉ > 10 s → nạp lại
không kịp). Không có bộ đếm lúc 17:xx nên chưa phải chứng minh; bản `[FX]` (mục 7) sẽ nói chắc khi có trận đông tương tự.
Ba khe `.moi` **chưa được swap** tính đến 04:01 (client và máy chủ vẫn chạy từ 20:23).

### 7.2 Sửa trong mã để máy khác không phải sửa ini (commit `[FX 08/09]`, script `goi_va_fx3_rep3_nganSach_0709.py`)

- `TextureResMgr::SetBudget`: kẹp mặc định **512 → 1024 MB** (RAM riêng ≈ 240 + 0,66 × texture → ~920 MB, dưới 2 GB vì Game.exe chưa LAA).
- `TextureResMgr::CapBudgetByVram(VRAM còn)`: gọi trong `KRepresentShell3::Create` ngay sau `CreateDevice`; nếu **không** có `Rep3CacheMB`
  thì kẹp thêm ≤ ½ VRAM còn (texture ở POOL_DEFAULT = VRAM), ghi `[REP3] cache texture: VRAM con X -> kep ngan sach A -> B`. Máy chủ vẫn giữ
  ini 1500 nên trên máy chủ không đổi gì.
- Còn lại (chưa làm, cần chủ quyết): **LARGEADDRESSAWARE cho Game.exe** để 1500 MB thật sự an toàn (+2 GB địa chỉ trên Windows 64-bit;
  phải rà mã dùng bit dấu con trỏ; là cờ linker của `S3Client.vcxproj`, đụng khe `Game.exe.moi` của phiên UITOADO).

**Bẫy build Represent3:** gói NuGet `Sources\packages\Microsoft.DXSDK.D3DX.9.29.952.8` trong git **thiếu** `build\native\release\` và `debug\`
(bị `.gitignore` dòng 46 `Release/` nuốt) → `LNK1181 d3dx9.lib`. Chép hai thư mục đó từ `C:\Users\nguye\.nuget\packages\microsoft.dxsdk.d3dx\9.29.952.8\build\native\`
vào là link được (đã chép trong worktree delta, không commit được vì bị ignore).

### 7.1 Đọc gì sau khi chủ đánh một trận (đúng luật "đo trước")

```bash
grep -a "\[FX\] 10s" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_auto.log" | tail -20
grep -a "\[FX-SV\]" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/jx_auto_server.log" | tail -20
grep -a "fx:" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_rep3.log" | tail -10
```

Cách suy: với NPC KHÁC, mỗi 10 s: `rx95` (máy chủ báo bắt đầu) → `start` (client ra lệnh) → `fire` (client tới khung 60 %, tạo đạn).
Hiệu số `start − fire − fire_fail` = số chiêu **mất trên đường diễn hoạt**, phải bằng `bo_tgt + huy_hurt(truoc60) + huy_lenh(*) + huy_chet`
(±1 vì lệch cửa sổ). Đối chiếu máy chủ: `[FX-SV] ban` (gần người thật) so với client `fire`: **máy chủ bắn mà client không bắn** = số
hiệu ứng mất thật sự; ngược lại là hiệu ứng "ảo". `add_*`/`kieu(*)` > 0 là đạn không tạo được dù đã tới khung 60 %. `chet_som` là đạn
đã tạo rồi biến mất. Phía vẽ (`jx_rep3.log`): `tex_null`/`anh_null`/`tao_hong`/`khung_khong_tex` > 0 là lớp vẽ bỏ hình; `giai_ma N khung X ms`
là chi phí giải mã đồng bộ mỗi 30 s (chia 30 × fps để ra ms mỗi khung vẽ).

Từ đó chọn cách sửa: `huy_hurt/huy_lenh` lớn → gói "skill đã bắn" từ máy chủ (6.3); `bo_tgt` lớn → giữ mục tiêu mồ côi lâu hơn khi cuộn vùng;
`add_vung` lớn → đạn nhắm ngoài vùng đã nạp (bình thường nếu ngoài màn hình); `buff_het_o` > 0 → nới 6 ô; `tex_null/tao_hong` > 0 → lớp vẽ.

### 7.3 KẾT QUẢ ĐO CẢ TRẬN — Tống Kim 09:22–09:42 08/09 (số đầy đủ ở BANGIAO_BANGTHONG_DONGNGUOI_0609.md 8.26)

Máy chủ CoreServer 352b207f (pid 22664); client pid 27684 (CoreClient 1fddde04, bể đạn 500, 12,2 phút) rồi pid 8916 (CoreClient **b670be43**,
bể đạn **3000**, 6,9 phút tới hết trận). Log không còn bị xoay. Đọc theo công thức mục 7.0.

| Tầng | Số đo | Kết luận |
|---|---|---|
| Máy chủ, 32 ô quanh chủ (`[FX-SV]`) | bắt đầu 57.501 → bắn **54.049 (94,0 %)**; toàn máy chủ 374.451 → 327.243 (87,4 %); ngắt vì trúng đòn 1.125 | 6 % chiêu bị huỷ ngay trên máy chủ (trúng đòn/chết) = luật, client không thể hiện hơn 94 % |
| Client nhận 95 → bắt đầu | 36.999 → 35.494 (noidx 128) / 14.462 → 13.226 (noidx 44) | ~4 % gói 95 tới khi NPC không có trong bảng client (`noidx`) hoặc bị đè ngay — cần đếm lý do `start < rx95` |
| Client bắt đầu → bắn (`fire`) | 32.234 / 35.494 = **90,8 %** ; 11.890 / 13.226 = **89,9 %** | client mất thêm 3–4 % so với máy chủ; giải thích được 1/3: bo_tgt 115/122, hurt < 60 % 426/142, chết 574/202, huy_sync 30; **2.145 + 840 chiêu chưa rõ** |
| Chiêu của chủ, gói 148 | fire_fail 0; 148 rx = cast (54.651 / 29.485), fail 0 | tầng "bắn thẳng" không mất |
| **Bể đạn** (`add_full`) | pid 27684: **36.127 lần Add hỏng** (đỉnh 2.853/10 s: zone 17.316, circle 8.706, wall 4.401, spread 3.902, line 1.381, ext 741); pid 8916: **0**, mức dùng cao nhất **1.593/3000** | **GỐC LỚN NHẤT của "mất hiệu ứng khi đông"** — máy chủ 20.000 ô nên sát thương vẫn tính, chỉ client không thấy. Nới 3000 giải quyết hết, còn dư gấp đôi |
| Đạn chết sớm vì mất người phóng | pid 8916: **3.262 viên** = ô NPC trống 3.088 + mồ côi 174 (client gỡ NPC 6,9 phút: DEL 2.532, ORPHAN 2.771, XOAXA 136; bảng NPC đỉnh 375/800) | ~8 viên/s tan sớm vì NPC phóng rời bảng client (ORPHAN cuộn vùng ≈ DEL máy chủ) — bước tiếp: đếm theo loại gỡ, cân nhắc cho đạn bay nốt khi chỉ client gỡ |
| Đạn mất mục tiêu bám | 52.065 / 25.566 | không mất hình, đạn bay thẳng — luật gốc |
| buff hết 6 ô/loại | 203 / 173 | hiếm, chờ chủ nếu muốn nới `m_cStateSpr` |
| Vẽ (`jx_rep3.log`) | `tex_null 0 tao_hong 0 khung_khong_tex 0`; giải mã 12–393 khung/30 s (1–14 ms); fps 62–63; `anh_null` 165–170k/30 s = 200 sprite biểu cảm thiếu tệp | lớp vẽ không rớt; D3D9Ex (`Rep3Ex=1`) không giảm RAM → đã tắt |

**Đã chốt:** giữ `MAX_MISSLE` client 3000 (live 09:35, +15 MB tĩnh). **Còn lại theo thứ tự lợi:** (1) bộ đếm "NPC bị gỡ giữa thi triển" +
"hết diễn hoạt chưa bắn" + lý do `start < rx95` để đóng 3 % chưa rõ; (2) tách `ownerlost trong` theo DEL/ORPHAN/XOAXA; (3) `noidx`;
(4) 200 sprite biểu cảm thiếu (`spr/Ui3/<GBK>/140–339.spr`) — mỗi khung vẽ xin lại ~300 lần.

## 8. Bản đồ mã

| Việc | Tệp và chỗ |
|---|---|
| Nhận gói chiêu 95 / 148 | `Sources/Core/Src/KProtocolProcess.cpp:998` `NetCommandSkill`, `:3469` `s2cDirectlyCastSkill`; đăng ký `:250`, `:303` |
| Diễn hoạt và bắn | `Sources/Core/Src/KNpc.cpp` `DoOrdinSkill` 3134-3275, `OnSkill` 3403-3452, `IsReachFrame` 6440, `DoHurt` 2100-2185 |
| Tạo đạn | `Sources/Core/Src/KSkills.cpp` `Cast` 413, `CastMissles` 693, 5 kiểu 1199-1890; `KMissleSet.cpp:67` `Add` |
| Đạn sống/chết, vẽ | `Sources/Core/Src/KMissle.cpp` `Activate` 465, `Paint` 1110; `KMissleRes.cpp:114` `Draw` |
| Buff trên thân | `Sources/Core/Src/KNpcRes.cpp` 1287 `SetState`, 1330-1360 cấp ô, vẽ 387/533/684/716 |
| Cache ảnh | `Sources/Represent/Represent3/TextureResMgr.cpp` (`GetImage` 362, `CheckBalance` 48, ngân sách 87-107, kích vượt 456), `TextureRes.cpp` (`PrepareFrameData` 550, `CreateTexture16Bit` 609) |
| Vẽ sprite | `Sources/Represent/Represent3/KRepresentShell3.cpp` `DrawPrimitives` 815, `DrawSpriteAlpha` ~2900-2984, thống kê 2560-2590 |
| Tuỳ chọn giảm | `Sources/Core/Src/KOption.h`, `CoreShell.cpp:22960-22982`, `Sources/S3Client/Ui/UiCase/UiOptions2.cpp` |
| Máy chủ phát chiêu | `Sources/Core/Src/KNpc.cpp` `DoSkill` 3018-3070, `CastAutoSkillAt` 3723, `Cast(id,lv)` ~6290 |
| Log | `jx_auto.log` (client, xoay 64 MB — trận 20:27 nằm ở cả `.log.1` và `.log`), `jx_rep3.log`, `jx_paint.log` |

## 7.4 08/09 15:3x — [HUSK] HIỆU ỨNG THEO SỰ KIỆN MÁY CHỦ (chủ: "làm hiệu ứng theo sự kiện máy chủ để hết 3 % mất chiêu")

**Cách làm (đúng thiết kế PHUONGAN_KIENTRUC_HIEUUNG_RAM_0809.md 1.2, bước A1):**
- Gói mới `s2c_skillfired = 224` (`S2C_SKILL_FIRED` 16 byte: ProtocolType, dwID người phát, WORD skill, BYTE cấp, nMpsX, nMpsY;
  nMpsX = −1 → nMpsY = dwID mục tiêu, như gói 148). Máy chủ phát đúng lúc `KSkill::Cast` thật sự chạy ở 6 điểm bắn theo hoạt ảnh:
  `OnSkill` (khung 60 %), `OnSpecial1` (chiêu con), `DoBlurMove` (chiêu con tại điểm đến), `OnManyAttack`, `OnRunAttack`, `OnJumpAttack`
  (2 điểm `Cast(id,lv)`/`CastAutoSkillAt` vẫn dùng 148 như cũ). Phát cho vùng hiện tại + 8 vùng kề như gói 95.
- Client (`KProtocolProcess::s2cSkillFired`): NPC **khác** → `pSkill->Cast` ngay, không phụ thuộc client đang ở khung nào / đang bận /
  hồi chiêu / lệch nhịp; **chính mình** → bỏ qua (vẫn tự mô phỏng, cảm giác không đổi). Ở 6 điểm bắn, client **không** tự `Cast`
  cho NPC khác nữa (macro `HUSK_CAST`, đếm `bo_cuc`).
- [b] An toàn khi lệch phiên bản: client chỉ ngừng tự mô phỏng **sau khi đã nhận gói 224 đầu tiên** từ máy chủ đang nối
  (`g_nHUSK_daNhan`, đặt lại 0 khi gửi hello). Máy chủ cũ / chưa restart / `[Server] HieuUngSuKien=0` → client chạy y như cũ,
  không bao giờ mất hiệu ứng. Hello phiên bản 4; máy chủ chỉ phát 224 khi **không còn** client nào báo < 4 (client cũ không hiểu mã 224
  sẽ hỏng luồng gói).
- Cổng lùi: `[Client] HieuUngSuKien=0` (client báo hello 3 → máy chủ không phát cho ai), `[Server] HieuUngSuKien=0` (đọc lại mỗi 10 s).
- Bộ đếm: dòng `[FX] 10s ... | HUSK(224): rx ve hong noidx minh noskill notgt bo_cuc` (client, `jx_auto.log`);
  `[FX-SV] ... | goi 224 da ban: phat=N bo_vi_client_cu=N` (máy chủ). Kỳ vọng: `ve` ≈ số chiêu máy chủ `ban` gần chủ; `bo_cuc` = số
  lần client đã giao cho máy chủ (trước đây là `fire` KHÁC).

**Bản build:** `CoreServer.dll.moi` 45bbadfb (bin\server), `CoreClient.dll.moi` (bin\client, bản [b]), `Game.exe.moi` c7b6a21f
(Game.exe phải build lại vì [RAMTINH] đổi bố cục `KNpc`/`KSubWorld`, xem BANGIAO_RAM_CLIENT_0809.md mục 7). Commit 6707507c (+[b]).
Thứ tự swap: client (ChoiGame.bat) trước hay máy chủ (ChayGameServer.bat) trước đều an toàn nhờ [b]; **hiệu ứng theo sự kiện chỉ chạy
sau khi máy chủ đã restart**.

**Đọc sau một trận:** `grep -a "\[FX\] 10s" bin\client\jx_auto.log | tail` — cột `HUSK(224)` phải có `rx` > 0 và `ve` ≈ `rx`;
`hong`/`noidx`/`notgt` là phần còn lại (người phóng / mục tiêu không có trong bảng NPC client — cùng loại với `bo_tgt` cũ).
