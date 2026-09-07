# LƯỠNG NGHI CHÂN KHÍ (CÔN LÔN 120) — "KHIÊN CÒN MÀ VẪN CHẾT" — MỔ LINUX SO DỰ ÁN — 07/09/2026

Chủ game: "kiểm tra skill Lưỡng Nghi Chân Khí Côn Lôn; khi nhảy skill Lưỡng Nghi Chân Khí vẫn còn thì cũng bị đánh chết; đọc bản Linux kiểm tra lại".

## 1. Cơ chế (giống nhau ở hai bản về dữ liệu)

| Id | Tên | Kiểu | Nội dung |
|---|---|---|---|
| 717 | Lưỡng Nghi Chân Khí (bị động 120, bảng `kunlun120`) | 3 | `autorescueskill = {721×256+cấp}, {−1}, {20×18×256 + xác suất 15→65 %}`: khi **đòn vừa nhận đưa máu từ ≥ 25 % xuống < 25 %**, tung xác suất ⇒ tự phát chiêu 721 cùng cấp, hồi 20 s |
| 721 | Lưỡng Nghi Chân Khí – Ma Pháp Đôn (`kunlun120mofadun`) | Linux **14**, dự án 2 (đạn tại chỗ `SKILL_MF_AtFirer`) | `staticmagicshield_p = 1800 → 10 050 (%)`, 5 → 10 s; `StartEvent → 722` |
| 722 | – Gia Tốc (`kunlun120jiasu`) | 14 / 2 | `fastwalkrun_p 5 → 36 %`, 3 → 10 s |

Khiên tĩnh (`staticmagicshield_p`) = **nội lực tối đa × % / 100** điểm (cấp 20: 100 lần nội lực tối đa), handler Linux `0x08096DC0` = dự án `KNpcAttribModify::StaticMagicShieldP` (đã làm chuẩn đợt HOTHAN 01/09). Khiên trừ dần theo sát thương; hết khiên thì sát thương còn lại mới trừ máu. Kiểu chiêu 14 của Linux (`KSkill::Cast → 0x080EA720`) = phát `StartSkillId` rồi tạo một "đạn tại chỗ" và kích hoạt ngay trong cùng lời gọi; dự án dùng kiểu 2 + dạng đạn tại người phát — cùng bản chất.

Linux **không** gỡ trạng thái khi khiên vỡ: vòng `0x08089D8D` duyệt danh sách toàn cục `0x8fbfe64` gọi `RemoveState`, nhưng không có chỗ nào trong nhị phân ghi vào danh sách đó (rỗng) ⇒ biểu tượng LNCK vẫn còn sau khi khiên hết ở cả hai bản. "Nhảy skill" trong câu hỏi hiểu là chiêu **tự kích hoạt** (autorescue); nhảy khinh công không đụng tới khiên (`DoJump` hai bản không xoá trạng thái).

## 2. Lệch thật — khiên tĩnh của dự án **không chặn độc / cháy theo nhịp và sát thương phản đòn**

| | Linux | Dự án (trước) |
|---|---|---|
| Khối khiên tĩnh trong `CalcDamage` | `0x08089D5D`: `if (khiên > 0) { if (dmg < khiên) { khiên −= dmg; log "StaticMagicShield: Use=%d Rest=%d"; return 1 } else { dmg −= khiên; khiên = 0 } }` — **không xét** cờ `bReturn` `[ebp+0x2c]` hay loại sát thương | `KNpc.cpp:4151`: `if (!bReturn && khiên > 0 && dmg > 0)` |
| Nhịp độc | `0x0808BDAA → CalcDamage(type 4 = độc, min = max = v0, [esp+0x24] = 1)` ⇒ **vẫn bị khiên chặn** | `KNpc.cpp:1499` gọi `CalcDamage(…, damage_poison, −1, FALSE, FALSE, TRUE)` ⇒ `bReturn = TRUE` ⇒ **xuyên khiên** |
| Nhịp cháy (`m_BurnState`) | như trên | `KNpc.cpp:1527` `bReturn = TRUE` ⇒ xuyên khiên |
| Sát thương phản đòn (type 6, gọi đệ quy `0x0808A1CF`) | qua khiên | xuyên khiên |

Hệ quả: Côn Lôn có Lưỡng Nghi Chân Khí đang sáng (khiên còn hàng chục lần nội lực) vẫn chết vì **độc Ngũ Độc / cháy** đếm nhịp — đúng hiện tượng chủ tả. Đòn thường, chiêu, chí mạng đều qua `CalcDamage` với `bReturn = FALSE` nên vẫn bị chặn như Linux.

Ghi chú thêm: `CalcDamage` Linux trả 0 (không sát thương gì) khi `KNpc+0x18e0 != 0` = thuộc tính `ignoredamage` (idx 217, handler `0x08095D50`) — không liên quan 721.

## 3. Thi công `[LNCK 07/09]` commit **bc1a8224** origin/main

`KNpc.cpp:4151` bỏ điều kiện `!bReturn` ⇒ khiên tĩnh chặn mọi sát thương đi qua `CalcDamage` (đòn thường, chiêu, độc/cháy theo nhịp, phản đòn) như Linux. Gói `ReverseTools/goi_va_lnck_0709.py`. Cùng mã CoreServer + CoreClient (client chỉ hiển thị, nhưng build cả hai như thường lệ). Không đổi dữ liệu (721/722/717 hai bản giống nhau). Áp cho mọi chiêu có `staticmagicshield_p`: 721 Côn Lôn, 2134 Phất Y (Vũ Hồn), 2139 Tiêu Dao Vũ.

