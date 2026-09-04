# THIẾT KẾ 03/09/2026 — WAuto: "Tìm ac chính" + "Đánh cùng mục tiêu ac chính" (học từ auto Thái Lan)

Phiên `wauto` (Claude Fable 5.1). Trạng thái: **THIẾT KẾ, chưa thi công** — chờ chủ duyệt 6 lựa chọn ở mục 8.

## 0. Tóm tắt một đoạn

Ac phụ (các cửa sổ game khác cùng máy) tự đi theo ac chính "bất cứ nơi đâu" trong cùng map và đánh đúng mục tiêu ac chính đang đánh.
Không cần hỏi máy chủ, không có gói mạng mới: **WAuto.exe đã biết map + toạ độ của từng cửa sổ** (game gửi `IPCMainSync`), chỉ thiếu
**mục tiêu đang đánh** và **nhịp cập nhật** (hiện chỉ gửi khi máy chủ đồng bộ chính mình). Thiết kế: game gửi thêm một gói vị trí nhỏ
mỗi 300 ms; WAuto ghép vị trí + mục tiêu của ac chính vào `autoData` của từng ac phụ (trường mới ở CUỐI struct, đi xuống game mỗi 54 ms
như mọi ô cấu hình khác); trong CoreClient thêm máy `AC_Process` đi tới ac chính và giao mục tiêu cho máy PK. Đúng nguyên lý auto Thái
(đọc bộ nhớ ac chính → RUNTO), nhưng WAuto không cần đọc bộ nhớ vì đã có kênh IPC sẵn.

## 1. Đã đối chiếu (mã thật)

| Đã có | Ở đâu | Dùng vào việc |
|---|---|---|
| Game → WAuto: tên, map (`nMapId`, `szMap`), toạ độ mps (`mapx/mapy`), máu, cấp, kỹ năng, `dwPID` | `IPCMainSync`, gửi từ `KProtocolProcess::SyncCurNormalData` (KProtocolProcess.cpp:1905-1972) mỗi lần máy chủ đồng bộ chính mình | WAuto đã lưu vào `gameNode.player.{nMapId,nX,nY,szName}` (WAuto.cpp:2835-2848) |
| WAuto → Game: toàn bộ `autoData` mỗi **54 ms** (`GAMELOOPINTV`) cho cửa sổ đang tick | `IPCGameLoop{autoData setting}` (WAuto.cpp:3352-3366) → `S3Client.cpp:1231 ExtAutoLoop(&pGL->setting)` | Trường mới ở CUỐI `autoData` tự tới game, không cần gói mới; WAuto cũ gửi ngắn thì game xoá trắng phần đuôi (S3Client.cpp:1228-1243) |
| Thứ tự máy trong game | `S3Client.cpp:826-900`: CT → TK → LD → HD → ST → (`nBS`) → DT khi `nBS==0` | Chỗ cắm `nAC` |
| Máy PK nhận mục tiêu bằng `ea.uNpcID` khi máy sự kiện trả 2 | tiền lệ Tống Kim (`TKP_FIGHT` giao `ea.uNpcID`, máy PK không xoá khi `nBS==2`) | "Đánh cùng mục tiêu" = giao `uNpcID` của ac chính |
| Tìm NPC theo ID trong vùng đồng bộ | `KNpcSet::SearchID(dwID)` (KNpcSet.cpp:221) | ac phụ tra mục tiêu ac chính |
| Đi tới toạ độ + luật ngựa mới | `DT_WalkTo` / `DT_DuocLenNgua` (đợt 4 TK) | di chuyển tới ac chính |
| Mục tiêu hiện tại của người chơi | `KPlayer::GetTargetNpc()` = `m_nPeapleIdx`; khi auto đánh: `m_sExtAuto.uNpcID` | ac chính báo mục tiêu |
| ID còn trống trong `Resource.h` | ~~628–639~~ → **632–639 (8)** sau khi `wauto-ca` lấy 628–631 (22:2x); khối ngoài dải 700–715 | ô cấu hình mới (cần 9) → nâng `IDC_INDEX_END` lên 700 |
| Bẫy WAuto cũ / mới lệch struct | `.dat` di trú theo `offsetof` (tiền lệ CT), game xoá trắng đuôi gói ngắn | an toàn khi 3 tệp không lên cùng lúc |

