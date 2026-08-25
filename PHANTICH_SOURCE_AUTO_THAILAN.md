# PHÂN TÍCH SOURCE "AUTO THÁI LAN" (`D:\Source_ANTITHAILAN`)

> Ngày phân tích: **24/08/2026**. Toàn bộ số liệu dưới đây là **đo thật** trên cây nguồn, không suy đoán.
> Câu hỏi gốc của chủ game: *"source đó có code tính năng thật không, hay chỉ là phần thông tin/UI còn tính năng thật phải móc ở CoreClient?"*

---

## 0. KẾT LUẬN 1 DÒNG

**CÓ code tính năng thật, đầy đủ, build ra đúng exe đang phát hành.** Auto tự viết 100% phần **"bộ não"** (quyết định) bằng C#; phần **"bàn tay"** (đi/đánh/nhặt/mua/click NPC) thì **không viết lại** mà **dò địa chỉ hàm sẵn có của client rồi gọi từ ngoài vào**. **Không cần sửa CoreClient, không inject DLL riêng, chạy trên client GỐC.**

---

## 1. NHẬN DẠNG DỰ ÁN

| Mục | Giá trị |
|---|---|
| Đường dẫn | `D:\Source_ANTITHAILAN\Source_ANTITHAILAN` |
| Tên project thật | `AutoThaiLan.csproj` (project cũ: `Keoxe365.sln`) |
| AssemblyName / Namespace | `AutoThaiLan` |
| Loại | `WinExe`, **.NET Framework 4.0**, **x86** |
| Tham chiếu ngoài | **KHÔNG có** — chỉ `System`, `System.Windows.Forms`, `System.Drawing`, `System.Security`, `System.ServiceProcess` |
| Game đích | **Võ Lâm 1 / JX1** — lớp cửa sổ `"Sword3 Class"` (`Class56.cs:78`) |
| Chứng chỉ ký | `antithailan.pfx` (có sẵn trong cây) |
| Tài nguyên nhúng | `Sign` (112 KB, **đã mã hoá**, không phải PE) |

> ⚠️ **Có 2 bản giống hệt lồng nhau**: `Source_ANTITHAILAN\Source_ANTITHAILAN\...` — dễ sửa nhầm cây.

---

## 2. BẰNG CHỨNG ĐÂY LÀ SOURCE THẬT (không phải vỏ)

```
md5  bc09b9a058d67a3a357c85bd069e8fa6  Keoxe365/obj/x86/Release/AutoThaiLan.exe   (do build sinh ra)
md5  bc09b9a058d67a3a357c85bd069e8fa6  Release/AutoThaiLan.exe                     (bản phát hành)
```

**Khớp 100%.** ⇒ Cây nguồn này đã được biên dịch thành công và cho ra **đúng binary đang chạy ngoài đời** (29/07/2024).

Ngoài ra:
* `1.956` lệnh `ReadProcessMemory`, `637` lệnh `WriteProcessMemory`.
* `277` chuỗi chữ ký AOB (có wildcard `??`).
* Quét **158 checkbox** của `Form1`: **0 cái** chỉ nằm trong designer — mọi nút đều chảy vào engine.

---

## 3. QUY MÔ MÃ

| Thư mục | Số file | Số dòng | Vai trò |
|---|---|---|---|
| `Class\` | 97 | **90.095** | **Engine — 0 dòng UI** |
| `Keoxe365\` | 45 form | 60.911 | Form (designer + handler + một phần logic) |
| `Struct\` | 70 | 1.748 | struct/enum dữ liệu |
| gốc | 8 | 150 | enum + interface |
| **Tổng** | **232 `.cs`** | **153.067** | |

**`Form1.cs` (826 KB / 20.370 dòng)**:
* `1` – `1187`: khai báo control
* `1188` – `1995`: hằng số, bảng whitelist domain, kiểm tra file `hosts`
* **`1996` – `9733`: `InitializeComponent()`** ← designer thuần
* **`9733` – `20370`: 12.632 dòng handler** — ghi cờ vào `GStruct51[]` (1 struct = 1 acc) rồi lưu file

> `Class108.cs` bị comment sạch 2.149 dòng (chỉ là data blob do trình biên dịch sinh, **không ai gọi**, vô hại).

---

## 4. KIẾN TRÚC 3 TẦNG

```
┌─ TẦNG UI ─────────────────────────────────────────────┐
│ Form1.cs + 45 FormXxx.cs                              │
│   checkBoxTiepCan → gstruct51_0[n].int_95             │
│   → Class56.smethod_13() lưu ra file cấu hình         │
└───────────────────────┬───────────────────────────────┘
                        ▼
