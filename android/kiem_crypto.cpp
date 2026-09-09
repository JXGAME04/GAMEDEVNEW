// [ANDROID 08/09] Kiem thuat toan AES-128 / SHA-256 cua Platform/KPosixCrypto.cpp bang vector chuan (FIPS-197 C.1, FIPS-180-4 "abc")
// ngay tren Windows:  cl /EHsc /DJX_CRYPTO_TEST_HOST /I..\Sources\Engine\Src\Platform kiem_crypto.cpp && kiem_crypto.exe
#include <stdio.h>
#include <string.h>
#include "../Sources/Engine/Src/Platform/KPosixCrypto.cpp"

static int hex(const char* h, unsigned char* out, int n) { for (int i = 0; i < n; i++) { unsigned v; sscanf(h + i * 2, "%2x", &v); out[i] = (unsigned char)v; } return n; }
static void dump(const unsigned char* p, int n) { for (int i = 0; i < n; i++) printf("%02x", p[i]); }

int main()
{
	int loi = 0;
	// FIPS-197 C.1: AES-128
	unsigned char key[16], pt[16], ct[16], out[16];
	hex("000102030405060708090a0b0c0d0e0f", key, 16);
	hex("00112233445566778899aabbccddeeff", pt, 16);
	hex("69c4e0d86a7b0430d8cdb78070b4c55a", ct, 16);
	AES_KEY ek, dk;
	AES_set_encrypt_key(key, 128, &ek); AES_encrypt(pt, out, &ek);
	printf("AES128 enc: "); dump(out, 16); printf(" %s\n", memcmp(out, ct, 16) == 0 ? "OK" : "SAI"); if (memcmp(out, ct, 16)) loi++;
	AES_set_decrypt_key(key, 128, &dk); AES_decrypt(ct, out, &dk);
	printf("AES128 dec: "); dump(out, 16); printf(" %s\n", memcmp(out, pt, 16) == 0 ? "OK" : "SAI"); if (memcmp(out, pt, 16)) loi++;
	// FIPS-197 C.3: AES-256
	unsigned char key256[32], ct256[16];
	hex("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f", key256, 32);
	hex("8ea2b7ca516745bfeafc49904b496089", ct256, 16);
	AES_set_encrypt_key(key256, 256, &ek); AES_encrypt(pt, out, &ek);
	printf("AES256 enc: "); dump(out, 16); printf(" %s\n", memcmp(out, ct256, 16) == 0 ? "OK" : "SAI"); if (memcmp(out, ct256, 16)) loi++;
	AES_set_decrypt_key(key256, 256, &dk); AES_decrypt(ct256, out, &dk);
	printf("AES256 dec: "); dump(out, 16); printf(" %s\n", memcmp(out, pt, 16) == 0 ? "OK" : "SAI"); if (memcmp(out, pt, 16)) loi++;
	// SHA-256("abc") = ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad
	unsigned char md[32], want[32];
	hex("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", want, 32);
	SHA256((const unsigned char*)"abc", 3, md);
	printf("SHA256 abc: "); dump(md, 32); printf(" %s\n", memcmp(md, want, 32) == 0 ? "OK" : "SAI"); if (memcmp(md, want, 32)) loi++;
	// SHA-256 chuoi dai 56 byte (2 khoi)
	hex("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1", want, 32);
	SHA256((const unsigned char*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, md);
	printf("SHA256 2 khoi: "); dump(md, 32); printf(" %s\n", memcmp(md, want, 32) == 0 ? "OK" : "SAI"); if (memcmp(md, want, 32)) loi++;
	printf("%s\n", loi ? "CO LOI" : "TAT CA OK");
	return loi;
}
