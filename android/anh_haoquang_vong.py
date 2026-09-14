# -*- coding: utf-8 -*-
r"""[HAOQUANG 14/09] Sinh anh VONG HAO QUANG duoi chan (chu 14/09: "hay lam va co nut tat mo" sau muc 10.2 mo game 3D Kiem Vong Giang Ho:
halo_npc_purple/gold/pink, halo_boss_red = vong phap tran phang tren dat gan sys_foot, song thuong truc; 09:0x: "vong hao quang do mac
phai lam giong nhu 3d, khong dung vong sang co san"):
  * spr\haoquang\vongquai.spr  : 12 khung 128x64 (elip 2:1), neo (64, 32) = tam vong tai diem dat chan NPC. Quai tinh anh (boss_blue...).
  * spr\haoquang\vongboss.spr  : 16 khung 176x88, neo (88, 44). Boss (boss_gold / boss_event / boss_war).
  * spr\haoquang\vongnguoi.spr : 16 khung 144x72, neo (72, 36). Vong DO MAC cua nhan vat minh (kieu khac quai: hai vong dong tam + 6 canh
                                 hoa quay + 12 nut nguoc chieu + quang giua), nhuom theo pham chat trang bi.
Anh TRANG: KNpc::Paint ve bang IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST nhan mau (quai theo m_Type = BOSS_STATE cung mau ten quai KNpc.cpp:8527,
nguoi theo pham chat) -> mot anh dung cho moi mau. Ve o do phan giai gap doi tren khung TRON roi ep 2:1 (LANCZOS) nen vach quay dung phoi canh.
Dinh dang SPR nhu android/anh_vatroi_cotsang.py; ALPHA 8 BIT 0..255 (TextureRes.cpp RenderToA8R8G8B8: a << 24 - KHONG phai 0..31).
Ghi vao lop ghi de android\du_lieu_ghi_de\spr\haoquang\, D:\jx1_android_data\spr\haoquang\ (may ao) va tuy chon goi may chu tai (tham so 1,
nho --chi-manifest). Xem truoc: --xem <png>.
Dung: python android\anh_haoquang_vong.py [D:\jx1_android_data_dt_v4] [--xem out.png]
"""
import math
import os
import struct
import sys

from PIL import Image, ImageDraw, ImageFilter

GOC = os.path.dirname(os.path.abspath(__file__))
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "spr", "haoquang"), os.path.join(r"D:\jx1_android_data", "spr", "haoquang")]
XEM = None
args = [a for a in sys.argv[1:]]
if "--xem" in args:
    i = args.index("--xem"); XEM = args[i + 1]; del args[i:i + 2]
if args:
    DICH.append(os.path.join(args[0], "spr", "haoquang"))

SS = 2      # sieu lay mau