┌─ TẦNG NÃO (quyết định) ── 90.095 dòng C# ─────────────┐
│ Class78  = bộ điều phối: MỖI ACC bung 13 luồng        │
│            (Class35/54/61/62/93/96/106…)              │
│ Class21,41 = chọn mục tiêu, so bình phương k/c, đánh  │
│ Class6,61  = train, di chuyển, phù, lượm rác          │
│ Class59,57 = Tống Kim, tập kết                        │
│ Class45    = nạp Config\tbRoad.txt (bảng đường đi)    │
└───────────────────────┬───────────────────────────────┘
                        ▼
┌─ TẦNG TAY (thực thi) ─────────────────────────────────┐
│ Class56 = bảng 246 địa chỉ (dò bằng AOB lúc chạy)     │
│ Class73 = ráp shellcode + CreateRemoteThread          │
│ Class24 = 49 DllImport (WinAPI)                       │
│           ↓                                            │
│    GỌI THẲNG HÀM CỦA GAME CLIENT                      │
└───────────────────────────────────────────────────────┘
```

---

## 5. ⭐ BẢNG ĐỊA CHỈ — TRỌNG TÂM CỦA CẢ HỆ

### 5.1. `Class56.cs` — **246 địa chỉ có tên**, ráp thành mảng 254 mục ở `Class56.cs:2835`

Cấu trúc 1 mục (`GStruct53`, xem `Struct\GStruct53.cs`):

```csharp
public static GStruct53 gstruct53_8 = new GStruct53   // Class56.cs:222
{
    string_0 = "FUNC_BAO_DANH",   // tên do tác giả đặt
    int_4 = 0, int_0 = 1, int_2 = 0
};
```

**Tên trong bảng chính là tên lớp C++ của CoreClient/Engine** — đây là bản đồ đối chiếu quý nhất:

`KPLAYER_ADDR` · `KPLAYER_LEFT_SKILL` · `KPLAYER_RIGHT_SKILL` · `KPLAYER_MOVEITEM` · `KPLAYER_SIZE` · `KITEM_INFO_BASE_ADDR` · `KITEM_ID_SIZE` · `KITEM_POS_SIZE` · `KITEM_SET_PRICE` · `KNPC_BASE_SIZE` · `KNPC_NAME_OFFSET` · `KSHOP_BASE_ADDR` · `KSHOP_SIZE` · `KINFO_SIZE` · `KTabFileLoad` · `m_nObjectIdx` · `m_nPeopleIdx`

### 5.2. Nhóm **HÀM GAME GỌI ĐƯỢC** (auto dùng để hành động)

| Hành động | Hàm client |
|---|---|
| Di chuyển | `RUNTO_FUNC_ADDR`, `CLICKTO_FUNC_ADDR` |
| Nhặt đồ | `PICKUP_OBJECT_FUNC_ADDR` |
| Dùng / vứt vật phẩm | `ITEM_USE_FUNC_ADD`, `ITEM_THROW_FUNC_ADD` |
| Mua shop | `FUNC_SHOP_BUY` |
| Click menu NPC | `FUNC_MENU_0_CLICK_INDEX`, `FUNC_MENU_1_CLICK_INDEX`, `FUNC_MENU_0_CLOSE` |
| Chạy script | `DOSCRIPT_FUNC_ADD` |
| Chat / nói / in chữ | `PLAYER_CHAT_FUNC_ADD`, `PLAYER_SAY_FUNC_ADDR`, `PLAYER_PRINT_FUNC_ADD` |
| **Tống Kim** | `FUNC_BAO_DANH`, `TONG_PUSH_MONEY_FUNC`, `TONG_SPEC_FUNC`, `TONG_ENTER_BASE_ADDR`, `TONG_TUYENCHI_BASE_ADDR`, `TONG_FUNC_ACCEPT_CHANGE_COLOR`, `TONG_NAME_2ID` |
| Giao dịch | `TRADE_FUNC_ADDR`, `TRADE_ACCEPT_FUNC_ADDR`, `TRADE_APPLY_FUNC_ADDR`, `TRADE_PUT_MONEY_FUNC_ADDR` |
| Tổ đội | `TEAM_LEAVE_FUNC_ADDR`, `TEAM_CREATE`, `TEAM_MEMBER`, `TEAM_CAPTAIN_NAME_OFF`, `REPLY_INVITE_FUNC_ADDR`, `FOLLOW_INDEX_FUNC_ADDR` |
| Rương / hộp | `BOX_ACCEPT_FUNC`, `BOX_INC_POINT_FUNC_ADDR`, `BOX_UNLOCK_BASE_ADDR`, `BOX_CHETAO_BASE_ADDR`, `BOX_THUTHAP_BASE_ADDR`, `BOX_VANSUTHONG_BASE_ADDR` |
| Kỹ năng | `SKILL_INC_LEVEL_FUNC_ADDR`, `SKILL_INFO_BASE_0_ADDR`, `SCRIPT_LEFT_SKILL_BASE_ADDR`, `SCRIPT_RIGHT_SKILL_BASE_ADDR` |
| Sửa đồ | `REPAIR_FUNC`, `REPAIR_MONEY_FUNC` |
| Khác | `RETURNCITY_BASE_ADDR`, `KYTRANCAC_CLOSE_FUNC`, `CLEAR_SMS_FUNC_ADDR`, `LASTCHANEL_FUNC`, `FLAG_BOXENTER_FUNC_ADDR`, `OPEN_FUNC_SPEC`, `FUNC_SWITCH_KETHOP`, `FUNC_SPEC_ADDR` |

### 5.3. Nhóm **DỮ LIỆU ĐỌC RA** (offset struct trong client)

* **Nhân vật**: `HP1/HP2_OFFSET`, `MP1/MP2_OFFSET`, `MONEY_1/2_OFFSET`, `CHAR_FIGHT_OFFSET`, `ATTACK_RANGE`, `POS_X/Y_SIZE`, `RIDE_HORSE_OFFSE`, `STATS_TOTAL_OFFSET`, `TOTAL_POIN_SKILL_OFFSET`, `DANHHIEU_STATUS_OFFSET`, `KILLER_STATUS_OFFSET`
* **NPC/quái**: `NPC_BASE_ADD`, `NPC_COUNT_BASE_ADDR`, `NPC_DATA_SIZE`, `NPC_NAME_OFFSET`, `NPC_DOING_OFFSET`, `NPC_EXISTS_OFFSET`, `NPC_COLOR_OFFSET`, `NPC_LEVEL_CAPTION`, `NPC_EXP_1`, `NPC_GOLD`, `NPC_SEX_OFFSET`, `NPC_PROFILE_STATUS_OFFSET`
* **Vật phẩm rơi (object)**: `OBJECT_BASE_ADDR`, `OBJECT_COUNT_BASE_ADDR`, `OBJECT_POS_X/Y`, `OBJECT_NAME_SIZE`, `OBJECT_STACKCOUNT_SIZE`, `OBJECT_TIME_SIZE`, `OBJ_EXIST`, `OBJ_GROWN`
* **Vật phẩm trong túi**: `ITEM_BASE_ADD`, `ITEM_HOLD_BASE_ADDR`, `ITEM_GENRE`, `ITEM_KIND`, `ITEM_LEVEL`, `ITEM_SERIES`, `ITEM_COLOR`, `ITEM_PRICE`, `ITEM_DURATION`, `ITEM_MAGIC_ATTRIB`, `ITEM_REQUIRE_ATTRIB`, `ITEM_IDKEY`, `ITEM_INTRO`, `ITEM_LOCK_OFFSET`, `ITEM_WIDTH/HEIGHT`
* **Buff**: `BUFF_BASE_ADDR`, `BUFF_SKILL_ID_OFFSET`, `BUFF_TIME_OFFSET`, `BUFF_DOING_OFFSET`, `BUFF_POS_X/Y_OFFSET`
* **Giao diện**: `DIALOG_BASE_ADDR`, `DIALOG_TEXT_OFFSET`, `MENU_0/1_COUNT_*`, `SMS_BASE_ADDR`, `SORTCUT_BASE_ADDR`, `HELP_BASE_ADDR`, `LASTCHANEL_*`, `STRING_INPUT_SIZE`, `MOUSE_POS`, `FLAG_BOX_TITLE`, `FLAG_IS_STICK`, `FLAG_SHOW_GAME`
* **Tống Kim**: `TONGKIM_BASE_ADDR`, `TONGKIM_PLAYER_BASE_ADDR`, `TONGKIM_POINT_MAX_OFFSET`, `TONGKIM_SOLUONG_NGUOI`, `TONGKIM_THOIGIAN_BATDAU`, `TONGKIM_THOIGIAN_HIENTAI`
* **Hệ thống**: `ENGINE_HANDLE`, `GATEWAY_BASE_ADDR`, `GATEWAY_CONNECT_OFFSET`, `SERVER_BASE_ADDR`, `MAPID_BASE_ADDR`, `MAP_NAME_BASE_ADDR`, `GET_TICK_COUNT`, `CPU_SLEEP_1/2/3`, `CPU_GIAM_MUC_1/2`, `INFECT_SIZE/LENGTH`, `MSVCR80_invalid_parameter_noinfo`

### 5.4. Cách dò địa chỉ — **AOB signature có wildcard**

```csharp
// Class69.cs:17
string_0 = "RAINBOW_CASH_1",
string_1 = "3B 48 38 72 04",                       // mẫu byte cần tìm
byte_0   = Class12.smethod_8("8B 45 FC 8B 4D 08"), // byte sẽ ghi đè
int_2    = -6                                       // dịch offset sau khi tìm thấy
```

`Class12.smethod_8()` đổi chuỗi hex → `byte[]`, ký tự `??` thành `0x3F` (dấu wildcard).
⇒ **Không hard-code địa chỉ** ⇒ cắm được vào **nhiều bản client khác nhau** của nhiều server.

---

## 6. ⭐ TẦNG GỌI HÀM CLIENT — `Class73.cs:280`

```csharp
byte[] array = Class12.smethod_8("60" + string_0 + "E8 00 00 00 00" + string_1 + "61" + text);
//                                 ↑pushad   ↑đẩy tham số   ↑call tương đối  ↑dọn stack ↑popad
Class24.WriteProcessMemory(gstruct51_0.int_137, num2, array, array.Length, ref int_);
...
Class24.smethod_31(num8, gstruct51_0.int_137, num7);   // vá lại offset của lệnh E8
```

```csharp
// Class73.cs:298 — chạy shellcode
public static bool smethod_12(int int_0, uint uint_54)
{
    uint num2 = Class24.CreateRemoteThread(int_0, IntPtr.Zero, 0u, uint_54, 0u, 0u, out num);
    Class24.WaitForSingleObject(num2, 30000u);
    ...
}
```

* Vùng shellcode cấp phát bằng `VirtualAllocEx` (`Class24.cs:239`), con trỏ chạy `gstruct51_0.uint_18` tăng dần.
* Luồng thoát sạch bằng `ntdll!RtlExitUserThread` (`Class73.cs:187`).
* 4 nơi gọi `CreateRemoteThread`: `Class24.cs:1480`, `Class52.cs:8`, `Class73.cs:303`, `Class95.cs:104`.

### 🔴 KHÔNG inject DLL riêng

`Class24.cs:1462 smethod_67()` là hàm inject `LoadLibraryA` kinh điển — nhưng **grep toàn bộ 232 file: 0 nơi gọi**. Code chết. Bản phát hành `Release\` **không kèm DLL nào của game**.

---

## 7. VÁ BYTE CHỐNG CRASH (`Class69.cs` — 32 mục)

Vá **trong bộ nhớ lúc chạy**, không sửa file trên đĩa. Tên mục cho biết vá vào module nào của client:

* `ENGINE_DISABLE_CASH_1` → **Engine.dll**
* `RAINBOW_CASH_1` → **Rainbow.dll**
* `LUALIBDLL_CASH_1..13` → **LuaLib.dll** (13 điểm!)
* `MSVCR80_CASH_2` → **msvcr80.dll**
* `GAME_DISABE_CASH_1..21` → **Game.exe**

Ví dụ `Class69.cs:386` ghi `{51,192,195,144}` = **`33 C0 C3 90`** (`xor eax,eax; ret; nop`) → **vô hiệu hoá hẳn một hàm**.
⇒ Đây chính là ô *"Fix lỗi game để hạn chế bị diss"* trên giao diện.

---

## 8. LUỒNG CẤU HÌNH UI → ENGINE (đã truy 2 ví dụ đầy đủ)

**Ví dụ A — "Tiếp cận":**
```
checkBoxTiepCan_CheckedChanged (Form1.cs:12269)
  → gstruct51_0[n].int_95 = Checked
  → Class56.smethod_13()  (lưu file)
  → engine đọc: Class21.cs:181, 588, 741, 820, 904 ; Class41.cs:263, 834, 908 ; Class103.cs:565
