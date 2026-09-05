# TỪ ĐIỂN CẤU HÌNH GAME (sinh tự động từ cfgw_vietngu - đừng sửa tay)

Mỗi khoá: tên - `KHOÁ` - giá trị hiện tại trong tệp - kiểu - hiệu lực - mức nguy cơ. Web admin hiện đúng nội dung này (bảng `gcfg` cột `ten`, `giai_thich`, `canh_bao`, `nguy_co`).

## Kinh nghiệm (`EXP`) - 29 khoá

Mọi hệ số quyết định tốc độ lên cấp: hệ số nhân của máy chủ (C++), hệ số theo mốc cấp, VIP, chuyển sinh, phạt chênh cấp, chia exp tổ đội và trần exp bảo rương. Đây là nhóm ảnh hưởng mạnh nhất tới nhịp phát triển của cả server.

### Chênh cấp tối đa vẫn nhận đủ exp  `Exp.ChenhCapMax`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi nhân vật cao cấp hơn quái: chênh lệch (cấp nhân vật trừ cấp quái) nhỏ hơn hoặc bằng số này thì nhận đủ exp; chênh nhiều hơn thì exp bị chia cho ChiaKhiChenh. Mặc định 9.
  Nhân vật thấp cấp hơn quái chỉ nhận 1 điểm exp gốc (rồi mới nhân hệ số), trừ khi cả hai đều từ MienTruCap trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt quá lớn (200) là bỏ hẳn phạt chênh cấp: cấp 150 cày quái cấp 20 vẫn ăn đủ exp, mất ý nghĩa bản đồ theo cấp. Khoảng cho phép 0..200.

### Mức chia exp khi chênh cấp quá mức  `Exp.ChiaKhiChenh`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi chênh cấp vượt ChenhCapMax, exp gốc bị chia cho số này. Mặc định 10, tức là còn 10%. Đặt 1 là không phạt.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 1 là bỏ phạt chênh cấp hoàn toàn. Đặt rất lớn (1000) thì đánh quái thấp cấp gần như không có exp, người chơi tưởng lỗi. Khoảng cho phép 1..100000.

### Hệ số exp chuyển sinh, cấp dưới MocCap3  `Exp.CsDuoi`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Thay cho HeSo1..HeSo3 khi nhân vật đủ điều kiện chuyển sinh (xem CsLanToiThieu, CsMap) và cấp nhỏ hơn MocCap3. Mặc định 160. Vẫn được cộng VipCong và nhân ExpRate.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Là hệ số nhân thẳng vào exp (không phải phần trăm). Đặt cao hơn nhiều so với HeSo3 thì người chuyển sinh lên cấp quá nhanh so với người thường ở cùng bản đồ. Khoảng cho phép 1..100000.

### Số lần chuyển sinh tối thiểu để ăn hệ số chuyển sinh  `Exp.CsLanToiThieu`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Nhân vật có số lần chuyển sinh lớn hơn số này (và đang đứng ở bản đồ CsMap) mới dùng hệ số CsDuoi / CsTren. Mặc định 3, tức là từ lần chuyển sinh thứ 4 trở đi.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 0 thì ai chuyển sinh 1 lần cũng ăn hệ số chuyển sinh. Đặt quá cao thì không ai đạt tới và hai hệ số CsDuoi / CsTren vô nghĩa. Khoảng cho phép 0..100.

### Bản đồ áp dụng luật chuyển sinh  `Exp.CsMap`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Mã bản đồ mà nhân vật đã chuyển sinh nhiều lần (hơn CsLanToiThieu) dùng hệ số riêng CsDuoi / CsTren thay cho hệ số theo cấp. Mặc định 341. 0 = không bản đồ nào áp dụng.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Gõ sai mã bản đồ thì luật chuyển sinh âm thầm không chạy ở đâu cả (không có báo lỗi). Mã phải là bản đồ có thật trong MapList.ini. Khoảng cho phép 0..100000.

### Hệ số exp chuyển sinh, cấp từ MocCap3 trở lên  `Exp.CsTren`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Thay cho HeSo4 khi nhân vật đủ điều kiện chuyển sinh và cấp từ MocCap3 trở lên. Mặc định 50, nghĩa là người chuyển sinh cấp cao lên chậm hơn hệ số thường (100) tại bản đồ CsMap.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Là hệ số nhân thẳng vào exp. Đặt cao hơn HeSo4 là đảo ngược ý đồ hãm tốc độ cấp cao. Khoảng cho phép 1..100000.

### Hệ số exp thứ nhất (cấp dưới MocCap1)  `Exp.HeSo1`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Hệ số nhân exp cho cấp dưới MocCap1 (tân thủ). Công thức: exp nhận = exp gốc x ExpRate x (HeSo1 + VipCong). Mặc định gốc 80, nghĩa là nhân 80 lần.
  Không phải phần trăm: 100 là nhân 100 lần chứ không phải giữ nguyên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Hệ số này nhân thẳng vào exp, nhân thêm với ExpRate: tăng gấp đôi ở đây là gấp đôi exp cho cả dải cấp. Đặt quá nhỏ (1..5) thì dải cấp đó gần như không lên nổi, người chơi bỏ game. Khoảng cho phép 1..100000. Đổi xong nhớ theo dõi log exp vài phút.

### Hệ số exp thứ hai (cấp từ MocCap1 đến dưới MocCap2)  `Exp.HeSo2`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Hệ số nhân exp cho cấp từ MocCap1 đến dưới MocCap2. Công thức: exp nhận = exp gốc x ExpRate x (HeSo2 + VipCong). Mặc định gốc 70, nghĩa là nhân 70 lần.
  Không phải phần trăm: 100 là nhân 100 lần chứ không phải giữ nguyên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Hệ số này nhân thẳng vào exp, nhân thêm với ExpRate: tăng gấp đôi ở đây là gấp đôi exp cho cả dải cấp. Đặt quá nhỏ (1..5) thì dải cấp đó gần như không lên nổi, người chơi bỏ game. Khoảng cho phép 1..100000. Đổi xong nhớ theo dõi log exp vài phút.

### Hệ số exp thứ ba (cấp từ MocCap2 đến dưới MocCap3)  `Exp.HeSo3`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Hệ số nhân exp cho cấp từ MocCap2 đến dưới MocCap3 (đang là dải 80 đến 139). Công thức: exp nhận = exp gốc x ExpRate x (HeSo3 + VipCong). Mặc định gốc 280, nghĩa là nhân 280 lần.
  Không phải phần trăm: 100 là nhân 100 lần chứ không phải giữ nguyên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Hệ số này nhân thẳng vào exp, nhân thêm với ExpRate: tăng gấp đôi ở đây là gấp đôi exp cho cả dải cấp. Đặt quá nhỏ (1..5) thì dải cấp đó gần như không lên nổi, người chơi bỏ game. Khoảng cho phép 1..100000. Đổi xong nhớ theo dõi log exp vài phút.

### Hệ số exp thứ tư (cấp từ MocCap3 trở lên)  `Exp.HeSo4`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Hệ số nhân exp cho cấp từ MocCap3 trở lên. Công thức: exp nhận = exp gốc x ExpRate x (HeSo4 + VipCong). Mặc định gốc 100, nghĩa là nhân 100 lần.
  Không phải phần trăm: 100 là nhân 100 lần chứ không phải giữ nguyên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Hệ số này nhân thẳng vào exp, nhân thêm với ExpRate: tăng gấp đôi ở đây là gấp đôi exp cho cả dải cấp. Đặt quá nhỏ (1..5) thì dải cấp đó gần như không lên nổi, người chơi bỏ game. Khoảng cho phép 1..100000. Đổi xong nhớ theo dõi log exp vài phút.

### Cấp miễn phạt chênh cấp  `Exp.MienTruCap`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi cả nhân vật lẫn quái đều từ cấp này trở lên thì bỏ mọi phạt chênh cấp, nhận đủ exp gốc. Mặc định 90 (cấp 90 trở lên đánh boss cấp 95 vẫn đủ exp).
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt thấp (ví dụ 1) là bỏ phạt chênh cấp cho mọi cấp. Đặt cao hơn cấp tối đa thì không ai được miễn. Khoảng cho phép 1..200.

### Mốc cấp thứ nhất của bảng hệ số exp  `Exp.MocCap1`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Nhân vật có cấp nhỏ hơn mốc này (và không rơi vào mốc thấp hơn) dùng hệ số exp HeSo1. Ba mốc phải tăng dần: MocCap1 < MocCap2 < MocCap3; từ MocCap3 trở lên dùng HeSo4. Mặc định gốc 50.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Ba mốc không tăng dần thì có khoảng cấp bị xếp sai hệ số (ví dụ MocCap2 nhỏ hơn MocCap1 làm HeSo2 không bao giờ được dùng). Khoảng cho phép 1..200. Đổi mốc là đổi tốc độ lên cấp của cả một dải cấp, hãy đối chiếu với bốn hệ số bên dưới trước khi lưu.

### Mốc cấp thứ hai của bảng hệ số exp  `Exp.MocCap2`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Nhân vật có cấp nhỏ hơn mốc này (và không rơi vào mốc thấp hơn) dùng hệ số exp HeSo2. Ba mốc phải tăng dần: MocCap1 < MocCap2 < MocCap3; từ MocCap3 trở lên dùng HeSo4. Mặc định gốc 80.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Ba mốc không tăng dần thì có khoảng cấp bị xếp sai hệ số (ví dụ MocCap2 nhỏ hơn MocCap1 làm HeSo2 không bao giờ được dùng). Khoảng cho phép 1..200. Đổi mốc là đổi tốc độ lên cấp của cả một dải cấp, hãy đối chiếu với bốn hệ số bên dưới trước khi lưu.

### Mốc cấp thứ ba của bảng hệ số exp  `Exp.MocCap3`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Nhân vật có cấp nhỏ hơn mốc này (và không rơi vào mốc thấp hơn) dùng hệ số exp HeSo3. Ba mốc phải tăng dần: MocCap1 < MocCap2 < MocCap3; từ MocCap3 trở lên dùng HeSo4. Mặc định gốc 140.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Ba mốc không tăng dần thì có khoảng cấp bị xếp sai hệ số (ví dụ MocCap2 nhỏ hơn MocCap1 làm HeSo2 không bao giờ được dùng). Khoảng cho phép 1..200. Đổi mốc là đổi tốc độ lên cấp của cả một dải cấp, hãy đối chiếu với bốn hệ số bên dưới trước khi lưu.

### Phần trăm exp mỗi người khi tổ đội 2 người  `Exp.ToDoi2`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 2 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 2 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 3 người  `Exp.ToDoi3`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 3 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 3 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 4 người  `Exp.ToDoi4`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 4 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 4 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 5 người  `Exp.ToDoi5`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 5 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 5 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 6 người  `Exp.ToDoi6`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 6 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 6 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 7 người  `Exp.ToDoi7`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 7 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 7 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi tổ đội 8 người  `Exp.ToDoi8`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Khi tổ đội có 8 người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%). Ví dụ 80 = mỗi người nhận 80%. Sau đó mới nhân ExpRate và hệ số cấp.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 100 cho mọi cỡ đội là đi đội 8 người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100.

### Phần trăm exp mỗi người khi số người tổ đội ngoài 2..8  `Exp.ToDoiKhac`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **An toàn**
- Dùng khi số người hưởng exp không rơi vào 2..8 (trường hợp hiếm, ví dụ đội lớn hơn 8 do mở rộng). Mặc định 60. Cách tính giống ToDoi2..ToDoi8.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Bình thường ít khi dùng tới; đặt 100 hoặc 1 đều ít ảnh hưởng nhưng vẫn nên giữ cùng mức với ToDoi8. Khoảng cho phép 1..100.

### Hệ số exp cộng thêm cho VIP  `Exp.VipCong`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Nhân vật đang có trạng thái VIP (cờ VIP = 2) được cộng thêm số này vào hệ số exp theo cấp trước khi nhân: (HeSoN + VipCong). Ví dụ HeSo3 = 280, VipCong = 20 thì VIP nhận 300 thay vì 280 (hơn khoảng 7%). 0 = VIP không hơn gì người thường.
  Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt lớn hơn hệ số theo cấp (ví dụ 280) là VIP nhận gấp đôi người thường, gây mất cân bằng và khiếu nại. Khoảng cho phép 0..100000.

### Hệ số nhân kinh nghiệm toàn máy chủ (C++)  `ServerConfig.ExpRate`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Hệ số nhân cuối cùng cho mọi kinh nghiệm đánh quái, boss và nhiệm vụ cộng qua máy chủ (hàm AddSelfExp).
  Công thức: exp nhận = exp gốc (sau khi chia tổ đội, phạt chênh cấp, buff x2) x ExpRate x (hệ số theo mốc cấp + VIP). Ví dụ ExpRate = 1 và hệ số cấp 80 thì mỗi 100 exp gốc thành 8.000 exp; ExpRate = 2 là gấp đôi toàn bộ.
  Đây là khoá C++, sửa trên web có hiệu lực trong 30 giây, không cần khởi động lại. Muốn chỉnh nhịp lên cấp thì ưu tiên chỉnh khoá này thay vì các hệ số theo cấp.
- **Cảnh báo:** Đây là hệ số nhân trực tiếp, không phải phần trăm: 1 sang 2 là gấp đôi exp của cả server ngay lập tức và exp đã cộng cho người chơi không thu hồi được. Gõ nhầm thêm một số 0 (10 thay vì 1) sẽ làm cả server lên cấp 200 trong vài giờ. Chỉ tăng từng bước nhỏ, đổi xong theo dõi 10 phút. Máy chủ từ chối giá trị ngoài 1..100000.

### Hệ số nhân exp thưởng của script (EXP_RATE)  `GLB_TILE_EXP`
- Giá trị trong tệp: `20` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Hệ số EXP_RATE trong script (lib_server.lua): nhân vào exp thưởng của nhiệm vụ, sự kiện, hoạt động nào có nhân với EXP_RATE. không ảnh hưởng exp đánh quái (phần đó do ServerConfig.ExpRate và nhóm Exp.* của C++ quyết định). Mặc định 20.
  Giá trị được chốt khi nạp lib_server.lua nên đổi trên web chỉ có hiệu lực sau khi khởi động lại máy chủ.
- **Cảnh báo:** Là hệ số nhân thẳng: 20 sang 40 là gấp đôi exp thưởng của mọi nhiệm vụ / sự kiện dùng EXP_RATE. Exp đã phát không thu hồi được. Đặt 0 làm mọi thưởng đó về 0. Cần khởi động lại máy chủ mới có hiệu lực.

### Mốc chuyển sinh để chọn trần exp bảo rương  `BRXP_MOC_CS`
- Giá trị trong tệp: `4`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số lần chuyển sinh nhỏ hơn hoặc bằng mốc này dùng trần BRXP_TRAN_CS4; đúng bằng mốc + 1 dùng BRXP_TRAN_CS5; cao hơn nữa dùng BRXP_TRAN_CS6. Mặc định 4.
  Có hiệu lực trong vòng 1,5 phút (script đọc mỗi lần mở rương).
- **Cảnh báo:** Đặt sai mốc là người chuyển sinh cao bị áp trần thấp hoặc ngược lại. Khoảng hợp lý 0..10.

### Trần exp mỗi ngày từ bảo rương, chuyển sinh thấp (tới mốc)  `BRXP_TRAN_CS4`
- Giá trị trong tệp: `50`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Tổng exp một nhân vật (chuyển sinh thấp (tới mốc)) được nhận trong một ngày từ tất cả chín loại bảo rương, tính bằng triệu exp (50 = 50 triệu). Vượt trần thì mở rương không cộng exp nữa.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đơn vị là triệu: gõ 50000000 nghĩa là 50 nghìn tỷ, coi như bỏ trần. Đặt 0 là mở rương không có exp. Đây là trần kinh tế ẩn chống bơm exp qua rương, hãy giữ ba mức tăng dần.

### Trần exp mỗi ngày từ bảo rương, chuyển sinh bằng mốc + 1  `BRXP_TRAN_CS5`
- Giá trị trong tệp: `80`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Tổng exp một nhân vật (chuyển sinh bằng mốc + 1) được nhận trong một ngày từ tất cả chín loại bảo rương, tính bằng triệu exp (80 = 80 triệu). Vượt trần thì mở rương không cộng exp nữa.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đơn vị là triệu: gõ 50000000 nghĩa là 50 nghìn tỷ, coi như bỏ trần. Đặt 0 là mở rương không có exp. Đây là trần kinh tế ẩn chống bơm exp qua rương, hãy giữ ba mức tăng dần.

### Trần exp mỗi ngày từ bảo rương, chuyển sinh cao hơn  `BRXP_TRAN_CS6`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Tổng exp một nhân vật (chuyển sinh cao hơn) được nhận trong một ngày từ tất cả chín loại bảo rương, tính bằng triệu exp (100 = 100 triệu). Vượt trần thì mở rương không cộng exp nữa.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đơn vị là triệu: gõ 50000000 nghĩa là 50 nghìn tỷ, coi như bỏ trần. Đặt 0 là mở rương không có exp. Đây là trần kinh tế ẩn chống bơm exp qua rương, hãy giữ ba mức tăng dần.

## Exp kỹ năng (`EXP_KYNANG`) - 2 khoá

Tốc độ luyện kỹ năng 90 và 120 khi đánh quái. Đặt 0 là tắt hẳn việc luyện.

### Tốc độ luyện kỹ năng 120  `ServerConfig.Skill120Rate`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Giống Skill90Rate nhưng cho kỹ năng 120: mỗi lần giết quái cộng Skill120Rate x hệ số tăng cường đang có. 0 = tắt hẳn việc luyện kỹ năng 120.
  Bản gốc dùng giá trị từ 100 trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 0 thì kỹ năng 120 không bao giờ lên. Đặt quá cao làm mất giá trị của kỹ năng 120 là mục tiêu dài hạn của người chơi. Khoảng cho phép 0..100000.

