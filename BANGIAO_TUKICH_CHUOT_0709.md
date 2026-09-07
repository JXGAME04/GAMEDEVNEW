# BÀN GIAO — TỰ KÍCH CHUỘT ĐÁNH QUÁI: quái chết thì ĐỨNG YÊN (07/09/2026, 11:30)

> Chủ báo: *"kiểm tra lại tự kích chuột đánh quái · đang bị tình trạng khi tự kích đánh quái thì
> khi quái chết - hoặc rớt item thì player tự chạy tới vị trí quái hoặc item (không mở WAuto)"*.

---

## 1. Trạng thái — CHỜ SWAP, chỉ **một** tệp

| Tệp | md5 | cỡ (byte) | Nội dung |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | `15bd934d` | 2.612.736 | `origin/main` `0747c8e6` **+** commit `e2a03fc0` (vá này) |

Bản đang chạy: `CoreClient.dll` `7bcf5119` · `Game.exe` `610a1b63`.

**Cách đổi:** thoát hẳn `Game.exe` → chạy `bin\client\ChoiGame.bat`. **Không** cần đổi `Game.exe`
hay `WAuto.exe` (vá chỉ nằm trong CoreClient, không đụng gói tin / cấu trúc / vtable).

### Khe `.moi` dùng chung với phiên khác — đã kiểm

- Khe trước đó là `90eeaa8d` của phiên **LNCK** (đặt 11:19). Bản lưu của họ
  `CoreClient.dll.moi.lnck_90eeaa8d_1122` vẫn còn nguyên, tôi không đụng.
- `15bd934d` là **siêu tập** của `90eeaa8d`: nó build từ `origin/main` `0747c8e6`, mà `0747c8e6`
  đã chứa `bc1a8224` (LNCK — `KNpc.cpp::CalcDamage` bỏ điều kiện `!bReturn`). Kiểm bằng
  `git diff HEAD -- Sources/` = **chỉ 114 dòng thêm của tôi**, không xoá dòng nào.
- `bin\server\CoreServer.dll.moi` (`a71d3305`) là của họ — **không đụng**; vá này thuần client.
- Đã nhắn phiên LNCK. Ai đặt lại `.moi` phải merge `origin/main` ≥ `e2a03fc0` trước.

---

## 2. Gốc — hai đường, đều là mã gốc JX1 (dự án chưa từng đụng: `git log` ba tệp đều sạch)

`Ui\autoexec.lua:15` gán `LButton` → `Mouse_Action()` (`Sources\S3Client\Ui\ShortcutKey.cpp:1681`).
Mỗi cú kích chuột trái làm **ba việc, việc thứ ba là VÔ ĐIỀU KIỆN**:

```
Mouse_Action(x,y)
 ├─ FindSelectNPC(x,y)    → địch  → LockSomeoneUseSkill → Npc[tôi].m_nPeopleIdx = quái
 │                        → không → LockSomeoneAction(0) → m_nPeopleIdx = 0
 ├─ FindSelectObject(x,y) → có đồ → LockObjectAction(idx) → m_nObjectIdx = đồ
 └─ GotoWhere(x,y,0)      ← LUÔN chạy → SendCommand(do_run) + SendClientCmdRun(x,y)
```

Rồi mỗi nhịp `KNpcAI::ProcessPlayer()` (`KNpcAI.cpp:796`, client) chạy hai máy:
`FollowPeople(m_nPeopleIdx)` (trong tầm → `do_skill`, ngoài tầm → **`do_run` tới quái**) và
`FollowObject(m_nObjectIdx)` (chỉ **nhặt** khi đã ở trong 75 đơn vị, tự nó không di chuyển).

### 2.1 Cú kích kế tiếp rơi vào chỗ trống — thủ phạm chính khi tự kích chuột

Con trỏ vẫn nằm nguyên chỗ con quái. Quái chết:

- **không rớt đồ** → `FindSelectNPC` / `FindSelectObject` đều rỗng, nhưng `GotoWhere` vẫn gửi
  `do_run` → chạy tới đúng ô quái vừa chết;
- **có rớt đồ** → `LockObjectAction(đồ)` **+** `GotoWhere` → chạy tới món đồ, tới nơi
  `FollowObject` → `CheckObject` → `PickUpObj` nhặt luôn.

Với người bấm tay một cái thì đây là "bấm đâu đi đó" đúng ý; với tự kích chuột thì thành phiền.

### 2.2 Lệnh chạy treo không ai huỷ khi mục tiêu chết

`KNpc::DoDeath` (nhánh `#ifndef _SERVER`, `KNpc.cpp:1850`) và `KNpcAI::FollowPeople`
(`KNpcAI.cpp:864`) khi mục tiêu chết **chỉ** đặt `m_nPeopleIdx = 0` rồi `return` — **không gửi lệnh
dừng**. Cái `do_run(vị trí quái)` đã gửi cho máy chủ vẫn còn hiệu lực → quái chết giữa lúc đang
đuổi thì nhân vật chạy nốt tới xác. Đường này dính **kể cả khi không tự kích chuột**.

### 2.3 Đã loại trừ

| Nghi can | Kết luận |
|---|---|
| WAuto | Không dính: WAuto/auto trong game đi qua `KPlayerAuto::MoveTo` và `GotoWhereDirect`, **không** qua `GotoWhere`. |
| Auto trong game (`KUiAuto`/`KUiFight`) | `PlayerAuto()` chỉ chạy khi `m_bActiveAuto` (nút Bắt đầu). `UserData\*\UiAutoConfig.ini` không có khoá `AutoAttack` → mặc định 0. |
| Tự đặt `m_nObjectIdx` khi đồ rơi | Không có: quét toàn bộ phép gán `Npc[].m_nObjectIdx` chỉ thấy `LockObjectAction` và `KPlayer::ProcessMouse` — đều phải có cú kích chuột. |

