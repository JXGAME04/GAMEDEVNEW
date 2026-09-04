# REPRESENT3 — SẬP, GIẬT, RAM. Phiên 03/09/2026 tối

**Nhánh `rep3-0309`, worktree `D:\GAMEDEVNEW_wt_rep3`. Đã build, đã push, ĐÃ CHÉP vào client.**

Chủ game giao ba việc, theo thứ tự: *"tìm lỗi crash"*, *"tại sao Represent3.dll lại tốn gấp đôi ram"*,
*"lag giựt hơn Represent2"*, rồi *"dựa vào 2.0 để làm theo"* với ràng buộc *"không được giảm chất lượng"*.

Tiếp nối `BANGIAO_REPRESENT3_VLTK2_0309.md`. ⚠️ Tài liệu đó có **hai chỗ sai đã đính chính ở mục 5** dưới đây.

---

## 1. Trạng thái chốt phiên

| Tệp trong `bin\client` | md5 | Ghi chú |
|---|---|---|
| `Represent3.dll` | **`113eeb1d`** | đang chạy, gồm cả 3 bản vá |
| `Represent3.dll.truoc` | `74ac07ad` | bản gốc để quay lui |
| `CoreClient.dll`, `Game.exe`, `Engine.dll`, `Represent2.dll` | — | **không đụng tới** |

`[Client]` trong `config.ini` sạch, không có khóa `Rep3*` nào, tức chạy toàn mặc định:
texture **32 bit đầy đủ**, kho `POOL_DEFAULT`, ngân sách 512 MB, ghi thống kê 30 giây một lần.

5 commit, đã push hết:

| Commit | Việc |
|---|---|
| `7c068772` | vá SẬP: `PrepareFrameData` ghi ngược dấu âm vào bộ đệm dùng chung |
| `01a05927` | bộ thử: bỏ trần 60 sprite để đo được ở quy mô thật |
| `25e7133d` | vá GIẬT: ảnh nạp hỏng bị thử lại mỗi khung vẽ |
| `bd14cc6d` | đính chính RAM, hạ trần 768→512, thêm bản 4444 có kiểm biên |
| `76b618cf` | port theo 2.0: kiểm NULL chống sập + bỏ chế độ dọn hung |

---

## 2. Lỗi SẬP — `ucl_nrv2b_decompress_8 + 0x154`

### 2.1 Cái bẫy trong nhật ký

`jx_crash.log` báo `0xC0000005` đọc địa chỉ loạn. **Nhưng đó không phải cái giết tiến trình.**
Ảnh đổ của Windows lúc 19:26 cho `0xC0000374`, tức **hỏng vùng nhớ động**. Lỗi đọc chỉ là triệu chứng
bắt sớm của cùng một vết ghi tràn. Hai mục đó cũng thiếu ngăn xếp vì bộ ghi nhật ký tự chết khi lần
dbghelp trên vùng nhớ đã hỏng.

Lần sập 20:24 có ngăn xếp đầy đủ và nó chốt luôn chẩn đoán:

```
ucl_nrv2b_decompress_8       n2b_d.c:113
XPackFile::ExtractRead       XPackFile.cpp:262
XPackFile::GetSprFrame       XPackFile.cpp:583   <-- NHÁNH GIẢI NÉN
KPakList::GetSprFrame        KPakList.cpp:194
SprGetFrame                  KPakFile.cpp:114
Represent3.dll (4 khung)
KNpcRes::Draw                KNpcRes.cpp:577
```

### 2.2 Chuỗi nhân quả

1. Trong pak, `XPackSprFrameInfo.lSize` **ÂM** nghĩa là khung lưu **thô, không nén**.
2. `XPackFile::GetSprFrame` đọc dấu này để chọn `DirectRead` thay vì `ExtractRead`.
3. `TextureResSpr::PrepareFrameData` **đổi dấu ngay trong bộ đệm SPRHEAD dùng chung với engine**.
   Vị trí trùng khít: `sizeof(KPAL24)`=3 nên `m_pHeader + 32 + Colors*3` trúng đúng bảng khung, và
   `sizeof(SPROFFS)`=8=`sizeof(XPackSprFrameInfo)` nên `.Length` trúng đúng `.lSize`.
   Bộ đệm đó chính là `m_pHeader` mà `SprGetFrame` nhận vào.
