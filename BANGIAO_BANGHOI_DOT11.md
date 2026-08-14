# BÀN GIAO ĐỢT 11 — HỆ BANG HỘI JX2 (phiên 14/08/2026)

> Phiên này làm **cả 8 việc** chủ game giao trong `BANGIAO_BANGHOI_DOT10.md`, mỗi việc đều
> chạy **điều tra lại từ bản Linux trước khi sửa** (6 phiên điều tra + 4 phiên phản biện độc lập).
> Mọi fix đã build sạch 4 đích, commit riêng từng việc, đã push `gamedevnew/main` và đã chép
> client sang cây PATCHFULL chủ game chạy thật.

## 0. CHUỖI COMMIT (theo thứ tự)

| Commit | Việc |
|---|---|
| `3dfb606` | ① Vá hồi quy mất danh sách thành viên + rác (RepositionRows đặt mặc định CẢ 4 mảng: vị trí + kích thước + **màu chữ** mỗi vòng — phản biện phát hiện màu cũng là trạng thái kẹt) |
| `459b802` | ⑥ Nhật ký nguyên văn TCVN3 + tách đúng 2 đường tiền (bỏ cộng đôi của DONATE, thêm `COP_DEPOSIT_MONEY` 28 = MONEYFUND_ADD, nút "Gửi" ngân quỹ) |
| `d54e8f5` | ⑤ Hết nút ".." — `WndLabeledButton.cpp:128` thiếu `+1` so với WndList/WndList2; nhãn về nguyên văn blueprint; bỏ SetLabel nút sắp xếp; nới `Rt_BtnDepose` 110→122 |
| `0607c3c` | ③ Đổi phe đổi màu tên: relay JX2 phát `BE_CHANGED_CAMP` (`JX2_BroadcastCampSync` sao khuôn DBChangeCamp bỏ phần tiền JX1) + client thêm `break;` bị thiếu (gói phe rơi vào nhánh JX2 thành MEMBER_SYNC rác) |
| `6ad5f8b` | ④ Panel thành viên = ĐÚNG 5 trường stringtable gốc (:475-479); cột 3 danh sách = **giá trị tiêu chí sắp xếp** ("loại hình"); mục sắp xếp 4 = Ẩn sĩ |
| `2f0ee12` | ② Trang **2×2 "Xem tin Bang khác"** nguyên văn blueprint (39 section `Ozm_*`/`PageBg5`, trang server `PAGE_OTHERZM=7`, 4 bang/gói, xem không cần bang) + sửa 2 lỗi phản biện của ⑥ (log chết vì client gửi cứng 10 vạn → **hộp nhập số tiền** `KUiTongGetString` + `sJX2_Msg2Tong` vô điều kiện như bản gốc; bỏ trần tổng-quỹ 2 tỷ tự bịa) |
| `2876a6f` | ⑦+⑧ **LIÊN MINH** (đầy đủ 3 tầng) + **ĐẠI THẦN** trang con 4 |
| `7d57cb1` | Vá tràn mảng `m_FunBtn[23]` → `[26]` (bảng nút lên 24 mục) — tự phát hiện khi rà soát |
| `f8253fd` | Sàn phản biện 2×2 bắt 6 lỗi, sửa cả 6 — **lớn nhất: `KUiTongGetString` dùng `KWndText32` nên KHÔNG GÕ ĐƯỢC** (lỗi ngủ có sẵn của lớp, đổi sang `KWndEdit32` + mở lại nhánh Enter bị comment); gate PHÁT về `>=100`; ApplyBtn W=126 (21<21 vẫn cắt); JiYu H=48 (3 dòng lời nhắn); xóa chữ mẫu design-time (KWndText::Init đọc `Text=`); `m_ZmBg.Enable(false)` |
| `65e1043` | `KWndEdit::GetText` 3 tham số (build gãy sau khi đổi lớp) |
| `41ead8e` | Sàn phản biện LIÊN MINH bắt 4 lỗi, sửa cả 4 — **lớn nhất: duyệt không cần ĐƠN** (minh chủ ép bang bất kỳ vào + trừ oan 1 triệu → thêm field **54** = UnionID đang xin, TOP 22, relay chỉ duyệt bang ĐÃ xin); 5 đường từ chối im lặng ở relay giờ báo rõ; trần `[LevelUnionNum]` đọc nhầm cấp JX1 (1-9, thang 10-90 chết) → đọc cấp kiến thiết field 13 ánh xạ 0→3…5→8; Trục xuất/Cách chức qua HỘP XÁC NHẬN (2 khóa [UnionStr] hết chết); ô nhập tên 20→31 byte |

## 1. LIÊN MINH — MÔ HÌNH ĐÃ CHỌN (đọc trước khi đụng)

**Không có bảng DB mới.** Liên minh = trạng thái DẪN XUẤT từ field JX2 của TỪNG bang
(mọi thứ đã nằm sẵn trong `TTongStruct` nên tự bền vững):

