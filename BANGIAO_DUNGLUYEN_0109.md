# BÀN GIAO — DUNG LUYỆN VĂN CƯƠNG + HỘ THÂN [HOTHAN2] — 01/09/2026 (chiều/tối)

Chủ game: "hãy làm dung luyện trước … làm giống 100% bản nguồn Linux - box dung luyện" → "hãy làm tiếp toàn bộ cho tôi - yêu cầu phải phản biện".
Commit đợt 1: `2815d2d3` (D:\GAMEDEVNEW). Đặc tả gốc: `scratchpad\dacta_dungluyen.md` (6 tác tử + chốt).

## 1. Kiến trúc (tệp : thay đổi)
| Lớp | Tệp | Việc |
|---|---|---|
| Vật phẩm | `Core\Src\KItem.h/.cpp` | `m_nFusionP[6]`, `m_uFusionSeed[6]`; `FUS_GetInfo` (cache fusion.txt + magicattrib_ge), `FUS_GenAttrib` (LCG), `FUS_ModifyAttrib` (móc sau `PF_ModifyStoneAttrib` ở Apply/Remove), `FUS_AppendDesc` (tooltip, chèn trước CẢ 2 `PF_AppendDesc`), `FUS_BuildInfo` (bảng box), `GetFusionCap/Qual` (cột 58/59, 66/67), `operator=(KBASICPROP_FUSION&)`, tooltip viên genre 8 (overload 1 cạnh khối starstone) |
| Bảng | `KBasPropTbl.h/.CPP` | `KBASICPROP_FUSION` 22 cột, `KBPT_Fusion` ("fusion.txt" tên trần), `m_BPTFusion` CUỐI `paryBPT`, `GetFusionRecord(P)` |
| Sinh | `KItemGenerator.h/.CPP`, `KItemSet.cpp` | `Gen_Fusion` (seed ≠ 0, server `g_GetRandomSeed`), `FUS_TraRowInfo`, case `item_fusion` |
| DB | `KPlayerDBFuns.cpp` | Save: 9 trường đóng gói (ghi tường minh cả khi 0), Load: giải nén khi `iidentify=='FUS1'`, chỉ nhận P có trong fusion.txt và món vàng/bạch kim hoặc viên genre 8 |
| Giao thức | `Headers\KMantleProtocol.h`, `KProtocolDef.h`, `KProtocol.cpp`, `KProtocolProcess.h/.cpp`, `KItemList.cpp` | `ITEM_SYNC_FUSION` 41 B, `s2c_syncfusion`, gửi sau PFPACK trong `SyncItem`, client `s2cSyncItemFusion` (+`UpdataCurData` nếu đang mặc) |
| Lõi | `KItemList.h/.cpp` | `SmeltEquip(nEqu,nFusion,bCheck)`, `UnSmeltEquip(nEqu,bCheck)`, `FUS_DangMac` |
| Lua API | `KJx2WarInfra.cpp`, `ScriptFuns.cpp` | 10 hàm (tên Linux) + `OpenSmeltBox` nType=5 |
| Box | `KProtocolProcess.cpp` (case 5 + whitelist), `CoreShell.h/.cpp` (`GDCNI_OPEN_SMELT_BOX`, `GDI_FUSION_INFO`), `S3Client\Ui\UiCase\UiSmelt.h/.cpp` (MỚI), `GameSpaceChangedNotify.cpp`, `S3Client.vcxproj` | KUiSmelt : KWndPageSet, 2 KUiSmeltPage |
| Hộ thân | `KNpc.h/.cpp` | `CalcDamage(..., int nTotalAvg=0)`; khiên điểm; manashield_p; ReceiveDamage tính nTotalAvg |

