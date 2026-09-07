# BÀN GIAO 06/09 đêm — Nhân vật thật CaiBang "hay bị mất đồ": tìm ra gốc và đã vá

Chủ (06/09 ~22:50): *"nhân vật CaiBang của tôi đang bị tình trạng hay bị mất đồ cần điều tra để fix"*.

Nhánh `botnoi-0609`, commit `e06a12b6` đã lên `origin/main`. `bin\server\CoreServer.dll.moi` = **d0bc87a4372ae2d7da27ad6a0f8724cc** (18.465.280) chờ swap.

## 1. Bằng chứng: mất đúng những món nằm CHUNG một ô trang bị

Máy chủ đã có sẵn hệ giám sát (`jx1_role.role_history` + `role_anomaly`, ngưỡng tụt 5 vật phẩm/lần lưu). CaiBang có **161 cảnh báo `mat_vat_pham` riêng hôm nay** (mọi nhân vật khác chỉ 1-2).

Đọc thẳng blob `role_history` (công cụ mới `ReverseTools/quet_noi.py` cùng bộ script trong bàn giao này), đếm món theo **ô trang bị**:

| Bản lưu | Số món "đang mặc" | Số ô thật | Ô bị hai món |
|---|---|---|---|
| 20:45:49 | 17 | 14 | ô 4, ô 5, ô 10 |
| 20:51:19 | 15 | 14 | ô 7 |
| 20:55:49 | 15 | 14 | ô 7 |
| 21:09:34 | **14** | 14 | không |

Mỗi lần vào lại game mất **đúng số món dư**. Món mất là đồ thật: nhẫn (detail 7, particular 8, cấp 8), giáp (detail 2, particular 3, cấp 9).

Loại trừ nguyên nhân hợp lệ:
- **Không phải bán**: tiền túi + tiền rương không tăng; có lần mất 8 món mà tiền đổi **+0**.
- **Không phải giao dịch / mua bán**: `jx1_game.mua_shop`, `giao_dich`, `game_log` **không có dòng nào** của CaiBang trong khung giờ đó.
- **Không phải đồ hết hạn**: mọi món mất đều có trường hạn `0/0/0`.
- **Không phải bot**: mọi dòng `[BotNoi]` / `[BotTrangBi]` / `[BotVuKhi]` trong `bot.log` đều là tên bot, không có tên người thật.

## 2. Cơ chế (hai lỗi nối nhau, đều trong `Sources/Core/Src/KItemList.cpp`)

**Lỗi 1 — tạo ra hai món cùng một ô.** `KItemList::Equip` gán thẳng `m_EquipItem[nEquipPlace] = nIdx` mà **không gỡ món đang ở ô đích**. Đường của người chơi (`MoveItem`, nhánh `pos_equip`) chỉ `UnEquip` ô **nguồn** rồi `Equip` vào ô **đích**, nên kéo đồ từ ô trang bị này sang ô trang bị khác đang có đồ là sinh trùng: cả hai món cùng `nPlace = pos_equip`, cùng số ô, và **cả hai đều được ghi xuống DB**. Cùng chỗ này `Equip` cũng không xoá `m_Hand`, nên còn để lại món "ma" trên tay (chính là gốc vụ bot ném đồ chiều nay).

**Lỗi 2 — mất món khi nạp.** Lúc vào game, `LoadDBPlayerInfo` gọi `AddKIL(nIndex, nLocal, x, y)`; nhánh `pos_equip` gặp ô đã bị chiếm thì `return 0` — món thứ hai **không vào danh sách và biến mất vĩnh viễn**, không một dòng log. Các nhánh lưới (túi, rương, rương mở rộng) cũng bỏ món im lặng khi trùng toạ độ.

## 3. Đã vá — `KItemList.cpp` (tệp dùng chung client + server), 4 hunk

Bản vá idempotent: `ReverseTools/goi_va_matdo_ochongo_0609.py`.

- **H1** `Equip`: ô đích đang có món khác thì `UnEquip` món đó trước, trả về hành trang (hết chỗ thì giữ trên tay) — không bao giờ còn hai món một ô.
- **H2** `Equip`: xoá `m_Hand` khi chính món đó vừa rời tay lên người.
- **H3** `AddKIL` nhánh `pos_equip`: ô đã chiếm thì **cứu món** — đặt vào hành trang, rồi rương; chỉ bỏ khi thật sự không còn chỗ.
- **H4** `AddKIL` nhánh hành trang / rương: trùng ô lưới thì tìm ô trống khác thay vì bỏ món.

H3 và H4 là lớp an toàn: kể cả sau này có đường nào khác sinh trùng, đồ vẫn không mất.

## 4. Nhị phân

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **d0bc87a4372ae2d7da27ad6a0f8724cc** | 18.465.280 | tắt GameServer → `ChayGameServer.bat` |

Build từ `origin/main` (đã gồm đợt 5 bot + bản sửa hộp thoại của phiên Lua54). Nhãn kiểm: đủ 14 nhãn của bản `.moi` trước (`chong binh`, `[BotMac]`, `chuc vu JX2`, `[BotMach]`, `[BotTayDiem]`, `Chieu Da Ngoc Su Tu`, `bot_hoc120`, `[RELAY`, `AUC_MsgTong`, `CL_Cong`, `st_ledger`, `[RoleChk2]`, `UpdateBattleBoxAll`, `[S13-TELE-CU]`). Bản `.moi` cũ giữ tên `CoreServer.dll.moi.lua54_34a3fdd5_2129`.

**Client**: `KItemList.cpp` dùng chung nên client cũng nên nhận bản mới, nhưng dữ liệu lưu là do **máy chủ** quyết định, nên swap `CoreServer.dll` là đã hết mất đồ. Bản `CoreClient.dll` tôi build (eb01f766) **nhỏ hơn** bản client đang chạy (bcf80dec, 22:28 của phiên khác) nên **tôi không đặt** `.moi` client để khỏi nuốt việc phiên đó; khi phiên client build lần tới từ `origin/main` là tự có.

## 5. Nghiệm thu sau swap

Chạy lại bộ đếm ô trang bị (script trong `ReverseTools`), hoặc đơn giản:

```bash
grep -c "mat_vat_pham.*CaiBang" bin/multiserver/mysql_roledb.log
```

Sau khi swap, con số này phải **ngừng tăng** (trừ các lần uống hết bình thuốc — thuốc lẻ vẫn tụt hợp lệ). Kiểm chắc hơn: vào game, kéo đồ từ ô trang bị này sang ô trang bị khác đang có đồ, thoát ra vào lại — món cũ phải nằm trong hành trang, **không mất**.

## 6. Việc còn mở

- **Đồ trong rương của CaiBang tụt 31 → 2 món** trong khoảng 19:03–20:45 (trang bị + phù). Không phải bán (tiền không tăng), không phải hết hạn, và **không thấy trùng ô lưới** trong các bản lưu đó — nên đây là **đường thứ hai chưa xác định**, có thể liên quan thao tác rương của WAuto. Bản vá H4 đã chặn khả năng mất do trùng ô, nhưng cần thêm dữ liệu (một lần tái hiện có ghi giờ) mới kết luận được.
- **Đồ đã mất có thể lấy lại**: `role_history` còn giữ blob đầy đủ (ví dụ id 694963 lúc 20:45:49 có đủ 17 món, id 693958 lúc 19:03 có 31 món rương). Khôi phục là ghi đè bản ghi nhân vật, **tôi chưa làm** — chờ chủ quyết vì phải tắt nhân vật khỏi game và sẽ mất mọi thứ nhặt được sau mốc đó.
