# BÀN GIAO — WAuto Ác chính: ác phụ sang map ác chính CHỈ BẰNG THẦN HÀNH PHÙ (17/09/2026)

> Chủ game: *"cần bạn đọc WAuto phần Ác chính - cần fix lại cho nick phụ đi theo ác chính phải dùng
> thần hành phù để di chuyển theo sau - không cần di chuyển theo tới xa phu - chỉ cần dùng thần hành phù"*.

Tiếp nối `BANGIAO_WAUTO_ACCHINH_0309.md` (đợt 1) và bộ vá 04/09 (`goi_va_wauto_acchinh_sangmap_0409.py`,
`goi_va_wauto_ac_waypoint_0409.py` — hai đường Xa Phu nay bị bỏ).

---

## 1. Trạng thái — CHỜ SWAP, hai tệp `.moi` ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`

| Tệp | md5 | sha256[:8] | cỡ (byte) | Nội dung |
|---|---|---|---|---|
| `CoreClient.dll.moi` | `7f0577a7` | `c666aa5c` | 2.679.296 | máy Ác chính sang map bằng Thần Hành Phù (mục 3) — **tập cha** của bản đang chạy `676d452f` (mục 6) |
| `WAuto.exe.moi` | `3293885f` | `86ef6813` | 467.968 | chỉ đổi chữ: nhãn ô *"Ác chính ở map khác thì dùng Thần Hành Phù sang"*, tooltip, note tab Ác chính |

Bản đang chạy: `CoreClient.dll` `676d452f` (16/09 09:20, dựng từ nhánh `claude/wauto-auto-horse-bugs-ad45c9`) ·
`WAuto.exe` `b2d93a37` (07/09) · `Game.exe` 17/09 08:56 (GIOIHAN, **không đụng**).

### Cách đổi

1. Thoát hẳn `Game.exe` **và** `WAuto.exe` (bat đổi tên `WAuto.exe`, đang mở thì báo lỗi và không mở game).
2. Chạy `bin\client\ChoiGame.bat` — đổi cả hai `.moi`.
3. `autoData` **không đổi** nên hai tệp không bắt buộc lên cùng lúc; WAuto cũ + CoreClient mới vẫn chạy đúng
   (chỉ nhãn ô còn ghi "qua Xa Phu"). Cấu hình `APdata\<ID>.dat` giữ nguyên, ô *Ác chính ở map khác…*
   (`bAcChinhVaoMap`, mặc định BẬT từ 04/09) vẫn là công tắc của tính năng.
4. Lùi: `.truoc` của cả hai.

**Chưa test sống** (cần hai tài khoản đăng nhập). Cách kiểm ở mục 5.

---

## 2. Trước / sau

```
TRƯỚC (04/09) — ác chính ở map khác, ô "Ác chính ở map khác thì tự qua Xa Phu sang" bật:
   map ác chính là thành thị  -> LD_DiThanh : có phù thì dùng phù, 12 s chưa tới -> CHẠY BỘ TỚI XA PHU
                                              "Những thành thị đã đi qua" -> tên thành (tốn tiền xe)
   map ác chính là bãi/động   -> AC_DiWayPoint: CHẠY BỘ TỚI XA PHU -> "Những nơi đã đi qua" -> tên map
   đang đứng ở map không có Xa Phu -> dùng PHÙ VỀ THÀNH trước rồi mới đi tiếp
   -> ác phụ bỏ bãi, chạy về Xa Phu, mất 1–3 phút, tốn tiền

SAU (17/09) — AC_DiThanHanh, CHỈ Thần Hành Phù (6/1/1271), không có bước Xa Phu nào:
   (a) bãi luyện công (31 map)  -> gửi lệnh c2sdnmbr_movemapid -> máy chủ chạy GotoMapId(map) -> nhảy ngay
   (b) 7 thành thị              -> dùng phù -> "Sử dụng thuật thần hành…" -> "Thành thị" -> tên thành -> cửa đầu
   (c) 4 thôn trấn              -> dùng phù -> "Sử dụng thuật thần hành…" -> "Thôn trang" -> tên thôn
   tới map ác chính -> AC_DiTheo đi bộ nốt đoạn trong map (như cũ)
```