## 2. Cây chạy (E:\…\TESTLOFFF_ONLINE, không git)
- Server+client `settings\item\fusion.txt` (MỚI, 351 dòng), `smeltupgradetable.txt` (MỚI), `magicscript.txt` (sửa 352 dòng cột Script + 101 tên/intro; backup `.truoc_dungluyen_0109`).
- Server `script\global\smelt_system.lua`, `smeltsystem\{smeltupgrade,lang}.lua`, `item\item_jingtiekuang.lua`, `item_jingjingkuang.lua`, `npcchucnang\thoren.lua` (backup `.truoc_dungluyen_0109`).
- Client `Ui\Ui3\smeltsystem\{smeltsystem,smelt,distill}.ini`, `spr\vng\ui\smelt\{bg_main,page_bg,close,tab,scroll_btn,btn4,effect}.spr`.

## 3. Luồng chơi
Thợ rèn → "Trang bị dung luyện" → Dung luyện Văn Cương (mở box) / Nâng cấp Văn Tinh / Phân giải Văn Tinh / Hướng dẫn. Box thẻ 1: trang bị vàng/bạch kim + Văn Cương → "Dung luyện" (100 vạn) → `doSmeltBox` → `DoSmeltEquip` → `SmeltEquip` C → SyncItem → Msg2Player mã kết quả → mở lại box. Thẻ 2: trang bị + Huyền Hỏa Than (4125) → "Tinh chế" → `doUnSmeltBox` → xoá 6 ô, AddItem(8,1,P,…) + SetFusionMagicSeed(seed cũ) + SetFusionIsSmelted. Văn Tinh chuột phải → `smeltupgrade.lua main()` nâng Văn Cương cùng loại/cấp. Nguồn Văn Cương/Văn Tinh: Tinh Thiết Khoáng (4428) / Tinh Tinh Khoáng (4429, cần Huyền Hỏa Than).

## 4. Quyết định thay chủ (chủ có thể đổi)
- 147 dòng goldequip JX1 mới để trống cột 58/59 → không dung luyện được (Linux không có dòng đó).
- Không port thưởng 20.000.000 exp "Nộp trang bị đã dung luyện" (task 3998 JX1 đang dùng cho event câu hỏi).
- Thuộc tính Văn Cương áp THÔ (không nhân hệ số ngựa) như Linux.
- Chưa có nguồn cấp Tinh Thiết Khoáng / Tinh Tinh Khoáng / Huyền Hỏa Than cho người chơi (Linux: boss/sự kiện) — GM tạo bằng AddItem để thử; chủ quyết đưa vào shop xu hay drop.
- Bùa chú (ignoreskill_p / returnskill_p / attribconstdata.ini) khác Linux — chưa động (xem memory kiểm toán).

## 5. Kiểm thử tổng (sau swap)
(a) Đăng nhập nhân vật nhiều đồ nhất → túi nguyên. (b) `AddItem(8,1,1,1,0,0,0)` → viên có tooltip (thuộc tính theo seed, phẩm chất). (c) Đặt trang bị vàng cap>0 + viên → Dung luyện → tooltip trang bị có dòng Văn Cương; mặc/tháo chỉ số đổi; logout/login giữ (dem_truong_trong.py: `iidentify` 'FUS1' đúng món). (d) Thí luyện → 6 viên về túi có "Đã qua thí luyện" + cùng giá trị. (e) Người khác xem đồ thấy dòng Văn Cương. (f) Đủ 8 mã lỗi. (g) Quay lui CoreServer cũ: túi vẫn đọc được.


## 6. PHẢN BIỆN 02/09 (phiên wauto-e7, tiếp phiên hết hạn mức) — 14 lỗi xác nhận, đã vá, build sạch, bộ `.moi` mới
Cách làm: workflow 8 lăng kính (DB · sinh/áp thuộc tính · giao thức · lõi+Lua API+give-box · client UI · Lua · hộ thân · dữ liệu/mã hoá) → mỗi phát hiện 2 tác tử xác minh (bác bỏ + tái hiện) → soát thiếu → vòng 2. 55/65 tác tử xong, 10 cuối bị giới hạn phiên nên vòng 2 tự xác minh bằng tay trên mã. Nhãn mã: `[DUNGLUYEN-PB 01/09]`. Script vá idempotent: scratchpad `p1_va_cpp.py` (12 mục, latin-1) + `p2_va_lua_data.py`.

