/* [ANDROID 08/09] Tap con API OpenSSL ma client dung (KIniFile.cpp: AES-128 ECB giai ma) - cai trong Platform/KPosixCrypto.cpp.
   Chi cho ban Android (thu muc posixssl chi vao duong include cua CMake Android); Windows dung OpenSSL vcpkg that. */
#ifndef JX_POSIXSSL_AES_H
#define JX_POSIXSSL_AES_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define AES_BLOCK_SIZE 16
#define AES_MAXNR 14
typedef struct aes_key_st { unsigned int rd_key[4 * (AES_MAXNR + 1)]; int rounds; } AES_KEY;
int  AES_set_encrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);
int  AES_set_decrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);
#ifdef __cplusplus
}
#endif
#endif
