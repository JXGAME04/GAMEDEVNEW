#!/bin/sh
# [IOS-PHATHANH 15/09] Dung ban PHAT HANH cho iOS: Archive + dSYM day du + tu kiem.
#
# Vi sao phai co script rieng, khong chay xcodebuild archive trong build/ios-dev:
#   1. Luc Archive, Xcode bat che do "install": san pham bi doi sang DerivedData va Release-iphoneos chi con
#      LIEN KET TREO. Ban thu hang ngay hong theo (cmake bao "unable to create directory"), phai xoa lien ket
#      roi dung lai. Vi vay ban phat hanh dung CAY DUNG RIENG.
#   2. Xcode thu dSYM tu BuildProductsPath cua Archive, nhung CMake ghim CONFIGURATION_BUILD_DIR nen dSYM roi ra
#      cho khac => thu muc dSYMs trong Archive TRONG. Script chep vao va kiem lai.
#
# Dung:  sh ios/dung_ban_phat_hanh.sh [cay_dung] [duong_dan_archive]
# Bien moi truong:
#   JX_IOS_DOI          ma doi ky (mac dinh PK9QTZYMSL)
#   JX_IOS_KY           ten chung thu ky (mac dinh "Apple Development"; tai khoan tra phi dung "Apple Distribution")
#   JX_IOS_SO_BAN_DUNG  CFBundleVersion - moi lan tai len App Store Connect PHAI tang
#   JX_IOS_MA_GOI       ma goi cho san (mac dinh vn.jx1.mobile)
#   JX_IOS_NGHIEM=1     che do NGHIEM: chi chap nhan goi ky bang chung thu Distribution va khong co get-task-allow,
#                       va kho du lieu phai la https://
#   JX_IOS_KHO_DU_LIEU  [16/09] dia chi kho du lieu ghi SAN vao goi (Info.plist JxKhoDuLieu); nhieu guong cach nhau bang
#                       dau cach. Script TAI THAT manifest + chu ky tu moi guong va kiem bang khoa cong khai trong app.
#   JX_IOS_CHO_PHEP_KHO_RONG=1  [16/09] cho phep dong goi khi kho rong (CHI de thu quy trinh; goi do khong gui duyet duoc:
#                       may cai moi se chan o man "Thieu du lieu game" - rot dieu 2.1 chac chan).
#
# [16/09] Script luon truyen -DJX_IOS_KHOA_NOI_BO=OFF: goi phat hanh khong co UIFileSharingEnabled, khong HTTP LAN, khong doc
# may_chu_tai.txt, khong API noi bo suspend - va tu kiem lai tren goi + nhi phan (chot 4).
set -e

# In ra duong dan tuyet doi, da bo "." ".." va lien ket mem. Khong tao thu muc.
duong_that() {
  _d=$1
  if [ -d "$_d" ]; then (cd "$_d" && pwd -P); else
    _p=$(dirname "$_d"); _b=$(basename "$_d")
    if [ -d "$_p" ]; then printf '%s/%s\n' "$(cd "$_p" && pwd -P)" "$_b"; else printf '%s\n' "$_d"; fi
  fi
}

GOC=$(cd "$(dirname "$0")/.." && pwd -P)
[ -f "$GOC/ios/CMakeLists.txt" ] || { echo "HONG: khong thay $GOC/ios/CMakeLists.txt - chay tu trong kho ma nguon"; exit 1; }

CAY=$(duong_that "${1:-$GOC/build/ios-store}")
RA=$(duong_that "${2:-$CAY/jx1ios.xcarchive}")
DOI=${JX_IOS_DOI:-PK9QTZYMSL}
KY=${JX_IOS_KY:-Apple Development}
SO=${JX_IOS_SO_BAN_DUNG:-1}
MA_MONG=${JX_IOS_MA_GOI:-vn.jx1.mobile}
KHO=${JX_IOS_KHO_DU_LIEU:-}

# Chot 1: khong duoc dung cay ban thu. So bang duong dan da phan giai, vi "build/ios-dev" (tuong doi),
# "build/./ios-dev", lien ket mem... deu tro toi cung cho ma so chuoi thuong khong bat duoc.
CAY_THU=$(duong_that "$GOC/build/ios-dev")
[ "$CAY" != "$CAY_THU" ] || { echo "HONG: khong duoc dung cay ban thu ($CAY_THU) de dong goi - Archive se pha ban thu"; exit 1; }

