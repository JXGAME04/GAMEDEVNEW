# ĐẶC TẢ LÔI ĐÀI BANG HỘI bản CN gốc — kết xuất workflow wf_7fbfbf9a-54d (21/08/2026, 11 tác tử, 1,81 triệu token)



## dac_ta

# ĐẶC TẢ HỆ THỐNG "LÔI ĐÀI BANG HỘI" (citywar_arena) — BẢN CN GỐC, TÁI DỰNG ĐỂ VIẾT TRONG GS

Tổng hợp từ 8 báo cáo dịch ngược `jx_linux_y` (GS) + `s3relay_y` (relay) + script. Mọi khẳng định kèm nguồn `(binary:địa chỉ)` hoặc `(tệp:dòng)`. Ký hiệu: **R** = relay `s3relay_y`, **G** = GS `jx_linux_y`. Mục 6 liệt kê các điểm chưa rõ / mâu thuẫn — người viết lại PHẢI đọc trước khi chốt thiết kế.

---

## 0. Tóm tắt kiến trúc gốc (để hiểu vì sao phải viết lại)

| Thành phần gốc | Ở đâu | Vai trò |
|---|---|---|
| `KCityWarDataRelay` | R, obj = `[0x837ead4]+0x2c8` qua `0x080FC0B2`, vtable `17KCityWarDataRelay` `0x081DB5A8` | **Chủ dữ liệu**: nhận báo danh, cắt danh sách, bốc thăm bảng đấu, chuyển pha, lưu DB, phát tin |
| `KCityWarDataGS` | G, obj toàn cục `0x0830A680`, vtable `0x08251168` (cha `KCityWarDataCore` `0x082511B0`), ctor `0x08058830` | **Bản sao chỉ-đọc + báo kết quả**: nhận đồng bộ SDB, dựng lại cặp đấu, phục vụ 14 hàm Lua |
| Lớp lõi chung | `BuildArenaPairs` G `0x0805A480` ≡ R `0x080A00EA`; `AddArenaResult` lõi G `0x0805C540` ≡ R `0x080A04A6` | Cùng một mã nguồn C++ (cùng bố cục struct, relay lệch −8 byte vì thiếu vptr — báo cáo 2) |
| Kênh GS→relay | gói byte0 `0x0A`, sub 1..6, gửi qua `KRelayClient::Send` G `0x080777A0` (this `0x0836EA80`); nhận R `0x0809EC68`, bảng `0x081DB374` | báo danh / thuế / kết quả lôi đài / kết quả công thành / bổ nhiệm |
| Kênh relay→GS | SDB `0x0F/0x27|0x29` theo TÊN bản ghi GBK; GS lọc `0x0820A460` → `0x08059660` (`城战-城区数据`) / `0x08058490` (`城战-擂台赛目前阶段`) | đồng bộ toàn bộ trạng thái |
| Kênh tin tức | R `0x08095798` → gói GM-chat `{3,0x21,"GM",…,"dw AddLocalNews(\"…\")"}` broadcast mọi GS (`0x080E4B7C`) | thông báo pha |
| Lịch | R task script `relaysetting\task\citywar_0N_*.lua` gọi 4 API Lua relay `StartSignUp 0x080FF2E6 / EndSignUp 0x080FF356 / StartArena 0x080FF3C6 / StartCityWar 0x080FF436` | nhịp 18h/19h/20h/20h hôm sau |

**Dự án JX1 không có relay ⇒ toàn bộ cột "R" phải chuyển vào `KJx2CityWar.cpp`**, kênh SDB/0x0A biến thành gọi hàm trực tiếp, kênh tin tức thành `AddLocalNews` nội bộ, lịch chuyển vào GlbTimer/timerserver.

---

## 1. MÔ HÌNH DỮ LIỆU

### 1.1 Hằng số cấu hình (`relaysetting\citywar.ini [CitySettings]`, báo cáo 8 :65-91; R đọc `0x081DB274`)

| Tên | Giá trị | Offset R | Offset G | Dùng ở |
|---|---|---|---|---|
| `SignUpFee` | 1.000.000 | R+0x04 | G+0x0C | sàn phí đấu thầu (R `0x0809B237`; G `0x0805867B`) |
| `MinTongLevel` | 18 | R+0x08 | — | cấp bang tối thiểu (R `0x0809B1E5`) |
| `MinTongCrowNumber` | 37 | R+0x14 | — | số thành viên tối thiểu (R `0x0809B155`) |
| `WarCycleValue` | 7 | — | — | chu kỳ tuần |
| `AreaName01..07` | Phượng Tường(map1), Thành Đô(11), Đại Lý(162), Biện Kinh(37), Tương Dương(78), Dương Châu(80), Lâm An(176) | R+0x58+i*0x20 | G+0x60+i*0x20 (`0x0830A6E0`) | CityID 1..7; **slot 0 không dùng** |
| Sàn/trần nhập phí ở client | 1.000.000 .. 99.999.999 | — | — | `infocenter_head.lua:118` `AskClientForNumber` |
| Số bang tối đa sau cắt | **16** | — | — | R `0x0809C816` (`size > 0x10`) |
| Số khe lôi đài | 8 (ArenaID 0..7 = map 213..220) | — | — | mọi hàm Lua G kiểm `arg<=7`; `timer.lua:8-20` |

### 1.2 Trạng thái toàn cục (bản ghi SDB `城战-擂台赛目前阶段` `R 0x081DB28E` / `G 0x0826BB56`)

| Trường | Kiểu | R | G | Ý nghĩa |
|---|---|---|---|---|
| field 0 | u8 `bArenaRunning` | R+0x19C | G+0x1A4 = `0x0830A824` | 1 khi lôi đài đang diễn ra (StartArena đặt `0x0809D107`; kết thúc cây đặt 0 `0x0809DCxx`; G tự tắt khi dựng cặp thấy cây xong `0x0805A6E7`) |
| field 1 | i32 `nArenaCity` | R+0x1A0 | G+0x1A8 = `0x0830A828` | CityID (1..7) đang đấu lôi đài; −1/0 khi rỗi (R `0x0809DCDC`) |

⇒ **Tại một thời điểm chỉ MỘT thành có lôi đài** (mọi hàm Lua G đọc `[0x0830A828]`, `GetArenaCityArea 0x080FDFF0` bỏ qua tham số).

### 1.3 Theo thành — `CITY_INFO[8]`, stride 0x72 (bản ghi SDB `城战-城区数据` `R 0x081DB2A2` / `G 0x0826BB6A`, khoá = CityID+1, GS trừ 1 tại `0x0805969E`)

Địa chỉ: R `this+0x1A0+c*0x72` (code truy cập +4,+8,+9,+0xA,+0x2A,+0x4A…), G `this+0x1AC+c*0x72` = `0x0830A82C+c*0x72` (các hàm Lua tính từ `0x0830A820+0x72*c+0xC`, `0x080FDF5D`).

| field (key2) | Offset trong struct | Kiểu | Tên đề xuất | Ghi bởi | Đọc bởi |
|---|---|---|---|---|---|
| 0 | +0x00 | i32 | `nIdx` | ctor | — |
| 1 | +0x04 | u8 | `bTaxSet` (đã đặt thuế) | R cmd2 `0x0809E78E` | thuế (ngoài phạm vi) |
| **2** | **+0x05** | **u8** | **`nState`** | R StartSignUp/EndSignUp/StartArena/AddArenaResult/StartCityWar | G `NumOfSignUpTongs` (1..3), `HaveBeginWar` (==5), `CW_GetCityStatus` R |
| 3 | +0x06 | char[32] | `szOwnerTong` chủ thành | R (AppointViceroy/kết quả công thành) | G `GetCityOwner 0x080FFF40`, `GetCityWarBothSides`; nhận field 3 còn gọi `0x080FB030(0x09780B60,c,tên)` |
| 4 | +0x26 | char[32] | `szOwnerMaster` (tên bang chủ người, R lấy tongInfo+0x44 `0x08099AE1`) | R | — |
| **5** | **+0x46** | **char[32]** | **`szChallenger`** khiêu chiến giả | R EndSignUp (1 bang) `0x0809C451`, kết thúc cây `0x0809DBF7`, AppointChallenger `0x08099040` | G `GetCityWarBothSides 0x080FFDA0` (trả (challenger, owner)), R kiểm BECHALLENGER |
| 6 | +0x66 | i32 | thuế giao dịch (ctor −1) | R cmd2 | — |
| 7 | +0x6A | i32 | chỉ số giá (ctor −1) | R cmd2 | — |
| 8 | +0x6E | i32 | thời điểm chiếm thành `time()` (R `0x08099B63`) | R | — |
| 9 | — | — | bỏ qua | — | — |
| **10** | vector riêng | chuỗi | danh sách báo danh + phí | R mỗi lần SignUp `0x0809B4AA`, ClearCity | G `0x08059978` |
| **11** | vector riêng | u16[] | bảng đấu (bracket) | R EndSignUp `0x0809C98E` | G `0x08059A00` → dựng cặp |
| 12 | — | i32 | số bản ghi kết quả (chỉ đối chiếu, `Arena Result Count Error 0x08250F38`) | R `0x0809D77F` | G `0x08059820` |
| ≥13 | — | 37 byte | kết quả trận thứ (field−13) | R `0x0809D67C` | G `0x08059800` → `AddArenaResult(bKeep=1)` |

**Máy trạng thái `nState`** (suy từ mọi điểm ghi/kiểm — báo cáo 1, 2, 5, 6):

| Giá trị | Nghĩa | Ai đặt | Ai đòi |
|---|---|---|---|
| 0 | rỗi | EndSignUp khi 0 bang `R 0x0809C18B` | — |
| 1 | đang mở báo danh | StartSignUp `R 0x0809ABCE` | SignUp `R 0x0809AF1E`, EndSignUp `R 0x0809C0F5` |
| 2 | đã chốt báo danh, có bảng đấu, chờ StartArena | EndSignUp ≥2 bang `R 0x0809C858` | StartArena `R 0x0809CF94` |
| 3 | lôi đài đang diễn ra | StartArena `R 0x0809D100` | AddArenaResult `R 0x0809D54F` |
| 4 | đã có khiêu chiến giả, chờ công thành | EndSignUp 1 bang `R 0x0809C41E`; kết thúc cây `R 0x0809DCD5` | StartCityWar chỉ đòi `szChallenger` ≠ rỗng `R 0x0809E16C` |
| 5 | công thành đang diễn ra | StartCityWar `R 0x0809E3A1` | G `HaveBeginWar 0x08104808` |

`NumOfSignUpTongs` chỉ trả số khi state ∈ {1,2,3} (`G 0x080FDF91-9E`); kiểm ALREADYSIGNUP dùng {1,2,3} (`R 0x0809B01E`).

### 1.4 Theo thành — 5 vector (stride 12 mỗi thành; R +0x530/+0x590/+0x5F0/+0x650/+0x6B0; G +0x53C/+0x59C/+0x5FC/+0x65C/+0x6DC = `0x0830ABBC/AC1C/AC7C/ACDC/AD5C`)

```c
struct ARENA_PAIR   { u16 nTongA; u16 nTongB; u8 nFlag; };          // 5 byte, stride 5 (imul 0xCCCCCCCD G 0x8139642)
struct ARENA_RESULT { u16 nTongA; u16 nTongB; u8 nResult; u8 extra[32]; }; // 37 byte (imul 0x914C1BAD)

std::vector<std::string> vecSignUpTong[8]; // tên bang báo danh, thứ tự push; CHỈ SỐ trong vector = "tong index" dùng ở mọi nơi khác
std::vector<int>         vecSignUpFee[8];  // phí đấu thầu song song (R dùng để cắt >16; G chỉ lưu, mặc định SignUpFee G 0x080599B0)
std::vector<ARENA_RESULT> vecResult[8];    // nResult: 0 chưa, 1 = A thắng, 2 = B thắng
std::vector<u32>         vecBracket[8];    // size = 2^k >= N (≤16); low16 = tong index hoặc 0xFFFF (bye); high16 = 0 sống / 1 vừa thua / 2 đã loại
int                      nArenaLevel[8];   // G 0x0830AD3C+c*4: vòng đang chờ, 1-based; 0 = chưa/đã xong
std::vector<ARENA_PAIR>  vecPair[8];       // cặp của VÒNG HIỆN TẠI; chỉ số = ArenaID (0..7) = map 213+ArenaID
```
Bằng chứng: ctor `G 0x08058830` (5 mảng vector, vòng 0x60), `GetArenaBothSides` lõi `G 0x08139610` đọc `+0x6DC` rồi tra tên `+0x53C`, `OnCityData G 0x08059660` field 10/11.

Ngoài ra: `std::map<std::string,{int camp; char[32]}>` G+0x740 (`0x0830ADC0`) nhận gói relay→GS `0x0A/0x03` (`R 0x0809A954`, `G 0x08058D40`) — phục vụ `GetCityWarTongCamp`, camp = TONG task value 2 (= `TONG_GetCurCamp`), KHÔNG phải 1 thủ/2 công (báo cáo 5).

### 1.5 Trạng thái bang (Tong::CityWarState, R `0x080D5F28`→`0x080C46C8`, ghi DB field 0xb + gói client `{6,0x1b,tongId,state}`)

0 = bình thường (reset sau lôi đài `R 0x0809DD11`), 2 = đang đấu lôi đài (StartArena `R 0x0809D095-D0ED`), 3 = khiêu chiến giả (`R 0x0809DD7C`), 4 = đang công thành (StartCityWar `R 0x0809E352/E384` cho cả chủ thành và khiêu chiến).

---

## 2. MÁY TRẠNG THÁI + LỊCH

### 2.1 Lịch tuần CN gốc (báo cáo 8: `relaysetting\task\citywar_0N_*.lua`, `tasklist.ini Task_14..41`)

Mỗi thành 1 ngày D cố định (ngày ghi danh, `%w`): Dương Châu(6)=CN(0), Thành Đô(2)=T2, Đại Lý(3)=T3, Phượng Tường(1)=T4, Tương Dương(5)=T5, Biện Kinh(4)=T6, Lâm An(7)=T7 (`TB_CITYWAR_ARRANGE citywar_head.lua:22-30`; tệp startarena: `TaskSetMode(1); TaskSetStartDay(1,d); TaskInterval(7); TaskTime(20,0)` với mốc 2005-01-01 = thứ 7, `R TaskSetStartDay 0x080FECB6` năm mặc định 0x7D5).

| Giờ | Ngày | API relay | Điều kiện | Hiệu ứng |
|---|---|---|---|---|
| 18:00 | D | `StartSignUp(c)` | `cw_CanStart(c,1)` (bảng ngày nghỉ `TAB_NONE_CITYWAR`, chỉ kiểm ngày, không kiểm thành — `citywar_head.lua:32-51`) | xoá dữ liệu thành cũ (`R 0x08098B3C`), state=1, tin `G_CITYWAR_NOTIFY_SIGNUP` |
| 18:00–19:00 | D | người chơi báo danh | xem 2.2 | push vào 2 vector, đồng bộ field 10 |
| 19:00 | D | `EndSignUp(c)` | state==1 | 0 bang→state 0; 1 bang→state 4 + challenger; ≥2→cắt ≤16, bốc thăm, state 2, field 11 |
| 20:00 | D | `StartArena(c)` | state==2, bracket≠∅, chưa có kết quả, `bArenaRunning==0` (`R 0x0809D004-D036`) | bang báo danh → TongState 2; state 3; `bArenaRunning=1`, `nArenaCity=c`; tin `ARENA_START` |
| 20:00 → ? | D | các vòng lôi đài tự chạy (2.4) | — | mỗi trận GS báo `NotifyArenaResult` → relay ghi; hết cây → state 4, challenger = vô địch, `bArenaRunning=0`, `nArenaCity=−1` |
| 0:00 | D+1 | `citywar_clearchallenger` (VN) xoá League 508/509 | — | **không** đụng `szChallenger` trong CITY_INFO |
| 20:00 | D+1 | `StartCityWar(c)` | `szChallenger` ≠ rỗng (`R 0x0809E16C`) | chủ rỗng → challenger chiếm luôn (`WAR_RESULT3 0x8389948`, `R 0x0809E1AA`); có chủ → TongState 4 cả hai, state 5 |
| sau đó | D+1 | GS `NotifyWarResult` (`0x0A/5`) | — | relay xử `0x0809E65C` (ngoài phạm vi) |

Mỗi ngày trong tuần luôn có 1 thành báo danh+lôi đài và 1 thành khác công thành. Bản VN thay pha 2 bằng `GetRandomChallenger` (`citywar_head.lua:64-97,115-145`) nhưng vẫn gọi `StartArena(c)` hàng tuần (tệp startarena không có nhánh `vn`) → lôi đài chết vì GS VN không có `SignUpCityWarArena`.

### 2.2 Báo danh — 11 điều kiện (R `0x0809AE32`, gọi từ gói `0x0A/1`)

Gói GS→relay `0x0A/1` 74 byte: `+2 i32 city, +6 char[32] tên người, +0x26 char[32] tên bang, +0x46 i32 fee` (G dựng `0x0805CA10`, gọi từ `KCityWarDataGS::SignUp 0x08058650` ← `LuaSignUpCityWar 0x08121B50` — **thân còn, không đăng ký**, báo cáo 5/6). Phía GS kiểm trước: city 1..7, `nState==1` (`G 0x08121C7E`), fee ≥ SignUpFee (`G 0x0805867B`), người gọi là bang chủ (`strcmp` `G 0x080586A1`, lỗi `"%s over oper signup."`).

Relay kiểm theo thứ tự, mỗi lỗi gửi 1 tin cho người chơi qua `0x08095956` (gói relay→GS `0x0A/2`):

| # | Điều kiện thất bại | Chuỗi | Địa chỉ |
|---|---|---|---|
| 1 | tên bang rỗng | (bỏ qua) | `0x0809AE40` |
| 2 | tra bang thất bại (`0x080D6554`) | TONGERROR | `0x0809AE8D` |
| 3 | người gửi ≠ bang chủ (tongInfo+0x64) | NOTTONGLEADER | `0x0809AEED` |
| 4 | `nState[city] != 1` | SIGNUPTIMEOUT | `0x0809AF26` |
| 5 | bang đã có trong `vecSignUpTong[i]` với `nState[i]∈{1,2,3}`, i=0..7 | ALREADYSIGNUP | `0x0809B01E-B046` |
| 6 | bang == `szChallenger[i]` bất kỳ | BECHALLENGER | `0x0809B073-B0BF` |
| 7 | bang == `szOwnerTong[i]` bất kỳ | BECITYOWNER | `0x0809B0EC-B138` |
| 8 | thành viên (tongInfo+0x18 + +0x1c + +0x20 + 1) < MinTongCrowNumber | TOOFEWCROWS | `0x0809B155-B1B1` |
| 9 | tongInfo+0x24 ≠ 0 (đang "khảo nghiệm") | TESTINGPERIOD | `0x0809B1BB` |
| 10 | cấp bang (tongInfo+0x14) < MinTongLevel | LEVELLOW | `0x0809B1E5` |
| 11 | fee < SignUpFee HOẶC quỹ bang (i64 tongInfo+4) < fee | NOTENOUGHFEE | `0x0809B237-B2B1` |

Đạt: trừ quỹ bang `0x080D69E6(tongMgr,0,fee,tên)` (`0x0809B2D9`); push tên (`0x0809B326`), push fee (`0x0809B39C`); lưu DB + đồng bộ field 10 (`0x0809B3DE-B502`, nối bằng `'\n'` `0x081DB0DD`); gửi bang chủ SIGNUP_OK; `AddLocalNews` SIGNUP_TOALL (`0x0809B5A4`); gửi camp bang xuống GS (`0x0809AEC4`→`0x0809A954`).

### 2.3 Kết báo danh (R `0x0809C0BA`)

```
if state != 1: log PERIODNOTMATCH(1, state); return                       // 0x0809C0F1-C144
N = size(vecSignUpTong[c])
N == 0: state = 0; lưu field 2; AddLocalNews NO_SIGNUP                    // 0x0809C17A-C3DB
N == 1: state = 4; szChallenger = vecSignUpTong[0]; lưu field 2, 5;
        AddLocalNews SINGLE_SIGNUP                                         // 0x0809C3E0-C7F2
N >= 2: if N > 16: CutTo16()  (2.3.1)                                      // 0x0809C816
        BuildBracket() (3.1); state = 2; lưu field 2 + field 11 (u16[] low16 của bracket);
        AddLocalNews SIGNUP_COMPLETE(%s thành, %d bang)                    // 0x0809C7F7-CC58
```
**Lưu ý**: với N≥2, EndSignUp KHÔNG chọn khiêu chiến giả; với N==1 KHÔNG hoàn tiền đấu thầu (không thấy gọi `0x080D69E6` ở nhánh này — báo cáo 1).

**2.3.1 Cắt >16** (R `0x0809B736`): sort theo `vecSignUpFee` giảm dần (so sánh `0x0809B8C2 cmp/setl`, hoán đổi đồng bộ cả 2 vector `0x080A12CA/12EE/1324`); các bang từ vị trí 16 trở đi: hoàn tiền `0x080D69E6` (`0x0809BB7A`), gửi bang chủ SIGNUP_OUT (`0x0809BC36`), xoá khỏi 2 vector, lưu DB.

### 2.4 Lôi đài chạy (sau StartArena)

1. GS đã có bracket (field 11 lúc EndSignUp) và đã dựng `vecPair` (`G 0x08059A7F` → `0x0805A480`), nhưng `IsArenaBegin` còn trả 0 vì `bArenaRunning==0`.
2. StartArena ghi field 1 (`nArenaCity`) rồi field 0 (`bArenaRunning=1`) (`R 0x0809D21B / 0x0809D2CF`) → G nhận `0x08058490`.
3. Timer toàn cục 18 (mission 8, 5 phút, `settings/timertask.txt:19`, `citywar_global/timer.lua:8-20`): với i=1..8, `IsArenaBegin(i-1)==1` → `OpenMission(5)` trên map 213+i-1 nếu `GetMissionV(1)==0`.
4. Mission 5 (`citywar_arena/mission.lua`): lấy `GetArenaBothSides(ArenaID)` → MissionS(1)=A (phe 1 vàng), MissionS(2)=B (phe 2 tím); 8 phút vào sân (GO_TIME=24 tick × 20 s, `timer.lua:3-23`), tổng 25 phút (`TIMER_2`), tối đa 16 người/phe (`head.lua JoinCamp`), chết = `DelMSPlayer` (`death.lua`). Thắng: phe kia trống (`timer.lua:41-61`) hoặc hết giờ đông hơn; hoà → tổng cấp THẤP hơn thắng (`totaltimer.lua:37-74`).
5. `WinBonus(camp)` (`head.lua:141-151`): `NotifyArenaResult(ArenaID, camp==1 ? 1 : 0)` + `AddTongExp(tên, 1200)`.
6. G `0x08122800` → `0x08058330`: cặp `vecPair[nArenaCity][ArenaID]` phải `nFlag==0` (`0x805839F`, lỗi "repeat error"); bản ghi `{A,B, nResult = (arg2>=1)?1:2, 0[32]}` (`0x080583B7-C1`); gọi `AddArenaResult(bFromRelay=0)` `0x080593A0`: ghi/đè `vecResult` (`0x08059447/591`), bên thua = (nResult==2 ? A : B) đánh `high16 |= 1` trong bracket (`0x080595F6`), dựng lại cặp `0x0805A480(c,0)`, gửi gói `0x0A/4` 43 byte `{0x0A,4,i32 city, ARENA_RESULT}` (`0x080594F6-0x0805953A`).
7. Relay `0x0809D4F0`: đòi state==3, `bArenaRunning`, `nArenaCity==city`, A,B < N, result≠0 (`0x0809D54F-D628`); lõi `0x080A04A6` (cùng bước 6); lưu field 13+n rồi field 12 → **mọi GS** nhận và chạy `AddArenaResult(bKeep=1)` (không gửi lại). Còn `bArenaRunning` → tin ARENA_RESULT(thắng, thua); hết cây → tin ARENA_RESULT2, `szChallenger=vô địch`, state=4, `nArenaCity=−1`, `bArenaRunning=0`, TongState: mọi bang báo danh 0, vô địch 3; lưu field 0/1 toàn cục + field 2/5 thành (`0x0809DA2B-0x0809E112`).
8. Vòng kế: `vecPair` mới, `IsArenaBegin(k)` lại =1 cho các cặp chưa đấu → timer 18 mở lại mission 5 (mission trước đã `CloseMission` ⇒ `MissionV(1)==0`). Không có gói "mở khe" riêng (báo cáo 2).

Thời lượng một vòng ≈ 25 phút + tối đa 5 phút chờ timer 18 ⇒ 16 bang (4 vòng) ≈ 2 giờ; không có giới hạn giờ kết thúc trong binary.

---

## 3. THUẬT TOÁN GHÉP CẶP / XẾP VÒNG (chính xác theo relay)

### 3.1 `BuildBracket` (R `0x0809CC62`) — chạy 1 lần lúc EndSignUp

```
N = size(vecSignUpTong[c]); if N <= 1 return 0                      // 0x0809CC9A
size = 1; while size < N: size <<= 1                                 // 0x0809CCF0-CD07   (N=2→2, 3..4→4, 5..8→8, 9..16→16)
bracket = vector<u32>(size, 0xFFFFFFFF)                              // 0x0809CD31-CD6D
used = vector<int>(N, 0); srand(time(NULL))                          // 0x0809CD6F-CD7D
for i = 0; i < size; i += 2:                 // KHE CHẴN trước         // 0x0809CD95-CE2D
    r = rand() % N; while used[r]: r = (r+1) % N                     //  tiến vòng tròn
    used[r] = 1; bracket[i] = r
for i = 1; i < size && count_used < N; i += 2:   // KHE LẺ bằng phần còn lại   // 0x0809CE3F-CEEA
    r = rand() % N; while used[r]: r = (r+1) % N
    used[r] = 1; bracket[i] = r
vecResult[c].clear()  (0x080A0CF6);  BuildArenaPairs(c, 0)  (0x080A00EA)
```
Hệ quả: vì `size/2 ≥ N/2` khe chẵn luôn đủ cho ⌈N/2⌉... chính xác: khe chẵn có `size/2` ô ≥ N/2; nếu N ≤ size/2 thì không xảy ra (size là lũy thừa nhỏ nhất ≥ N ⇒ N > size/2) ⇒ khe chẵn luôn lấp đầy hết `size/2` bang, khe lẻ nhận `N − size/2` bang, còn `size − N` khe lẻ = **bye**. Ví dụ N=3: bracket 4 = [a, b, c, −1]; N=5: bracket 8, 3 bye; N=16: không bye. **Không** xếp hạt giống theo phí/cấp — hoàn toàn ngẫu nhiên.

Đồng bộ field 11: chỉ gửi low16 mỗi khe (`R 0x0809C9A1-CA62`), GS nạp thành u32 với high16=0 (`G 0x08059A00`).

### 3.2 `BuildArenaPairs(c, bKeep)` (G `0x0805A480` ≡ R `0x080A00EA`; bản inline trong `0x0805C540`) — chạy sau mỗi kết quả

```
size = size(bracket); if size <= 1: return
n = size; level = 1
loop:
    vecPair[c].clear()                                               // 0x805A51B
    seg = size / n; allDone = true
    for i = 0; i < size; i += 2*seg:
        A = khe đầu tiên trong [i, i+seg)     có low16 != 0xFFFF && high16 != 2   // 0x0805A568-587
        B = khe đầu tiên trong [i+seg, i+2seg) cùng điều kiện                    // 0x0805A5A8-5C1
        if A && B:
            flag = (A.high16 != 0) ? 2 : (B.high16 != 0) ? 1 : 0   // 2 = A thua, 1 = B thua, 0 = chờ đấu
            if flag == 0: allDone = false
            push {A.low16, B.low16, flag}                            // 0x0805A5D4-620
        // thiếu 1 bên ⇒ không tạo cặp ⇒ bên còn lại "bye", tự lên vòng sau
    nArenaLevel[c] = level                                           // 0x0805A62B
    if !allDone: return                                              // đây là vòng hiện tại
    mọi khe high16 != 0 → high16 = 2                                 // 0x0805A688-6A9 (loại hẳn)
    n >>= 1; level++
    if n <= 1: nArenaLevel[c] = 0; if !bKeep: bArenaRunning = 0; return   // 0x0805A6C5-6E7
```

