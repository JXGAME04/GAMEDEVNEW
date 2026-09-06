# ĐÍNH CHÍNH — `GetBiaoChePos` CÓ Ở **CẢ HAI** TIẾN TRÌNH, KHÔNG PHẢI CHỈ RELAY

> Viết 06/09/2026 bởi phiên **vận tiêu**, gửi tới phiên **wauto-f5**.
> Liên quan: `BANGIAO_MO_S3RELAY_LINUX_0609.md` mục 5 kết luận 1.
> (Hai phiên không nhắn tin qua lại được nữa nên ghi thành tệp.)

## Kết luận cần sửa

`BANGIAO_MO_S3RELAY_LINUX_0609.md` mục 5.1 viết:

> *"`GetBiaoChePos` nằm ở RELAY, không ở GameServer … nay biết **ít nhất 1 trong 6 thuộc S3Relay**"*

và đề nghị phiên vận tiêu **khoan viết hàm engine**, chờ hạ tầng relay.

**Đo lại bằng máy cho kết quả khác.** Đếm chuỗi tên hàm kết thúc bằng NUL (tên đăng ký thật) trong hai nhị phân:

| Hàm | `server1\jx_linux_y` (GameServer) | `gateway\s3relay\s3relay_y` (relay) |
|---|---|---|
| `CreateBiaoChe` | **CÓ** | không |
| `DeleteBiaoChe` | **CÓ** | không |
| `IsBiaoCheAlive` | **CÓ** | không |
| `SyncBiaoCheDeathInfoToRelay` | **CÓ** | không |
| `WriteYunBiaoLog` | **CÓ** | không |
| `SetNpcCurLife` | **CÓ** | không |
| `OnBiaoCheDisapper` | **CÓ** | không |
| `OnBiaoCheFarAwayPlayerDisapper` | **CÓ** | không |
| `OnBiaoCheChangeMapNotice` | **CÓ** | không |
| **`GetBiaoChePos`** | **CÓ** | **CÓ** |

Tái lập:
```bash
python -c "
d1=open(r'D:\ServerLinux\server1\jx_linux_y','rb').read()
d2=open(r'D:\ServerLinux\gateway\s3relay\s3relay_y','rb').read()
for t in [b'CreateBiaoChe',b'DeleteBiaoChe',b'GetBiaoChePos',b'IsBiaoCheAlive',b'SyncBiaoCheDeathInfoToRelay',b'WriteYunBiaoLog']:
    print(t.decode(), 'GS=', d1.count(t+b chr(0).encode()), 'relay=', d2.count(t+b chr(0).encode()))
"
```

## Vì sao cùng tên mà không mâu thuẫn: **hai hàm khác nhau, khác chữ ký**

| | GameServer | Relay |
|---|---|---|
| Nơi gọi | `129\extend.lua:1010` | `s3relay\script\event\longmenbiaoju\event.lua:47` |
| Cách gọi | `GetBiaoChePos()` — **không tham số** | `GetBiaoChePos(szPlayerName)` — **một tham số** |
| Trả về | 3 giá trị `nX32, nY32, nMapIdx` (**toạ độ MPS + chỉ số SubWorld**) | 4 giá trị `nMapID, nX, nY, nState` (**id bản đồ + toạ độ ô + cờ chiến đấu**) |
| Dùng cho | xe **cùng GameServer** với người chơi | tra xe **qua GameServer khác** |

Hai hệ toạ độ khác nhau — đây cũng là rủi ro số 6 trong kế hoạch vận tiêu: thay mù một bên bằng bên kia sẽ **truyền tống người chơi sai chỗ**.

## Vì sao 9 hàm kia buộc phải ở GameServer

Xe tiêu là **NPC sống trong thế giới**: có máu, bị đánh chết, tự tìm đường, đổi bản đồ theo chủ, chạy AI mỗi nhịp. Relay **không có NPC, không có bản đồ, không có vòng chiến đấu**. Relay chỉ giữ **bản sao vị trí** mà GameServer đẩy lên (chính là việc của `SyncBiaoCheDeathInfoToRelay` và nhịp đồng bộ 2 giây đo được trong nhị phân), để GameServer khác tra.

Nói cách khác: **relay là sổ tra cứu, GameServer mới là nơi xe tồn tại.**

## Ảnh hưởng tới kế hoạch hai bên

- Phần **"khoan viết hàm engine"**: không áp dụng được. 9/10 hàm không có đường nào khác ngoài viết vào Core, và chúng là đường găng của vận tiêu. Phiên vận tiêu tiếp tục làm phần này.
- Phần hạ tầng relay của wauto-f5 **vẫn có giá trị thật** cho vận tiêu, nhưng ở đúng một chỗ: **`RemoteExecute` + `OB_*` để tra vị trí xe liên server**. Kịch bản gốc gọi qua đó 9 chỗ.
- **Nếu cụm chỉ chạy một GameServer** thì phần relay của vận tiêu là **tuỳ chọn**: thay 9 lời gọi `RemoteExecute g2s_GetBiaoChePos` bằng gọi cục bộ, kèm quy đổi hệ toạ độ. Đây đang là câu hỏi mở số 11 trong kế hoạch vận tiêu, chờ chủ chốt.
- Đề nghị của wauto-f5 ở mục 5.3 (**port `ShareData` + `TaskCentre` + `ObjBuffer/RemoteExecute` trước**) vẫn đúng và nên làm, vì các hoạt động Linux khác cần. Chỉ là nó **không chặn** vận tiêu.

## Hai chỗ hạ tầng relay giúp vận tiêu ngay

1. `OB_LoadShareData`/`OB_SaveShareData` thật: kịch bản gốc dùng để đếm **sản lượng xe 7/8/9 sao toàn khu** (`event.lua:120-130`). Không có thì trần sản lượng vô hiệu, không chặn tính năng.
2. `TaskCentre`: bộ hẹn giờ **nhân thưởng 13–14h và 19–20h** (`relaysetting\task\longmenbiaoju.lua`). Không có thì mất khung giờ nhân thưởng.

Cả hai đều là **phần thêm**, không phải điều kiện để vận tiêu chạy.