Auto Thái làm gì (đã mổ, memory `jx1-thailan-tim-acchinh-cochec-0309`): đọc bộ nhớ client ac chính 200 ms/lần (map, x/y, `m_nPeopleIdx` → ID
mục tiêu), nhiều máy thì TCP "@|map|..|x|y|..|target|..*"; theo khi cùng map và cách > 200; dừng cách 150; "cùng mục tiêu" = tìm ID trong
mảng NPC của mình rồi đánh; khác map chỉ Thất Thành có "vào map ac chính".

## 2. Dòng dữ liệu

```
 Game (ac chính)  ── PRG_VITRI mỗi 300 ms ──►  WAuto.exe (m_vGameNode)
   CoreClient                                     │ tra node có player.szName == apdata.szAcChinhTen
   map, x, y, mục tiêu dwID, còn sống, camp        ▼
                                              ghi vào apdata của TỪNG ac phụ (trường AC_* ở cuối autoData)
                                                  │ IPCGameLoop mỗi 54 ms (đã có)
 Game (ac phụ)  ◄──────────────────────────────────┘
   S3Client: nAC = ATYPE_ACCHINH  (sau các máy sự kiện, trước Dã Tẩu)
   CoreShell AC_Process:
     - cùng map && cách > KC   → DT_WalkTo(điểm cách ac chính 150)   trả 1 (cầm lái)
     - bCungMucTieu && mục tiêu ac chính thấy được → ea.uNpcID = ID  trả 2 (máy PK đánh)
     - còn lại                   → trả 0 (auto thường: đánh quái quanh, nhặt đồ...)
```

## 3. Thay đổi cấu trúc (đều ở CUỐI, tương thích ngược)

### 3.1 `ipc_shared.h`
```cpp
enum PROTGAMEID { ..., PRG_TEAMNAMELIST, PRG_VITRI };          // thêm CUỐI enum
struct IPCViTri : public SharedState                             // game → WAuto, 300 ms
{
	UINT dwPID;        // Player.m_dwID (khớp gameNode.player.dwPID)
	int  nMapId;
	int  nX, nY;       // mps
	UINT uMucTieu;     // dwID NPC đang đánh (ưu tiên ea.uNpcID của auto, không có thì m_nPeapleIdx); 0 = không
	int  bSong;        // m_CurrentLife > 0 && m_Doing != do_death/do_revive
	int  nCamp;        // m_CurrentCamp (Tống Kim: 1 Tống / 2 Kim)
};
struct autoData {
	... (giữ nguyên tới nCTCapBinh)
	// ---- Ac chính (03/09) ---- cấu hình (WAuto lưu .dat):
	int  bTimAcChinh;      // ô "Tìm ac chính"
	int  nAcChinhKC;       // khoảng cách bắt đầu đi tìm (mps), mặc định 200 (Thái: 200)
	int  bAcChinhThanh;    // ô "Tìm trong thành" (0 = ac chính đứng trong thành thì không theo)
	int  bCungMucTieu;     // ô "Đánh cùng mục tiêu ac chính"
	int  bAcChinhVaoMap;   // ô "Ac phụ tự vào map ac chính" (đợt 2 — xem mục 8 Q2)
	char szAcChinhTen[32]; // tên nhân vật ac chính mà cửa sổ này theo ("" = không)
	// ---- WAuto điền mỗi vòng trước khi gửi IPCGameLoop (không phải cấu hình):
	int  nACLaChinh;       // 1 = cửa sổ này CHÍNH LÀ ac chính (máy AC không làm gì)
	int  nACMap, nACX, nACY;
	UINT uACMucTieu;
	int  nACSong, nACCamp;
	UINT uACTuoi;          // ms kể từ lần WAuto nhận PRG_VITRI cuối của ac chính (0 = chưa có)
};
```
Mặc định trong constructor: `nAcChinhKC = 200`, còn lại 0. `.dat` cũ: di trú theo `offsetof(autoData, bTimAcChinh)` như CT.

