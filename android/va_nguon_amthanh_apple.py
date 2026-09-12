# -*- coding: utf-8 -*-
"""[IOS-AM 11/09] Bat backend Core Audio cua miniaudio cho iOS va macOS.

Sources/Engine/Src/KSoundMa.cpp dat MA_ENABLE_ONLY_SPECIFIC_BACKENDS roi chi liet ke
WASAPI (Windows), AAUDIO + OPENSL (Android) va NULL. Tren Apple khong co backend nao trong
so do -> miniaudio roi ve backend NULL: ma_engine_init() van TRA VE THANH CONG, van giai ma
duoc tep nhac, log van in "san sang: 48000 Hz, 2 kenh" - nhung khong co gi ra loa.
Im tieng ma khong bao loi, nen rat kho thay.

Sua: them MA_ENABLE_COREAUDIO trong #ifdef JX_APPLE, va in them ten backend that su dung
de lan sau nhin log la biet ngay.

Khong dong toi ban Windows va Android: ca hai phan them deu trong #ifdef JX_APPLE.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'Engine', 'Src', 'KSoundMa.cpp')
MOC = b'IOS-AM'

CU1 = (b"#define MA_ENABLE_OPENSL\n"
       b"#define MA_ENABLE_NULL\n")
MOI1 = (b"#define MA_ENABLE_OPENSL\n"
        b"#define MA_ENABLE_NULL\n"
        b"#ifdef JX_APPLE\t// [IOS-AM 11/09] iOS va macOS chi co Core Audio. Thieu dong nay thi\n"
        b"// MA_ENABLE_ONLY_SPECIFIC_BACKENDS o tren chi bat WASAPI/AAudio/OpenSL -> Apple roi ve\n"
        b"// backend NULL: van bao \"san sang\", van giai ma duoc nhac, nhung IM TIENG hoan toan.\n"
        b"#define MA_ENABLE_COREAUDIO\n"
        b"#endif\n")

CU2 = b'\tSDL_Log("[SDL] miniaudio %s san sang: %u Hz, %u kenh", MA_VERSION_STRING, ma_engine_get_sample_rate(pEngine), ma_engine_get_channels(pEngine));\n'
MOI2 = (CU2 +
        b"#ifdef JX_APPLE\t// [IOS-AM 11/09] in ten backend that: \"NULL\" nghia la khong ra loa\n"
        b"\t{\n"
        b"\t\tma_device* pDev = ma_engine_get_device(pEngine);\n"
        b"\t\tSDL_Log(\"[SDL] miniaudio backend: %s\", (pDev && pDev->pContext) ? ma_get_backend_name(pDev->pContext->backend) : \"(khong co thiet bi)\");\n"
        b"\t}\n"
        b"#endif\n")

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi, khong lam gi'); return 0
    for cu, moi, ten in ((CU1, MOI1, 'bat COREAUDIO'), (CU2, MOI2, 'log ten backend')):
        n = d.count(cu)
        if n != 1:
            print('LOI: %s - tim thay %d cho khop, can 1' % (ten, n)); return 1
        d = d.replace(cu, moi)
    open(TEP, 'wb').write(d)
    print('da va %s' % TEP)
    return 0

sys.exit(main())
