# -*- coding: utf-8 -*-
# [MATDO 06/09 dem] Chu game: "nhan vat CaiBang cua toi dang bi tinh trang hay bi mat do".
#
# GOC (do tren role_history cua chinh CaiBang, jx1_role):
#   Ban luu 20:45:49 co 17 mon "dang mac" nhung chi 14 O trang bi: O 4, O 5, O 10 moi o chua HAI mon.
#   Ban 20:51 con 15 mon (o 7 trung 2), ban 21:09 con 14 (het trung). Moi lan nap lai MAT dung so
#   mon thua: dt=7 pt=8 cap=8, dt=2 pt=3 cap=9, ... Tien KHONG tang, khong co dong nao trong
#   mua_shop / giao_dich / game_log -> khong phai ban hay giao dich, la MAT do loi.
#
# CO CHE:
#   1. TAO TRUNG: KItemList::Equip (KItemList.cpp:1393) gan thang m_EquipItem[nEquipPlace] = nIdx
#      ma KHONG kiem o do da co mon chua va KHONG UnEquip mon cu. Duong nguoi choi (MoveItem,
#      case pos_equip) chi UnEquip o NGUON (SrcPos->nX) roi Equip vao o DICH (DesPos->nX) - keo
#      tu o trang bi nay sang o trang bi khac dang co mon => HAI mon cung mot o, ca hai cung
#      nPlace = pos_equip, cung nX -> luu xuong DB ca hai.
#      Ngoai ra Equip KHONG xoa m_Hand (cung goc voi loi bot nem do 06/09) nen con sinh mon "ma".
#   2. MAT KHI NAP: LoadDBPlayerInfo goi m_ItemList.AddKIL(nIndex, nLocal, x, y); AddKIL
#      (KItemList.cpp:241) o nhanh pos_equip: `if (m_EquipItem[nX]) return 0;` -> mon thu hai bi
#      BO IM LANG, khong vao danh sach -> mat vinh vien. Cac nhanh luoi (pos_equiproom,
#      pos_repositoryroom, pos_exbox*) cung tra 0 khi PlaceItem that bai (trung o luoi).
#
# SUA (KItemList.cpp - tep DUNG CHUNG client + server, phai build CA HAI):
#   H1 Equip(): o dich da co mon KHAC -> UnEquip mon do truoc (mon cu ve pos_equiproom neu con
#      cho, khong thi ve tay/giu nguyen o luoi) => khong bao gio sinh hai mon mot o.
#   H2 Equip(): xoa m_Hand khi chinh mon do dang o tay (het mon "ma" o tay).
#   H3 AddKIL(): nhanh pos_equip - o da bi chiem thi KHONG bo mon nua, ma dat vao hanh trang
#      (CheckCanPlaceInEquipment) hoac ruong; het cho thi giu o tay. Chi bo khi that su khong
#      con cho nao => KHONG con mat do khi nap.
#   H4 AddKIL(): cac nhanh luoi - PlaceItem that bai (trung o) thi tim o trong khac trong CHINH
#      phong do, roi den hanh trang; het cho moi thoi.
# Idempotent. Chi ASCII. Tham so 1 = goc cay (mac dinh worktree botnoi).
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi"
P = os.path.join(ROOT, "Sources", "Core", "Src", "KItemList.cpp")

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

# ---------------- H1 + H2: Equip khong tao trung o, va don m_Hand ----------------
ap("H1+H2 Equip: don o dich + xoa m_Hand",
 '	m_EquipItem[nEquipPlace] = nIdx;\n',
 '	// [MATDO 06/09] O DICH DA CO MON KHAC -> phai go mon cu ra TRUOC. Truoc day gan de len:\n'
 '	// mon cu van con nPlace = pos_equip / nX = o nay -> HAI mon mot o -> luu xuong DB ca hai ->\n'
 '	// lan nap sau AddKIL(pos_equip) thay o da chiem va BO mon thu hai = MAT DO (do that tren\n'
 '	// role_history cua CaiBang: 17 mon mac / 14 o luc 20:45, con 14 mon luc 21:09).\n'
 '	// Duong nguoi choi (MoveItem case pos_equip) chi UnEquip o NGUON nen keo tu o nay sang o\n'
 '	// khac dang co do la sinh trung.\n'
 '	if (m_EquipItem[nEquipPlace] && m_EquipItem[nEquipPlace] != nIdx)\n'
 '	{\n'
 '		const int nCu = m_EquipItem[nEquipPlace];\n'
 '		UnEquip(nCu, nEquipPlace);\n'
 '		const int nListCu = FindSame(nCu);\n'
 '		if (nListCu)\n'
 '		{\n'
 '			int cx = -1, cy = -1;\n'
 '			if (m_Items[nListCu].nPlace == pos_equip\n'
 '			 && CheckCanPlaceInEquipment(Item[nCu].GetWidth(), Item[nCu].GetHeight(), &cx, &cy)\n'
 '			 && m_Room[room_equipment].PlaceItem(cx, cy, nCu, Item[nCu].GetWidth(), Item[nCu].GetHeight()))\n'
 '			{\n'
 '				m_Items[nListCu].nPlace = pos_equiproom;\n'
 '				m_Items[nListCu].nX = cx;\n'
 '				m_Items[nListCu].nY = cy;\n'
 '			}\n'
 '			else if (m_Items[nListCu].nPlace == pos_equip && !m_Hand)\n'
 '			{\n'
 '				m_Items[nListCu].nPlace = pos_hand;   // tui day -> giu tren tay nhu duong client that\n'
 '				m_Items[nListCu].nX = 0;\n'
 '				m_Items[nListCu].nY = 0;\n'
 '				m_Hand = nCu;\n'
 '			}\n'
 '		}\n'
 '	}\n'
 '	// [MATDO 06/09] mon vua roi TAY de mac len nguoi -> phai xoa m_Hand, khong thi m_Hand con tro\n'
 '	// vao mon DA MAC: lan InsertEquipment sau se "nem mon dang o tay" = nem chinh do dang mac.\n'
 '	if (m_Hand == nIdx)\n'
 '		m_Hand = 0;\n'
 '	m_EquipItem[nEquipPlace] = nIdx;\n')

