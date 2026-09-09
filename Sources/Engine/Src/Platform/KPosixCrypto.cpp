/*===========================================================================
  KPosixCrypto.cpp - [ANDROID 08/09] Cai tap con OpenSSL ma client dung (xem posixssl/openssl/*.h):
    AES-128/192/256 (khoi don, ECB - KIniFile.cpp giai ma INI), SHA-256 (BauCua.cpp), RAND_bytes.
  Thuat toan chuan (FIPS-197, FIPS-180-4), tu viet, khong phu thuoc ngoai. Chi bien dich khi JX_POSIX (vao libJxPosix.so).
===========================================================================*/
#if !defined(_WIN32) || defined(JX_CRYPTO_TEST_HOST)   /* JX_CRYPTO_TEST_HOST: kiem thuat toan tren Windows (android/kiem_crypto.cpp) */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#ifndef JX_CRYPTO_TEST_HOST
#include <unistd.h>
#include <fcntl.h>
#endif
#include "posixssl/openssl/aes.h"
#include "posixssl/openssl/sha.h"
#include "posixssl/openssl/rand.h"

/*---------------------------------------------------------------- AES */
static const uint8_t SBOX[256] = {
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16 };
static uint8_t s_inv[256]; static int s_invReady = 0;
static void aes_init_inv(void) { if (!s_invReady) { for (int i = 0; i < 256; i++) s_inv[SBOX[i]] = (uint8_t)i; s_invReady = 1; } }
static inline uint8_t xt(uint8_t x) { return (uint8_t)((x << 1) ^ ((x & 0x80) ? 0x1b : 0)); }
static inline uint8_t mul(uint8_t a, uint8_t b) { uint8_t r = 0; while (b) { if (b & 1) r ^= a; a = xt(a); b >>= 1; } return r; }

int AES_set_encrypt_key(const unsigned char* userKey, const int bits, AES_KEY* key)
{
	if (!userKey || !key) return -1;
	int nk = bits / 32; if (nk != 4 && nk != 6 && nk != 8) return -2;
	int nr = nk + 6; key->rounds = nr;
	uint8_t* w = (uint8_t*)key->rd_key;   /* luu theo byte, 16 byte / vong */
	memcpy(w, userKey, (size_t)nk * 4);
	uint8_t rcon = 1;
	for (int i = nk; i < 4 * (nr + 1); i++)
	{
		uint8_t t[4]; memcpy(t, w + (i - 1) * 4, 4);
		if (i % nk == 0) { uint8_t u = t[0]; t[0] = (uint8_t)(SBOX[t[1]] ^ rcon); t[1] = SBOX[t[2]]; t[2] = SBOX[t[3]]; t[3] = SBOX[u]; rcon = xt(rcon); }
		else if (nk > 6 && i % nk == 4) { for (int j = 0; j < 4; j++) t[j] = SBOX[t[j]]; }
		for (int j = 0; j < 4; j++) w[i * 4 + j] = (uint8_t)(w[(i - nk) * 4 + j] ^ t[j]);
	}
	return 0;
}
int AES_set_decrypt_key(const unsigned char* userKey, const int bits, AES_KEY* key) { return AES_set_encrypt_key(userKey, bits, key); }
static void add_rk(uint8_t* s, const uint8_t* rk) { for (int i = 0; i < 16; i++) s[i] ^= rk[i]; }
void AES_encrypt(const unsigned char* in, unsigned char* out, const AES_KEY* key)
{
	const uint8_t* w = (const uint8_t*)key->rd_key; uint8_t s[16]; memcpy(s, in, 16); add_rk(s, w);
	for (int r = 1; r <= key->rounds; r++)
	{
		uint8_t t[16];
		for (int i = 0; i < 16; i++) t[i] = SBOX[s[i]];
		for (int c = 0; c < 4; c++) for (int rr = 0; rr < 4; rr++) s[c * 4 + rr] = t[((c + rr) % 4) * 4 + rr];   /* ShiftRows (cot-chinh) */
		if (r != key->rounds)
			for (int c = 0; c < 4; c++)
			{
				uint8_t a0 = s[c * 4], a1 = s[c * 4 + 1], a2 = s[c * 4 + 2], a3 = s[c * 4 + 3];
				s[c * 4] = (uint8_t)(xt(a0) ^ (xt(a1) ^ a1) ^ a2 ^ a3); s[c * 4 + 1] = (uint8_t)(a0 ^ xt(a1) ^ (xt(a2) ^ a2) ^ a3);
				s[c * 4 + 2] = (uint8_t)(a0 ^ a1 ^ xt(a2) ^ (xt(a3) ^ a3)); s[c * 4 + 3] = (uint8_t)((xt(a0) ^ a0) ^ a1 ^ a2 ^ xt(a3));
			}
		add_rk(s, w + r * 16);
	}
	memcpy(out, s, 16);
}
void AES_decrypt(const unsigned char* in, unsigned char* out, const AES_KEY* key)
{
	aes_init_inv();
	const uint8_t* w = (const uint8_t*)key->rd_key; uint8_t s[16]; memcpy(s, in, 16); add_rk(s, w + key->rounds * 16);
	for (int r = key->rounds - 1; r >= 0; r--)
	{
		uint8_t t[16];
		for (int c = 0; c < 4; c++) for (int rr = 0; rr < 4; rr++) t[((c + rr) % 4) * 4 + rr] = s[c * 4 + rr];   /* InvShiftRows */
		for (int i = 0; i < 16; i++) s[i] = s_inv[t[i]];
		add_rk(s, w + r * 16);
		if (r != 0)
			for (int c = 0; c < 4; c++)
			{
				uint8_t a0 = s[c * 4], a1 = s[c * 4 + 1], a2 = s[c * 4 + 2], a3 = s[c * 4 + 3];
				s[c * 4] = (uint8_t)(mul(a0, 14) ^ mul(a1, 11) ^ mul(a2, 13) ^ mul(a3, 9)); s[c * 4 + 1] = (uint8_t)(mul(a0, 9) ^ mul(a1, 14) ^ mul(a2, 11) ^ mul(a3, 13));
				s[c * 4 + 2] = (uint8_t)(mul(a0, 13) ^ mul(a1, 9) ^ mul(a2, 14) ^ mul(a3, 11)); s[c * 4 + 3] = (uint8_t)(mul(a0, 11) ^ mul(a1, 13) ^ mul(a2, 9) ^ mul(a3, 14));
			}
	}
	memcpy(out, s, 16);
}