### Tốc độ luyện kỹ năng 90  `ServerConfig.Skill90Rate`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Cẩn thận**
- Mỗi lần giết quái, exp kỹ năng 90 đang luyện được cộng = Skill90Rate x hệ số tăng cường đang có trên nhân vật (buff luyện kỹ năng). 0 = tắt hẳn việc luyện kỹ năng 90 (không cộng gì). Bot SimCity không được cộng.
  Bản gốc dùng giá trị từ 100 trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Đặt 0 làm người chơi tưởng lỗi game vì kỹ năng 90 không lên nữa. Đặt quá cao (hàng nghìn) thì kỹ năng 90 đầy trong vài phút, mất ý nghĩa cày. Khoảng cho phép 0..100000.

## Tiền (vạn / lượng) (`TIEN`) - 2 khoá

Hệ số nhân tiền quái rơi và tiền thưởng sự kiện. Đổi ở đây là đổi lạm phát của cả server, hãy tăng từng bước nhỏ.

### Hệ số nhân tiền quái rơi (C++)  `ServerConfig.MoneyRate`
- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **Nguy hiểm**
- Nhân vào số tiền (lượng) rơi ra mỗi khi quái chết. Tiền gốc = exp của quái x MoneyScale của bảng rớt / 100, rồi nhân với khoá này.
  1 = giữ nguyên; 2 = mọi quái rơi gấp đôi tiền. Khoá C++, sửa trên web có hiệu lực trong 30 giây.
- **Cảnh báo:** Tăng khoá này là tăng lạm phát của toàn server: tiền do bot và người cày đổ vào thị trường sẽ nhân lên tương ứng và không rút lại được. Chỉ tăng từng bước (1 sang 2), theo dõi giá chợ vài ngày rồi mới cân nhắc tiếp. Không đặt 0 (máy chủ từ chối, khoảng cho phép 1..100000).

### Hệ số nhân tiền thưởng của script (MONEY_RATE)  `GLB_TILE_TIEN`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Hệ số MONEY_RATE trong script (lib_server.lua): nhân vào tiền thưởng của thư viện câu hỏi, sự kiện Hoa Đăng và các script khác có dùng MONEY_RATE. Không ảnh hưởng tiền quái rơi (do ServerConfig.MoneyRate). Mặc định 1.
  Chốt khi nạp lib_server.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng ở đây là bơm tiền vào server qua sự kiện, gây lạm phát không rút lại được. Đặt 0 làm các thưởng tiền đó về 0. Cần khởi động lại máy chủ mới có hiệu lực.

## Rớt đồ (`ROTDO`) - 40 khoá

Xác suất và số lượng vật phẩm rơi từ quái thường, boss xanh và các nhánh sự kiện (Hiệp Cốt Nhu Tình, Huy Hoàng Đơn, Tiền Đồng, trang bị HKMP...). Danh mục vật phẩm nằm ở settings\droprate\*.ini, không nằm ở đây.

### Hệ số nhân tiền quái thường rơi (script)  `DRQ_HESO_TIEN`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Nhân thêm vào số tiền do script Droprate_normal.lua tính cho quái thường, độc lập với ServerConfig.MoneyRate của C++ (hai hệ số nhân chồng nhau). 1 = giữ nguyên.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Nhân chồng với MoneyRate: đặt 2 ở đây và 2 ở MoneyRate là tiền rơi gấp 4. Gây lạm phát không rút lại được; chỉ tăng từng bước nhỏ.

### Cấp quái xếp vào bậc rớt đồ 1  `DRQ_CAP_BAC_TOITHIEU`
- Giá trị trong tệp: `10` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Quái có cấp dưới số này dùng bảng rớt đồ bậc 1 (bảng thấp nhất). Từ cấp này trở lên, bậc được tính theo cấp quái. Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt quá cao (ví dụ 60) là quái cấp 59 vẫn rơi đồ bậc tân thủ. Phải nhỏ hơn DRQ_CAP_BAC_TOIDA.

### Cấp quái bốc ngẫu nhiên bậc 9 đến 10  `DRQ_CAP_BAC_TOIDA`
- Giá trị trong tệp: `100` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Quái có cấp trên số này không tính bậc theo cấp nữa mà bốc ngẫu nhiên bảng bậc 9 hoặc 10 (hai bảng cao nhất). Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Hạ thấp là quái trung cấp cũng rơi đồ bậc cao nhất, phá giá trang bị. Phải lớn hơn DRQ_CAP_BAC_TOITHIEU.

### Số món boss xanh rơi mỗi lần chết  `DRQ_QUAY_BOSSXANH`
- Giá trị trong tệp: `8`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Mỗi lần boss xanh (boss thường trên bản đồ) chết, script quay bảng rớt đồ đúng số lần này, mỗi lần một món. Mặc định 8.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng lên vài chục là mỗi boss xanh xả cả đống đồ, bot và người săn boss làm ngập chợ. Đặt 0 là boss xanh không rơi gì.

### Mẫu số bốc tiền của boss xanh  `DRQ_MAU_BOSSXANH`
- Giá trị trong tệp: `10`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc tiền khi boss xanh chết: bốc từ 1 đến số này, lớn hơn DRQ_NGUONG_TIEN_BOSSXANH thì rơi tiền. Mặc định 10 (với ngưỡng 8 là khoảng 20%).
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt nhỏ hơn hoặc bằng ngưỡng thì boss xanh không bao giờ rơi tiền. Đặt 0 làm hàm random lỗi trong script (ScriptError). Giữ từ 1 trở lên và lớn hơn ngưỡng.

### Ngưỡng rơi tiền của boss xanh  `DRQ_NGUONG_TIEN_BOSSXANH`
- Giá trị trong tệp: `8`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc được (1 đến DRQ_MAU_BOSSXANH) phải lớn hơn ngưỡng này thì boss xanh mới rơi tiền. Mặc định 8 với mẫu 10 nghĩa là bốc được 9 hoặc 10 mới rơi (khoảng 20%).
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt 0 là boss xanh luôn rơi tiền; đặt bằng hoặc lớn hơn mẫu số là không bao giờ rơi. Cả hai đều đổi kinh tế server.

### Mẫu số bốc rớt của quái thường  `DRQ_MAU_QUAI`
- Giá trị trong tệp: `30`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mẫu số dùng chung cho quái thường: bốc từ 1 đến số này, đúng bằng DRQ_NGUONG_TIEN thì rơi tiền, nhỏ hơn DRQ_NGUONG_ITEM thì quay bảng rớt đồ. Mặc định 30.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đây là nút chỉnh tỉ lệ rớt của mọi quái thường trên mọi bản đồ. Hạ từ 30 xuống 10 là tỉ lệ rớt đồ tăng gấp ba. Đặt 0 làm script lỗi (ScriptError) và quái không rơi gì.

### Số trúng để quái thường rơi tiền  `DRQ_NGUONG_TIEN`
- Giá trị trong tệp: `2`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc được (1 đến DRQ_MAU_QUAI) đúng bằng số này thì quái thường rơi tiền. Mặc định 2 với mẫu 30 nghĩa là khoảng 3,2% số quái chết rơi tiền. Đặt -1 là tắt hẳn rơi tiền.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đây là xác suất trúng một số, không phải ngưỡng: đổi 2 thành 10 không tăng tỉ lệ (vẫn 1/mẫu). Muốn tăng tỉ lệ tiền phải giảm DRQ_MAU_QUAI. Đặt lớn hơn mẫu số là không bao giờ rơi tiền. Đặt -1 để tắt.

### Ngưỡng quay bảng rớt đồ của quái thường  `DRQ_NGUONG_ITEM`
- Giá trị trong tệp: `4`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc được (1 đến DRQ_MAU_QUAI) nhỏ hơn số này (và không trúng nhánh tiền) thì quay bảng rớt đồ theo bậc. Mặc định 4 với mẫu 30: các số 1, 3 là rớt đồ (2 đã dành cho tiền), tức khoảng 2/31.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đây là nút tỉ lệ rớt đồ của toàn bộ quái thường: 4 lên 8 là tỉ lệ tăng gấp đôi trên cả server. Đặt 0 hoặc 1 là quái thường không rớt đồ. Đặt bằng mẫu số là quái nào cũng rớt.

### Số món quái thường rơi mỗi lần trúng  `DRQ_QUAY_QUAI`
- Giá trị trong tệp: `1`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Khi quái thường trúng nhánh rớt đồ, script quay bảng đúng số lần này. Mặc định 1 (một món mỗi lần trúng).
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt 2 là mỗi lần trúng rơi hai món, tương đương tăng gấp đôi sản lượng đồ toàn server. Đặt 0 là không rơi.

### Mẫu số bốc nhánh rớt sự kiện  `DRQ_MAU_SUKIEN`
- Giá trị trong tệp: `4`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc quyết định có gọi nhánh rớt sự kiện (dropeventmap) hay không sau khi quái thường (từ bậc DRQ_CAP_SUKIEN) chết. Mặc định 4.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt 0 làm script lỗi. Đặt nhỏ hơn hoặc bằng ngưỡng sự kiện là mọi quái đủ bậc đều gọi nhánh sự kiện, đồ sự kiện ngập server.

### Bậc quái tối thiểu để có rớt sự kiện  `DRQ_CAP_SUKIEN`
- Giá trị trong tệp: `7` bac · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Chỉ quái thuộc bậc rớt đồ từ số này trở lên (bậc tính theo cấp quái, 1 đến 10) mới có cơ hội gọi nhánh rớt sự kiện. Mặc định 7.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Hạ xuống 1 là quái tân thủ cũng rơi đồ sự kiện, bot cày bản đồ thấp thu đồ sự kiện hàng loạt. Khoảng hợp lý 1..10.

### Ngưỡng gọi nhánh rớt sự kiện  `DRQ_NGUONG_SUKIEN`
- Giá trị trong tệp: `2`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc được (1 đến DRQ_MAU_SUKIEN) nhỏ hơn số này thì gọi dropeventmap (rớt sự kiện trên bản đồ). Mặc định 2 với mẫu 4 (chú thích trong mã ghi khoảng 40%). Đặt -1 là tắt hẳn nhánh sự kiện.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt lớn hơn mẫu số là 100% quái đủ bậc rơi đồ sự kiện. Đặt 0 hoặc 1 là không bao giờ. Đặt -1 để tắt hẳn khi hết sự kiện.

### Mẫu số bốc của nhánh rớt sự kiện bản đồ  `SKD_RANDMAP`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh rớt sự kiện bản đồ, Hiệp Cốt Nhu Tình, Huy Hoàng Đơn, mảnh HKMP: máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 100.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Mẫu số bốc của nhánh Phong Lăng Độ  `SKD_RANDPLD`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh Phong Lăng Độ (hàm thần rồng): máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 100.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Mẫu số bốc của nhánh boss sự kiện  `SKD_RANDOTHER`
- Giá trị trong tệp: `10`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh boss sự kiện (dropeventboss) và Tiền Đồng (dropntiendong): máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 10.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Mẫu số bốc của nhánh Tống Kim  `SKD_RANDTK`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh Tống Kim (hàm thần rồng): máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 100.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Mẫu số bốc của nhánh Vượt ải  `SKD_RANDVA`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh Vượt ải (hàm thần rồng): máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 100.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Mẫu số bốc của nhánh nhiệm vụ dropnvdt / dropnvdt01  `SKD_DROPMDTB`
- Giá trị trong tệp: `95`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mẫu số của phép bốc dùng cho nhánh nhiệm vụ dropnvdt / dropnvdt01: máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định 95.
  Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.
  Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.

### Ngưỡng rớt vật phẩm sự kiện bản đồ  `SKD_MAP_NGUONG`
- Giá trị trong tệp: `80`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi vật phẩm SKD_MAP_ITEM. Mặc định 80 với mẫu 100 = 20%.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt 0 là 100% rơi, đặt từ mẫu số trở lên là không bao giờ rơi. Đây là vật phẩm sự kiện, đặt thấp làm ngập server.

### Mã vật phẩm rớt sự kiện bản đồ  `SKD_MAP_ITEM`
- Giá trị trong tệp: `4854`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã vật phẩm (id trong bảng vật phẩm) rơi ra khi trúng nhánh sự kiện bản đồ. Mặc định 4854.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Mã không tồn tại làm script lỗi (ScriptError) hoặc rơi vật phẩm sai. Chỉ đổi khi chắc chắn mã mới có trong bảng vật phẩm của cả máy chủ lẫn client.

### Ngưỡng rớt trang bị Hiệp Cốt Nhu Tình  `SKD_HCNT_NGUONG`
- Giá trị trong tệp: `50`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi một trang bị Hiệp Cốt Nhu Tình trong dải mã SKD_HCNT_MA_MIN..MAX. Mặc định 50 với mẫu 100 = 50%.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt 0 là luôn rơi. Trang bị này có giá trị cao, hạ ngưỡng là phá giá.

### Dải mã trang bị Hiệp Cốt Nhu Tình, mã đầu  `SKD_HCNT_MA_MIN`
- Giá trị trong tệp: `185`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã vật phẩm nhỏ nhất của dải bốc ngẫu nhiên trang bị Hiệp Cốt Nhu Tình. Mặc định 185 (dải 185..192).
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Mã đầu lớn hơn mã cuối hoặc trỏ vào vật phẩm không tồn tại làm rơi đồ sai hoặc lỗi script. Đổi cả cặp MIN/MAX cùng lúc.

### Dải mã trang bị Hiệp Cốt Nhu Tình, mã cuối  `SKD_HCNT_MA_MAX`
- Giá trị trong tệp: `192`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã vật phẩm lớn nhất của dải bốc ngẫu nhiên trang bị Hiệp Cốt Nhu Tình. Mặc định 192.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Phải lớn hơn hoặc bằng SKD_HCNT_MA_MIN và mọi mã trong dải phải là vật phẩm có thật.

### Dải mã vật phẩm Sinh Tồn (PUBG), mã đầu  `SKD_PUBG_MA_MIN`
- Giá trị trong tệp: `4851`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã vật phẩm nhỏ nhất của dải thưởng hoạt động Sinh Tồn. Mặc định 4851 (dải 4851..4853). Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Sai dải là thưởng Sinh Tồn rơi vật phẩm khác. Đổi cả cặp MIN/MAX và khởi động lại máy chủ.

### Dải mã vật phẩm Sinh Tồn (PUBG), mã cuối  `SKD_PUBG_MA_MAX`
- Giá trị trong tệp: `4853`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã vật phẩm lớn nhất của dải thưởng hoạt động Sinh Tồn. Mặc định 4853. Cần khởi động lại máy chủ.
- **Cảnh báo:** Phải lớn hơn hoặc bằng SKD_PUBG_MA_MIN.

### Dải mã trang bị Hoàng Kim Môn Phái rơi, mã đầu  `SKD_HKMP_MA_MIN`
- Giá trị trong tệp: `0`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã nhỏ nhất của dải trang bị Hoàng Kim Môn Phái có thể rơi (bảng goldequip). Mặc định 0 (dải 0..139). Cần khởi động lại máy chủ.
- **Cảnh báo:** Sai dải là rơi trang bị HKMP không đúng bộ. Đổi cả cặp MIN/MAX.

### Dải mã trang bị Hoàng Kim Môn Phái rơi, mã cuối  `SKD_HKMP_MA_MAX`
- Giá trị trong tệp: `139`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã lớn nhất của dải trang bị Hoàng Kim Môn Phái có thể rơi. Mặc định 139. Cần khởi động lại máy chủ.
- **Cảnh báo:** Phải lớn hơn hoặc bằng SKD_HKMP_MA_MIN và không vượt quá số dòng thật của bảng goldequip.

### Hạn dùng trang bị HKMP rơi ra  `SKD_HKMP_HAN_NGAY`
- Giá trị trong tệp: `7` ngay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Trang bị Hoàng Kim Môn Phái rơi từ sự kiện có hạn dùng bằng số ngày này rồi tự biến mất. Mặc định 7. Cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 có thể thành vĩnh viễn tuỳ hàm tạo vật phẩm, làm trang bị HKMP miễn phí tràn server. Giữ từ 1 ngày trở lên.

### Ngưỡng rớt Huy Hoàng Đơn  `SKD_HMD_NGUONG`
- Giá trị trong tệp: `1`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi Huy Hoàng Đơn. Mặc định 1 với mẫu 100 = 99%.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đang gần như luôn rơi. Tăng ngưỡng để giảm tỉ lệ; đặt từ 100 trở lên là không bao giờ rơi.

### Số Huy Hoàng Đơn mỗi lần rơi  `SKD_HMD_SOLUONG`
- Giá trị trong tệp: `10` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Mỗi lần trúng, rơi ra đúng số Huy Hoàng Đơn này. Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Nhân thẳng vào lượng Huy Hoàng Đơn đổ vào server. Đặt 0 là không rơi.

### Mã vật phẩm Huy Hoàng Đơn  `SKD_HMD_ITEM`
- Giá trị trong tệp: `4844`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã vật phẩm của Huy Hoàng Đơn dùng trong nhánh rớt này. Mặc định 4844.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Chỉ đổi khi mã Huy Hoàng Đơn trong bảng vật phẩm thay đổi; mã sai làm rơi vật phẩm khác hoặc lỗi script.

### Ngưỡng rớt mảnh đồ phổ HKMP  `SKD_MANHHKMP_NGUONG`
- Giá trị trong tệp: `80`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDMAP) nhỏ hơn ngưỡng này thì rơi mảnh đồ phổ Hoàng Kim Môn Phái. Mặc định 80 với mẫu 100 = 79%. Chú ý chiều so sánh ngược với các ngưỡng khác.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng ngưỡng là tăng tỉ lệ (ngược với SKD_HMD_NGUONG). Đặt 101 là luôn rơi, đặt 0 hoặc 1 là không bao giờ.