# Chot 2: duong dan archive phai dung duoi .xcarchive VA phai nam trong cay dung (truoc khi rm -rf).
case "$RA" in
  *.xcarchive) ;;
  *) echo "HONG: duong dan archive phai tan cung bang .xcarchive (dang co: $RA)"; exit 1 ;;
esac
case "$RA/" in
  "$CAY"/*) ;;
  *) echo "HONG: archive phai nam trong cay dung $CAY (dang co: $RA)"; exit 1 ;;
esac

# [IOS-PHATHANH 16/09] Chot 3: KHO DU LIEU. Rong = may cai moi khong khoi dong duoc (rot 2.1 chac chan) -> HONG mac dinh,
# chi cho qua khi noi ro la dang thu quy trinh. Co dia chi thi KIEM THAT truoc khi ton 10 phut dong goi:
#   - tai manifest.txt + manifest.sig tu TUNG guong, kiem chu ky bang dung khoa cong khai dang nam trong ios/JxTaiDuLieu.mm
#     (kho ky bang khoa khac = moi may cai moi deu ket o man Thu lai);
#   - phienban.txt (neu kho co) KHONG duoc doi ban cao hon JX_PHIEN_BAN_APP dang dong goi (ios/JxIosMain.cpp) - khong thi
#     nguoi duyet cai xong gap "Ban game da cu" khong loi thoat. Luat van hanh: chi nang phienban.txt SAU khi ban moi
#     da "Ready for Sale", khong bao gio nang trong luc dang duyet.
mkdir -p "$CAY"   # phai tao truoc: shell mo tep log TRUOC khi cmake tao thu muc
if [ -z "$KHO" ]; then
  if [ "${JX_IOS_CHO_PHEP_KHO_RONG:-0}" = "1" ]; then
    echo "CANH BAO: JX_IOS_KHO_DU_LIEU rong -> goi nay KHONG gui duyet duoc (may cai moi chan o 'Thieu du lieu game')"
  else
    echo "HONG: JX_IOS_KHO_DU_LIEU rong. Dat dia chi kho (https://.../), hoac JX_IOS_CHO_PHEP_KHO_RONG=1 neu chi thu quy trinh"; exit 1
  fi
else
  PB_APP=$(sed -n 's/^#define JX_PHIEN_BAN_APP[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$GOC/ios/JxIosMain.cpp" | head -1)
  [ -n "$PB_APP" ] || { echo "HONG: khong doc duoc JX_PHIEN_BAN_APP trong ios/JxIosMain.cpp"; exit 1; }
  KK="$CAY/kiem_kho"; rm -rf "$KK"; mkdir -p "$KK"
  python3 - "$GOC/ios/JxTaiDuLieu.mm" > "$KK/khoa.der" <<'EOF'
import re, sys
s = open(sys.argv[1], encoding="utf-8", errors="replace").read()
m = re.search(r"s_jxKhoaCongKhai\[\]\s*=\s*\{(.*?)\};", s, re.S)
sys.stdout.buffer.write(bytes(int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", m.group(1))))
EOF
  openssl ec -pubin -inform DER -in "$KK/khoa.der" -out "$KK/khoa.pem" >/dev/null 2>&1 \
    || { echo "HONG: khong doc duoc khoa cong khai s_jxKhoaCongKhai trong ios/JxTaiDuLieu.mm"; exit 1; }
  for goc in $KHO; do   # $KHO KHONG ngoac kep: tach theo dau cach thanh tung guong (sh khong co mang)
    case "$goc" in */) ;; *) goc="$goc/" ;; esac
    if [ "${JX_IOS_NGHIEM:-0}" = "1" ]; then
      case "$goc" in https://*) ;; *) echo "HONG (nghiem): kho phai la https:// (ATS chan http ra Internet): $goc"; exit 1 ;; esac
    fi
    curl -fsS -m 30 -o "$KK/manifest.txt" "${goc}manifest.txt" || { echo "HONG: khong tai duoc ${goc}manifest.txt"; exit 1; }
    curl -fsS -m 30 -o "$KK/manifest.sig" "${goc}manifest.sig" || { echo "HONG: khong tai duoc ${goc}manifest.sig"; exit 1; }
    base64 -D -i "$KK/manifest.sig" -o "$KK/sig.der" 2>/dev/null || { echo "HONG: ${goc}manifest.sig khong phai base64"; exit 1; }
    openssl dgst -sha256 -verify "$KK/khoa.pem" -signature "$KK/sig.der" "$KK/manifest.txt" >/dev/null 2>&1 \
      || { echo "HONG: chu ky manifest o $goc KHONG khop khoa cong khai trong app (ky lai: python3 android/ky_manifest.py)"; exit 1; }
    SO_TEP=$(wc -l < "$KK/manifest.txt" | tr -d ' ')
    PB_KHO=""
    if grep -qa 'phienban.txt$' "$KK/manifest.txt"; then
      PB_KHO=$(curl -fsS -m 30 "${goc}phienban.txt" 2>/dev/null | head -1 | tr -cd '0-9' || true)
      if [ -n "$PB_KHO" ] && [ "$PB_KHO" -gt "$PB_APP" ]; then
        echo "HONG: ${goc}phienban.txt doi ban $PB_KHO > ban dang dong goi $PB_APP -> nguoi duyet se bi chan 'Ban game da cu'"; exit 1
      fi
    fi
    echo "== kho $goc: chu ky HOP LE, $SO_TEP tep, phienban.txt=${PB_KHO:-(khong co)}, app=$PB_APP"
  done