def vong_khung(i, n, r, boss):
    """Khung i/n cua vong QUAI/BOSS ban kinh r (px man hinh, chieu ngang). Ve tren khung tron 2r x 2r o do phan giai SS, ep ve 2r x r."""
    D = 2 * r * SS
    c = D / 2.0
    R = r * SS
    im = Image.new("RGBA", (D, D), (255, 255, 255, 0))
    # 1) quang mo (ring rong roi lam nhoe)
    glow = Image.new("RGBA", (D, D), (255, 255, 255, 0))
    dg = ImageDraw.Draw(glow)
    dg.ellipse((c - R * 0.97, c - R * 0.97, c + R * 0.97, c + R * 0.97), outline=(255, 255, 255, 150), width=int(6 * SS))
    glow = glow.filter(ImageFilter.GaussianBlur(4 * SS))
    im.alpha_composite(glow)
    if boss:
        # nen mo o giua (phap tran)
        nen = Image.new("RGBA", (D, D), (255, 255, 255, 0))
        dn = ImageDraw.Draw(nen)
        dn.ellipse((c - R * 0.55, c - R * 0.55, c + R * 0.55, c + R * 0.55), fill=(255, 255, 255, 34))
        nen = nen.filter(ImageFilter.GaussianBlur(3 * SS))
        im.alpha_composite(nen)
    d = ImageDraw.Draw(im)
    # 2) vong ngoai sac
    d.ellipse((c - R * 0.93, c - R * 0.93, c + R * 0.93, c + R * 0.93), outline=(255, 255, 255, 235), width=int(1.6 * SS))
    # 3) vong trong dut khuc quay (so vach DV, moi vach chiem 55 %): goc quay moi khung = 360/DV/n -> lap kin
    DV = 8 if not boss else 10
    a0 = 360.0 / DV / n * i
    r2 = R * (0.74 if not boss else 0.78)
    for k in range(DV):
        s = a0 + k * 360.0 / DV
        d.arc((c - r2, c - r2, c + r2, c + r2), start=s, end=s + 360.0 / DV * 0.55, fill=(255, 255, 255, 215), width=int(2.2 * SS))
    # 4) nut sang tren vong ngoai, quay nguoc (so nut KN, goc moi khung = 360/KN/n)
    KN = 4 if not boss else 6
    b0 = -360.0 / KN / n * i
    for k in range(KN):
        t = math.radians(b0 + k * 360.0 / KN)
        x = c + R * 0.93 * math.cos(t)
        y = c + R * 0.93 * math.sin(t)
        rr = 3.2 * SS
        d.ellipse((x - rr, y - rr, x + rr, y + rr), fill=(255, 255, 255, 255))
        # duoi sang nho phia sau nut
        for j in range(1, 5):
            tt = math.radians(b0 + k * 360.0 / KN - j * 4.0)
            xx = c + R * 0.93 * math.cos(tt)
            yy = c + R * 0.93 * math.sin(tt)
            r3 = rr * (1.0 - j * 0.18)
            d.ellipse((xx - r3, yy - r3, xx + r3, yy + r3), fill=(255, 255, 255, int(200 - j * 40)))
    if boss:
        # 5) boss: them 3 cung dai ngoai cung quay cung chieu vach, cham hon
        r4 = R * 0.995
        a1 = 360.0 / 3 / n * i * 0.5
        for k in range(3):
            s = a1 + k * 120.0
            d.arc((c - r4, c - r4, c + r4, c + r4), start=s, end=s + 50, fill=(255, 255, 255, 170), width=int(1.3 * SS))
    # ep 2:1 + ha do phan giai
    return im.resize((2 * r, r), Image.LANCZOS)


def vong_nguoi(i, n, r):
    """Khung i/n cua vong DO MAC (nguoi choi): hai vong dong tam, 6 canh hoa quay xuoi, 12 nut quay nguoc, quang mo giua. Khac vong quai."""
    D = 2 * r * SS
    c = D / 2.0
    R = r * SS
    im = Image.new("RGBA", (D, D), (255, 255, 255, 0))
    # quang mo ngoai + quang giua
    glow = Image.new("RGBA", (D, D), (255, 255, 255, 0))
    dg = ImageDraw.Draw(glow)
    dg.ellipse((c - R * 0.96, c - R * 0.96, c + R * 0.96, c + R * 0.96), outline=(255, 255, 255, 130), width=int(5 * SS))
    dg.ellipse((c - R * 0.42, c - R * 0.42, c + R * 0.42, c + R * 0.42), fill=(255, 255, 255, 48))
    glow = glow.filter(ImageFilter.GaussianBlur(4 * SS))
    im.alpha_composite(glow)
    d = ImageDraw.Draw(im)
    # hai vong dong tam
    d.ellipse((c - R * 0.95, c - R * 0.95, c + R * 0.95, c + R * 0.95), outline=(255, 255, 255, 225), width=int(1.4 * SS))
    d.ellipse((c - R * 0.62, c - R * 0.62, c + R * 0.62, c + R * 0.62), outline=(255, 255, 255, 150), width=int(1.2 * SS))
    # 6 canh hoa (cung day) giua hai vong, quay xuoi: goc moi khung = 60/n
    a0 = 60.0 / n * i
    r2 = R * 0.80
    for k in range(6):
        s = a0 + k * 60.0
        d.arc((c - r2, c - r2, c + r2, c + r2), start=s + 8, end=s + 44, fill=(255, 255, 255, 205), width=int(5.5 * SS))
        # dau canh nhon: cham nho o hai dau
        for e in (s + 6, s + 46):
            t = math.radians(e)
            x = c + r2 * math.cos(t); y = c + r2 * math.sin(t)
            rr = 1.6 * SS
            d.ellipse((x - rr, y - rr, x + rr, y + rr), fill=(255, 255, 255, 120))
    # 12 nut tren vong ngoai, quay nguoc: goc moi khung = 30/n
    b0 = -30.0 / n * i
    for k in range(12):
        t = math.radians(b0 + k * 30.0)
        x = c + R * 0.95 * math.cos(t); y = c + R * 0.95 * math.sin(t)
        rr = (2.6 if k % 3 == 0 else 1.7) * SS
        d.ellipse((x - rr, y - rr, x + rr, y + rr), fill=(255, 255, 255, 255 if k % 3 == 0 else 200))
    return im.resize((2 * r, r), Image.LANCZOS)