Tính chất cần tái tạo đúng:
- **ArenaID** = chỉ số cặp trong `vecPair` của vòng hiện tại, đếm từ trái sang phải, bỏ qua đoạn thiếu bên — **không cố định theo vị trí khe**; vòng sau đánh số lại từ 0.
- Trong một vòng, cặp đã có kết quả (flag≠0) và cặp chưa đấu (flag 0) cùng tồn tại; `IsArenaBegin` chỉ =1 với flag 0 (`G 0x8139651`). Vòng chỉ tiến khi TẤT CẢ cặp có kết quả.
- Tổng số vòng = `log2(size)` (`GetArenaTotalLevel 0x08129510` đếm chia đôi `size(vecBracket)`); `nArenaLevel` 1-based; 0 = chưa bắt đầu hoặc đã kết thúc. Chung kết khi `nArenaLevel == log2(size)` (`GetArenaSchedule 0x081223A3`), bán kết khi `== log2(size)−1`.
- Trường hợp đặc biệt: N=0/1 không có bracket (EndSignUp xử riêng); N=2 → 1 vòng, 1 cặp; N=3 → 2 vòng (1 cặp + 1 bye, rồi chung kết); vô địch = khe duy nhất còn high16==0 sau khi `n<=1`; relay lấy tên thắng từ bản ghi cuối (`0x0809D856-D86B`).
- Kết quả trùng: `AddArenaResult` tìm bản ghi cùng (A,B) với nResult==0 để đè, ngược lại push (`G 0x08059447/0x08059591`); cặp đã có flag≠0 thì `NotifyArenaResult` từ chối ("repeat error").

### 3.3 Ví dụ N=5 (bracket 8): khe chẵn 0,2,4,6 = 4 bang ngẫu nhiên, khe 1 = bang thứ 5, khe 3,5,7 = bye
Vòng 1 (seg=1): cặp0=(k0,k1); đoạn [2,3],[4,5],[6,7] thiếu bên → k2,k4,k6 bye → `vecPair`=1 cặp, ArenaID 0 duy nhất → chỉ map 213 mở. Vòng 2 (seg=2): cặp0=(thắng(k0,k1), k2), cặp1=(k4,k6) → map 213, 214. Vòng 3: chung kết.

---

## 4. CHỮ KÝ 14 HÀM LUA PHÍA GS VÀ GIÁ TRỊ TRẢ THEO TRẠNG THÁI

Ký hiệu trạng thái: **S0** rỗi/báo danh (state 0/1), **S2** đã có bracket chưa StartArena, **S3** đang lôi đài, **S4** đã có challenger, **S5** công thành. `c*` = `nArenaCity`.

| Hàm (G địa chỉ) | Tham số | Trả | S0 | S2 | S3 (vòng L, cặp k flag 0) | S4/S5 |
|---|---|---|---|---|---|---|
| `IsArenaBegin(id)` `0x08101620` | id 0..7 | 1 số | 0 | 0 (`bArenaRunning`=0) | 1 nếu `vecPair[c*][id]` tồn tại && flag==0, còn lại 0 | 0 |
| `GetArenaBothSides(id)` `0x08104680` | id | 2 chuỗi | "","" | "","" | tên A, tên B (từ `vecSignUpTong[c*]`) nếu flag==0; "","" + log lỗi nếu không | "","" |
| `GetArenaCityArea(id)` `0x080FDFF0` | id ≤7 (không dùng) | 1 số | 0 (c* ngoài 1..7) | 0 | c* | 0 (c*=−1) |
| `GetArenaTargetCity()` `0x080FBE20` | — | 1 số | 0 | 0 | c* | 0 |
| `GetArenaLevel()` `0x08104640` | — | 1 số / 0 giá trị + log nếu c* sai | (0 giá trị) | (0 giá trị) | `nArenaLevel[c*]` (1..) | (0 giá trị) |
| `GetArenaTotalLevel()` `0x08129510` | — | 1 số | 0 / lỗi | — | `log2(size(vecBracket[c*]))` | 0 |
| `GetArenaTotalLevelByCity(c)` `0x0812B880` | c 1..7 | 1 số | 0 nếu bracket rỗng | log2(size) | log2(size) | log2(size) (bracket chưa xoá tới StartSignUp kế) |
| `GetArenaSchedule(c)` `0x08122000` | c | 1 chuỗi | `G_ScriptFuns_2` "chưa tiến hành" (pair rỗng hoặc nLevels==0) | chuỗi vòng 1 "%s đấu với %s" | tiêu đề (quirk `strncpy(..,3)` `0x081220C5`) + "Hai bên quyết đấu/Bán kết/Lượt %u" + từng cặp "đấu với"/"thắng" nối bằng `G_CITYWARDATA_7` | tuỳ pair còn lại |
| `GetArenaInfoByCity(c[,lvl])` `0x08121D00` | c (lvl bị bỏ qua) | 1 chuỗi | `G_CITYWARDATA_8` "chưa tiến hành" nếu `vecResult` rỗng | như S0 | "<< Khu vực thành %s >>\n" + mọi bản ghi kết quả; buffer 0x200, đầy → `G_ScriptFuns_3` | toàn bộ kết quả |
| `NotifyArenaResult(id, bAWin)` `0x08122800` | id ≤7, bAWin ≥1 = A thắng, else B thắng | 0 giá trị | lỗi log | lỗi (c* sai) | ghi kết quả (mục 2.4 bước 6) | lỗi |
| `NumOfSignUpTongs(c)` `0x080FDEF0` | c 1..7 | 1 số | 0 nếu state 0; N nếu state 1 | N | N | 0 |
| `GetSignUpTongName(c, i)` `0x0812C690` | c, i 0-based | 1 chuỗi | tên hoặc "" (không kiểm state) | tên | tên | tên cho tới khi StartSignUp kế xoá |
| `AppointChallenger(tênThành, tênBang)` `0x0812CD00` | 2 chuỗi, ≤0x1F | 0 giá trị | gửi relay `0x0A/6` flag 1 (`0x080584F0`); relay `0x08098FA8` ghi `szChallenger` + **xoá toàn bộ báo danh/bracket/kết quả thành** (`0x080988C4`) | như | như | như |
| `GetCityWarTongCamp(tênBang)` `0x081228C0` | chuỗi | 1 số / nil | camp từ map +0x740 (chỉ có với bang đã báo danh/chủ thành, gửi bởi relay `0x0809A954`, `0x0809AA1A`) | | | |

Hàm liên quan khác: `GetCityWarBothSides(c) 0x080FFDA0` → (challenger, owner) chỉ khi cả hai ≠ rỗng; `HaveBeginWar(c) 0x08104770` → state==5; `GetCityOwner(c) 0x080FFF40` → `szOwnerTong`; `IsSigningUp(c) 0x08115930` tồn tại, nhận 1..7 (nội dung chưa đọc — báo cáo 7); `SignUpCityWarArena(c, fee)` **không đăng ký** (bảng Lua `0x088C5E24..0x088C7A9C` không có; thân `LuaSignUpCityWar 0x08121B50` không xref).

Script gọi thực tế (báo cáo 7): `timer.lua` → IsArenaBegin; `mission.lua` → GetArenaBothSides, GetArenaCityArea, GetCityAreaName; `head.lua:143/147` → NotifyArenaResult; `infocenter_head.lua:34-79` PreEnterGame/EnterGame/EnterBattle (không NPC nào gọi hiện nay); còn lại 0 caller.

---

## 5. ĐIỂM NỐI VỚI DỰ ÁN JX1 (`KJx2CityWar.cpp`) — KHÔNG RELAY

Hiện trạng theo báo cáo 5 (`KJx2CityWar.cpp` dòng trích dẫn; **chưa tự kiểm lại trong phiên này**): `LuaGetCityWarBothSides :407` (đúng thứ tự (challenger, owner)), `LuaHaveBeginWar :504` (`JX2CW_STATE_ATWAR=2` thay byte 5 — tương đương), `LuaAppointViceroy :546`, `LuaAppointChallenger :581` (ghi ngay trong GS), `LuaNumOfSignUpTongs/GetSignUpTongName :619/:628` (đọc League 508), `LuaGetCityWarTongCamp :741` (trả 1/2 — KHÁC gốc), nhóm Arena stub `KJx2CityWar.h:44-54`, `.cpp ~675-735`; đăng ký `ScriptFuns.cpp:14766-14795`. Báo danh VN hiện qua League 508/509 + item 1508 + bốc thăm 19h `script\timerserver_ctc.lua`.

### 5.1 Phải VIẾT MỚI trong `KJx2CityWar.cpp`
1. **Dữ liệu mục 1.2–1.4** gắn vào struct thành hiện có (7 thành → giữ mảng 8, chỉ số = CityID 1..7): thêm `nState` 6 giá trị (ánh xạ sang `JX2CW_STATE_*` hiện có — cần đối chiếu), `szChallenger` (đã có), `vecSignUpTong/Fee/Result/Bracket/Pair`, `nArenaLevel`, toàn cục `bArenaRunning`, `nArenaCity`.
2. **`SignUpCityWarArena(CityID, Fee)`** = GS-side `0x08058650` + relay-side `0x0809AE32` gộp: 11 điều kiện mục 2.2 (nguồn tongInfo thay bằng `KTongSet` dự án; "khảo nghiệm" tongInfo+0x24 cần tìm trường tương ứng hoặc bỏ — ghi vào 6), trừ quỹ bang, push 2 vector, `AddLocalNews` SIGNUP_TOALL, Msg2Player SIGNUP_OK. Đăng ký Lua tên `SignUpCityWarArena` để `infocenter_head.lua:118` chạy.
3. **`StartSignUp(c)` / `EndSignUp(c)` / `StartArena(c)` / `StartCityWar(c)`** = `0x0809AB8C / 0x0809C0BA / 0x0809CF60 / 0x0809E13C` thành hàm C++ nội bộ, expose Lua (tên trùng relay) để gọi từ `timerserver_ctc.lua` hoặc GlbTimer. `StartSignUp` phải xoá sạch 5 vector + `nArenaLevel=0` (`G 0x0805973B-97B4` là hiệu ứng của `R 0x08098B3C`).
4. **`BuildBracket`** (3.1, dùng `rand()` sau `srand(time)`) và **`BuildArenaPairs`** (3.2) — chép đúng, kể cả ArenaID đánh số lại mỗi vòng.
5. **`AddArenaResult`** = G `0x080593A0` + R `0x0809D4F0` gộp (không gửi gói): kiểm, ghi kết quả, đánh thua, dựng lại, tin ARENA_RESULT/ARENA_RESULT2, kết cây → `szChallenger`, state 4, TongState (nếu dự án có trường `CityWarState` của bang — cần đối chiếu `KTong`), `bArenaRunning=0`.
6. **14 hàm Lua mục 4** thay stub: đọc trực tiếp cấu trúc trên; giữ nguyên ngữ nghĩa trả về (kể cả "" "" và 0 giá trị) vì script `citywar_arena\*.lua` dựa vào đó.
7. **Stringtable**: `G_CITYWAR_NOTIFY_*`/`WARNING_*` (relay `lang/vn/stringtable_relay.txt`, nạp `0x0811AB30-AC66`) và `G_CITYWARDATA_0..8` (`stringtable_core.txt:1035-1042`, `_7` trong bản VN còn `<<未翻译>>`) phải có trong bảng chuỗi của dự án.

### 5.2 Phải ĐỔI / THAY
- `timerserver_ctc.lua` bốc thăm 19h (= `GetRandomChallenger` VN) → thay bằng `EndSignUp(c)` 19:00 + `StartArena(c)` 20:00 cùng ngày, `StartCityWar(c)` 20:00 hôm sau (hoặc giữ 20:30 như bản nháp `congthanhchien\citywar_startcitywar_4.lua:6` — quyết định của chủ game). Giữ `cw_CanStart`/`TAB_NONE_CITYWAR` nếu muốn ngày nghỉ. Múi giờ: xem memory `gio-server-mui-gio-wauto`.
- League 508/509 + "Khiêu chiến lệnh" (item 1508, League 538): **hoặc** bỏ hẳn (đường CN gốc thu tiền đấu thầu), **hoặc** giữ làm điều kiện phụ — nhưng `NumOfSignUpTongs/GetSignUpTongName` phải chuyển nguồn sang `vecSignUpTong` và có cổng state∈{1,2,3}. `citywar_clearchallenger` xoá 508/509 lúc 0:00 không được đụng `szChallenger`.
- `LuaAppointChallenger` (:581) phải thêm hành vi gốc: xoá 5 vector + `nArenaLevel` của thành (`R 0x080988C4`) — cần thiết để GM "chốt tay" không để lại bracket mồ côi.
- `LuaGetCityWarTongCamp` (:741): gốc trả `TONG_GetCurCamp` của bang — hiện 0 caller script, để nguyên hoặc đổi, ghi rõ trong tài liệu.
- NPC: gắn `ArenaMain`/`PreEnterGame` (`infocenter_head.lua:34-79,141-156`) vào NPC nghị sự (`sevencity/dialog_npc.lua:34-43` hiện không gọi) vì `manager.lua` trên map 213+ không kiểm `IsArenaBegin`.
- `citywar_arena/mission.lua` cần map 213..220 tồn tại và mission 5/global mission 8 + timer 18 đăng ký trong `settings/task/missions.txt:6,9`, `timertask.txt:19`, `autoexec.lua:142`.

### 5.3 Persist (mirror `settings\jx2citywar.txt`)
Relay lưu DB bảng `城战-城区数据` (record key = CityID+1, field 2/5/10/11/12/13+n) và `城战-擂台赛目前阶段` (field 0/1) sau MỖI thay đổi; GS gốc không lưu gì. Dự án phải mirror tối thiểu: `nState`, `szChallenger`, `vecSignUpTong+Fee`, `vecBracket` (low16 đủ — high16/`vecPair`/`nArenaLevel` tái dựng bằng cách nạp bracket rồi replay toàn bộ `vecResult` qua `AddArenaResult(bKeep=1)` đúng như GS nhận field 11 rồi field ≥13, `G 0x08059A7F`/`0x08059800`), `vecResult`, `bArenaRunning`, `nArenaCity`. Thứ tự nạp phải là: bracket → kết quả → toàn cục (nếu nạp toàn cục trước, `BuildArenaPairs(bKeep=0)` có thể tắt `bArenaRunning` sớm — lý do gốc dùng bKeep=1 khi nạp).

---

## 6. CHƯA RÕ / MÂU THUẪN GIỮA CÁC BÁO CÁO

