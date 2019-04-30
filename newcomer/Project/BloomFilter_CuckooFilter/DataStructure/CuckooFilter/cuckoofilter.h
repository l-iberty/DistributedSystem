#ifndef CUCKOO_FILTER_H
#define CUCKOO_FILTER_H

#include <vector>
#include "simplehash.h"
using namespace std;

#define ROUND_UP(a,b) ((a)+(b)-1)/(b)*(b) /* 把a按照b取上整 */
#define EXPANDING_FACTOR 1.5 /* 扩展因子. 扩容时将每个buckets扩展到原来的1.5倍 */
#define EXPANDING_SIZE 5 /* 扩容时将每个bucket扩展几路槽位 */

typedef unsigned int(*hash_func)(const char*);
typedef unsigned char(*fingerprint_func)(const char*);

struct cuckoo_filter
{
	unsigned char **buckets; /* 存放指纹的二维哈希表 */
	int nr_buckets; /* 每个桶对应buckets的一行. nr_buckets = buckets.row_size */
	int nr_slots;   /* 每个桶有几路槽位. nr_slots = buckets.col_size */
	int nr_items;   /* buckets存放了多少个指纹? */
	int *slot_idx;     /* 保存每个bucket中的下一个可被踢出的指纹所在的槽位 */
	int max_kicks;
	hash_func hash;
	fingerprint_func fingerprint;
};

cuckoo_filter* create_cuckoo_filter(
	int nr_buckets,
	int nr_slots,
	hash_func hash,
	fingerprint_func fingerprint,
	int max_kicks);

bool cuckoo_filter_add(cuckoo_filter *filter, const char *key);

bool cuckoo_filter_contain(cuckoo_filter *filter, const char *key);

bool cuckoo_filter_remove(cuckoo_filter *filter, const char *key);

bool cuckoo_filter_update(cuckoo_filter *filter, const char *old_key, const char *new_key);

void cuckoo_filter_expand(cuckoo_filter *filter);

void cuckoo_filter_free(cuckoo_filter *filter);

#endif /* CUCKOO_FILTER_H */