### Ngưỡng rớt trang bị boss sự kiện  `SKD_BOSS_NGUONG`
- Giá trị trong tệp: `1`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDOTHER) phải lớn hơn ngưỡng này thì boss sự kiện rơi một trang bị trong dải SKD_BOSS_MA_MIN..MAX. Mặc định 1 với mẫu 10 = 90%.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt từ 10 trở lên (bằng mẫu) là boss sự kiện không rơi trang bị. Đặt 0 là luôn rơi.

### Dải mã trang bị boss sự kiện, mã đầu  `SKD_BOSS_MA_MIN`
- Giá trị trong tệp: `753`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã nhỏ nhất của dải trang bị boss sự kiện. Mặc định 753 (dải 753..770).
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Sai dải là boss rơi vật phẩm không đúng bộ hoặc lỗi script. Đổi cả cặp MIN/MAX.

### Dải mã trang bị boss sự kiện, mã cuối  `SKD_BOSS_MA_MAX`
- Giá trị trong tệp: `770`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã lớn nhất của dải trang bị boss sự kiện. Mặc định 770.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Phải lớn hơn hoặc bằng SKD_BOSS_MA_MIN.

### Ngưỡng rớt Tiền Đồng  `SKD_TIENDONG_NGUONG`
- Giá trị trong tệp: `1`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số bốc (1 đến SKD_RANDOTHER) phải lớn hơn ngưỡng này thì rơi Tiền Đồng. Mặc định 1 với mẫu 10 = 90%.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đặt từ 10 trở lên là không rơi Tiền Đồng. Đặt 0 là luôn rơi.

### Số Tiền Đồng mỗi lần rơi, ít nhất  `SKD_TIENDONG_SL_MIN`
- Giá trị trong tệp: `1` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Mỗi lần trúng rơi ngẫu nhiên từ SL_MIN đến SL_MAX Tiền Đồng. Mặc định 1. Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Phải nhỏ hơn hoặc bằng SKD_TIENDONG_SL_MAX, nếu không hàm random lỗi.

### Số Tiền Đồng mỗi lần rơi, nhiều nhất  `SKD_TIENDONG_SL_MAX`
- Giá trị trong tệp: `3` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Cận trên của số Tiền Đồng rơi mỗi lần. Mặc định 3. Cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt cao là bơm Tiền Đồng vào server. Phải lớn hơn hoặc bằng SKD_TIENDONG_SL_MIN.

### Mã vật phẩm Tiền Đồng  `SKD_TIENDONG_ITEM`
- Giá trị trong tệp: `4835`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mã vật phẩm của Tiền Đồng dùng trong nhánh rớt này. Mặc định 4835.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Mã sai làm rơi vật phẩm khác hoặc lỗi script. Chỉ đổi khi bảng vật phẩm đổi.

## Lịch hoạt động (`LICH`) - 5 khoá

Thời lượng các pha của Tống Kim (báo danh, trận, Nguyên Soái, dọn NPC) và số người mỗi phe. Khung giờ trận Tống Kim vẫn nằm trong bảng TK_LICH của script.

### Tống Kim: số phút báo danh  `TK_PHUT_BAODANH`
- Giá trị trong tệp: `1` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Độ dài pha báo danh trước mỗi trận Tống Kim (đồng hồ 1 của nhiệm vụ). Trong pha này người chơi và bot chỉ được đứng trong doanh trại, chưa được ra trận. Mặc định gốc 10, đang chạy 1 (chú thích trong mã vẫn ghi 10).
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là không có pha báo danh, người đến sớm ra trận trước người đến sau. Đặt quá dài (30) là người chơi chờ chán. Cần khởi động lại máy chủ.

### Tống Kim: số phút cả trận  `TK_PHUT_TRAN`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Độ dài một trận Tống Kim từ lúc bắt đầu tới lúc tính kết quả. Mặc định gốc 70, đang chạy 30. Phải khớp với khung giờ trong bảng TK_LICH của script (giờ kết thúc).
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Trận dài hơn khoảng cách giữa hai khung giờ trong TK_LICH là trận sau mở đè trận trước. Đặt quá ngắn (5) thì Nguyên Soái chưa kịp ra đã hết trận. Cần khởi động lại máy chủ.

### Tống Kim: phút Nguyên Soái xuất hiện  `TK_PHUT_NGUYENSOAI`
- Giá trị trong tệp: `1` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Sau khi trận bắt đầu bao nhiêu phút thì Nguyên Soái hai phe xuất hiện. Mặc định gốc 30, đang chạy 1.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Phải nhỏ hơn TK_PHUT_TRAN, nếu không Nguyên Soái không bao giờ ra và không ai nhận điểm giết Nguyên Soái. Cần khởi động lại máy chủ.

### Tống Kim: phút dọn NPC sau trận  `TK_PHUT_XOANPC`
- Giá trị trong tệp: `1` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Sau khi trận kết thúc bao nhiêu phút thì xoá toàn bộ NPC của trận (Nguyên Soái, cờ, trap). Mặc định 1.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 có thể xoá NPC ngay lúc đang tổng kết. Đặt dài mà trận kế mở sớm là NPC cũ còn đứng trong trận mới. Cần khởi động lại máy chủ.

### Tống Kim: số người tối đa mỗi phe  `TK_NGUOI_MOI_PHE`
- Giá trị trong tệp: `2000` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người (kể cả bot) mỗi phe trong một trận. Mặc định 2000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Số này cao cùng với nhiều bot làm một bản đồ quá đông, máy chủ phát gói tới hàng trăm người mỗi hành động, gây 'không thấy địch'. Muốn hạn chế bot thì dùng BOT_TK_TRAN. Cần khởi động lại máy chủ.

## Hoạt động (`HOATDONG`) - 47 khoá

Công tắc bật/tắt từng hoạt động và tham số ba hoạt động bản Linux (Sát Thủ, Phong Lăng Độ, Vượt ải), hệ Bắc Đẩu lệnh bài, exp hai vật phẩm dùng chung.

### Bật hoạt động: Tống Kim  `BAT_TONGKIM`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt chiến trường Tống Kim theo lịch TK_LICH. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Hạ Thuỷ Hoàng  `BAT_HATHUYHOANG`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt hoạt động Hạ Thuỷ Hoàng. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Boss Hoàng Kim  `BAT_BOSS_HOANGKIM`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt lịch xuất hiện Boss Hoàng Kim (tiểu và đại). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Hoa Đăng / Trạng Nguyên  `BAT_HOADANG`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt sự kiện Hoa Đăng và Trạng Nguyên (câu hỏi). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Trống bang hội  `BAT_TRONG_BANGHOI`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt Trống Khải Hoàn của bang hội (xem nhóm Trống bang hội). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Kiếm Môn Quan  `BAT_KIEMMONQUAN`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt hoạt động Kiếm Môn Quan. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Loa Vận Tiêu  `BAT_VANTIEU_LOA`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt loa thông báo của Vận Tiêu (NPC vận tiêu vẫn chạy dù tắt khoá này). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Đua Top  `BAT_DUATOP`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt sự kiện Đua Top. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Sòng Bạc  `BAT_SONGBAC`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt hoạt động Sòng Bạc. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật tự kích tài khoản  `BAT_CHECK_KICK`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- 1 = bật cơ chế tự kiểm tra và kích (đá) tài khoản vi phạm khỏi máy chủ theo nhịp timerserver. 0 = tắt (đang tắt).
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Bật mà luật kiểm tra chưa đúng là đá nhầm người chơi thật hàng loạt. Chỉ bật sau khi đã thử trên máy chủ thử nghiệm.

### Bật hoạt động: Sinh Tồn (PUBG)  `BAT_PUBG`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt hoạt động Sinh Tồn. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Cụm 3 hoạt động bản Linux  `BAT_HD3`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt cụm Sát Thủ / Phong Lăng Độ / Vượt ải bản Linux (nhóm HD3_*). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Viêm Đế Bảo Tàng  `BAT_VIEMDE`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt Viêm Đế Bảo Tàng (nhóm Viêm Đế; YDBZ_BAT cũng phải bật). timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Bật hoạt động: Công Thành + Lôi Đài bang hội (JX2)  `BAT_CTC_JX2`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật, 0 = tắt Công Thành Chiến và Lôi Đài bang hội bản JX2. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.
  Có hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy.
- **Cảnh báo:** Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.

### Sát thủ: cấp tối thiểu (chỉ hiển thị)  `HD3_ST_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cấp tối thiểu hiện trên menu admin của hoạt động Săn Boss Sát Thủ. Bản Linux không chặn cấp ở bước nhận nhiệm vụ (nhóm boss chia theo cấp 20..90 sẵn trong killbosshead). Mặc định 90.
- **Cảnh báo:** Đổi chỉ đổi chữ trên menu, không chặn ai.

### Sát thủ: số lần giết boss tối đa mỗi ngày  `HD3_ST_MAX_NGAY`
- Giá trị trong tệp: `8` lan · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số lần giết boss Sát Thủ mỗi người mỗi ngày (gốc Linux KILLER_MAXCOUNT = 8). Mặc định 8.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là Sát Thủ lệnh và exp Sát Thủ vào server nhiều hơn tương ứng. Cần khởi động lại máy chủ.

### Bắc Đẩu: bật hệ lệnh bài  `HD3_BD_BAT`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = bật hệ Bắc Đẩu (gom lệnh bài từ các hoạt động rồi đổi thưởng; sinh NPC Bắc Đẩu). 0 = tắt hẳn: không sinh NPC, không phát lệnh bài.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Tắt giữa chừng là lệnh bài người chơi đang giữ không đổi được nữa cho tới khi bật lại.

### Bắc Đẩu: giờ NPC bắt đầu làm việc (giờ chẵn)  `HD3_BD_GIO_MO`
- Giá trị trong tệp: `8` gio · kiểu số nguyên · khoảng 0..23 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Giờ trong ngày NPC Bắc Đẩu bắt đầu làm việc, chỉ ghi giờ 0..23 (giờ máy chủ). Bản Linux 8.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Chỉ nhận số giờ 0..23, không phải HHMM. Đặt sau giờ đóng là NPC không bao giờ làm việc.

### Bắc Đẩu: giờ NPC nghỉ (giờ chẵn)  `HD3_BD_GIO_DONG`
- Giá trị trong tệp: `22` gio · kiểu số nguyên · khoảng 0..23 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Giờ trong ngày NPC Bắc Đẩu nghỉ, chỉ ghi giờ 0..23. Bản Linux 22.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Chỉ nhận 0..23. Phải lớn hơn giờ mở.

### Bắc Đẩu: cấp tối thiểu nhận nhiệm vụ  `HD3_BD_CAP_TOITHIEU`
- Giá trị trong tệp: `150` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp tối thiểu (hoặc đã trùng sinh) để nhận nhiệm vụ Bắc Đẩu. Bản Linux 150.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp là nhiều người hơn nhận 8 triệu exp mỗi nhiệm vụ (HD3_BD_EXP_NHIEMVU).

### Bắc Đẩu: số lần nhận nhiệm vụ mỗi ngày  `HD3_BD_SO_LAN_NGAY`
- Giá trị trong tệp: `30` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số nhiệm vụ Bắc Đẩu mỗi người nhận được mỗi ngày. Bản Linux 30.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Nhân với HD3_BD_EXP_NHIEMVU là exp tối đa mỗi ngày từ Bắc Đẩu (30 x 8 triệu = 240 triệu). Tăng là tăng trần đó.

### Bắc Đẩu: exp thưởng mỗi nhiệm vụ đi đường  `HD3_BD_EXP_NHIEMVU`
- Giá trị trong tệp: `8000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp thưởng mỗi nhiệm vụ đi đường hoàn thành. Bản Linux 8.000.000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Nhân với 30 nhiệm vụ mỗi ngày; tăng gấp đôi là thêm 240 triệu exp mỗi người mỗi ngày.

### Bắc Đẩu: hạn dùng lệnh bài  `HD3_BD_HAN_LENHBAI`
- Giá trị trong tệp: `1440` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số phút lệnh bài tồn tại trước khi hết hạn. Bản Linux 1440 = 1 ngày.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt ngắn là người chơi chưa kịp đổi đã mất lệnh bài, khiếu nại.

### Bắc Đẩu: số lần dùng Huyết Linh Đơn mỗi ngày  `HD3_BD_LINHDAN_NGAY`
- Giá trị trong tệp: `15` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số lần dùng Bắc Đẩu Huyết Linh Đơn mỗi người mỗi ngày. Bản Linux 15.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Tăng là thưởng của đơn này không trần.

### Bắc Đẩu: phát kèm Lệnh bài Bắc Đẩu chung  `HD3_BD_QUYDOI_LENHBAI`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = mỗi hoạt động phát kèm 1 'Lệnh bài Bắc Đẩu' (mã dùng để đổi thưởng 15/20) bên cạnh lệnh bài riêng. 0 = y hệt bản Linux: chỉ phát lệnh bài riêng, mà 13 lệnh bài riêng đó không có chỗ tiêu, chỉ để sưu tập.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là người chơi không đổi được thưởng Bắc Đẩu nữa (không có lệnh bài chung).

### Bắc Đẩu: phát thêm lệnh bài riêng của từng hoạt động  `HD3_BD_LENHBAI_RIENG`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- 0 = không (mặc định): mỗi hoạt động chỉ cho 1 'Lệnh bài Bắc Đẩu' chung (xếp chồng 50/ô, mã duy nhất đổi thưởng). 1 = có, đúng bản Linux: phát thêm lệnh bài riêng (chỉ để sưu tập), người chơi nhận 2 vật phẩm mỗi lần.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Bật là túi người chơi thêm vật phẩm vô dụng mỗi lần hoàn thành hoạt động.

### Phong Lăng Độ: cấp tối thiểu (chỉ hiển thị)  `HD3_PLD_CAP_TOITHIEU`
- Giá trị trong tệp: `1` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cấp hiện trên menu admin. Bản Linux chỉ đòi có môn phái (fld_head.lua:40), không chặn cấp. Mặc định 1.
- **Cảnh báo:** Đổi chỉ đổi chữ trên menu.

### Phong Lăng Độ: sức chứa mỗi thuyền  `HD3_PLD_SUC_CHUA`
- Giá trị trong tệp: `100` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số người tối đa mỗi chuyến thuyền (fld_haveroom, đã nối HD_CFG đọc lúc chạy). Mặc định 100.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt nhỏ là người đến sau không lên thuyền được. Cần khởi động lại máy chủ.

### Vượt ải: cấp tối thiểu (chỉ hiển thị)  `HD3_VA_CAP_TOITHIEU`
- Giá trị trong tệp: `50` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cấp hiện trên menu admin. Giới hạn thật nằm ở bảng tbLevels trong include.lua (sơ cấp 50-89, cao cấp 90+); đổi ở đây không có tác dụng. Mặc định 50.
- **Cảnh báo:** Đổi chỉ đổi chữ trên menu.

### Vượt ải: phút báo danh  `HD3_VA_PHUT_BAODANH`
- Giá trị trong tệp: `10` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút báo danh mỗi lượt (đã nối vào challengeoftime include.lua). Mặc định 10.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Cộng với phút làm nhiệm vụ không được vượt 60 vì lượt mở mỗi giờ. Cần khởi động lại máy chủ.

### Vượt ải: phút làm nhiệm vụ  `HD3_VA_PHUT_NHIEMVU`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút tổ đội được ở trong ải. Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt ngắn là không kịp qua 28 ải. Báo danh + nhiệm vụ không vượt 60 phút. Cần khởi động lại máy chủ.

### Vượt ải: số người tối đa một đội  `HD3_VA_NGUOI_TOIDA`
- Giá trị trong tệp: `8` nguoi · kiểu số nguyên · khoảng 1..8 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số người tối đa một tổ đội vào ải (LIMIT_PLAYER_COUNT = 8). Mặc định 8.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt trên 8 vượt cỡ tổ đội của engine, không có tác dụng. Cần khởi động lại máy chủ.

### Vượt ải: giờ chốt bảng xếp hạng ngày  `HD3_VA_GIO_XEPHANG`
- Giá trị trong tệp: `0` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Giờ chốt bảng xếp hạng ngày, dạng HH:MM giờ máy chủ (00:00 = nửa đêm). Mặc định 00:00.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đổi giờ chốt giữa ngày là bảng xếp hạng chốt lệch. Cần khởi động lại máy chủ.

### Vượt ải: số lượt mỗi người mỗi ngày  `HD3_VA_LUOT_NGAY`
- Giá trị trong tệp: `1` lan · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số lượt vào Vượt ải mỗi người mỗi ngày (COUNT_LIMIT gốc Linux = 1). Mặc định 1.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là thưởng Vượt ải (Thiên Niên Linh Dược, rương) nhân lên. Cần khởi động lại máy chủ.

### Sát thủ: phần trăm exp mỗi lần giết boss  `HD3_ST_HESO_EXP`
- Giá trị trong tệp: `100` % · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Hệ số phần trăm exp thưởng mỗi lần giết một boss Sát Thủ: 100 = giữ nguyên bản Linux (15.000 exp ở nhóm cấp 20, tăng dần tới 200.000 ở nhóm cấp 90); 200 = gấp đôi; 50 = một nửa.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Là phần trăm, 100 là giữ nguyên: gõ 1 là exp còn 1%, gõ 1000 là gấp 10. Cần khởi động lại máy chủ.

### Sát thủ: exp ô quay thay Túi được phẩm (nhóm cấp 90)  `HD3_ST_EXP_QUAY`
- Giá trị trong tệp: `5000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số exp của ô quay thay cho Túi được phẩm trong bảng thưởng boss Sát Thủ cấp 90 (ô này chiếm khoảng 68% lượt quay). Mức 10 triệu cố định vẫn giữ. Mặc định 5.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Ô này trúng gần 7/10 lượt nên tăng là tăng exp trung bình mỗi lần giết boss rất mạnh. Cần khởi động lại máy chủ.