### 3.2 `KPlayer.h` — `ExtAuto` thêm cuối: `int nACHold; UINT uACNext; int nACThieu; UINT uACMoveT; int nACMyX, nACMyY; int nACRepath;`
(cầm lái / nhịp / đo kẹt như tầng săn TK). Game.exe không đọc ExtAuto → chỉ CoreClient.

### 3.3 `CoreShell.h` — `ATYPE_ACCHINH` thêm CUỐI enum (sau `ATYPE_CONGTHANH`).


> **Cập nhật 22:30 03/09 (phiên `wauto-ca`, Represent3, nhánh `rep3-0309` + cây `E:\Src_Auto_Ngoai`):** họ đã thêm vào CUỐI `autoData` 3 int
> `bWANpcTheSame, bWAMissle, nWAMissleIndex`, thêm `PRT_HIENTHI` cuối `PROTTOOLID`, struct `IPCHienThi`, `ATYPE_HIENTHI` cuối enum, và chiếm
> `Resource.h` **628–631** (2 ô tab Cơ bản). Hệ quả cho thiết kế này khi thi công:
> 1. Trường AC_* phải nằm **SAU** 3 trường đó (lấy `ipc_shared.h` mới nhất, đủ 3 bản khớp nhau); `ATYPE_ACCHINH` sau `ATYPE_HIENTHI`; `PRG_VITRI` vẫn cuối `PROTGAMEID`.
> 2. Dải ID còn trống chỉ **632–639 (8)** → tab "Ac chính" cần 9 ô: **nâng `IDC_INDEX_END 640 → 700`** (700–715 là khối ngoài dải, không chồng) rồi dùng 632–640.
> 3. Bộ swap của họ (autoData đổi) cũng là 3 tệp; hai bên phải build từ cùng một `main` đã gộp `rep3-0309`, nếu không WAuto/Game/CoreClient lệch đuôi struct.

## 4. Phía game (CoreClient + Game.exe)

### 4.1 Gửi vị trí (CoreShell, chạy trong nhịp `ATYPE_ACCHINH`, mọi cửa sổ)
Mỗi 300 ms: điền `IPCViTri` rồi `SendDataToTool` (hàm sẵn có, cùng đường `IPCMainSync`). Rẻ: 36 byte / 300 ms.
Chỉ gửi khi `pAp->bTimAcChinh || pAp->bCungMucTieu || pAp->nACLaChinh` để cửa sổ không dùng tính năng không tốn gì.

### 4.2 `S3Client.cpp` — cổng
```cpp
int nAC = 0;
if (pApData->bTimAcChinh == 1 || pApData->bCungMucTieu == 1 || pApData->nACLaChinh == 1)
	nAC = OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ACCHINH, (int)pApData);
```
đặt **sau** `nST` và **trước** `nBS`: `nBS = nCT ? nCT : (nTK ? ... : (nST ? nST : nAC))`. Dã Tẩu (`nBS == 0`) tự động nhường khi AC cầm lái.
Trong Tống Kim/Công Thành/Liên đấu: máy sự kiện đang cầm lái nên AC không chạy — riêng Tống Kim xử lý ở 4.4.

