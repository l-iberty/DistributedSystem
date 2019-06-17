#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
using namespace std;

#define ROUND_UP(a,b) ((a)+(b)-1)/(b)*(b) /* 把a按照b取上整 */
#define SET_BIT(bitset,i) bitset[(i) / CHAR_BIT] |= (1 << ((i) % CHAR_BIT)) /* 将bitset的位i置1 */
#define GET_BIT(bitset,i) (bitset[(i) / CHAR_BIT] & (1 << ((i) % CHAR_BIT))) /* 取bitset的位i */

typedef unsigned int(*hash_func)(const char*);

struct bloom_filter
{
	char *bitset;   /* 二进制串, 长度是8的倍数 */
	int bitset_len; /* 二进制串bitset的长度 */
	int nr_items;   /* Bloom Filter包含的元素个数 */
	vector<hash_func> hash_funcs;  /* 哈希函数指针数组 */
};

bloom_filter* create_bloom_filter(int nr_bits, vector<hash_func>& hash_funcs);

void bloom_filter_add(bloom_filter *filter, const char *key);

bool bloom_filter_contain(bloom_filter *filter, const char *key);

void bloom_filter_free(bloom_filter *filter);

#endif /* BLOOM_FILTER_H */