| Chỗ chứa | Nghĩa | Nguồn gốc |
|---|---|---|
| field **10** | UnionID = `g_String2Id(tên liên minh)` | quy ước gốc (TONG_GetUnionID đọc sẵn) |
| field **49** | Giờ rời liên minh (epoch) — luật **chờ 3 ngày** 259200s, gác ở GS | dịch ngược `jx_linux_y 0x80d0e50/0x80d18e0` |
| field **50** | 1 = bang minh chủ | bổ sung của ta (ghi rõ) |
| field **51/52/53** | NameID Thừa Tướng / Nguyên Soái / Tiên Phong | bổ sung của ta |
| field **54** | UnionID bang này ĐANG XIN vào (minh chủ chỉ duyệt được bang đã xin) | bổ sung của ta (chống ép vào + trừ oan tiền) |
| `m_szLeagueTName[32]` | TÊN liên minh (field legacy có sẵn, trước không ai ghi) | tái dụng theo gợi ý điều tra |

Đồng bộ tên: `STRING kind UNION (defTONG_JX2_STR_UNION=4)` — relay phát khi đổi + dump khi GS nối.
`STONG_JX2_TONG_OP_COMMAND` **nới thêm `m_szName[32]` ở CUỐI** (relay+GS build cùng nhau, bảng size dùng sizeof nên tự khớp).
`TONG_JX2_INFO_SYNC` **nới đuôi** (cả 2 bản KProtocol.h): `m_dwUnionID + m_szUnionName[32] + m_bUnionLeader` → ô "Liên minh" trang Tin tức và ô TxtTongUnion trang Chức năng hiện tên thật.

**Luật bản gốc đã cài** (bằng chứng trong commit message `2876a6f`): lập miễn phí/cấm trùng tên;
vào = bang chủ XIN → minh chủ DUYỆT, trần bang theo cấp bang minh chủ đúng `[LevelUnionNum]`
(0-9:3, 10-29:4, 30-49:5, 50-69:6, 70-89:7, 90+:8 — **bảng phân tích cũ thiếu bậc 4**),
ngân quỹ MỖI bên >999999 lượng và trừ 100 vạn MỖI bên; minh chủ rời = GIẢI TÁN;
chỉ minh chủ được đuổi; **đổi phe bị CẤM khi trong liên minh**; mọi thông báo nguyên văn stringtable VN.

**Đường đi:** client (5 nút trang con 3, ẩn/hiện theo trạng thái) → COP 29-33 → GS kiểm (bang chủ,
3 ngày, đã/chưa trong liên minh — trả lời chi tiết bằng chuỗi gốc, mã 20 = "im lặng" vì handler
tự nhắn) → TOP 17-20 → relay thi hành + nói trên kênh chat bang (`sJX2_SayTong` như DBChangeCamp).

## 2. ĐẠI THẦN (trang con 4)

2 nút ủy nhiệm/cách chức (nhãn nguyên văn kể cả DẤU CÁCH ĐẦU của blueprint) → COP 34/35 →
TOP 21 → field 51/52/53. Chọn thành viên trong danh sách rồi bấm nút, nhập số chức 1/2/3.
**Deviation ghi rõ:** bản gốc gate "quốc chủ" = bang chủ bang chiếm thành (Lâm An/Biện Kinh,
`MSG_NW_INSTATE_NOTKING`) — hệ công thành chưa có nên tạm gate bang chủ; hiệu ứng skill NW
953-962 (hào quang/kỹ năng đại thần) **chưa port** — cần port hệ nationalwar mới có.

## 3. DEVIATION CÓ CHỦ ĐÍCH (đối chiếu 100% Linux)

1. `Ozm_ApplyBtn` W 100→120 và `Rt_BtnDepose` 110→122: engine JX1 cắt nhãn theo `W*2/F`,
   JX2 gốc không cắt — nới để CHỮ hiện đủ như bản gốc.
2. `Fun_BtnLeaveUnion` dời (231,228)→(131,254): blueprint để chồng khít lên nút Duyệt đơn;
   bản gốc giấu/hiện bằng C++ theo trạng thái, ta tách ô cho cả hai cùng hiện được khi là minh chủ.
3. "Xem chi tiết" trên thẻ 2×2: bản gốc mở trang chi tiết bang (suy đoán, script client gốc
   không có trong máy) — ta hiện hộp thông tin đủ trường (tên/bang chủ/phe/cấp/nhân số/2 ngưỡng).
4. Xin vào liên minh: bản gốc có hàng đợi đơn ở relay (cấu trúc chưa dịch ngược được) — ta làm
   "xin = thông báo tới minh chủ online, duyệt = minh chủ gõ tên bang" (không mất dữ liệu, không đoán).