### 4.3 `CoreShell.cpp` — `AC_Process(nPlayerIdx, pAp, uCurTime)` trả 0 / 1 / 2
```
if nACLaChinh → gửi PRG_VITRI theo nhịp; return 0            (ac chính không bị điều khiển)
if !bTimAcChinh && !bCungMucTieu → return 0
if szAcChinhTen rỗng || uACTuoi == 0 || uACTuoi > 5000 → return 0    (không có/mất ac chính: auto thường)
if !nACSong → return 0                                            (ac chính chết: đứng đánh bình thường, không lao theo xác)
if nACMap != map mình:
     bAcChinhVaoMap ? (đợt 2: đi Xa Phu/THP tới map đó, khuôn LD_DiThanh) : return 0
if !bAcChinhThanh && map mình là THÀNH (g_MoveStation có map) → return 0
d = khoảng cách tới (nACX, nACY)
// (a) cùng mục tiêu — ưu tiên trước cả đi theo, để ac phụ nhập trận ngay khi tới tầm
if bCungMucTieu && uACMucTieu:
     idx = NpcSet.SearchID(uACMucTieu)
     if idx > 0 && Npc[idx] sống && GetRelation(mình, idx) == enemy (hoặc kind_player khác camp trong TK):
          ea.uNpcID = uACMucTieu; ea.uACNext = now+300; return 2       (máy PK đánh, đuổi theo như tiền lệ TK)
// (b) đi theo
if d > nAcChinhKC:
     đích = điểm trên đoạn (mình → ac chính) cách ac chính 150 mps (Thái: -150), lệch góc theo chỉ số cửa sổ ±40 mps để 3–4 ac phụ không chồng ô
     DT_WalkTo(đích, 100); đo kẹt: 3 s không nhúc nhích → RemoveFlag + FindPath lại (2 lần) → bỏ 10 s
     nACHold = 1; return 1
nACHold = 0; return 0                                             (đủ gần: auto thường đánh quanh, nhặt đồ)
```
Nhịp 300 ms (`uACNext`). Ngựa: qua `DT_WalkTo` → theo luật đợt 4 (đường xa tự lên, gặp địch xuống, cấu hình xuống thì không lên).

### 4.4 Tống Kim (chỉ khi ac phụ bật "Tìm ac chính" và ac chính cùng map trận)
Trong `TKP_FIGHT`, giữa bước "săn người khác màu" và "vị trí địch máy chủ báo": nếu có ac chính hợp lệ cùng map → **đi theo ac chính thay cho rảo/hỏi máy chủ** (cùng khuôn (b)); "cùng mục tiêu" chèn TRƯỚC bước 1 (tướng). Ac chính vẫn chạy toàn bộ máy TK của nó. Đúng luật Thái: "check Tìm ac chính thì chỉ ac chính chạy toạ độ".

## 5. Phía WAuto.exe

### 5.1 Nhận `PRG_VITRI` (`ProcIpcCommand`)
Ghi vào `gameNode.player`: `nMapId, nX, nY` (đã có), thêm `uMucTieu, bSong, nCamp, uViTriT = timeGetTime()`. Kiểm `p->Size >= sizeof(IPCViTri)`.

### 5.2 Ghép cho ac phụ (ngay trước `memcpy(&gl.setting, &gnode.apdata)` WAuto.cpp:3365)
```
if (apdata.szAcChinhTen[0]) {
    main = node có player.dwPID && strcmp(player.szName, szAcChinhTen) == 0
    apdata.nACLaChinh = (main == &gnode)
    if (main && main->player.uViTriT) { nACMap/X/Y, uACMucTieu, nACSong, nACCamp = main->player.*; uACTuoi = now - uViTriT (≥1) }
    else uACTuoi = 0
}
```
Rẻ: quét ≤ vài chục node mỗi 54 ms.

### 5.3 Giao diện
- **Tab mới "Ac chính"** (tab thứ 16, nhóm *Chiến đấu*), 9 control: dùng 632–640 sau khi nâng `IDC_INDEX_END` lên 700 (xem cập nhật 22:30):
  `[v] Tìm ac chính` · `Khoảng cách [200]` · `[v] Tìm trong thành` · `[v] Đánh cùng mục tiêu ac chính` · `[ ] Ac phụ tự vào map ac chính` (đợt 2, mờ) ·
  `Ac chính: [combo tên các cửa sổ đang đăng nhập + "(không)"]` · dòng trạng thái *"Ac chính: X · map Y · cách Z · mục tiêu N"* (từ `uACTuoi`).
- Menu chuột phải ở danh sách nhân vật: **"Đặt làm ac chính cho tất cả cửa sổ khác"** → ghi `szAcChinhTen` cho mọi node ≠ node này, `SaveRoleDataFast`.
- Lưu: `SaveRoleData` (ô) + `.dat` di trú `offsetof(bTimAcChinh)`; `UpdateUI`; tooltip. `WA_SO_TAB 15 → 16`, `s_aTabDay`, `ShowTab`, `nGrpId`.

## 6. Tình huống biên

