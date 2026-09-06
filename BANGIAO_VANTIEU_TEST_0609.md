# BÀN GIAO — VẬN TIÊU LONG MÔN TIÊU CỤC, SẴN SÀNG TEST (06/09/2026)

Nhánh `vantieu-0509`, worktree `D:\GAMEDEVNEW_wt_vantieu`.

---

## 1. CÁCH CHẠY

Máy chủ đang chạy. Cần **tắt GameServer rồi chạy lại** để nạp bản mới:

```bat
cd /d E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server
ChayGameServer.bat
```

Bat tự đổi `CoreServer.dll.moi` thành `CoreServer.dll` (bản cũ giữ thành `.truoc`).

**Đã đặt sẵn**: `CoreServer.dll.moi` 18.446.336 byte, mã băm đầu `8af0c392`.
Đã đối chiếu có đủ mọi tính năng đang chạy (Lua 5.4, Chiến Lệnh, Đấu Giá, S13, cấu hình web, RelayRpc) và thêm xe tiêu.

**Muốn lùi**: đổi `CoreServer.dll.truoc` về `CoreServer.dll`, khởi động lại. Script mới nằm im, không chạy.

---

## 2. TEST NHANH AI BÁM CHỦ (5 phút, làm trước)

Đây là phần chú yêu cầu. Test bằng bộ thử, chưa cần NPC hay vật phẩm.

Gọi từ lệnh bài admin hoặc dòng lệnh GM:
```
DynamicExecute("\\script\\event\\lmbiaoche\\thu_xetieu.lua", "XT_Tao")
```

| # | Việc | Kết quả đúng |
|---|---|---|
| 1 | `XT_Tao` | Hiện "Đã tạo Tiêu Xa, chỉ số NPC = N". Xe xuất hiện cạnh chú, **có thanh máu, đánh được** |
| 2 | Đi bộ ra xa | Xe đuổi theo khi cách hơn khoảng 6-7 ô, dừng lại khi đã gần |
| 3 | Chạy thật xa trong cùng bản đồ | Xe ngừng đuổi khi quá xa, nhật ký ghi `OnBiaoCheFarAwayPlayerDisapper` |
| 4 | Đứng yên 5 phút ở chỗ xa | Xe **biến mất**, nhật ký ghi `OnBiaoCheDisapper` |
| 5 | Giẫm trap sang bản đồ khác | **Xe theo sang**, hiện "Tiêu Xa đã theo ngươi sang bản đồ mới" |
| 6 | Dùng phù về thành | Xe **bị bỏ lại** và bắt đầu đếm 5 phút. Đây là hành vi **cố ý của bản gốc**, không phải lỗi |
| 7 | Chết rồi hồi sinh | Xe **đứng chờ**, không biến mất |
| 8 | `XT_ViTri`, `XT_Song` | Báo đúng bản đồ, toạ độ, trạng thái |
| 9 | Cho quái đánh chết xe | Xe chết là mất hẳn, **không hồi sinh** |
| 10 | `XT_Xoa` | Xe biến mất ngay |
| 11 | Thoát game rồi vào lại trong 5 phút | Xe vẫn còn, bám lại chú |
| 12 | Thoát game quá 5 phút | Xe đã biến mất |

**Nhật ký**: `bin\server\Logs\KSG_YunBiaoLog_<ngày>.txt`

**Nếu có sự cố**: `DynamicExecute("\\script\\event\\lmbiaoche\\thu_xetieu.lua", "XT_Tat")` tắt ngay hệ xe tiêu mà **không phải lùi nhị phân**. Bật lại bằng `XT_Bat`.

Các con số lấy thẳng từ nhị phân Linux: đuổi theo khi bình phương khoảng cách vượt 46224, báo quá xa khi vượt 262143, lạc chủ 300 giây.

---

## 3. TEST VẬN TIÊU BANG (sau khi mục 2 chạy đúng)

45 NPC sẽ tự sinh lúc khởi động: 7 NPC "Tiếp Dẫn áp Tiêu Bang" ở 7 thành, 23 NPC "Nhận Hàng áp Tiêu Bang" ở các hang động, 14 Long Môn Tiêu Sư, 1 ông chủ tiêu cục.