Kết quả mong đợi: đang có Lưỡng Nghi Chân Khí thì độc/cháy chỉ làm mòn khiên, không mất máu cho tới khi khiên vỡ; log `[HOTHAN] … khien tinh hap thu tron` xuất hiện cả ở nhịp độc.

**Trạng thái 11:22 07/09**: build từ origin/main `bc1a8224` (gộp TRANPHAI60 `962a8255`, DELTA f, BANGSAT2; compile 0 lỗi, link thật): `bin/server/CoreServer.dll.moi` = **a71d3305** (18 482 176), `bin/client/CoreClient.dll.moi` = **90eeaa8d** (2 611 712) — khe trước đó trống (live server e8130f46 từ 11:02, client 7bcf5119). Bản sao `.moi.lnck_*`. Đã báo DELTA + TRANPHAI60/MATDO. Chờ chủ swap (server đủ; client cùng mã nhưng không bắt buộc cho tính năng này).

**11:3x**: phiên TUKICH thay khe client bằng `15bd934d` (origin/main `e2a03fc0`, chứa `bc1a8224` ⇒ vẫn có LNCK; bản 90eeaa8d giữ tên `.moi.lnck_*`); khe server vẫn `a71d3305`. Build sau: base ≥ `e2a03fc0`.

**11:38**: DELTA đợt g thay cả hai khe: `CoreServer.dll.moi` = `1d06f7f0` (origin/main `be72d6b6`, đã kiểm chứa `bc1a8224` ⇒ LNCK còn nguyên; bản a71d3305 giữ tên `.moi.lnck_*`), `CoreClient.dll.moi` = `1ca74e6e` (⊇ TUKICH). Chờ chủ swap cả hai.


---

# 4. "Lưỡng Nghi còn 8 s vẫn bị đánh chết" — đo trên live (14:xx 07/09, bản 9408925d đã có [LNCK])

`jx_auto_server.log` (AutoLog đang bật) ghi khiên tĩnh của nhân vật `pidx=1` (cấp 156, máu 17 795, `Npc 91527`) — 5 lần Lưỡng Nghi Chân Khí bật rồi vỡ trong ~4 phút:

| t (ms) | Sự kiện |
|---|---|
| …547 801 | khiên hấp thụ trọn 3, **còn 156 397** (khiên vừa bật = 156 400) |
| …548 194 (+0,4 s) | **khiên vỡ**: đòn 8 770 > khiên còn 6 568 |
| …577 414 (+29 s, bật lại sau hồi 20 s) | hấp thụ 4, còn 156 396 |
| …578 362 (+0,9 s) | vỡ: 6 732 > 405 |
| …717 588 | hấp thụ 8 770, còn 147 630 |
| …718 023 (+0,4 s) | vỡ: 6 732 > 4 678 |
| …757 751 / …758 035 | hấp thụ 968 còn 155 432 → vỡ 8 770 > 1 410 |
| …790 144 / …790 299 | hấp thụ 2 332 còn 154 068 → vỡ 1 736 > 214 |

Kết luận: khiên **hoạt động đúng** (hấp thụ trước kháng, trừ dần, vỡ khi hết), nhưng **bể chỉ có 156 400 điểm** = nội lực tối đa 8 689 × 18 (`staticmagicshield_p = 1800` ⇒ Lưỡng Nghi Chân Khí 717 đang ở **cấp 1**), và nhân vật bị nhiều bot đánh dồn ~6 700–8 800 mỗi đòn ⇒ ~17 đòn trong 0,4 s làm cạn bể trong **dưới 1 giây**, trong khi biểu tượng vẫn đếm 10 s. Linux cùng công thức (`+0x1468 += max(manamax Âm +0x1a1c, Dương +0x1a20) × v0 / 100`, handler `0x08096DC0`, tiêu hao `0x08089D5D` cùng trước kháng) và cũng **không** gỡ biểu tượng khi khiên vỡ ⇒ Linux trong cùng tình huống (bị đánh dồn) cũng chết như vậy. "Bất tử 10 s" chỉ đúng khi tổng sát thương nhận trong 10 s < bể: cấp 1 = 18 lần nội lực, cấp 20 = 100 lần (nhân vật này ≈ 869 000).

Không có đường sát thương nào khác né khiên ngoài các đường Linux cũng né: chí tử (25 % máu hiện tại, ghi thẳng máu `0x0808B0F8` = `KNpc.cpp:5057`), `life_v` tức thời âm, rút máu theo nhịp (`lifereplenish_v` âm). Độc/cháy theo nhịp và phản đòn đã qua khiên từ bản [LNCK].

Nếu chủ muốn Lưỡng Nghi Chân Khí "bất tử" thật khi bị đánh dồn: nâng cấp kỹ năng 717 (cấp 20 → 100 lần nội lực) hoặc đổi dữ liệu `staticmagicshield_p` (ví dụ 100000 = 1 000 lần) — đây là **thay đổi ngoài Linux**, chờ chủ quyết; có thể thêm tuỳ chọn gỡ biểu tượng 721 khi khiên vỡ để người chơi khỏi hiểu nhầm (Linux không gỡ).