### 6.1 Lỗi → cách vá (tệp)
| # | Mức | Lỗi | Vá |
|---|---|---|---|
| 1 | critical | `LoadPlayerItemList` (KPlayerDBFuns.cpp:635-701) KHÔNG có `case item_fusion` (8) lẫn `item_starstone` (9, có sẵn từ 29/08): NewItem genre 0, kích thước 0 → AddKIL thất bại im lặng → **viên Văn Cương / Tinh Thần Thạch trong túi hoặc rương MẤT sau relogin**, rò slot ItemSet | thêm 2 case gọi `Gen_Fusion` / `Gen_StarStone` (đúng khuôn AddItemSet2); khối FUS1 phía dưới ghi đè seed thật |
| 2 | critical | Tinh Thiết Khoáng 4428 / Tinh Tinh Khoáng 4429 **dùng vô hạn**: script Linux dựa quy ước "main trả ≠1 → engine trừ 1", JX1 chỉ mô phỏng cho danh sách `szJx2[]` | thêm 2 script vào `sIsJx2ItemScript` (KItemList.cpp) — giữ script y Linux |
| 3 | high | Ô nguyên liệu `Region.h=1` đè cột 1 của trang bị rộng 2 ô (giáp 2x3, nón/giày 2x2, đai 2x1, đa số vũ khí) → `PlaceItem` từ chối, **không bao giờ đủ 2 món** trong khay | nguyên liệu = **cột 2** (UiSmelt.cpp pick/drop + UpdateItem; `smelt_system.lua` FUS_LayDoTrongKhay `nSlot == 2`) |
| 4 | high | `GDI_FUSION_INFO` (và tiền lệ `GDI_MANTLE_HIDDEN_DESC` của PF13) tra `ItemSet.SearchID(dwID)` trong khi `Obj.uId` của khay = CHỈ SỐ item (CoreShell.cpp:1496) → bảng thông tin trong box luôn rỗng; box tẩy luyện 2 cột dòng ẩn rỗng | `nIdx = (int)uParam` ở CoreShell.cpp 842/867 |
| 5 | high | `GetItemBillType` đọc `uPrice` — server KHÔNG bao giờ xoá uPrice (`SetPrice(0)` chỉ xoá `m_Items.nPrice`, DB lưu `iiduphong9`) → mọi trang bị **từng bày bán hoặc mua từ sạp (kể cả sạp bot)** bị chặn dung luyện vĩnh viễn (mã 14) | `GetItemBillType` dùng `m_ItemList.GetPrice` (giá sạp thật). GỐC tiền tồn: `SetPrice(0)` xoá uPrice; `c2sTradeBuy` bản sao người mua `uPrice = 0` (trước: món gỡ khỏi sạp **tự bày lại sau relogin**, món mua **tự lên sạp người mua** giá cũ) |
| 6 | medium | Nút **Hủy** giữ box mở nhưng server `RecoveryBoxCmd` gán `m_dwGiveBoxId = 0` → bấm Dung luyện/Tinh chế sau đó bị nuốt im lặng | Hủy = `CloseWindow(true)` như UiAffairItem/UiMantleInlay/UiMantleWash |
| 7 | medium | Gỡ dung luyện: `ClearFusion` nhưng `SyncItem` chỉ gửi `s2c_syncfusion` khi `GetFusionNum() > 0` → client giữ **Văn Cương ma** (tooltip + chỉ số) tới relogin | gửi cho mọi trang bị HK/BK có `GetFusionCap() > 0` (41 B, kể cả 6 ô = 0) |
| 8 | medium | 350 Intro Văn Tinh dài 258–288 byte > trần 255 (`m_szIntro[256]`) → tooltip mất số / thẻ đóng `<color><bclr>` | dữ liệu: rút câu mở đầu → tối đa 229 byte, `magicscript.txt` server = client, backup `.truoc_pb_0109` |
| 9 | low | `smeltupgrade.lua` đếm `CalcItemCount(3,…)` chỉ hành trang chính, `ConsumeItem(3,…)` trừ cả túi mở rộng → từ chối sai | `CalcEquiproomItemCount(8,1,P,-1)` |
| 10 | low | Xem trang bị người khác (`s2c_viewequip`) / xem sạp không mang Văn Cương | `SendEquipItemInfo` / `SendSellItemInfo` gửi kèm `ITEM_SYNC_FUSION` cho món có dữ liệu (item tạm client có `SetID` → `SearchID` tìm được) |
| 11 | low | `manashield_p` bỏ kẹp 100 nhưng không kẹp cut ≤ dmg → cộng dồn > 100 cho sát thương ÂM = hồi máu (dữ liệu hiện max 100) | `if (nManaDamage > nDamage) nManaDamage = nDamage;` |
| 12 | low | Seed viên từ `g_GetRandomSeed()` (chỉ đổi khi có `g_Random`) → 2 viên tạo liên tiếp có thể trùng seed | `GetRandomNumber(1, 0x7FFFFFF0)` như đường dự phòng SmeltEquip |
| 13 | low | `Get_SizeItem` thiếu case 8/9 → `CheckFreeBoxItem(8|9,…)` luôn 0 | thêm 2 case (fusion.txt / starstone.txt cột 7-8) |
| 14 | low | `Gen_Fusion` nhận nStackNum thô dù nMaxStack = 1 → N lần dung luyện/1 seed; trang bị HK sinh từ `GoldItem.txt` (`nGoldId ≠ 0`) không có nRow → `GetFusionCap` đọc nhầm dòng 0 goldequip | `SetStackNum(1)`; `sFUS_LaTrangBiVang` thêm `nGoldId == 0` |