# ---------------- H3: AddKIL pos_equip - cuu mon thay vi bo ----------------
ap("H3 AddKIL pos_equip: cuu mon khi o da chiem",
 '	case pos_equip:\n'
 '		if (nX < 0 || nX >= itempart_num)\n'
 '			return 0;\n'
 '		if (m_EquipItem[nX])\n'
 '			return 0;\n'
 '		m_Items[i].nPlace = pos_equip;\n'
 '		m_Items[i].nX = nX;\n'
 '		m_Items[i].nY = 0;\n'
 '		break;\n',
 '	case pos_equip:\n'
 '		if (nX < 0 || nX >= itempart_num)\n'
 '			return 0;\n'
 '		// [MATDO 06/09] O da co mon (ban luu co HAI mon cung mot o trang bi - xem Equip):\n'
 '		// TRUOC DAY return 0 = bo mon thu hai IM LANG => nguoi choi mat do sau moi lan vao game.\n'
 '		// Nay cuu mon: dat vao hanh trang, roi ruong; het cho that su moi thoi.\n'
 '		if (m_EquipItem[nX])\n'
 '		{\n'
 '			int rx = -1, ry = -1;\n'
 '			if (CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &rx, &ry)\n'
 '			 && m_Room[room_equipment].PlaceItem(rx, ry, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '			{\n'
 '				m_Items[i].nPlace = pos_equiproom;\n'
 '				m_Items[i].nX = rx;\n'
 '				m_Items[i].nY = ry;\n'
 '				break;\n'
 '			}\n'
 '			if (CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &rx, &ry, room_repository)\n'
 '			 && m_Room[room_repository].PlaceItem(rx, ry, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '			{\n'
 '				m_Items[i].nPlace = pos_repositoryroom;\n'
 '				m_Items[i].nX = rx;\n'
 '				m_Items[i].nY = ry;\n'
 '				break;\n'
 '			}\n'
 '			return 0;\n'
 '		}\n'
 '		m_Items[i].nPlace = pos_equip;\n'
 '		m_Items[i].nX = nX;\n'
 '		m_Items[i].nY = 0;\n'
 '		break;\n')

# ---------------- H4: AddKIL luoi - trung o thi tim o trong khac ----------------
ap("H4 AddKIL pos_equiproom: trung o thi tim o trong",
 '	case pos_equiproom://xu ly xep chong item cho nay\n'
 '		if (!m_Room[room_equipment].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '			return 0;\n',
 '	case pos_equiproom://xu ly xep chong item cho nay\n'
 '		// [MATDO 06/09] trung o luoi (hai mon cung toa do trong ban luu) -> TRUOC DAY bo mon im\n'
 '		// lang = mat do. Nay tim o trong khac trong chinh hanh trang truoc khi chiu thua.\n'
 '		if (!m_Room[room_equipment].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '		{\n'
 '			int qx = -1, qy = -1;\n'
 '			if (!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &qx, &qy)\n'
 '			 || !m_Room[room_equipment].PlaceItem(qx, qy, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '				return 0;\n'
 '			nX = qx;\n'
 '			nY = qy;\n'
 '		}\n')

ap("H4b AddKIL pos_repositoryroom: trung o thi tim o trong",
 '	case pos_repositoryroom:\n'
 '		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '			return 0;\n',
 '	case pos_repositoryroom:\n'
 '		// [MATDO 06/09] nhu tren: trung o trong ruong thi tim o trong khac, khong bo mon.\n'
 '		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '		{\n'
 '			int qx = -1, qy = -1;\n'
 '			if (!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &qx, &qy, room_repository)\n'
 '			 || !m_Room[room_repository].PlaceItem(qx, qy, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))\n'
 '				return 0;\n'
 '			nX = qx;\n'
 '			nY = qy;\n'
 '		}\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