fi

echo "== cau hinh cay dung rieng: $CAY (so ban dung=$SO, ky=$KY, khoa noi bo=OFF)"
cmake -S "$GOC/ios" -B "$CAY" -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 \
  -DJX_IOS_SO_BAN_DUNG="$SO" \
  -DJX_IOS_KHOA_NOI_BO=OFF \
  -DJX_IOS_KHO_DU_LIEU="$KHO" \
  -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$DOI" \
  -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE=Automatic \
  -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="$KY" \
  -DCMAKE_XCODE_GENERATE_SCHEME=ON > "$CAY/cauhinh.log" 2>&1 \
  || { echo "HONG: cau hinh loi, xem $CAY/cauhinh.log"; exit 1; }

# Xoa dSYM cu de chac chan dSYM chep vao Archive la cua lan dung nay.
rm -rf "$CAY/Release-iphoneos/jx1ios.app.dSYM"

echo "== dong goi Archive: $RA"
rm -rf "$RA"
xcodebuild -project "$CAY/JX1Ios.xcodeproj" -scheme jx1ios -configuration Release \
  -destination 'generic/platform=iOS' -derivedDataPath "$CAY/dd" -archivePath "$RA" \
  -allowProvisioningUpdates archive > "$CAY/archive.log" 2>&1 \
  || { echo "HONG: archive loi, xem $CAY/archive.log"; exit 1; }

# Chep dSYM vao Archive (Xcode khong tu lam duoc vi CMake ghim CONFIGURATION_BUILD_DIR).
DSYM_NGUON="$CAY/Release-iphoneos/jx1ios.app.dSYM"
DSYM="$RA/dSYMs/jx1ios.app.dSYM"
[ -d "$DSYM_NGUON" ] || { echo "HONG: khong sinh duoc dSYM o $DSYM_NGUON"; exit 1; }
mkdir -p "$RA/dSYMs"
rm -rf "$DSYM"
cp -R "$DSYM_NGUON" "$RA/dSYMs/"

APP="$RA/Products/Applications/jx1ios.app"
BIN="$APP/jx1ios"
[ -f "$BIN" ] || { echo "HONG: Archive khong co app (kiem INSTALL_PATH cua jx1ios)"; exit 1; }
[ -d "$DSYM" ] || { echo "HONG: Archive khong co dSYM"; exit 1; }