### Sát thủ: số Sát Thủ lệnh mỗi lần giết boss  `HD3_ST_SO_LENH`
- Giá trị trong tệp: `1` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Sát Thủ lệnh phát mỗi lần giết boss (bản Linux 1). Lệnh cùng cấp và cùng ngũ hành với boss; gộp 5 cái cùng cấp thành Sát Thủ Giản (vé vào Vượt ải).
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Tăng là vé Vượt ải rẻ đi tương ứng.

### Sát thủ: giá thuê xe tới chỗ boss  `HD3_ST_TIEN_XE`
- Giá trị trong tệp: `1000` luong · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Số tiền (lượng) trừ khi bấm dẫn đường tới boss trên F11 (khuôn cũ MONEY_GO_BOSS = 1000). Mặc định 1000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là dẫn đường miễn phí, không hại nhiều.

### Phong Lăng Độ: có thuyền phu ở bờ Bắc  `HD3_PLD_CO_THUYENPHU_BAC`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- 1 = giữ như dự án cũ: có NPC thuyền phu ở bờ Bắc đưa về bờ Nam. 0 = đúng bản Linux: chỉ 3 thuyền phu bờ Nam.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là người ở bờ Bắc phải tự về bằng cách khác. Cần khởi động lại máy chủ.

### Phong Lăng Độ: phí qua sông từ bờ Bắc  `HD3_PLD_PHI_VENAM`
- Giá trị trong tệp: `1000` luong · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Số tiền (lượng) thuyền phu bờ Bắc thu để đưa về bờ Nam. Dự án cũ 1000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là miễn phí, không hại nhiều.

### Phong Lăng Độ: số Thi Gia Chi ấn rơi từ boss Đầu Lĩnh  `HD3_PLD_SO_AN_BOSS`
- Giá trị trong tệp: `2` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số Thi Gia Chi ấn (6,1,1095) rơi từ boss Thuỷ Tặc Đầu Lĩnh 725 (gốc 2; bang có Đảo Chủ tiền đại thì gấp đôi theo getSignetDropRate).
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là bơm ấn vào server. Cần khởi động lại máy chủ.

### Phong Lăng Độ: tỉ lệ rơi Hải Long Châu từ boss  `HD3_PLD_TILE_HAILONG`
- Giá trị trong tệp: `0.005`  · kiểu số thực · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Tỉ lệ rơi Hải Long Châu (6,1,2124) từ boss, ghi dạng số thập phân 0 đến 1 (0.005 = 0,5%). Gốc 0.005.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Ghi 5 thay vì 0.005 là 500%, boss nào cũng rơi. Chỉ nhận 0..1. Cần khởi động lại máy chủ.

### Phong Lăng Độ: phần trăm rơi Truy Công Lệnh từ Thuỷ Tặc thường  `HD3_PLD_TILE_TRUYCONG`
- Giá trị trong tệp: `50` % · kiểu số nguyên · khoảng 0..100 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Tỉ lệ phần trăm rơi Truy Công Lệnh (6,1,2024) từ Thuỷ Tặc thường trong giờ sự kiện (gốc: số bốc nhỏ hơn 50 trên 100 = 49%). Gốc 50.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Là phần trăm 0..100. Đặt 100 là quái nào cũng rơi. Cần khởi động lại máy chủ.

### Phong Lăng Độ: phần trăm rơi Truy Công Lệnh từ boss Đầu Lĩnh  `HD3_PLD_TILE_TRUYCONG_BOSS`
- Giá trị trong tệp: `100` % · kiểu số nguyên · khoảng 0..100 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Tỉ lệ phần trăm rơi Truy Công Lệnh từ mỗi boss Đầu Lĩnh trong giờ hoạt động. 100 = luôn rơi (đúng bản Linux). Hạ xuống nếu thấy ra quá nhiều.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Là phần trăm 0..100.

### Phong Lăng Độ: số Bảo Rương Thuỷ Tặc khi cập bến  `HD3_PLD_THUONG_CAPBEN`
- Giá trị trong tệp: `2` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số Bảo Rương Thuỷ Tặc (6,1,3361) phát khi cập bến thành công. Gốc 2.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là tăng thưởng mỗi chuyến, nhân với 24 chuyến mỗi ngày. Cần khởi động lại máy chủ.

### Exp của Hoàng Chân Đơn (2273)  `HCD_EXP`
- Giá trị trong tệp: `2000000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng khi dùng Hoàng Chân Đơn, đi theo đường exp có trần cấp 200. Mặc định 2.000.000.000 (2 tỷ).
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đây là vật phẩm thưởng của nhiều hoạt động (Bang Chiến, Thành Bảo). Tăng là mọi nguồn phát đơn này cùng tăng exp. Số đã rất lớn, đừng thêm số 0.

### Exp của Quả Đại Hoàng Kim (4864)  `QDHK_EXP`
- Giá trị trong tệp: `200000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng khi dùng Quả Đại Hoàng Kim, đi theo đường exp thường. Mặc định 200.000.000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Vật phẩm thưởng của nhiều hoạt động; tăng là mọi nguồn phát cùng tăng.

## Tống Kim (`TONGKIM`) - 17 khoá

Cấp tối thiểu, phí, điểm tích luỹ, cờ, doanh trại và Nguyên Soái của chiến trường Tống Kim. Phần lớn chỉ đổi sau khi khởi động lại máy chủ vì được chốt lúc nạp script.

### Tống Kim: tiền đổi Nhạc Vương Kiếm  `TKT_GIA_DOI_NHACVUONGKIEM`
- Giá trị trong tệp: `10000000` luong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số tiền (lượng) phải trả khi đổi Nhạc Vương Kiếm tại NPC Tống Kim, cùng với TKT_SL_HONTHACH_DOI_KIEM Hồn Thạch. Mặc định 10.000.000 lượng.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Hạ giá là Nhạc Vương Kiếm rẻ đi cho toàn server. Cần khởi động lại máy chủ.

### Tống Kim: phí mỗi lần trinh sát  `TKT_PHI_TRINHSAT`
- Giá trị trong tệp: `500` luong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số tiền (lượng) trừ mỗi lần dùng chức năng trinh sát (xem quân số hai phe). Mặc định 500.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là trinh sát miễn phí, không hại gì nhiều. Cần khởi động lại máy chủ.

### Tống Kim: số Hồn Thạch đổi một Nhạc Vương Kiếm  `TKT_SL_HONTHACH_DOI_KIEM`
- Giá trị trong tệp: `100` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số Nhạc Vương Hồn Thạch cần nộp để đổi một Nhạc Vương Kiếm. Mặc định 100.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Hạ thấp là kiếm tràn server. Cần khởi động lại máy chủ.

### Tống Kim: cấp tối thiểu báo danh  `TKT_CAP_TOI_THIEU`
- Giá trị trong tệp: `80` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Nhân vật phải đạt cấp này mới được báo danh Tống Kim. Mặc định 80.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Hạ thấp là người cấp thấp vào trận bị giết liên tục và có thể bị lợi dụng để farm điểm. Cần khởi động lại máy chủ.

### Tống Kim: chênh lệch quân số tối đa hai phe  `TKT_LECH_QUANSO_TOIDA`
- Giá trị trong tệp: `20` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Khi phe đông hơn phe kia quá số người này thì phe đông bị chặn báo danh cho tới khi cân lại. Mặc định 20.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là hai phe phải đúng bằng nhau, nhiều người không báo danh được. Đặt rất lớn là một phe có thể đông áp đảo. Cần khởi động lại máy chủ.

### Tống Kim: phí báo danh (chỉ hiển thị)  `TKT_PHI_BAO_DANH`
- Giá trị trong tệp: `20000` luong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số tiền ghi trong lời thoại báo danh. Hiện chỉ hiện chữ, không trừ tiền vì mọi lệnh Pay trong script đã bị vô hiệu. Mặc định 20.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đổi chỉ đổi con số hiển thị, không đổi tiền thật. Muốn thu phí thật phải sửa script. Cần khởi động lại máy chủ.

### Tống Kim: điểm tích luỹ thưởng phe thắng  `TKT_THUONG_TICHLUY_THANG`
- Giá trị trong tệp: `1200` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm tích luỹ cộng thêm cho mỗi người phe thắng khi hết trận, nhân với cấp quân hàm (1 đến 6) của người đó. Mặc định 1200.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đây là nguồn điểm tích luỹ (đổi thưởng) chính của Tống Kim; tăng gấp đôi là thưởng cả server gấp đôi mỗi trận. Giữ thắng > hoà > thua. Cần khởi động lại máy chủ.

### Tống Kim: điểm tích luỹ thưởng phe thua  `TKT_THUONG_TICHLUY_THUA`
- Giá trị trong tệp: `300` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm tích luỹ cộng thêm cho mỗi người phe thua khi hết trận, nhân với cấp quân hàm (1 đến 6) của người đó. Mặc định 300.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đây là nguồn điểm tích luỹ (đổi thưởng) chính của Tống Kim; tăng gấp đôi là thưởng cả server gấp đôi mỗi trận. Giữ thắng > hoà > thua. Cần khởi động lại máy chủ.

### Tống Kim: điểm tích luỹ thưởng khi hoà  `TKT_THUONG_TICHLUY_HOA`
- Giá trị trong tệp: `600` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm tích luỹ cộng thêm cho mỗi người khi hoà khi hết trận, nhân với cấp quân hàm (1 đến 6) của người đó. Mặc định 600.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đây là nguồn điểm tích luỹ (đổi thưởng) chính của Tống Kim; tăng gấp đôi là thưởng cả server gấp đôi mỗi trận. Giữ thắng > hoà > thua. Cần khởi động lại máy chủ.

### Tống Kim: điểm tối thiểu để nhận thưởng  `TKT_DIEM_TOITHIEU_NHAN_THUONG`
- Giá trị trong tệp: `1000` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Người có điểm trong trận dưới ngưỡng này thì hết trận không được nhận thưởng (chống vào trận đứng không). Mặc định 1000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là ai vào cũng có thưởng, bot và người treo máy nhận thưởng miễn phí. Đặt quá cao là người chơi bình thường không đạt. Cần khởi động lại máy chủ.

### Tống Kim: số cờ tối đa mỗi phe cắm được  `TKT_SO_CO_TOIDA`
- Giá trị trong tệp: `30` la · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Mỗi phe chỉ được cắm tối đa bấy nhiêu lá cờ trong một trận. Mặc định 30.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là không cắm được cờ. Cần khởi động lại máy chủ.

### Tống Kim: điểm mỗi lần cắm cờ thành công  `TKT_DIEM_CAM_CO`
- Giá trị trong tệp: `300` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm cộng cho cá nhân và cho phe mỗi lần mang cờ về cắm thành công. Mặc định 300.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Tăng cao là cắm cờ thành cách farm điểm chính, lấn át giết người. Cần khởi động lại máy chủ.

### Tống Kim: mức giảm tốc độ người vác cờ  `TKT_MUC_GIAM_TOCDO_GIU_CO`
- Giá trị trong tệp: `60`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Người đang vác cờ bị giảm tốc độ chạy theo mức này (đơn vị tốc độ của engine). Mặc định 60.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là vác cờ chạy như thường, cờ về quá dễ. Đặt quá cao là người vác cờ đứng yên. Cần khởi động lại máy chủ.

### Tống Kim: số giây tối đa trong doanh trại  `TKT_GIAY_TRONG_DOANHTRAI`
- Giá trị trong tệp: `90` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Người chơi đứng trong doanh trại (hậu doanh) quá số giây này sẽ bị đẩy ra ngoài. Mặc định 90.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt quá ngắn là người vừa hồi sinh chưa kịp mua máu đã bị đẩy ra. Đặt 0 có thể làm cơ chế đẩy hoạt động lạ. Cần khởi động lại máy chủ.

### Tống Kim: số giây bảo hộ khi ra khỏi trại  `TKT_GIAY_DELAY_RA_TRAI`
- Giá trị trong tệp: `10` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số giây còn được giữ trạng thái bảo hộ khi bước ra khỏi doanh trại. Mặc định 10.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt quá dài là người ra trại bất tử lâu, lợi dụng đánh trước cửa trại. Cần khởi động lại máy chủ.

### Tống Kim: máu Nguyên Soái  `TKT_MAU_NGUYENSOAI`
- Giá trị trong tệp: `5000000` mau · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Máu của Nguyên Soái hai phe khi xuất hiện, còn được nhân với GLB_MANH_NGUYENSOAI_TK. Mặc định 5.000.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Máu quá thấp là Nguyên Soái chết ngay khi ra, trận kết thúc sớm; quá cao là không ai hạ nổi. Đổi phải tính cả hệ số GLB_MANH_NGUYENSOAI_TK. Cần khởi động lại máy chủ.

### Tống Kim: điểm cho người hạ Nguyên Soái  `TKT_DIEM_GIET_NGUYENSOAI`
- Giá trị trong tệp: `5000` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm cộng cho người đánh đòn cuối hạ Nguyên Soái (người đó ăn trọn). Mặc định 5000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Số này lớn hơn cả trận giết người nên rất dễ gây tranh chấp đòn cuối. Cần khởi động lại máy chủ.

## Công thành chiến (`CONGTHANH`) - 22 khoá

Độ dài trận, số cổng, Long Trụ, số người mỗi phe, điểm công trạng và exp thưởng của Công Thành Chiến bản JX2.

### Công thành: điểm công trạng phá một Long Trụ  `CTC_DIEM_PHA_LONGTRU`
- Giá trị trong tệp: `300` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm công trạng cộng cho người phá được một Long Trụ. Mặc định 300.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Tăng cao là phá trụ lấn át mọi cách ghi điểm khác. Cần khởi động lại máy chủ.

### Công thành: exp thưởng top 10 (bản trong lib_ctc)  `CTC_EXP_TOP10_BANSAO`
- Giá trị trong tệp: `3000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Exp thưởng cho 10 người dẫn đầu công trạng, bản sao đọc tại lib_ctc.lua. Có hai khoá cùng nghĩa (CTC_EXP_TOP10 ở missions\citywar_global\head.lua) - hãy đặt cả hai bằng nhau. Mặc định 3.000.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt lệch với CTC_EXP_TOP10 là hai chỗ phát exp khác nhau. Cần khởi động lại máy chủ.

### Công thành: giá mua lệnh bài đăng ký  `CTC_GIA_LENHBAI`
- Giá trị trong tệp: `200000` luong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số tiền (lượng) bang phải trả để mua lệnh bài đăng ký Công Thành. Mặc định 200.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt 0 là bang nào cũng đăng ký được không tốn gì. Cần khởi động lại máy chủ.

### Công thành: tiền hoàn khi trả lại lệnh bài  `CTC_GIA_TRA_LAI_LENHBAI`
- Giá trị trong tệp: `10000` luong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Số tiền (lượng) hoàn lại khi bang trả lệnh bài. Mặc định 10.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt lớn hơn CTC_GIA_LENHBAI là mua rồi trả để lấy lời, nhân tiền vô hạn. Cần khởi động lại máy chủ.

### Công thành: số người liên minh phe công  `CTC_MAX_LIENMINH_CONG_VEBINH`
- Giá trị trong tệp: `5` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số người ngoài bang được trợ giúp phe công (số NPC vệ binh dùng), bản do NPC vệ binh đọc (lib_ctc.lua). Bản 'cửa trận' là CTC_MAX_LIENMINH_CONG/THU. Mặc định 5.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Hai bản (vệ binh và cửa trận) đang khác nhau (5 và 50); đổi nên đổi cả hai cho khớp. Cần khởi động lại máy chủ.

### Công thành: số người liên minh phe thủ  `CTC_MAX_LIENMINH_THU_VEBINH`
- Giá trị trong tệp: `5` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số người ngoài bang được trợ giúp phe thủ (số NPC vệ binh dùng), bản do NPC vệ binh đọc (lib_ctc.lua). Bản 'cửa trận' là CTC_MAX_LIENMINH_CONG/THU. Mặc định 5.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Hai bản (vệ binh và cửa trận) đang khác nhau (5 và 50); đổi nên đổi cả hai cho khớp. Cần khởi động lại máy chủ.

### Công thành: số người tối đa phe công (NPC vệ binh dùng)  `CTC_MAX_NGUOI_PHE_CONG_VEBINH`
- Giá trị trong tệp: `50` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người phe công mà NPC vệ binh kiểm khi cho vào. Bản cửa trận là CTC_MAX_NGUOI_PHE_CONG/THU (200). Mặc định 50.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đang lệch với bản cửa trận (50 và 200): người thứ 51 qua cửa được nhưng vệ binh chặn. Nên đặt hai bản bằng nhau. Cần khởi động lại máy chủ.

### Công thành: số người tối đa phe thủ (NPC vệ binh dùng)  `CTC_MAX_NGUOI_PHE_THU_VEBINH`
- Giá trị trong tệp: `50` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người phe thủ mà NPC vệ binh kiểm khi cho vào. Bản cửa trận là CTC_MAX_NGUOI_PHE_CONG/THU (200). Mặc định 50.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đang lệch với bản cửa trận (50 và 200): người thứ 51 qua cửa được nhưng vệ binh chặn. Nên đặt hai bản bằng nhau. Cần khởi động lại máy chủ.

### Công thành: chu kỳ loa tình hình  `CTC_CHUKY_LOA_GIAY`
- Giá trị trong tệp: `60` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Trong trận, cứ bao nhiêu giây loa thông báo tình hình (trụ, cổng, điểm) một lần. Mặc định 60.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt nhỏ (5) là loa dồn dập che khung chat. Cần khởi động lại máy chủ.

### Công thành: độ dài một trận  `CTC_DODAI_TRAN_PHUT`
- Giá trị trong tệp: `90` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút một trận Công Thành Chiến kéo dài trước khi tính kết quả. Mặc định 90.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt ngắn là phe công không kịp phá đủ cổng và trụ, phe thủ luôn thắng. Cần khởi động lại máy chủ.

### Công thành: exp thưởng top 10 công trạng  `CTC_EXP_TOP10`
- Giá trị trong tệp: `3000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Exp thưởng cho 10 người dẫn đầu bảng công trạng sau trận (đọc tại missions\citywar_global\head.lua). Có một bản sao CTC_EXP_TOP10_BANSAO ở lib_ctc.lua, nên đặt bằng nhau. Mặc định 3.000.000.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đây là exp phát trực tiếp, tăng gấp 10 là bơm 30 triệu exp cho 10 người mỗi trận. Cần khởi động lại máy chủ.