/*---------------------------------------------------------------- SHA-256 */
static const uint32_t K256[64] = {
0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };
static inline uint32_t ror(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
static void sha256_block(SHA256_CTX* c, const unsigned char* p)
{
	uint32_t w[64];
	for (int i = 0; i < 16; i++) w[i] = ((uint32_t)p[i * 4] << 24) | ((uint32_t)p[i * 4 + 1] << 16) | ((uint32_t)p[i * 4 + 2] << 8) | p[i * 4 + 3];
	for (int i = 16; i < 64; i++) { uint32_t s0 = ror(w[i - 15], 7) ^ ror(w[i - 15], 18) ^ (w[i - 15] >> 3), s1 = ror(w[i - 2], 17) ^ ror(w[i - 2], 19) ^ (w[i - 2] >> 10); w[i] = w[i - 16] + s0 + w[i - 7] + s1; }
	uint32_t a = c->h[0], b = c->h[1], cc = c->h[2], d = c->h[3], e = c->h[4], f = c->h[5], g = c->h[6], h = c->h[7];
	for (int i = 0; i < 64; i++)
	{
		uint32_t t1 = h + (ror(e, 6) ^ ror(e, 11) ^ ror(e, 25)) + ((e & f) ^ (~e & g)) + K256[i] + w[i];
		uint32_t t2 = (ror(a, 2) ^ ror(a, 13) ^ ror(a, 22)) + ((a & b) ^ (a & cc) ^ (b & cc));
		h = g; g = f; f = e; e = d + t1; d = cc; cc = b; b = a; a = t1 + t2;
	}
	c->h[0] += a; c->h[1] += b; c->h[2] += cc; c->h[3] += d; c->h[4] += e; c->h[5] += f; c->h[6] += g; c->h[7] += h;
}
int SHA256_Init(SHA256_CTX* c)
{
	if (!c) return 0;
	static const uint32_t H0[8] = { 0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19 };
	memcpy(c->h, H0, sizeof(H0)); c->len = 0; c->num = 0; return 1;
}
int SHA256_Update(SHA256_CTX* c, const void* data, size_t len)
{
	const unsigned char* p = (const unsigned char*)data; if (!c) return 0;
	c->len += len;
	while (len)
	{
		size_t n = 64 - c->num; if (n > len) n = len;
		memcpy(c->buf + c->num, p, n); c->num += (unsigned)n; p += n; len -= n;
		if (c->num == 64) { sha256_block(c, c->buf); c->num = 0; }
	}
	return 1;
}
int SHA256_Final(unsigned char* md, SHA256_CTX* c)
{
	if (!c || !md) return 0;
	unsigned long long bits = c->len * 8; unsigned char pad = 0x80; SHA256_Update(c, &pad, 1);
	unsigned char z = 0; while (c->num != 56) SHA256_Update(c, &z, 1);
	unsigned char lenb[8]; for (int i = 0; i < 8; i++) lenb[i] = (unsigned char)(bits >> (56 - 8 * i));
	SHA256_Update(c, lenb, 8);
	for (int i = 0; i < 8; i++) { md[i * 4] = (unsigned char)(c->h[i] >> 24); md[i * 4 + 1] = (unsigned char)(c->h[i] >> 16); md[i * 4 + 2] = (unsigned char)(c->h[i] >> 8); md[i * 4 + 3] = (unsigned char)c->h[i]; }
	return 1;
}
unsigned char* SHA256(const unsigned char* d, size_t n, unsigned char* md)
{
	static unsigned char s_md[32]; if (!md) md = s_md;
	SHA256_CTX c; SHA256_Init(&c); SHA256_Update(&c, d, n); SHA256_Final(md, &c); return md;
}

/*---------------------------------------------------------------- RAND */
#ifndef JX_CRYPTO_TEST_HOST
int RAND_bytes(unsigned char* buf, int num)
{
	if (!buf || num < 0) return 0;
	int got = 0;   /* getrandom() chi co tu API 28 (bionic) -> doc /dev/urandom */
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd >= 0) { while (got < num) { ssize_t r = read(fd, buf + got, (size_t)(num - got)); if (r <= 0) break; got += (int)r; } close(fd); }
	return got == num ? 1 : 0;
}
int RAND_pseudo_bytes(unsigned char* buf, int num) { return RAND_bytes(buf, num); }
#endif /* JX_CRYPTO_TEST_HOST */
#endif /* !_WIN32 */