### 6.2 Phát hiện bị BÁC BỎ (không sửa, đừng đi lại)
- `IsMyItem(nItemIndex)` 1 tham số vẫn trả 1/0: `GetPlayerIndex` đẩy global lên stack nên `Lua_GetTopIndex > 1` luôn đúng.
- Nhịp độc/đốt (`bReturn`) đi qua `manashield_p` = ĐÚNG Linux (hàm tick trạng thái 0x0808B610 → 0x0808BDF9 → BeHurt → 0x08089EC3).
- Khiên điểm `dynamicmagicshield_v` = giảm PHẲNG V điểm/đòn chia theo tỉ lệ 4 hệ, KHÔNG phải bể trừ dần (đúng disasm 0x0808A070). Côn Lôn 550 = miễn 550 sát thương/đòn trước kháng — **cân bằng chờ chủ**.
- Tiền lệ `GDI_MANTLE_HIDDEN_DESC` (PF13) cùng lỗi #4 → đã vá chung.

### 6.3 Chưa sửa — chờ chủ quyết / ngoài phạm vi
1. **Lò rèn / KItemCompound** (`LuaAddItemEx` tạo item MỚI qua `Gen_ExistEquipment`) không chép `m_nFusionP/m_uFusionSeed` (lẫn `m_nPfPack` phi phong) → tái tạo trang bị đã dung luyện = **Văn Cương huỷ im lặng, không hoàn** (tiền tồn với phi phong).
2. Đồ để trong khay give-box lúc đăng xuất được lưu `nPlace = pos_affairitem` và nạp lại vô hình (tiền tồn; `KJx2WarInfra_ClearAffairBox` trả về túi khi mở box kế).
3. Quái VÀNG nhân `SpecialRate` 50% TRƯỚC khiên tĩnh + khiên điểm (KNpc.cpp:3520) → khiên hiệu quả gấp đôi trên NPC vàng có khiên; hiện chưa NPC vàng nào gán skill khiên. Cách sửa gọn: dời khối 3520-3523 xuống sau khiên điểm.
4. Công cụ ngoài game đọc 9 trường DB tái dụng (`ToolsMySQL\sinh_role_item.py`, `schema_tracuu.sql`: gia_bay_ban/khoa_*) sẽ đọc rác ở món có dấu `FUS1`.
5. Phân phối client: thiếu `settings\item\fusion.txt` = **Game.exe KHÔNG khởi động** (`KBPT_Fusion` trong `paryBPT`); mọi client cũ nhận `s2c_syncfusion` = lệch luồng → bộ 3 phải cùng nhau, kèm `settings\item\{fusion,smeltupgradetable,magicscript}.txt` + `Ui\Ui3\smeltsystem\*.ini` + `spr\vng\ui\smelt\*.spr`.
6. Nguồn cấp Tinh Thiết Khoáng / Tinh Tinh Khoáng / Huyền Hỏa Than cho người chơi vẫn chưa có (GM `AddItem(6,1,4428,1,0,0,0)` để thử).