4. Lần gọi **thứ hai** cho cùng khung thấy `lSize` dương, đẩy dữ liệu **thô** vào bộ giải nén UCL.
5. `n2b_d.c` biên dịch **thiếu `SAFE`** nên `fail()` rỗng: không một lệnh kiểm biên nào.
   Đã dịch ngược `Engine.dll` `0x35944-0x3597B` xác nhận.
6. Con trỏ tra ngược chạy loạn, và ghi tràn `malloc(lSize)`.

### 2.3 Chứng minh trên pak thật

Quét đúng 40 pak mà `package.ini` nạp, tìm khung thô rồi thử cả hai đường:

| Đường | Kết quả |
|---|---|
| Đúng: đọc thẳng 10 byte | `SPRFRAME` hợp lệ, rộng 1 cao 1 |
| Sai: đẩy vào bộ giải nén | đòi lùi **17.829.888 byte** khi mới ghi ra 0 byte |

Khung thô nằm trong `sprvlngaothe.pak`, `updatejx14.pak`, `updatejx15.pak`, đều là pak sprite nhân vật,
khớp với ngăn xếp rơi trong `KNpcRes::Draw`.

### 2.4 Vì sao chỉ mới xảy ra

Represent2 (`KImageStore2`) **không bao giờ đụng bảng khung**, nó gọi thẳng `SprGetFrame`.
Chủ bật `Represent=3` lúc 17:40, vết sập UCL đầu tiên trong toàn bộ nhật ký là 18:21.

### 2.5 Vì sao "RAM trên 1 GB là bị"

Quan sát của chủ là **điều kiện kích hoạt**, không phải trùng hợp. Lỗi chỉ nổ ở lần lấy khung **thứ hai**.
Lần đầu xong là có texture, `PrepareFrameData` thấy texture còn thì thoát sớm. Chỉ khi bộ đệm dọn bớt,
texture bị bỏ, thì lần vẽ sau mới lấy lại và đọc trúng dấu đã bị lật. RAM cao nghĩa là đang cache nhiều
sprite, tức cảnh đông, tức dọn và lấy lại liên tục, nên càng dễ trúng vài khung thô hiếm hoi.

**Vá `7c068772`**: dùng biến tạm, không đụng bộ đệm dùng chung. Đúng lối `GetPixelAlpha` đã dùng sẵn.
Đã grep lại: sau vết này không còn chỗ nào ghi vào SPRHEAD hay bảng khung của engine.

---

## 3. Lỗi GIẬT — 19.100 lượt nạp mỗi giây

Bản vá `[REP3 03/09 LOAD]` của phiên trước bỏ việc ghi nhớ ảnh nạp hỏng, ý tốt là để ảnh lỗi tạm thời
còn nạp lại được. Hậu quả: **mỗi ảnh không tồn tại bị thử nạp lại ở mọi khung vẽ**, mỗi lượt quét chỉ mục
của cả 40 pak rồi đọc đĩa.

| Số đo từ `jx_rep3.log` | Giá trị |
|---|---|
| Ảnh không tồn tại | 200 tệp `\spr\Ui3\...\1xx-3xx.spr` |
| Lượt nạp trong 30 giây | 572.990 |
| Quy ra mỗi giây | **19.100** |

200 ảnh nhân 62 khung một giây ra đúng khoảng đó. Bản cũ `74ac07ad` không bị vì nó vẫn chèn mục rỗng
vào bộ đệm nên lần sau tra ra ngay. Đó là lý do bản cũ mượt mà bản mới giật.

**Vá `25e7133d`**: chèn lại mục rỗng như bản gốc, và chỉ thử nạp lại mỗi 10 giây thay vì mỗi khung.

**Đo lại sau vá: 2,2 lượt mỗi giây, giảm 8.575 lần.**

---

## 4. Lỗi SẬP thứ hai — do chính bản vá giật gây ra

Nhóm mổ 2.0 phát hiện: `TextureResMgr::GetImage` nhánh **lệch kiểu** trỏ thẳng vào
`m_TextureResList[pos].m_pTextureRes->m_nTexMemUsed` mà **không kiểm NULL**. Bản 2.0 có kiểm
(`0x10025A31 cmp dword [eax+0x10],0 / je`), ta thiếu.

Từ khi `25e7133d` cho chèn lại mục rỗng, một ảnh hỏng bị hỏi lại bằng **kiểu khác** sẽ trỏ vào con trỏ
rỗng và sập ngay. Đây là lỗ hổng bản vá của tôi làm sống lại. **Vá `76b618cf`.**

