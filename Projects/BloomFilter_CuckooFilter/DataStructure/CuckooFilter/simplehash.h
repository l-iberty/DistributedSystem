#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

/* RS */
unsigned int rs_hash(const char *str);

/* JS */
unsigned int js_hash(const char *str);

/* ELF */
unsigned int elf_hash(const char *str);

/* BKDP */
unsigned int bkdr_hash(const char *str);

/* SDBM */
unsigned int sdbm_hash(const char *str);

/* DBJ */
unsigned int dbj_hash(const char *str);

/* DEK */
unsigned int dek_hash(const char *str);

/* AP */
unsigned int ap_hash(const char *str);

/* 随意选择一个哈希函数用于计算指纹 */
unsigned char fingerprint(const char *str);

#endif /* SIMPLE_HASH_H */