---

## 3. Chủ quyết (07/09)

1. **"Đứng yên hẳn, không nhặt"** — quái chết thì huỷ lệnh chạy treo **và** bỏ qua cú kích rơi
   đúng ô quái vừa chết, kể cả khi ở đó có đồ rơi.
2. **"Luôn bật"** — không thêm ô tuỳ chọn.

---

## 4. Đã làm (commit `e2a03fc0`, bộ vá `ReverseTools\goi_va_tukich_chuot_0709.py`)

| Tệp | Nội dung |
|---|---|
| `Core\Src\CoreShell.cpp` | Chốt "ô quái vừa chết": `DEAD_TGT_GUARD_MS = 2000` ms, `DEAD_TGT_GUARD_RANGE = 96` (~3 ô). `DeadTargetGuardBlock()` + `g_OnLockedTargetDead()`. `GotoWhere` và `LockObjectAction` bỏ qua lệnh nhắm vào vùng chốt. |
| `Core\Src\KNpc.cpp` | `DoDeath`, nhánh `#ifndef _SERVER`: mục tiêu đang khoá chết → gọi `g_OnLockedTargetDead(m_Index)` trước khi xoá khoá. |
| `Core\Src\KNpcAI.cpp` | `FollowPeople` nhánh `do_death/do_revive`: gọi thêm `g_OnLockedTargetDead(nIdx)` (dự phòng khi `DoDeath` không chạy). |

### Luật của chốt

- **Bật** khi con quái *đang bị khoá* chết. Ghi lại toạ độ mps của nó + `GetTickCount()`.
- **Huỷ lệnh chạy treo**: nếu lúc đó nhân vật đang `do_walk`/`do_run` thì gửi *"đi tới chỗ mình
  đang đứng"* (`SendCommand(do_walk, x, y)` + `SendClientCmdWalk`) — đúng khuôn đã có sẵn ở
  `KNpcAI::FollowPeople` nhánh gặp NPC đối thoại.
- **Chặn**: mọi `GotoWhere` / `LockObjectAction` nhắm vào trong 96 đơn vị quanh ô đó.
- **Mỗi lần chặn thì gia hạn** → tự kích chuột bấm mãi một chỗ **không bao giờ** kéo được nhân vật
  tới. Ngừng bấm 2 giây là chốt tự hết → bấm lại vẫn nhặt đồ bình thường.
- **Gỡ chốt ngay** khi: nhắm ra ngoài vùng (người chơi chủ động đổi chỗ), **hoặc** đang khoá một
  mục tiêu còn sống (lệnh đó là lệnh áp sát, phải giữ).

### Không đụng tới

`FollowPeople` áp sát mục tiêu còn sống · WAuto và auto trong game (`MoveTo` / `GotoWhereDirect`) ·
mọi đường đánh nhau · `GotoWhereDirect` (đường tìm đường của auto) · máy chủ.

---

## 5. Cách kiểm sau swap

1. Vào map luyện công, **giữ / tự kích chuột** vào một con quái ở xa cho tới khi nó chết.
   Kỳ vọng: quái chết → nhân vật **đứng lại tại chỗ**, không chạy tới xác, không chạy tới đồ rơi.
2. Ngưng bấm ~2 giây rồi bấm vào món đồ → **vẫn nhặt được** (chốt đã hết hạn).
3. Bấm sang con quái khác ngay sau khi quái cũ chết → **vẫn chạy tới đánh bình thường**
   (chốt tự gỡ vì có mục tiêu còn sống).
4. Nhật ký: `findstr /C:"[TUKICH]" bin\client\jx_auto.log`
   - `[TUKICH] muc tieu N chet luc dang duoi -> dung tai (x,y)` = đã huỷ lệnh chạy treo;
   - `[TUKICH] bo qua lenh nham o quai vua chet: diem=(..) oquai=(..)` = đã chặn cú kích.

---

## 6. Bẫy / điểm còn hở

1. **Đổi map trong vòng 2 giây sau khi giết quái**: toạ độ mps ghi trong chốt là của map cũ; nếu
   điểm bấm ở map mới tình cờ rơi trong 96 đơn vị của toạ độ đó thì một cú kích bị bỏ qua (tối đa
   2 giây, tự hết). Xác suất rất nhỏ, chưa vá để giữ thay đổi tối thiểu.
2. **Đang kẹt khung hình đánh (`IsCanInput()` = FALSE)** thì `LockSomeoneUseSkill` trả 0 →
   `m_nPeopleIdx` = 0 → nếu con quái mới đứng đè lên ô quái cũ trong 2 giây thì cú kích đó bị bỏ
   qua. Hết khung hình là khoá được ngay nên thực tế chỉ mất vài phần mười giây.
3. `DEAD_TGT_GUARD_RANGE = 96` (~3 ô) chọn để bao cả xác quái lẫn đồ rơi vung quanh. Nếu chủ thấy
   rộng/hẹp quá thì đổi một số ở `CoreShell.cpp`, build lại CoreClient là xong.
4. **Khe `.moi` dùng chung**: xem mục 1. Kiểm md5 trước khi đặt, giữ bản của phiên khác bên cạnh.
5. Bộ vá lùi được: `python ReverseTools/goi_va_tukich_chuot_0709.py --lui`
   (bản gốc nằm ở `*.truoc_tukich_0709`).

---

## 7. Chưa test thật

Mới build xong (`Client Release|Win32` Core: COMPILE PASS / LINK PASS) và đặt `.moi` lúc 11:27 —
**chủ chưa swap, chưa đánh thử**. Mục 5 là các bước cần chạy khi vào game.
