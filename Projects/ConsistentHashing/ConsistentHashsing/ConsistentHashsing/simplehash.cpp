#include <string.h>
#include "simplehash.h"

/* RS */
unsigned int rs_hash(const char *str)
{
	unsigned int hash = 0;
	int a = 378551, b = 63689;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash = hash*a + str[i];
		a = a*b;
	}
	return hash;
}

/* JS */
unsigned int js_hash(const char *str)
{
	unsigned int hash = 1315423911;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash ^= ((hash << 5) + str[i] + (hash >> 2));
	}
	return hash;
}

/* ELF */
unsigned int elf_hash(const char *str)
{
	unsigned int hash = 0, x = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash = (hash << 4) + str[i];
		if ((x = hash & 0xF0000000) != 0)
		{
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}
	return hash;
}

/* BKDP */
unsigned int bkdr_hash(const char *str)
{
	unsigned int hash = 0;
	int seed = 13131;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash = (hash*seed) + str[i];
	}
	return hash;
}

/* SDBM */
unsigned int sdbm_hash(const char *str)
{
	unsigned int hash = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash = str[i] + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

/* DBJ */
unsigned int dbj_hash(const char *str)
{
	unsigned int hash = 5381;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		hash = ((hash << 5) + hash) + str[i];
	}
	return hash;
}

/* DEK */
unsigned int dek_hash(const char *str)
{
	unsigned int len = strlen(str);
	int hash = len;
	for (int i = 0; i < len; i++)
	{
		hash = ((hash << 5) ^ (hash >> 27) ^ str[i]);
	}
	return hash;
}

/* AP */
unsigned int ap_hash(const char *str)
{
	unsigned int hash = 0xAAAAAAAA;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		if (i & 1)
		{
			hash ^= (~((hash << 11) + str[i] ^ (hash >> 5)));
		}
		else
		{
			hash ^= ((hash << 7) ^ str[i] * (hash >> 3));
		}
	}
	return hash;
}

/* Murmur */
unsigned int murmur_hash(const char *str)
{
	unsigned int m = 0x5bd1e995;
	int len = strlen(str);
	int r = 24;
	int seed = 97;
	unsigned int h = seed ^ len;
	// Mix 4 bytes at a time into the hash
	while (len >= 4)
	{
		unsigned int k = *(unsigned int *)str;
		k *= m;
		k ^= k >> r;
		k *= m;
		h *= m;
		h ^= k;
		str += 4;
		len -= 4;
	}
	// Handle the last few bytes of the input array
	switch (len)
	{
	case 3: h ^= str[2] << 16;
	case 2: h ^= str[1] << 8;
	case 1: h ^= str[0];
		h *= m;
	};
	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	return h;
}