---

## 3. Cơ chế (CoreShell.cpp, `AC_DiThanHanh` + khối "map khác" trong `AC_Process`)

### 3.1 Ba đường, chọn theo map ác chính đang đứng (`pAp->nACMap`)

| Nhóm | Map | Cách đi | Nguồn |
|---|---|---|---|
| (a) Bãi luyện công | 31 map `g_GoMapID[]` = `tab_lv20map…tab_lv90map` của `shenxingfu.lua` (875, 322, 321, 75, 225–227, 336, 340, 144, 93, 124, 152, 224, 198, 320, 181, 319, 123, 206, 79, 56, 166, 182, 164, 21, 167, 193, 170, 19, 7) | gửi `c2sdnmbr_movemapid` + id map, máy chủ gọi `GotoMapId(map)` → `gopos_step3lvXX` → `NewWorld`. **Không cần thoại.** Máy chủ tự kiểm phù trong túi, **cấp tối thiểu của bãi** (lv90 cần ≥ 90…) và map đang đứng có cấm phù không. | y hệt "Đi map luyện công" của tab Hậu cần (`nHomeStep == 10`) |
| (b) Thành thị | 1 Phượng Tường, 11 Thành Đô, 162 Đại Lý, 37 Biện Kinh, 78 Tương Dương, 80 Dương Châu, 176 Lâm An | `AutoUseItem(6,1,1271)` → thoại: *"Sử dụng thuật thần hành đi đến nơi chỉ định"* → *"Thành thị"* → tên thành → mục đầu mang tên thành (*Trung Tâm*; Lâm An = *Lâm An Nam*) | chuỗi menu chép từ `DTP_CITYHOP` (r5) đã chạy thật |
| (c) Thôn trấn | 53 Ba Lăng, 20 Giang Tân, 121 Long Môn, 54 Nam Nhạc | như (b) nhưng chọn *"Thôn trang"* → tên thôn | `THON_ARRAY` của script |

Map ngoài 42 map trên (thôn không có trong menu phù: 99/100/101/153/174, map sự kiện, Hoa Sơn…) → **không đi**,
báo *"Ac chính đang ở map N - Thần Hành Phù không có mục tới map này nên không tự sang được."* một lần / 60 s.

### 3.2 Luật một lượt

- Bắt đầu lượt: phải có phù trong túi/ô dùng nhanh (`TK_DemThanHanhPhu`), không thì báo *"Không có Thần Hành Phù
  trong túi…"* (một lần / 60 s), thả máy, nhịp sau kiểm lại (mua/nhặt được phù là đi ngay).
- (a): gửi lệnh, 4 s chưa đổi map thì gửi lại, tối đa 3 lần. (b)/(c): dùng phù, 4 s không ra thoại mới thì dùng
  lại, tối đa 3 lần; thoại nào không phải menu phù thì đóng (`GDCNI_UI_ACT`) để khỏi kẹt.
- Hết 3 lần hoặc quá 60 s → **lượt hỏng**: báo *"Dùng Thần Hành Phù sang map ác chính không được (map đang đứng
  cấm phù / chưa đủ cấp / thoại không ra) - nghỉ 1 phút rồi thử lại."*, thả máy (auto thường chạy tại chỗ),
  60 s sau tự thử lại. Không bao giờ quay vòng gửi lệnh liên tục.
- Đang chết → im lặng, không gửi gì; hồi sinh xong tiếp tục lượt.
- Ác chính đổi map giữa chừng → làm lại lượt với map mới. Về cùng map / mất ác chính → xoá trạng thái lượt.
- Trong lúc đi máy giữ `nACHold = 1` (như cũ): không đánh, không nhặt, không Dã Tẩu / Hậu cần.