### Công thành: số Long Trụ trong trận  `CTC_SO_LONGTRU`
- Giá trị trong tệp: `3` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Số Long Trụ sinh ra trong trận; phá đủ toàn bộ là phe công thắng ngay. Mặc định 3.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Số trụ phải có đủ toạ độ đặt trong bảng của script; đặt nhiều hơn số toạ độ là trụ không sinh được. Cần khởi động lại máy chủ.

### Công thành: số cổng thành phải phá  `CTC_SO_CONG_THANH`
- Giá trị trong tệp: `3` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Số cổng thành phe công phải phá. phải khớp số dòng trong bảng DoorPos của script. Mặc định 3.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt khác số dòng bảng DoorPos là script lỗi khi sinh cổng hoặc trận không thể kết thúc. Đừng đổi nếu chưa sửa bảng DoorPos. Cần khởi động lại máy chủ.

### Công thành: số người tối đa phe thủ (cửa trận)  `CTC_MAX_NGUOI_PHE_THU`
- Giá trị trong tệp: `200` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người phe thủ được vào trận, bản do cửa trận kiểm. Mặc định 200. Bản NPC vệ binh (CTC_MAX_NGUOI_PHE_*_VEBINH) đang là 50.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Quá đông trong một bản đồ là tải gói tin cao, dễ 'không thấy địch'. Nên khớp với bản vệ binh. Cần khởi động lại máy chủ.

### Công thành: số người tối đa phe công (cửa trận)  `CTC_MAX_NGUOI_PHE_CONG`
- Giá trị trong tệp: `200` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người phe công được vào trận, bản do cửa trận kiểm. Mặc định 200. Bản NPC vệ binh (CTC_MAX_NGUOI_PHE_*_VEBINH) đang là 50.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Quá đông trong một bản đồ là tải gói tin cao, dễ 'không thấy địch'. Nên khớp với bản vệ binh. Cần khởi động lại máy chủ.

### Công thành: số người liên minh phe thủ (cửa trận)  `CTC_MAX_LIENMINH_THU`
- Giá trị trong tệp: `50` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số người ngoài bang được vào trợ giúp phe thủ, bản do cửa trận kiểm. Mặc định 50. Bản NPC vệ binh đang là 5.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Nên khớp với bản vệ binh, nếu không người liên minh qua cửa rồi bị vệ binh chặn. Cần khởi động lại máy chủ.

### Công thành: số người liên minh phe công (cửa trận)  `CTC_MAX_LIENMINH_CONG`
- Giá trị trong tệp: `50` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số người ngoài bang được vào trợ giúp phe công, bản do cửa trận kiểm. Mặc định 50. Bản NPC vệ binh đang là 5.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Nên khớp với bản vệ binh, nếu không người liên minh qua cửa rồi bị vệ binh chặn. Cần khởi động lại máy chủ.

### Công thành: cấp NPC Long Trụ  `CTC_CAP_LONGTRU`
- Giá trị trong tệp: `10` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Cấp của NPC Long Trụ khi sinh ra, quyết định máu của trụ. Mặc định 10.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Tăng cấp là trụ trâu hơn nhiều, phe công khó thắng. Cần khởi động lại máy chủ.

### Công thành: cấp NPC cổng thành  `CTC_CAP_CONG_THANH`
- Giá trị trong tệp: `60` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Cấp của NPC cổng thành, quyết định cổng khó phá tới đâu. Mặc định 60.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Cấp quá cao là không phá nổi cổng trong 90 phút. Cần khởi động lại máy chủ.

### Công thành: điểm công trạng mỗi lần hạ gục đối phương  `CTC_DIEM_GIET_NGUOI`
- Giá trị trong tệp: `75` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Điểm công trạng cơ bản mỗi lần hạ gục người phe kia. Mặc định 75. Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng cao là giết người lấn át phá trụ/cổng.

### Công thành: điểm thưởng mỗi mốc liên trảm  `CTC_DIEM_LIEN_TRAM`
- Giá trị trong tệp: `150` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Điểm thưởng thêm mỗi lần đạt mốc liên trảm (giết liên tiếp không chết). Mặc định 150. Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng cao khuyến khích cao thủ farm người yếu.

### Công thành: chu kỳ quét mở trận  `CTC_CHUKY_QUET_MO_TRAN_PHUT`
- Giá trị trong tệp: `5` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Cứ bao nhiêu phút script Công Thành quét lịch một lần để xem có tới giờ mở trận không. Mặc định 5.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt lớn là trận mở trễ tới bấy nhiêu phút so với lịch. Cần khởi động lại máy chủ.

## Lôi đài (`LOIDAI`) - 5 khoá

Lôi đài bang hội (đấu trường JX2): số người mỗi bang, thời gian chờ, độ dài trận, exp bang thắng.

### Lôi đài: số người tối đa mỗi bang  `CTLD_MAX_NGUOI_MOI_PHE`
- Giá trị trong tệp: `16` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Mỗi bang được đưa tối đa bấy nhiêu người vào đấu trường Lôi Đài. Mặc định 16.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt lớn là đấu trường quá đông cho bản đồ nhỏ. Cần khởi động lại máy chủ.

### Lôi đài: chu kỳ loa  `CTLD_CHUKY_LOA_GIAY`
- Giá trị trong tệp: `20` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Cứ bao nhiêu giây loa tình hình trận Lôi Đài một lần. Mặc định 20.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt nhỏ là loa dồn dập. Cần khởi động lại máy chủ.

### Lôi đài: độ dài một trận  `CTLD_DODAI_TRAN_PHUT`
- Giá trị trong tệp: `25` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút một trận Lôi Đài kéo dài (chú thích trong mã ghi nhầm là một tiếng). Mặc định 25.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt ngắn là trận kết thúc hoà nhiều. Cần khởi động lại máy chủ.

### Lôi đài: thời gian chờ hai bên vào đấu trường  `CTLD_GIAY_CHO_VAO_DAUTRUONG`
- Giá trị trong tệp: `480` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số giây chờ hai bang vào đấu trường trước khi bắt đầu (chú thích trong mã ghi nhầm là 10 phút). Mặc định 480 = 8 phút.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Đặt quá ngắn là bang vào chậm bị xử thua. Cần khởi động lại máy chủ.

### Lôi đài: exp bang hội cho bên thắng  `CTLD_EXP_BANG_THANG`
- Giá trị trong tệp: `1200` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Kinh nghiệm bang hội (không phải exp nhân vật) cộng cho bang thắng một trận Lôi Đài. Mặc định 1200.
  Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực.
- **Cảnh báo:** Tăng cao là bang mạnh lên cấp bang rất nhanh. Cần khởi động lại máy chủ.

## Bang chiến (`BANGCHIEN`) - 11 khoá

Võ Lâm Đệ Nhất Bang (tongwar): cấp tối thiểu, giờ khai chiến / kết thúc, độ dài trận, số người, số mạng, điểm giết và liên trảm.

### Bang chiến: cấp tối thiểu  `TW_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp tối thiểu để báo danh và vào đấu trường Bang Chiến (Võ Lâm Đệ Nhất Bang). Mặc định 90.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp là người cấp thấp vào trận làm mồi. Không đặt cao hơn cấp phổ biến của server.

### Bang chiến: giờ khai chiến  `TW_GIO_KHAICHIEN`
- Giá trị trong tệp: `2000` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Giờ khai chiến trong ngày thi đấu, dạng HH:MM giờ máy chủ. Driver tick 15 phút một lần với cửa sổ khớp cộng trừ 5 phút quanh mốc. Mặc định 20:00.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đổi giờ mà quên đổi TW_GIO_KETTHUC và TW_PHUT_TRAN là trận kết thúc sai lúc. Lời thoại NPC (head.lua:492) vẫn ghi giờ cũ, chỉ là chữ.

### Bang chiến: giờ đóng trận  `TW_GIO_KETTHUC`
- Giá trị trong tệp: `2130` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Giờ đóng trận trong ngày thi đấu, dạng HH:MM giờ máy chủ. Mặc định 21:30. Nên bằng giờ khai chiến cộng TW_PHUT_TRAN.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt trước giờ khai chiến là trận vừa mở đã đóng.

### Bang chiến: độ dài trận  `TW_PHUT_TRAN`
- Giá trị trong tệp: `90` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút một trận Bang Chiến. Nên bằng (TW_GIO_KETTHUC trừ TW_GIO_KHAICHIEN). Mặc định 90.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Lệch với hai mốc giờ là trận bị cắt ngang hoặc kéo quá giờ đóng. Cần khởi động lại máy chủ.

### Bang chiến: phút chuẩn bị trước khai chiến  `TW_PHUT_CHUANBI`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút mở cửa cho vào chuẩn bị trước giờ khai chiến. Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là không có thời gian vào sân. Cần khởi động lại máy chủ.

### Bang chiến: số người tối đa mỗi phe  `TW_NGUOI_TOIDA`
- Giá trị trong tệp: `150` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Trần số người mỗi phe được vào trận. Mặc định 150.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Quá đông trong bản đồ 605-607 là tải gói tin cao. Cần khởi động lại máy chủ.

### Bang chiến: số người tối thiểu mỗi phe  `TW_NGUOI_TOITHIEU`
- Giá trị trong tệp: `5` nguoi · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Phe có ít hơn số người này khi vào trận bị xử thua. Mặc định 5.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt cao là bang nhỏ luôn bị xử thua. Cần khởi động lại máy chủ.

### Bang chiến: số giây tối đa ở hậu doanh  `TW_GIAY_HAUDOANH`
- Giá trị trong tệp: `120` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Đứng trong hậu doanh quá số giây này thì bị đẩy ra ngoài. Mặc định 120.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Quá ngắn là chưa kịp hồi máu đã bị đẩy. Cần khởi động lại máy chủ.

### Bang chiến: số mạng tối đa một trận  `TW_SO_MANG`
- Giá trị trong tệp: `10` mang · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Chết đủ số mạng này là bị loại khỏi trận. Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 1 là chết một lần bị loại, trận kết thúc rất nhanh. Đặt rất lớn là không ai bị loại.

### Bang chiến: điểm mỗi lần giết địch  `TW_DIEM_KILL`
- Giá trị trong tệp: `75` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm cộng khi giết một địch, nhân với hệ số quân hàm RANK_PKBONUS (missions\tongwar\head.lua:44). Mặc định 75.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng cao là giết người lấn át chiến thuật. Cần khởi động lại máy chủ.

### Bang chiến: điểm thưởng liên trảm  `TW_DIEM_LIENTRAM`
- Giá trị trong tệp: `150` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Điểm thưởng mỗi 3 mạng giết liên tiếp không chết. Mặc định 150.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng cao khuyến khích cao thủ farm. Cần khởi động lại máy chủ.

## Bách nhân lôi đài (`BACHNHAN`) - 19 khoá

Bách Nhân Lôi Đài (map 960): giờ mở, exp mỗi nhịp, Lôi Chủ, NPC Cổ Thủ buff x2, nhịp đấu và NPC lối vào.

### Bách nhân: cấp tối thiểu  `BR_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp tối thiểu để vào Bách Nhân Lôi Đài (bản đồ 960, vào từ NPC ở Lâm An). Mặc định 90.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp là người yếu vào bị NPC cao thủ hạ ngay.

### Bách nhân: giờ mở trong ngày  `BR_GIO_MO`
- Giá trị trong tệp: `1200` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Giờ mở cửa trong ngày, dạng HH:MM giờ máy chủ. Mặc định 12:00.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Giờ mở sau giờ đóng là không bao giờ mở.

### Bách nhân: giờ đóng (giờ chẵn 0 đến 23)  `BR_GIO_DONG_H`
- Giá trị trong tệp: `0` gio · kiểu số nguyên · khoảng 0..23 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Giờ đóng cửa trong ngày, chỉ ghi giờ (0 đến 23), 0 = 0 giờ đêm, tới giờ này mọi người được trả về. Mặc định 0.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Chỉ nhận số giờ 0..23, không phải HHMM: gõ 2300 là sai. Đặt trùng giờ mở là đóng ngay khi mở.

### Bách nhân: chu kỳ phát exp  `BR_PHUT_CHUKY_EXP`
- Giá trị trong tệp: `5` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cứ bao nhiêu phút phát một nhịp exp cho người đang trong bản đồ. Mặc định 5.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 1 là exp phát gấp 5 lần. Đặt 0 có thể làm nhịp chạy mỗi tick.

### Bách nhân: exp mỗi nhịp cho người trong bản đồ  `BR_EXP_TICK`
- Giá trị trong tệp: `1000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng mỗi nhịp cho mỗi người đang trong bản đồ (có buff Cổ Thủ thì gấp đôi). Mặc định 1.000.000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đây là exp treo máy: người đứng trong bản đồ cũng nhận. Nhân với 50 nhịp mỗi ngày (BR_TRAN_LUOT_NGAY) là 50 triệu exp mỗi người mỗi ngày.

### Bách nhân: exp cộng thêm cho Lôi Chủ  `BR_EXP_LOICHU`
- Giá trị trong tệp: `2000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng thêm mỗi nhịp cho người đang giữ đài (Lôi Chủ). Mặc định 2.000.000.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Giữ đài lâu là exp rất lớn, nhân với số nhịp mỗi ngày.

### Bách nhân: trần số nhịp exp mỗi người mỗi ngày  `BR_TRAN_LUOT_NGAY`
- Giá trị trong tệp: `50` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mỗi người chỉ được nhận tối đa bấy nhiêu nhịp exp mỗi ngày (task daily 2709). Mặc định 50.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 có thể thành không giới hạn tuỳ script; đặt cao là exp treo máy không trần.

### Bách nhân: chu kỳ NPC Cổ Thủ xuất hiện  `BR_PHUT_COTHU`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cứ bao nhiêu phút NPC Cổ Thủ (phát buff exp x2) xuất hiện một lần. Mặc định 30.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt nhỏ là buff x2 gần như liên tục.

### Bách nhân: tỉ lệ người được buff x2 mỗi đợt  `BR_TILE_BUFFX2`
- Giá trị trong tệp: `0.2`  · kiểu số thực · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Tỉ lệ số người trong bản đồ được Cổ Thủ buff mỗi đợt, ghi dạng số thập phân 0 đến 1 (0.2 = 20%). Mặc định 0.2.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Ghi 20 thay vì 0.2 là 2000%, ai cũng được buff. Chỉ nhận 0..1.

### Bách nhân: số giây chờ người khiêu chiến  `BR_GIAY_CHO_KHIEUCHIEN`
- Giá trị trong tệp: `30` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hết số giây này không ai khiêu chiến thì gọi NPC cao thủ lên đài. Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là NPC lên đài ngay, người chơi không kịp khiêu chiến. Cần khởi động lại máy chủ.

### Bách nhân: số giây đếm ngược trước khi đánh  `BR_GIAY_DEM_NGUOC`
- Giá trị trong tệp: `3` giay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Đếm ngược bấy nhiêu giây trước khi lượt đấu bắt đầu. Mặc định 3.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt quá dài là nhịp đấu chậm. Cần khởi động lại máy chủ.

### Bách nhân: số phút mỗi lượt đấu  `BR_PHUT_MOI_LUOT`
- Giá trị trong tệp: `3` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hết số phút này chưa ai chết thì so sát thương: Lôi Chủ thắng nếu chịu ít sát thương hơn. Mặc định 3.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt quá ngắn là lượt đấu luôn phân định bằng so sát thương. Cần khởi động lại máy chủ.

### Bách nhân: số phút đứng yên tối đa  `BR_PHUT_DUNG_YEN`
- Giá trị trong tệp: `90` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Người đứng yên trong bản đồ quá số phút này bị đá ra. Mặc định 90.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt quá ngắn là người đang xem bị đá; quá dài là treo máy ăn exp thoải mái. Cần khởi động lại máy chủ.

### Bách nhân: số phút hiệu lực buff x2  `BR_PHUT_BUFF_X2`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Buff exp x2 của Cổ Thủ kéo dài bấy nhiêu phút. Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt bằng hoặc dài hơn chu kỳ Cổ Thủ là buff liên tục. Cần khởi động lại máy chủ.

### Bách nhân: chuỗi thắng tối đa của một người  `BR_TRAN_CHUOI`
- Giá trị trong tệp: `100`  · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Chuỗi thắng đạt tới số này thì loa 'truyền thuyết' và đặt lại chuỗi. Mỗi 10 trận thắng ở đài 1 loa toàn server. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt nhỏ là loa liên tục.

### Bách nhân: cấp gốc NPC cao thủ  `BR_CAP_NPC_GOC`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp gốc của NPC cao thủ; cấp thật = gốc + số lượt thắng chia 10, trần gốc + 9. Mặc định 90.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt cao là NPC quá mạnh, không ai giữ đài nổi.

### Bách nhân: bản đồ đặt NPC lối vào  `BR_NPC_MAP`
- Giá trị trong tệp: `176` map · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã bản đồ đặt NPC lối vào Bách Nhân Lôi Đài (đặt lúc máy chủ khởi động). Mặc định 176 (Lâm An).
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Sai mã bản đồ là NPC lối vào không xuất hiện, không ai vào được. Cần khởi động lại máy chủ.