5. Câu xác nhận `[UnionStr]` bỏ `%s` (AskThenSendOp không thay thế được).
6. Log "phát tiền cho thành viên" không có hàm gốc (bản gốc chỉ phát CỐNG HIẾN) — mô phỏng mẫu
   `:377` đổi đơn vị lượng, gate `>100` strict.

## 4. BẪY MỚI GHI NHẬN TRONG PHIÊN

- 🔴 **Heredoc bash nuốt `\\` thành `\`** (cả trong old-anchor lẫn nội dung + cả pattern python
  qua heredoc): khối nào chứa `\\` phải đi đường **file python viết bằng Write tool**. Octal đơn
  (`\256`) thì an toàn.
- 🔴 `CTongControl` member **private** — helper static trong KTongJX2Relay.cpp phải đi qua
  accessor (`JX2_NameID/JX2_TongLevel/JX2_LeagueName` đã thêm); `CTongSet` là friend nên method
  của nó truy cập thẳng được.
- 🔴 Nâng `TJX2_FUN_BTNS` **phải nâng `m_FunBtn[]`** trong .h (đã dính 2 lần trong phiên — hiện [26]).
- ⚠️ `KTongProtocol.h` từng có dòng `#define defTONG_JX2_STR_RECRUIT` **LẶP** (một dòng LF trần) — đã dọn.
- ⚠️ Màu chữ control cũng là trạng thái kẹt theo trang như SetSize — khối mặc định đầu
  `RepositionRows` giờ reset cả màu.

## 5. CÒN LẠI SO VỚI BẢN LINUX (nói thật, KHÔNG phải 100% toàn hệ)

8 việc chủ game giao = XONG và đúng chuẩn đã điều tra. Toàn HỆ bang hội còn thiếu
(như đánh giá đợt 6, chưa nằm trong 8 việc): sản xuất vật phẩm công phường (FoundryItem/durability),
lãnh địa map động 586-597, công thành/thành chiến + gate "quốc chủ" thật, hiệu ứng skill NW đại thần,
hệ League LG_/LGM_ đầy đủ, hàng đợi đơn xin vào bang đặt ở relay (hiện ở RAM GS), cửa sổ con
phát tiền/委任 riêng theo blueprint (đang phủ bằng nút + hộp nhập), 7 chuỗi log >95 byte bị cắt
(muốn đủ phải nới `defTONG_JX2_RECORD_LEN` = đổi format DB), client chỉ vẽ 12/16 dòng nhật ký mỗi gói.

## 6. TEST NHANH CHO CHỦ GAME

1. **Hồi quy:** vào "Xem tin Bang khác" rồi quay lại tab Thành viên — danh sách phải còn nguyên, hết rác, hết chữ xanh lạ.
2. **2×2:** nút đáy "Xem tin Bang khác" → 4 thẻ như bản Linux; "Xin gia nhập"/"Xem chi tiết"/Trang trước-kế.
3. **Nút "..":** Gửi/Cất/Phát hiện đủ chữ.
4. **Tiền:** Gửi (kiến thiết) / Gửi (ngân quỹ — nút mới cạnh ô Ngân quỹ) / Rút / Phát — mở HỘP NHẬP SỐ; nhập ≥100 vạn thì nhật ký ghi câu có dấu nguyên văn; mọi mức đều báo trên chat bang.
5. **Đổi phe:** trừ tiền xong **màu tên trên đầu** mọi thành viên online đổi (chính 255,168,94 / tà 255,146,255 / trung lập 85,255,145). Đang trong liên minh thì bị chặn bằng câu gốc.
6. **Panel thành viên:** bấm 1 người → đúng 5 dòng (Danh hiệu-tên / danh hiệu / Đẳng cấp hiện tại / Điểm cống hiến hiện tại / Thời gian nhập bang).
7. **Liên minh:** trang con 3: bang A "Lập liên minh" (nhập tên) → bang B "Vào liêm minh" (gõ tên bang A) → bang chủ A nhận tin, bấm "Gia nhập liên minh" gõ tên B → cả 2 bị trừ 100 vạn ngân quỹ, ô Liên minh hiện tên; B "Hủy liên minh" → 3 ngày sau mới vào lại được (báo số giây).
8. **Đại thần:** trang con 4, chọn thành viên → " ủy nhiệm đại thần" → nhập 1/2/3 → chat + nhật ký báo "được phong làm Thừa Tướng/…".

⚠️ **Server:** GS + Relay + client phải chạy bản MỚI CÙNG NHAU (gói TONG_OP/INFO đã nới đuôi).
Binaries đã nằm ở `bin\server\CoreServer.dll` (03:18) + `bin\multiserver\S3Relay.exe` (03:21);
nếu server đang chạy thì dùng `CAPNHAT_SERVER_JX2.bat` như mọi khi. **KHÔNG cần xóa DB bang**
(TTongStruct không đổi format; field mới nằm trong mảng KV có sẵn).
