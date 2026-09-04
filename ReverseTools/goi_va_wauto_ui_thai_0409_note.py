# -*- coding: utf-8 -*-
"""
goi_va_wauto_ui_thai_0409_note.py - DOT 3 cua viec dung lai giao dien WAuto theo auto Thai:
HOP NOTE HUONG DAN (bat chuoc FormTip cua auto Thai) + noi dung cho ca 16 tab.

  - Resource.h : IDD_HELP_DIALOG 750, IDC_HELP_TEXT 751
  - WAuto.rc   : mau hop thoai note
  - WAuto.cpp  : s_aNote[16], s_szNoteChung, s_szNoteVungTren, HelpDlgProc, WA_MoNote
                 + moc tam: bam chu "Vo Lam Ngao The" o day cua so = mo note cua tab dang mo
                 (dot 4 se noi them nut [Tro giup] / [H] / [?] o vung tren).

Chay: python goi_va_wauto_ui_thai_0409_note.py [--thu]
WAuto.cpp / WAuto.rc UTF-16LE; Resource.h ASCII. Idempotent.
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="utf-16"):
        self.path = path; self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)

    def them_sau_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0] + 1] + self.N(moi).split(self.nl) + L[a[0] + 1:])
        self.log.append("  them sau dong: %s" % dau)

    def them_truoc_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0]] + self.N(moi).split(self.nl) + L[a[0]:])
        self.log.append("  them truoc dong: %s" % dau)

    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi, 1); self.log.append("  thay: %s" % dau)

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# =====================================================================
# NOI DUNG NOTE - 16 tab. '|' = xuong dong, '||' = dong trong.
# Viet theo khuon cua auto Thai: danh so tung o, cau canh bao, dong "LIEN QUAN".
# =====================================================================
NOTE = {}

NOTE[0] = ("CƠ BẢN — thông tin nhân vật và các thiết lập chung||"
 "1. Bảng số liệu phía trên chỉ để XEM: Sinh lực, Nội lực, Thể lực, Bản đồ, Toạ độ, Đẳng cấp, "
 "Kinh nghiệm của nhân vật đang chọn trong danh sách.||"
 "2. Cài đặt theo + [Đồng bộ]: chép TOÀN BỘ thiết lập của một nhân vật khác sang nhân vật đang "
 "chọn. Dùng khi mở thêm cửa sổ mới cho khỏi chỉnh lại từ đầu.||"
 "3. [v] Tự động đăng nhập: WAuto tự gõ tài khoản và chọn nhân vật khi game mở. Danh sách nhân "
 "vật tự đăng nhập nằm ngay dưới, tối đa 8 dòng.||"
 "4. [v] Ẩn cửa sổ game: giấu cửa sổ game đang chọn xuống khay. Bấm lại để hiện.||"
 "5. [v] Thoát game khi dùng Thổ Địa Phù / [v] Thoát khi đứng im 15 phút / [v] Hẹn giờ thoát: "
 "ba cách tự đóng game cho an toàn khi treo máy qua đêm.||"
 "6. [v] Tán gẫu lặp lại: tự nói một câu theo kênh đã chọn. Câu tối đa 78 ký tự.||"
 "7. KHỐI NHẸ MÁY (hai dòng cuối): ''Người chơi khác mặc chung 1 bộ'' và ''Mọi chiêu dùng chung "
 "1 hiệu ứng'' — bật khi đông người (Tống Kim, Công Thành) để đỡ giật hình.||"
 "LIÊN QUAN: muốn auto tự về thành bán đồ, mua thuốc thì sang tab Hậu cần.")

NOTE[1] = ("PHỤC HỒI — tự uống thuốc, Thổ Địa Phù, các thứ tự dùng||"
 "1. KHỐI HỒI PHỤC: hai dòng Sinh lực và Nội lực, mỗi dòng có Mức 1, Mức 2 và nhịp giây. "
 "Tụt dưới Mức 1 thì uống bình thường; tụt dưới Mức 2 thì uống gấp. Nhịp giây là khoảng cách "
 "tối thiểu giữa hai lần uống — để quá nhỏ sẽ phí thuốc.||"
 "2. KHỐI THỔ ĐỊA PHÙ: đủ một trong các điều kiện đã tick thì dùng phù về thành — Sinh lực <, "
 "Nội lực <, hết bình sinh lực, hết bình nội lực, đầy hành trang, đủ tiền, đồ hư dưới mức.||"
 "3. KHỐI TỰ SỬ DỤNG: cắn thuốc khi đầy hành trang, mở túi máu, giải độc, kinh nghiệm x2, "
 "kỹ năng x2, Nga My buff khi mất máu, buff tổ đội, Côn Lôn buff tổ đội, buff cùng phe, "
 "về thành khi chết.||"
 "CẢNH BÁO: bật ''Về thành khi chết'' trong Tống Kim hay Công Thành là mất lượt đánh — hai máy "
 "đó tự lo việc hồi sinh tại trận.||"
 "LIÊN QUAN: mua thuốc ở tab Hậu cần; số bình mang theo khi vào Tống Kim đặt ở tab TK.")

NOTE[2] = ("CHIẾN ĐẤU — auto chọn mục tiêu và ra chiêu thế nào||"
 "1. [v] Tự đánh phạm vi: KHÔNG tick thì auto đứng im. Ô số bên cạnh là TẦM QUÉT tính bằng mps — "
 "auto chỉ thấy mục tiêu trong bán kính đó. Combo bên phải chọn giữ nguyên trên ngựa hay tự xuống ngựa.||"
 "2. [v] Tiếp cận mục tiêu: mục tiêu xa hơn tầm chiêu thì chạy lại gần đúng số mps trong ô. "
 "Số nên đặt bằng tầm chiêu trừ khoảng 12, không thì đứng đánh hụt.||"
 "3. [v] Tự vệ đánh người: chỉ đánh trả người đã đánh mình trước. Ô số là tầm nhìn riêng cho "
 "việc đánh trả.||"
 "4. [v] Né boss vàng: gặp boss hoàng kim thì bỏ qua, tránh chết oan khi luyện cấp.||"
 "5. KHỐI KỸ NĂNG (dưới kẻ ngang):|"
 "- Bật hỗ trợ #1 #2 #3: ba chiêu tự bật lại khi hết hiệu lực.|"
 "- Kỹ năng đánh boss: chiêu riêng dùng khi mục tiêu là boss.|"
 "- Sinh lực % < và Nội lực % <: tụt dưới ngưỡng thì đánh chiêu đã chọn.|"
 "- Vòng sáng #1 #2: hai vòng sáng giữ thường trực.|"
 "- Đổi với tay trái ... giây: cứ chừng ấy giây đổi sang chiêu tay trái một nhịp.||"
 "LƯU Ý: hai dòng Vòng sáng dùng chung ô với tab PK — sửa ở tab nào cũng là sửa cùng một giá trị.||"
 "LIÊN QUAN: bảng chiêu kết hợp và tiên chiêu nằm ở tab Chiêu KH.")

NOTE[3] = ("DI CHUYỂN — đi theo người, chạy toạ độ, quanh điểm||"
 "1. [v] Theo sau: chọn tên trong combo rồi đặt khoảng cách. Nhân vật bám theo người đó trong "
 "cùng bản đồ.||"
 "2. Bản đồ + [Lấy]: ghi lại bản đồ hiện tại làm bản đồ làm việc.||"
 "3. [v] Quanh điểm + X/Y + [Lấy]: đánh quanh một điểm cố định, không đi lang thang.||"
 "4. [v] Chạy toạ độ: chạy vòng theo danh sách tối đa 24 điểm bên dưới. Nút [Lấy] thêm điểm "
 "đang đứng; Lên/Xuống đổi thứ tự; Đọc tệp / Ghi tệp để dùng lại cho nhân vật khác.||"
 "5. [v] Lên ngựa khi di chuyển: chỉ áp dụng lúc đi đường dài.||"
 "6. [v] Đánh quái trên đường đi / [v] Vây quái khi đến toạ độ: vừa đi vừa đánh, tới nơi thì "
 "gom quái quanh điểm.||"
 "CẢNH BÁO: chạy toạ độ trong map sự kiện (Tống Kim, Công Thành) bị các máy sự kiện tạm dừng — "
 "đó là cố ý, không phải lỗi.||"
 "LIÊN QUAN: luật lên/xuống ngựa khi gặp địch nằm ở tab PK và tab Chiến đấu.")

NOTE[4] = ("NHẶT ĐỒ — nhặt gì, bỏ gì||"
 "1. [v] Tự nhặt đồ: công tắc chính. [v] Chạy tới phạm vi + số mps: cho phép chạy tới chỗ đồ rơi.||"
 "2. [v] Trong thành: có nhặt khi đang ở thành hay không.||"
 "3. Loại: lọc thô theo loại vật phẩm (tất cả / tiền và đặc phẩm / đồ màu / đồ 1 ô / chỉ tiền).||"
 "4. [v] Không nhặt theo tên + nút [Cài đặt tên không nhặt]: danh sách tên KHÔNG nhặt. "
 "Danh sách chỉ liệt kê tên vật phẩm ĐANG CÓ TRONG TÚI, nên muốn cấm một món thì phải có nó "
 "trong túi một lần.||"
 "5. [v] Lọc: giữ lại trang bị có dòng thuộc tính đã chọn, giá trị từ mức đã đặt trở lên.||"
 "6. [v] Giá trang bị > / [v] Cấp > / [v] Giữ nhẫn, dây chuyền, ngọc bội cấp >: ba mức lọc "
 "nhanh theo giá bán, theo cấp và theo loại trang sức.||"
 "7. [v] Xếp hành trang / [v] Xếp rương: tự dồn ô trống cho gọn.||"
 "LIÊN QUAN: bán rác và cất đồ nằm ở tab Hậu cần.")

NOTE[5] = ("HẬU CẦN — về thành, bán, mua, sửa, cất đồ||"
 "1. [v] Tự quay lại: bật chu trình đầy đủ — về thành, bán rác, mua bình, sửa đồ, rồi lên lại bãi.||"
 "2. [v] Bán rác + combo: bán những gì bộ lọc tab Nhặt đồ cho là rác.||"
 "3. [v] Bán ngựa mặt nạ: bán luôn ngựa và mặt nạ nhặt được.||"
 "4. [v] Sửa trang bị / [v] Sửa đồ trên bãi: sửa ở thành, hoặc dùng búa sửa ngay tại chỗ.||"
 "5. [v] Rút tiền + Mật khẩu rương: mật khẩu này DÙNG CHUNG cho cả tab Dã Tẩu và Liên đấu. "
 "Bỏ trống thì mọi việc cần mở rương đều không chạy.||"
 "6. [v] Mua sinh lực / nội lực / giải độc + combo loại + số lượng.||"
 "7. [v] Mua phù / [v] Giữ tiền / [v] Cất đồ + combo.||"
 "8. [v] Đi xa phu + combo / [v] Đi bản đồ + combo: cách quay lại bãi sau khi xong việc ở thành.||"
 "LIÊN QUAN: điều kiện nào thì về thành đặt ở tab Phục hồi (khối Thổ Địa Phù).")

NOTE[6] = ("TỔ ĐỘI — mời và vào nhóm||"
 "1. Combo chế độ MỜI: không mời / mời người trong danh sách / mời tất cả quanh mình.||"
 "2. Combo chế độ VÀO: không vào / chỉ vào nhóm của người trong danh sách / vào bất kỳ lời mời nào.||"
 "3. Combo tên quanh mình + [Thêm]: bắt tên người đang đứng gần rồi thêm vào danh sách.||"
 "4. Danh sách 24 tên + các nút Lên/Xuống/Xoá/Xoá hết/Đọc tệp/Ghi tệp.||"
 "5. [v] Rời nhóm khi thành viên < N người trong M phút: tự tan nhóm khi mọi người rớt hết.||"
 "6. [v] Bỏ thành viên vắng mặt > M phút: đá người treo máy ra khỏi nhóm.||"
 "LƯU Ý: trong Tống Kim máy chủ tự bỏ tổ đội lúc báo danh — đừng trông vào tổ đội trong trận đó.")

NOTE[7] = ("PK — đánh người theo phím nóng||"
 "1. [v] Bật chế độ đánh AutoPK: bật/tắt nhanh ngay trong game bằng phím tắt, không cần quay ra "
 "cửa sổ auto.||"
 "2. [v] Dùng phím ... + Tầm quét: chọn phím giữ để đánh và bán kính tìm địch.||"
 "3. [v] Tiếp cận / [v] Đuổi / [v] Vẽ vòng: chạy lại gần đúng số mps rồi đánh; địch chạy thì bám "
 "theo; vẽ vòng tầm quét lên màn hình cho dễ nhìn (chỉ để xem).||"
 "4. [v] Đánh người / [v] Đánh quái / [v] Xuống ngựa: lọc loại mục tiêu và tư thế đánh. "
 "Ô ''Xuống ngựa'' cũng là thứ chặn auto tự lên ngựa khi đi đường.||"
 "5. Ưu tiên: ''Khoảng cách'' đánh đứa gần nhất; ''Ngũ hành'' đánh theo hệ khắc, bấm [Thứ tự] "
 "để mở bảng rồi kéo Lên/Xuống.||"
 "6. Cast bùa #1 #2 #3: ba lá bùa tự dùng khi vào trận.||"
 "LƯU Ý: tab PK mượn lại các ô kỹ năng của tab Chiến đấu — sửa ở đâu cũng là một giá trị.||"
 "LIÊN QUAN: trong Tống Kim và Công Thành, máy sự kiện giao mục tiêu cho chính tầng PK này.")

NOTE[8] = ("DÃ TẨU — tự làm nhiệm vụ Dã Tẩu||"
 "1. [v] Bật auto Dã Tẩu: công tắc chính.||"
 "2. Sáu ô loại nhiệm vụ: Mua vật phẩm, Tìm vật phẩm, Tìm trang bị, Địa đồ - Mật chỉ, Nâng chỉ số, "
 "Sơn Hà Xã Tắc. Bỏ tick loại nào thì gặp loại đó auto sẽ xử theo ô ''Khi bỏ qua''.||"
 "3. Khi bỏ qua + Cách huỷ: treo máy chờ, hay huỷ nhiệm vụ để nhận cái khác.||"
 "4. Thưởng Exp-Bạc và Thưởng Điểm-Vật: chọn cửa thưởng mỗi lần hoàn thành.||"
 "5. [v] Lấy đồ/tiền từ rương: cần mật khẩu rương đặt ở tab Hậu cần.||"
 "6. [v] Thiếu đồ thì mua ở sạp + Giá sạp tối đa: tự đi 10 thành tìm sạp bán món còn thiếu.||"
 "7. [v] Xong nhiệm vụ tự lên map luyện công.||"
 "CẢNH BÁO: mốc 40 nhiệm vụ có phần thưởng riêng — đừng huỷ nhiệm vụ lung tung khi gần mốc.")

NOTE[9] = ("TỐNG KIM (TK) — tự báo danh và đánh trận||"
 "1. [v] Bật auto Tống Kim: tới giờ tự dừng việc đang làm, đi Chiêu Thu, báo danh, vào trận.||"
 "2. Bốn khung giờ: giờ khai trận theo đồng hồ MÁY CHỦ. Ô ''Lệch giờ'' là số phút chênh giữa "
 "máy chủ và máy này (âm được).||"
 "3. Đi sớm (phút): tới điểm báo danh trước giờ bao lâu.||"
 "4. Phe: tự theo phe hay ép một phe.||"
 "5. Mua máu + Số bình: mua thuốc ở hậu doanh trước khi ra trận, mỗi mạng mua lại.||"
 "6. [v] Tự ăn thuốc Tống Kim + Loại thuốc: ăn thuốc riêng của trận.||"
 "7. Ưu tiên đánh: ''Địch gần nhất'' hay ''Hiệu Úy / Phó Tướng / Đại Tướng''.||"
 "8. Hết trận về + combo 7 thành.||"
 "CÁCH AUTO TÌM ĐỊCH (bản 04/09): tướng (nếu chọn ưu tiên) → người khác màu trong tầm PK → "
 "săn người khác màu ở xa → lính NPC → vị trí địch do máy chủ báo → rảo khắp bản đồ theo lưới. "
 "Không còn chạy theo bảng toạ độ cố định như trước.||"
 "LIÊN QUAN: ac phụ bám theo ac chính trong trận thì bật ở tab Ác chính.")

NOTE[10] = ("LIÊN ĐẤU — tự đăng ký và đánh giải||"
 "1. [v] Bật auto Liên đấu: tới giờ tự tới NPC đăng ký.||"
 "2. Hai khung giờ, mỗi khung có giờ và phút riêng.||"
 "3. Số lượt và số phút mỗi lượt: tự bỏ cuộc khi quá giờ để khỏi kẹt.||"
 "4. Đi sớm (phút): tới trước giờ bao lâu.||"
 "5. Tên đăng ký: tối đa 16 ký tự.||"
 "6. Combo phủ: Tương Dương, Dương Châu hay Lâm An — nơi có NPC đăng ký.||"
 "LƯU Ý: trong đấu trường auto đánh theo cấu hình tab PK, không theo tab Chiến đấu.")

NOTE[11] = ("HOẠT ĐỘNG — Bạch Nhân, Bang Chiến, Tín Sứ||"
 "1. Mỗi hoạt động có một ô bật riêng; bật cái nào thì tới giờ auto tự đi làm cái đó.||"
 "2. Lôi Chủ Bạch Nhân: auto nhận mục tiêu do máy chủ chỉ định và bám theo đúng ô ''Đuổi theo "
 "mục tiêu'' của tab PK.||"
 "3. Bang Chiến: đánh theo phe bang hội, dùng cấu hình tab PK.||"
 "4. Tín Sứ: tự nhận và trả nhiệm vụ Tín Sứ; phần thưởng nay trả qua thư.||"
 "CẢNH BÁO: ba hoạt động này và Tống Kim, Công Thành loại trừ lẫn nhau — cùng giờ thì máy nào "
 "cầm lái trước sẽ giữ, cái còn lại nhường.")

NOTE[12] = ("SÁT THỦ — săn boss theo Sát Thủ Giản||"
 "1. [v] Bật săn boss Sát thủ: tự nhận Sát Thủ Giản, đi tới boss, đánh và nhận thưởng.||"
 "2. Các ô đi kèm chọn loại boss, cách di chuyển và cách xử lý khi boss đã bị người khác giết.||"
 "LƯU Ý: đây là tab NGẮN nhất, cửa sổ auto sẽ thu lại khi mở tab này — không phải lỗi hiển thị.||"
 "LIÊN QUAN: mục tiêu boss được giao thẳng cho tầng PK, nên tầm đánh và chiêu lấy ở tab PK.")

NOTE[13] = ("CHIÊU KH — bảng chiêu kết hợp và tiên chiêu||"
 "1. [v] Bật chiêu KH: công tắc chính của cả tab.||"
 "2. BẢNG 6 KHE: sáu chiêu bắn tuần tự, mỗi khe có ô ''trễ (ms)'' riêng. Auto bắn khe 1, chờ trễ, "
 "sang khe 2... hết vòng quay lại.||"
 "3. TIÊN CHIÊU: một chiêu bắn theo kiểu riêng — mỗi mục tiêu mới một lần, theo chu kỳ giây, "
 "hay khi còn xa. Kèm điều kiện tăng Nộ / Âm Luật nếu chọn.||"
 "4. Chèn Nộ: cho phép chen chiêu cần Nộ vào giữa vòng khi đủ tầng.||"
 "CẢNH BÁO: chiêu KHÔNG dùng được với vũ khí đang cầm sẽ bị client từ chối. Từ bản 04/09 auto "
 "hỏi trước rồi cấm chiêu đó 30 giây và sang khe kế, log ghi [CHIEU-CAM] — nếu thấy dòng đó thì "
 "bỏ chiêu ấy khỏi bảng hoặc đổi vũ khí.||"
 "LIÊN QUAN: chiêu lướt bị giựt lùi là do khoảng cách lướt lớn hơn 256 mps.")

NOTE[14] = ("CÔNG THÀNH (CTC) — tự tham chiến đúng giờ||"
 "1. [v] Bật auto Công Thành Chiến: tới giờ khai chiến tự dừng việc đang làm, ĐI BỘ tới Xa Phu "
 "của thành, chọn Đi Chiến trường công thành, chọn phe theo bang, sang hậu phương, đạp cửa tập "
 "kết, vào doanh rồi ra trận. Không dùng phù, không nhảy map.||"
 "2. Khai chiến + Lệch giờ + Đi sớm + Cửa sổ (phút): giờ theo đồng hồ máy chủ; cửa sổ là thời "
 "gian còn được phép vào trận sau mốc giờ.||"
 "3. Thành báo danh: Xa Phu của thành nào cũng có mục đi chiến trường; chọn ''Thành đang đứng'' "
 "là dùng Xa Phu ngay tại chỗ.||"
 "4. Phe: tự theo bang hội, hoặc ép bên công / bên thủ khi có lệnh bài.||"
 "5. Bên công / Bên thủ: phá cổng rồi hạ Long trụ, canh trụ và chiếm lại, hay chỉ đánh người.||"
 "6. [v] Nghe loa khai chiến của máy chủ là đi ngay: không cần đúng khung giờ.||"
 "7. [v] Mua Ngũ Hoa Ngọc Lộ ở Tuỳ Quân dược Y + Số bình + Cấp bình: mua trong doanh trước khi "
 "ra trận, mỗi mạng mua lại.||"
 "8. Hết trận về + combo 7 thành.||"
 "YÊU CẦU: nhân vật phải ở trong bang tham chiến ít nhất 5 ngày mới đạp được cửa tập kết.")

NOTE[15] = ("ÁC CHÍNH — ac phụ đi theo và đánh cùng mục tiêu||"
 "1. Chọn ác chính: dùng combo ở HÀNG THỨ HAI trên đầu cửa sổ, hoặc bấm chuột phải vào tên nhân "
 "vật trong danh sách rồi chọn ''Đặt làm ác chính cho tất cả cửa sổ khác''.||"
 "2. [v] Tìm ác chính + số mps (hàng thứ ba trên đầu cửa sổ): ac phụ tự chạy theo ác chính khi "
 "cách xa hơn số đó, dừng lại cách chừng 150 mps. Ác chính chết hoặc mất tin quá 5 giây thì thôi theo.||"
 "3. [v] Trong thành: ác chính đứng trong thành thì ac phụ vẫn theo. Tắt để tránh cả bầy chạy "
 "theo khi ác chính về thành.||"
 "4. [v] Đánh cùng mục tiêu ác chính (ô trong tab này): ac phụ đánh đúng con hay người mà ác "
 "chính đang đánh, khi nhìn thấy được. Không thấy thì đi theo ác chính trước.||"
 "5. Dòng trạng thái dưới cùng của tab cho biết ác chính đang ở map nào, cách bao xa, mục tiêu "
 "số mấy, còn sống hay đã chết.||"
 "LƯU Ý: cửa sổ ĐANG LÀ ác chính thì không bị điều khiển, auto của nó chạy như thường.||"
 "TRONG TỐNG KIM: bật ''Tìm ác chính'' thì ac phụ bám ác chính thay cho việc tự rảo bản đồ.")

NOTE_CHUNG = ("WAuto — hướng dẫn chung||"
 "1. Danh sách trên cùng là các cửa sổ game đang mở. Ô TÍCH đầu dòng = BẬT auto cho nhân vật đó; "
 "bỏ tích là auto ngừng cầm lái ngay.||"
 "2. Năm cột: Nhân vật | SL (sinh lực) | NL (nội lực) | Việc (auto đang làm gì) | Bản đồ.||"
 "3. Bốn nút lớn là NHÓM; hàng nút nhỏ bên dưới là TAB trong nhóm đó. Mọi thiết lập đều theo "
 "TỪNG NHÂN VẬT và được lưu ngay khi đổi, không cần bấm Lưu.||"
 "4. Rê chuột vào bất kỳ ô nào để đọc ghi chú nhanh; bấm [Trợ giúp] để đọc hướng dẫn dài của "
 "tab đang mở.||"
 "5. Cấu hình lưu ở thư mục APdata theo mã tiến trình của từng nhân vật.||"
 "6. Muốn chép thiết lập từ nhân vật này sang nhân vật khác: tab Cơ bản, mục ''Cài đặt theo'' "
 "rồi bấm [Đồng bộ].")

NOTE_VUNGTREN = ("VÙNG TRÊN — ba hàng trên đầu cửa sổ||"
 "HÀNG 1: dòng chữ trạng thái, nút [A] quét lại danh sách cửa sổ game, nút [H] mở hướng dẫn "
 "chung, nút [Trợ giúp] mở hướng dẫn của tab đang mở.||"
 "HÀNG 2: combo chế độ nhanh (Thường / PK / TK / CTC / Liên đấu / Dã Tẩu) — chọn một mục là vừa "
 "nhảy sang tab đó vừa bật công tắc chính của nó; nút [Ác chính] đặt nhân vật đang chọn làm ác "
 "chính cho mọi cửa sổ khác; combo bên cạnh là ác chính của riêng cửa sổ đang chọn; nút [X] xoá "
 "ác chính.||"
 "HÀNG 3: [v] Tìm ác chính + khoảng cách mps + [v] Trong thành — giống hệt auto Thái, chỉnh "
 "được ở mọi tab chứ không phải mở riêng tab Ác chính.||"
 "LƯU Ý: ba hàng này luôn hiện ở mọi tab.")


def c_str(s):
    """Doi chuoi Python -> chuoi C++ (WAuto.cpp la UTF-16 nen giu nguyen tieng Viet)."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def khoi_note():
    d = ['// ===================== (04/09) NOTE HUONG DAN KIEU AUTO THAI =====================',
         '// Bat chuoc FormTip cua auto Thai: bam nut -> hop chu khong modal, dan canh cua so chinh,',
         "// dau '|' trong chuoi = xuong dong, tu dong dem nguoc roi dong. Tooltip re chuot GIU NGUYEN.",
         'static const wchar_t* const s_aNote[WA_SO_TAB] = {']
    for i in range(16):
        d.append('\tL"%s",' % c_str(NOTE[i]))
    d.append('};')
    d.append('static const wchar_t* const s_szNoteChung   = L"%s";' % c_str(NOTE_CHUNG))
    d.append('static const wchar_t* const s_szNoteVungTren = L"%s";' % c_str(NOTE_VUNGTREN))
    d.append("""
static HWND     s_hNote = NULL;		// chi MOT hop note song mot luc (y nhu Thai)
static int      s_nNoteConLai = 0;
static wchar_t  s_szNoteTen[64] = L"";

static INT_PTR CALLBACK HelpDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_TIMER:
		if (wParam == 1)
		{
			--s_nNoteConLai;
			if (s_nNoteConLai <= 0)
			{
				DestroyWindow(hDlg);
				return TRUE;
			}
			wchar_t wb[128];
			GVWPRINT(wb, L"HƯỚNG DẪN — %s (%ds)", s_szNoteTen, s_nNoteConLai);
			SetWindowTextW(hDlg, wb);
		}
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDOK)
		{
			DestroyWindow(hDlg);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		return TRUE;
	case WM_DESTROY:
		KillTimer(hDlg, 1);
		if (s_hNote == hDlg)
			s_hNote = NULL;
		return TRUE;
	}
	return FALSE;
}

// Mo hop note. wszTen = ten tab (hien tren tieu de), wszNoiDung dung '|' de xuong dong.
static void WA_MoNote(HWND hDlg, const wchar_t* wszTen, const wchar_t* wszNoiDung, int nGiay)
{
	if (s_hNote)
	{
		DestroyWindow(s_hNote);
		s_hNote = NULL;
	}
	if (!wszNoiDung || !wszNoiDung[0])
		wszNoiDung = L"(chưa có hướng dẫn cho mục này)";
	s_hNote = CreateDialogParamW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_HELP_DIALOG), hDlg, HelpDlgProc, 0);
	if (!s_hNote)
		return;
	// '|' -> xuong dong (khuon cua Thai)
	size_t nLen = wcslen(wszNoiDung);
	wchar_t* pBuf = new wchar_t[nLen * 2 + 2];
	size_t k = 0;
	for (size_t i = 0; i < nLen; ++i)
	{
		if (wszNoiDung[i] == L'|')
		{
			pBuf[k++] = L'\\r';
			pBuf[k++] = L'\\n';
		}
		else
			pBuf[k++] = wszNoiDung[i];
	}
	pBuf[k] = 0;
	SetDlgItemTextW(s_hNote, IDC_HELP_TEXT, pBuf);
	delete [] pBuf;
	lstrcpynW(s_szNoteTen, wszTen ? wszTen : L"", 64);
	s_nNoteConLai = (nGiay > 0) ? nGiay : 600;
	wchar_t wb[128];
	GVWPRINT(wb, L"HƯỚNG DẪN — %s (%ds)", s_szNoteTen, s_nNoteConLai);
	SetWindowTextW(s_hNote, wb);
	// Dat CANH cua so chinh (khong che mat cac o dang chinh); het cho ben phai thi dan mep trai
	RECT rcMain{}, rcNote{};
	GetWindowRect(hDlg, &rcMain);
	GetWindowRect(s_hNote, &rcNote);
	int nRong = rcNote.right - rcNote.left;
	int x = rcMain.right + 4;
	if (x + nRong > GetSystemMetrics(SM_CXSCREEN))
		x = (rcMain.left > nRong + 4) ? (rcMain.left - nRong - 4) : rcMain.left;
	SetWindowPos(s_hNote, HWND_TOPMOST, x, rcMain.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	SetTimer(s_hNote, 1, 1000, NULL);
	ShowWindow(s_hNote, SW_SHOW);
}

// Mo note cua TAB DANG MO
static void WA_MoNoteTab(HWND hDlg)
{
	int n = m_nTabIndex;
	if (n < 0 || n >= WA_SO_TAB)
		n = 0;
	WA_MoNote(hDlg, s_aTenTab[n], s_aNote[n], 600);
}
// ===================== HET NOTE HUONG DAN =====================
""")
    return "\n".join(d)


