#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "cuckoofilter.h"

extern FILE *log_file;

/* buckets[idx]是否为空 */
static bool empty_bucket(cuckoo_filter *filter, int idx);

/* 查找[idx]的下一个非空槽位(可被踢出的槽位) */
static int lookup_evicted(cuckoo_filter *filter, int idx);

/* 查找bucket[idx]的第一个空槽位 */
static int lookup_empty(cuckoo_filter *filter, int idx);

/* 在查找bucket[idx]中查找指纹fp所在槽位 */
static int lookup_fp(cuckoo_filter *filter, int idx, unsigned char fp);

static void print_buckets(cuckoo_filter *filter);


cuckoo_filter* create_cuckoo_filter(
	int nr_buckets,
	int nr_slots,
	hash_func hash,
	fingerprint_func fingerprint,
	int max_kicks)
{
	cuckoo_filter *filter = (cuckoo_filter*)malloc(sizeof(cuckoo_filter));
	
	/* allocate memory for buckets. */
	filter->buckets = (unsigned char**)malloc(sizeof(unsigned char*) * nr_buckets);
	assert(filter->buckets);

	for (int i = 0; i < nr_buckets; i++)
	{
		filter->buckets[i] = (unsigned char*)malloc(nr_slots);
		assert(filter->buckets[i]);
		memset(filter->buckets[i], 0, nr_slots);
	}

	filter->slot_idx = (int*)malloc(sizeof(int) * nr_buckets);
	assert(filter->slot_idx);
	for (int i = 0; i < nr_buckets; i++)
	{
		filter->slot_idx[i] = -1;
	}

	filter->nr_buckets = nr_buckets;
	filter->nr_slots = nr_slots;
	filter->nr_items = 0;
	filter->max_kicks = max_kicks;
	filter->hash = hash;
	filter->fingerprint = fingerprint;

	return filter;
}

bool cuckoo_filter_add(cuckoo_filter *filter, const char *key)
{
	unsigned int idx, idx1, idx2, c;
	unsigned int hash_val = filter->hash(key);
	unsigned char fp = filter->fingerprint(key);
	char fp_str[2] = { fp, 0 };

	filter->nr_items++;

	for (int i = 0; i < filter->max_kicks; i++)
	{
		/* 计算第一个bucket的位置, 如果能在其中找到一个空槽位, 就将指纹fp填入. */
		idx1 = hash_val % filter->nr_buckets;
		if ((c = lookup_empty(filter, idx1)) != -1)
		{
			filter->buckets[idx1][c] = fp;
			return true;
		}

		/* 计算第二个bucket的位置, 如果能在其中找到一个空槽位, 就将指纹fp填入. */
		idx2 = (idx1 ^ filter->hash(fp_str)) % filter->nr_buckets;
		if ((c = lookup_empty(filter, idx2)) != -1)
		{
			filter->buckets[idx2][c] = fp;
			return true;
		}

		/* 两个bucket的槽位都被占用, 从中随机选择一个踢出. */
		srand((unsigned int)time(NULL));
		idx = (rand() % 2) ? idx1 : idx2;
		
		/* 找到bucket中下一个可被踢出的槽位. */
		c = lookup_evicted(filter, idx); 
		if (c == -1) return false;

		/* 取出原指纹(已被踢出)后, 填入新指纹. */
		unsigned char evicted_fp = filter->buckets[idx][c];
		fp_str[0] = evicted_fp;
		filter->buckets[idx][c] = fp;
		fp = evicted_fp;

		/* 计算被踢指纹的新的位置. */
		hash_val = idx ^ filter->hash(fp_str);
	}

	//printf("No enough space. Before expanding...\n");
	//print_buckets(filter);

	/* 记录每次扩容时的空间利用率 */
	fprintf(log_file, "storage utilization when buckets are full: %.2f%%\n",
		(double)filter->nr_items / (filter->nr_buckets * filter->nr_slots) * 100);

	/* 扩容后直接把fp放进新增的槽位. */
	cuckoo_filter_expand(filter);
	c = lookup_empty(filter, idx);
	if (c == -1) return false;

	filter->buckets[idx][c] = fp;
	
	//printf("After expanding...\n");
	//print_buckets(filter);

	return true;
}

bool cuckoo_filter_contain(cuckoo_filter *filter, const char *key)
{
	unsigned int idx1, idx2;
	unsigned int hash_val = filter->hash(key);
	unsigned char fp = filter->fingerprint(key);
	char fp_str[2] = { fp, 0 };

	/* 计算两个bucket的位置 */
	idx1 = hash_val % filter->nr_buckets;
	idx2 = (idx1 ^ filter->hash(fp_str)) % filter->nr_buckets;

	/**
	 * 在任意一个bucket中只要找到key对应的指纹, 就说明key存在. 但是和Bloom Filter一样,
	 * 有可能把实际上不存在的key误判为存在, 因为计算哈希指纹时, 有可能多个key对应与同一个
	 * 指纹. 同样地, 和Bloom Filter一样, 只要key确实存在, 就不会发生误判.
	 */
	return (lookup_fp(filter, idx1, fp) != -1) || (lookup_fp(filter, idx2, fp) != -1);
}