### Bách nhân: mẫu NPC lối vào  `BR_NPC_ID`
- Giá trị trong tệp: `1747`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã mẫu (template) trong npcs.txt của NPC lối vào. Mặc định 1747.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Sai mẫu là sinh NPC khác hoặc không sinh được. Cần khởi động lại máy chủ.

### Bách nhân: cấp NPC lối vào  `BR_NPC_LEVEL`
- Giá trị trong tệp: `95` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Cấp của NPC lối vào khi sinh. Mặc định 95.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Chỉ ảnh hưởng hiển thị / máu NPC lối vào. Cần khởi động lại máy chủ.

## Tỷ võ (`TYVO`) - 4 khoá

Tỷ Võ Đài (đăng ký qua Công Bình Tử): cấp tối thiểu, số người mỗi đội, phút chờ và tổng phút một trận.

### Tỷ võ: cấp tối thiểu  `BW_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cấp tối thiểu để đăng ký Tỷ Võ Đài qua NPC Công Bình Tử (khán giả không bị chặn). Mặc định 90.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp không hại nhiều vì là đấu tự nguyện.

### Tỷ võ: số người tối đa mỗi đội  `BW_DOI_TOIDA`
- Giá trị trong tệp: `8` nguoi · kiểu số nguyên · khoảng 1..8 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Số người tối đa mỗi đội. Menu NPC hiện '1 vs 1' tới '8 vs 8'; đặt quá 8 thì menu vẫn chỉ hiện 8 lựa chọn (sửa thêm chữ ở bwmanager.lua:55). Mặc định 8.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt trên 8 không có tác dụng trên menu. Đặt 0 là không lập được đội.

### Tỷ võ: phút chờ trước khi đánh  `BW_PHUT_CHO`
- Giá trị trong tệp: `2` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số phút chờ hai đội vào đài trước khi đánh. Mặc định 2.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Phải nhỏ hơn BW_PHUT_TRAN. Cần khởi động lại máy chủ.

### Tỷ võ: tổng phút một trận kể cả chờ  `BW_PHUT_TRAN`
- Giá trị trong tệp: `12` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Tổng số phút một trận kể cả thời gian chờ (mặc định 12 = 2 chờ + 10 đánh).
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt nhỏ hơn hoặc bằng BW_PHUT_CHO là không có thời gian đánh. Cần khởi động lại máy chủ.

## Thành bảo / Tín sứ (`THANHBAO`) - 20 khoá

Bang hội Thành Bảo (thủ cây Thần Mộc, map 984) và nhiệm vụ Tín Sứ Thiên Bảo Khố: cấp, lượt, giờ nhận điểm, giá đổi lệnh bài, bùa Thủ Vệ.

### Tín sứ: cấp tối thiểu nhận nhiệm vụ  `TS_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Cấp tối thiểu để nhận nhiệm vụ Tín Sứ Thiên Bảo Khố tại Dịch Quan Ba Lăng Huyện (bản đồ 11) hoặc Đại Lý (162). Gốc Linux 120, chủ game đã hạ 90.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Hạ thấp là người yếu vào ải chết liên tục. Cần khởi động lại máy chủ.

### Tín sứ: số lượt thường mỗi ngày  `TS_LUOT_THUONG_NGAY`
- Giá trị trong tệp: `2` lan · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số lượt nhận nhiệm vụ Tín Sứ miễn phí mỗi ngày (đếm ở task 4128). Mặc định 2.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là tăng exp và điểm Tín Sứ toàn server mỗi ngày. Cần khởi động lại máy chủ.

### Tín sứ: số lượt mua thêm bằng Thiên Khố Bảo Lệnh  `TS_LUOT_ITEM_NGAY`
- Giá trị trong tệp: `1` lan · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số lượt nhận thêm mỗi ngày bằng vật phẩm Thiên Khố Bảo Lệnh (6,1,3431). Mặc định 1.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là người có nhiều lệnh làm được nhiều lượt hơn hẳn. Cần khởi động lại máy chủ.

### Tín sứ: số Hành Hiệp Lệnh khi trả nhiệm vụ lần đầu trong ngày  `TS_TRA_HANHHIEP`
- Giá trị trong tệp: `3` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Lần trả nhiệm vụ đầu tiên trong ngày tại trạm dịch nhận bấy nhiêu Hành Hiệp Lệnh (6,1,2575). Mặc định 3.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là bơm Hành Hiệp Lệnh vào server. Cần khởi động lại máy chủ.

### Tín sứ: số Tín Sứ Bảo Rương mỗi lần trả nhiệm vụ  `TS_TRA_BAORUONG`
- Giá trị trong tệp: `2` cai · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- mọi lần trả nhiệm vụ nhận bấy nhiêu Tín Sứ Bảo Rương (6,1,3430). Mặc định 2.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Rương có exp và vật phẩm, tăng là tăng thưởng toàn server. Cần khởi động lại máy chủ.

### Thành bảo: cấp tối thiểu  `TC_CAP_TOITHIEU`
- Giá trị trong tệp: `90` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp tối thiểu để tham gia Bang Hội Thành Bảo (bản đồ 984), áp dụng ở cả ba chỗ kiểm. Mặc định 90.
  Chú ý: client hiện chưa có dữ liệu bản đồ 984 nên người chơi chưa vào được cho tới khi bổ sung pak client (máy chủ đã sẵn sàng).
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp là người yếu vào bị Thủ Vệ hạ.

### Thành bảo: số phút phải vào bang trước khi tham gia  `TC_VAO_BANG_PHUT`
- Giá trị trong tệp: `1440` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Phải là thành viên bang ít nhất bấy nhiêu phút mới được tham gia (1440 = 1 ngày, chống nhảy bang). Mặc định 1440.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là vừa vào bang đã đánh được, bang lớn kéo người vào tạm.

### Thành bảo: thứ mở cửa trong tuần  `TC_THU`
- Giá trị trong tệp: `0`  · kiểu số nguyên · khoảng 0..6 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Ngày trong tuần mở Thành Bảo: 0 = Chủ nhật, 1 = Thứ hai ... 6 = Thứ bảy. Mặc định 0.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Chỉ nhận 0..6. Cần khởi động lại máy chủ. Lời thoại NPC ghi cứng giờ cũ, chỉ là chữ.

### Thành bảo: giờ bắt đầu nhận điểm Thần Mộc  `TC_GIO_NHANDIEM_TU`
- Giá trị trong tệp: `1900` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Đầu khung giờ được nhận điểm Thần Mộc, dạng HH:MM giờ máy chủ. Mặc định 19:00.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt sau giờ kết thúc là không ai nhận được điểm.

### Thành bảo: giờ kết thúc nhận điểm Thần Mộc (HHMM)  `TC_GIO_NHANDIEM_DEN`
- Giá trị trong tệp: `2400` HHMM · kiểu số nguyên · khoảng 0..2400 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cuối khung giờ nhận điểm, ghi dạng số HHMM, 2400 = hết ngày. Mặc định 2400.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Ghi dạng số HHMM (1930 = 19:30), tối đa 2400. Nhỏ hơn giờ bắt đầu là khung rỗng.

### Thành bảo: số phút lưu trú tối thiểu  `TC_PHUT_TOITHIEU`
- Giá trị trong tệp: `45` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Phải ở trong bản đồ ít nhất bấy nhiêu phút mới được nhận điểm. Mặc định 45.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là vào ra nhận điểm ngay.

### Thành bảo: giá đổi Thần Mộc Lệnh Thanh Đồng  `TC_GIA_LENH_1`
- Giá trị trong tệp: `10` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số điểm Thần Mộc để đổi một Thần Mộc Lệnh Thanh Đồng (3205). Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là đổi miễn phí. Ba mức nên tăng dần.

### Thành bảo: giá đổi Thần Mộc Lệnh Bạch Ngân  `TC_GIA_LENH_2`
- Giá trị trong tệp: `20` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số điểm Thần Mộc để đổi một Thần Mộc Lệnh Bạch Ngân (3206). Mặc định 20.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là đổi miễn phí. Ba mức nên tăng dần.

### Thành bảo: giá đổi Thần Mộc Lệnh Hoàng Kim  `TC_GIA_LENH_3`
- Giá trị trong tệp: `120` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số điểm Thần Mộc để đổi một Thần Mộc Lệnh Hoàng Kim (3207). Mặc định 120.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là đổi miễn phí. Ba mức nên tăng dần.

### Thành bảo: số lệnh đổi tối đa một lần  `TC_DOI_LENH_TOIDA`
- Giá trị trong tệp: `100` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Mỗi lần đổi tối đa bấy nhiêu Thần Mộc Lệnh. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Chỉ là tiện ích, không đổi tổng số được đổi.

### Thành bảo: trần mỗi loại lệnh mỗi tuần  `TC_LENH_TUAN`
- Giá trị trong tệp: `5` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Mỗi người mỗi tuần đổi tối đa bấy nhiêu lệnh mỗi loại. Mặc định 5.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt cao là thưởng Thành Bảo (exp, chân nguyên) không trần.

### Thành bảo: giá Bùa triệu Thủ Vệ  `TC_GIA_BUA`
- Giá trị trong tệp: `200` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số điểm Thần Mộc để mua một Bùa triệu Thủ Vệ (3204). Mặc định 200.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 0 là Thủ Vệ miễn phí, bang lớn triệu kín bản đồ.

### Thành bảo: số bùa mua tối đa một lần  `TC_MUA_BUA_TOIDA`
- Giá trị trong tệp: `100` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Mỗi lần mua tối đa bấy nhiêu bùa. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Chỉ là tiện ích.

### Thành bảo: trần tổng số Thủ Vệ trong bản đồ  `TC_TRAN_THUVE`
- Giá trị trong tệp: `100` con · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Tổng số NPC Thủ Vệ được đứng trong bản đồ cùng lúc. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt cao là bản đồ đầy NPC, tải cao và khó di chuyển.

### Thành bảo: bán kính dùng bùa quanh cây  `TC_BANKINH_BUA`
- Giá trị trong tệp: `15` met · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Phải đứng cách cây Thần Mộc trong bán kính này (mét) mới dùng được bùa. Mặc định 15.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt quá nhỏ là không ai dùng được bùa.

## Viêm Đế (`VIEMDE`) - 11 khoá

Viêm Đế Bảo Tàng (vượt ải tổ đội): công tắc, giờ mở báo danh, cấp, số lần, cỡ tổ đội, số phòng, thời gian báo danh và thời hạn trận.

### Viêm Đế: bật toàn bộ tính năng  `YDBZ_BAT`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- 1 = bật Viêm Đế Bảo Tàng. 0 = tắt hẳn (driver không làm gì). Công tắc BAT_VIEMDE ở nhóm Hoạt động cũng phải bật.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tắt giữa chừng cần khởi động lại máy chủ mới có hiệu lực; trận đang chạy không bị cắt.

### Viêm Đế: các giờ mở báo danh  `YDBZ_GIO`
- Giá trị trong tệp: `"0825,1025,1425,1625,1825,2025,2225"`  · kiểu chuỗi · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Danh sách giờ mở báo danh trong ngày, dạng HHMM cách nhau bằng dấu phẩy, giờ máy chủ. Gốc Linux: 0825,1025,1425,1625,1825,2025,2225.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Sai định dạng (thiếu số 0 đầu, dấu chấm phẩy, khoảng trắng lạ) là script không nhận giờ nào, hoạt động không mở. Mỗi mốc cách nhau ít nhất báo danh + trận (5 + 30 phút).

### Viêm Đế: cấp tối thiểu báo danh  `YDBZ_CAP_TOITHIEU`
- Giá trị trong tệp: `120` cap · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Cấp tối thiểu để báo danh. Gốc head.lua:48 = 120.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Hạ thấp là đội yếu không qua nổi ải, tốn lượt.

### Viêm Đế: số lần tham gia tối đa mỗi tuần  `YDBZ_LAN_TUAN`
- Giá trị trong tệp: `10` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Trần số lần mỗi người tham gia trong tuần (mang Viêm Đế Lệnh thì được miễn trần, đúng bản gốc). Gốc 10.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Tăng là thưởng Viêm Đế nhân lên. Nên lớn hơn hoặc bằng trần ngày.

### Viêm Đế: số lần tham gia tối đa mỗi ngày  `YDBZ_LAN_NGAY`
- Giá trị trong tệp: `4` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Trần số lần mỗi người tham gia trong ngày. Gốc 4.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt lớn hơn trần tuần là vô nghĩa.

### Viêm Đế: số người tối thiểu một tổ đội  `YDBZ_DOI_MIN`
- Giá trị trong tệp: `4` nguoi · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Tổ đội phải có ít nhất bấy nhiêu người mới được vào. Gốc 4.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt 1 là đi một mình được, ải thiết kế cho 4-6 người.

### Viêm Đế: số người tối đa một tổ đội  `YDBZ_DOI_MAX`
- Giá trị trong tệp: `6` nguoi · kiểu số nguyên · khoảng 1..8 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Tổ đội tối đa bấy nhiêu người. Gốc 6.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Phải lớn hơn hoặc bằng YDBZ_DOI_MIN và không quá 8 (cỡ tổ đội engine).

### Viêm Đế: số phòng (tổ đội) tối đa mỗi đợt  `YDBZ_PHONG_TOIDA`
- Giá trị trong tệp: `15` phong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Số tổ đội (= số phòng chờ) tối đa mỗi đợt. Gốc 15. Mỗi phòng là một bản đồ riêng (hiện 1003-1017).
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đổi số này phải khai thêm bản đồ vào MapList.ini và WorldSet_GameServer.ini (xem ReverseTools\viemde\v02_khai_bando.py), nếu không phòng thứ 16 trở đi không có bản đồ và lỗi. Cần khởi động lại máy chủ.

### Viêm Đế: thời gian báo danh  `YDBZ_PHUT_BAODANH`
- Giá trị trong tệp: `5` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số phút báo danh mỗi đợt. Gốc ready.lua:38 = 5.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt quá dài là chồng lên mốc giờ kế trong YDBZ_GIO. Cần khởi động lại máy chủ.

### Viêm Đế: thời hạn một trận  `YDBZ_PHUT_TRAN`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số phút tối đa một trận. Gốc head.lua:42 = 30.
  Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy).
- **Cảnh báo:** Đặt ngắn là không kịp qua 3 đường x 10 ải và boss cuối.

### Viêm Đế: bản đồ đặt NPC Bình Bình cô nương  `YDBZ_NPC_MAP`
- Giá trị trong tệp: `37` map · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mã bản đồ đặt 4 NPC 'Bình Bình cô nương' (mẫu 1320). Gốc 37 = Biện Kinh.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Sai mã bản đồ là NPC không xuất hiện, không ai báo danh được. Toạ độ NPC gắn với bản đồ 37. Cần khởi động lại máy chủ.

## Bạn đồng hành (`BANDONGHANH`) - 15 khoá

Hệ Bạn Đồng Hành (pet): số ô trang bị, Tu Chân, cho ăn, giá đổi tên / ngoại quan, cấp và kỹ năng bí kíp.

### Bạn đồng hành: số ô trang bị  `BDH_SO_O_TRANGBI`
- Giá trị trong tệp: `10` o · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Số ô trang bị của Bạn Đồng Hành. Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Giảm khi người chơi đã mặc đủ ô là trang bị ở ô bị bỏ có thể mất hoặc không tháo được. Không giảm khi server đang có người chơi.

### Bạn đồng hành: số chân nguyên đổi 1 điểm Tu Chân  `BDH_CHANNGUYEN_DOI_1_TUCHAN`
- Giá trị trong tệp: `200` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Bỏ ra bấy nhiêu điểm chân nguyên để đổi lấy 1 điểm Tu Chân. Mặc định 200.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Hạ thấp là Tu Chân rẻ đi, kỹ năng bí kíp lên tối đa nhanh. Cần khởi động lại máy chủ.

### Bạn đồng hành: trần điểm Tu Chân  `BDH_TUCHAN_TOI_DA`
- Giá trị trong tệp: `20000` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số điểm Tu Chân tối đa một Bạn Đồng Hành tích được. Mặc định 20.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt thấp hơn số điểm người chơi đang có là điểm dư bị kẹt. Cần khởi động lại máy chủ.

### Bạn đồng hành: số lần cho ăn mỗi loại quả mỗi ngày  `BDH_SO_LAN_CHO_AN_MOI_NGAY`
- Giá trị trong tệp: `4` lan · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Mỗi ngày được cho ăn tối đa bấy nhiêu lần mỗi loại quả (đếm riêng từng loại). Mặc định 4.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là Bạn Đồng Hành lên cấp nhanh hơn tương ứng. Cần khởi động lại máy chủ.

### Bạn đồng hành: giá đổi ngoại quan  `BDH_GIA_DOI_NGOAI_QUAN`
- Giá trị trong tệp: `5` xu · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số Xu phải trả để đổi ngoại quan Bạn Đồng Hành. Mặc định 5.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là miễn phí. Cần khởi động lại máy chủ.

### Bạn đồng hành: giá đổi tên  `BDH_GIA_DOI_TEN`
- Giá trị trong tệp: `5` xu · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Số Xu phải trả để đổi tên Bạn Đồng Hành. Mặc định 5.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là miễn phí. Cần khởi động lại máy chủ.

### Bạn đồng hành: cấp tối đa  `BDH_CAP_TOI_DA`
- Giá trị trong tệp: `130` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Cấp tối đa của Bạn Đồng Hành. Mặc định 130. Tên biến này rất chung, chỉ đổi trong tệp petsys\common.lua.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng cấp tối đa mà bảng exp theo cấp không có dòng tương ứng là lỗi khi lên cấp. Cần khởi động lại máy chủ.

### Bạn đồng hành: mỗi bao nhiêu cấp mở thêm nhóm ngoại quan  `BDH_BUOC_CAP_NGOAI_QUAN`
- Giá trị trong tệp: `10` cap · kiểu số nguyên · khoảng 1..200 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Cứ mỗi bấy nhiêu cấp thì mở thêm một nhóm ngoại quan. Mặc định 10.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 làm phép chia lỗi. Cần khởi động lại máy chủ.