def rle_khung(im):
    w, h = im.size
    px = im.load()
    out = bytearray()
    for y in range(h):
        x = 0
        while x < w:
            a = px[x, y][3]
            q = 0 if a < 8 else a          # alpha 8 bit 0..255 (khong phai 0..31)
            n = 1
            while x + n < w and n < 255:
                a2 = px[x + n, y][3]
                q2 = 0 if a2 < 8 else a2
                if q2 != q:
                    break
                n += 1
            out += bytes((n, q))
            if q:
                out += b"\x00" * n
            x += n
    return bytes(out)


def ghi_spr(cac_khung, path, cx, cy, itv):
    w, h = cac_khung[0].size
    pal = bytes([255, 255, 255]) + b"\x00" * (768 - 3)
    kb = [struct.pack("<HHHH", w, h, 0, 0) + rle_khung(k) for k in cac_khung]
    out = bytearray(struct.pack("<4sHHHHHHHH", b"SPR\x00", w, h, cx, cy, len(kb), 256, 1, itv) + b"\x00" * 12)
    out += pal
    o = 0
    for k in kb:
        out += struct.pack("<II", o, len(k)); o += len(k)
    for k in kb:
        out += k
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    return len(out)


def nhuom(k, m):
    r, g, b, a = k.split()
    return Image.merge("RGBA", (r.point(lambda v, m=m: m[0]), g.point(lambda v, m=m: m[1]), b.point(lambda v, m=m: m[2]), a))


def main():
    quai = [vong_khung(i, 12, 64, False) for i in range(12)]      # 128x64
    boss = [vong_khung(i, 16, 88, True) for i in range(16)]       # 176x88
    nguoi = [vong_nguoi(i, 16, 72) for i in range(16)]            # 144x72
    for dich in DICH:
        n1 = ghi_spr(quai, os.path.join(dich, "vongquai.spr"), 64, 32, 70)
        n2 = ghi_spr(boss, os.path.join(dich, "vongboss.spr"), 88, 44, 70)
        n3 = ghi_spr(nguoi, os.path.join(dich, "vongnguoi.spr"), 72, 36, 70)
        print("da ghi:", dich, "vongquai.spr %d B, vongboss.spr %d B, vongnguoi.spr %d B" % (n1, n2, n3))
    if XEM:
        # xem truoc: hang 1 quai/boss 5 mau; hang 2 vong nguoi 4 pham chat (hoang kim, do tim, bach kim, xanh) + 4 khung
        mau = [(110, 120, 255), (255, 217, 78), (230, 90, 255), (255, 60, 40), (60, 220, 60)]
        W, H = 5 * 190 + 20, 20 + 88 + 20 + 72 + 20 + 72 + 20
        nen = Image.new("RGBA", (W, H), (58, 66, 40, 255))
        dd = ImageDraw.Draw(nen)
        for i, m in enumerate(mau):
            k = quai[(i * 3) % 12] if i != 1 and i != 3 else boss[(i * 3) % 16]
            k = nhuom(k, m)
            x = 10 + i * 190 + (176 - k.size[0]) // 2
            nen.alpha_composite(k, (x, 20 + (88 - k.size[1]) // 2))
            cx, cy = 10 + i * 190 + 88, 20 + 44
            dd.rectangle((cx - 8, cy - 46, cx + 8, cy), fill=(120, 90, 60, 255))
        for i, m in enumerate([(255, 217, 78), (230, 90, 255), (200, 240, 255), (90, 230, 90)]):
            k = nhuom(nguoi[(i * 4) % 16], m)
            nen.alpha_composite(k, (10 + i * 190 + 16, 128))
            cx, cy = 10 + i * 190 + 88, 128 + 36
            dd.rectangle((cx - 8, cy - 46, cx + 8, cy), fill=(120, 90, 60, 255))
        for i in range(5):
            nen.alpha_composite(nguoi[i * 3], (10 + i * 190 + 16, 220))
        nen.convert("RGB").save(XEM)
        print("xem truoc:", XEM)


if __name__ == "__main__":
    main()