RC_HELP = """
IDD_HELP_DIALOG DIALOGEX 0, 0, 210, 180
STYLE DS_SETFONT | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "HƯỚNG DẪN"
FONT 9, "Segoe UI", 400, 0, 0xa3
BEGIN
\tEDITTEXT IDC_HELP_TEXT, 3, 3, 204, 174, ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL | NOT WS_TABSTOP
END
"""


def main():
    print("== goi_va_wauto_ui_thai_0409_note %s ==" % ("(THU - khong ghi)" if THU else ""))
    # 1. Resource.h
    r = Tep(os.path.join(WA_E, "Resource.h"), "latin-1")
    r.them_sau_dong("#define IDM_ACCHINH_ALL",
                    "// (04/09) hop note huong dan kieu auto Thai (ngoai dai an/hien cua ShowTab)\n"
                    "#define IDD_HELP_DIALOG\t\t750\n"
                    "#define IDC_HELP_TEXT\t\t751",
                    "IDD_HELP_DIALOG")
    r.ghi()

    # 2. WAuto.rc - them mau hop thoai note ngay truoc IDD_NOPICK_DIALOG
    rc = Tep(os.path.join(WA_E, "WAuto.rc"))
    rc.them_truoc_dong("IDD_NOPICK_DIALOG DIALOGEX", RC_HELP, "IDD_HELP_DIALOG DIALOGEX")
    # moc tam: bam chu "Vo Lam Ngao The" o day = mo note cua tab dang mo
    rc.thay('CTEXT "Võ Lâm Ngạo Thế",IDC_STRING_HOMEPAGE,5,346,150,10,SS_CENTERIMAGE',
            'CTEXT "Võ Lâm Ngạo Thế",IDC_STRING_HOMEPAGE,5,346,150,10,SS_CENTERIMAGE | SS_NOTIFY',
            "IDC_STRING_HOMEPAGE,5,346,150,10,SS_CENTERIMAGE | SS_NOTIFY")
    rc.ghi()

    # 3. WAuto.cpp
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    t.them_truoc_dong("static void CreateAllTips(HWND hDlg)", khoi_note(), "static void WA_MoNoteTab(")
    t.them_truoc_dong("\t\t\tcase IDC_BTN_4_NOP:",
                      "\t\t\tcase IDC_STRING_HOMEPAGE:\n"
                      "\t\t\t{\t// (04/09) bam dong chu duoi cung = mo note huong dan cua tab dang mo\n"
                      "\t\t\t\tif (HIWORD(wParam) == STN_CLICKED)\n"
                      "\t\t\t\t\tWA_MoNoteTab(hDlg);\n"
                      "\t\t\t\tbreak;\n"
                      "\t\t\t}",
                      "case IDC_STRING_HOMEPAGE:")
    doi = t.ghi()
    if LOI:
        print("LOI:")
        for l in LOI: print("  " + l)
        sys.exit(1)
    if not THU:
        for f in ("Resource.h", "WAuto.rc", "WAuto.cpp"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