### Bạn đồng hành: cấp nhân vật tối thiểu để tạo  `BDH_CAP_NHANVAT_TOI_THIEU`
- Giá trị trong tệp: `150` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Nhân vật phải đạt cấp này mới tạo được Bạn Đồng Hành. Mặc định 150.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Hạ thấp là tân thủ có pet sớm, đổi cân bằng đầu game. Cần khởi động lại máy chủ.

### Bạn đồng hành: cấp bắt đầu học kỹ năng bí kíp  `BDH_CAP_MO_KYNANG_BIKIP`
- Giá trị trong tệp: `21` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Bạn Đồng Hành phải đạt cấp này mới học được kỹ năng từ Bí kíp. Mặc định 21.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Cần khởi động lại máy chủ.

### Bạn đồng hành: số ô kỹ năng bị động tối đa  `BDH_SO_O_KYNANG_TOI_DA`
- Giá trị trong tệp: `4` o · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Số ô kỹ năng bị động tối đa. Mặc định 4.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Giảm khi pet đã học đủ ô là kỹ năng ở ô bị bỏ mất tác dụng. Cần khởi động lại máy chủ.

### Bạn đồng hành: mỗi bao nhiêu cấp mở thêm ô kỹ năng  `BDH_BUOC_CAP_MO_O_KYNANG`
- Giá trị trong tệp: `5` cap · kiểu số nguyên · khoảng 1..200 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Cứ mỗi bấy nhiêu cấp mở thêm một ô kỹ năng, tới trần BDH_SO_O_KYNANG_TOI_DA. Mặc định 5.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 làm phép chia lỗi. Cần khởi động lại máy chủ.

### Bạn đồng hành: cấp tối đa mỗi kỹ năng bí kíp  `BDH_KYNANG_BIKIP_CAP_TOI_DA`
- Giá trị trong tệp: `30` cap · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Mỗi kỹ năng bí kíp nâng tối đa tới cấp này (bằng điểm Tu Chân). Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Tăng là kỹ năng pet mạnh hơn trần cũ. Cần khởi động lại máy chủ.

### Bạn đồng hành: điểm Tu Chân mỗi cấp kỹ năng  `BDH_TUCHAN_MOI_CAP_KYNANG`
- Giá trị trong tệp: `10` diem · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Nâng kỹ năng từ cấp N lên N+1 tốn N nhân số này điểm Tu Chân. Mặc định 10.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Hạ thấp là kỹ năng lên tối đa rất nhanh. Cần khởi động lại máy chủ.

### Bạn đồng hành: số ngày danh hiệu tồn tại  `BDH_THOIHAN_DANHHIEU_NGAY`
- Giá trị trong tệp: `30` ngay · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Danh hiệu Bạn Đồng Hành tồn tại bấy nhiêu ngày. Mặc định 30. Khai lại y hệt ở partner_reward2.lua.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Cần khởi động lại máy chủ.

## Dã Tẩu (`DATAU`) - 3 khoá

Nhiệm vụ Dã Tẩu: số nhiệm vụ mỗi vòng, số vòng mỗi chu kỳ, số chu kỳ tối đa.

### Dã Tẩu: số nhiệm vụ mỗi vòng  `DT_SO_NV_MOI_VONG`
- Giá trị trong tệp: `20` nhiem vu · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Phải hoàn thành bấy nhiêu nhiệm vụ Dã Tẩu mới qua một vòng mới (thưởng vòng). Mặc định 20.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Hạ thấp là thưởng vòng phát dày hơn cho cả người lẫn bot Dã Tẩu. Cần khởi động lại máy chủ.

### Dã Tẩu: số vòng mỗi chu kỳ  `DT_SO_VONG_MOI_CHU_KY`
- Giá trị trong tệp: `20` vong · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Một chu kỳ Dã Tẩu gồm bấy nhiêu vòng. Mặc định 20.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đổi giữa chừng làm bộ đếm vòng của người đang làm dở lệch chu kỳ. Cần khởi động lại máy chủ.

### Dã Tẩu: số chu kỳ tối đa trước khi đếm về 0  `DT_SO_CHU_KY_TOI_DA`
- Giá trị trong tệp: `20` chu ky · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Đếm chu kỳ tới số này rồi quay về 0. Mặc định 20.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Không ảnh hưởng nhiều; đặt 0 có thể làm phép chia dư lỗi. Cần khởi động lại máy chủ.

## Boss Hoàng Kim (`BOSS`) - 16 khoá

Boss Hoàng Kim (tiểu, đại, Phong Lăng Độ): thời gian tồn tại, máu, né tránh, hồi máu, exp giết và exp lân cận, bán kính chia exp.

### Boss: số phút tồn tại nếu không ai giết  `BHK_PHUT_BOSS_TON_TAI`
- Giá trị trong tệp: `120` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Boss Hoàng Kim sống bấy nhiêu phút rồi tự biến mất nếu không ai giết. Mặc định 120.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt ngắn là boss biến mất khi đang đánh dở. Cần khởi động lại máy chủ.

### Boss: máu Boss Tiểu Hoàng Kim  `BHK_MAU_BOSS_TIEU`
- Giá trị trong tệp: `18000000` mau · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Máu của Boss Tiểu Hoàng Kim (ghi chú trong mã: mặc định cũ 8.000.000). Đang 18.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Máu quá thấp là boss chết một đòn; quá cao là không ai hạ nổi trong 120 phút. Cần khởi động lại máy chủ.

### Boss: máu Boss Đại Hoàng Kim (đang 1!)  `BHK_MAU_BOSS_DAI`
- Giá trị trong tệp: `1` mau · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Máu của boss đại, boss máy chủ và boss Phong Lăng Độ. hiện đang là 1 (chết một đòn) trong khi ghi chú nói mặc định 18.000.000 - rất có thể là giá trị thử nghiệm bỏ quên.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đang 1 máu: boss đại chết ngay đòn đầu, ai chạm trước ăn 25 triệu exp. Nếu không cố ý thì đặt lại 18.000.000. Cần khởi động lại máy chủ.

### Boss: chỉ số né tránh  `BHK_NETRANH_BOSS`
- Giá trị trong tệp: `5000`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Chỉ số né tránh của boss; càng cao người chơi càng hay đánh trượt. Mặc định 5000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Quá cao là boss không thể đánh trúng. Cần khởi động lại máy chủ.

### Boss: lượng máu tự hồi mỗi nhịp  `BHK_HOIMAU_BOSS`
- Giá trị trong tệp: `300` mau · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Boss tự hồi bấy nhiêu máu mỗi nhịp. Mặc định 300.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt lớn hơn sát thương của cả nhóm là boss bất tử. Cần khởi động lại máy chủ.

### Boss: exp gắn thẳng vào NPC boss đại  `BHK_EXP_NPC_BOSS_DAI`
- Giá trị trong tệp: `0` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Exp gắn thẳng vào con NPC boss đại (đang 0; exp thưởng thật phát ở tệp death*). Mặc định 0.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt khác 0 là cộng thêm exp ngoài phần thưởng death*, dễ thành hai lần thưởng. Cần khởi động lại máy chủ.

### Boss: exp gắn thẳng vào NPC boss tiểu  `BHK_EXP_NPC_BOSS_TIEU`
- Giá trị trong tệp: `0` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Exp gắn thẳng vào con NPC boss tiểu (đang 0; exp thưởng thật phát ở tệp death*). Mặc định 0.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt khác 0 dễ thành hai lần thưởng. Cần khởi động lại máy chủ.

### Boss: exp cho người hạ Boss Hoàng Kim Phong Lăng Độ  `BHK_EXP_GIET_BOSS_PLD`
- Giá trị trong tệp: `50000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người hạ Boss Hoàng Kim Phong Lăng Độ và tổ đội của họ. Mặc định 50.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Exp phát thẳng, nhân với số boss mỗi ngày; thêm một số 0 là bơm hàng trăm triệu exp. Cần khởi động lại máy chủ.

### Boss: exp cho người đứng gần khi hạ Boss Hoàng Kim Phong Lăng Độ  `BHK_EXP_LANCAN_BOSS_PLD`
- Giá trị trong tệp: `20000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người đứng trong bán kính chia exp quanh xác Boss Hoàng Kim Phong Lăng Độ nhưng khác tổ đội với người hạ. Mặc định 20.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt cao là đứng xem boss cũng có exp lớn, bot đứng gần boss hưởng ké. Cần khởi động lại máy chủ.

### Boss: bán kính chia exp quanh xác Boss Hoàng Kim Phong Lăng Độ  `BHK_PHAMVI_HUONG_EXP_PLD`
- Giá trị trong tệp: `200` o · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Bán kính (ô) quanh xác Boss Hoàng Kim Phong Lăng Độ mà người chơi phải đứng trong đó mới được chia exp lân cận. Mặc định 200.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt rất lớn là cả bản đồ hưởng exp lân cận. Cần khởi động lại máy chủ.

### Boss: exp cho người hạ Boss Đại Hoàng Kim  `BHK_EXP_GIET_BOSS_DAI`
- Giá trị trong tệp: `25000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người hạ Boss Đại Hoàng Kim và tổ đội của họ. Mặc định 25.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Exp phát thẳng, nhân với số boss mỗi ngày; thêm một số 0 là bơm hàng trăm triệu exp. Cần khởi động lại máy chủ.

### Boss: exp cho người đứng gần khi hạ Boss Đại Hoàng Kim  `BHK_EXP_LANCAN_BOSS_DAI`
- Giá trị trong tệp: `10000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người đứng trong bán kính chia exp quanh xác Boss Đại Hoàng Kim nhưng khác tổ đội với người hạ. Mặc định 10.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt cao là đứng xem boss cũng có exp lớn, bot đứng gần boss hưởng ké. Cần khởi động lại máy chủ.

### Boss: bán kính chia exp quanh xác Boss Đại Hoàng Kim  `BHK_PHAMVI_HUONG_EXP_DAI`
- Giá trị trong tệp: `200` o · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Bán kính (ô) quanh xác Boss Đại Hoàng Kim mà người chơi phải đứng trong đó mới được chia exp lân cận. Mặc định 200.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt rất lớn là cả bản đồ hưởng exp lân cận. Cần khởi động lại máy chủ.

### Boss: exp cho người hạ Boss Tiểu Hoàng Kim  `BHK_EXP_GIET_BOSS_TIEU`
- Giá trị trong tệp: `20000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người hạ Boss Tiểu Hoàng Kim và tổ đội của họ. Mặc định 20.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Exp phát thẳng, nhân với số boss mỗi ngày; thêm một số 0 là bơm hàng trăm triệu exp. Cần khởi động lại máy chủ.

### Boss: exp cho người đứng gần khi hạ Boss Tiểu Hoàng Kim  `BHK_EXP_LANCAN_BOSS_TIEU`
- Giá trị trong tệp: `10000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cho người đứng trong bán kính chia exp quanh xác Boss Tiểu Hoàng Kim nhưng khác tổ đội với người hạ. Mặc định 10.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt cao là đứng xem boss cũng có exp lớn, bot đứng gần boss hưởng ké. Cần khởi động lại máy chủ.

### Boss: bán kính chia exp quanh xác Boss Tiểu Hoàng Kim  `BHK_PHAMVI_HUONG_EXP_TIEU`
- Giá trị trong tệp: `200` o · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Bán kính (ô) quanh xác Boss Tiểu Hoàng Kim mà người chơi phải đứng trong đó mới được chia exp lân cận. Mặc định 200.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt rất lớn là cả bản đồ hưởng exp lân cận. Cần khởi động lại máy chủ.

## Vận tiêu (`VANTIEU`) - 19 khoá

Thưởng vận tiêu ba loại tiêu (đồng / bạc / vàng): exp, Hộ Mạch Đơn, Chân Nguyên Đơn, rương, Lệnh Bài Boss, điểm bang hội.

### Vận tiêu: số lần cộng exp nền  `VT_EXP_NEN_LAN`
- Giá trị trong tệp: `1000` lan · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Exp nền được cộng thành nhiều lần: số lần nhân với VT_EXP_NEN_MOI. Mặc định 1000 (1000 x 50.000 = 50 triệu).
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Nhân với exp mỗi lần; tăng là tăng exp nền mọi chuyến tiêu.

### Vận tiêu: exp mỗi lần cộng nền  `VT_EXP_NEN_MOI`
- Giá trị trong tệp: `50000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp mỗi lần cộng nền, nhân với VT_EXP_NEN_LAN. Mặc định 50.000.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Nhân với 1000 lần; thêm một số 0 là 500 triệu mỗi chuyến.

### Vận tiêu: exp nền cộng thêm  `VT_EXP_NEN_SUM`
- Giá trị trong tệp: `500000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp nền cộng thêm một lần cho mỗi chuyến (thông báo trong game chỉ ghi 50.000.000). Mặc định 500.000.000.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Số này đã gấp 10 lần thông báo hiển thị; tăng nữa là lệch xa lời hứa với người chơi.

### Vận tiêu: exp cộng thêm khi giao tiêu đồng  `VT_EXP_DONG`
- Giá trị trong tệp: `100000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng thêm khi giao tiêu đồng thành công (thông báo trong game ghi 10.000.000). Mặc định 100.000.000.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đang gấp 10 lần số thông báo; tăng nữa là bơm exp lớn mỗi chuyến.

### Vận tiêu: exp cộng thêm khi giao tiêu bạc  `VT_EXP_BAC`
- Giá trị trong tệp: `300000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng thêm khi giao tiêu bạc thành công (thông báo trong game ghi 30.000.000). Mặc định 300.000.000.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đang gấp 10 lần số thông báo; tăng nữa là bơm exp lớn mỗi chuyến.

### Vận tiêu: exp cộng thêm khi giao tiêu vàng  `VT_EXP_VANG`
- Giá trị trong tệp: `500000000` exp · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Exp cộng thêm khi giao tiêu vàng thành công (thông báo trong game ghi 50.000.000). Mặc định 500.000.000.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Đang gấp 10 lần số thông báo; tăng nữa là bơm exp lớn mỗi chuyến.

### Vận tiêu: số Hộ Mạch Đơn khi giao tiêu đồng  `VT_HOMACH_DONG`
- Giá trị trong tệp: `300` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Hộ Mạch Đơn thưởng khi giao tiêu đồng. Mặc định 300.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Hộ Mạch Đơn vào server mỗi chuyến.

### Vận tiêu: số Chân Nguyên Đơn khi giao tiêu đồng  `VT_CHANNGUYEN_DONG`
- Giá trị trong tệp: `50` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Chân Nguyên Đơn thưởng khi giao tiêu đồng. Mặc định 50.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Chân Nguyên Đơn vào server mỗi chuyến.

### Vận tiêu: số rương trang bị xanh khi giao tiêu đồng  `VT_RUONG_DONG`
- Giá trị trong tệp: `2` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số rương trang bị xanh thưởng khi giao tiêu đồng. Mặc định 2.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là trang bị xanh tràn chợ.

### Vận tiêu: số Hộ Mạch Đơn khi giao tiêu bạc  `VT_HOMACH_BAC`
- Giá trị trong tệp: `400` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Hộ Mạch Đơn thưởng khi giao tiêu bạc. Mặc định 400.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Hộ Mạch Đơn vào server mỗi chuyến.

### Vận tiêu: số Chân Nguyên Đơn khi giao tiêu bạc  `VT_CHANNGUYEN_BAC`
- Giá trị trong tệp: `70` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Chân Nguyên Đơn thưởng khi giao tiêu bạc. Mặc định 70.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Chân Nguyên Đơn vào server mỗi chuyến.

### Vận tiêu: số rương trang bị xanh khi giao tiêu bạc  `VT_RUONG_BAC`
- Giá trị trong tệp: `5` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số rương trang bị xanh thưởng khi giao tiêu bạc. Mặc định 5.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là trang bị xanh tràn chợ.

### Vận tiêu: số Hộ Mạch Đơn khi giao tiêu vàng  `VT_HOMACH_VANG`
- Giá trị trong tệp: `500` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Hộ Mạch Đơn thưởng khi giao tiêu vàng. Mặc định 500.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Hộ Mạch Đơn vào server mỗi chuyến.

### Vận tiêu: số Chân Nguyên Đơn khi giao tiêu vàng  `VT_CHANNGUYEN_VANG`
- Giá trị trong tệp: `100` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Chân Nguyên Đơn thưởng khi giao tiêu vàng. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bơm Chân Nguyên Đơn vào server mỗi chuyến.

### Vận tiêu: số rương trang bị xanh khi giao tiêu vàng  `VT_RUONG_VANG`
- Giá trị trong tệp: `10` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số rương trang bị xanh thưởng khi giao tiêu vàng. Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là trang bị xanh tràn chợ.

### Vận tiêu: số Lệnh Bài Boss mỗi chuyến  `VT_LENHBAI_BOSS`
- Giá trị trong tệp: `10` cai · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số Lệnh Bài Boss thưởng mỗi chuyến (mọi loại tiêu). Mặc định 10.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là lệnh bài boss tràn server.

### Vận tiêu: điểm bang hội khi giao tiêu đồng  `VT_DIEM_TONG_DONG`
- Giá trị trong tệp: `100` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Điểm cộng cho bang hội khi giao tiêu đồng. Mặc định 100.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bang có nhiều người vận tiêu lên cấp bang rất nhanh.

### Vận tiêu: điểm bang hội khi giao tiêu bạc  `VT_DIEM_TONG_BAC`
- Giá trị trong tệp: `200` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Điểm cộng cho bang hội khi giao tiêu bạc. Mặc định 200.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bang có nhiều người vận tiêu lên cấp bang rất nhanh.

