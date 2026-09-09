#!/bin/bash
# [ANDROID 08/09] LDPlayer 9 tren may nay: tro thu muc chia se "Misc" -> D:\jx1_android_data (chu cho phep), khoi dong lai,
# cai APK, mo app, chup man hinh + lay log. Dung: bash android/ldplayer_chay_thu.sh [apk] [giay cho]
export MSYS_NO_PATHCONV=1
LD="E:/LDPlayer/LDPlayer9"
ADB="C:/Users/nguye/AppData/Local/Android/Sdk/platform-tools/adb.exe"
CFG="$LD/vms/config/leidian0.config"
APK="${1:-D:/GAMEDEVNEW_wt_mobile/android/apk/jx1mobile-debug-0809c.apk}"
CHO="${2:-40}"
OUT="C:/Users/nguye/AppData/Local/Temp/claude/J--CayChay-Src-Auto-Ngoai-WAuto-WAuto/f7fe3690-d1f7-481d-b4f6-845330c7c14f/scratchpad/ld"
mkdir -p "$OUT"
DATA="D:/jx1_android_data"

if ! grep -q '"statusSettings.sharedMisc": "D:/jx1_android_data"' "$CFG"; then
  echo "=== tat LDPlayer de doi thu muc chia se Misc -> $DATA"
  "$LD/ldconsole.exe" quit --index 0; sleep 8
  cp "$CFG" "$CFG.bak-jx1-$(date +%H%M%S)"
  python - "$CFG" <<'EOF'
import sys, io, re
p = sys.argv[1]; s = io.open(p, encoding="utf-8").read()
s2, n = re.subn(r'"statusSettings\.sharedMisc"\s*:\s*"[^"]*"', '"statusSettings.sharedMisc": "D:/jx1_android_data"', s)
io.open(p, "w", encoding="utf-8", newline="\n").write(s2); print("sua sharedMisc:", n)
EOF
  echo "=== khoi dong lai LDPlayer"
  "$LD/ldconsole.exe" launch --index 0
  for i in $(seq 1 40); do sleep 5; b=$("$ADB" -s emulator-5554 shell getprop sys.boot_completed 2>/dev/null | tr -d '\r'); [ "$b" = "1" ] && break; done
  echo "boot_completed=$b sau $((i*5)) s"; sleep 10
fi
"$ADB" connect 127.0.0.1:5555 >/dev/null 2>&1
D=emulator-5554
echo "=== /mnt/shared/Misc:"; "$ADB" -s $D shell "ls /mnt/shared/Misc | head; ls -la /mnt/shared/Misc/config.ini"
echo "=== cai APK: $APK"; "$ADB" -s $D install -r "$APK" 2>&1 | tail -1
"$ADB" -s $D shell "rm -f /mnt/shared/Misc/jx_android.log /mnt/shared/Misc/jx_rep3.log /mnt/shared/Misc/jx_crash.log" 2>/dev/null
"$ADB" -s $D logcat -c
echo "=== mo app"; "$ADB" -s $D shell am start -n vn.jx1.mobile/org.libsdl.app.SDLActivity 2>&1 | tail -1
sleep "$CHO"
"$ADB" -s $D exec-out screencap -p > "$OUT/shot1.png" 2>/dev/null; ls -la "$OUT/shot1.png" | awk '{print "shot1", $5}'
echo "=== tien trinh:"; "$ADB" -s $D shell "ps -A | grep jx1" | head -2
echo "=== logcat (JX1/SDL/loi):"; "$ADB" -s $D logcat -d -s JX1:* SDL:* DEBUG:* AndroidRuntime:* libc:* 2>/dev/null | tail -60
echo "=== jx_android.log:"; cat "$DATA/jx_android.log" 2>/dev/null | tail -20
echo "=== jx_rep3.log:"; cat "$DATA/jx_rep3.log" 2>/dev/null | grep -a 'GPU\|REP3\|API\|loi\|FAIL' | head -20
echo "=== jx_crash.log:"; cat "$DATA/jx_crash.log" 2>/dev/null | tail -5