| Tình huống | Xử lý |
|---|---|
| Ac chính thoát game / mất kết nối | `dwPID == 0` → `uACTuoi = 0` → ac phụ về auto thường; trạng thái "Ac chính: mất kết nối" |
| Ac chính chết | `nACSong == 0` → không theo, không nhận mục tiêu; hồi sinh thì theo lại |
| Ac chính đổi map | `nACMap ≠` → đứng lại (đợt 1) / đi Xa Phu (đợt 2) |
| Ac chính trong thành, ô "Tìm trong thành" tắt | không theo (đúng luật Thái, tránh Thổ địa phù về hàng loạt) |
| Mục tiêu ac chính ngoài vùng đồng bộ của ac phụ | `SearchID` = 0 → đi theo ac chính trước, tới tầm mới nhận |
| Nhiều ac phụ chồng ô | điểm đích lệch góc theo chỉ số cửa sổ |
| Tường chắn / kẹt | đo kẹt 3 s + repath 2 lần + bỏ 10 s (khuôn `TK_SanNguoi`) |
| WAuto.exe cũ + CoreClient mới | đuôi `autoData` = 0 → máy AC im; CoreClient cũ + WAuto mới: trường mới bị bỏ qua |
| Ac chính tự là ac phụ của người khác (vòng) | `nACLaChinh` tính theo tên chính xác; cấm chọn chính mình trong combo |

## 7. Chi phí, tệp, kiểm thử

- Tệp đổi: `ipc_shared.h` (3 bản: Core, E:\Src_Auto_Ngoai, WAutoUI), `KPlayer.h`, `CoreShell.h`, `CoreShell.cpp`, `S3Client.cpp`, `WAuto.cpp/.rc/Resource.h`.
  **Swap 3 tệp cùng lúc**: `CoreClient.dll.moi` + `Game.exe.moi` + `WAuto.exe.moi` (đổi tay WAuto). Bộ vá idempotent riêng `ReverseTools\goi_va_wauto_acchinh_0309.py`.
- CPU/mạng: +36 byte/300 ms mỗi cửa sổ qua shared memory; 0 gói mạng game; máy AC tick 300 ms.
- Test: 2 cửa sổ cùng máy: (1) kéo ac chính đi 500 mps → ac phụ theo, dừng cách ~150; (2) ac chính đánh quái → ac phụ đánh cùng con; (3) ac chính chết → ac phụ đứng đánh thường; (4) ac chính về thành, ô "Tìm trong thành" tắt → ac phụ không theo; (5) Tống Kim 2 acc cùng phe: ac phụ theo ac chính thay rảo. Log: `[AC] theo (x,y) d=... ` / `[AC] muc tieu id=...` / `[AC] mat ac chinh`.
- Ước lượng: 1 buổi thi công + 1 buổi test với chủ.

## 8. Chờ chủ quyết (mặc định đề xuất in đậm)

1. **Phạm vi đợt 1 = các cửa sổ trên CÙNG một máy tính** (WAuto quản). Nhiều máy tính (máy chính/máy phụ qua mạng như auto Thái) để đợt sau — cần thêm TCP giữa hai WAuto.
2. Khác map: **đợt 1 chỉ theo khi cùng map**; "Ac phụ tự vào map ac chính" (đi Xa Phu / Thần Hành Phù) làm đợt 2.
3. Giao diện: **tab mới "Ac chính"** trong nhóm Chiến đấu (9 ô, vừa dải ID còn trống) — hay nhét vào tab PK?
4. Tống Kim: **ac phụ theo ac chính thay cho rảo/hỏi máy chủ** khi bật "Tìm ac chính" (ac chính vẫn chạy máy TK đầy đủ)?
5. Chọn ac chính: **combo tên trong tab (lưu theo nhân vật) + chuột phải "Đặt làm ac chính cho tất cả"**. Có cần "Ac chính dự phòng" (Thái: đổi ac chính khi bấm phím) ngay không?
6. Số mặc định: **khoảng cách 200 mps, dừng cách 150, ac chính mất tin 5 s coi là mất**; nhịp 300 ms.

Chủ gật (hoặc đổi số) là tôi thi công theo đúng bảng trên.