bool cuckoo_filter_remove(cuckoo_filter *filter, const char *key)
{
	unsigned int idx1, idx2, i;
	unsigned int hash_val = filter->hash(key);
	unsigned char fp = filter->fingerprint(key);
	char fp_str[2] = { fp, 0 };

	/* 计算两个bucket的位置 */
	idx1 = hash_val % filter->nr_buckets;
	idx2 = (idx1 ^ filter->hash(fp_str)) % filter->nr_buckets;

	/**
	 * 找到指纹fp所在槽位, 清零. 注意到这种删除操作是安全的, 假设有n个key的指纹是相同的,
	 * 对应的n个相同的指纹都被保存在两个bucket内, 删除一个指纹后还剩下(n-1)个指纹, 另外
	 * 的(n-1)个key必能哈希到这两个bucket, 所以必能匹配到各自的指纹, 保证查找总能成功.
	 */
	if ((i = lookup_fp(filter, idx1, fp)) != -1)
	{
		filter->buckets[idx1][i] = 0;
		filter->nr_items--;
		return true;
	}
	else if ((i = lookup_fp(filter, idx2, fp)) != -1)
	{
		filter->buckets[idx2][i] = 0;
		filter->nr_items--;
		return true;
	}
	else /* 在两个bucket内都未能匹配到指纹fp */
	{
		return false;
	}
}

bool cuckoo_filter_update(cuckoo_filter *filter, const char *old_key, const char *new_key)
{
	if (cuckoo_filter_remove(filter, old_key)) /* 先删除old_key */
	{
		if (cuckoo_filter_add(filter, new_key)) /* 再添加new_key */
		{
			return true;
		}
		else /* 添加new_key失败, 恢复old_key */
		{
			cuckoo_filter_add(filter, old_key);
		}
	}
	return false;
}

/**
 * 扩容时是给每一个bucket扩容, 即增加bucket的槽位, 而不是增加bucket的个数.
 * 如果增加bucket的个数, 则nr_bucket值的改变会导致后续查询操作出错, 因为
 * 定位bucket时, 需要把哈希值向nr_bucket取模. 如果添加key时的nr_bucket和
 * 查询key时的nr_bucket不相等, 那么显然会出错.
 */
void cuckoo_filter_expand(cuckoo_filter *filter)
{
	//int new_nr_slots = filter->nr_slots + EXPANDING_SIZE; /* 空间利用率较高 */
	int new_nr_slots = filter->nr_slots * EXPANDING_FACTOR; /* 空间利用率较低, 但能减低扩容的频率 */
	int diff = new_nr_slots - filter->nr_slots;
	
	for (int r = 0; r < filter->nr_buckets; r++)
	{
		filter->buckets[r] = (unsigned char*)realloc(filter->buckets[r], new_nr_slots);
		
		assert(filter->buckets[r]);

		/* 将新增的槽位置0 */
		memset(filter->buckets[r] + filter->nr_slots, 0, diff);
	}
	filter->nr_slots = new_nr_slots;
}

void cuckoo_filter_free(cuckoo_filter *filter)
{
	for (int i = 0; i < filter->nr_buckets; i++)
	{
		free(filter->buckets[i]);
	}
	free(filter->buckets);
	free(filter->slot_idx);
	free(filter);
}

/* buckets[idx]是否为空 */
static bool empty_bucket(cuckoo_filter *filter, int idx)
{
	assert(idx >= 0 && idx < filter->nr_buckets);

	for (int i = 0; i < filter->nr_slots; i++)
	{
		if (filter->buckets[idx][i] != 0) { return false; }
	}
	return true;
}

/* 返回bucket[idx]的下一个非空槽位(可被踢出的槽位) */
static int lookup_evicted(cuckoo_filter *filter, int idx)
{
	assert(idx >= 0 && idx < filter->nr_buckets);

	filter->slot_idx[idx] = (filter->slot_idx[idx] + 1) % filter->nr_slots;
	return filter->slot_idx[idx];
}

/* 返回bucket[idx]的第一个空槽位 */
static int lookup_empty(cuckoo_filter *filter, int idx)
{
	return lookup_fp(filter, idx, 0);
}

/* 在查找bucket[idx]中查找指纹fp所在的槽位 */
static int lookup_fp(cuckoo_filter *filter, int idx, unsigned char fp)
{
	assert(idx >= 0 && idx < filter->nr_buckets);

	for (int i = 0; i < filter->nr_slots; i++)
	{
		if (filter->buckets[idx][i] == fp) { return i; }
	}
	return -1;
}

static void print_buckets(cuckoo_filter *filter)
{
	for (int i = 0; i < filter->nr_buckets; i++)
	{
		for (int j = 0; j < filter->nr_slots; j++)
		{
			printf("%.2x ", filter->buckets[i][j]);
		}
		printf("\n");
	}
}