1. **Ai/khi nào xoá dữ liệu thành cũ**: báo cáo 1 chưa đọc `R 0x08098B3C` (gọi đầu StartSignUp); báo cáo 6 khẳng định đó là "reset danh sách" gửi SDB flag>>5==3 field 10 → G xoá 5 vector (`0x08059728`). Chấp nhận theo 6 nhưng chưa có disasm thân `0x08098B3C`.
2. **`GetArenaTotalLevel` đếm gì**: báo cáo 4 gọi vector `+0x65C` là "số bang báo danh" và suy "3 bang→1 vòng"; báo cáo 2/3/5 xác định `+0x65C` là **bracket (size 2^k)**. Theo ctor và `OnCityData` field 11 → bracket đúng ⇒ 3 bang → 2 vòng. Đặc tả dùng kết luận sau.
3. **Cờ thua trong bracket**: báo cáo 2/4 `|0x10000` (`G 0x080595F6`), báo cáo 6 viết `+0x20000`. Đọc `0x080595F0-FC` cho 0x10000; 0x20000 là nâng lên "đã loại" ở `0x0805A69C`. Dùng 1 = vừa thua, 2 = loại.
4. **Tham số `NotifyArenaResult`**: binary `arg2>=1 → 1 (A thắng)` (`0x080583B7`); script truyền 1/0. Khớp, nhưng giá trị 2 từ script cũng = A thắng — không phải "2 = B thắng" như ARENA_RESULT.nResult.
5. **Ý nghĩa CITY +0x04 (field 1), +0x26 (field 4), +0x66/+0x6A/+0x6E**: báo cáo 6 nói thuế/giá/thời điểm chiếm; báo cáo 3/5 "chưa rõ". Không ảnh hưởng lôi đài.
6. **Khung giờ StartCityWar**: CN gốc 20:00 D+1 (`citywar_0N_startcitywar.lua`, `TB_CITYWAR_ARRANGE` cột 2); bản nháp VN `congthanhchien\` 20:30 cùng ngày. Chủ game chọn.
7. **Không có giới hạn thời gian lôi đài** trong binary: nếu một trận không bao giờ `NotifyArenaResult` (mission không mở vì map thiếu), cây treo với state 3 tới StartSignUp tuần sau (xoá) — và khi đó `bArenaRunning` vẫn 1 ⇒ StartArena tuần sau lỗi `0x0809D036`. Cần quy tắc dự án: ví dụ StartCityWar/StartSignUp cưỡng chế tắt.
8. **Điều kiện "khảo nghiệm" (tongInfo+0x24) và 3 trường đếm thành viên +0x18/+0x1c/+0x20**: suy từ phép cộng, chưa đối chiếu struct bang relay; dự án phải tự ánh xạ sang `KTong`.
9. **Hoàn phí**: chỉ thấy hoàn khi cắt >16 (`0x0809BB7A`). Không thấy hoàn/thưởng tiền cho thắng/thua lôi đài ở cả GS lẫn relay (lời thoại `camper.lua:44` nói cược 1000 vạn, +800 vạn — không có mã). Báo cáo 7 giả định "relay xử lý tiền cược" — **không có bằng chứng**; coi như không có.
10. **Bye ở vòng 1 chỉ rơi vào khe lẻ** (3.1) — suy từ thuật toán; chưa chạy thử trên binary.
11. **`IsSigningUp 0x08115930`** tồn tại trên G nhưng nội dung đọc trường nào chưa dịch (khả năng `nState==1`).
12. **Bản relay `BuildArenaPairs 0x080A00EA`** chỉ kiểm chứng qua offset, chưa đọc từng lệnh để xác nhận 100% giống G `0x0805A480` (báo cáo 2).
13. **Offset relay +0x530 vs +0x534** (báo cáo 1 vs 5): cùng vector, 5 trỏ vào `begin` (+4). Không mâu thuẫn thực.
14. **Vtable GS `0x08251168` vs `0x082511B0`**: lớp con vs lớp cha; cùng 1 hàm ảo `AddArenaResult`. Không mâu thuẫn.
15. **NPC map 213-220** (manager/camper1/camper2/leavetrap) — dữ liệu map không còn trong `D:\ServerLinux\server1\maps\map_publish`; chỉ có log boot nạp script (`Logs/KSG_ScriptOutputLog_20260806.txt:3745-3753`). Dự án phải tự đặt NPC.
16. **Gói relay→GS `0x0A/3` (camp) và vòng đồng bộ `0x0809AA1A`**: caller chưa truy; dự án không cần nếu `GetCityWarTongCamp` giữ bản hiện tại.
17. **`GetJoinTongTime()>=7200`** (`camper.lua:79-95`) đơn vị chưa xác minh.
18. Các báo cáo đều KHÔNG đọc `KJx2CityWar.cpp` trực tiếp ngoài báo cáo 5 (số dòng trích) — mục 5 cần kiểm lại trên mã nguồn trước khi thi công.

## phan_bien

```json
[
 {
  "sai_sot": [
   {
    "muc": "1.3 Địa chỉ struct CITY_INFO phía relay",
    "bao_cao_sai": "\"Địa chỉ: R this+0x1A0+c*0x72\" (rồi liệt kê code truy cập +4,+8,+9,+0xA…) trong khi bảng field lại ghi nState = +0x05, owner = +0x06, challenger = +0x46",
    "su_that": "Gốc struct phía relay là this+0x1A4+c*0x72 (lệch đúng 8 so với G +0x1AC). Mọi điểm đọc nState đều là `imul eax,c,0x72; lea eax,[this+eax]; add eax,0x1A0; movzx [eax+9]` → +0x1A4+0x05. Viết +0x1A0 làm người đọc lắp bảng offset +0x05/+0x06/+0x46 vào sai gốc 4 byte.",
    "nguon": "R 0x0809C0E6-0x0809C0F1 (EndSignUp), R 0x0809CF85-0x0809CF94 (StartArena), R 0x0809C84D-0x0809C858 (ghi state 2)",
    "muc_do": "NHE"
   },
   {
    "muc": "3.2 BuildArenaPairs — vị trí ghi nArenaLevel",
    "bao_cao_sai": "Pseudo-code đặt `nArenaLevel[c] = level // 0x0805A62B` SAU vòng for (ghi vô điều kiện mỗi vòng)",
    "su_that": "Lệnh ghi `mov [edx+eax*4+0xc],ecx` tại 0x0805A62B nằm TRONG nhánh `if A && B` (sau push cặp, trước nhảy về đầu vòng lặp 0x0805A62F). Vòng nào không tạo được cặp nào thì nArenaLevel không đổi. Với N≥2 thực tế luôn có ≥1 cặp/vòng nên không gây sai lệch chạy thật, nhưng chép mã phải đặt đúng chỗ.",
    "nguon": "G 0x0805A5C8-0x0805A62F: `cmp cx,-1; je 0x805a62f` / `cmp ax,-1; je 0x805a62f` nhảy qua cả đoạn push + ghi level",
    "muc_do": "NHE"
   },
   {
    "muc": "3.2 / 2.4 bước 6 — AddArenaResult tìm bản ghi trùng",
    "bao_cao_sai": "\"AddArenaResult tìm bản ghi cùng (A,B) với nResult==0 để đè\"",
    "su_that": "Vòng tìm khớp KHÔNG phân biệt thứ tự: khớp khi (rec.A==A && rec.B==B) HOẶC (rec.B==A && rec.A==B). Ngoài ra nếu bản ghi khớp đã có nResult≠0 thì KHÔNG đè, KHÔNG đánh thua, KHÔNG dựng lại cặp, nhưng với bKeep=0 VẪN gửi gói 0x0A/4 lên relay (relay sẽ push thêm bản ghi mới vì lõi relay cũng kiểm tương tự). Dự án viết lại nên kiểm (A,B)/(B,A) và chặn hẳn trường hợp trùng.",
    "nguon": "G 0x08059425-0x0805943B (so A rồi B, nhánh 0x08059408 so B với A, 0x08059560 so A với B), G 0x0805943D `cmp byte [eax+4],0; jne 0x80594e8` → 0x080594E8 kiểm bKeep rồi gửi gói 0x2B byte qua 0x080777A0",
    "muc_do": "NHE"
   },
   {
    "muc": "5.3 Persist — lý do thứ tự nạp bracket → kết quả → toàn cục",
    "bao_cao_sai": "\"nếu nạp toàn cục trước, BuildArenaPairs(bKeep=0) có thể tắt bArenaRunning sớm — lý do gốc dùng bKeep=1 khi nạp\"",
    "su_that": "Khi GS nhận field 11 (bracket) nó gọi BuildArenaPairs(c, bKeep=0) (0x08059A70 đẩy 0) nhưng bracket vừa nạp có high16=0 toàn bộ nên vòng 1 luôn có cặp flag 0 → không bao giờ đi tới nhánh n<=1 tắt bArenaRunning. bKeep=1 chỉ dùng ở field ≥13 (0x08059805) để khi replay bản ghi CUỐI (cây xong) không tự tắt bArenaRunning — tức là quan trọng là kết quả phải đi qua bKeep=1, còn thứ tự nạp toàn cục trước/sau không gây tắt sớm. Lý do nêu trong đặc tả sai, kết luận (replay với bKeep=1) vẫn đúng.",
    "nguon": "G 0x08059A4A `movzx edx, word [ebx+edi*2]` (high16=0), 0x08059A70 `mov [esp+8],0` → call 0x805a480; G 0x08059805 `mov [esp+0xc],1` → call [vtable] AddArenaResult; G 0x0805A6CF-0x0805A6E7",
    "muc_do": "NHE"
   },
   {
    "muc": "4 / 6.4 — tham số bAWin của NotifyArenaResult",
    "bao_cao_sai": "\"arg2>=1 → 1 (A thắng)\" được hiểu là so sánh có dấu; đặc tả chỉ bàn 0/1/2",
    "su_that": "Phép so sánh là KHÔNG DẤU (`cmp [ebp+0x10],1; sbb eax,eax; not; add 2`): mọi giá trị ≠0 kể cả ÂM đều cho nResult=1 (A thắng); chỉ đúng 0 mới là B thắng. Tương tự kiểm phí ở GS 0x0805867B (`ja`) và relay 0x0809B240 (`ja`) đều không dấu → fee âm vượt sàn SignUpFee (chỉ bị chặn bởi AskClientForNumber phía client). Dự án viết lại nên kiểm có dấu.",
    "nguon": "G 0x080583A9-0x080583C1; G 0x0805867B `cmp [eax+0xc],edi; ja`; R 0x0809B23A-0x0809B240",
    "muc_do": "NHE"
   },
   {
    "muc": "2.2 — phạm vi CityID kiểm ở GS-side SignUp 0x08058650",
    "bao_cao_sai": "\"Phía GS kiểm trước: city 1..7, nState==1 (G 0x08121C7E)…\" gộp chung như thể thân 0x08058650 kiểm",
    "su_that": "Thân KCityWarDataGS::SignUp 0x08058650 chỉ kiểm `city <= 7` (không chặn 0), chỉ số người chơi 1..0x4AE, fee ≥ SignUpFee, và strcmp 2 chuỗi trong struct người chơi (+0x5974 vs +0x5A90). Kiểm 1..7 và nState==1 nằm ở wrapper Lua 0x08121B50 như đặc tả dẫn — cần ghi rõ để khi gộp hàm không bỏ sót chặn city=0.",
    "nguon": "G 0x0805866B `cmp ebx,7; ja`, 0x08058670-0x08058679, 0x0805867B, 0x08058680-0x080586A1",
    "muc_do": "NHE"
   }
  ],
  "bo_sung": [
   "ĐÃ KIỂM ĐÚNG (mở lại disasm): BuildArenaPairs G 0x0805A480 — size = (end-begin)>>2 của +0x65C, return nếu ≤1; vecPair +0x6DC/+0x6E0 clear tại 0x0805A51B; seg = size/n; tìm khe đầu low16≠0xFFFF && high16≠2 (0x0805A568-587 / 0x0805A5A8-5C1); flag: A.high16≠0 → 2 (0x0805A5EF), else B.high16≠0 → 1 (0x0805A740), else 0 và allDone=0 (0x0805A70E); stride cặp 5 byte (0x0805A61D add 5); nâng high16≠0 thành 2 qua `movzx cx; add 0x20000` (0x0805A699-6A2); n>>=1, level++ (0x0805A6B4-6B7); kết thúc: vecPair.end=begin, nArenaLevel[c]=0 tại +0x6BC+4c, và chỉ khi bKeep==0 mới ghi byte +0x1A4=0 (0x0805A6CF-6E7).",
   "ĐÃ KIỂM ĐÚNG: BuildBracket R 0x0809CC62 — N từ +0x530 (size ≤1 → return 0); size lũy thừa 2 (0x0809CCF9-CD07); bracket +0x650 clear rồi push −1 size lần (0x0809CD23-CD6D); srand(time) 0x0809CD75-CD7D; vòng khe chẵn i+=2 KHÔNG kiểm count (0x0809CD95-CE2D), vòng khe lẻ i=1,i+=2 có điều kiện i<size && count<N (0x0809CECC-CEEA); rand()%N rồi tiến vòng tròn +1 (0x0809CDA7-CDB3).",
   "ĐÃ KIỂM ĐÚNG: AddArenaResult G 0x080593A0 — stride bản ghi 0x25 (0x08059419 add edx,0x25; 0x080595D5); bên thua: nResult==2 → A (0x08059620) ngược lại B; quét bracket tìm low16==loser rồi ghi `movzx si; add 0x10000` (0x080595F0-FC) = high16 đặt 1; gọi BuildArenaPairs(c, bKeep) truyền nguyên bKeep (0x08059608), không phải hằng 0; gói 0x0A/4 dài 0x2B gửi qua 0x080777A0 this=0x0836EA80 chỉ khi bKeep==0.",
   "ĐÃ KIỂM ĐÚNG: GetArenaBothSides lõi 0x08139610 (id≤7, imul 0xCCCCCCCD, byte+4==0 mới trả tên từ +0x53C), IsArenaBegin 0x08101620 (byte 0x830A824, dword 0x830A828 trong 1..7, id≤7), NumOfSignUpTongs 0x080FDEF0 (struct từ 0x830A820+0x72c+0xC, byte +5 ∈{1,2,3}, đếm vector 0x830ABBC+12c), GetArenaTotalLevel 0x08129510 (chia đôi size +0x65C của thành nArenaCity), GetArenaLevel 0x08104640 (0x830AD3C+4c), GetCityWarBothSides 0x080FFDA0 (+0x06 owner, +0x46 challenger, cả hai ≠rỗng), GetArenaSchedule strncpy(…,3) tại 0x081220C5, OnCityData field 11 nạp u16→u32 high16=0 (0x08059A4A), field 12 đối chiếu count*0x25 (0x08059820-3C), field 13+ gọi AddArenaResult bKeep=1 (0x08059805).",
   "ĐÃ KIỂM ĐÚNG: Relay EndSignUp 0x0809C0BA đòi state==1 (0x0809C0F5, log PERIODNOTMATCH(1,state)), cắt khi N>0x10 gọi 0x0809B736 (0x0809C816-C82D), gọi BuildBracket 0x0809CC62 rồi ghi state 2 (0x0809C858) và lưu SDB 城战-城区数据 (0x081DB2A2, key 0xe). StartArena 0x0809CF60: state==2 (0x0809CF94), bracket +0x650 rỗng / kết quả +0x5F0 không rỗng / byte +0x19C≠0 → lỗi (0x0809D004-D04F). Relay AddArenaResult: vô địch = bên thắng của chính bản ghi vừa nhận (0x0809D83A-D86B), sau lõi kiểm +0x19C để rẽ ARENA_RESULT / kết cây (0x0809D872-D87B). SignUp relay: thành viên = (+0x18)+(+0x1c)+(+0x20)+1 so MinTongCrowNumber R+0x14 (0x0809B155-B177), +0x24≠0 → TESTINGPERIOD (0x0809B1BB), cấp +0x14 < R+0x08 → LEVELLOW (0x0809B1E5-B1F3).",
   "Gợi ý cho thiết kế: vì GS gốc không hề kiểm trùng theo thứ tự (A,B)/(B,A) và không có cơ chế huỷ/timeout trận (mục 6.7 đúng), hàm AddArenaResult viết lại nên (a) kiểm trùng 2 chiều, (b) từ chối hẳn khi cặp đã có flag≠0 thay vì chỉ chặn ở NotifyArenaResult, (c) có đường cưỡng chế kết cây khi StartSignUp/StartCityWar chạy mà bArenaRunning còn 1."
  ]
 },
 {
  "sai_sot": [
   {
    "muc": "5.1 mục 2 + 5.2 NPC (đường báo danh Lua)",
    "bao_cao_sai": "Đăng ký Lua `SignUpCityWarArena` để `infocenter_head.lua:118` chạy, và gắn `ArenaMain` vào NPC nghị sự là đủ để mở đường báo danh đấu thầu.",
    "su_that": "Chính script CN đánh dấu đường này là BỎ: `infocenter_head.lua:87` `--报名指定城市擂台赛 (废弃2006-11-22)`, `:98` `SignUpTheOne (废弃2006-11-22)`, `:115` `SignUpFinal (废弃2006-11-22)`. `SignupACity`/`SignUpTheOne` (hai hàm duy nhất gọi `AskClientForNumber(\"SignUpFinal\",...)` :92/:109) KHÔNG được gọi từ `ArenaMain` — `ArenaMain:141-156` chỉ đưa lựa chọn `SignUpCityWar` (đường khiêu chiến lệnh/League 508, `:601`) và điều kiện mở là `getSigningUpCity(1)`/`getSignUpState(nCityId)==1` (League), không phải `IsSigningUp`. Gắn `ArenaMain` vào NPC sẽ vẫn đi đường lệnh bài; muốn đường đấu thầu gốc phải nối lại `SignupACity`/`SignUpTheOne` (hoặc viết NPC mới) — đặc tả phải nói rõ.",
    "nguon": "D:\\ServerLinux\\server1\\script\\missions\\citywar_global\\infocenter_head.lua:84,87-119,141-156,601",
    "muc_do": "NANG"
   },
   {
    "muc": "2.1 Lịch (dòng 0:00 D+1)",
    "bao_cao_sai": "`citywar_clearchallenger` (VN) chạy 0:00 D+1 xoá League 508/509.",
    "su_that": "Tệp `congthanhchien\\citywar_clearchallenger.lua:8-9` đặt `TaskInterval(1440); TaskTime(23,45)` = 23:45 CÙNG NGÀY D, và thư mục `congthanhchien\\` KHÔNG có trong `relaysetting\\task\\tasklist.ini` (chỉ có 28 mục `citywar_0N_*`, dòng 55-142) ⇒ trên relay VN đang chạy, task này không được nạp. Bản project port sang `ctc6_daily0h` (0h) là quyết định riêng của project, không phải hành vi relay.",
    "nguon": "D:\\ServerLinux\\gateway\\s3relay\\relaysetting\\task\\congthanhchien\\citywar_clearchallenger.lua:8-9; relaysetting\\task\\tasklist.ini:55-142 (grep congthanhchien = 0); D:\\GAMEDEVNEW\\serverscript_jx2\\citywar_e\\timerserver_ctc.lua:144",
    "muc_do": "VUA"
   },
   {
    "muc": "5 / 5.2 (đường dẫn bốc thăm 19h)",
    "bao_cao_sai": "Bốc thăm 19h của dự án nằm ở `script\\timerserver_ctc.lua`.",
    "su_that": "Tệp thật là `D:\\GAMEDEVNEW\\serverscript_jx2\\citywar_e\\timerserver_ctc.lua` (`ctc6_endsignup :105-127` gọi `AppointChallenger`, `ctc6_startwar :135-139` gọi `CTC_JX2_SetCityState(i,2)`, tick `CTC_JX2_Tick :158`). Không có `script\\timerserver_ctc.lua` trong `D:\\ServerLinux\\server1\\script`.",
    "nguon": "D:\\GAMEDEVNEW\\serverscript_jx2\\citywar_e\\timerserver_ctc.lua:96-197",
    "muc_do": "NHE"
   },
   {
    "muc": "4 bảng + 6.11 (IsSigningUp)",
    "bao_cao_sai": "`IsSigningUp` tồn tại trên G nhưng 'nội dung chưa đọc', và không nhắc dự án.",
    "su_that": "Dự án ĐÃ có `LuaIsSigningUp` `KJx2CityWar.cpp:606-614` (đọc League 508 task 1 == 1) và đăng ký `ScriptFuns.cpp:14778`; dự án còn có `LuaCTC_JX2_SetCityState` `:654-671` (đăng ký `:14783`) là cơ chế flip state 20h hiện tại. Mục 5.1-3 `StartCityWar` viết mới phải THAY thế/thu hồi `CTC_JX2_SetCityState` và `IsSigningUp` phải đổi nguồn sang `nState==1` — đặc tả bỏ sót hai hàm này.",
    "nguon": "D:\\GAMEDEVNEW\\Sources\\Core\\Src\\KJx2CityWar.cpp:606-614,654-671; ScriptFuns.cpp:14778,14783",
    "muc_do": "VUA"
   },
   {
    "muc": "5.1 mục 1 / 5.3 Persist",
    "bao_cao_sai": "Chỉ cần 'thêm nState 6 giá trị (ánh xạ sang JX2CW_STATE_*)'.",
    "su_that": "Bộ nạp mirror `KJx2CityWar.cpp:187` ép `nState` về `NORMAL` nếu ngoài 0..2 (`(nState >= 0 && nState <= 2) ? nState : JX2CW_STATE_NORMAL`), và `LuaCTC_JX2_SetCityState :664` cũng chặn `<= JX2CW_STATE_ATWAR`. Nếu thêm state 3..5 mà không sửa hai chốt này thì sau restart mọi thành đang lôi đài/bracket về 0 — lỗi persist. Ngoài ra dòng 'C' mirror hiện là `id state tax pp date` (:184) — chưa có chỗ cho challenger/vector.",
    "nguon": "D:\\GAMEDEVNEW\\Sources\\Core\\Src\\KJx2CityWar.cpp:184-190,664",
    "muc_do": "VUA"
   },
   {
    "muc": "5.2 ScriptFuns đăng ký",
    "bao_cao_sai": "Đăng ký `ScriptFuns.cpp:14766-14795`.",
    "su_that": "Khối đăng ký thật chạy `:14767` (`GetCityWarBothSides`) đến `:14799` (`ReduceArenaCredits`); nhóm Arena `:14785-14795` kèm 4 hàm `*ArenaCredits` `:14796-14799` (extern `:13294-13297`) mà đặc tả không nhắc — khi thay stub phải giữ nguyên các hàm Credits.",
    "nguon": "D:\\GAMEDEVNEW\\Sources\\Core\\Src\\ScriptFuns.cpp:13282-13297,14767-14799",
    "muc_do": "NHE"
   }
  ],
  "bo_sung": [
   "Đã KIỂM KHỚP (không sai): `LuaGetCityWarBothSides :407` trả (challenger, owner) :412-413; `LuaHaveBeginWar :504` so `JX2CW_STATE_ATWAR` :508; `LuaAppointChallenger :581` ghi challenger + WARDECIDED :591-592 không xoá vector (đúng như 5.2 yêu cầu bổ sung); `LuaNumOfSignUpTongs/GetSignUpTongName :619/:628` đọc League 508; `LuaGetCityWarTongCamp :741` trả 1 thủ/2 công :755-761; stub Arena :679-738; `JX2CW_STATE_NORMAL/WARDECIDED/ATWAR = 0/1/2` `KJx2CityWar.h:17-19`.",
   "Đã KIỂM KHỚP script GS: `citywar_global/timer.lua:10-20` gọi `IsArenaBegin(i-1)` → `OpenMission(5)` khi `GetMissionV(1)==0`; `mission.lua:13-17` gọi `GetArenaBothSides(GetMissionV(MS_ARENAID))`, `GetArenaCityArea(WorldID-WORLDIDXBEGIN)`; `head.lua:141-151 WinBonus` → `NotifyArenaResult(id, 1/0)` + `AddTongExp(…,1200)` (`WIN_TONGEXP=1200` head.lua:17); `GO_TIME = 480*FRAME2TIME/TIMER_1 = 24` tick×20 s = 8 phút (head.lua:30-32), `TIMER_2 = 25*60` (:31), `MAX_MEMBER_COUNT=16` (:12); hoà → `GetTotalLevel()==1` khi tổng cấp phe 1 < phe 2 → `WinBonus(1)` (totaltimer.lua:21-29,69-73) = phe tổng cấp THẤP thắng. `timertask.txt` dòng 19 = index 18 `citywar_global\\timer.lua`; `missions.txt` 5 = citywar_arena/mission.lua, 8 = citywar_global/mission.lua.",
   "Đã KIỂM KHỚP relay: `TB_CITYWAR_ARRANGE citywar_head.lua:22-30` đúng bảng ngày (Phượng Tường {3,4}, Dương Châu {0,1}, Lâm An {6,0}); `cw_CanStart :32-51` chỉ kiểm ngày theo `TAB_NONE_CITYWAR[nPhase]`; `citywar_01_startarena.lua:4-13,20-21` `TaskSetMode(1); TaskSetStartDay(1,5)` (chú thích 2005-01-05 thứ Tư) `TaskInterval(7); TaskTime(20,0)` gọi `StartArena(1)` KHÔNG có nhánh `vn`; `citywar_01_endsignup.lua:5-8,15-20` chạy hàng ngày 19:00, nhánh `vn` → `cw_endsignup_fun(3,1)`; `cw_endsignup_fun :115-145` dùng League 509 + `GetRandomChallenger :64` rồi `citywar_appointviceroy/appointchallenger` qua GlobalExecute; `congthanhchien\\citywar_startcitywar_4.lua:5-6` 20:30 hàng ngày; `citywar.ini` SignUpFee=1000000 (:69), MinTongLevel=18 (:72), MinTongCrowNumber=37 (:81), WarCycleValue=7 (:88).",
   "Bổ sung cho 5.2: `cw_endsignup_fun :144` tắt League 508 task1=0 mỗi ngày và `cw_startsignup_fun :101` gọi `citywar_tbLadder:Reset()` — khi thay bằng `StartSignUp/EndSignUp` mới phải quyết định số phận `citywar_tbLadder` (dùng bởi `citywar_CheckVotes infocenter_head.lua:122-124`) và League 508/509 mà `LuaIsSigningUp/NumOfSignUpTongs/GetSignUpTongName` đang đọc.",
   "Bổ sung cho 6: 11 hàm Arena trong dự án hiện được gọi từ script với GIÁ TRỊ STUB (IsArenaBegin=0) nên `citywar_global/timer.lua` không bao giờ mở mission 5 — sau khi thay stub, nếu `GetArenaBothSides` trả \"\",\"\" (flag≠0) thì `mission.lua:13-17` vẫn set MissionS rỗng và chạy mission với 2 phe tên rỗng; cần kiểm `tong1 ~= \"\"` trước `StartMissionTimer` (mission.lua:14-20) hoặc đảm bảo IsArenaBegin và GetArenaBothSides nhất quán trong cùng tick.",
   "Bổ sung: relay có hai bản `tasklist.ini` (`relaysetting\\task\\tasklist.ini` và `task\\new\\tasklist.ini`) cùng nội dung citywar_0N; đặc tả nên ghi rõ bản nào đang nạp (xem `relaysetting\\task\\…` trong cấu hình relay) trước khi kết luận lịch đang chạy."
  ]
 }
]
```

# PHỤ LỤC — báo cáo từng tác tử dịch ngược


## relay:StartSignUp+EndSignUp


### ket_luan

RELAY StartSignUp (0x080FF2E6) và EndSignUp (0x080FF356) là 2 hàm Lua nhận ĐÚNG 1 tham số số = CityID (slot 1..7 trùng AreaName01..07 của citywar.ini; slot 0 không dùng; getSigningUpCity trong relaysetting/task/congthanhchien/citywar_head.lua:34 trả 1..7). Cả hai lấy đối tượng KCityWarDataRelay (global 0x837ead4 -> +0x2c8, hàm 0x080FC0B2) rồi gọi lõi: StartSignUp -> 0x0809AB8C (đặt byte trạng thái city = 1, ghi DB, phát AddLocalNews "mở báo danh"); EndSignUp -> 0x0809C0BA (đòi trạng thái == 1; 0 bang -> trạng thái 0 + tin NO_SIGNUP; ĐÚNG 1 bang -> trạng thái 4, bang đó thành "khiêu chiến giả" ngay (tên chép vào city+0x4a, DB field 5); >= 2 bang -> nếu > 16 thì cắt còn 16 bang đấu thầu cao nhất (0x0809B736, hoàn tiền + gửi SIGNUP_OUT), bốc thăm bảng đấu loại trực tiếp (0x0809CC62) ghi vector this+0x650[city], trạng thái = 2 (chờ StartArena, StartArena 0x0809CF60 đòi == 2), DB field 11 = bảng đấu). Khi >= 2 bang thì EndSignUp KHÔNG chọn khiêu chiến giả - chỉ xếp lịch lôi đài. Relay KHÔNG gửi gói dữ liệu cấu trúc nào xuống GS trong 2 hàm này: chỉ broadcast gói "GM chat" (byte0=1, byte1=0x21, tên "GM", nội dung `dw AddLocalNews("...")`) tới MỌI GameServer đang nối (0x08095798 -> 0x08065E98 -> 0x080E4B7C duyệt danh sách session). Danh sách bang báo danh được nạp vào relay bằng gói GS->relay proto 0x0A sub 1 (0x0809ED36 -> 0x0809AE32): cityid/tên bang/tên bang chủ/tiền đấu thầu; kiểm 10 điều kiện, trừ quỹ bang, push vào vector<string> this+0x530[city] và vector<int> this+0x590[city]. CW_GetCityStatus đọc chính byte trạng thái city+9 (qua GetCityData 0x080C88CE trả city+4 rồi +5).

### chi_tiet

- [Chữ ký Lua] StartSignUp 0x080FF2E6: lua_gettop(0x081BC890) phải == 1 (0x080FF2F8), lua_tonumber(0x081BDCB0) -> fistp -> ebx = CityID (0x080FF32C-338); gọi 0x080FC0B2 (lấy KCityWarDataRelay: [0x837ead4] -> 0x0810D9BC trả [obj+0x2c8]) rồi 0x0809AB8C(obj, city) tại 0x080FF346; trả 0 kết quả. EndSignUp 0x080FF356 y hệt, gọi 0x0809C0BA(obj, city) tại 0x080FF3B6. Cả hai lõi kiểm 0<=city<=7 (0x0809AB97-ABA5; 0x0809C0C5-C0D3). Lớp này có vtable tên '17KCityWarDataRelay' (chuỗi 0x081DB5A8), bản GS tương ứng '16KCityWarDataCore' (0x081DB5BC).
- [StartSignUp lõi 0x0809AB8C] 0x0809ABB8 gọi 0x08098B3C(obj,city) (dọn dữ liệu city cũ, chưa đọc sâu); 0x0809ABCE `mov byte [obj+0x1a0+city*0x72+9], 1` = trạng thái ĐANG BÁO DANH; 0x0809ABD2-AC47 lưu DB bảng tên GBK 0x081DB2A2 = '城战-城区数据' (thành chiến - dữ liệu thành), khóa record = city+1 (0x0809ABFD), field id 2 ([ebp-0x1c]=2), dữ liệu = 1 byte trạng thái, qua 0x08106072 (update) + 0x08106974 (commit) hoặc 0x081065DE (insert mới); lỗi thì log 'save citywar data[region:%d object:%d] failed!' (0x081DB2B0). 0x0809ACDE-ACF8 sprintf(fmt=[0x8389914]=G_CITYWAR_NOTIFY_SIGNUP 'Lôi đài trong thành %s đã bắt đầu báo danh, hoan nghênh các bang hội đã đạt đến đẳng cấp %d...', tên thành = obj+0x58+city*0x20, cấp = obj+8 = MinTongLevel). 0x0809AD03-AD6C ghép 'dw AddLocalNews("' (0x081DB2F8) + tin + '")' (0x081DB30A) rồi gọi 0x08095798 = gửi lệnh GM xuống GS. 0x0809AE1D ghi log tệp (0x08095B1A -> \Logs\KSG_CityWarLog.txt, fmt '[%04d-%02d-%02d %02d:%02d:%02d]: %s').
- [Gửi xuống GS] 0x08095798: dựng gói: byte 3, byte 0x21, tên người gửi 'GM' (0x081DB216, 0x20 byte), word len, word 1, rồi chuỗi lệnh; gọi 0x08065E98 bọc thêm header {1, 0x21, 0,0,0, -1, len+1} + byte 'a' (0x08065F3F) + nội dung, rồi lấy [0x837ead4]+0x2b0 (0x0810D91C) và gọi 0x080E4B7C(mgr, buf, len) = lặp danh sách session ở mgr+0x14 (0x080E4BA3-4C31) gửi cho MỌI GameServer đang nối. Tức relay -> GS chỉ là kênh 'chat GM' chạy lệnh 'dw <lua>' trên GS; không có opcode dữ liệu riêng cho báo danh.
- [EndSignUp lõi 0x0809C0BA] 0x0809C0F1-C0F7: nếu byte trạng thái city+9 != 1 -> sprintf([0x838999c]=G_CITYWAR_WARNING_PERIODNOTMATCH 'Giai đoạn trước không phù hợp, lẽ ra là %d, thì lại là %d', 1, trạng thái hiện tại) ghi log (0x0809C144) và THOÁT. 0x0809C14E-C16D: size(vector tên bang obj+0x530+city*12+4) qua 0x08075E0A. ==0 (0x0809C17A-C3DB): trạng thái = 0 (0x0809C18B), lưu DB field 2, AddLocalNews([0x8389924]=G_CITYWAR_NOTIFY_NO_SIGNUP '%s cuộc thi đấu Lôi đài trong thành không người báo danh, tuần này vạn sự vô ưu'). ==1 (0x0809C3E0-C7F2): trạng thái = 4 (0x0809C41E), tên bang[0] (0x08092686 operator[]) strncpy 0x20 byte vào obj+0x1e0+city*0x72+0xa = city+0x4a (0x0809C451-C472) = Ô KHIÊU CHIẾN GIẢ; lưu DB field 2 (trạng thái) và field 5 (tên 0x20 byte, 0x0809C5A9-C5C4); AddLocalNews([0x8389928]=G_CITYWAR_NOTIFY_SINGLE_SIGNUP '%s ... chỉ có %s một bang hội báo danh, xem như đã đủ tư cách khiêu chiến'). >=2 (0x0809C7F7-CC58): nếu size > 16 (0x0809C816) gọi 0x0809B736 cắt danh sách; gọi 0x0809CC62 xếp bảng đấu; trạng thái = 2 (0x0809C858); lưu DB field 2 và field 11 (0x0809C98E; dữ liệu = mảng WORD = bảng đấu obj+0x650, size*2 byte, 0x0809C9A1-CA62); AddLocalNews([0x838992c]=G_CITYWAR_NOTIFY_SIGNUP_COMPLETE '%s Lôi đài trong thành đã chuẩn bị hoàn tất, tổng cộng có %d bang hội tham chiến, lịch thi đấu đã xác định...').
- [Bảng đấu 0x0809CC62] n = số bang; nếu n<=1 trả 0. bracketSize = lũy thừa 2 nhỏ nhất >= n (0x0809CCF0-CD07); vector obj+0x650[city] clear rồi push bracketSize phần tử -1 (0x0809CD31-CD6D); srand(time) (0x0809CD6F-CD7D); vòng 1 (0x0809CD95-CE2D): với i=0,2,4.. < bracketSize: idx = rand()%n, tăng vòng tới khi chưa dùng, đánh dấu dùng, bracket[i]=idx; vòng 2 (0x0809CE3F-CEEA): i=1,3,5.. và còn bang chưa dùng: bracket[i]=idx ngẫu nhiên. => cặp (bracket[2k], bracket[2k+1]); -1 = bye. Sau đó 0x080A0CF6 clear vector obj+0x5f0[city] và 0x080A00EA(obj,city,0) dựng danh sách trận vòng hiện tại (dùng bởi StartArena). Giá trị bracket là CHỈ SỐ vào vector tên bang obj+0x530[city].
- [Cắt >16: 0x0809B736] size > 0x10 mới làm; sắp xếp theo vector tiền đấu thầu obj+0x590[city] giảm dần (so sánh 0x0809B8C2 `cmp ebx,eax / setl`, dùng hoán đổi 0x080A12CA/12EE/1324); các bang từ vị trí 16 trở đi: hoàn tiền vào quỹ bang (0x080D69E6 tại 0x0809BB7A) và gửi cho bang chủ tin [0x8389950]=G_CITYWAR_NOTIFY_SIGNUP_OUT 'Số tiền tích lũy chưa đủ, bang hội của Bạn báo danh %s thất bại' qua 0x08095956 (0x0809BC36), rồi xóa khỏi cả 2 vector; lưu lại DB (0x0809B9xx-BCxx). Chuỗi log [0x83899bc]=L_CW_SIGNUP, [0x83899c0]=L_CW_KICKOUT_TONG.
- [Kho dữ liệu báo danh - cách nạp] Gói GS->relay proto byte0 = 0x0A (jump table 0x081D9738 mục 10 -> 0x08078CE0 -> 0x0809EC68), byte1 = sub (jump table 0x081DB374: 1=SignUp 0x0809ED36, 2=0x0809ED7E->0x0809E78E, 3, 4=0x0809EDD8->0x0809D4F0, 5=0x0809EE0E->0x0809E65C, 6=0x0809EE40). Sub 1 layout (0x0809ED44-ED72): +2 int cityid, +6 char tên bang[32], +0x26 char tên người gửi (bang chủ)[32], +0x46 int tiền đấu thầu; gọi 0x0809AE32(obj, city, tongName, playerName, fee). Phía GS script gọi SignUpCityWarArena(CityID, Fee) (infocenter_head.lua:118, AskClientForNumber 1.000.000..99.999.999) khớp 2 tham số này.
- [SignUp handler 0x0809AE32 - 10 điều kiện, mỗi lỗi gửi tin riêng cho người chơi qua 0x08095956(obj, playerName, msg)] (1) tên bang rỗng -> bỏ qua (0x0809AE40); (2) 0x080D6554 tra thông tin bang thất bại -> [0x838997c] TONGERROR (0x0809AE8D); (3) tên bang chủ (tongInfo+0x64) != playerName -> [0x8389978] NOTTONGLEADER (0x0809AEED); (4) trạng thái city != 1 -> [0x8389964] SIGNUPTIMEOUT 'Giai đoạn báo danh đã kết thúc' (0x0809AF26); (5) duyệt city 0..7: bang đã có trong vector tên obj+0x530[i] VÀ trạng thái city i thuộc {1,2,3} -> [0x8389968] ALREADYSIGNUP (0x0809B01E-B046); (6) bang == khiêu chiến giả city i (city+0x4a) -> [0x8389984] BECHALLENGER (0x0809B073-B0BF); (7) bang == chủ thành city i (city+0xa) -> [0x8389988] BECITYOWNER (0x0809B0EC-B138); (8) số thành viên (tổng 3 field tongInfo +0x18/+0x1c/+0x20 + 1) < obj+0x14 = MinTongCrowNumber(37) -> [0x838998c] TOOFEWCROWS (0x0809B155-B1B1); (9) tongInfo+0x24 != 0 -> [0x8389990] TESTINGPERIOD 'đang trong thời gian khảo nghiệm' (0x0809B1BB-B1DB); (10) cấp bang (tongInfo+0x14) < obj+8 = MinTongLevel(18) -> [0x8389974] LEVELLOW (0x0809B1E5-B22D); (11) fee < obj+4 = SignUpFee(1.000.000) HOẶC quỹ bang (int64 tongInfo+0x4/+0x8) < fee -> [0x8389970] NOTENOUGHFEE 'không đủ để nộp phí báo danh %d lượng' (0x0809B237-B2B1). Đạt: 0x080D69E6(tongMgr, 0, fee, tongName) trừ quỹ bang (0x0809B2D9); push_back tên vào obj+0x530[city] (0x08075D96 tại 0x0809B326), push_back fee vào obj+0x590[city] (0x080A10BC tại 0x0809B39C); lưu DB (serialize 2 vector với khóa fmt 'TaxPoint%02d' 0x081DB0DD, 0x0809B3DE-B502); gửi bang chủ [0x8389918] SIGNUP_OK; AddLocalNews [0x838991c] SIGNUP_TOALL '%s bang hội đã báo danh tham gia ... thành %s' (0x0809B5A4-B632).
- [CW_GetCityStatus 0x080FF5FA] lua_gettop==1, tonumber -> cityId; 0x080FC0B2 lấy obj; 0x080C88CE(obj, cityId) đòi 1<=cityId<=7 (KHÔNG nhận 0) và trả obj+0x1a0+cityId*0x72+4; nếu NULL -> lua_pushnil(0x081BD270); ngược lại đọc byte [+5] = city+9 = CHÍNH byte trạng thái mà StartSignUp/EndSignUp ghi, lua_pushnumber (0x081BD140). CW_GetOccupant 0x080FF6A2 cùng cách, đọc chuỗi tại [+6] = city+0xa = tên bang chủ thành (32 byte), rỗng -> nil.
- [Mã trạng thái city+9 suy ra] 0 = không có gì; 1 = đang báo danh (StartSignUp đặt; SignUp/EndSignUp đòi); 2 = đã xếp lịch lôi đài (EndSignUp >=2 bang đặt; StartArena 0x0809CF60 đòi ==2 tại 0x0809CF94, sai -> PERIODNOTMATCH); 3 = lôi đài đang/đã diễn (chỉ thấy trong kiểm ALREADYSIGNUP {1,2,3}, chưa xác định ai đặt); 4 = đã có khiêu chiến giả, chờ StartCityWar (EndSignUp đặt khi đúng 1 bang).
- [Thông báo] Các chuỗi tin lấy từ lang/vn/stringtable_relay.txt (nạp bằng 0x0811C71A tại 0x0811AB30-AC66 vào mảng con trỏ 0x8389914.. theo thứ tự G_CITYWAR_NOTIFY_SIGNUP, _OK, _TOALL, _LOG, _NO_SIGNUP, _SINGLE_SIGNUP, _SIGNUP_COMPLETE, _ARENA_START, ... _WARNING_PERIODNOTMATCH = 0x838999c). Log tệp relay: \Logs\KSG_CityWarLog.txt (0x081DB25B). Cấu hình đọc từ \relaysetting\citywar.ini (0x081DB274): obj+4 SignUpFee, obj+8 MinTongLevel, obj+0x14 MinTongCrowNumber, tên thành obj+0x58+i*0x20.
- [Script relay gốc CN] relaysetting/task/congthanhchien/citywar_startsignup_1.lua:19 StartSignUp(nCityId) lúc 18:00 hàng ngày; citywar_endsignup_2.lua:14 EndSignUp(nCityId) lúc 19:00; nCityId = getSigningUpCity(1) theo TB_CITYWAR_ARRANGE (thứ trong tuần -> thành 1..7). Bản VN đang chạy HinhThucCongThanh=0 (citywar_head.lua:8) tức vẫn gọi StartSignUp/EndSignUp; nhánh else là bốc thăm GetRandomChallenger qua League 508.

### cau_truc_du_lieu

KCityWarDataRelay (obj = [0x837ead4]+0x2c8, vtable '17KCityWarDataRelay'):
  +0x04 int SignUpFee (ini, 1.000.000); +0x08 int MinTongLevel (18); +0x14 int MinTongCrowNumber (37); +0x2b? khác chưa đọc
  +0x58 + i*0x20 : char AreaName[i][32], i = 0..7 (tên thành, dùng trong tin)
  +0x1a0 + city*0x72 : CITY[8], stride 0x72 = 114 byte, city slot 1..7 dùng (0 bỏ):
      +0x00..0x03 : (4 byte, GetCityData trả city+4)
      +0x09 : BYTE trạng thái lôi đài (0 rỗi / 1 báo danh / 2 đã xếp lịch / 3 lôi đài / 4 đã có khiêu chiến giả) = CW_GetCityStatus, DB field 2
      +0x0a : char OwnerTong[32] = bang chủ thành (CW_GetOccupant, kiểm BECITYOWNER)
      +0x4a : char Challenger[32] = khiêu chiến giả (EndSignUp 1 bang ghi; kiểm BECHALLENGER), DB field 5
      (còn 0x6a..0x71 chưa rõ: thuế/lịch)
  +0x530 + city*12 : std::vector<std::string> SignUpTongs[8] (tên bang báo danh, thứ tự push)
  +0x590 + city*12 : std::vector<int> SignUpFees[8] (tiền đấu thầu song song với +0x530)
  +0x5f0 + city*12 : std::vector<...> CurrentRoundMatches[8] (dựng bởi 0x080A00EA sau khi xếp bảng)
  +0x650 + city*12 : std::vector<int> Bracket[8] (kích thước = 2^k >= n, giá trị = chỉ số vào SignUpTongs, -1 = bye; cặp (2k,2k+1)); DB field 11 lưu dạng WORD[]
  +0x734 : log object (0x0817FA60 printf-log)
  +0x968 : chat/player mgr (gửi tin cho người chơi 0x08095956)
  +0x96c : DB handle (bảng GBK '城战-城区数据' 0x081DB2A2, record key = city+1, field 2 = trạng thái 1 byte, field 5 = Challenger 32 byte, field 11 = bracket; bảng '城战-擂台赛目前阶段' 0x081DB28E dùng ở chỗ khác)
Gói GS->relay báo danh: byte0=0x0A, byte1=0x01, +2 int CityID, +6 char Tong[32], +0x26 char Player[32], +0x46 int Fee (74 byte).
Gói relay->GS: {1,0x21, int 0, int 0, int 0, int -1, WORD len} + 'a' + "dw AddLocalNews(\"...\")" (GM chat broadcast tới mọi GS, 0x08065E98).

### chua_ro

- Hàm 0x08098B3C gọi đầu StartSignUp (dọn dữ liệu city trước khi mở báo danh?) chưa dịch ngược - có thể nó xóa SignUpTongs/Fees/Challenger cũ.
- Ai đặt trạng thái 3 và chuyển 2->4 sau lôi đài (nằm trong StartArena 0x0809CF60 / xử lý kết quả lôi đài sub 2..6 của proto 0x0A), ngoài phạm vi 2 hàm này.
- Làm sao GS biết trạng thái/lịch lôi đài (GetArenaSchedule, GetArenaBothSides, NumOfSignUpTongs...) - relay KHÔNG đẩy dữ liệu trong StartSignUp/EndSignUp; khả năng GS hỏi relay qua các sub-proto 2..6 hoặc relay đồng bộ KCityWarDataCore ở chỗ khác (GameSvrReady). Chưa xác minh.
- Các offset field còn lại của CITY (0x00-0x08, 0x6a-0x71) và ý nghĩa 3 trường đếm thành viên tongInfo +0x18/+0x1c/+0x20 chỉ suy từ phép cộng, chưa đối chiếu struct bang.
- Bản GS VN (jx_linux_y) không có SignUpCityWarArena nên gói 0x0A sub 1 hiện không bao giờ được gửi; cần dịch ngược thêm phía GS nếu muốn tái tạo chính xác hàm gửi gói.


## relay:StartArena


### ket_luan

relay:StartArena (Lua 0x080FF3C6 -> KCityWarDataRelay::StartArena 0x0809CF60) KHONG sap xep va KHONG ghep cap. No chi: (1) kiem tra city.state==2 (da EndSignUp), mang khe (slot) da co, chua co ban ghi ket qua, chua co dau truong nao dang chay; (2) doi trang thai city-war cua MOI bang da bao danh sang 2 (dang thi dau loi dai); (3) dat city.state=3, bArenaRunning=1, nArenaCity=cityIdx; (4) ghi 3 ban ghi SDB (dong bo xuong moi GS qua goi 0x0F/0x29); (5) loa AddLocalNews G_CITYWAR_NOTIFY_ARENA_START. Viec ghep cap da lam truoc do trong relay:EndSignUp (0x0809C0BA -> BuildArenaSlots 0x0809CC62): bang bao danh (toi da 16; >16 bi cat boi 0x0809B736) duoc xep NGAU NHIEN (rand) vao cay loai truc tiep kich thuoc = luy thua 2 nho nhat >= N; khe chan 0,2,4.. lap truoc, khe le 1,3,5.. lap bang phan con lai, khe trong = 0xFFFF = 'bye'. Cap dau tung vong (ArenaID 0..7) duoc SUY RA lai tu mang khe + danh sach ket qua boi ham loi dung chung (Core) BuildArenaPairs: GS 0x0805A480 (ban relay 0x080A00EA): vong L chia mang khe thanh cac doan 2^L, moi cap doan lay 'khe con song dau tien' cua nua trai vs nua phai; bang khong co doi thu (bye) tu dong len vong sau; ArenaID = thu tu cap duoc tao trong vong hien tai (khong gan co dinh theo vi tri khe). 'level' = vong dang cho dau, tang +1 khi TAT CA cap cua vong hien tai da co ket qua; 0 = chua bat dau / da ket thuc; tong so vong = log2(kich thuoc cay). GS bao ket qua bang goi GS->relay {0x0A,0x04,u32 city, record 37 byte {u16 A,u16 B,u8 result(1=A thang,2=B thang),32 byte 0}}; relay nhan trong KCityWarDataRelay::AddArenaResult 0x0809D4F0: danh dau bang thua (high16 cua khe=1), goi Core dung lai cap vong ke tiep, luu ban ghi ket qua qua SDB (object 12 = so ban ghi, 13+n = tung ban ghi) -> GS nhan object>12 tu chay AddArenaResult(bKeep=1) nen GS cung tu dung lai cap vong sau; khi cay ket thuc relay dat city.state=4, challenger = bang vo dich, nArenaCity=-1, bArenaRunning=0, loa ARENA_RESULT2, reset trang thai cac bang (0) va dat bang thach dau = 3. Relay KHONG gui goi 'mo khe' rieng: GS tu mo nhiem vu 5 tren map 213+ArenaID khi timer.lua thay IsArenaBegin(ArenaID)==1 (= ton tai cap index ArenaID voi flag ket qua =0). StartCityWar (0x0809E13C) doi chung: yeu cau challenger khac rong; neu thanh chua co chu -> challenger nhan thanh luon (WAR_RESULT3); nguoc lai dat chu thanh va challenger tong-state=4, city.state=5.

### chi_tiet

- CHU KY LUA (relay): StartArena(nCityIdx) 0x080FF3C6: lua_gettop==1 (0x81bc890), lua_tonumber (0x81bdcb0), lay doi tuong CityWar toan cuc qua 0x080FC0B2 (doc [0x837ead4]), goi 0x0809CF60(this, idx); tra 0 gia tri. StartCityWar(nCityIdx) 0x080FF436 y het, goi 0x0809E13C. EndSignUp 0x080FF356 -> 0x0809C0BA; StartSignUp 0x080FF2E6 -> 0x0809AB8C. nCityIdx hop le 0..7 (0x0809CF6B).
- StartArena 0x0809CF60 tung buoc: 0x0809CF8B-0x0809CF96 city[idx].state (byte this+0x1a0+idx*0x72+9) phai ==2, neu khong -> log G_CITYWAR_WARNING_PERIODNOTMATCH (global 0x838999c, format (2,state)) qua 0x08095B1A. 0x0809D004 size(vector<DWORD> this+0x650+idx*12) ==0 -> loi; 0x0809D02A count(vector 37-byte this+0x5f0+idx*12)!=0 -> loi; 0x0809D036 byte this+0x19c (bArenaRunning)!=0 -> loi; loi -> log L_START_ARENA_ERROR (0x83899c4). 0x0809D095-0x0809D0ED: for moi ten bang trong vector<string> this+0x530+idx*12 goi TongMgr(0x0807584E)->SetCityWarState(name,2) = 0x080D5F28 (tim bang theo ten 0x080D1A5A, goi Tong::SetCityWarState 0x080C46C8: ghi DB field 0xb va gui goi client {6,0x1b,tongId,state}). 0x0809D100 state=3; 0x0809D107 bArenaRunning=1; 0x0809D114 nArenaCity(this+0x1a0)=idx. 0x0809D11A-0x0809D38F: 3 lan SDB write 0x08106072 (DB this+0x96c): key GBK 0x81db2a2 '城战-城区数据' region=idx+1 object=2 data=&state(1 byte); key 0x81db28e '城战-擂台赛目前阶段' region=1 object=0 data=&nArenaCity(4 byte); cung key region=0 object=0 data=&bArenaRunning(1 byte). Neu write fail va 0x08106974 fail -> xoa (0x081065DE) hoac log 'save citywar data[region:%d object:%d] failed!' (0x81db2b0). 0x0809D3A0-0x0809D42A: format G_CITYWAR_NOTIFY_ARENA_START (global 0x8389930, gan tai 0x0811ABB9) voi ten thanh (this+0x58+idx*32) roi ghep 'dw AddLocalNews("'+msg+'")' (0x81db2f8/0x81db30a) va goi 0x08095798 (thuc thi Lua loa toan bo GS). Cuoi cung log ra KSG_CityWarLog qua 0x0809D4DB.
- EndSignUp 0x0809C0BA (can de hieu cay): state phai ==1 (0x0809C0F5). N=0 -> state=0, loa G_CITYWAR_NOTIFY_NO_SIGNUP (0x8389924). N==1 -> state=4 ngay, challenger (city+0x46, tuc this+0x1e0+idx*0x72+0xa) = ten bang duy nhat, loa SINGLE_SIGNUP (0x8389928) (0x0809C3FF-0x0809C472). N>=2: neu N>16 goi 0x0809B736 (sap xep theo vector phu this+0x590, giu 16, loai bang thua kem loa G_CITYWAR_NOTIFY_SIGNUP_OUT 0x8389950 + hoan tra qua 0x080D69E6/0x080D6554) (0x0809C816-0x0809C82D); roi BuildArenaSlots 0x0809CC62; state=2 (0x0809C858); ghi SDB object 2 (state) va object 0xb (mang khe, moi khe 2 byte = low16 cua DWORD, do dai 2*size: 0x0809C98E-0x0809CA86); loa G_CITYWAR_NOTIFY_SIGNUP_COMPLETE (0x838992c).
- BuildArenaSlots 0x0809CC62 (thuat toan xep khe): N=size(names 0x530) (0x0809CC9A), N<=1 -> 0. used=vector<int>(N,0) (0x080A0876/0x080A10BC). size=1; while size<N: size<<=1 (0x0809CCF0-0x0809CD07). clear slots 0x650 (0x080A0D12) va push size lan 0xFFFFFFFF (0x0809CD31-0x0809CD6D). srand(time(NULL)) (0x0809CD6F-0x0809CD7D). Vong 1: for i=0;i<size;i+=2: r=rand()%N (0x0809CD95), tien r vong tron toi bang chua dung (0x0809CDA7-0x0809CDD5), used[r]=1, slot[i]=r. Vong 2: for i=1; i<size && dem<N; i+=2: tuong tu cho khe le (0x0809CE3F-0x0809CEEA). clear ket qua 0x5f0 (0x080A0CF6) roi goi Core BuildArenaPairs 0x080A00EA(this,idx,0) tao cap vong 1. => KHONG sap xep theo cap/level/tien; hoan toan ngau nhien; bang o khe chan ma khe le ke ben trong = duoc 'bye' vong 1.
- Core BuildArenaPairs (GS 0x0805A480(this,city,bKeepRunning); relay 0x080A00EA; cung logic duoc inline trong AddArenaResult GS 0x0805C540/0x080593A0): slots = vector<DWORD> +0x65c+city*12 (GS), low16 = chi so bang (0xFFFF = trong), high16 = 0 con song / 1 vua thua / 2 da bi loai vong truoc. n=size; clear pairs (+0x6dc, phan tu 5 byte {u16 A,u16 B,u8 result}); level=1; while n>1: seg=size/n; for i=0;i<size;i+=2*seg: A = khe dau tien trong [i,i+seg) co low16!=0xFFFF && high16!=2 (0x0805A568-0x0805A587); B = tuong tu trong [i+seg,i+2*seg) (0x0805A5A8-0x0805A5C1); neu ca hai co: flag = 2 neu A.high16!=0, =1 neu B.high16!=0, =0 (va allDone=0) neu ca hai song; push {A.low16,B.low16,flag} (0x0805A5D4-0x0805A620); ArenaLevel[city] (+0x6bc+city*4; Lua GetArenaLevel doc 0x830ad3c[nArenaCity] tai 0x08104653) = level (0x0805A62B). Neu allDone==0 -> dung (cap vong nay la cap dang cho). Nguoc lai: moi khe co high16!=0 -> high16=2 (0x0805A688-0x0805A6A9), n>>=1, level++, lap lai. Het vong lap (n<=1): ArenaLevel=0, va neu bKeepRunning==0 -> bArenaRunning(+0x1a4)=0 (0x0805A6C5-0x0805A6E7). => ArenaID = chi so cap trong vector pairs cua vong hien tai (theo thu tu doan tu trai sang phai, bo qua doan khong du 2 ben). Le/bye: doan chi co 1 ben -> khong tao cap, bang do tu dong con song sang vong sau. Tong so vong = log2(size) (GS GetArenaTotalLevel 0x08129510: dem so lan chia doi size(+0x65c) tai 0x08129538-0x0812955D).
- GS doc cap: IsArenaBegin(ArenaID) 0x08101620: can bArenaBegin(0x830a824)!=0, nArenaCity(0x830a828) 1..7, goi 0x08139610(0x830a680, city, ArenaID, out1, out2): pair=pairs[city][ArenaID] (+0x6dc), chi tra 1 neu pair.result==0 (0x08139651) va copy ten A,B tu vector<string> +0x53c (0x08139668-0x081396B1). GetArenaBothSides 0x08104680 dung cung 0x08139610. timer.lua:10-20 moi tick: IsArenaBegin(i-1)==1 -> SubWorld 213+i-1, OpenMission(5) neu chua mo; citywar_arena/mission.lua:11-21 InitMission lay GetArenaBothSides roi bat timer; head.lua:143/147 NotifyArenaResult(ArenaID, 1/0). => Relay KHONG gui goi 'mo khe'; GS tu mo dua tren du lieu SDB da dong bo (mang khe object 11 + ban ghi ket qua 13+) va tu dung lai cap bang cung Core.
- GS NotifyArenaResult 0x08122800 -> KCityWar::AddArenaResult 0x08058330(this, ArenaID, bAWin): city=this+0x1a8; pair=pairs[city][ArenaID]; pair.result!=0 -> 'AddArenaResult repeat error' (0x8250edc); record 37 byte {u16 A=pair.A, u16 B=pair.B, u8 result = (bAWin==1?1:2), 32 byte 0} (0x080583A9-0x080583E9); vcall vtable[0] (KCityWarDataGS vtable 0x08251160 slot0 = 0x080593A0)(this, city, &record, 0); roi log '%s %s %s' win/lose (0x8250f89). 0x080593A0: cap nhat/chen record vao vector +0x5fc (stride 0x25), danh dau khe thua high16=1 (0x080595F0-0x080595FC), goi BuildArenaPairs 0x0805A480(this,city,0) (0x08059610), va neu bKeep==0 gui goi xuong relay: buf 0x2b byte {u8 0x0a, u8 0x04, u32 city, 37 byte record} qua 0x080777A0(0x836ea80 relay-client, buf, 0x2b) (0x080594F3-0x08059553).
- Relay nhan ket qua: KCityWarDataRelay vtable 0x081DB580 slot0 = 0x0809D4F0 AddArenaResult(this, city, record, bKeep). bKeep!=0 (khi nap DB) -> chi goi Core 0x080A04A6 (0x0809D515-0x0809D530). Kiem tra: city.state==3 (0x0809D54F, loi PERIODNOTMATCH(3,state)); bArenaRunning, nArenaCity==city, A<N, B<N, result!=0 (0x0809D5AB-0x0809D628) neu sai -> log L_ARENA_RESULT_RECORD_ERROR (0x83899c8). Goi Core 0x080A04A6(this,city,record,0) (0x0809D66F) = danh dau thua + dung lai cap vong sau (+ tat bArenaRunning neu het). SDB write key 0x81db2a2 region=city+1 object=(so ban ghi)+0xc data=record 37 byte (0x0809D674-0x0809D750), roi object=0xc data=so ban ghi 4 byte (0x0809D755-0x0809D835). Tinh ten thang/thua (doi cho neu result==2, 0x0809D856-0x0809D86B). Neu bArenaRunning con 1: loa G_CITYWAR_NOTIFY_ARENA_RESULT (0x8389934) (thang, thua) (0x0809D881-0x0809DA21) -> GS tiep tuc vong sau tu dong. Neu =0 (het cay): loa G_CITYWAR_NOTIFY_ARENA_RESULT2 (0x8389938) (thang, thua, ten thanh) (0x0809DA2B-0x0809DBE0); challenger(city+0x46)=ten bang thang (0x0809DBF7-0x0809DCBF); city.state=4 (0x0809DCD5); nArenaCity=-1 (0x0809DCDC); moi bang bao danh SetCityWarState 0 (0x0809DD11-0x0809DD69); bang thach dau SetCityWarState 3 (0x0809DD7C-0x0809DD90); SDB write object0 bArenaRunning, object1 nArenaCity, region city+1 object2 state, object5 challenger 32 byte (0x0809DD95-0x0809E112); log L_ARENA_END (0x83899cc). Relay KHONG tu goi StartCityWar — do task script relay (citywar_startcitywar_4) goi sau.
- Goi relay->GS: SDB record, dung 0x0F/0x29 (relay dung goi tai 0x08106692: [0]=0x0F,[1]=0x29,[2..5]=region,[6..9]=object,[0xa]=keyLen,[0xb]=flags(bit5-7: 3=xoa),[0xc..0xd]=dataLen, key tai +0xf, data sau key; tong = 0xf+keyLen+dataLen). GS: bang proto 0x08269A0C[0x0F] -> 0x081DAAF0, sub 0x25..0x29 -> 0x081D9430: 0x29 -> 0x0820ACC0 -> 0x0820AC50 (kiem tra do dai, 'Sth. is wrong while receiving SDB Record data' 0x826baf8) -> 0x0820A460 so khoa: '城战-擂台赛目前阶段' -> KCityWarDataCore::OnGlobalData 0x08058490 (object0 -> byte +0x1a4 bArenaBegin, object1 -> int +0x1a8 nArenaCity); '城战-城区数据' -> OnCityData 0x08059660 (region 1..8 = city idx+1; bang nhay 0x08251128: obj0 int +0x1ac cityId, obj1 byte +0x1b0, obj2 byte +0x1b1 state, obj3 char[32] +0x1b2 chu thanh (dang ky vao 0x9780b60 qua 0x080FB030 = GetCityOwner), obj4 char[32] +0x1d2, obj5 char[32] +0x1f2 challenger, obj6/7/8 int +0x212/+0x216/+0x21a, obj10 chuoi ten bang cach nhau bang '\n' -> vector<string> +0x53c, obj11 mang u16 -> vector<DWORD> +0x65c roi BuildArenaPairs(this,city,0) (0x08059A00-0x08059A7F), obj12 so ban ghi ket qua (chi doi chieu, 'Arena Result Count Error'), obj>=13 -> vcall AddArenaResult(this,city,data,1) (0x08059800); flags>>5==3 voi obj 10 = xoa toan bo du lieu thanh (0x08059728-0x080597BA)).
- Ma trang thai (tu ma): city.state (obj2): 0 nghi, 1 dang bao danh, 2 da chot bao danh (co cay), 3 loi dai dang dien ra, 4 da co challenger (cho cong thanh), 5 cong thanh dang dien ra (StartCityWar 0x0809E3A1). Tong::CityWarState (0x080D5F28, 0..5): 2 = dang dau loi dai (StartArena), 0 = reset (het loi dai), 3 = challenger, 4 = dang cong thanh (StartCityWar 0x0809E352/0x0809E384 cho chu thanh va challenger).
- StartCityWar 0x0809E13C doi chung: 0x0809E16C challenger (city+0x46) rong -> log L_START_CITY_WAR_ERROR (0x83899d0). 0x0809E19E chu thanh (city+6, cung truong GetCityOwner 0x080FC21E doc tai 0x08104535) rong -> nhanh 0x0809E1AA: loa G_CITYWAR_NOTIFY_WAR_RESULT3 (0x8389948) (ten thanh, challenger) = challenger chiem thanh khong can danh. Co chu -> 0x0809E33C: SetCityWarState(chu,4), SetCityWarState(challenger,4), city.state=5, ghi SDB. (Ban Lua 4 tham so cua StartCityWar khong ton tai: chi 1 tham so.)

### cau_truc_du_lieu

KCityWarDataCore (dung chung relay/GS, relay lech -8 so GS vi GS co vptr+...; GS doi tuong toan cuc 0x0830A680, vtable GS 0x08251160 slot0=AddArenaResult 0x080593A0; relay vtable 0x081DB580 slot0=0x0809D4F0, slot1=0x080958D4, slot2=0x08095938): GS +0x1a4 (relay +0x19c) u8 bArenaRunning; GS +0x1a8 (relay +0x1a0) int nArenaCity (1..7, -1 khi nghi); GS +0x1ac (relay +0x1a4) CITY[8] stride 0x72: +0 int cityId, +4 u8 (obj1), +5 u8 state (obj2), +6 char[32] chu thanh (obj3), +0x26 char[32] (obj4, chua ro), +0x46 char[32] challenger (obj5), +0x66/+0x6a/+0x6e int (obj6/7/8); relay +0x58+idx*32 ten thanh (dung khi loa). Moi thanh 5 vector stride 12 (begin/end/cap): GS +0x53c (relay +0x530) vector<string> ten bang bao danh (obj10); GS +0x59c (relay +0x590) vector phu song song (relay dung de cat >16, chua ro noi dung); GS +0x5fc (relay +0x5f0) vector<ARENA_RECORD 0x25 byte {u16 A; u16 B; u8 result 1=A thang 2=B thang; char extra[32]=0}> (obj12 = dem, obj13+n = tung ban ghi); GS +0x65c (relay +0x650) vector<DWORD> mang khe cay loai truc tiep, size = luy thua 2 >= N, toi da 16: low16 = chi so bang trong vector ten (0xFFFF = trong/bye), high16 = 0 song / 1 vua thua / 2 bi loai (obj11 chi gui low16, 2 byte/khe); GS +0x6bc+city*4 (0x0830AD3C) int ArenaLevel[8] (0 = chua/het, 1.. = vong dang cho); GS +0x6dc (relay +0x6b0) vector<PAIR 5 byte {u16 A; u16 B; u8 result}> cap cua vong hien tai, chi so = ArenaID 0..7 (map 213+ArenaID). Relay +0x734 KLog, +0x96c con tro SDB (0x08106072 write / 0x08106974 / 0x081065DE). Goi GS->relay 0x0A/0x04: {u8 0x0A,u8 0x04,u32 city,ARENA_RECORD} 0x2b byte. Goi relay->GS SDB 0x0F/0x29: {u8 0x0F,u8 0x29,u32 region,u32 object,u8 keyLen,u8 flags,u16 dataLen,u8 ?, key[keyLen], data[dataLen]}; khoa GBK '城战-擂台赛目前阶段' (0x81db28e, region0 obj0 = bArenaRunning, region1 obj0 = nArenaCity) va '城战-城区数据' (0x81db2a2, region = cityIdx+1).

### chua_ro

- Noi dung chinh xac cua vector phu relay +0x590 / GS +0x59c va tieu chi sap xep trong 0x0809B736 khi >16 bang bao danh (co goi std::sort 0x080A12CA/0x080A1324 tren vector nay; chua dich nguoc so sanh) — chi anh huong truong hop >16 bang.
- Truong CITY +0x26 (obj4, char[32]) va +4 (obj1) chua xac dinh y nghia (co the la chu thanh truoc/ bang lien minh).
- Byte +0xe trong goi SDB 0x0F/0x29 chua doc ra y nghia; goi 0x0F/0x27 (handler GS 0x0820AD00, chi nhan flags>>5==0) co the la bien the 'ghi' khac, chua thay relay dung cho citywar.
- Ban relay cua BuildArenaPairs 0x080A00EA chi kiem chung qua tham chieu offset (0x650/0x5f0/0x6b0 tai 0x080A00BD-0x080A0501) va viec dung chung lop Core; chua doc tung lenh de xac nhan 100% giong GS 0x0805A480.
- 0x804c26c/0x804c3cc doan la time()/srand(); 0x804c54c la rand() (PLT, chua doi chieu ten).
- Relay co tu dong goi StartCityWar sau khi loi dai ket thuc hay khong: trong 0x0809D4F0 KHONG co; can doc task script relay (citywar_startcitywar_4.lua / citywar_head.lua) de xac nhan lich.


## gs:IsArenaBegin+BothSides+CityArea+Level


### ket_luan

5 hàm Lua Arena trên GS đều chỉ ĐỌC một đối tượng toàn cục duy nhất: `KCityWarDataGS` tại 0x0830A680 (tên lớp lấy từ chuỗi lỗi "KCityWarDataGS::AddArenaResult error" 0x8250EB0; ctor tại 0x08058830 đặt vtable 0x82511B0 rồi 0x8251168). Các trường Arena: cờ bắt đầu `bArenaBegin` = byte 0x0830A824 (this+0x1A4), thành đang đấu `nArenaCity` = int 0x0830A828 (this+0x1A8), vòng đấu `nArenaLevel[8]` = int[] 0x0830AD3C (this+0x6BC), danh sách cặp đấu vòng hiện tại `vecArenaPair[8]` = std::vector<{u16,u16,u8}> (5 byte/phần tử) tại 0x0830AD5C (this+0x6DC, stride 12), tên bang tra qua `vecSignUpTong[8]` = vector<std::string> tại 0x0830ABBC (this+0x53C). Đối tượng này KHÔNG do Lua GS ghi: nó được đồng bộ từ relay qua cơ chế "SDB/GlobalData" theo TÊN bản ghi: (1) "城战-擂台赛目前阶段" (0x826BB56, 20 byte) → handler 0x08058490 ghi bArenaBegin (key1=0, 1 byte) và nArenaCity (key1=1, 4 byte); (2) "城战-城区数据" (0x826BB6A) → handler `KCityWarDataGS::OnCityData` 0x08059660 với key1 = khe+1 (1..8), key2 = loại dữ liệu 0..0xC (0xA = danh sách bang báo danh, 0xB = bảng đấu u16[] → gọi bộ dựng nhánh 0x0805A480, >0xC = bản ghi kết quả 0x25 byte). Phía relay cùng chuỗi tên 0x81DB28E/0x81DB2A2 được ghi bằng SDB set 0x8106072 + 0x81065DE (vd 0x0809D21B ghi nArenaCity từ relay-this+0x1A0, 0x0809D2CF ghi cờ bắt đầu). GS chỉ tự ghi duy nhất qua NotifyArenaResult → `KCityWarDataGS::AddArenaResult` 0x08058330 → virtual 0x080593A0 → base 0x0805C540 (ghi vector kết quả +0x5FC, đánh dấu thua ở +0x65C, dựng lại nhánh).

### chi_tiet

- IsArenaBegin(nArenaID) 0x08101620: gettop>0 (0x8101632) && byte[0x830A824]!=0 (0x810163B) && nArenaCity=[0x830A828] trong 1..7 (0x8101644-50) && arg1 (lua_tonumber 0x810167B) <=7 (0x8101699) → gọi KCityWarData::GetArenaBothSides(this=0x830A680, city=nArenaCity, idx=arg, out1, out2) 0x8139610 (0x81016BB); có cặp hợp lệ → push 1 (0x81016C4) ngược lại push 0. Trả 1 giá trị.
- GetArenaBothSides(nArenaID) 0x08104680: cùng điều kiện (0x81046A1 cờ, 0x81046E8 city 1..7, 0x8104724 idx<=7) → 0x8139610 (0x8104746) → pushstring(out1) 0x810475A, pushstring(out2) 0x8104766; lỗi → push "","" + log "LuaGetArenaBothSides() error..." (0x825CF3C). Trả 2 chuỗi (Tong1=khe .a, Tong2=khe .b).
- GetArenaCityArea(nArenaID) 0x080FDFF0: chỉ kiểm arg<=7 (0x80FE02F) rồi push [0x830A828] nếu 1..7 (0x80FE050-6A), ngược lại push 0. KHÔNG dùng arg để tra cứu gì — mọi khe đấu đều thuộc cùng 1 thành nArenaCity.
- GetArenaLevel() 0x08104640: không đọc tham số; city=[0x830A828] phải 1..7 (0x810464E) → push int[0x830AD3C + city*4] (0x8104653, fild) = nArenaLevel[city]; lỗi → log "LuaGetArenaLevel() usage error..." (0x825CF18), trả 0 giá trị.
- GetArenaTargetCity() 0x080FBE20: push [0x830A828] nếu 1..7 (0x80FBE50), ngược lại push 0. Không tham số. Giống hệt GetArenaCityArea nhưng không cần arg.
- KCityWarData::GetArenaBothSides 0x08139610 (this, city<=7, idx>=0, char out1[0x20], char out2[0x20]): vec = this+0x6DC+city*12 (0x8139631-34), size = (end-begin)/5 (imul 0xCCCCCCCD 0x8139642) ⇒ phần tử 5 byte; idx<size; phần tử e = begin+idx*5; CHỈ hợp lệ khi e.flag(+4)==0 (0x8139651); out1 = strncpy(this+0x53C+city*12 vector<string>[e.u16@+0].c_str(), 0x20) (0x8139670-86), out2 = ...[e.u16@+2] (0x8139699-B1). Tên bang = chuỗi trong vector báo danh, chỉ số u16 là vị trí trong vector đó.
- Ctor KCityWarDataGS 0x08058830: this=0x830A680; khởi tạo 5 mảng 8 vector (stride 0xC, vòng 0x60): 0x830ABBC(+0x53C, vector<std::string>, dtor loop kiểm 0x82E89E0 tại 0x8058A05), 0x830AC1C(+0x59C), 0x830AC7C(+0x5FC), 0x830ACDC(+0x65C), 0x830AD5C(+0x6DC); byte 0x830A824=0 (0x8058976); int 0x830A828=0 (0x8058982); vòng 8 City stride 0x72 từ 0x830A82C: memset 0x72 byte (0x80589C7), City.idx=i (0x80589D9), City+0x66=-1, City+0x6A=-1 (0x80589DC/E2), nArenaLevel[i]=0 (0x80589E9 = 0x830AD3C+i*4); 0x830A820=0 (0x8058AF9); map rb-tree tại +0x740 (0x830ADC0, 0x8058B17-3F).
- Handler SDB "城战-擂台赛目前阶段" (chuỗi 0x826BB56, so 20 byte tại 0x820A664-72) → 0x820ABA0 gọi 0x08058490(this, flag, key1, key2, pData): flag>>5==3 → bỏ qua (0x80584A7, thao tác xoá/đặc biệt); key1==0 → byte[this+0x1A4] = *(u8*)pData (0x80584C8-CB) = bArenaBegin; key1==1 → int[this+0x1A8] = *(int*)pData (0x80584D8-DA) = nArenaCity; khác → log "CityWarData: unexpected data received....." (0x8250F0C).
- Handler SDB "城战-城区数据" (0x826BB6A, 14 byte, 0x820A678-86) → 0x820ABD8 gọi KCityWarDataGS::OnCityData 0x08059660(this, flag, key1, key2, pData, nLen): key1==0 → log "CityWarData: city count %d" (0x825110B); key1 1..8 → slot = key1-1 (0x805969E); key2>0xC → virtual[0] AddArenaResult(this, slot, pData, 1) (0x8059800-18, bản ghi 0x25 byte từ relay); bảng nhảy 0x8251128 key2: 0→City.idx(int, +0x1AC) 0x8059858; 1→City+4 (u8, +0x1B0) 0x8059870; 2→City+5 (u8, +0x1B1) 0x8059888; 3→City+6 char[0x20] (+0x1B2) + gọi 0x80FB030(0x9780B60, slot, str) 0x80598A0; 4→City+0x26 char[0x20]; 5→City+0x46 char[0x20]; 6→City+0x66 int; 7→City+0x6A int; 8→City+0x6E int; 9→không làm gì; 0xA→chuỗi tách bằng '\n' (0x8257FE2) đổ vào vector<string> +0x53C[slot] (0x8059997 gọi 0x805C2D0) và parse "%d" từng dòng vào vector<int> +0x59C[slot] (0x8059AD8-B5D) = danh sách bang báo danh; 0xB→u16[nLen/2] (0x8059A15 shr 1) đổ vào vector<u32> +0x65C[slot] (mỗi phần tử = u16 chỉ số bang, state=0) rồi gọi BuildArenaRound 0x0805A480(this, slot, 0) (0x8059A7F) = BẮT ĐẦU ĐẤU TRƯỜNG; 0xC→kiểm *(int*)pData == số bản ghi kết quả (0x8059820-3C, lỗi log 0x8250F38). flag>>5==3 && key2==0xA && !(flag&2) → XOÁ toàn bộ slot: nArenaLevel[slot]=0 (0x805973B), end=begin cho 5 vector (0x8059758-B4).
- BuildArenaRound 0x0805A480(this, city, bKeepFlag): đọc vector nhánh +0x65C[city] (u32: low16 = chỉ số bang, high16 = trạng thái 0 còn/1 thua/2 loại hẳn); n = size, cần n>=2; duyệt từng vòng (n/2, n/4 ... tới 1): end=begin (0x805A51B) rồi ghép từng cặp (bỏ qua phần tử state==2 hoặc 0xFFFF) thành {u16 a,u16 b,u8 flag} 5 byte push vào +0x6DC[city] (0x805A609-620 hoặc push_back 0x805A2F0), flag=0 nếu cả hai state 0, =1/2 nếu một bên đã thua; nArenaLevel[city] = số vòng hiện tại (0x805A62B: [this+0xC+(0x1AC+city)*4] = +0x6BC+city*4); kết thúc vòng: mọi phần tử state≠0 được đặt state=2 (0x805A69C add 0x20000); nếu duyệt hết mà không dừng → xoá cặp, level=0 và (bKeepFlag==0) bArenaBegin=0 (0x805A6D3-E7). Bản sao inline giống hệt nằm trong base AddArenaResult 0x805C6xx-0x805C97B.
- KCityWarDataGS::AddArenaResult(nArenaID, nWinner) 0x08058330 (gọi từ Lua NotifyArenaResult tại 0x81228A0): nArenaID>=0 (0x8058345), city=nArenaCity 1..7 (0x8058368), idx<size cặp (0x8058387), cặp phải flag==0 (0x805839F, nếu không log "repeat error" 0x8250EDC); dựng bản ghi 0x25 byte {u16 a=pair.a, u16 b=pair.b, u8 result = (nWinner>=1 ? 1 : 2) (sbb/not/+2 tại 0x80583B7-C1), 32 byte 0} rồi gọi virtual[0](this, city, rec, 0) (0x8058407 → 0x80593A0 → base 0x805C540); log "%s %s %s" tên bang a, "win"/"lose" (0x8250F85/0x82683FA), tên bang b (0x805844C). Base 0x805C540: tìm bản ghi trùng (a,b) trong +0x5FC (stride 0x25, imul 0x914C1BAD) → ghi đè nếu flag cũ==0, ngược lại push_back; result==2 → bên a thua (0x805C6B7/0x805C987), ngược lại bên b thua → đặt state=1 (+0x10000, 0x805C726) trong +0x65C; rồi dựng lại nhánh.
- Trạng thái City (stride 0x72) được Lua khác đọc cùng cách: NumOfSignUpTongs 0x80FDEF0 tính 0x830A820 + city*0x72 (0x80FDF5D-66, city 1..7 dùng TRỰC TIẾP làm khe) và kiểm City+5 (state) trong 1..3 (0x80FDF91-9E) rồi đọc +0x53C+city*12. ⇒ trong GS khe = CityID (1..7), khe 0 không dùng; còn handler sync dùng khe = key1-1 ⇒ relay gửi key1 = khe+1 (khe 0 = key1 1, key1 0 = thông điệp đếm thành).
- Phía relay (s3relay_y): chuỗi "城战-擂台赛目前阶段" 0x81DB28E được 10 chỗ tham chiếu (0x8095D3C/0x8095DC1 = nạp từ SDB vào relay-this+0x19C (byte) / +0x1A0 (int); 0x8097E35, 0x8098742, 0x809D21B/0x809D2DB, 0x809DDA1/0x809DE67 = ghi). Tại 0x809D22A-79: key{name,len 0x14,key1=1,key2=0} + data{ptr=relay-this+0x1A0, len 4} → 0x8106072 (SDB set) → 0x8106974 → 0x81065DE (phát tới GS); 0x809D2CF tiếp tục với key1=0 (cờ bắt đầu). Bản ghi "城战-城区数据" 0x81DB2A2 có ~80 chỗ tham chiếu 0x8095E41-0x809EABF (toàn bộ bộ ghi dữ liệu thành, không dịch ở nhiệm vụ này).

### cau_truc_du_lieu

// Toàn cục: KCityWarDataGS g_CityWarData @ 0x0830A680 (this). Kế thừa KCityWarData (vtable 0x82511B0, virtual[0]=AddArenaResult 0x805C540) → KCityWarDataGS (vtable 0x8251168, virtual[0]=0x80593A0).
struct ArenaPair {            // 5 byte, KHÔNG đệm (stride 5, imul 0xCCCCCCCD)
  uint16_t nTong1;            // +0  chỉ số trong vecSignUpTong[city]
  uint16_t nTong2;            // +2
  uint8_t  nFlag;             // +4  0 = cặp đang đấu (hợp lệ cho IsArenaBegin/GetArenaBothSides), 1/2 = một bên đã thua/đã có kết quả
};
struct ArenaResult {          // 0x25 = 37 byte (imul 0x914C1BAD)
  uint16_t nTong1, nTong2;    // +0,+2
  uint8_t  nResult;           // +4  1 = Tong1 thắng (Tong2 bị đánh dấu thua), 2 = Tong2 thắng
  uint8_t  reserved[0x20];    // +5  GS ghi 0; relay có thể gửi dữ liệu thêm
};
struct CityInfo {             // stride 0x72 = 114 byte, KHÔNG đệm
  int32_t  nIdx;              // +0x00 (sync key2=0)
  uint8_t  byA;               // +0x04 (key2=1)
  uint8_t  byState;           // +0x05 (key2=2) NumOfSignUpTongs đòi 1..3
  char     szName1[0x20];     // +0x06 (key2=3) chủ thành? → gọi 0x80FB030(0x9780B60, city, s)
  char     szName2[0x20];     // +0x26 (key2=4)
  char     szName3[0x20];     // +0x46 (key2=5)
  int32_t  nV6;               // +0x66 (key2=6) ctor = -1
  int32_t  nV7;               // +0x6A (key2=7) ctor = -1
  int32_t  nV8;               // +0x6E (key2=8)
};
class KCityWarData {
  void*    vtable;                         // +0x000 0x830A680
  int      nA,nB,nC,nD,nE,nF,nG;           // +0x004..+0x01C (0x830A684..0x830A69C, =0 trong ctor)
  // ... +0x020..+0x033 chưa rõ
  rb_tree  map1;                           // +0x034 (0x830A6B4 header, size +0x044)
  rb_tree  map2;                           // +0x04C (0x830A6CC header, size +0x05C)
  int      arr8x32[8][8];                  // +0x060 (0x830A6E0, 8 khối 0x20)
  int      arr7[7];                        // +0x164 (0x830A7E4), +0x160 0x830A7E0
  int      arr8[8];                        // +0x180 (0x830A800)
  int      nUnk1A0;                        // +0x1A0 (0x830A820)
  uint8_t  bArenaBegin;                    // +0x1A4 (0x830A824)  ← SDB "城战-擂台赛目前阶段" key1=0
  int32_t  nArenaCity;                     // +0x1A8 (0x830A828)  ← key1=1; GS dùng 1..7 làm KHE trực tiếp
  CityInfo city[8];                        // +0x1AC (0x830A82C) stride 0x72, khe = CityID, khe 0 không dùng
  std::vector<std::string> vecSignUpTong[8];   // +0x53C (0x830ABBC) stride 12 — tên bang báo danh (key2=0xA, tách '\n')
  std::vector<int>         vecSignUpId[8];     // +0x59C (0x830AC1C) — số "%d" đi kèm mỗi dòng
  std::vector<ArenaResult> vecArenaResult[8];  // +0x5FC (0x830AC7C) — kết quả (key2>0xC hoặc NotifyArenaResult)
  std::vector<uint32_t>    vecBracket[8];      // +0x65C (0x830ACDC) — low16 = chỉ số bang, high16 = 0 còn/1 thua/2 loại (key2=0xB u16[])
  int32_t  nArenaLevel[8];                     // +0x6BC (0x830AD3C) — vòng hiện tại 0=chưa/xong,1,2,...
  std::vector<ArenaPair>   vecArenaPair[8];    // +0x6DC (0x830AD5C) — cặp đấu vòng hiện tại; nArenaID Lua = chỉ số trong vector này
  rb_tree  mapX;                               // +0x740 (0x830ADC0, size +0x750)
};
// Số khe đấu: tối đa 8 (arg <=7 ở mọi hàm Lua), thực tế = số cặp vòng hiện tại = size(vecArenaPair[nArenaCity]).

### chua_ro

- Lệch chỉ số khe: hàm Lua/NumOfSignUpTongs dùng CityID (1..7) TRỰC TIẾP làm khe (0x80FDF5D, 0x8139631), còn OnCityData 0x8059660 dùng key1-1 (0x805969E). Suy ra relay gửi key1 = CityID+1 hoặc relay đánh số thành 0..7 ở key1 rồi +1 — chưa đối chiếu mã ghi phía relay (0x8095E41…).
- Ý nghĩa chính xác các trường CityInfo +4/+5 (byA/byState), 3 chuỗi 0x20 byte và 3 int +0x66/+0x6A/+0x6E chưa xác nhận (chỉ biết state 1..3 hợp lệ cho báo danh; hàm 0x80FB030 nhận szName1 cần dịch riêng).
- Nghĩa của ArenaPair.nFlag = 1 vs 2 khi dựng nhánh (0x805A5EF/0x805A740) chỉ suy ra: 2 = bên thứ hai có state≠0, 1 = bên thứ nhất; chưa thấy code nào phân biệt 1/2 ngoài '≠0 = không đấu'.
- nResult của NotifyArenaResult (1 = Tong1 thắng khi nWinner>=1) suy từ sbb/not/+2 tại 0x80583B7-C1; cần đối chiếu với thân Lua NotifyArenaResult 0x8122800 để chắc tham số thứ 2 là gì (1/2 hay 0/1).
- Cờ flag>>5==3 và flag&2 của gói SDB (0x80584A7, 0x8059684, 0x8059736) = loại thao tác (xoá/đồng bộ đầu) — chưa đọc bộ nhận SDB 0x820A460 đủ sâu để đặt tên.
- Các trường +0x004..+0x033, 2 map rb-tree +0x34/+0x4C, mảng +0x60/+0x164/+0x180 và map +0x740 của KCityWarData chưa dịch (không thuộc nhóm Arena).
- Relay: mới xác nhận 0x809D21B/0x809D2CF là nơi ghi SDB cho nArenaCity/bArenaBegin; chưa lần tới Lua StartArena 0x080FF3C6 để biết giá trị city/cờ lấy từ đâu (thuộc nhiệm vụ relay).


## gs:TotalLevel+Schedule+Info+Notify


### ket_luan

Cả 5 hàm đều là lớp vỏ Lua bọc đối tượng toàn cục KCityWarDataGS tại 0x830a680 (typeinfo '14KCityWarDataGS' 0x825116c; vtable 0x8251168 chỉ có 1 hàm ảo = AddArenaResult 0x80593a0). Không có chu kỳ thời gian nào nằm trong 5 hàm này — chúng chỉ ĐỌC/GHI bộ nhớ cục bộ của GS; nhịp báo danh→bốc thăm→thi đấu do relay điều khiển (StartSignUp/EndSignUp/StartArena, ngoài phạm vi nhiệm vụ). 'Total level' = floor(log2(số bang đã báo danh của thành)) = số vòng loại trực tiếp (8 bang→3, 4→2, 2→1, 3→1, 5..7→2). GetArenaSchedule trả về MỘT chuỗi ghép từ stringtable G_CITYWARDATA_0..7 (lang/vn/stringtable_core.txt:1035-1042): tiêu đề + 'Hai bên quyết đấu:'/'Trận bán kết:'/'Lượt thi đấu thứ %u:' + từng cặp '%s đấu với %s' hoặc '%s thắng %s'. NotifyArenaResult(nArenaID, bSide1Win): nArenaID là CHỈ SỐ TRẬN trong lịch vòng hiện tại của thành mình; gọi AddArenaResult → ghi bản ghi 37 byte vào vector kết quả cục bộ, đánh dấu bang thua (|0x10000) trong vector báo danh, dựng lại lịch vòng kế, RỒI gửi gói 43 byte lên relay qua KRelayClient 0x836ea80: {BYTE 0x0A; BYTE 4; DWORD nCity; WORD idxA; WORD idxB; BYTE result (1=A thắng, 2=B thắng); BYTE pad[32]=0}. Cùng họ gói 0x0A sub 2/3/5/6 là các gói CityWar khác của GS gửi relay.

### chi_tiet

- [GetArenaTotalLevel 0x08129510] không nhận tham số; nCity = [0x830a828] (= this+0x1a8 của KCityWarDataGS = chỉ số thành của chính GS này); nếu nCity ngoài 1..7 (0x0812951E cmp edx,6) → puts('LuaGetArenaTotalLevel() usage error...' 0x825d5b4), trả 0. Ngược lại: count = (end-begin)/4 của vector<int> tại 0x830acdc+nCity*12 (= this+0x65c+city*12 = danh sách bang báo danh); vòng lặp 0x08129558: while(count>1){count>>=1; lvl++} ⇒ lvl = floor(log2(count)); lua_pushnumber(lvl) tại 0x08129572.
- [GetArenaTotalLevelByCity 0x0812B880] giống hệt nhưng nCity = lua_tonumber(1) (0x0812B8A1), kiểm 1..7 (0x0812B8C2), lỗi → 'LuaGetArenaTotalLevelByCity() usage error...' 0x825d648; cùng vector 0x830acdc+city*12, cùng công thức floor(log2) (0x0812B900).
- [GetArenaSchedule 0x08122000] nCity = tonumber(1), kiểm 1..7 (0x08122062). nLevels = floor(log2(số bang báo danh)) (0x08122088). matches = vector bản ghi 5 byte tại 0x830ad5c+city*12 (this+0x6dc; imul 0xCCCCCCCD = chia 5, bước +5 tại 0x08122266); names = vector<std::string> tại 0x830abbc+city*12 (this+0x53c; phần tử 4 byte = con trỏ chuỗi, dùng thẳng làm %s); curLevel = [0x830ad3c+city*4] (this+0x6bc). Nếu matches rỗng HOẶC nLevels==0 (0x081220B0/0x081220B8) → pushstring(G_ScriptFuns_2 = 'Lôi đài trong bổn thành hiện vẫn chưa tiến hành thi đấu', con trỏ 0x978a458, nạp tại 0x08180B47). Ngược lại: strncpy(buf, G_CITYWARDATA_0 '< Vòng đấu Lôi Đài mới nhất >\n', 3) (0x081220C5 — n=3, chỉ chép 3 ký tự, quirk/bug gốc); nếu curLevel==nLevels → strcat G_CITYWARDATA_1 'Hai bên quyết đấu: ' (0x081223A3); nếu curLevel==nLevels-1 → strcat G_CITYWARDATA_2 'Trận thi đấu bán kết: ' (0x0812238D); còn lại sprintf(G_CITYWARDATA_3 'Lượt thi đấu thứ %u: ', curLevel) (0x0812215A). Rồi duyệt từng bản ghi {WORD A; WORD B; BYTE result}: A,B phải < names.size() (0x081222E1/0x081221EA, không thì dừng); result==0 → sprintf(G_CITYWARDATA_4 '%s đấu với %s', names[A], names[B]) (0x08122320); result==1 → G_CITYWARDATA_5 '%s thắng %s' với (names[A], names[B]) (0x08122340); result khác (=2) → (names[B], names[A]) (0x0812221B). Giữa các bản ghi strcat G_CITYWARDATA_7 (0x0812224C, con trỏ 0x978aeb0) — trong bản VN chuỗi này là '<<未翻译>>' chưa dịch (stringtable_core.txt:1042), chứng tỏ hàm này chưa từng chạy trên VN. Kết thúc lua_pushstring(buf) 0x08122383. Bảng con trỏ chuỗi nạp tại 0x08184B43..: 0x978ae94=_0, ae98=_1, ae9c=_2, aea0=_3, aea4=_4, aea8=_5, aeac=_6, aeb0=_7, aeb4=_8 (thứ tự 'setup key → call → lưu eax vào slot kế').
- [GetArenaInfoByCity 0x08121D00] chỉ đọc tham số 1 (nCity, 1..7; lỗi 'LuaGetArenaInfoByCity() usage error...' 0x825d420) — tham số ArenaLevel ghi trong camper.lua:17 bị binary BỎ QUA. Vector kết quả = bản ghi 37 byte (0x25; imul 0x914C1BAD = chia 37; bước +0x25 tại 0x08121EDA) tại 0x830ac7c+city*12 (this+0x5fc). Nếu (end-begin)<=0x24 (chưa có bản ghi nào, 0x08121D91) → strncpy(buf, G_CITYWARDATA_8 'Thi đấu vẫn chưa tiến hành...', 0x1ff) và trả. Ngược lại sprintf(buf, G_CITYWARDATA_6 '<< Khu vực thành %s >>\n', 0x830a6e0+city*32) (tên thành char[32]/thành tại this+0x60); duyệt bản ghi {WORD A; WORD B; BYTE result; BYTE extra[32]} với cùng quy tắc result 0/1/2 như trên (0x08121F28/0x08121F78/0x08121E6B), ngăn cách G_CITYWARDATA_7; nếu buffer 0x200 còn dưới 0x31 byte (0x08121E3B) → pushstring(G_ScriptFuns_3 'Khu vực trong bổn thành gần đây không có trận thi đấu Lôi đài nào', 0x978a45c).
- [NotifyArenaResult 0x08122800] cần ≥2 tham số (0x0812281A); nArenaID = tonumber(1) phải ≤7 (0x08122848, không kiểm âm), nResult = tonumber(2); lỗi → 'LuaNotifyArenaResult() usage error...' 0x825d448; gọi 0x08058330(this=0x830a680, nArenaID, nResult); luôn trả 0 giá trị Lua.
- [KCityWarDataGS::NotifyArenaResult 0x08058330] city = [this+0x1a8] (0x08058347), phải ≤7; nArenaID phải < số bản ghi 5 byte của lịch hiện tại this+0x6dc+city*12 (0x08058387) — tức nArenaID là CHỈ SỐ TRẬN trong vòng hiện tại (khớp ArenaID 0..7 = WorldID-213). Nếu bản ghi đã có result≠0 → 'KCityWarDataGS::AddArenaResult repeat error !!!' (0x8250edc), lỗi khác → 'KCityWarDataGS::AddArenaResult error !!!' (0x8250eb0). Dựng bản ghi 37 byte trên stack: WORD A=rec.A, WORD B=rec.B, BYTE result = (nResult>=1)?1:2 (sbb/not/add 2 tại 0x080583B7-0x080583BE: bSide1Win khác 0 → 1 = bên A thắng; 0 → 2 = bên B thắng), 32 byte 0; gọi hàm ảo [vtable+0](this, city, &rec, 0) = AddArenaResult 0x80593a0; rồi printf('%s %s %s\n', names[A], 'win'/'lose', names[B]) (0x0805844C; 'win' 0x8250f85, 'lose' 0x82683fa).
- [AddArenaResult 0x080593A0 (this, city, rec37, bFromRelay)] tìm trong vector 37 byte this+0x5fc+city*12 bản ghi cùng (A,B) có result==0 → chép đè 37 byte (0x08059447); nếu không tìm thấy → push_back (0x08059591/0x0805963D). Sau đó: bên thua = (result==2 ? A : B) (0x08059493/0x08059620); tìm trong vector báo danh this+0x65c+city*12 (vector<int>, low16 = chỉ số bang trong names, high16 = trạng thái) phần tử có low16 == bên thua và ghi = low16|0x10000 (0x080595F6 — trạng thái 1 = vừa thua); gọi 0x0805A480(this, city, bFromRelay) dựng lại lịch. Nếu bFromRelay≠0 → về (0x080594ED). Nếu ==0 (gọi từ Lua) → gói {BYTE 0x0A; BYTE 0x04; DWORD city; 37 byte rec} = 0x2B byte (0x080594F6-0x0805953A) gửi bằng 0x080777A0(obj 0x836ea80, &pkt, 0x2B) — obj 0x836ea80 là client nối relay (77 tham chiếu; cùng hàm gửi dùng cho mọi gói CityWar: sub 6 dài 0x27 tại 0x08058539/0x08058599, sub 5 dài 7 tại 0x080585F8, sub 2 dài 0x20 tại 0x0805CABB, sub 3 dài 0x22 tại 0x0805CF1F).
- [Dựng lịch 0x0805A480 (this, city, bFromRelay)] N = số phần tử vector báo danh; nếu N<=1 về. Duyệt các vòng với cỡ nhóm g = N, N/2, N/4... (sar [ebp-0x54] tại 0x0805A6B4), mỗi vòng ghi bản ghi 5 byte vào lịch this+0x6dc: A = phần tử 'còn sống' đầu tiên của nhóm chẵn, B = của nhóm lẻ (sống = low16≠0xFFFF và high16≠2, 0x0805A56B-0x0805A578); result = 2 nếu A có high16≠0 (A thua), 1 nếu B thua, 0 nếu chưa đấu (0x0805A5DF-0x0805A5EF, 0x0805A700); ghi level hiện tại vào this+0x6bc+city*4 (0x0805A62B); sau mỗi vòng không còn trận chờ, mọi bang có high16≠0 được nâng lên trạng thái 2 (|0x20000, 0x0805A69C = loại hẳn) và tiếp vòng sau; gặp vòng còn trận result==0 thì dừng — đó là vòng hiện tại. Nếu chạy hết mọi vòng → curLevel=0 (0x0805A6DA) và nếu bFromRelay==0 → byte this+0x1a4 = 0 (0x0805A6E7).
- [Ý nghĩa 'level'] theo camper.lua:13-14 (GBK): GetArenaLevel = vòng hiện tại (0 = đã kết thúc hoặc chưa bắt đầu, 1 = vòng 1, ...), GetArenaTotalLevel = tổng số vòng. Kết hợp với binary: curLevel tại this+0x6bc+city*4 (=0x830ad3c+city*4) đếm từ 1, trận chung kết khi curLevel==floor(log2(N)).
- [Script gọi] head.lua:143/147 WinBonus(camp): NotifyArenaResult(GetMissionV(MS_ARENAID), 1 hoặc 0); infocenter_head.lua:478 ArenaInfo(): Say(GetArenaSchedule(nCityId)) (ghi chú '废弃 2006-11-22' = bỏ từ 2006); infocenter_head.lua:487 GetArenaInfoByCity đã bị comment. Không script nào gọi GetArenaTotalLevel/ByCity.

### cau_truc_du_lieu

KCityWarDataGS g_CityWarData @ 0x830a680 (typeinfo 0x8251180, vtable 0x8251168: [0]=AddArenaResult 0x80593a0). Offset suy ra (city = 0..7, mảng 8 phần tử, chỉ số 1..7 hợp lệ với Lua):
  +0x060  char szCityName[8][32]           (0x830a6e0 + city*32; dùng %s trong GetArenaInfoByCity 0x08121D9A)
  +0x1a4  BYTE  cờ (đặt 0 khi lịch dựng xong do GS tự báo; 0x0805A6E7)
  +0x1a8  int   nMyCity                      (0x830a828; GetArenaTotalLevel/NotifyArenaResult)
  +0x53c  std::vector<std::string> names[8]  (0x830abbc + city*12; phần tử 4 byte = char*; chỉ số = 'tong index' trong mọi bản ghi)
  +0x5fc  std::vector<ARENA_RESULT> results[8] (0x830ac7c + city*12) — ARENA_RESULT 37 byte: {WORD idxA; WORD idxB; BYTE result (0 chưa đấu,1 A thắng,2 B thắng); BYTE extra[32]=0}
  +0x65c  std::vector<int> signup[8]         (0x830acdc + city*12) — low16 = tong index (0xFFFF = ô trống/bye), high16 = trạng thái: 0 còn sống, 1 vừa thua (|0x10000), 2 đã loại (|0x20000)
  +0x6bc  int   curLevel[8]                  (0x830ad3c + city*4; 1-based; 0 = chưa/đã xong)
  +0x6dc  std::vector<ARENA_MATCH> schedule[8] (0x830ad5c + city*12) — ARENA_MATCH 5 byte: {WORD idxA; WORD idxB; BYTE result}; chỉ số phần tử = nArenaID (0..7) = map 213+i
Gói GS→relay (hàm gửi 0x080777A0, đối tượng relay client 0x836ea80): protocol 0x0A, sub 4 'ArenaResult' = 1+1+4+37 = 43 byte: {BYTE 0x0A; BYTE 4; DWORD nCity; ARENA_RESULT}. Các sub cùng họ thấy được: 2 (0x20 B), 3 (0x22 B), 5 (7 B), 6 (0x27 B, có cờ 1/0).
Chuỗi: con trỏ stringtable BSS 0x978ae94.. = G_CITYWARDATA_0..; 0x978a458 = G_ScriptFuns_2, 0x978a45c = G_ScriptFuns_3 (nạp qua 0x8188700 từ lang/vn/stringtable_core.txt). PLT: 0x804b08c strncpy, 0x804af8c sprintf, 0x804ac7c strcat, 0x804b47c puts, 0x804b41c printf.

### chua_ro

- Phía relay xử lý gói 0x0A/4 ra sao (có phát lại cho các GS khác, có lưu DB không) — chưa dịch ngược s3relay_y trong nhiệm vụ này; đường ngược (relay → GS) gọi AddArenaResult với bFromRelay≠0 cũng chưa tìm hàm nhận.
- Chu kỳ thời gian (giờ báo danh / giờ bốc thăm / giờ thi đấu) không nằm trong 5 hàm này; cần dịch StartSignUp/EndSignUp/StartArena trên relay và citywar.ini.
- Ý nghĩa 32 byte extra trong ARENA_RESULT: GS luôn ghi 0; có thể relay điền (thời gian/ghi chú) — chưa xác định.
- Hàm 0x0805A480 dựng lịch: cách ghép cặp khi N không phải lũy thừa 2 (ô 0xFFFF = bye) đã thấy nhưng chưa mô phỏng đầy đủ mọi nhánh (0x0805A71F gọi 0x805a2f0 = push_back vào schedule chưa đọc); trận bye được ghi result tự động 1/2 hay bỏ qua chưa chốt.
- strncpy(buf, G_CITYWARDATA_0, 3) trong GetArenaSchedule: không rõ là bug gốc hay chủ ý (chỉ chép 3 byte tiêu đề); bản CN gốc của _0 có thể khác.
- Ai nạp vector signup/names trên GS (nhận từ relay khi EndSignUp?) — 14 hàm luamap và gói 0x0A sub khác chưa đối chiếu; GS không có SignUpCityWarArena nên nguồn dữ liệu duy nhất còn lại phải là gói relay→GS.


## gs:SignUpTongs+AppointChallenger+TongCamp


### ket_luan

Trên GS (jx_linux_y) toàn bộ nhóm hàm này là mặt ngoài của một đối tượng toàn cục KCityWarDataGS tại 0x0830A680 (vtable 0x082511B0, ctor 0x08058830). GS KHÔNG tự sinh dữ liệu: bảng 8 thành (stride 0x72) + 8 vector<string> "bang báo danh" + 8 vector<int> phí + dữ liệu lôi đài đều được relay đẩy sang qua kênh dữ liệu-có-tên "城战-城区数据" (GBK, 0x0826BB6A; bộ nhận 0x0820A460 → KCityWarDataGS::OnData 0x08059660, bảng nhảy 13 subcmd tại 0x08251128: 0=index,1=byte,2=STATE,3=tên chủ thành,4=tên thái thú(viceroy),5=tên KHIÊU CHIẾN GIẢ,6/7/8=int,10=DANH SÁCH BANG BÁO DANH (chuỗi tách '\n', mỗi tên ≤0x40) + phí, 11=bảng đấu lôi đài (u16[]), 12=kiểm số kết quả). Key1 của gói = slot+1 (GS trừ 1), Lua city 1..7 = slot. Chiều GS→relay đi bằng gói nhị phân byte0=0x0A ("CityWar") qua KRelayClient 0x0836EA80 (+0x38 Send 0x080777A0): cmd1=BÁO DANH (0x0805CA10, 0x4A byte: city@+2, tên người chơi[32]@+6, tên bang[32]@+0x26, phí@+0x46), cmd5=đổi state (0x080585B0), cmd6=Appoint (0x0805 84F0/8550, 0x27 byte: flag@+2 1=Challenger/0=Viceroy, cityIdx@+3, tên bang[32]@+7). Relay nhận tại 0x0809EC68 (bảng 0x081DB374: 1→SignUp 0x0809AE32, 2→0x0809E78E, 4→AddArenaResult 0x0809D4F0 (đòi state==3), 5→0x0809E65C, 6→0x0809EE40 → AppointViceroy 0x08098F44 / AppointChallenger 0x08098FA8). AppointChallenger trên GS CHỈ GỬI RELAY (không ghi gì local, trả 0 giá trị); relay mới ghi challenger vào city[idx]+0x4A, đồng bộ lại GS (key2=5) và XÓA SẠCH danh sách báo danh/phí/kết quả lôi đài của thành (0x080988C4). GetCityWarTongCamp tra map<string,{int camp; char[32]}> tại this+0x740 do relay bơm bằng gói relay→GS 0x0A/0x03 (0x0809A954) — camp = TONG task value 2 (= TONG_GetCurCamp), KHÔNG phải 1 thủ/2 công. Đặc biệt: thân hàm LuaSignUpCityWar VẪN CÒN trong GS tại 0x08121B50 (chuỗi "LuaSignUpCityWar() usage error..." 0x0825D3FC) nhưng không có xref/không đăng ký tên → chết; script CN tự ghi "(废弃2006-11-22)" tại infocenter_head.lua:98/115 — tức bản CN gốc cũng đã bỏ đường báo danh này, thay bằng lệnh bài/League.

### chi_tiet

- [GS] NumOfSignUpTongs 0x080FDEF0: gettop>0; city=tonumber(1) phải 1..7 (0x080FDF55-5B); copy struct thành (0x0830A820+0x72*city, +0xC.. → local) rồi kiểm byte STATE (S+5) ∈ {1,2,3} (0x080FDF91-9E `sub 1; cmp dl,2; ja`) → trả size vector<string> tại 0x0830ABBC+12*city (0x080FDFA0-B1, sar 2 = sizeof(std::string*)=4); state khác → 0. Chữ ký: (nCity 1..7) → số.
- [GS] GetSignUpTongName 0x0812C690: gettop>1; city 1..7; idx=tonumber(2) (0x0812C728); idx<0 hoặc idx>=size → trả chuỗi static rỗng (guard 0x0830A640 / static string 0x09780D74 khởi tạo từ "" 0x08250931, `__cxa_guard_acquire` 0x0804B62C); hợp lệ → copy phần tử vector<string>[idx] (0x0812C780-9C) → pushstring. KHÔNG kiểm state. Chữ ký: (nCity, nIdx 0-based) → tên bang.
- [GS] Kho dữ liệu: ctor KCityWarDataGS 0x08058830: this=0x0830A680; 5 mảng 8 vector tại this+0x53C (=0x0830ABBC, tên bang báo danh), +0x59C (0x0830AC1C, int phí/đấu thầu - 0x08059 9B0-F5: assign(size, this+0xC=SignUpFee) rồi parse %d), +0x5FC (kết quả lôi đài 0x25 byte/phần tử, 0x080593C0+), +0x65C (u16 bảng đấu), +0x6BC; bảng thành 8 slot stride 0x72 tại this+0x1AC (0x0830A82C): +0 int idx, +4 byte, +5 byte STATE, +6 char[32] chủ thành, +0x26 char[32] thái thú, +0x46 char[32] khiêu chiến giả, +0x66/+0x6A/+0x6E int; map<string,int> tên thành→idx tại this+0x4C (GetCityIndexByName 0x08139BF0); map<string,{int camp;char[32]}> tại this+0x740 (tra 0x08058BB0, ghi 0x08058D40).
- [GS] Bộ nhận dữ liệu từ relay: KCityWarDataGS::OnData 0x08059660 (this, byte flag, int key1, int key2, data, len): key1 phải 1..8 → slot=key1-1 (0x08059695-9E; 0 → in "CityWarData: city count %d"); key2 0..12 nhảy bảng 0x08251128: 0→0x08059858 idx, 1→0x08059870 byte+4, 2→0x08059888 STATE byte+5, 3→0x080598A0 strncpy 32 chủ thành +6 rồi gọi 0x080FB030(bảng thuế 0x09780B60, city, tên) đồng bộ chủ thành cho hệ thuế, 4→0x080598E0 thái thú +0x26, 5→0x08059908 khiêu chiến giả +0x46, 6/7/8→int +0x66/+0x6A/+0x6E, 10→0x08059978 tách chuỗi theo '\n' (0x08257FE2) mỗi tên ≤0x40 (0x08059AF7) push_back vector +0x53C rồi phần còn lại parse "%d" vào +0x59C, 11→0x08059A00 u16[] vào +0x65C rồi 0x0805A480 dựng lịch đấu, 12→0x08059820 so hash số kết quả ("Arena Result Count Error"). flag>>5==3 (0x08059728) = XÓA slot (key2 phải 0xA). Người gọi: 0x0820A460 — bộ nhận kênh dữ-liệu-có-tên của relay, so tên "城战-城区数据" (0x0820A678-86) / "城战-擂台赛目前阶段" (→0x08058490, kết quả lôi đài) / "GoldBoss" / "Lottery*".
- [RELAY] Nguồn danh sách báo danh: KCityWarDataRelay (this+0x1A0 bảng thành stride 0x72: +8 byte, +9 STATE, +0xA chủ thành, +0x2A thái thú, +0x4A khiêu chiến; vector +0x534 tên báo danh, +0x594 phí, +0x5F4 kết quả lôi đài, +0x654 bảng đấu, +0x6D4). SignUp 0x0809AE32 (gọi từ handler cmd1 0x0809ED36): tra bang (0x080D6554), người gửi phải là bang chủ (so tongInfo+0x64, 0x0809AEC4-E4), STATE thành phải ==1 (0x0809AF1E-24), bang chưa báo danh ở thành nào (duyệt +0x534 các thành, 0x0809AF46-B02E, nếu thành đó state 1..3 → từ chối), không phải chủ thành đó; gửi camp bang sang GS (0x0809AEC4→0x0809A954). EndSignUp cắt còn tối đa 16 bang theo phí (0x0809B736: size>0x10 → sort theo +0x594, gọi từ 0x0809C0BA). Mọi ghi đều kèm SetData kênh "城战-城区数据" key1=slot+1, key2 (bảng 84 điểm ghi: 0x08096D48 key2=0xA danh sách, 0x08096EE1 0xB, 0x08096FD3 0xC, 0x08098999 0xA trong ClearCity 0x080988C4).
- [GS] AppointChallenger 0x0812CD00: gettop==2 bắt buộc (0x0812CD17), arg1 tên thành, arg2 tên bang, cả hai non-empty & strlen≤0x1F (0x0812CD4A-6E), tên thành → idx qua map this+0x4C (0x0812CD89 call 0x08139BF0; sai → puts "AppointChallenger() usage error!" 0x0825D678); rồi 0x080584F0(this, idx, tên): dựng gói {0x0A,0x06,flag=1,int idx,char[32]} 0x27 byte, strncpy 0x1F, gửi relay 0x0836EA80 (0x08058539). Trả 0 giá trị; KHÔNG đổi state local. AppointViceroy 0x0812CDD0 giống hệt nhưng flag=0 (0x08058550) và chấp nhận arg1 là số 1..7 (0x0812CED8-CF09) hoặc tên.
- [RELAY] Nhận cmd6 0x0809EE40: len==0x27, flag≤1, idx 0..7; bảng con trỏ thành viên 0x08295D58: flag0→0x0809EF24→AppointViceroy 0x08098F44 (1..7, gọi 0x08099110), flag1→0x0809EEFA→AppointChallenger 0x08098FA8: tra bang tồn tại (0x08098FCA call 0x080D6554; không → log "AppointChallenger error...." 0x081DB330), strncpy 32 vào this+0x1E0+0x72*idx+0xA (= city[idx]+0x4A khiêu chiến), SetData key("城战-城区数据", idx+1, 5) ptr/len 0x20 (0x08099040-8D), thất bại → log "save citywar data[region:%d object:%d] failed!"; sau đó 0x08098F2F → 0x080988C4(this, idx): clear +0x534/+0x594/+0x654/+0x5F4/+0x6D4, đặt this+0x6B0+4*idx... =0 và xóa các key 0xA,... trong kho → GS nhận flag>>5==3 xóa slot. Tức: AppointChallenger = 'chốt' khiêu chiến giả và HỦY toàn bộ báo danh/lôi đài của thành (đúng đường VN bốc thăm 19h: citywar_head.lua:124-138 GetRandomChallenger → GlobalExecute dwf citywar_function.lua citywar_appointchallenger → AppointChallenger).
- [GS] GetCityWarTongCamp 0x081228C0: gettop>0, arg1 tostring → 0x08058BB0(this, tên, &out): tìm trong std::map<std::string, {int camp; char sz[32]}> tại this+0x740 (so sánh chuỗi 0x08058C14-3F); thấy → pushnumber(out.camp) (0x08122920); không thấy → puts "LuaGetCityWarTongCamp() usage error..." + lua_pushnil (0x08122910 call 0x08232E70). Map được ghi bởi handler relay→GS proto 0x0A sub 3: 0x0805CE10 (bảng byte1: 1→0x0805CDA0 gửi thẳng người chơi theo id +2, 2→0x0805CB70 msg2player, 3→0x0805CB30 → 0x08058D40(this, tên@+2, data@+0x22)); dispatcher relay-packet byte0 bảng 0x08269A0C: 0x0A→0x081DAA50. Relay gửi bằng 0x0809A954(this, tênBang, tongInfo): header 0x0809EFF0 {0x0A, 0x03}, tên[32]@+2, int camp@+0x22 = tongInfo[0] = TONG task value 2 (0x080D65DC→0x080D7CAE→0x080C74AA(obj,2) — chính là hàm TONG_GetCurCamp 0x08147E90 dùng), chuỗi 32 byte@+0x26 = tongInfo+0x84; len 0x46, phát tới mọi GS (0x080E4B7C với danh sách this+0x968). Gọi khi: bang báo danh (0x0809AEC4) và vòng đồng bộ 0x0809AA1A (chủ thành mọi thành + mọi bang trong vector báo danh). ⇒ Camp = phe hiện tại của bang trong kho Tong của relay, chỉ có cho các bang liên quan công thành; script VN hiện KHÔNG gọi hàm này (chỉ có dòng chú thích camper.lua:8).
- [GS] GetCityWarBothSides 0x080FFDA0: city 1..7; copy struct; cần CẢ HAI S+6 (chủ) và S+0x46 (khiêu chiến) non-empty (0x080FFE3E-4B) → push (khiêu chiến = [ebp-0x44]=S+0x46) TRƯỚC, (chủ = [ebp-0x84]=S+6) SAU (0x080FFE88-A4); thiếu → "","" (0x080FFE50). Tức (CÔNG, THỦ) — khớp chú thích camper.lua:6 và chiều đảo ở citywar_city/mission.lua:86. Hàm kế 0x080FFEB0 = GetCityAreaName (bảng 0x0830A6E0 stride 0x20).
- [GS] HaveBeginWar 0x08104770: city 1..7; copy struct; trả 1 khi STATE byte S+5 == 5 (0x08104808-0F, 0x08104856-5D), khác → 0; sai tham số → 0 (pushnumber 0 tại 0x08104811). Vậy máy trạng thái byte STATE (relay ghi qua key2=2): 1=đang báo danh (SignUp đòi ==1), 2=hết báo danh, 3=lôi đài (AddArenaResult đòi ==3), 5=đang công thành; NumOfSignUpTongs chỉ trả số khi 1..3.
- [GS] Hàm báo danh gốc LuaSignUpCityWar 0x08121B50 (KHÔNG được đăng ký: xref 0x08121B50 = rỗng; chuỗi "SignUpCityWarArena"/"SignUpCityWar" không tồn tại trong GS — --find "SignUp" chỉ ra "SignUpFee"): gettop>1; lấy player (0x08107860), tên người; city=tonumber(1) 1..7; STATE S+5 phải ==1 (0x08121C7E); fee=tonumber(2) → KCityWarDataGS::SignUp 0x08058650(this, city, playerIdx, fee): city≤7, fee ≥ this+0xC (SignUpFee từ citywar.ini, 0x0805867B), strcmp(tên người, tên bang chủ trong khối bang của người chơi +0x5A90)==0 (0x080586A1) nếu khác → printf "%s over oper signup.\n" (0x08250F93) + Msg2Player 0x08058600 với chuỗi 0x0978A54C; đạt → 0x0805CA10 gửi relay cmd1. Script gọi nó: infocenter_head.lua:118 `SignUpCityWarArena(CityID, Fee)` trong SignUpFinal, và dòng 98/115 ghi '(废弃2006-11-22)' → CN gốc đã bỏ từ 2006.
- [DỰ ÁN KJx2CityWar.cpp] Đã có (khác cơ chế, chạy trong GS, không relay): LuaGetCityWarBothSides (:407, thứ tự (challenger, owner) ✓ khớp gốc), LuaHaveBeginWar (:504, nState==JX2CW_STATE_ATWAR=2 thay byte state==5 của gốc — tương đương ngữ nghĩa), LuaAppointViceroy (:546), LuaAppointChallenger (:581: ghi szChallenger + state WARDECIDED ngay trong GS — gốc chỉ gửi relay; dự án KHÔNG xóa danh sách báo danh như relay 0x080988C4, nhưng danh sách của dự án nằm trong League 508 nên không cùng kho), LuaNumOfSignUpTongs/LuaGetSignUpTongName (:619/:628: đếm/đọc member League 508 theo tên thành — gốc đọc vector đồng bộ từ relay và có cổng state 1..3), LuaGetCityWarTongCamp (:741: trả 1 thủ/2 công/nil theo owner/challenger — KHÁC gốc: gốc trả TONG_GetCurCamp của bang, chỉ với bang trong map +0x740; 0 call site script nên chưa gây lỗi). STUB: cả nhóm Arena (IsArenaBegin…NotifyArenaResult, KJx2CityWar.h:44-54, .cpp ~675-735 trả 0/""), SignUpCityWarArena không đăng ký (giống gốc). Đăng ký tại ScriptFuns.cpp:14766-14795.

### cau_truc_du_lieu

GS KCityWarDataGS @0x0830A680 (vtable 0x082511B0, 1 virtual dtor 0x0805C540): +0x0C int SignUpFee; +0x4C std::map<std::string,int> tênThành→idx; +0x1AC..: KCity[8] stride 0x72 (slot 0 = thành 0 không dùng; Lua city c = slot c; gói relay key1 = slot+1): {+0 int idx; +4 byte; +5 byte STATE (1 báo danh/2 hết báo danh/3 lôi đài/5 đang đánh); +6 char[32] chủ thành; +0x26 char[32] thái thú; +0x46 char[32] khiêu chiến giả; +0x66,+0x6A,+0x6E int (key2 6/7/8)} → slot c tại 0x0830A82C+0x72*c (các hàm Lua copy từ 0x0830A820+0x72*c+0xC). +0x53C vector<std::string>[8] tên bang báo danh (0x0830ABBC+12*c, key2=10, tách '\n', tên ≤0x40); +0x59C vector<int>[8] phí đấu thầu (0x0830AC1C, mặc định SignUpFee); +0x5FC vector<ArenaResult 0x25 byte {u16 a; u16 b; byte win; ...}>[8] (0x0830AC7C, key2 riêng qua kênh "城战-擂台赛目前阶段"); +0x65C vector<u16>[8] bảng đấu (0x0830ACDC, key2=11; phần tử |0x10000 = đã thắng); +0x6BC [8] (0x0830AD3C); +0x6DC/+0x6E0 mốc vòng đấu; +0x740 std::map<std::string,{int camp; char[32]}> camp bang (ghi 0x08058D40 từ gói 0x0A/0x03). Gói GS→relay 0x0A: cmd1 báo danh 0x4A byte {0A,01, int city@2, char name[32]@6, char tong[32]@0x26, int fee@0x46}; cmd5 0x?? {0A,05,int city@2,byte@6}; cmd6 0x27 byte {0A,06, byte flag@2 (1 challenger/0 viceroy), int cityIdx@3, char tong[32]@7}. Gói relay→GS 0x0A sub3 0x46 byte {0A,03, char tong[32]@2, int camp@0x22, char[32]@0x26}. Kênh dữ liệu-có-tên: tên GBK "城战-城区数据" (len 0xE) key1=slot+1 (2..8), key2=subcmd 0..12; "城战-擂台赛目前阶段" cho kết quả lôi đài. RELAY KCityWarDataRelay: +0x1A0 KCity[8] stride 0x72 {+8 byte; +9 STATE; +0xA chủ[32]; +0x2A thái thú[32]; +0x4A khiêu chiến[32]}; +0x534 vector<string>[8] báo danh; +0x594 vector<int>[8] phí; +0x5F4 kết quả lôi đài; +0x654 bảng đấu; +0x6D4; +0x968 danh sách GS; +0x96C kho dữ liệu toàn cục (find 0x08106072, commit 0x08106974, push 0x081065DE); tongInfo từ 0x080D6554: +0 camp (task 2 = TONG_GetCurCamp), +0x64 tên bang chủ, +0x84 chuỗi 32 byte.

### chua_ro

- Ý nghĩa chính xác các int +0x66/+0x6A/+0x6E (key2 6/7/8) và byte +4 (key2 1) của KCity trên GS — chưa thấy hàm Lua nào đọc trong phạm vi nhiệm vụ này (có thể là thuế/ngày chiếm; cần đối chiếu GetArenaSchedule/GetArenaInfoByCity do nhánh khác).
- Chuỗi 32 byte tongInfo+0x84 được relay gửi kèm camp (GS cất vào map +0x740 nhưng GetCityWarTongCamp không dùng): chưa xác định là tên liên minh hay tên thành.
- Thời điểm relay gọi vòng đồng bộ camp 0x0809AA1A (nghi là khi GS kết nối / StartArena) — chưa truy ngược caller.
- Hàm 0x0809E78E (relay cmd2) và 0x0809E65C (cmd5) chỉ nhận diện sơ bộ (cmd2 nhận city + 2 tên + int, kiểm tên với chủ/thái thú; cmd5 đổi byte state) — nhánh relay thuộc nhiệm vụ khác.
- Ý nghĩa chuỗi 0x0978A54C (thông báo gửi người chơi khi không phải bang chủ) nằm trong .bss (nạp từ file cấu hình lúc chạy) nên không đọc được nội dung tĩnh.


## giao-thuc:GS<->relay


### ket_luan

Giao thức arena GS<->relay trên bản VN là HAI kênh độc lập, đều còn nguyên trong binary: (A) GS->relay "lệnh" = gói đầu byte 0x0A (KCityWarDataRelay nhận tại relay 0x0809EC68, switch pkt[1] = 1..6; GS gửi qua KRelayClient::Send 0x080777A0 với this=g_RelayClient 0x0836EA80); (B) relay->GS "đồng bộ dữ liệu" = cơ chế SDB (shared-record) tổng quát: gói 0x0F/0x27 (cập nhật) hoặc 0x0F/0x29 (xoá/đặt lại) mang TÊN bản ghi GBK + khoá + chỉ số trường + dữ liệu; GS lọc theo tên tại 0x0820A460 và giao cho KCityWarDataGS::OnRecord 0x08059660 (bản ghi "城战-城区数据", khoá = city+1, 13 trường cố định + trường >=13 là kết quả trận) hoặc KCityWarDataCore 0x08058490 (bản ghi "城战-擂台赛目前阶段": trường 0 = byte đang đấu lôi đài, trường 1 = dword thành đang đấu). Cũng có kênh relay->GS 0x0A (1 = chạy script cho người chơi, 2 = nhắn tin hệ thống tới playerId, 3 = ghi bản ghi theo tên) và kênh relay->GS 0x03/0x21 (lệnh GM "dw AddLocalNews(...)" broadcast). KHÔNG có opcode "huỷ báo danh": GS->relay chỉ có 1 báo danh/2 thuế/3 rỗng/4 kết quả lôi đài/5 kết quả công thành/6 bổ nhiệm; việc loại khỏi danh sách do relay tự làm (EndSignUp cắt >16, hoàn phí) rồi đồng bộ lại trường 10. Đặc biệt: GS binary VẪN CÓ LuaSignUpCityWar (0x08121B50 vùng, chuỗi lỗi "LuaSignUpCityWar() usage error" 0x0825D3FC) gọi KCityWarDataGS::SignUp 0x08058650 -> gói 0x0A/1, nhưng KHÔNG được đăng ký trong bảng Lua (bảng 0x088C5E24..0x088C7A9C không có tên SignUpCityWar/SignUpCityWarArena) nên script infocenter_head.lua:118 gọi SignUpCityWarArena là gọi hàm nil.

### chi_tiet

- === A. GS -> RELAY (byte[0]=0x0A). Relay: bảng nhảy 0x081D9738[0x0A]=0x08078CE0 -> 0x0809EC68(this=KCityWarDataRelay, pkt, len), switch pkt[1] qua bảng 0x081DB374 (1..6). GS gửi bằng 0x080777A0(0x0836EA80, buf, len) = KRelayClient::Send (đẩy vào hàng đợi this+0x38 -> 0x080776E0).
- A1. BÁO DANH (op 1), 74 byte (0x4A): [0]=0x0A [1]=1 [2..5]=dword city(0..7) [6..0x25]=char[32] tên người chơi (player+0x5974) [0x26..0x45]=char[32] tên bang (lấy qua 0x080D0780 từ player+0x5A90) [0x46..0x49]=dword phí đấu thầu (Fee). GS gửi: 0x0805CA10(playerIdx, city, fee) được gọi từ KCityWarDataGS::SignUp 0x08058650 (kiểm city<=7, player 1..0x4AE, fee >= this+0xC SignUpFee; nếu không thoả -> log '%s over oper signup.' 0x08250F93 + nhắn 0x0978A54C), được gọi từ LuaSignUpCityWar (thân 0x08121B50.., gọi 0x08058650 tại 0x08121CEB; arg1=city 1..7, arg2=fee; kiểm byte cờ mở báo danh struct+5). Relay nhận: 0x0809ED36 -> KCityWarDataRelay::SignUp 0x0809AE32(this, city, tongName=pkt+0x26, playerName=pkt+6, fee=dword[0x46]): kiểm bang tồn tại (0x080D6554, lỗi TONGERROR), người gửi là bang chủ (so tên với tonginfo+0x64, NOTTONGLEADER), cờ báo danh mở (+0x1A0+c*0x72+9 ==1, SIGNUPTIMEOUT), chưa là chủ thành/khiêu chiến thành khác (BECITYOWNER/BECHALLENGER), phí >= this+4 (NOTENOUGHFEE 0x8389970), trừ phí quỹ bang 0x080D69E6; push tên vào vector +0x530+c*12 và phí vào +0x590+c*12; rồi ghi SDB trường 10 (0x0809B4AA) và trả lời người chơi bằng 0x08095956.
- A2. ĐẶT THUẾ (op 2), 78 byte (0x4E): [0]=0x0A [1]=2 [2]=dword a (thuế giao dịch) [6]=dword b (chỉ số giá) [0xA]=dword city [0xE]=char[32] tên người chơi [0x2E]=char[32] tên bang. GS: 0x0805CAA0 từ 0x08058700 (gọi từ 0x081F681C, kiểm a<=this+0x14 MaxExchangeTax, b<=this+0x18 MaxPriceParam, cờ +0x1B0 'set tax twice'). Relay: 0x0809ED7E -> 0x0809E78E ghi trường 1 (byte), 6, 7.
- A3. op 3: relay handler 0x0809EDD2 rỗng (push ebp; pop ebp; ret) - không dùng.
- A4. KẾT QUẢ TRẬN LÔI ĐÀI (op 4), 43 byte (0x2B): [0]=0x0A [1]=4 [2..5]=dword city [6..0x2A]=bản ghi 37 byte {word tongA(chỉ số trong danh sách báo danh), word tongB, byte result (1 = A thắng, 2 = B thắng, 0 = chưa), 32 byte 0}. Lua NotifyArenaResult 0x08122800(arenaId 0..7, bResult) -> KCityWarDataGS::AddArenaResult 0x08058330(this=0x0830A680, arenaId, bResult): lấy cặp đấu thứ arenaId trong vector cặp +0x6DC+city*12 (phần tử 5 byte: word A, word B, byte trạng thái), city = this+0x1A8 (thành đang đấu), byte= bResult>=1 ? 1 : 2, log '%s win/lose' 0x08250F89; rồi gọi vfunc[0] 0x080593A0(this, city, rec, 0): ghi vào vector kết quả +0x5FC+city*12 (37 byte/phần tử), cập nhật trạng thái thua (+0x20000) trong lịch +0x65C, gọi lại 0x0805A480 dựng vòng sau, và gửi gói {0x0A,4,city,rec}. Relay: 0x0809EDD8 -> 0x0809D4F0(this, city, rec, 0): kiểm pha ==3 (PERIODNOTMATCH), +0x19C (đang đấu) && +0x1A0==city, chỉ số A/B < số bang báo danh, result!=0 (lỗi L_ARENA_RESULT_RECORD_ERROR); lưu 0x080A04A6; ghi SDB trường (12 + số kết quả) = bản ghi 37 byte (0x0809D67C), rồi trường 12 = dword số kết quả (0x0809D77F); nếu xong vòng cuối -> đặt bang thắng làm khiêu chiến (trường 5), trường 2, bản ghi giai đoạn trường 0/1 (0x0809DD9D/0x0809DE63).
- A5. KẾT QUẢ CÔNG THÀNH (op 5), 7 byte: [0]=0x0A [1]=5 [2..5]=dword city [6]=byte (1 = khiêu chiến thắng/ có kết quả, 0). GS: Lua NotifyWarResult 0x08122940 -> 0x080585B0 (đặt +0x1B1=0 rồi gửi). Relay: 0x0809EE0E -> 0x0809E65C(this, city, byte).
- A6. BỔ NHIỆM (op 6), 39 byte (0x27): [0]=0x0A [1]=6 [2]=byte loại (1 = khiêu chiến, 0 = chủ thành/viceroy) [3..6]=dword city [7..0x26]=char[32] tên bang (memcpy 0x1F). GS: 0x080584F0 (loại 1) từ Lua AppointChallenger 0x0812CD00 (tại 0x0812CDA4); 0x08058550 (loại 0) từ Lua AppointViceroy 0x0812CDD0 (0x0812CEB5/0x0812CF1E). Relay: 0x0809EE40 kiểm len==0x27, byte[2]<=1, city 0..7, gọi bảng con trỏ thành viên 0x08295D58: loại 0 -> 0x0809EEFA -> 0x08098F44 (AppointViceroy -> SetCityMaster 0x08099110, lỗi 'AppointViceroy(%d, %s) failed!' 0x081DB310); loại 1 -> 0x0809EF24 -> 0x08098FA8 (AppointChallenger: kiểm bang tồn tại, lỗi 'AppointChallenger error....' 0x081DB330, ghi trường 5).
- === B. RELAY -> GS ĐỒNG BỘ (SDB). Gói: [0]=0x0F [1]=0x27 (cập nhật; GS 0x0820AD00 chỉ nhận khi flag>>5==0) hoặc 0x29 (GS 0x0820ACC0 nhận khi dataLen!=0 hoặc flag>>5==3 = xoá/reset) [2..5]=dword khoá (city+1, GS trừ 1) [6..9]=dword chỉ số trường [0xA]=byte độ dài tên [0xB]=byte cờ [0xC..0xD]=word độ dài dữ liệu [0xE]=byte (không dùng) [0xF..]=tên bản ghi (GBK) rồi dữ liệu; kiểm tổng len == nameLen+dataLen+0xF tại 0x0820AC50 (lỗi 'Sth. is wrong while receiving SDB Record data' 0x0826BAF8). Dispatch GS: bảng 0x08269A0C[0x0F]=0x081DAAF0 -> 0x081D9430 (pkt[1] 0x25..0x29) -> 0x0820AC50 -> 0x0820A460 so tên: 'Adventure'/'LuckyStar'/'GoldBoss'/..., '城战-擂台赛目前阶段' (0x0826BB56, 20 byte) -> 0x08058490(this=0x0830A680...), '城战-城区数据' (0x0826BB6A, 14 byte) -> 0x08059660. Relay ghi bản ghi bằng 0x08106072(sdb=this+0x96C, &rec{namePtr,len,key,field}, &data{ptr,len}, 0) + 0x08106974 (đánh dấu đổi) + 0x081065DE(...,1,0) phát tới mọi GS; tên bản ghi tại relay 0x081DB28E / 0x081DB2A2.
- B1. Bản ghi '城战-城区数据' khoá city+1 (GS struct 0x72 byte tại 0x0830A680+0x1AC+c*0x72; relay tại this+0x1A4+c*0x72): trường 0 = dword chỉ số thành (+0); 1 = byte cờ đã đặt thuế (+4, GS +0x1B0); 2 = byte PHA BÁO DANH/LÔI ĐÀI (+5, GS +0x1B1): 0 chưa, 1 đang mở báo danh (relay StartSignUp 0x0809AB8C ghi =1 tại 0x0809ABDA), 2 đã đóng báo danh (EndSignUp 0x0809C0BA ghi tại 0x0809C864; =4 ở nhánh 0x0809C41E; =0 ở 0x0809C18B khi 0 bang), 3 đang đấu lôi đài (StartArena 0x0809CF60 tại 0x0809D100/0x0809D122); 3 = char[32] tên bang chủ thành (+6, GS +0x1B2, GS gọi 0x080FB030(0x09780B60, city, tên) cập nhật quản lý thành); 4 = char[32] tên bang chủ (người) (+0x26, relay lấy tonginfo+0x44 tại 0x08099AE1); 5 = char[32] tên bang KHIÊU CHIẾN (+0x46, GS +0x1F2); 6 = dword thuế giao dịch (+0x66); 7 = dword chỉ số giá (+0x6A); 8 = dword thời điểm chiếm thành time() (+0x6E, relay 0x08099B63..0x08099B77); 9 = bỏ qua; 10 = DANH SÁCH BÁO DANH: chuỗi 'tên1\ntên2...' + NUL + 'phí1\nphí2...' (relay 0x0809B3DE..0x0809B4A2 ghép bằng '\n' 0x081DB0DD; GS 0x08059978 tách tên vào vector +0x53C, tách phí bằng strtok '\n' sscanf vào +0x59C); 11 = LỊCH LÔI ĐÀI: mảng WORD (len/2 phần tử) = thứ tự ngẫu nhiên chỉ số bang báo danh (relay EndSignUp 0x0809C95F..0x0809CA01 từ vector +0x650, lịch tạo bởi 0x0809CC62 dùng rand 0x0804C54C, cần >=2 bang); GS 0x08059A00 đổ vào +0x65C (dword: low=chỉ số, high=trạng thái) rồi 0x0805A480(this,city,0) dựng cây loại trực tiếp vào +0x6DC (cặp 5 byte: A,B,trạng thái; 2 = bye) và đặt +0x1A4=0; 12 = dword số kết quả (GS 0x08059820 chỉ đối chiếu, lỗi 'Arena Result Count Error' 0x08250F38); >=13 = bản ghi kết quả 37 byte thứ (field-13) (GS 0x08059800 gọi vfunc[0] với flag=1: chỉ lưu, không gửi lại). Cờ đặc biệt: flag>>5==3 && field==10 && !(flag&2) -> GS 0x08059728 xoá sạch danh sách báo danh/phí/kết quả/lịch của thành (reset khi StartSignUp, relay 0x08098B3C).
- B2. Bản ghi '城战-擂台赛目前阶段' khoá 0: trường 0 = byte đang đấu lôi đài (GS 0x0830A824 = this+0x1A4; relay this+0x19C) - IsArenaBegin 0x08101620 đọc byte này và 0x0830A828; trường 1 = dword thành đang đấu (GS 0x0830A828 = this+0x1A8; relay this+0x1A0) - GetArenaTargetCity 0x080FBE20 đọc 0x0830A828. Relay ghi cả hai khi StartArena (0x0809D217 field1=city, 0x0809D2D7 field0=1) và khi kết thúc lôi đài (0x0809DD9D/0x0809DE63).
- === C. RELAY -> GS kênh 0x0A (GS bảng 0x08269A0C[0x0A]=0x081DAA50 -> 0x0805CE10(this=0x097AC9D0, pkt, len)): sub 1 = {0x0A,1,dword playerId,4 byte,script...} -> 0x0805CDA0 tra player theo id (0x080C3DE0) rồi 0x0805CBF0 chạy script Lua với PlayerIndex/PlayerId (lỗi 'Exception Have Caught When Execute Relay CallBack Script' 0x082511E8, 'CityWar get wrong playerID from relay' 0x082511B4); sub 2 = {0x0A,2,dword playerId,word len(msg+1),msg} -> 0x0805CB70 nhắn hệ thống 0x081C9220 loại 1 (relay tạo ở 0x08095956: tra playerId theo tên 0x080845EC, 0x0809EFF0 ghi 2 byte đầu, gửi 0x080E3BE2) - đây là đường trả lời báo danh (các chuỗi G_CITYWAR_WARNING_* / NOTIFY_* nạp vào 0x08389914..0x083899C4 tại 0x0811AAF0); sub 3 = {0x0A,3,char name[32]@2,data@0x22} -> 0x0805CB30 -> 0x08058D40 ghi vào map +0x740 của KCityWarDataGS.
- === D. Tin tức: relay 0x08095798 tạo gói {byte 3, byte 0x21, char[32] 'GM' (0x081DB216), word len, word 1, script} = lệnh GM broadcast (GS 0x081DA9A8 nhánh byte[1]==0x21) với nội dung 'dw AddLocalNews("...")' (0x081DB2F8/0x081DB30A) cho mọi thông báo giai đoạn (NOTIFY_SIGNUP, ARENA_START 0x8389930, ...). Log CityWar: 0x08095B1A ghi Logs\KSG_CityWarLog.txt (0x081DB25B).
- === E. Vòng đời theo binary: relay timer StartSignUp(city) (Lua 0x080FF2E6 -> 0x0809AB8C: reset danh sách 0x08098B3C, pha=1, thông báo) -> người chơi qua GS gửi 0x0A/1 -> relay gom vào danh sách, đồng bộ trường 10 -> EndSignUp (0x080FF356 -> 0x0809C0BA: 0 bang: NO_SIGNUP pha=0; 1 bang: SINGLE_SIGNUP -> làm khiêu chiến luôn trường 5; >16 bang: 0x0809B736 cắt bớt/hoàn phí; tạo lịch 0x0809CC62, pha=2, trường 11) -> StartArena (0x080FF3C6 -> 0x0809CF60: cần lịch, +0x19C=1, +0x1A0=city, pha=3, bản ghi giai đoạn) -> GS dựng cây từ trường 11, script citywar_arena/head.lua:143/147 gọi NotifyArenaResult -> 0x0A/4 -> relay ghi kết quả trường 13+n và trường 12, GS dựng vòng sau -> hết -> relay đặt khiêu chiến, tắt +0x19C -> StartCityWar (0x080FF436 -> 0x0809E13C) -> GS NotifyWarResult 0x0A/5 -> AppointViceroy/AppointChallenger 0x0A/6.
- === F. Script bằng chứng: GS infocenter_head.lua:118 'SignUpCityWarArena(CityID, Fee)' (hàm chết, bảng Lua không có); :601-731 SignUpCityWar/checkSignUpCityWar thay bằng League LEAGUETYPE_CITYWAR_SIGN + khiêu chiến lệnh; citywar_arena/camper.lua:9-13 mô tả API IsArenaBegin/GetArenaCityArea/GetArenaBothSides/NotifyArenaResult/GetArenaLevel; relay relaysetting/task/citywar_0N_*.lua gọi StartSignUp/EndSignUp/StartArena/StartCityWar(N); congthanhchien/citywar_head.lua:162 GlobalExecute 'dwf ... citywar_appointchallenger' (bốc thăm VN). citywar.ini [CitySettings] SignUpFee=1000000 (this+0xC ở GS / this+4 ở relay), MinTongLevel=18, MaxExchangeTax=20, MaxPriceParam=20.

### cau_truc_du_lieu

KCityWarDataGS (GS, đối tượng toàn cục 0x0830A680, vtable 0x08251168 chỉ 1 hàm ảo 0x080593A0 'lưu+gửi kết quả', lớp cha KCityWarDataCore vtable 0x082511B0; ctor 0x08058830): +0x0C SignUpFee, +0x14 MaxExchangeTax, +0x18 MaxPriceParam (đọc từ citywar.ini [CitySettings]); +0x1A4 byte bArenaRunning (0x0830A824, IsArenaBegin); +0x1A8 dword nArenaCity (0x0830A828, GetArenaTargetCity); +0x1AC..: 8 x CITY_INFO[0x72] {+0 dword idx; +4 byte bTaxSet; +5 byte nSignUpStage(0/1/2/3/4); +6 char[32] szOwnerTong; +0x26 char[32] szOwnerMaster; +0x46 char[32] szChallenger; +0x66 dword nExchangeTax; +0x6A dword nPriceParam; +0x6E dword nOccupyTime}; +0x53C 8 x vector<string*> tên bang báo danh (stride 12); +0x59C 8 x vector<int> phí; +0x5FC 8 x vector<ARENA_RESULT[37]> {word A, word B, byte result(1 A thắng/2 B thắng), byte[32] 0}; +0x65C 8 x vector<dword> lịch (low16 = chỉ số bang, high16 = 0 sống/1/2 loại); +0x6BC 8 dword vòng hiện tại; +0x6DC 8 x vector<PAIR[5]> {word A, word B, byte state(0 chờ/2 bye)} (NotifyArenaResult lấy cặp arenaId 0..7 ở đây); +0x740 map tên->giá trị (relay->GS 0x0A/3). Lua: NotifyArenaResult(arenaId 0..7, bAWin) -> 0x08058330(this, arenaId, bAWin). KCityWarDataRelay (relay, lấy qua 0x080FC0B2 từ 0x0837EAD4; vtable 0x081DB580): +4 SignUpFee; +0x50+c*0x20+8 tên thành; +0x19C byte bArenaRunning; +0x1A0 dword nArenaCity; +0x1A4+c*0x72 CITY_INFO (cùng bố cục, code truy cập dạng 0x1A0+c*0x72+{4,8,9,0xA,0x2A,0x4A,0x6A,0x6E,0x72}); +0x530/+0x590/+0x5F0/+0x650 (+c*12) = vector tên/phí/kết quả/lịch; +0x734 KLog; +0x968 tong manager; +0x96C SDB (bản ghi tên GBK, khoá, trường). Gói GS->relay: 0x0A/1 74B, 0x0A/2 78B, 0x0A/4 43B, 0x0A/5 7B, 0x0A/6 39B (bố cục ở chi_tiet). Gói relay->GS SDB: header 15B {0x0F, 0x27|0x29, dword key, dword field, byte nameLen, byte flag, word dataLen, byte pad} + tên + dữ liệu.

### chua_ro

- Ý nghĩa chính xác từng bit của byte cờ SDB (flag, pkt[0xB]) ngoài hai trường hợp thấy trong code: flag>>5==0 (cập nhật thường) và flag>>5==3 (xoá/đặt lại, kèm bit 2 = bỏ qua reset); chưa lần vào lõi SDB (0x08106072/0x081065DE ở relay, 0x0820A460 nhánh khác ở GS).
- Giá trị pha 4 (relay 0x0809C41E ghi stage=4 trong EndSignUp) chưa xác định ngữ nghĩa (có thể = 'huỷ lôi đài / chờ công thành trực tiếp' khi chỉ 1 bang).
- Ai ở relay gửi gói 0x0A/3 (tên + dữ liệu -> GS 0x08058D40 map +0x740) và 0x0A/1 (chạy script cho người chơi): không thấy trong vùng KCityWarDataRelay đã dump; có thể là hệ khác dùng chung kênh 0x0A.
- Chi tiết thuật toán cắt danh sách >16 bang (0x0809B736): thấy sort (0x080A1680/0x080A159A/0x080A1564), hoàn phí 0x080D69E6 và nhắn 0x08095956 nhưng chưa đọc đủ để khẳng định tiêu chí (suy đoán: giữ 16 bang phí cao nhất, loại còn lại = SIGNUP_OUT).
- Vai trò chính xác của vector +0x59C (phí) phía GS sau khi nhận trường 10 (chỉ thấy lưu, chưa thấy hàm Lua nào đọc ngoài GetSignUpTongName/NumOfSignUpTongs đọc tên).
- Hàm GS 0x080FB030(0x09780B60, city, tên) khi nhận trường 3 chưa dịch ngược (giả định = cập nhật chủ thành cho KCity manager).
- Các gói relay->GS 0x0F/0x25, 0x26, 0x28 (cùng dispatcher 0x081D9430) không thuộc arena; không phân tích.


## lua:GS citywar_arena+infocenter


### ket_luan

Phía GS, lôi đài bang hội (mission 5, map 213-220) là MỘT CỖ MÁY HOÀN TOÀN BỊ ĐỘNG: script không tự quyết định "ai đấu với ai / khi nào" mà chỉ poll IsArenaBegin(i) (đọc cặp tên do relay đẩy xuống qua object CityWar 0x830a680, thành đích ở [0x830a828], cờ bật [0x830a824]) rồi mở mission 5 trên map 213+i; mission.lua chép cặp tên vào MissionS(1|2), pha vào sân 8 phút (GO_TIME=24 tick×20s — comment ghi 10'), tổng TIMER_2=25' (đánh thực ≈17'), thắng = bên còn đông người hơn khi hết giờ (totaltimer) hoặc bên kia trống (timer.ReportBattle), hòa thì bên có TỔNG cấp thấp hơn thắng; WinBonus → NotifyArenaResult(ArenaID, 1|0) + AddTongExp(tên bang, 1200). Nhánh báo danh gốc (SignupACity/SignUpTheOne/SignUpFinal → IsSigningUp + SignUpCityWarArena(CityID, Fee) với Fee do người chơi nhập 1.000.000..99.999.999, SignUpFee citywar.ini=1000000 chỉ là mức sàn do relay kiểm) bị ghi chú "(废弃2006-11-22)" và KHÔNG còn nút nào dẫn tới; VN thay bằng ArenaMain → SignUpCityWar (League 508/509 + khiêu chiến lệnh, chỉ 18h-19h). Điểm PK (MS_TONG1VALUE/2VALUE, death.lua) được cộng nhưng KHÔNG dùng để phân thắng bại. PreEnterGame/EnterGame/EnterBattle (infocenter_head.lua:34-79) là lối vào sân cho người chơi; GetArenaLevel/GetArenaTargetCity/GetArenaTotalLevel(ByCity)/GetArenaInfoByCity/NumOfSignUpTongs/GetSignUpTongName KHÔNG được script GS nào gọi (chỉ trong ghi chú), GetArenaSchedule chỉ nằm trong ArenaInfo() đã bị tháo khỏi menu.

### chi_tiet

- (1) BÁO DANH — NPC: infocenter_head.lua được Include bởi script/missions/sevencity/dialog_npc.lua:2 (NPC nghị sự công thành, main() dòng 34-43 chỉ mở GiveTiaoZhanLing/ViewTiaoZhanLing/thất thành đại chiến, KHÔNG gọi ArenaMain), vng_event/denbu_congthanh/congthanh.lua:3, event/vng/mergeserver201105/cityvar7.lua:2; infocenter.lua (citywar_global) main() chỉ Say 1 câu chỉ sang 'xa phu công thành'. Hàm ArenaMain (infocenter_head.lua:141-156) hiện KHÔNG có caller nào trong cây GS (grep toàn script: 0 kết quả ngoài file định nghĩa; main() gốc ở dòng 82-85 bị comment). Tệp NPC đặt trên map (manager.lua/camper1/camper2/leavetrap) không tra được vì D:\ServerLinux\server1\maps\map_publish chỉ còn wumumenpai/xiaoyao — chỉ có bằng chứng boot nạp: Logs/KSG_ScriptOutputLog_20260806.txt:3745-3753 ExecuteCode citywar_arena/manager|camper1|camper2|totaltimer|timer|leavetrap|death|mission.lua.
- (1) Điều kiện báo danh VN: checkSignUpCityWar (infocenter_head.lua:713-731): nTongID~=0 và GetTongMaster()==GetName() (715); giờ 18<=H<19 (717); TONG_GetExpLevel(nTongID)>=18 (719); không là chủ thành (checkCityOwner 733-740 dùng GetCityOwner(i)); không đang là phe khiêu chiến (checkCItyChallenger 742-749 dùng GetCityWarBothSides(i)); getSignUpState(nCityId)==1 (725; =LG_GetLeagueTask(508, tên thành, 1) dòng 556-558, relay set =1 lúc mở báo danh citywar_head.lua:103 và =0 khi đóng :144). Thành đang báo danh = getSigningUpCity(1) theo thứ trong tuần TB_CITYWAR_ARRANGE (citywar_function.lua:6-14, 68-75). Phí: VN KHÔNG thu tiền, nộp khiêu chiến lệnh (sure_signupcitywar 662-711: trừ League 538 'tiaozhanling' task 1, citywar_tbLadder:AddOneInGameServer → LG_ApplyDoScript(1,'','',\\script\\mission\\citywar_global\\ladder.lua,'citywar_tbLadder__AddOne',...) ladder.lua:28-37 đẩy sang relay; checkFirstSignUpChallenger 751-796 cập nhật League 509). Nhánh GỐC CN: SignupACity(sel)/SignUpTheOne (88-113) kiểm IsSigningUp(CityID)==1 (engine CÓ, 0x08115930, nhận số 1..7) → SetTaskTemp(15,CityID) → AskClientForNumber('SignUpFinal',1000000,99999999,...) → SignUpFinal(Fee) (116-119) gọi SignUpCityWarArena(CityID, Fee) — hàm này KHÔNG có trong jx_linux_y.luamap.full.txt (đã xác nhận PHULUC_HAM_CONGTHANH.md:126). SignUpFee=1000000 / MinTongLevel=18 / MinTongCrowNumber=37 nằm ở relaysetting/citywar.ini:69-81 (ghi chú dòng 2: tệp dùng chung relay+GS+client) — GS script không đọc trực tiếp.
- (2) TIMER 18: settings/timertask.txt:19 '18 \\script\\missions\\citywar_global\\timer.lua'; global mission 8 (settings/task/missions.txt:9 → citywar_global/mission.lua) được mở bởi script/global/autoexec.lua:142 OpenGlbMission(8); InitMission (mission.lua:4-7) SetGlbMissionV(1,1) + StartGlbMSTimer(8,18,INTERVAL=5*60*18 = 5 phút). timer.lua:8-20: for i=1..8: if IsArenaBegin(i-1)==1 → WorldIdx=SubWorldID2Idx(213+i-1); nếu >=0 → SubWorld=WorldIdx; if GetMissionV(1)==0 → OpenMission(5). Dòng 22-35 tương tự cho thành chiến: HaveBeginWar(i)==1 → map 221 OpenMission(6)+RunMission(6). demotimer.lua là bản demo không có trong timertask.txt. Engine IsArenaBegin 0x08101620: kiểm byte [0x830a824]!=0 (cờ hệ công thành bật) và [0x830a828] (CityID đích) trong 1..7, arg 0..7, rồi gọi 0x8139610(this=0x830a680, CityID, ArenaID, &buf1, &buf2) → trả 1 nếu hàm đó !=0; GetArenaBothSides 0x08104680 dùng đúng hàm 0x8139610 với cùng tham số rồi pushstring 2 tên (trả '' '' khi tắt, 0x08250931). Suy ra 'arena begin' == 'khe ArenaID của thành đích có cặp tên'.
- (3a) mission.lua (settings/task/missions.txt:6 mission 5): InitMission (2-23) xóa MissionV 1..40, MissionS 1..10; WorldID=SubWorldIdx2ID(SubWorld); nếu >=213: MissionV(MS_ARENAID=15)=WorldID-213; tong1,tong2=GetArenaBothSides(ArenaID); nếu cả hai ~='' → MissionS(1)=tong1, MissionS(2)=tong2, MissionS(3)=GetCityAreaName(GetArenaCityArea(ArenaID)), MS_STATE(1)=1, StartMissionTimer(5,16,TIMER_1=20s*18) và (5,17,TIMER_2=25*60*18). Nếu cặp tên rỗng thì mission mở nhưng MS_STATE=0 (camper.lua:23-33 sẽ nói 'chưa đến giờ'). RunMission (25-40): duyệt GetNextPlayer(5,idx,0) SetFightState(1) mọi người, MS_STATE=2. EndMission (42-54): xóa V/S, GameOver() (head.lua:61-78 LeaveGame từng người), StopMissionTimer 16,17. OnLeave(RoleIndex) (56-66): reset team/deathscript/PK, Msg2MSAll, TaskTemp(200)=0 (chú ý không SetLogoutRV(0) vì rớt mạng cũng OnLeave).
- (3b) timer.lua (timer 16, mỗi 20s): OnTimer 3-23: V=MissionV(MS_NEWSVALUE=9), V+1; nếu V==GO_TIME (=480*18/360=24 → 24×20s = 8 phút, comment ghi '报名时间为10') → RunMission(MISSIONID) (engine 0x08132E50 → script RunMission) và return; state 1 → ReportMemberState (25-39: thông báo còn X phút vào sân; nhánh V==GO_TIME dòng 27-33 không bao giờ chạy và có lỗi chính tả MSSIONID); state 2 → ReportBattle (41-61): nếu GetMSPlayerCount(5,1)<=0 → WinBonus(2)+CloseMission(5); nếu phe 2 <=0 → WinBonus(1)+CloseMission(5); còn lại báo số người + GetMSRestTime(5,17)/18. state 3 → Stop timer 16,17. totaltimer.lua (timer 17, hết 25 phút kể từ InitMission, tức đánh ≈17'): State==0 return; Rest1>Rest2 → WinBonus(1); Rest2>Rest1 → WinBonus(2); bằng nhau → GetTotalLevel() (37-74, hàm SCRIPT cục bộ: cộng GetLevel() từng người mỗi phe qua GetNextPlayer(5,idx,1|2); trả 1 nếu tổng phe1 < tổng phe2 → phe 1 thắng, ngược lại phe 2); AddGlobalNews(str); MS_STATE=3; CloseMission(5). Điểm PK MS_TONG1VALUE/2VALUE KHÔNG được đọc ở bất cứ đâu khi phân thắng bại.
- (3c) Ai được vào: manager.lua (NPC cửa, main 22-29) lưu Task 300/301/302 = điểm về, Say 8 lựa chọn '/EnterBattle' → EnterBattle(nBattleId) NewWorld(MapTab[n+1]) không kiểm gì (khác infocenter_head.lua:65-79 có kiểm IsArenaBegin). camper.lua (NPC chọn phe, camper1/2 chỉ Include): main 21-41 theo MS_STATE: 0 → nói chưa đến giờ/IsArenaBegin; 1 → OnReady (47-59: còn (GO_TIME-V)*20 giây, nút /OnJoin); 2 → OnFighting (97-103 chỉ báo tình hình, KHÔNG cho vào nữa). OnJoin 79-95: GetTongName()==GetMissionS(1) → JoinCamp(1) / ==MissionS(2) → JoinCamp(2), điều kiện GetJoinTongTime()>=7200 (đơn vị engine, comment 'thời gian gia nhập quá ngắn'); else ErrorMsg(4). JoinCamp (head.lua:81-129): SetFightState(0), LeaveTeam(), GetMSPlayerCount(5,Camp)>=MAX_MEMBER_COUNT=16 → từ chối; AddMSPlayer(5,Camp); TaskTemp(JOINSTATE=242)=1; SetCurCamp(Camp); TaskTemp(200)=1; SetLogoutRV(1); SetPunish(0); SetCreateTeam(0); SetPKFlag(1); ForbidChangePK(1); SetRevPos(20,10); SetDeathScript(citywar_arena\\death.lua); SetTempRevPos(GetLeavePos()); SetPos(CampPos1={1536,3223} vàng | CampPos2={1563,3195} tím); Msg2MSAll. Chú ý camper1 và camper2 là CÙNG một script (không cố định phe theo NPC). leavetrap.lua: trap rời sân → reset + NewWorld(GetLeavePos()).
- (3d) death.lua OnDeath(Launcher) 3-48: curcamp=GetCurCamp(); DelMSPlayer(5,curcamp) (người chết bị loại khỏi đếm phe → ảnh hưởng trực tiếp tới thắng bại theo số người); kẻ giết = NpcIdx2PIdx(Launcher); nếu >0: đổi PlayerIndex, TaskTemp(MS_TOTALPK=250)+1, Task(250)+1; curcamp==1 → MissionV(MS_TONG2VALUE=14)+1, curcamp==2 → MissionV(MS_TONG1VALUE=13)+1 (mỗi mạng +1, không phải 3/-1 như lời thoại PKWINBONUS=3/LOSEBONUS=1 khai báo head.lua:27-28 nhưng không dùng); Msg2MSAll. Sau đó Task(MS_TOTALKO=251)+1, SetCurCamp(GetCamp()), SetPunish(1), SetPKFlag(0), ForbidChangePK(0), SetLogoutRV(0), SetCreateTeam(1), SetDeathScript(''), SetRevPos(20,10), TaskTemp(242)=0, SetFightState(0), NewWorld(213+MissionV(15),1633,3292) — bị đá về cổng CÙNG map (khớp comment manager.lua:4). Không có hồi sinh tại chỗ/vào lại phe (camper ở state 2 chỉ OnFighting).
- (3e) Kết thúc: WinBonus(camp) head.lua:141-151: camp==1 → NotifyArenaResult(MissionV(15), 1) + AddTongExp(MissionS(1), WIN_TONGEXP=1200); else NotifyArenaResult(ArenaID, 0) + AddTongExp(MissionS(2), 1200); SubTongExp bị comment (LOSE_TONGEXP=1400 không dùng). Lời thoại OnHelp (camper.lua:44, manager.lua:37) mô tả luật CN gốc: cược 1000 vạn lượng, thắng +800 vạn +12.000 exp bang, thua -14.000 exp — KHÔNG khớp script hiện tại (1200/không trừ/không tiền). Engine CN gốc xử lý tiền cược phía relay (NotifyArenaResult → relay), script GS không Pay/Earn. CloseMission(5) → EndMission → GameOver đuổi hết ra.
- (4) PreEnterGame (infocenter_head.lua:34-51): TongName=GetTong(); for i=0..7: IsArenaBegin(i)==1 → Tong1,Tong2=GetArenaBothSides(i); nếu bang mình là 1 trong 2 → EnterBattle(i) return; else EnterGame(). EnterGame (53-63): dựng menu 8 lôi đài ghi '(Tong1 vs Tong2)' hoặc '(Khoảng trống)' → /EnterBattle. EnterBattle (65-79): IsArenaBegin(n)~=1 → return; SetFightState(0); lưu Task 300-302 điểm về; NewWorld(MapTab[n+1]). Cả 3 hàm KHÔNG có caller trong cây GS hiện tại (không NPC nào gọi PreEnterGame; chỉ manager.lua có EnterBattle riêng). GetArenaLevel: 0 lần gọi (chỉ ghi chú camper.lua:13). GetArenaTargetCity: chỉ trong comment infocenter_head.lua:142,665 (VN thay bằng getSigningUpCity(1)). GetArenaTotalLevel/GetArenaTotalLevelByCity/GetArenaInfoByCity: 0 lần gọi thật (AllArenaInfo 481-488 đã comment). GetArenaSchedule: ArenaInfo 472-479 gọi nhưng menu CityInfo (402-413) đã bỏ nút (ghi chú 400-401 '赛程安排/ArenaInfo' bị comment). NumOfSignUpTongs/GetSignUpTongName/AppointChallenger: 2 hàm đầu 0 lần gọi; AppointChallenger chỉ qua citywar_function.lua:269-273 citywar_appointchallenger (relay GlobalExecute dwf, citywar_head.lua:138) với điều kiện SubWorldID2Idx(2)>=0 (chỉ GS có map 2).
- DANH SÁCH HÀM ENGINE script lôi đài gọi (địa chỉ theo jx_linux_y.luamap.full.txt): Arena/CityWar: IsArenaBegin 0x08101620, GetArenaBothSides 0x08104680, GetArenaCityArea 0x080FDFF0, GetCityAreaName 0x080FFEB0, NotifyArenaResult 0x08122800, HaveBeginWar 0x08104770, GetCityWarBothSides 0x080FFDA0, GetCityOwner 0x080FFF40, IsSigningUp 0x08115930 (nhánh cũ), AppointChallenger 0x0812CD00, AppointViceroy 0x0812CDD0, GetArenaSchedule 0x08122000 (không tới được). Mission: OpenMission 0x081332F0, RunMission 0x08132E50, CloseMission 0x081327E0, GetMissionV 0x081072F0, SetMissionV 0x08107390, GetMissionS 0x08107160, SetMissionS 0x08107220, StartMissionTimer 0x08138840, StopMissionTimer 0x08134720, GetMSRestTime 0x081361C0, AddMSPlayer 0x081366A0, DelMSPlayer 0x081372A0, GetMSPlayerCount 0x081351F0, GetNextPlayer 0x08135760, Msg2MSAll 0x08134280, OpenGlbMission/SetGlbMissionV/StartGlbMSTimer/StopGlbMSTimer (citywar_global/mission.lua). Map/người chơi: SubWorldIdx2ID 0x081077D0, SubWorldID2Idx 0x08102580, NewWorld 0x0811B690, SetPos 0x0811B940, GetWorldPos 0x08128F30, SetFightState 0x08117A10, SetPKFlag 0x0810F610, ForbidChangePK 0x0810F590, SetPunish 0x0810F470, SetLogoutRV 0x08110500, SetRevPos 0x0811B370, SetTempRevPos 0x08110790, SetDeathScript 0x08110700, SetCreateTeam 0x08120FC0, LeaveTeam 0x08121060, GetCamp 0x08114650, GetCurCamp 0x081146C0, SetCurCamp 0x0811B1D0, NpcIdx2PIdx 0x081058B0, GetTask/SetTask, GetTaskTemp 0x08123A20, SetTaskTemp 0x08123950, GetName, GetLevel, AddGlobalNews 0x08125A90, WriteLog 0x081237D0, Say/Talk/TaskSay/TaskSayList, AskClientForNumber 0x08115CA0. Bang hội: GetTong 0x0811AAB0, GetTongName 0x0811AB30, GetTongMaster 0x081144F0, GetTongFigure 0x081145B0, GetJoinTongTime 0x08121100, AddTongExp 0x08105300, TONG_GetExpLevel 0x0818CA30. League/Ladder (nhánh VN): LG_GetLeagueObj, LG_GetLeagueObjByRole, LG_GetMemberCount, LG_GetMemberInfo, LG_GetMemberTask, LG_GetLeagueTask, LG_ApplyAppendMemberTask, LG_CreateLeagueObj/LG_SetLeagueInfo/LG_ApplyAddLeague/LG_FreeLeagueObj, LGM_CreateMemberObj/LGM_SetMemberInfo/LG_AddMemberToObj/LGM_ApplyAddMember/LGM_FreeMemberObj, LG_ApplyDoScript 0x0815B700, Ladder_GetLadderInfo 0x08159BD0, Ladder_ClearLadder. Ngoài ra GetArenaCredits/AddArenaCredits/ReduceArenaCredits/SetArenaCredits (0x08107E40-0x0812B6D0) tồn tại nhưng thuộc hệ tiền tệ exchangeshop (costcurrency.lua:133,142), không liên quan lôi đài bang hội.
- BẪY/LỆCH đáng ghi vào đặc tả: (a) thời gian vào sân thực 8' không phải 10'; đánh thực ≈17' vì timer 17 chạy từ InitMission. (b) camper1/camper2 cùng script — phe quyết định bởi tên bang, không bởi NPC. (c) chết = bị loại khỏi phe (DelMSPlayer) nhưng vẫn ở map → có thể nói chuyện camper nhưng state 2 không cho vào lại. (d) người chết bị SetRevPos(20,10) cố định (thành nào? CS_RevId=20 head.lua:2) khác leavetrap SetRevPos(99,43). (e) MAX 16 người/phe. (f) thưởng thực 1200 tong exp, không trừ, không tiền — lời thoại nói 12.000/−14.000/800 vạn. (g) timer 18 mở mission 5 chỉ khi GetMissionV(1)==0 trên map; nếu InitMission gặp tên rỗng thì MS_STATE=0 mà mission vẫn 'mở' → không mở lại cho tới khi CloseMission. (h) ArenaMain/PreEnterGame hiện KHÔNG được NPC nào gọi — dựng lại cần gắn vào NPC (manager.lua hiện là lối vào duy nhất, không kiểm IsArenaBegin).

### cau_truc_du_lieu

Mission 5 (map 213+ArenaID): MissionV: [1]=MS_STATE (0 chưa có cặp / 1 pha vào sân / 2 đang đánh / 3 xong), [9]=MS_NEWSVALUE (số tick timer16 đã qua, 20s/tick, GO_TIME=24), [10]/[11]=MS_TONG1ID/2ID (khai báo, không dùng), [13]=MS_TONG1VALUE, [14]=MS_TONG2VALUE (số mạng hạ được, không dùng phân định), [15]=MS_ARENAID (0..7 = WorldID-213). MissionS: [1]=bang phe 1 (vàng), [2]=bang phe 2 (tím), [3]=tên khu thành (GetCityAreaName(GetArenaCityArea(ArenaID))). Timer: 16=TIMER_1=360 frame (20s), 17=TIMER_2=27000 frame (25'). Người chơi: Task 300/301/302 = map,x,y điểm về; Task 250 tổng PK, 251 tổng chết; TaskTemp 242 JOINSTATE, 200 khóa đổi phe, 15 tạm (CityID hoặc CardID), 245 thành đang xem (player_tmp_task_def.txt:93). Tọa độ: cổng vào/đá ra {1633,3292}; CampPos1 {1536,3223}; CampPos2 {1563,3195}; hồi sinh CS_RevId=20,CS_RevData=10. Global mission 8 timer 18 chu kỳ 5'. Engine: object CityWar tại 0x830a680; [0x830a824] byte cờ bật hệ công thành; [0x830a828] dword CityID đích hiện tại (1..7); hàm thành viên 0x8139610(obj, CityID, ArenaID, char*name1, char*name2) → !=0 nếu khe ArenaID có cặp tên (dùng bởi IsArenaBegin 0x08101620 và GetArenaBothSides 0x08104680; chuỗi rỗng 0x08250931). League: 508 (LEAGUETYPE_CITYWAR_SIGN, tên = tên thành, task 1 = cờ đang báo danh, member task 1 = số khiêu chiến lệnh), 509 (CITYWAR_FIRST, bang đang dẫn), 538 'tiaozhanling' (kho khiêu chiến lệnh của bang, member task 1); Ladder 10261. citywar.ini [CitySettings]: SignUpFee=1000000, MinTongLevel=18, MinTongCrowNumber=37, WarCycleValue=7 (dòng 65-91). TB_CITYWAR_ARRANGE {báo danh wday, đánh wday} cho 7 thành (citywar_function.lua:6-14).

### chua_ro

- NPC nào trên map 213-220 gắn manager.lua / camper1 / camper2 / leavetrap và NPC nào gắn infocenter/ArenaMain: dữ liệu map (.wor/npc) không có trong D:\ServerLinux\server1\maps\map_publish (chỉ còn wumumenpai, xiaoyao); chỉ chứng minh được script được nạp lúc boot (log 20260806:3745-3753).
- Ý nghĩa/đơn vị của GetJoinTongTime()>=7200 (giây? phút?) chưa xác minh từ binary.
- Nội bộ hàm 0x8139610 (cách relay đẩy cặp tên vào object 0x830a680, cấu trúc khe Arena, vòng/level) chưa dịch ngược — cần để viết phần C++ KJx2CityWar; IsArenaBegin chỉ chứng minh được 'có cặp tên' chứ chưa thấy cờ trạng thái riêng.
- Luồng CN gốc của tiền cược (SignUpFee, hoàn 1000 vạn + 800 vạn cho bên thắng) nằm ở relay (StartSignUp/EndSignUp/StartArena + NotifyArenaResult) — chưa dịch ngược, script GS không có Pay/Earn nào cho lôi đài.
- Hàm SignUpCityWarArena thực sự từng làm gì (gói tin gửi relay) không thể biết từ GS binary VN vì hàm bị gỡ; IsSigningUp (0x08115930) còn tồn tại và nhận CityID 1..7 nhưng nội dung đọc trường nào chưa đọc hết.
- GetArenaCityArea(ArenaID) trả CityID nào khi nhiều thành cùng lúc: theo IsArenaBegin chỉ có MỘT thành đích [0x830a828] tại một thời điểm — cần xác nhận bằng disasm 0x080FDFF0.


## lua:relay congthanhchien CN


### ket_luan

Ban CN goc (ban con nguyen ven nhat la 7 tep relaysetting\task\citywar_0N_startarena.lua) chay CONG THANH THEO TUAN, moi thanh mot ngay co dinh, 4 pha: NGAY D 18:00 StartSignUp(city) -> 19:00 EndSignUp(city) -> 20:00 StartArena(city) (loi dai giua cac bang ghi danh) -> NGAY D+1 20:00 StartCityWar(city). Ngay D theo thanh (thu trong tuan, %w): Dương Châu=CN(0), Thành Đô=T2(1), Đại Lý=T3(2), Phượng Tường=T4(3), Tương Dương=T5(4), Biện Kinh=T6(5), Lâm An=T7(6); cong thanh ngay hom sau (TB_CITYWAR_ARRANGE {ngay_ghi_danh, ngay_cong_thanh, ten}). Ban VN giu nguyen khung gio/ngay nhung re nhanh bang GetProductRegion()=="vn" (settings\product_config.ini ProductRegion=4): pha 1 chi reset ladder + bat co League 508; pha 2 thay EndSignUp bang GetRandomChallenger (bang nop nhieu "Khieu chien lenh" nhat, hoa thi random) roi goi citywar_appointviceroy/appointchallenger tren GS; pha 3 StartArena(city) VAN DUOC RELAY GOI hang tuan (tep startarena khong co nhanh vn) nhung GS VN khong co SignUpCityWarArena nen loi dai chet; pha 4 StartCityWar giu nguyen. cw_CanStart(city, phase) chi kiem NGAY HOM NAY (yymmdd) co nam trong bang ngay nghi TAB_NONE_CITYWAR[phase] hay khong (khong lien quan city). TaskSetMode(1)+TaskSetStartDay(m,d)+TaskInterval(7) = lich tuan tinh tu moc ngay (nam mac dinh 2005); TaskSetMode(0)+TaskInterval(1440) = lich phut hang ngay. Thu muc congthanhchien\ la ban VN viet lai (co "Heart Doldly", co HinhThucCongThanh), KHONG duoc dang ky trong tasklist.ini nen khong chay.

### chi_tiet

- TASK DANG CHAY (tasklist.ini): relaysetting\task\tasklist.ini Task_14..Task_41 = citywar_01..07_{startsignup,endsignup,startarena,startcitywar}.lua (thu muc task goc, KHONG phai congthanhchien\), Task_66 = citywar_clearchallenger.lua, Task_90..94 = sevencity_1..5.lua. Khong co dong nao tro vao congthanhchien\ -> 12 tep trong congthanhchien\ la ban nhap/khong chay. Relay nap tu chuoi '\RelaySetting\Task\' + 'TaskList.ini' (s3relay_y offset 0x19EF82/0x19EF96, TaskCentre.cpp), ham bat buoc TaskShedule/TaskContent/GameSvrConnected/GameSvrReady (0x19F045..0x19F093).
- PHA 1 StartSignUp: citywar_0N_startsignup.lua:5-9 TaskInterval(1440) + TaskTime(18,0) (hang ngay 18:00); :16-22 if GetProductRegion()~='vn' then if cw_CanStart(N,1)==1 then StartSignUp(N) else cw_startsignup_fun(weekday,N). Ngay CN ghi trong comment dong 4 (星期三 PT, 星期一 TD, 星期二 DL, 星期五 BK, 星期四 TD, 星期日 DC, 星期六 LA). LUU Y: nhanh CN trong tep hien tai KHONG kiem thu -> neu chay o region CN se StartSignUp 7 thanh MOI NGAY; ban CN goc chac chan dung TaskSetMode(1)/TaskSetStartDay/TaskInterval(7) nhu tep startarena (ban VN da sua startsignup/endsignup/startcitywar sang 1440 + weekday check trong cw_*_fun).
- PHA 2 EndSignUp: citywar_0N_endsignup.lua:5-8 TaskInterval(1440) TaskTime(19,0); :15-21 CN: cw_CanStart(N,2) -> EndSignUp(N); VN: cw_endsignup_fun(weekday,N).
- PHA 3 StartArena (tep CN nguyen ban): citywar_0N_startarena.lua:4-13 TaskSetMode(1); TaskSetStartDay(1,d) (2005-01-d); TaskInterval(7); TaskTime(20,0); :20-22 if cw_CanStart(N,3)==1 then StartArena(N). Moc: 01=1/5(T4), 02=1/3(T2), 03=1/4(T3), 04=1/7(T6), 05=1/6(T5), 06=1/2(CN), 07=1/1(T7) (2005-01-01 la thu 7, khop). KHONG co nhanh GetProductRegion -> tren VN relay van goi StartArena(city) 20:00 dung ngay ghi danh hang tuan.
- PHA 4 StartCityWar: citywar_0N_startcitywar.lua:5-8 TaskInterval(1440) TaskTime(20,0); :15-21 CN: cw_CanStart(N,4) -> StartCityWar(N); VN: cw_start_fun(weekday_war,N). Ngay = ngay ghi danh + 1 (comment dong 4: PT 星期四, TD 星期二, DL 星期三, BK 星期六, TD 星期五, DC 星期一, LA 星期日) = cot 2 cua TB_CITYWAR_ARRANGE (citywar_head.lua:22-30).
- LICH TUAN CN (theo thanh, D = ngay ghi danh): CN: Dương Châu D, cong thanh T2 | T2: Thành Đô, war T3 | T3: Đại Lý, war T4 | T4: Phượng Tường, war T5 | T5: Tương Dương, war T6 | T6: Biện Kinh, war T7 | T7: Lâm An, war CN. Moi ngay trong tuan deu co 1 thanh ghi danh + loi dai 18:00-20:00 va 1 thanh khac cong thanh 20:00. WarCycleValue=7 trong citywar.ini [CitySettings] khop chu ky tuan.
- cw_CanStart(nCityId,nPhase) citywar_head.lua:32-51: nowday=tonumber(date('%y%m%d')); duyet TAB_NONE_CITYWAR[nPhase] (:8-14), moi phan tu {yymmdd} hoac {tu,den}; neu nowday trong khoang -> OutputMsg 'CityWar(%d) Can Not Start In This Phase(%d)' va return 0; nguoc lai return 1. nCityId CHI dung de in log. Bang nghi hien tai: 100414-100428, 70913-70930, 80317-80330, 81002-81016, 90402-90416, 91002-91016/91006; hang 4 (startcitywar) lech +1 ngay (100415, 70914-71001, 80318-80331, 81003, 90403, 91003) -> xac nhan cong thanh = ngay sau ghi danh.
- TaskSetMode / TaskSetStartDay (dich nguoc s3relay_y): TaskSetMode 0x080FEC30 doc 1 so -> setter 0x0811E24E: chi nhan 0..2, luu vao task+0x40. TaskInterval 0x080FEB4F -> 0x0811E146: mode 0: interval = n*60 (n PHUT, luu giay, 0x0811E16E-0x0811E17F: x*4*16-x*4 = x*60); mode 1 / mode 2: luu nguyen n (n NGAY cho mode 1 theo comment '一周一个循环' TaskInterval(7)); luu +0x2c va +0x50. TaskTime 0x080FEAAE -> 0x0811DCE8: gio 0..23 (+0x54), phut 0..59 (+0x58), giay=0 (+0x5c). TaskSetStartDay 0x080FECB6: 2 tham so -> nam mac dinh 0x7D5=2005 (0x080FECD6) + (thang, ngay); 3 tham so (0x080FED68-) = (nam,thang,ngay); setter 0x0811E2A4 kiem nam>1899 (0x76b), thang 1..12, ngay hop le theo thang (bitmask 0x15AA thang 31 ngay, 0xA50 thang 30 ngay, 4=thang 2 <=28/29). => Mode 1 = 'lich theo ngay': lan dau vao ngay StartDay, sau do moi TaskInterval ngay, tai TaskTime.
- KHAC BIET VN vs CN (citywar_head.lua VN 150 dong): (a) :99-108 cw_startsignup_fun: chi chay neu date('%w')==nweek va cw_CanStart(ncan,1); citywar_tbLadder:Reset(); LG_ApplySetLeagueTask(508, ten thanh, 1, 1) (task1=co 'dang mo ghi danh'); AddLocalNews; KHONG goi StartSignUp relay. (b) :115-145 cw_endsignup_fun: lay league 509 (LEAGUETYPE_CITYWAR_FIRST); GetRandomChallenger(:64-97) = duyet member league 508, lay LG_GetMemberTask(...,LGTSK_QINGTONGDING_COUNT=1) (so 'khieu chien lenh'), nhieu nhat thang, hoa random; neu GetCityOwner(ncan) rong -> GlobalExecute dwf citywar_function.lua citywar_appointviceroy(thanh, bang) (chiem thanh trong khong can danh), nguoc lai citywar_appointchallenger(thanh, bang); cuoi cung LG_ApplySetLeagueTask(508, thanh, 1, 0). KHONG goi EndSignUp/StartArena. (c) :109-113 cw_start_fun -> StartCityWar(ncan) (giong CN). (d) Ban CN: StartSignUp/EndSignUp/StartArena/StartCityWar la 4 API relay (0x080FF2E6/0x080FF356/0x080FF3C6/0x080FF436), loi dai quyet dinh khieu chien; ban VN bo loi dai, chon khieu chien bang boc tham luc 19:00.
- citywar_clearchallenger.lua (root, Task_66): :7-8 TaskInterval(1440) TaskTime(0,0); :11-12 va :16-17 goi clearCityWarLeague() + checkCityWarLeague() ca luc nap (trong TaskShedule) va 0:00 hang ngay: xoa het member league 508 va 509 cua 7 thanh (:42-68), tao lai league neu thieu (:20-40). Ban congthanhchien\citywar_clearchallenger.lua:9 doi sang 23:45 va :16 chi chay khi HinhThucCongThanh~=0.
- Thu muc congthanhchien\ (ban VN 'Heart Doldly', KHONG dang ky): citywar_head.lua:8 HinhThucCongThanh=0 (0 = dung API relay CN StartSignUp/EndSignUp/StartArena/StartCityWar, khac 0 = boc tham); :34-41 getSigningUpCity(nSel) tra thanh co TB_CITYWAR_ARRANGE[i][nSel]==weekday; :109,:125,:134 da comment bo cw_CanStart. 4 tep pha: citywar_startsignup_1.lua 18:00 (:6) -> StartSignUp(getSigningUpCity(1)) (:15-19); citywar_endsignup_2.lua 19:00 -> EndSignUp; citywar_startarena_3.lua 20:00 -> StartArena (chi khi HinhThucCongThanh==0, :12-14); citywar_startcitywar_4.lua 20:30 (:6) -> StartCityWar(getSigningUpCity(2)) (:11-14). Banner citywar_startsignup_1.lua:9-11: 'bao danh 18h-19h, tham gia 20h00 - ket thuc 21h30 Thu 6 hang tuan' (thuc ra la banner That Thanh Dai Chien). => Day la 1 bo task 'gop' chay moi ngay, tu chon thanh theo thu, y dinh khoi phuc loi dai CN (HinhThucCongThanh=0) nhung chua bat.
- sevencity_1..5 / sevencity_cleardata (root + congthanhchien, giong nhau): he 'That Thanh Dai Chien' rieng, chi thu 6 (day==5): 18:00 RelayProtocol:StartSignup, 19:00 CloseSignup, 20:00 BattleWorld:Clear()+Prepare, 20:30 Start, 21:30 BattleWorld:Close()+Close, 23:45 clear (Include \script\mission\sevencity\war.lua). Khong lien quan citywar_arena 213-220.
- citywar.ini (dung chung S3Relay/GameServer/S3Client): [CityArea] AreaName01..07 = Phượng Tường(map 1), Thành Đô(11), Đại Lý(162), Biện Kinh(37), Tương Dương(78), Dương Châu(80), Lâm An(176) -> city id 1..7 phai khop tham so StartSignUp/StartArena (comment citywar_0N_*.lua 'phai khop citywar.ini'). [CitySettings] SignUpFee=1000000 (phi ghi danh loi dai), MinTongLevel=18, MinTongCrowNumber=37, MaxExchangeTax=20, MaxPriceParam=20, StartSetTaxTime=22/EndSetTaxTime=23, WarCycleValue=7, SupplyLineBuildScale=30. [InitCityMaster] InitWithTopTongs=0.
- GetProductRegion: relay 0x08102082 doc ProductRegion qua giao dien 0x837ead4 -> chuoi tu bang 'cn, cn_ib, tw, my, vn, zh' (0x19B1BE..0x19B1D0); settings\product_config.ini ProductRegion=4 (4-VN) -> tra 'vn' -> tat ca tep root chay nhanh VN.

### cau_truc_du_lieu

Relay task object (s3relay_y, tu setter): +0x2c/+0x50 = interval (giay neu mode 0, don vi 'ngay' neu mode 1/2; <0 -> 0), +0x3c = con tro script (phai != 0 moi set duoc), +0x40 = mode (0..2; TaskSetMode), +0x54 = gio, +0x58 = phut, +0x5c = giay=0 (TaskTime). TaskSetStartDay(month,day) hoac (year,month,day) -> nam mac dinh 2005 (0x7D5 tai 0x080FECD6), kiem nam>1899, thang 1-12, ngay theo thang tai 0x0811E2AB-0x0811E398. tasklist.ini: [List] Count=128, [Task_N] TaskFile=..., ExcutedCount=-1 (TaskCountLimit(0) = khong gioi han). TB_CITYWAR_ARRANGE[i] = {weekday_ghi_danh, weekday_cong_thanh, ten_thanh} (i = city id 1..7 = AreaName0i trong citywar.ini). TAB_NONE_CITYWAR[phase][k] = {yymmdd} | {yymmdd_tu, yymmdd_den}. League 508 = LEAGUETYPE_CITYWAR_SIGN (ten = ten thanh; member = ten bang; task1 LGTSK_QINGTONGDING_COUNT = so khieu chien lenh, task cap league 1 = co dang mo ghi danh), 509 = LEAGUETYPE_CITYWAR_FIRST (khieu chien/thang loi dai).

### chua_ro

- Relay StartSignUp/EndSignUp/StartArena/StartCityWar gui goi gi xuong GS (noi dung protocol, StartArena co tu doc league 508 hay GS tu quan ly nguoi ghi danh loi dai) - can dich nguoc 0x080FF2E6..0x080FF436 (nhiem vu khac).
- Ban CN GOC cua startsignup/endsignup/startcitywar (truoc khi VN sua sang TaskInterval(1440)) chac chan dung TaskSetMode(1)+TaskSetStartDay nhu startarena - suy ra tu comment '星期X' + TAB_NONE_CITYWAR lech ngay, khong co tep goc de doi chieu.
- Ban CN co loi dai o 20:00 ngay D, nhung cong thanh 20:00 ngay D+1: thoi luong loi dai va cach GS chon 8 khe ArenaID (map 213-220) thuoc GS (citywar_arena\*.lua), chua doc trong nhiem vu nay.
- Gia tri cac chuoi log trong .bss (0x8389a78/ad8/adc/ae0/ae8/afc) cua TaskSetMode/Interval khong doc duoc tinh (nap luc chay) -> ten mode 2 chua ro (doan: mode 2 = chu ky thang/khac).
- citywar_clearchallenger root xoa league 508/509 luc 0:00 hang ngay - voi ban CN (loi dai 20:00 D, war 20:00 D+1) dieu nay xoa khieu chien 509 truoc cong thanh; chua xac dinh GS co giu ban sao khieu chien (AppointChallenger) doc lap voi league 509 hay khong.