### 6.4 Build + đặt `.moi` (02/09 00:24–00:25, incremental trên HEAD `5c83e77a` + vá)
| Tệp | md5 | Đặt tại |
|---|---|---|
| CoreServer.dll | `B0D3596B300F05B6B1E02EF289C39599` | `bin\server\CoreServer.dll.moi` (bản `3bedd3ac` của phiên bot backup `_moi_backup\0109_3bedd3ac\`) |
| CoreClient.dll | `2C14866841D3B0DE6639EA1966CF3129` | `bin\client\CoreClient.dll.moi` |
| Game.exe | `6AA1FEB9C55B563F49068A555B4F6841` | `bin\client\Game.exe.moi` |

Superset: chứa cả bot đợt c/d (`f58c1fdc`, `7f041f8d`) và Hoa Sơn đợt b–e. Kiểm obj: 12 obj server / 27 obj client mốc 00:24, phần còn lại mốc 00:13/00:14 (full build phiên Hoa Sơn), không trộn header.

⚠️ Hiện trạng lúc đặt: GameServer đang chạy `9d7ae996` (00:17, restart 00:19) = **dung luyện đợt 1 CHƯA vá**: cho tới khi swap, viên Văn Cương/Tinh Thần Thạch trong túi **mất sau relogin**, Tinh Thiết Khoáng dùng vô hạn, box không nhận trang bị rộng 2 ô.

### 6.5 Checklist swap (đúng thứ tự)
1. Tắt GameServer → chạy `bin\server\ChayGameServer.bat` (tự đổi `CoreServer.dll.moi` → `CoreServer.dll`, bản cũ thành `.truoc`).
2. Thoát HẲN Game.exe → chạy `bin\client\ChoiGame.bat` (tự đổi `CoreClient.dll.moi` + `Game.exe.moi`). Relog nhân vật KHÔNG đủ.
3. Kiểm đã chạy bản mới: `Get-FileHash bin\server\CoreServer.dll` = `B0D3596B…`, `Get-Process GameServer | select StartTime` mới hơn mốc đổi tên.
4. Dữ liệu đã nằm sẵn ở cả 2 cây (không cần chép): `magicscript.txt` md5 `cc47d296ccea` server = client.
Restart mà chưa làm bước 1-2 thì vẫn chạy bản `9d7ae996` cũ.

### 6.6 Kiểm thử bổ sung (ngoài mục 5)
(h) viên Văn Cương + Tinh Thần Thạch trong túi và rương → thoát/vào lại còn nguyên, tooltip cùng thuộc tính. (i) Giáp HK 2x3 + viên → cả hai vào khay, bảng bên phải hiện "Số lượng Văn Cương đã dung luyện x/y". (j) Bấm Hủy → box đóng, đồ về túi; mở lại từ NPC. (k) Tinh Thiết Khoáng chuột phải → nhận Văn Cương và khoáng bị trừ 1. (l) Tinh chế → tooltip trang bị hết dòng Văn Cương NGAY, 6 viên về túi "Đã qua thí luyện". (m) Trang bị từng bày sạp → gỡ khỏi sạp → dung luyện được; thoát/vào lại không tự bày lại. (n) Người khác xem trang bị / sạp thấy dòng Văn Cương. (o) Chuột phải Văn Tinh có Văn Cương cùng loại trong túi mở rộng → nâng cấp được. (p) Box tẩy luyện phi phong: 2 cột dòng ẩn có chữ.