```

**Ví dụ B — "Tự báo danh Tống Kim":**
```
checkBoxTubaoDanh_CheckedChanged (Form1.cs:16878)
  → Form1.int_44[0] = Checked   (int_44[1] = điểm báo danh)
  → engine đọc: Class59.cs:1649, 1692, 1921 ; Class64.cs:223 ; Class6.cs:1398 ; Class61.cs:430
```

Đã kiểm chứng thêm 6 tính năng "hiếm" đều có engine đứng sau:
`int_54` (lượm rác) → Class6/37/61 · `int_26` (chế Huyền tinh) → Class6/21/25 · `int_123` (tự click Nhiếp Thí Trần) → Class78 · `int_128` (Đường Môn boom) → **47 lần** ở Class21/41/57/64/83 · `int_84` (Thanh Thiên Địa Trọc) → Class57/78/98 · `int_115` (Chiến Long động) → Class20/35/57/6/75.

---

## 9. BẢN ĐỒ FILE → TÍNH NĂNG

### Đã truy vết chắc chắn ✔

| File | Vai trò |
|---|---|
| `Class24.cs` | **Lớp WinAPI** — 49 `DllImport`: `OpenProcess`, `RPM/WPM`, `VirtualAllocEx`, `CreateRemoteThread`, `SendInput`, `PostMessageA`, `SuspendThread`, `NtQueryInformationThread`, `CheckRemoteDebuggerPresent`, `QueryFullProcessImageName` |
| `Class56.cs` | **Bảng 246 địa chỉ** + đọc/ghi cấu hình từng acc (`GStruct51`) |
| `Class69.cs` | Chữ ký AOB + **32 bản vá chống crash** |
| `Class73.cs` | **Ráp shellcode, gọi hàm client**, `RtlExitUserThread` |
| `Class78.cs` | **Bộ điều phối**: mỗi acc bung 13 luồng |
| `Class21.cs`, `Class41.cs` | Chọn mục tiêu, tính khoảng cách, đánh, tiếp cận, quây quái, Tam Trụ, mua thuốc |
| `Class59.cs`, `Class57.cs` | **Tống Kim**, tập kết, Thất Thành |
| `Class45.cs` | Nạp + giải mã `Config\tbRoad.txt` → `GStruct21[]` (bảng đường đi) |
| `Class103.cs` | Danh sách boss + **tự ghép Sát Thủ Giản** (`Class103.int_2`) |
| `Class12.cs` | Bộ tiện ích/giải mã (xem §11) |

### Suy ra từ chuỗi nhận dạng (chưa truy hết) ~

| File | Dấu hiệu |
|---|---|
| `Class6.cs`, `Class61.cs` | *"Không tìm được đường lên bãi train."*, *Thần Hành Phù*, *Hồi thành phù*, *"Thoát game vì bị đồ sát."* |
| `Class68.cs` | *tín sứ*, *Dịch quan*, *"Kết thúc chạy Phong kỳ."* |
| `Class83.cs` | *Xa phu*, *Thất Thành*, *Thần Hành*, *"CTC Ac chính người dùng tự điều khiển.."* |
| `Class82.cs` | *"Sửa Đồ Tại Chỗ"*, *"(thiếu tiền sửa: "* |
| `Class29/46/63.cs` | **Bảng tên map + toạ độ** (669 + 448 + 10 chuỗi): Phượng Tường, Tần Lăng, Long Tuyền thôn, Nhạn Đãng sơn, Tương Dương… |
| `Class38.cs` | **748 chuỗi** — tên vật phẩm / dòng menu đối thoại NPC |
| `Class80.cs` | **950 chuỗi** — phái (Thiếu Lâm, Thiên Vương, Đường Môn, Nga My, Thúy Yên, Hoa Sơn…), chiêu thức |

---

## 10. BẢN ĐỒ GIAO DIỆN — 36 tab / 45 form

**Tab (`Form1`)**: Điều khiển · Hậu cần · Khác · Chung1 · Gán ô tắt · Nâng cao · Cài game · Boss · Phụ trợ · Chuyển thuốc · VSThông · Thông tin · Bán · Thiết lập 1/2 · Lic HDD · Chuyển chiêu trái · Chiêu phải (ms) · Gặp Boss · Bảng 1/2 · **Vũ khí** · **CTC** · **Chiêu thức** · **Lọc trang bị** · **Mua dùng** · **Mua thuốc** · **Phục hồi** · **Tam môn đài** · **Tam trụ** · Test · **Thất thành** · **Event** · **Chung2 (Tín sứ)** · **TK (Tống Kim)** · **Train**

**Form có logic thật** (không phải vỏ), đo bằng số lệnh gọi engine:

| Form | Lệnh memory | Gọi engine |
|---|---|---|
| `FormMayphu.cs` | 67 | 425 |
| `FormClickNPC.cs` | 37 | 196 |
| `FormNhiepTT.cs` (vượt ải) | 21 | 153 |
| `FormChayBoss.cs` | 26 | 144 |
| `FormTapKet.cs` | 19 | 95 |
| `FormRauria.cs` | 9 | 220 |
| `FormLocdo.cs` | 0 | 75 |

Các form lớn khác: `FormLocdoTest` · `FormLogin` · `FormTuychon` · `FormHaucanTuithuoc` · `FormTangdiem` · `FormNhiepTT` · `FormDame` · `Combo` · `FormPhongKy1` · `FormCompatibility` · `FormRaovat` · `FormDoiMauBang` · `FormLuomrac` · `FormDuongMon` · `FormTuyenchien` · `FormHuyenTinh` · `FormAntivirus` · `FormUutienNguHanh` · `FormMagic` · `FormBanEvent` · `FormTienSkill` · `FormXaphuCT` · `FormThuocTocdoDanh` · `FormSuado` · `FormPT` · `FormCuuSat` · `FormTutim` · `FormLoaitru` · `FormMenuClick` · `FormThuocLag` · `FormKhongCatdo` · `FormPushMoney` · `FormChayMuaMauTK` · `FormNgamyBuff` · `FormPhimtat` · `FormVideoHelp` · `TryNewVersion` · `ThemXoaDanhsach` · `Dangky`

---

## 11. LỚP TIỆN ÍCH / GIẢI MÃ (`Class12.cs`)

| Hàm | Chức năng |
|---|---|
| `smethod_8(string)` | hex `"8B 45 FC"` → `byte[]`; `??` → `0x3F` (wildcard AOB) |
| `smethod_15(string)` | **Base64 → UTF-8 string** |
| `smethod_47(string)` | string → `byte[]` (ANSI, tuỳ chọn thêm null-terminator) |
| `smethod_72(string)` | **Base64 + Deflate giải nén → string** |
| `smethod_33(path,…)` | đọc file (chọn được encoding) |

`Class65.smethod_7/11` = đọc/ghi cấu hình. `Class105.smethod_1/4` = ráp địa chỉ từ kết quả quét AOB.

---

## 12. BẢN QUYỀN / KHOÁ SERVER

* `Form1.cs:1210` — bảng **whitelist domain** `string[11,6]`: `{ tên miền, kiểu, hạn dùng (ddMMyy), mode, tên exe game, cờ }`. Ví dụ: `vlhkmp.net`, `volamhoainiem.click`, `jxtinhhuynhde.net`, `volamtk1.net`, `volamhoangkimpk.com`, `vlhaothien.net`, `hoiucpk.com`, `vltruyenky.net`, `vltuongphung.net`, `manhlongpk.net`, `volam2024.net`.
* `Form1.cs:1188` — `string_0 = "12.06"` (phiên bản), `customname = "hoiucpk"`, `custom_file_name = "hoiucpk.exe"` ⇒ **build riêng cho từng server** (xem `Release\Safed\`, `Release\vltuongphungT\`).
* `Form1.cs:1955-1985` — **tự xoá file `hosts`** nếu thấy chặn `cloud.updategame.xyz` / `*.updategame.xyz` rồi khởi động lại chính nó.
* Chống soi: `CheckRemoteDebuggerPresent`, `SuspendThread`, `NtQueryInformationThread`, `TerminateThread`. Có `FormAntivirus` + ô *"Báo keylog virus nếu có"*.
* `Release\NETSafePro_123456\` = bộ **obfuscator .NET** tác giả dùng (kèm `dnlib.dll`).

---

## 13. 🔴 CẠM BẪY KHI KHAI THÁC SOURCE NÀY

1. **Tên bị làm rối hoàn toàn** — `Class1..Class108`, `smethod_0..N`, `GStruct51.int_95`. Chỉ tên control UI là đã đặt lại có nghĩa.
   → **Cách lần**: `checkBoxX` → tìm handler trong `Form1.cs` → lấy tên trường số (`int_NN`) → `grep -rn "\.int_NN\b" Class/`.
2. **Chuỗi ký tự đã mã hoá** — `Class12.smethod_15("…")` (Base64), `Class12.smethod_72("…")` (Base64+Deflate). `Config\tbRoad.txt` cũng mã hoá từng dòng.
3. **Font TCVN3** — tiếng Việt trong mã lưu dạng `"Ph­îng T­êng"` = *Phượng Tường*. Đọc/sửa bằng công cụ thường sẽ hỏng byte. (Cùng bẫy với source SwordOnline.)
4. **Hai cây lồng nhau giống hệt** — dễ sửa nhầm.
5. **`grep -E "[ \t]"` KHÔNG hiểu `\t` là tab** trong POSIX ERE → mọi dòng thụt bằng tab sẽ bị bỏ sót. Dùng `[[:space:]]`.
6. `grep -P` báo lỗi *"supports only unibyte and UTF-8 locales"* → phải `export LC_ALL=C`.
7. **Bash heredoc rút `\\` thành `\`** → mọi nội dung có đường dẫn Windows phải ghi bằng công cụ Write, không dùng heredoc.

---

## 14. Ý NGHĨA ĐỐI VỚI WAuto (auto ngoại của mình)

* Bảng **246 địa chỉ có tên** ở `Class56.cs` = **bản đồ đối chiếu sẵn** giữa auto ngoại ↔ mã C++ `SwordOnline` của mình. Muốn biết `FUNC_BAO_DANH` làm gì → tra thẳng `KPlayer` / `KUiTong*` trong cây nguồn nhà.
* **277 chữ ký AOB** là thứ đáng chép nhất: giúp auto chạy được trên nhiều bản client mà không phải cập nhật địa chỉ tay mỗi lần server đổi build.
* Cách **gọi hàm client bằng shellcode** (`60 … E8 … 61` + `CreateRemoteThread`) là lý do server rất khó phân biệt với người thật: gói tin gửi đi **do chính client sinh ra**, hợp lệ 100%.
* **Không có A\***: đường đi dựa vào bảng mốc `tbRoad.txt` + bảng toạ độ cứng (`Class29/46/63`) + hàm `RUNTO/CLICKTO` của client. Khác hẳn hướng WAuto đang làm.

---

## 15. LỆNH TRA CỨU NHANH ĐÃ DÙNG

```bash
export LC_ALL=C
cd "D:/Source_ANTITHAILAN/Source_ANTITHAILAN/Keoxe365"

# bảng địa chỉ có tên
grep -oE 'string_0 = "[A-Za-z0-9_]+"' Class/Class56.cs | sed 's/string_0 = //;s/"//g' | sort -u

# ánh xạ chỉ số -> tên địa chỉ
awk '/public static GStruct53 gstruct53_[0-9]+ = new GStruct53/{v=$4}
     /string_0 = "/{if(v!=""){s=$0;sub(/.*string_0 = "/,"",s);sub(/".*/,"",s);print v" = "s;v=""}}' Class/Class56.cs

# lần 1 tính năng: checkbox -> trường -> engine
grep -n "checkBoxTubaoDanh" Keoxe365/Form1.cs
grep -rn "int_44" Class/

# liệt kê phương thức (nhớ [[:space:]], KHÔNG dùng \t)
grep -nE "^[[:space:]]*(private|public|protected|internal)[^=;]*\([^;]*\)[[:space:]]*$" Keoxe365/Form1.cs
```
