# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-2] May chu GIA de kiem Rainbow.dll (lop mang client) ma khong can may chu game:
 - nhan ket noi TCP, gui ngay goi bat tay ACCOUNT_BEGIN (34 byte: WORD wLen + ProtocolType 0x20, Mode 0, ServerKey, ClientKey)
   -> client dat m_uServerKey = ~ServerKey, m_uClientKey = ~ClientKey (SocketClient.cpp WaitAndVerifyCipher)
 - moi goi client gui: [WORD len (ke ca 2 byte dau)][payload XOR khoa client] (KSG_EncodeBuf: khoa KHONG doi giua cac goi)
 - tra loi: [WORD len]["ECHO:" + payload XOR khoa server]; payload "BYE" -> dong ket noi (kiem duong FD_CLOSE)
 - --cham N: ngu N ms truoc khi tra loi; --gop: gui 2 goi tra loi trong 1 lan send (kiem tach goi)
 - --day N: ap luc: sau bat tay, luong rieng day N goi/giay "PUSH:<seq>:<ms>:<dem>" (co ~200 B) lien tuc (nhu dong bo NPC/nguoi choi)
Dung: python may_chu_gia.py [--port 27015] [--cham 0] [--gop] [--thoi-gian 60] [--day 0]"""
import socket, struct, sys, time, argparse, threading

def xor_key(buf: bytes, key: int) -> bytes:
    """y het KSG_DecodeEncode: tung word 4 byte XOR key, phan du XOR tung byte cua key (thap -> cao)"""
    out = bytearray(buf)
    n4 = len(out) // 4
    kb = struct.pack("<I", key & 0xFFFFFFFF)
    for i in range(n4):
        for j in range(4):
            out[4 * i + j] ^= kb[j]
    k = key & 0xFFFFFFFF
    for i in range(4 * n4, len(out)):
        out[i] ^= (k & 0xFF); k >>= 8
    return bytes(out)

def goi(payload: bytes, key: int) -> bytes:
    enc = xor_key(payload, key)
    return struct.pack("<H", 2 + len(enc)) + enc

def luong_day(c, lock, skey, day, dung):
    """day 'day' goi/giay, moi goi ~200 B; dem so goi de client kiem mat/gap"""
    seq = 0; t0 = time.perf_counter(); dem = 0
    while not dung.is_set():
        seq += 1
        payload = ("PUSH:%d:%d:" % (seq, int(time.perf_counter() * 1000))).encode() + b"x" * 180
        try:
            with lock: c.sendall(goi(payload, skey))
        except Exception:
            break
        dem += 1
        # giu nhip
        t_muc = t0 + dem / float(day)
        d = t_muc - time.perf_counter()
        if d > 0: time.sleep(d)
    print("[may chu gia] luong day dung, da day %d goi" % dem, flush=True)

def chay(port, cham, gop, thoi_gian, skey, ckey, day):
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", port)); srv.listen(4); srv.settimeout(1.0)
    print("[may chu gia] nghe 127.0.0.1:%d skey=%08x ckey=%08x cham=%d gop=%s day=%d/s" % (port, skey, ckey, cham, gop, day), flush=True)
    t_end = time.time() + thoi_gian
    while time.time() < t_end:
        try:
            c, addr = srv.accept()
        except socket.timeout:
            continue
        print("[may chu gia] ket noi tu %s:%d" % addr, flush=True)
        ab = struct.pack("<BB6sII16s", 0x20, 0, b"\0" * 6, (~skey) & 0xFFFFFFFF, (~ckey) & 0xFFFFFFFF, b"\0" * 16)
        c.sendall(struct.pack("<H", 2 + len(ab)) + ab)
        buf = b""; n_goi = 0
        c.settimeout(5.0)
        lock = threading.Lock(); dung = threading.Event(); th = None
        if day > 0:
            th = threading.Thread(target=luong_day, args=(c, lock, skey, day, dung), daemon=True); th.start()
        try:
            while True:
                try:
                    d = c.recv(65536)
                except socket.timeout:
                    print("[may chu gia] 5 s khong co du lieu -> dong", flush=True); break
                if not d:
                    print("[may chu gia] client dong", flush=True); break
                buf += d
                tra = b""
                while len(buf) >= 2:
                    ln = struct.unpack("<H", buf[:2])[0]
                    if ln < 3 or len(buf) < ln: break
                    payload = xor_key(buf[2:ln], ckey); buf = buf[ln:]
                    n_goi += 1
                    if day == 0 or n_goi <= 3:
                        print("[may chu gia] goi %d (%d byte): %r" % (n_goi, ln, payload[:60]), flush=True)
                    if payload == b"BYE":
                        if tra:
                            with lock: c.sendall(tra)
                        print("[may chu gia] BYE -> dong ket noi phia may chu (da nhan %d goi)" % n_goi, flush=True)
                        raise ConnectionAbortedError
                    tra += goi(b"ECHO:" + payload, skey)
                    if not gop and tra:
                        if cham: time.sleep(cham / 1000.0)
                        with lock: c.sendall(tra)
                        tra = b""
                if gop and tra:
                    if cham: time.sleep(cham / 1000.0)
                    with lock: c.sendall(tra)
        except ConnectionAbortedError:
            pass
        except Exception as e:
            print("[may chu gia] loi:", e, flush=True)
        dung.set()
        if th: th.join(timeout=2)
        try: c.close()
        except Exception: pass
    srv.close(); print("[may chu gia] het gio, thoat", flush=True)

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=27015)
    ap.add_argument("--cham", type=int, default=0)
    ap.add_argument("--gop", action="store_true")
    ap.add_argument("--thoi-gian", type=int, default=60)
    ap.add_argument("--day", type=int, default=0)
    ap.add_argument("--skey", type=lambda s: int(s, 16), default=0x12345678)
    ap.add_argument("--ckey", type=lambda s: int(s, 16), default=0x9ABCDEF0)
    a = ap.parse_args()
    chay(a.port, a.cham, a.gop, a.thoi_gian, a.skey, a.ckey, a.day)