# UUID phai doc duoc va phai khop. Loc theo kien truc, khong lay bua dong dau.
KIEN_TRUC=$(lipo -archs "$BIN" | awk '{print $1}')
[ -n "$KIEN_TRUC" ] || { echo "HONG: khong doc duoc kien truc cua tep chay"; exit 1; }
U_APP=$(dwarfdump --uuid "$BIN"  | awk -v a="($KIEN_TRUC)" '$3==a{print $2}')
U_SYM=$(dwarfdump --uuid "$DSYM" | awk -v a="($KIEN_TRUC)" '$3==a{print $2}')
[ -n "$U_APP" ] || { echo "HONG: khong doc duoc UUID cua tep chay"; exit 1; }
[ -n "$U_SYM" ] || { echo "HONG: khong doc duoc UUID cua dSYM"; exit 1; }

# dSYM phai chua NGUON that, khong phai chi header (trong dSYM nay co 1512 dong .h).
# Dem theo tung phan, KHONG chong nhau: Lua54 nam trong Sources/ nen phai tru ra.
# So do duoc ngay 15/09 tren ban DAT: tong 466 | SDL3 .c/.m 106 | Lua54 .c 34 | game (tru Lua) 317 | ios/ 9.
dwarfdump --show-sources "$DSYM" > "$CAY/dsym_nguon.txt" 2>/dev/null || true
SO_NGUON=$(grep -acE '\.(c|cpp|m|mm)$' "$CAY/dsym_nguon.txt" || true)
SO_SDL=$(grep -acE 'SDL3-src/.*\.(c|m)$' "$CAY/dsym_nguon.txt" || true)
SO_LUA=$(grep -acE '/Sources/Library/Lua54/.*\.c$' "$CAY/dsym_nguon.txt" || true)
SO_GAME=$(grep -aE '/Sources/.*\.(cpp|c)$' "$CAY/dsym_nguon.txt" | grep -acv '/Sources/Library/Lua54/' || true)
SO_IOS=$(grep -acE '/ios/.*\.(cpp|mm)$' "$CAY/dsym_nguon.txt" || true)
KY_HIEU=$(nm "$BIN" 2>/dev/null | wc -l | tr -d ' ')
SO_BAN=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleVersion' "$APP/Info.plist" 2>/dev/null || echo '?')
MA_GOI=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' "$APP/Info.plist" 2>/dev/null || echo '?')
TEN_ICON=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIconName' "$APP/Info.plist" 2>/dev/null || echo '(thieu)')
SO_MUC=$(ls "$RA/Products/Applications" | wc -l | tr -d ' ')

echo "-- tu kiem --"
echo "kien truc            : $KIEN_TRUC"
echo "UUID app / dSYM      : $U_APP / $U_SYM"
echo "tep nguon trong dSYM : $SO_NGUON (SDL3 $SO_SDL, Lua54 $SO_LUA, game $SO_GAME, ios $SO_IOS)"
echo "ky hieu con lai      : $KY_HIEU"
echo "ma goi / so ban dung : $MA_GOI / $SO_BAN"
echo "ten bo icon          : $TEN_ICON"
echo "kho du lieu          : ${KHO:-(RONG - khong gui duyet duoc)}"

[ "$U_APP" = "$U_SYM" ] || { echo "HONG: UUID cua dSYM khong khop tep chay"; exit 1; }
[ "$SO_NGUON" -gt 400 ] || { echo "HONG: dSYM chi co $SO_NGUON tep nguon (do duoc 466) - thu vien tinh dang bi strip"; exit 1; }
[ "$SO_SDL"  -gt 80 ]   || { echo "HONG: dSYM chi co $SO_SDL tep nguon SDL (do duoc 106) - SDL3-static dang bi strip"; exit 1; }
[ "$SO_LUA"  -gt 25 ]   || { echo "HONG: dSYM chi co $SO_LUA tep nguon Lua (do duoc 34) - Lua54 dang bi strip"; exit 1; }
[ "$SO_GAME" -gt 250 ]  || { echo "HONG: dSYM chi co $SO_GAME tep nguon cua game (do duoc 317)"; exit 1; }
[ "$SO_IOS"  -ge 8 ]    || { echo "HONG: dSYM chi co $SO_IOS tep nguon lop dem iOS (do duoc 9)"; exit 1; }

