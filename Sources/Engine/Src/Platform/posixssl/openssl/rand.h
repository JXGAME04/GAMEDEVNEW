/* [ANDROID 08/09] Tap con API OpenSSL: RAND_bytes - cai trong Platform/KPosixCrypto.cpp (getrandom / /dev/urandom). Chi cho ban Android. */
#ifndef JX_POSIXSSL_RAND_H
#define JX_POSIXSSL_RAND_H
#ifdef __cplusplus
extern "C" {
#endif
int RAND_bytes(unsigned char *buf, int num);
int RAND_pseudo_bytes(unsigned char *buf, int num);
#ifdef __cplusplus
}
#endif
#endif
