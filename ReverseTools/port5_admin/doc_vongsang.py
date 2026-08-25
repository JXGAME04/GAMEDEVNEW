# -*- coding: utf-8 -*-
"""[25/08] Them muc 5b (danh hieu + vong sang) vao HUONGDAN_TEST_HOATDONG_2408.md."""
import io, os, shutil

P = r"D:\GAMEDEVNEW\HUONGDAN_TEST_HOATDONG_2408.md"
s = io.open(P, "r", encoding="utf-8", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "5b. TEST DANH HIỆU" in s:
    print("da them truoc do"); raise SystemExit

neo = "## 6. FILE CONFIG TRUNG TÂM"
assert s.count(neo) == 1, "neo = %d" % s.count(neo)

them = "\n".join([
"## 5b. TEST DANH HIỆU + VÒNG SÁNG (Bang Chiến) — menu *9*",
"",
"**Trước khi test phải thoát hẳn game rồi vào lại một lần.** Bảng danh hiệu chỉ được",
"client nạp **một lần duy nhất** lúc khởi động (`KNpc.cpp:6049-6056`), nên 9 danh hiệu",
"mới thêm sẽ không hiện nếu client vẫn đang mở từ trước. Server thì **không cần restart**",
"(engine đọc lại bảng mỗi lần gán danh hiệu).",
"",
"| Bấm menu *9* | Phải thấy gì |",
"|---|---|",
"| Quân hàm 1 — Binh Sĩ | chữ **Binh sĩ** trên đầu + **vòng sáng dưới chân** (skill 830) |",
"| Quân hàm 2 — Hiệu úy | chữ **Hiệu úy** + vòng sáng khác (831) |",
"| Quân hàm 3/4/5 | Thống Lĩnh / Phó tướng / Đại tướng + vòng sáng 832/833/834 |",
"| Danh hiệu 105/106 | **Võ Lâm Đệ Nhất Bang** + vòng tròn Võ Lâm Đại Hội (1169) |",
"| Danh hiệu 199 | **Cao Cấp Đệ Nhất Bang** + vòng tròn 1169 |",
"| Danh hiệu 3000 | **Võ Lâm Minh Chủ** + vòng tròn 1169 |",
"| Gỡ hết | mất sạch chữ lẫn vòng sáng |",
"",
"**Vì sao trước đây báo mà không hiện** — có **hai hệ danh hiệu song song**: script Bang",
"Chiến port từ Linux gọi hệ JX2 (`Title_AddTitle`), hệ này chỉ ghi vào bảng riêng trong",
"DLL và **không đặt biến mà client dùng để vẽ**. Đã bắc cầu sang hệ JX1 (`SetPlayerTitle`)",
"và thêm 9 danh hiệu Bang Chiến vào `settings\\PlayerTitle.txt` (id 287-295, **cả server",
"lẫn client**). Hai bảng đánh số **khác nghĩa nhau** (105 = Đệ Nhất Bang ở bảng JX2 nhưng",
"= *Hiệu úy* ở bảng JX1) nên phải tra bảng `TT_JX1ID`, không gọi thẳng.",
"",
"**Vòng sáng lấy từ đâu** — cột `ExtSkill1`/`ExtSkillLevel1` của chính bảng đó; engine tự",
"thi triển khi gán danh hiệu và **tự cấp lại mỗi lần đăng nhập**. Dùng đúng bộ vòng sáng",
"bản này đã có sẵn: quân hàm 5 cấp = **830-834** (giống quân hàm Tống Kim id 198-202),",
"danh hiệu bang = **1169** (giống id 286 *Quán Quân THĐNB*). **Không thiếu ảnh** — bảng",
"ảnh `settings\\npcres\\state_magic_table_name.txt` đã có sẵn Status136-140 và Status81.",
"",
"**Skill 661 không phải vòng sáng.** Nó là hiệu ứng *may mắn* vẽ **trên đầu**",
"(`lucky.spr`). Bản Linux vẫn gọi nên giữ nguyên cho đúng 100%, chỉ sửa lại lời thông báo",
"cho khỏi hiểu nhầm.",
"",
"Muốn **đổi hoặc bỏ** vòng sáng: sửa thẳng `settings\\PlayerTitle.txt` (cả 2 bản) —",
"đặt `ExtSkill1 = 0` là bỏ. Bảng tra đầy đủ nằm trong `cauhinh_hoatdong.lua`, mục",
"*DANH HIEU + VONG SANG*. Cố ý **không** tạo khoá Lua cho nó để tránh hai nguồn cùng",
"điều khiển một thứ.",
"",
"---",
"",
""])

s = s.replace(neo, them + neo, 1)
q = P + ".truoc_vongsang_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "w", encoding="utf-8", newline="").write(s)
print("da them muc 5b vao HUONGDAN_TEST_HOATDONG_2408.md")