### Vận tiêu: điểm bang hội khi giao tiêu vàng  `VT_DIEM_TONG_VANG`
- Giá trị trong tệp: `300` diem · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Điểm cộng cho bang hội khi giao tiêu vàng. Mặc định 300.
  Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tăng là bang có nhiều người vận tiêu lên cấp bang rất nhanh.

## Trống bang hội (`TRONGBANG`) - 6 khoá

Trống Khải Hoàn của bang: thời gian tồn tại, khung giờ được đánh, exp mỗi nhịp và trần exp mỗi người mỗi ngày.

### Trống bang: số phút trống tồn tại  `TBH_PHUT_TRONG_TON_TAI`
- Giá trị trong tệp: `30` phut · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Trống Khải Hoàn do bang chủ đánh ra tồn tại bấy nhiêu phút. Mặc định 30.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt dài là trống đứng chiếm chỗ lâu. Cần khởi động lại máy chủ.

### Trống bang: giờ sớm nhất được đánh trống  `TBH_GIO_MO`
- Giá trị trong tệp: `1900` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Giờ sớm nhất trong ngày được dùng Trống Khải Hoàn, dạng HH:MM giờ máy chủ. Mặc định 19:00.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt sau giờ đóng là không bao giờ đánh được. Cần khởi động lại máy chủ.

### Trống bang: giờ muộn nhất được đánh trống  `TBH_GIO_DONG`
- Giá trị trong tệp: `2359` HHMM · kiểu giờ HH:MM · khoảng 0..2359 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Giờ muộn nhất trong ngày được dùng Trống Khải Hoàn, dạng HH:MM giờ máy chủ. Mặc định 23:59.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Phải sau giờ mở. Cần khởi động lại máy chủ.

### Trống bang: trần exp mỗi người mỗi ngày  `TBH_EXP_TRAN_NGAY`
- Giá trị trong tệp: `200000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mỗi người nhận tối đa bấy nhiêu exp mỗi ngày từ đánh trống bang. Mặc định 200.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đây là trần kinh tế; bỏ trần là exp trống bang không giới hạn. Cần khởi động lại máy chủ.

### Trống bang: exp thấp nhất mỗi nhịp  `TBH_EXP_MIN`
- Giá trị trong tệp: `1000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp thấp nhất cộng cho mỗi nhịp (một lần đánh có 10 nhịp). Mặc định 1.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Nhân 10 nhịp mỗi lần đánh; phải nhỏ hơn hoặc bằng TBH_EXP_MAX. Cần khởi động lại máy chủ.

### Trống bang: exp cao nhất mỗi nhịp  `TBH_EXP_MAX`
- Giá trị trong tệp: `2000000` exp · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Exp cao nhất cộng cho mỗi nhịp (một lần đánh có 10 nhịp). Mặc định 2.000.000.
  Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Nhân 10 nhịp mỗi lần đánh; phải lớn hơn hoặc bằng TBH_EXP_MIN. Cần khởi động lại máy chủ.

## Chung (`CHUNG`) - 20 khoá

Hệ số sức mạnh boss / quái của các hoạt động bản Việt cũ, Nguyên Soái Tống Kim, và toàn bộ hệ BOT tự động (số bot, Dã Tẩu, bày sạp, Tống Kim).

### Hệ số sát thương Boss Hoàng Kim tiểu  `GLB_SATTHUONG_BOSS_HK`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hệ số nhân vào sát thương của Boss Hoàng Kim tiểu bản Việt cũ. 1 = giữ nguyên, 2 = gấp đôi. Hoạt động bản Việt cũ này hiện đang tắt (thay bằng bản Linux) nên khoá gần như không có tác dụng; giữ lại để tương thích.
  Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ.
- **Cảnh báo:** Không phải công tắc bật/tắt: đặt 0 là quái không gây sát thương (hoặc máu 0) chứ không phải tắt hoạt động. Chỉ có ý nghĩa khi bật lại hoạt động bản Việt cũ.

### Hệ số sát thương Thuỷ Tặc Phong Lăng Độ (bản Việt cũ)  `GLB_SATTHUONG_QUAI_PLD`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hệ số nhân vào sát thương quái Thuỷ Tặc trên thuyền Phong Lăng Độ của bản Việt cũ. 1 = giữ nguyên, 2 = gấp đôi. Hoạt động bản Việt cũ này hiện đang tắt (thay bằng bản Linux) nên khoá gần như không có tác dụng; giữ lại để tương thích.
  Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ.
- **Cảnh báo:** Không phải công tắc bật/tắt: đặt 0 là quái không gây sát thương (hoặc máu 0) chứ không phải tắt hoạt động. Chỉ có ý nghĩa khi bật lại hoạt động bản Việt cũ.

### Hệ số sát thương boss Thuỷ Tặc Đầu Lĩnh (bản Việt cũ)  `GLB_SATTHUONG_BOSS_TTDL`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hệ số nhân vào sát thương boss Thuỷ Tặc Đầu Lĩnh của bản Việt cũ. 1 = giữ nguyên, 2 = gấp đôi. Hoạt động bản Việt cũ này hiện đang tắt (thay bằng bản Linux) nên khoá gần như không có tác dụng; giữ lại để tương thích.
  Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ.
- **Cảnh báo:** Không phải công tắc bật/tắt: đặt 0 là quái không gây sát thương (hoặc máu 0) chứ không phải tắt hoạt động. Chỉ có ý nghĩa khi bật lại hoạt động bản Việt cũ.

### Hệ số máu và sát thương quái Vượt ải (bản Việt cũ)  `GLB_MANH_BOSS_VUOTAI`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Hệ số nhân vào máu và sát thương của quái và boss Vượt ải bản Việt cũ. 1 = giữ nguyên, 2 = gấp đôi. Hoạt động bản Việt cũ này hiện đang tắt (thay bằng bản Linux) nên khoá gần như không có tác dụng; giữ lại để tương thích.
  Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ.
- **Cảnh báo:** Không phải công tắc bật/tắt: đặt 0 là quái không gây sát thương (hoặc máu 0) chứ không phải tắt hoạt động. Chỉ có ý nghĩa khi bật lại hoạt động bản Việt cũ.

### Hệ số máu và sát thương Nguyên Soái Tống Kim  `GLB_MANH_NGUYENSOAI_TK`
- Giá trị trong tệp: `1` x · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Cẩn thận**
- Nhân vào máu (TKT_MAU_NGUYENSOAI) và sát thương của Nguyên Soái hai phe trong Tống Kim. 1 = giữ nguyên, 2 = gấp đôi. Đang chạy thật.
  Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt 0 là Nguyên Soái 0 máu, chết ngay khi xuất hiện và trận kết thúc lập tức. Đặt 10 là không ai hạ nổi. Tăng từng bước và thử một trận trước.

### Trần số dòng thuộc tính đồ rơi (chưa nối)  `GLB_MAX_DONG_THUOCTINH`
- Giá trị trong tệp: `10`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- Dự kiến là trần số dòng thuộc tính của trang bị rơi từ quái, nhưng hiện không tệp script nào đọc khoá này; trần thật đang nằm cứng trong mã C++. Đổi ở đây không có tác dụng.
- **Cảnh báo:** Đổi không có tác dụng gì cho tới khi có script hoặc C++ đọc khoá này. Đừng trông chờ đồ rơi đổi số dòng.

### Bật hệ bot tự động  `BOT_TU_GOI`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = mỗi lần máy chủ lên, script tự gọi bot và phân việc (Dã Tẩu, bày sạp, Tống Kim, đánh quái) theo các khoá BOT_* bên dưới. 0 = tắt toàn bộ hệ bot tự động (bot đang có vẫn tồn tại tới khi thoát).
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Tắt là server vắng bot, chợ không có sạp bot, Tống Kim không có bot. Bật lại sau khi tắt lâu có thể mất vài phút để xếp hàng sinh bot.

### Số bot gọi khi máy chủ lên  `BOT_SO_LUONG`
- Giá trị trong tệp: `1000`  · kiểu số nguyên · khoảng 0..1000 · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Tổng số bot tự động được sinh sau khi máy chủ lên. Trần cứng của engine là 1000 (PB_MAX_BOTS); đặt lớn hơn cũng chỉ sinh được 1000. Mặc định 1000.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Sinh hàng trăm bot cùng lúc là gánh nặng lên Goddess và băng thông; vụ sập 15:05 ngày 04/09 xảy ra khi 783 bot đăng nhập dồn. Tăng số bot phải đi cùng BOT_CHO_PHUT và BOT_GOI_GIAN đủ lớn. Không vượt 1000.

### Chờ bao lâu sau khi máy chủ lên mới gọi bot  `BOT_CHO_PHUT`
- Giá trị trong tệp: `2` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số phút chờ sau khi máy chủ lên rồi mới bắt đầu sinh bot, để Goddess (máy chủ dữ liệu) sẵn sàng. Mặc định 2.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Đặt 0 là bot đăng nhập khi Goddess chưa xong, dễ lỗi nạp dữ liệu nhân vật bot. Giữ từ 1 phút trở lên.

### Giãn cách tối thiểu giữa hai đợt sinh bot  `BOT_GOI_GIAN`
- Giá trị trong tệp: `10` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Nguy hiểm**
- Tối thiểu bao nhiêu phút giữa hai lần xếp hàng đợi sinh bot. Mặc định 10.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Đặt 0 là các đợt sinh bot chồng lên nhau, đăng nhập dồn và có thể sập máy chủ như ngày 04/09. Giữ từ 5 phút trở lên.

### Số bot đi làm Dã Tẩu  `BOT_DA_TAU`
- Giá trị trong tệp: `200` bot · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số bot được phân đi làm nhiệm vụ Dã Tẩu. Chỉ chọn được bot cấp từ 70 trở lên nên số thật có thể ít hơn. Mặc định 200.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Nhiều bot Dã Tẩu là nhiều bot chạy khắp bản đồ, tăng tải đường đi và gói tin. Không nên vượt số bot thật đang có.

### Số bot về thành bày sạp  `BOT_BAN_SAP`
- Giá trị trong tệp: `200` bot · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số bot về thành bày sạp bán hàng (theo lưới sạp SimCity, tránh quanh NPC). Mặc định 200.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Nhiều sạp là thành chật, người chơi khó bấm NPC. Số sạp bị giới hạn bởi số ô sạp có sẵn trên bản đồ.

### Ngưỡng phần trăm sạp còn lại để đặt lại sạp  `BOT_SAP_NGUONG`
- Giá trị trong tệp: `60` % · kiểu số nguyên · khoảng 0..100 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Khi số sạp bot còn bày dưới số phần trăm này so với BOT_BAN_SAP thì script dẹp hết sạp cũ và bày lại. Mặc định 60.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Đặt 100 là bày lại liên tục (sạp nhấp nháy, tốn gói tin). Đặt 0 là không bao giờ bày lại.

### Giãn cách tối thiểu giữa hai lần đặt lại sạp  `BOT_SAP_GIAN`
- Giá trị trong tệp: `15` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Tối thiểu bao nhiêu phút giữa hai lần dẹp và bày lại sạp bot. Mặc định 15.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Đặt 0 kết hợp ngưỡng cao là sạp bị dẹp/bày lại mỗi phút, người chơi đang xem sạp bị đóng.

### Trần số bot mỗi trận Tống Kim  `BOT_TK_TRAN`
- Giá trị trong tệp: `500` bot · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- Số bot tối đa tham gia một trận Tống Kim (chia hai phe). 0 = không giới hạn. Mặc định 500.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Quá nhiều bot trong một bản đồ Tống Kim là máy chủ phát gói tới hàng trăm người mỗi hành động; đây là gốc của hiện tượng 'không thấy địch/chiêu' ngày 03-04/09. Giữ dưới 500.

### Bot tự vào Tống Kim theo giờ  `BOT_TK_TU_DONG`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- 1 = bot tự báo danh và vào Tống Kim theo lịch TK_LICH. 0 = bot không tham gia Tống Kim.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Tắt là Tống Kim chỉ còn người thật, trận có thể không đủ người.

### Nhịp in trạng thái [BotAuto] ra log  `BOT_BAO_PHUT`
- Giá trị trong tệp: `10` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cứ bao nhiêu phút in một dòng trạng thái [BotAuto] vào logs\hethong.log. 0 = không in. Mặc định 10.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Chỉ ảnh hưởng log; đặt 1 làm log dài nhanh.

### Bot tự vào môn phái  `BOT_VAO_PHAI`
- Giá trị trong tệp: `0`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- 1 = bot chưa có phái sẽ tự vào phái. 0 = không (mặc định, vì bot đã trên cấp 100 và có phái sẵn).
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Bật khi bot đã có phái không hại gì; chỉ tốn vài lần kiểm tra. Bot mới sinh chưa có phái mà tắt thì bot không có kỹ năng phái.

### Bot tự bật đánh quái  `BOT_TU_DANH`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: trong 1,5 phút · nguy cơ: **Cẩn thận**
- 1 = script tự bật chế độ đánh quái cho bot (bot ra bản đồ luyện cấp). 0 = bot đứng yên ở thành sau khi sinh.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Tắt là toàn bộ bot đứng yên ở thành, trông như server chết. Bật với số bot lớn là tải đường đi và gói tin tăng.

### Nhịp gọi lại lệnh bật đánh quái cho bot  `BOT_DANH_GIAN`
- Giá trị trong tệp: `5` phut · kiểu số nguyên · hiệu lực: trong 1,5 phút · nguy cơ: **An toàn**
- Cứ bao nhiêu phút gọi lại lệnh bật đánh quái (để bot mới sinh hoặc bot vừa xong việc khác cũng được bật). Mặc định 5.
  Có hiệu lực trong vòng 1 phút.
- **Cảnh báo:** Đặt 0 hoặc 1 là gọi mỗi phút, tốn vài chục mili giây mỗi lần với 1000 bot; không hại nhưng vô ích.

## Hệ thống (`HETHONG`) - 5 khoá

Mốc giờ mở server, chế độ thử nghiệm, nhịp nạp lại script và ghi log. Nhóm này ảnh hưởng toàn máy chủ - web luôn hỏi lại trước khi lưu.

### Mốc giờ mở cửa máy chủ (yymmddHHMM)  `GLB_GIO_MO_SERVER`
- Giá trị trong tệp: `2506251900`  · kiểu số nguyên · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- Mốc thời gian mở cửa, viết liền dạng năm-tháng-ngày-giờ-phút hai chữ số mỗi phần: 2506251900 = 19:00 ngày 25/06/2025. Bảy chỗ trong script đọc mốc này để chặn người chơi làm một số việc trước giờ mở (giờ máy chủ).
  Chú ý: script\lib\worldlibrary.lua còn một bản trùng tên (giá trị cũ 2104011900) không bao giờ thắng bản này. Chốt khi nạp script, cần khởi động lại máy chủ.
- **Cảnh báo:** Đặt mốc ở tương lai là các chức năng phụ thuộc giờ mở bị khoá cho mọi người chơi tới lúc đó. Gõ thiếu một chữ số là mốc thành ngày vô nghĩa. Nhóm Hệ thống, cần khởi động lại máy chủ.

### Chế độ thử nghiệm (NPC Hỗ Trợ Test)  `GLB_CHE_DO_TEST`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- 1 = mở chế độ thử nghiệm: hai NPC 'Hỗ Trợ Test' tại làng tân thủ (bản đồ 53) mở menu GM đầy đủ cho mọi người chơi (1 tỷ lượng, 100.000 Xu, lên thẳng cấp 200, không kiểm tài khoản GM, không giới hạn số lần). 0 = tắt, đổi về NPC tân thủ thật.
  Chốt khi nạp script, cần khởi động lại máy chủ. Xem BAOCAO_LOHONG_2908.md mục 1.
- **Cảnh báo:** Đang mở trên máy chủ thật là lỗ hổng lớn nhất của server: ai cũng tự phát đồ GM. Trước khi mở cửa cho người chơi thật phải đặt 0 và khởi động lại máy chủ.

### Nhịp tự nạp lại script (phút)  `CH_NAPLAI_PHUT`
- Giá trị trong tệp: `1` phut · kiểu số nguyên · khoảng 0..60 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **Nguy hiểm**
- timerserver.lua tự nạp lại chính nó (33 Include, khoảng 103 tệp, gần 1 MB) theo nhịp này. 1 = mỗi phút (mặc định, sửa script ăn ngay). 5 = năm phút một lần, nhẹ máy chủ hơn năm lần. 0 = tắt hẳn: sửa script phải khởi động lại máy chủ mới có hiệu lực.
  Khoá này được đọc lúc tick nên đổi có hiệu lực ở nhịp kế tiếp; đã đặt 0 thì chỉ khởi động lại mới bật lại được.
- **Cảnh báo:** Đặt 0 là khoá luôn cơ chế nạp lại: mọi khoá script trên web (kể cả chính khoá này) chỉ còn đổi được khi khởi động lại máy chủ. Đặt quá lớn làm sửa script chờ lâu mới ăn. Khoảng hợp lý 0..60.

### Ghi log hệ cấu hình  `CH_LOG_CAUHINH`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- 1 = hệ cấu hình ghi nhật ký vào logs\hethong.log (hàm GhiLog của script). 0 = im lặng. Cần khởi động lại máy chủ.
- **Cảnh báo:** Tắt thì khi cấu hình không ăn sẽ không còn dấu vết để tra. Chỉ tắt khi log quá nhiều.

### Ghi log hệ thưởng  `CH_LOG_THUONG`
- Giá trị trong tệp: `1`  · kiểu bật/tắt · khoảng 0..1 · hiệu lực: cần khởi động lại máy chủ · nguy cơ: **An toàn**
- 1 = hệ phát thưởng (ch_thuong_lib.lua) ghi mỗi lần phát thưởng vào logs\hethong.log. 0 = im lặng. Có hiệu lực trong vòng 1,5 phút.
- **Cảnh báo:** Tắt thì mất dấu vết ai nhận thưởng gì, khó điều tra khiếu nại hay nhân bản đồ. Nên để 1.