# Moc cu the: dem co the trung khop nhau, nhung thieu dung mot trong nhung tep nay la co van de that.
for moc in 'SDL3-src/.*/src/SDL\.c$' 'SDL3-src/.*/video/uikit/SDL_uikitwindow\.m$' 'Lua54/src/lapi\.c$' 'ios/JxIosMain\.cpp$' 'Sources/Core/Src/CoreShell\.cpp$'; do
  grep -qaE "$moc" "$CAY/dsym_nguon.txt" || { echo "HONG: dSYM thieu moc $moc"; exit 1; }
done

# Strip: phai NHO hon nguong tren (da strip) nhung van phai co ky hieu (neu bang 0 la nm loi, khong phai da strip).
[ "$KY_HIEU" -gt 50 ]   || { echo "HONG: doc duoc $KY_HIEU ky hieu - nm loi chu khong phai da strip"; exit 1; }
[ "$KY_HIEU" -lt 2000 ] || { echo "HONG: tep chay chua duoc strip ($KY_HIEU ky hieu)"; exit 1; }

[ "$SO_MUC" -eq 1 ] || { echo "HONG: Products co $SO_MUC muc, le ra chi co jx1ios.app"; exit 1; }
[ -f "$APP/PrivacyInfo.xcprivacy" ] || { echo "HONG: thieu PrivacyInfo.xcprivacy trong app"; exit 1; }
[ "$TEN_ICON" = "AppIcon" ] || { echo "HONG: CFBundleIconName = $TEN_ICON (phai la AppIcon, neu khong bi ITMS-90713)"; exit 1; }
[ "$MA_GOI" = "$MA_MONG" ] || { echo "HONG: ma goi la $MA_GOI, mong doi $MA_MONG"; exit 1; }
[ "$SO_BAN" = "$SO" ] || { echo "HONG: CFBundleVersion trong goi la $SO_BAN, yeu cau $SO (Xcode khong thay duoc bien?)"; exit 1; }

# [IOS-PHATHANH 16/09] Chot 4: khoa THO phai bien mat khoi goi lan nhi phan (JX_IOS_KHOA_NOI_BO=OFF).
# PlistBuddy: khoa CO -> exit 0 (ke ca gia tri rong), khoa THIEU -> exit 1. Voi set -e phai boc trong if.
for k in UIFileSharingEnabled LSSupportsOpeningDocumentsInPlace NSAppTransportSecurity NSLocalNetworkUsageDescription; do
  if /usr/libexec/PlistBuddy -c "Print :$k" "$APP/Info.plist" >/dev/null 2>&1; then echo "HONG: goi con khoa tho $k"; exit 1; fi
done
# JxKhoDuLieu: THIEU khoa (mau plist hong / go sai ten bien - configure_file thay bien khong ton tai bang rong, khong bao)
# khac voi RONG (chua co may chu, da xet o chot 3).
if ! KHO_GOI=$(/usr/libexec/PlistBuddy -c 'Print :JxKhoDuLieu' "$APP/Info.plist" 2>/dev/null); then
  echo "HONG: Info.plist trong goi thieu khoa JxKhoDuLieu (mau ios/Info.plist hong hoac ten bien CMake sai)"; exit 1