---

## 5. RAM — hai lần đính chính

### 5.1 Đính chính báo cáo cũ, mục 10.8

Mục đó chỉ đúng **hướng** (thủ phạm là `D3DPOOL_MANAGED`) nhưng **số đo sai**, vì đo ở quy mô quá nhỏ:
bộ thử chặn cứng 60 sprite, mà ở 60 sprite bộ đệm không bao giờ lớn tới hạn mức. Nó ghi Represent2 97 MB
so với Represent3 105 MB, tức chênh 8 %, không hề ra "gấp đôi".

Nâng trần lên 4000 rồi đo lại ở **1075 sprite, 8600 lượt vẽ mỗi khung**:

| Biến thể | RAM riêng | ms/khung |
|---|---|---|
| Represent2 | 105,1 MB | 35,03 |
| Represent3 `74ac07ad`, MANAGED | **209,3 MB** = 1,99 lần | 18,92 |
| Represent3 mới, DEFAULT | **95,2 MB** | 16,72 |

### 5.2 Đính chính của chính tôi

Tôi từng viết trong mã rằng texture ở `POOL_DEFAULT` "không ăn RAM". **Sai.** Hồi quy nhật ký game thật:

| Mẫu | Công thức |
|---|---|
| 12 mẫu, một phiên | `RAM = 240,9 + 0,657 × (texture + raw spr)` |
| 93 mẫu, nhiều phiên | `RAM = 324 + 0,455 × (texture + raw spr)` |

Mỗi MB texture trong VRAM **vẫn** kéo theo khoảng 0,45 tới 0,66 MB RAM tiến trình, do Windows cấp bộ đệm
hệ thống cho mọi vùng nhớ Direct3D. Đổi sang `POOL_DEFAULT` vẫn giảm rất nhiều vì bỏ được bản sao chủ
đầy đủ, nhưng không về 0. Chú thích sai đã sửa trong `bd14cc6d`.

### 5.3 Mốc 200 MB không với tới được

Ngoại suy bộ đệm về 0 vẫn còn khoảng 324 MB. Đó là `Game.exe`, `CoreClient.dll`, engine, Lua, bản đồ.
**Không có cách chỉnh Represent3 nào đưa RAM về 200 MB.** Thực nghiệm cũng xác nhận: đặt
`Rep3CacheMB=120` chỉ kéo RAM xuống 403 MB chứ không xuống 200 MB, mà lại gây giật.

---

## 6. Mổ nhị phân `represent3free.dll` của VLTK 2.0

26 tác tử, có vòng phản biện. Các sự thật đã chốt, mỗi cái kèm địa chỉ đọc được:

| # | Sự thật | Bằng chứng |
|---|---|---|
| B8 | Texture 2.0 = **A8R8G8B8**, 1 mức mip, MANAGED, một lần `D3DXCreateTexture` | `0x10023A29 push 1` (POOL), `0x10023A2B push 0x15` |
| B8b | Hàm 4444 `0x10023710` có **0 caller, 0 xref** = mã chết | quét toàn DLL |
| B2 | Ngân sách 30/50/80/120 MB đo **kho RAW SPR**, không đo texture | `0x10025B31 cmp [esi+0x30], [esi+0x10]`; chuỗi HUD `0x100BC634` |
| B3 | 2.0 **không có trần byte cho texture** | toàn thân `0x10024F80..0x100251E4` không đọc trần nào |
| B7 | Mỗi lượt dọn bỏ **đúng 1 mục** rồi thoát hàm, chỉ khi nghỉ > 10 giây | `0x10025096 cmp ebx,0x2710` |
| B10 | Van "tối đa 4 tài nguyên mới mỗi khung" **chỉ áp cho lô vẽ dưới 4 phần tử** | `0x10013EF6 cmp eax,4 / jl` |
| B12 | 2.0 **kiểm NULL** ở nhánh lệch kiểu, ta thiếu | `0x10025A31` |

### 6.1 Đã port

**Bỏ chế độ dọn hung.** Ta đang `nMax = bOver ? 8 : 1` và `uIdle = bOver ? 1000 : 10000`.
2.0 chỉ có một chế độ. Số đo với `Rep3CacheMB=120`:

| Ngân sách | Lượt bỏ mỗi giây |
|---|---|
| 768 hoặc 512 MB | 30 |
| 120 MB, chế độ hung | **508** |

