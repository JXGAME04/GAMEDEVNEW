# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 WAUTO B2] BO SINH BANG: doc giao dien WAuto.exe (ban PC) ra mot bang du lieu cho bang WAuto trong game mobile.

Nguon (CHI DOC):
  WAutoUI/WAuto.rc      (UTF-16)  vi tri / loai / nhan cua tung dieu khien, dat theo the (so hieu trong ten IDC_xxx_<N>_yyy)
  WAutoUI/WAuto.cpp     (UTF-16)  SaveRoleData(): dieu khien nao ghi vao truong nao cua autoData (tick / o nhap / hop chon);
                                  cac doan CB_ADDSTRING: danh sach lua chon cua hop chon (chuoi chu hoac mang tinh)
  WAutoUI/Resource.h    (UTF-8)   gia tri so cua IDC (de loai khoi tab 0 cac o thuoc the D.nhap: 174, 177..185)
  Sources/Core/Src/ipc_shared.h   bo cuc struct autoData (offset / kieu / co) - qua wauto_dat.doc_struct()

Ra: android/wauto_bang.json  +  thong ke in ra man hinh. Buoc sau (sinh_bocuc_wauto.py) doc JSON nay sinh .ini + bang C.
The BO (khong sinh giao dien, ma van giu): 15 = Ac chinh (IDC ..._16_...), 16 = D.nhap (dung chung dai ID tab 0).
Dung: python android/sinh_bang_wauto.py [--in]
"""
import io
import json
import os
import re
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wauto_dat import doc_struct  # noqa: E402

RC = os.path.join(GOC, "WAutoUI", "WAuto.rc")
CPP = os.path.join(GOC, "WAutoUI", "WAuto.cpp")
RES = os.path.join(GOC, "WAutoUI", "Resource.h")

# so hieu trong ten IDC -> the (ShowTab cua WAuto.cpp): 0..10 -> 0..10, 12 -> 11 (H.dong), 13 -> 12 (Sat thu),
# 14 -> 13 (Chieu KH), 15 -> 14 (CTC), 16 -> 15 (Ac chinh, BO). The 16 (D.nhap, BO) dung ID 174, 177..185 cua dai tab 0.
PREFIX_TAB = {0: 0, 1: 1, 2: 2, 3: 3, 4: 4, 5: 5, 6: 6, 7: 7, 8: 8, 9: 9, 10: 10, 12: 11, 13: 12, 14: 13, 15: 14, 16: 15}
TEN_TAB = ["Cơ bản", "Phục hồi", "Chiến đấu", "Di chuyển", "Nhặt đồ", "Hậu cần", "Tổ đội", "PK", "Dã Tẩu", "Tống Kim",
           "Liên đấu", "H.động", "Sát thủ", "Chiêu KH", "CTC", "Ác chính", "Đ.nhập"]
TAB_BO = {15, 16}
ID_DNHAP = {174} | set(range(177, 186))     # khoi dang nhap cua tab 0 da tach sang tab 16 (ShowTab)

# ---------------------------------------------------------------- [WAUTO 12/09] ba bang tay
# 1. NOI_TAY: dieu khien co nam trong SaveRoleData nhung viet theo kieu bo do tu dong khong nhan ra
#    (gan thang vao mang chuoi, CB_GETITEMDATA, cong them hang so...). Ghi ro o day cho khoi doan mo.
NOI_TAY = {
    "IDC_EDITOR_10_TEN":  ("szLDTen", "chuoi"),         # GetDlgItemTextA(..., gnode.apdata.szLDTen, ...) - khong qua bien tam
    "IDC_COMBO_9_RH":     ("nTKRuongHuong", "chon"),    # gia tri = MA HUONG lay tu CB_GETITEMDATA (0..4, 5 = gan nhat)
    "IDC_COMBO_13_BOSS":  ("nSTBoss", "chon"),          # gia tri = 141 + chi so dong (WAuto.cpp:1328)
    "IDC_COMBO_3_FL":     ("szFollName", "chuoi"),      # ghi o CBN_SELCHANGE (WAuto.cpp:7340), khong o SaveRoleData
    "IDC_CHECKBOX_7_PYD": ("nPriority", "tick"),        # cap radio voi 7_PYS: PYD = (nPriority == 0) -> VIEC 9 dao lai
    # ba o chu duoc WAuto dien luc chay tu chinh autoData (UpdateUI) - can offset de mobile in ra
    "IDC_STRING_3_CM":    ("szMoveMap", "chuoi"),
    "IDC_STRING_3_PTX":   ("nPointX", "int"),
    "IDC_STRING_3_PTY":   ("nPointY", "int"),
}

# 2. BO_O_MOBILE: KHONG dung o do trong giao dien mobile (ma WAuto / autoData giu nguyen - luat 0.2 cua lo trinh).
#    a) mot may mot nhan vat; b) dien thoai khong co thu do; c) da chuyen vao BANG PHU (UiWAutoDsach).
BO_O_MOBILE = {
    # (a) chi co nghia khi WAuto.exe nhin nhieu cua so game
    "IDC_CHECKBOX_0_HG":  "an cua so game Windows - dien thoai khong co cua so",
    # (b) hop thoai chon tep cua Windows / ban phim vat ly
    "IDC_BTN_3_RF": "doc tep", "IDC_BTN_3_WF": "ghi tep",
    "IDC_BTN_4_FTR": "doc tep", "IDC_BTN_4_FTW": "ghi tep",
    "IDC_BTN_6_RF": "doc tep", "IDC_BTN_6_WF": "ghi tep",
    # bUseFKey = "chi PK khi dang GIU phim": dien thoai khong co ban phim -> Wnd_IsPKKeyDown() luon 0, bat len la
    # PK khong bao gio chay. Bo ca o tick lan o phim; JxWAutoNoiBo ep bUseFKey = 0 moi nhip (tep .dat cua ban PC
    # chep sang co the dang bat).
    "IDC_CHECKBOX_7_K": "chi PK khi giu phim - dien thoai khong co ban phim",
    "IDC_EDITOR_7_K": "bat phim ban phim vat ly",
    # (c) da chuyen vao bang phu cua danh sach (them / bot / xep thu tu nam trong bang do)
    "IDC_BTN_3_GE": "", "IDC_BTN_3_UP": "", "IDC_BTN_3_DN": "", "IDC_BTN_3_DL": "", "IDC_BTN_3_DA": "",
    "IDC_COMBO_4_FT": "", "IDC_EDITOR_4_FT": "", "IDC_STRING_4_FT": "",
    "IDC_BTN_4_FTD": "", "IDC_BTN_4_FTDA": "",
    "IDC_COMBO_6_NE": "", "IDC_BTN_6_NE": "", "IDC_BTN_6_UP": "", "IDC_BTN_6_DN": "", "IDC_BTN_6_DL": "", "IDC_BTN_6_DA": "",
    "IDC_COMBO_10_PT": "", "IDC_BTN_10_PTA": "", "IDC_BTN_10_PTD": "",
}

# 3. VIEC: ma hanh dong cho UiWAutoTrang.cpp (thay cho so sanh chuoi IDC luc chay). Khop WA_V_* trong UiWAutoBang.h.
VIEC = {
    "IDC_BTN_3_MG":     1,      # Di chuyen: lay ban do dang dung
    "IDC_BTN_3_PT":     2,      # Di chuyen: lay toa do dang dung lam diem co dinh
    "IDC_COMBO_3_FL":   3,      # Di chuyen: theo sau (ten nguoi choi quanh day -> szFollName)
    "IDC_LIST_3_CD":    4,      # Di chuyen: danh sach toa do di tuan
    "IDC_LIST_4_FT":    5,      # Nhat do: danh sach luat loc thuoc tinh
    "IDC_BTN_4_NOP":    6,      # Nhat do: bang ten khong nhat
    "IDC_LIST_6_NE":    7,      # To doi: danh sach ten moi / nhan nhom
    "IDC_BTN_7_PYS":    8,      # PK: thu tu ngu hanh
    "IDC_CHECKBOX_7_PYD": 9,    # PK: uu tien theo khoang cach (nPriority = 0)
    "IDC_CHECKBOX_7_PYS": 13,   # PK: uu tien theo ngu hanh (nPriority = 1) - cap radio voi 7_PYD
    "IDC_LIST_10_PT":   10,     # Lien dau: danh sach ban dien
    "IDC_COMBO_9_RH":   11,     # Tong Kim: ruong cua (danh sach doi theo thanh se ve)
    "IDC_COMBO_13_BOSS": 12,    # Sat thu: boss co dinh (gia tri = 141 + dong)
}

# 4. LOC_CHIEU: WAuto.exe che danh sach chieu thanh 7 danh sach con (WAuto.cpp:3057-3097); game chi tra MOT
#    danh sach phang -> phai loc lai ben mobile. Khop WA_LC_* trong UiWAutoBang.h.
LOC_CHIEU = {"SkillKAr": 1, "SkillSAr": 2, "SkillSEAr": 3, "SkillBAr": 4, "SkillPAr": 4, "SkillAAr": 5}

# 5. SO_LIEU: o chu duoc DIEN LUC CHAY tu trang thai nhan vat (WAuto.exe dien trong ProcIpcCommand / UpdateUI).
#    Gia tri = ma so lieu, khop WA_SL_* trong UiWAutoBang.h.
SO_LIEU = {
    "IDC_STRING_0_NL": 1, "IDC_STRING_0_NM": 2, "IDC_STRING_0_NS": 3,
    "IDC_STRING_0_MN": 4, "IDC_STRING_0_MC": 5, "IDC_STRING_0_NLV": 6, "IDC_STRING_0_NE": 7,
    "IDC_STRING_3_CM": 8, "IDC_STRING_3_PTX": 9, "IDC_STRING_3_PTY": 10,
}

CTL_RE = re.compile(r'^\s*(CONTROL|PUSHBUTTON|DEFPUSHBUTTON|EDITTEXT|COMBOBOX|LTEXT|RTEXT|CTEXT|GROUPBOX|LISTBOX)\b(.*)$')


def doc_u16(p):
    return io.open(p, "rb").read().decode("utf-16-le").replace("\r\n", "\n")


def parse_resource_h():
    s = io.open(RES, encoding="utf-8").read()
    return {m.group(1): int(m.group(2)) for m in re.finditer(r"#define\s+(IDC_\w+)\s+(\d+)", s)}


def parse_rc():
    """-> danh sach dieu khien cua hop thoai chinh: dict(kind, idc, label, x, y, w, h, style)"""
    s = doc_u16(RC)
    a = s.index("IDD_DLG_WAUTOMAIN DIALOGEX")
    b = s.index("\nEND", a)
    out = []
    for line in s[a:b].splitlines():
        m = CTL_RE.match(line)
        if not m:
            continue
        kind, rest = m.group(1), m.group(2)
        # tach cac tham so theo dau phay, NHUNG dau phay TRONG nhay kep khong tinh.
        # [WAUTO 12/09 a] Truoc day dung re.findall(r'"..."|[^,]+') - nhanh [^,]+ AN TRUOC ca dau nhay mo khi
        # phia truoc con khoang trang, nen nhan nao CO DAU PHAY thi ca dong dieu khien bi vut. Mat 5 dieu khien
        # that: IDC_CHECKBOX_0_MIS / _4_RG / _6_TL / _12_TS va nhan IDC_STRING_12_CAY. Nay quet tung ky tu.
        parts, buf, i = [], "", 0
        while i < len(rest):
            ch = rest[i]
            if ch == '"':
                j = rest.find('"', i + 1)
                if j < 0:
                    j = len(rest) - 1
                buf += rest[i:j + 1]
                i = j + 1
                continue
            if ch == ",":
                parts.append(buf)
                buf = ""
                i += 1
                continue
            buf += ch
            i += 1
        parts.append(buf)
        parts = [p.strip() for p in parts if p.strip()]
        label = ""
        if parts and parts[0].startswith('"'):
            label = parts.pop(0)[1:-1]
        if not parts:
            continue
        idc = parts.pop(0)
        if not idc.startswith("IDC_") and idc not in ("IDOK", "IDCANCEL"):
            continue
        cls = ""
        style = ""
        if kind == "CONTROL":
            cls = parts.pop(0).strip('"') if parts else ""
            style = parts.pop(0) if parts else ""
        nums = [p for p in parts if re.match(r"^-?\d+$", p)]
        if len(nums) < 4:
            continue
        x, y, w, h = [int(v) for v in nums[:4]]
        if kind == "CONTROL":
            if "BS_AUTOCHECKBOX" in style or "BS_AUTO3STATE" in style:
                k2 = "tick"
            elif cls == "Button":
                k2 = "nut"
            elif cls == "Static":
                k2 = "chu"
            elif "SysListView32" in cls:
                k2 = "bang"
            elif "msctls_trackbar" in cls:
                k2 = "truot"
            else:
                k2 = "khac:" + cls
        else:
            k2 = {"PUSHBUTTON": "nut", "DEFPUSHBUTTON": "nut", "EDITTEXT": "onhap", "COMBOBOX": "chon", "LTEXT": "chu",
                  "RTEXT": "chu", "CTEXT": "chu", "GROUPBOX": "khung", "LISTBOX": "dsach"}[kind]
        out.append(dict(kind=k2, idc=idc, label=label, x=x, y=y, w=w, h=h))
    return out


def tab_cua(idc, idc_val):
    m = re.match(r"IDC_[A-Z]+_(\d+)_", idc)
    if not m:
        return None
    n = int(m.group(1))
    tab = PREFIX_TAB.get(n)
    if tab == 0 and idc_val.get(idc) in ID_DNHAP:
        return 16
    return tab


def parse_bindings(cpp):
    """SaveRoleData -> {idc: (field, cach)} ; cach = tick / int / chuoi / chon"""
    a = cpp.index("static void SaveRoleData(")
    b = cpp.index("static void SaveRoleDataFast(")
    s = cpp[a:b]
    bind = {}
    for m in re.finditer(r"apdata\.(\w+(?:\[\w+\])?)\s*=\s*\(IsDlgButtonChecked\(hDlg,\s*(IDC_\w+)(?:\s*\+\s*\w+)?\)", s):
        bind[m.group(2)] = (m.group(1), "tick")
    # hop chon dang gon: apdata.X = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_x), CB_GETCURSEL, 0, 0);
    for m in re.finditer(r"apdata\.(\w+(?:\[\w+\])?)\s*=\s*\(int\)SendMessage\(GetDlgItem\(hDlg,\s*(IDC_COMBO_\w+)\),\s*CB_GETCURSEL", s):
        bind[m.group(2)] = (m.group(1), "chon")
    LOOPED.update(m.group(1) for m in re.finditer(r"(IDC_\w+)\s*\+\s*\w+", s))
    lines = s.splitlines()
    for i, l in enumerate(lines):
        m = re.search(r"GetDlgItemTextA?\(hDlg,\s*(IDC_\w+)(?:\s*\+\s*\w+)?,", l)
        if m:
            idc = m.group(1)
            for l2 in lines[i:i + 4]:
                m2 = re.search(r"apdata\.(\w+(?:\[\w+\])?)\s*=\s*(?:\(short\)|\(UINT\)|\(int\))?\s*atoi\(", l2)
                if m2:
                    bind[idc] = (m2.group(1), "int")
                    break
                m3 = re.search(r"strncpy\(gnode\.apdata\.(\w+)", l2)
                if m3:
                    bind[idc] = (m3.group(1), "chuoi")
                    break
        m = re.search(r"GetDlgItem\(\s*hDlg,\s*(IDC_COMBO_\w+)(?:\s*\+\s*\w+)?\s*\)", l)
        if m and not re.search(r"CB_GETCURSEL", l):
            idc = m.group(1)
            for l2 in lines[i:i + 9]:
                m2 = re.search(r"apdata\.(\w+(?:\[\w+\])?)\s*=\s*\(int\)SendMessage\(hCtrl,\s*CB_GETCURSEL", l2)
                if m2:
                    bind[idc] = (m2.group(1), "chon")
                    break
                m4 = re.search(r"apdata\.(\w+(?:\[\w+\])?)\s*=\s*gnode\.arSkill\[gnode\.(\w+)\[", l2)
                if m4:
                    bind[idc] = (m4.group(1), "chieu:" + m4.group(2))   # hop chon CHIEU: gia tri = ma chieu, danh sach loc theo SkillXAr
                    break
    return bind


LOOPED = set()      # IDC dung dang "IDC_x + k" trong vong lap: cac IDC lien tiep sau no cung mot truong mang (chi so k)


def mo_rong_vong_lap(tabs, off, idc_val):
    val_idc = {v: k for k, v in idc_val.items()}
    for tab, rows in tabs.items():
        for r in list(rows):
            if r["idc"] in LOOPED and r.get("truong") and off.get(r["truong"], {}).get("dims") and r.get("id") is not None:
                n = off[r["truong"]]["dims"][0]
                r["chi_so"] = 0
                for k in range(1, n):
                    idk = val_idc.get(r["id"] + k)
                    for r2 in rows:
                        if r2["idc"] == idk and "truong" not in r2:
                            r2.update(truong=r["truong"], chi_so=k, cach=r["cach"], off=r["off"], kieu=r["kieu"], co=r["co"])
                            if "loc_chieu" in r:     # [WAUTO 12/09] 6 khe Chieu ket hop deu loc theo SkillKAr
                                r2["loc_chieu"] = r["loc_chieu"]


def gan_chi_so_mang(tabs, off):
    """Truong MANG ghi trong vong lap (bTKGio[tki], bDTType[dti], nComboSkill[cki]...): cac IDC lien tiep cung truong
    khong co chi so -> gan 0, 1, 2... theo thu tu gia tri IDC."""
    for tab, rows in tabs.items():
        nhom = {}
        for r in rows:
            t = r.get("truong")
            if t and r.get("chi_so") is None and off.get(t, {}).get("dims"):
                nhom.setdefault(t, []).append(r)
        for t, rs in nhom.items():
            rs.sort(key=lambda r: (r.get("id") or 0))
            for i, r in enumerate(rs):
                r["chi_so"] = i


def parse_arrays(cpp):
    """mang chuoi tinh: static const wchar_t* [const] ten[...] = { L"..", ... }"""
    arr = {}
    for m in re.finditer(r"static\s+const\s+wchar_t\s*\*\s*(?:const\s+)?(\w+)\s*\[[^\]]*\]\s*=\s*\{(.*?)\};", cpp, re.S):
        arr[m.group(1)] = re.findall(r'L"((?:[^"\\]|\\.)*)"', m.group(2))
    return arr


def parse_options(cpp, arrays):
    """{idc combo: [chuoi] hoac 'dong:<ten mang / bieu thuc>'} - theo thu tu GetDlgItem(...IDC_COMBO...) roi CB_ADDSTRING."""
    opts = {}
    cur = None
    for l in cpp.splitlines():
        m = re.search(r"GetDlgItem\(\s*hDlg,\s*(IDC_COMBO_\w+)\s*\)", l)
        if m:
            cur = m.group(1)
            continue
        if "GetDlgItem(" in l:
            # [WAUTO 12/09 b] lay hCtrl cua MOT O KHAC (vi du "IDC_COMBO_14_S1 + cki" trong vong lap) -> QUEN hop dang theo.
            # Truoc day cur giu nguyen nen 20 ten boss cua IDC_COMBO_13_BOSS bi thay bang danh sach chieu cua vong lap ke tren.
            cur = None
            continue
        if "CB_RESETCONTENT" in l and cur:
            opts.setdefault(cur, [])
            continue
        m = re.search(r"CB_ADDSTRING,\s*0,\s*\(LPARAM\)\s*(.+?)\)\s*;", l)
        if m and cur:
            v = m.group(1).strip()
            m2 = re.match(r'L"((?:[^"\\]|\\.)*)"', v)
            lst = opts.setdefault(cur, [])
            if m2:
                if not (lst and isinstance(lst[-1], str) and lst[-1].startswith("dong:")):
                    lst.append(m2.group(1))
            else:
                m3 = re.match(r"(\w+)\[\w+\]", v)
                if m3 and m3.group(1) in arrays:
                    if lst == []:
                        lst.extend(arrays[m3.group(1)])
                else:
                    lst.append("dong:" + v)
    return opts


def parse_magic(cpp):
    """[WAUTO 12/09] g_MagicTable cua WAuto.cpp: 40 dong thuoc tinh cho bang LOC khi nhat do -> [(ma, ten)]"""
    a = cpp.index("static const MAGIC_INFO g_MagicTable")
    b = cpp.index("};", a)
    return [(int(m.group(1)), m.group(2)) for m in re.finditer(r'\{\s*(\d+),\s*L"([^"]*)"', cpp[a:b])]


def parse_tk_ruong(cpp):
    """[WAUTO 12/09] s_aTKRCo[7][5]: thanh nao CO huong ruong nao (hop 'Ruong cua' cua the Tong Kim doi theo thanh se ve)"""
    a = cpp.index("static const BYTE s_aTKRCo")
    b = cpp.index("};", a)
    return [[int(v) for v in re.findall(r"\d", m.group(1))] for m in re.finditer(r"\{\s*([01],\s*[01],\s*[01],\s*[01],\s*[01])\s*\}", cpp[a:b])]


def main():
    idc_val = parse_resource_h()
    cpp = doc_u16(CPP)
    truong, tong, macdinh = doc_struct()
    off = {t[0]: dict(kieu=t[1], dims=t[2], off=t[3], co=t[4]) for t in truong}
    ctls = parse_rc()
    bind = parse_bindings(cpp)
    arrays = parse_arrays(cpp)
    opts = parse_options(cpp, arrays)
    tabs = {}
    thongke = dict(dieu_khien=0, noi=0, khong_noi=0, chon_tinh=0, chon_dong=0, bo=0, bo_mobile=0)
    for c in ctls:
        tab = tab_cua(c["idc"], idc_val)
        if tab is None:
            continue            # dai nut chung / bang nhan vat / vach ngan
        if tab in TAB_BO:
            thongke["bo"] += 1
            continue
        if c["idc"] in BO_O_MOBILE:     # [WAUTO 12/09] khong dung o do tren dien thoai (ma WAuto giu nguyen)
            thongke["bo_mobile"] += 1
            continue
        thongke["dieu_khien"] += 1
        r = dict(c)
        r["id"] = idc_val.get(c["idc"])
        if c["idc"] in VIEC:
            r["viec"] = VIEC[c["idc"]]
        if c["idc"] in SO_LIEU:
            r["so_lieu"] = SO_LIEU[c["idc"]]
        b = bind.get(c["idc"]) or NOI_TAY.get(c["idc"])
        if b:
            ten, cach = b
            goc = re.match(r"(\w+)", ten).group(1)
            chi_so = re.search(r"\[(\d+)\]", ten)
            f = off.get(goc)
            r.update(truong=goc, chi_so=int(chi_so.group(1)) if chi_so else None, cach=cach,
                     off=f["off"] if f else None, kieu=f["kieu"] if f else None, co=f["co"] if f else None)
            if str(cach).startswith("chieu:"):
                r["loc_chieu"] = LOC_CHIEU.get(str(cach).split(":", 1)[1], 1)
            thongke["noi"] += 1
        elif c["kind"] in ("tick", "onhap", "chon"):
            thongke["khong_noi"] += 1
        if c["kind"] == "chon":
            o = opts.get(c["idc"], [])
            r["lua_chon"] = o
            if o and all(not str(x).startswith("dong:") for x in o):
                thongke["chon_tinh"] += 1
            else:
                thongke["chon_dong"] += 1
        tabs.setdefault(tab, []).append(r)
    gan_chi_so_mang(tabs, off)
    mo_rong_vong_lap(tabs, off, idc_val)
    thongke["noi"] = sum(1 for v in tabs.values() for r in v if r.get("truong"))
    thongke["khong_noi"] = sum(1 for v in tabs.values() for r in v if r["kind"] in ("tick", "onhap", "chon") and not r.get("truong"))
    thongke["chieu"] = sum(1 for v in tabs.values() for r in v if str(r.get("cach", "")).startswith("chieu"))
    out = dict(ten_tab=TEN_TAB, tabs={str(k): v for k, v in sorted(tabs.items())}, sizeof_autoData=tong, thongke=thongke,
               magic=parse_magic(cpp), tk_ruong_co=parse_tk_ruong(cpp))
    p = os.path.join(GOC, "android", "wauto_bang.json")
    io.open(p, "w", encoding="utf-8", newline="\n").write(json.dumps(out, ensure_ascii=False, indent=1))
    print("da ghi", p)
    print("thong ke:", thongke)
    for k in sorted(tabs):
        v = tabs[k]
        kinds = {}
        for r in v:
            kinds[r["kind"]] = kinds.get(r["kind"], 0) + 1
        chua = [r["idc"] for r in v if r["kind"] in ("tick", "onhap", "chon") and "truong" not in r]
        print("  tab %2d %-10s %3d dk %s%s" % (k, TEN_TAB[k], len(v), kinds, ("  CHUA NOI: " + ",".join(chua)) if chua else ""))
    if "--in" in sys.argv:
        for k in sorted(tabs):
            print("=== tab", k, TEN_TAB[k])
            for r in tabs[k]:
                print("   %-6s %-24s %-22s %s %s" % (r["kind"], r["idc"], r.get("truong", "-"), r["label"][:28], r.get("lua_chon", "")))


if __name__ == "__main__":
    main()