fi
[ "$KHO_GOI" = "$KHO" ] || { echo "HONG: JxKhoDuLieu trong goi la '$KHO_GOI', yeu cau '$KHO'"; exit 1; }
# strings: DOI CHUNG DUONG truoc (jx_ios.log luon co trong nhi phan), roi moi tin cac so 0. grep -c tra exit 1 khi dem = 0
# nen phai "|| true"; dung /usr/bin/grep de khong dinh ham boc grep cua shell nguoi dung.
SO_CHUNG=$(strings -a "$BIN" | /usr/bin/grep -c 'jx_ios.log' || true)
[ "$SO_CHUNG" -ge 1 ] || { echo "HONG: strings khong doc duoc tep chay (0 chuoi jx_ios.log) - phep do hong, khong ket luan duoc"; exit 1; }
SO_SUS=$(strings -a "$BIN" | /usr/bin/grep -cx 'suspend' || true)
[ "$SO_SUS" -eq 0 ] || { echo "HONG: tep chay con chuoi 'suspend' ($SO_SUS) - API noi bo chua bi loai (JX_IOS_NOI_BO?)"; exit 1; }
SO_MCT=$(strings -a "$BIN" | /usr/bin/grep -c 'may_chu_tai.txt' || true)
[ "$SO_MCT" -eq 0 ] || { echo "HONG: tep chay con chuoi 'may_chu_tai.txt' ($SO_MCT) - nhanh noi bo chua bi loai"; exit 1; }
SO_SEL=$(otool -v -s __TEXT __objc_methname "$BIN" 2>/dev/null | /usr/bin/grep -cw 'suspend' || true)
[ "$SO_SEL" -eq 0 ] || { echo "HONG: __objc_methname con 'suspend' ($SO_SEL)"; exit 1; }
echo "khoa tho             : da loai (plist sach, 0 'suspend', 0 'may_chu_tai.txt')"
# [IOS-DUNGLUONG 16/09] Chot 5: nhi phan dung API dung luong dia (nhom required-reason DiskSpace) thi PrivacyInfo phai khai,
# khong thi App Store Connect chan ngay khi tai len (ITMS-91053). Doi chung: nm -u phai doc duoc (>50 ky hieu).
SO_NMU=$(nm -u "$BIN" 2>/dev/null | wc -l | tr -d ' ')
[ "$SO_NMU" -gt 50 ] || { echo "HONG: nm -u chi doc duoc $SO_NMU ky hieu - phep do hong"; exit 1; }
if nm -u "$BIN" 2>/dev/null | /usr/bin/grep -qE '_statvfs$|_statfs$|_fstatvfs$|_fstatfs$|_getattrlist|VolumeAvailableCapacity|VolumeTotalCapacity|FileSystemFreeSize|FileSystemSize'; then
  if ! plutil -extract NSPrivacyAccessedAPITypes json -o - "$APP/PrivacyInfo.xcprivacy" 2>/dev/null | /usr/bin/grep -q 'NSPrivacyAccessedAPICategoryDiskSpace'; then
    echo "HONG: nhi phan dung API dung luong dia nhung PrivacyInfo.xcprivacy thieu NSPrivacyAccessedAPICategoryDiskSpace"; exit 1
  fi
  echo "dung luong dia       : co dung API, PrivacyInfo da khai DiskSpace"
fi

# Che do NGHIEM: chi dung khi that su di tai len. Goi ky bang chung thu phat trien se bi App Store Connect tu choi.
if [ "${JX_IOS_NGHIEM:-0}" = "1" ]; then
  codesign -d --entitlements :- "$APP" > "$CAY/quyen.txt" 2>/dev/null || true
  # DOC PLIST, dung do chu. Hai lan sai truoc khi ra cach nay:
  #   1. "grep -q ... | grep -q ...": -q chan dau ra nen lenh sau doc chuoi rong -> chot KHONG BAO GIO chay.
  #   2. "grep -A1 get-task-allow | grep '<true/>'": codesign in TOAN BO quyen tren MOT dong, nen -A1 tra ve ca
  #      tai lieu va bat trung <true/> cua khoa khac. Goi phat hanh that luon co beta-reports-active = true
  #      -> chot se chan nham dung ban dang muon tai len.
  QTA=$(plutil -extract get-task-allow raw -o - "$CAY/quyen.txt" 2>/dev/null || echo false)
  if [ "$QTA" = "true" ]; then
    echo "HONG (nghiem): goi con get-task-allow=true, la ban ky phat trien - App Store Connect se tu choi"; exit 1
  fi
  codesign -dvv "$APP" > "$CAY/chuky.txt" 2>&1 || true
  grep -qa 'Authority=Apple Distribution' "$CAY/chuky.txt" \
    || { echo "HONG (nghiem): khong phai chung thu Apple Distribution"; exit 1; }
fi

echo "DAT: $RA"
if [ "${JX_IOS_NGHIEM:-0}" != "1" ]; then
  echo "LUU Y: ban nay ky bang chung thu PHAT TRIEN, KHONG tai len App Store Connect duoc."
  echo "       Khi co tai khoan tra phi: dat JX_IOS_KY='Apple Distribution' JX_IOS_NGHIEM=1 JX_IOS_KHO_DU_LIEU=https://...,"
  echo "       roi them buoc xcodebuild -exportArchive voi ExportOptions.plist (method app-store-connect, uploadSymbols true)."
fi