Và nó bỏ đúng những khung **đang vẽ**: phần "vẽ khung này" 34 tới 124 MB trên bộ đệm chỉ 105 tới 155 MB.

**Kiểm NULL** như mục 4.

### 6.2 KHÔNG port, kèm lý do

- **Van 4 tài nguyên mới mỗi khung**: chỉ áp cho lô vẽ dưới 4 phần tử, tức đông người thì chính 2.0 cũng
  đi đường không có van. Port vào không giúp lúc đông.
- **Cổng VRAM còn dưới 10 MB**: card còn trống 3,9 GB, không bao giờ chạm.
- **Texture 16 bit**: 2.0 không dùng, và chủ đã ràng buộc không giảm chất lượng.

---

## 7. Cách kiểm sau khi chạy

Trong `jx_rep3.log` mới, so hai mốc 30 giây liên tiếp:

| Cột | Kỳ vọng |
|---|---|
| `nap` | gần như đứng yên khi đứng một chỗ. Lúc giật là 19.100 mỗi giây |
| `bo` | hiệu số khoảng 900 mỗi 30 giây. Lúc giật là khoảng 15.000 |
| `RAM rieng` | quanh 400 tới 530 MB tùy cảnh |
| `jx_crash.log` | không còn mục nào ở `ucl_nrv2b_decompress_8` |

---

## 8. Còn lại, chờ chủ quyết

1. **Lớp chắn thứ hai cho bộ giải nén.** Engine vẫn dùng bản UCL không kiểm biên. Bản an toàn
   `ucl_nrv2b_decompress_safe_8` có sẵn trong `Sources\Engine\Src\ucl\n2b_ds.c` và **đã nằm trong
   `Engine.vcxproj`**, cùng chữ ký hàm, chỉ cần đổi hai chỗ gọi trong `XPackFile.cpp`.
   Đã kiểm: build `Engine.dll` **không** tự chép đè vào client, nó chỉ chép `engine.lib`.
   Rủi ro: nếu có pak nào đang tràn nhẹ mà vẫn chạy thì sau khi đổi sẽ thành không nạp được ảnh đó.
2. **Hai chỗ cắt RAM không đụng hình ảnh, chưa làm**: ảnh nền giữ hai bản (`m_pSysMemTexture` sống mãi,
   chừng 25 MB), và dữ liệu RLE gốc giữ mãi cho sprite nạp một lần (16 tới 57 MB).
3. **Bộ tệp vẫn lệch nhánh.** `Represent3.dll` từ nhánh rep3, `CoreClient.dll` và `Game.exe` từ nhánh khác.
   Nên cơ chế tự lùi về Represent2 khi Represent3 lỗi **không có mặt**, và `MAX_NPC` client vẫn 256.
4. Chưa quét khung lưu thô trong `spr.pak`, `updatejx16.pak` vì quá lớn.

---

## 9. Bẫy cho phiên sau

- 🔴 **Bộ giải nén UCL của engine KHÔNG kiểm biên.** Bất cứ ai đưa dữ liệu sai vào nó đều phá vùng nhớ,
  và triệu chứng sẽ hiện ra ở một chỗ hoàn toàn khác.
- 🔴 **Cấm ghi vào bộ đệm SPRHEAD trả về từ `SprGetHeader`.** Engine đọc lại chính bộ đệm đó.
- 🔴 **`rep_harness` từng chặn 60 sprite.** Đo RAM ở quy mô đó không tài hiện được gì. Phải đo từ 1000 sprite.
- 🔴 **`POOL_DEFAULT` vẫn ăn RAM** khoảng 0,45 tới 0,66 MB cho mỗi MB texture. Đừng tin là 0.
- 🔴 **Bản 2.0 dùng 8888 chứ không phải 4444**, và ngân sách 30/50/80/120 MB của nó đo kho RLE
  chứ không đo texture. Mục 4 và 6 của `BANGIAO_REPRESENT3_VLTK2_0309.md` nói sai chỗ này.
- 🔴 **Vá nguồn phải lấy neo trực tiếp từ tệp và kiểm đếm bằng 1.** Phiên này từng vá hụt vì neo
  `\t\t\tm_nLoadCount++;` là hậu tố của `\t\t\t\t\t\tm_nLoadCount++;` ở chỗ khác, làm hỏng tệp.
  Phát hiện nhờ số byte không khớp, khôi phục bằng `git checkout -- <tệp>` rồi vá lại.