### 3.3 Ô "Trong thành" (`bAcChinhThanh`) — THÊM một luật

Từ 04/09, ác chính đứng ở **thành** (map có Xa Phu) mà ô *Trong thành* tắt thì ác phụ vẫn bị kéo sang thành rồi
mới đứng im — trái với mục đích của ô đó (*"tránh cả bầy chạy theo khi ác chính về thành"*). Nay: ô *Trong thành*
tắt → ác chính ở thành thì **không sang** (cùng luật với trường hợp cùng map). Muốn ác phụ theo cả vào thành thì
tick *Trong thành*. (Với bãi luyện công / thôn không có Xa Phu như Nam Nhạc thì luật này không chặn.)

### 3.4 Đã gỡ

`AC_DiWayPoint` (04/09) — không còn ai gọi. `LD_DiThanh` vẫn còn vì Tống Kim / Liên đấu / Công Thành dùng.
`KWayPointTables.h` giữ nguyên (chưa ai dùng, có thể cần sau).

### 3.5 Trạng thái mới trong `ExtAuto` (KPlayer.h, chỉ client)

`nACThp` (0 chưa bắt đầu / 1 đã gửi movemapid / 10 đã dùng phù, đang lần thoại / <0 = −lý do hỏng, nghỉ tới
`uACThpHan`), `nACThpTry`, `nACThpMap`, `uACThpT`, `uACThpHan`, `uACDlgSeen`. `autoData` (IPC WAuto) **không đổi**.

---

## 4. WAuto (chỉ chữ)

- `.rc`: ô 644 `IDC_CHECKBOX_16_VAOMAP`: *"Ác chính ở map khác thì tự qua Xa Phu sang"* → *"…thì dùng Thần Hành Phù sang"*.
- Tooltip ô đó + note tab Ác chính (mục 6 mới) ghi rõ: cần phù trong túi, sang được 7 thành / 4 thôn / 31 bãi, map khác
  hoặc hết phù thì dòng trạng thái báo, ô *Trong thành* tắt thì ác chính về thành cũng không sang.