| # | Bước | Kết quả đúng |
|---|---|---|
| 1 | Vào thành có NPC Tiếp Dẫn (Dương Châu, Lâm An, Biện Kinh, Thành Đô, Tương Dương, Phượng Tường, Đại Lý) | **Thấy NPC**. Không thấy là boot script chưa chạy |
| 2 | Bấm NPC khi chưa vào bang | Báo chưa gia nhập bang hội |
| 3 | Bấm NPC khi bang **không chiếm thành đó** | Báo "chỉ tin vào thực lực bang chiếm lĩnh thành" |
| 4 | Bang chủ bấm vào **thứ Bảy hoặc Chủ nhật, 12h-23h** | Có mục "Mở áp Tiêu Bang" |
| 5 | Mở xong, trưởng lão nhận tiêu | Xe 10 sao xuất hiện, loa toàn server |
| 6 | Đứng gần xe 1 phút | Được 4 triệu kinh nghiệm |
| 7 | Đưa xe tới NPC Nhận Hàng đúng tuyến | Nhận 8 Rương Tiêu Vật Bang |

**Lưu ý về điều kiện mở**: chú đã chọn giữ nguyên luật Linux. Nhưng máy chủ hiện **chưa ai ghi biến bang số 1149** (số thành chiếm trong tuần). Chưa đấu dây biến này vào công thành chiến thì **không bang nào mở được**. Đây là việc còn lại, không phải lỗi.

---

## 4. NHỮNG GÌ ĐÃ THAY ĐỔI

**Engine** (`CoreServer.dll`): lớp xe tiêu mới, 6 hàm cho script, 3 hàm engine gọi ngược, AI bám chủ. Kèm ba bản vá lỗi có sẵn: kiểm biên khi đọc mảng người chơi ở hai cổng sát thương, kiểm biên mảng tham số NPC, và hai lệnh chép chuỗi không giới hạn.

**Bảng dữ liệu** (đã ghi, có bản `.truoc_lmbc` để lùi):
- `settings\npcs.txt` server và client: ghi đè 7 hàng vốn là hàng rỗng. Không thêm hay xoá dòng nào.
- `settings\item\magicscript.txt` server và client: nối thêm 10 món ở cuối, đổi tên tại chỗ 3 dòng. Không xoá hay chèn giữa dòng nào.
- `client\settings\NpcRes\npc_res_kind_file_name.txt`: thêm 2 hàng.

**Script**: 46 tệp mới trong `script\event\longmenbiaoju`, `script\activitysys\config\129`, cùng 4 thư viện và 1 boot script. `startgame.lua` thêm 2 dòng.

**Chưa đụng**: hệ vận tiêu cũ `script\event\event_vantieu` vẫn nguyên vẹn và vẫn chạy. Tôi để việc gỡ nó sang bước riêng, làm cùng lúc khi chú xác nhận hệ mới chạy được, để máy chủ không mất tính năng giữa chừng.

---

## 5. NHỮNG ĐIỂM CÒN MỞ

| # | Việc | Ảnh hưởng |
|---|---|---|
| 1 | Biến bang 1149 chưa nối vào công thành chiến | **Chặn** vận tiêu bang. Cần chép logic ghi biến vào đường thắng cuộc của công thành |
| 2 | Nhánh cá nhân 1-9 sao: 48 thành viên và khoảng 30 hằng cân bằng là **tôi tự viết**, không phải số gốc Linux | Chơi được nhưng số thưởng là do tôi đặt. Chú xem lại khi test |
| 3 | 3 món mới (Thề Non Hẹn Biển, Địa Linh Đơn, Lệnh bài Cổ Tháp) chưa có script | Nhận được nhưng bấm không chạy. Script gốc nằm ngoài phạm vi port |
| 4 | Mã 30557 "Túi Dược Phẩm": tên trùng nhưng ảnh khác | Cần chú xác nhận có phải cùng một món |
| 5 | Kỹ năng bị động 1470 của xe tiêu | Bảng NPC của JX1 không có cột này nên mất. Xe vẫn chạy bình thường |
| 6 | Sáu lỗi có sẵn trong mã Linux nhánh cá nhân | Đã xác minh là thật, chưa vá. Không chặn test |

---

## 6. GIT

Nhánh `vantieu-0509` đã đẩy đầy đủ. Nhánh chính **chưa gộp được** vì phiên `wauto-f5` đang sửa dở tệp bàn giao của họ trong thư mục làm việc chung. Khi họ commit xong thì gộp được ngay, không có xung đột mã.
