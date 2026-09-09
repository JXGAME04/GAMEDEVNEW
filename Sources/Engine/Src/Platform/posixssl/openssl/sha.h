/* [ANDROID 08/09] Tap con API OpenSSL: SHA-256 (BauCua.cpp) - cai trong Platform/KPosixCrypto.cpp. Chi cho ban Android. */
#ifndef JX_POSIXSSL_SHA_H
#define JX_POSIXSSL_SHA_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SHA256_DIGEST_LENGTH 32
#define SHA_DIGEST_LENGTH 20
typedef struct SHA256state_st { unsigned int h[8]; unsigned long long len; unsigned char buf[64]; unsigned int num; } SHA256_CTX;
int SHA256_Init(SHA256_CTX *c);
int SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA256_Final(unsigned char *md, SHA256_CTX *c);
unsigned char *SHA256(const unsigned char *d, size_t n, unsigned char *md);
#ifdef __cplusplus
}
#endif
#endif
