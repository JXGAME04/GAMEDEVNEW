/*
 * KMachineId.h - [MAYID 14/09] Ma may on dinh cho client PC.
 *
 * Vi sao co tep nay: ma may cu lay tu GetCurrentHwProfile().szHwProfileGuid, ma gia tri do chi la mot khoa
 * registry sinh luc CAI Windows. Phong net ghost dia tu mot may mau => CA DAN MAY mang CUNG MOT GUID.
 * Khi ca phong lai ra Internet qua cung mot tuong lua (cung mot IP), may chu thay ca phong la MOT may
 * => nguoi vao duoc nguoi khong. Day la NGUYEN NHAN that su cua trieu chung do.
 *
 * Mo-dun nay lay ma may tu PHAN CUNG THAT - thu KHONG di theo khi nhan ban dia:
 *   1. UUID he thong trong SMBIOS  (gan voi BO MACH CHU)
 *   2. So se-ri O DIA VAT LY        (gan voi o dia, khong phai volume serial)
 *   3. Dia chi MAC cua card mang that
 *
 * Chuoi tra ve: <hang><32 chu so hex>  = 33 ky tu, vua trong sHWID[64] va con cho may chu noi them " <IP>".
 *   hang 'A' = tim duoc >= 2 nguon manh
 *   hang 'B' = tim duoc 1 nguon manh
 *   hang 'C' = KHONG co nguon manh nao
 *
 * !! HANG KHONG PHAI LA GIAY MIEN - doc ky truoc khi dung o may chu.
 * Ban nhap dau tien cua tep nay ghi "hang 'C' thi may chu khong duoc tu choi dang nhap". DO LA SAI va da bo:
 * hang cung do CLIENT khai, nen client sua doi chi can gui "C000..." la thoat sach gioi han - dung bang mot
 * cua hau, chinh la lo ma viec nay dinh bit. Hang 'C' doi CA BA nguon cung that bai, ma rieng MAC thi gan nhu
 * may Windows nao cung doc duoc, nen hang 'C' that su gan bang khong.
 *   => Thay NHIEU hang 'C' = dau hieu CLIENT GIA, khong phai dau hieu may yeu.
 *   => May chu VAN DEM hang 'C' nhu thuong; hang chi dung de GHI LOG va theo doi, khong dung de mien tru.
 *
 * Ham an toan tuyet doi: khong bao gio nem, khong bao gio tra NULL, tinh MOT LAN roi nho lai.
 */

#ifndef JX_KMACHINEID_H
#define JX_KMACHINEID_H

/*
 * Tra ve ma may (33 ky tu + NUL). Con tro tinh, khong duoc giai phong.
 */
const char* JX_GetMachineId(void);

/*
 * Hang tin cay cua ma may: 'A', 'B' hoac 'C'. Bang ky tu dau cua JX_GetMachineId().
 */
char JX_GetMachineIdTier(void);

#endif /* JX_KMACHINEID_H */