- Nguồn `E:\Src_Auto_Ngoai\WAuto\WAuto` + mirror `WAutoUI\` trong worktree (cùng bộ vá).

---

## 5. Cách kiểm sau swap

1. Hai cửa sổ, ác phụ tick *Tìm ác chính* + *Ác chính ở map khác thì dùng Thần Hành Phù sang*, túi ác phụ có Thần Hành Phù.
2. Ác chính dùng phù sang **bãi luyện công** (vd Khỏa Lang động 75, ác phụ ≥ 90): ác phụ phải nhảy theo trong ~2 s, chat
   *"Ac chính ở map khác - dùng Thần Hành Phù sang."*; log
   `findstr /C:"[AC-PHU]" bin\client\jx_auto.log` → `[AC-PHU] gui movemapid map=75 lan 1/3 (dang o map …)` rồi
   `[AC] theo ac chinh (x,y) d=…`.
3. Ác chính về **thành** với *Trong thành* **bật** (vd Tương Dương 78): log `[AC-PHU] dung phu lan 1/3 - dich 'Tương Dương'
   (nhom 0) map=78` → `thoai: chon 'Sử dụng thuật thần hành…'` → `chon 'Thành thị'` → `chon 'Tương Dương Phủ'` →
   `chon 'Tương Dương Trung Tâm'` → đổi map. *Trong thành* **tắt** → không có dòng nào, ác phụ đứng bãi đánh tiếp.
4. Cất phù vào rương → chat *"Không có Thần Hành Phù trong túi…"* đúng một lần, ác phụ auto tại chỗ; bỏ phù lại túi → đi.
5. Ác phụ cấp thấp hơn bãi (vd 85 với bãi 90): 3 dòng `gui movemapid … lan 1/3, 2/3, 3/3` cách nhau 4 s, rồi
   `[AC-PHU] hong (ly do 3 …) - nghi 60 giay roi thu lai` + chat vàng. Đây là máy chủ từ chối (*"phía trước nguy hiểm"*), không phải lỗi.

Log ghi TCVN3 (đọc qua `vn_edit.py --read`).

---

## 6. Nhánh, build, kiểm nhị phân

- Nhánh `claude/wauto-ac-chinh-phu-32d2e8` (worktree `D:\GAMEDEVNEW\.claude\worktrees\machine-code-limit-analysis-ac47d1`)
  = `main` `f7324d86` **+ gộp** `claude/wauto-auto-horse-bugs-ad45c9` `52b663f3` (`ce7977d2`) **+** `60d72b26` (đợt này).
- **Vì sao phải gộp nhánh horse-bugs:** `CoreClient.dll` đang chạy `676d452f` = đúng bản dựng tại `52b663f3` của worktree
  `wauto-auto-horse-bugs-ad45c9` (md5 khớp tệp `Sources\Core\ClientRelease\CoreClient.dll` ở đó), nhánh này **chưa vào
  `main`** (12 commit 14–16/09: ngựa, `[DT-HUY]`, bình máu kẹt tay, Tin Sứ…). Dựng từ `main` không thì DLL mới **rớt** các
  vá đó. Đã kiểm chuỗi trong DLL mới: `[TK-BOMAY]` 2/2, `[TK-DONTUI]` 4/4, `[PK-NGUA]` 2/2, `[FIGHT-HORSE]` 2/2, `[HD-TS]` 1/1,
  `[DT-HUY]` 7/7, `[DT-CUON]` 2/2, `[PK-KHIEN]` 1/1, `[TK-NPC]` 3/3, `[HC-BAN]` 1/1, `[DT-LA]` 1/1 (mới/cũ) — tập cha;
  thêm `[AC-PHU]` 5, `movemapid`, 4 câu thông báo mới; **0** câu "qua Xa Phu" cũ.
- `main` sau `52b663f3` có thêm vá BAOMAT/MAYID (`KItemList::SetPrice` chặn giá âm, `KProtocolProcess` phía máy chủ) —
  vào theo, không liên quan client.
- Build: `build.py --config "Client Release" --platform Win32 --project Core --solution-dir <worktree>`; WAuto:
  `MSBuild WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32` tại `E:\Src_Auto_Ngoai\WAuto\WAuto`.
- Bộ vá idempotent: `ReverseTools\goi_va_wauto_acchinh_thp_1709.py [--thu] [--root <worktree>] [--khong-wauto]`.

---

## 7. Bẫy

1. Worktree mới **thiếu `Lib\debug64\`** (git bỏ qua) → `LNK1181 common.lib` dù compile 0 lỗi; chép từ `D:\GAMEDEVNEW\Lib\debug64`.
2. TCVN3 **không có chữ hoa có dấu**: "Ác chính" phải viết "Ac chính" trong `.cpp` (bộ vá bắt được ngay); WAuto UTF-16 thì viết được.
3. `ChoiGame.bat` đổi cả `WAuto.exe` → phải **thoát WAuto** trước, không thì bat báo lỗi và không mở game.
4. Thần Hành Phù chỉ được máy chủ đếm **trong túi** (`GetItemCount(0,6,1,1271)`), để trong rương coi như không có.
5. Map cấm phù (script `shenxingfu.lua`): 324 báo danh, 44, 175, 197, 208–223, 336–339, 341, 342, 375–395, 416–511, và
   `CheckAllMaps` → đứng ở đó thì lượt hỏng (lý do 3) chứ không phải lỗi auto; Tống Kim / Công Thành đang cầm máy thì
   máy Ác chính vốn không chạy.
