#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "bloomfilter.h"
#include "simplehash.h"
using namespace std;

#define BUFSIZE 512

vector<string> read_keys(const char *filename);

int main()
{
	int bloom_size; /* Bloom Filter的大小, 即bitset的长度 */
	vector<string> keys = read_keys("keys");

	vector<hash_func> hash_funcs = 
	{
		rs_hash,
		js_hash,
		elf_hash,
		bkdr_hash,
		sdbm_hash,
		dbj_hash,
		dek_hash,
		ap_hash
	};

	cout << "size of bloom filter? ";
	cin >> bloom_size;
	bloom_filter *filter = create_bloom_filter(bloom_size, hash_funcs);

	/* 一边查询一边添加. 所有元素都是独一无二的, 所以只要添加前该元素被判断为"存在", 就记为一次"false positive". */
	int err_cnt = 0;
	for (string key : keys)
	{
		if (bloom_filter_contain(filter, key.c_str()))
		{
			//printf("Bloom filter already contains \"%s\".\n", key.c_str());
			err_cnt++;
		}
		bloom_filter_add(filter, key.c_str());
	}

	printf(">>Bloom Filter<<\n");
	bool error = false;
	for (string key : keys)
	{
		if (!bloom_filter_contain(filter, key.c_str()))
		{
			/* 如果元素确实存在, 那么就不应被误判为"不存在". */
			printf("error: not found \"%s\"\n.", key.c_str());
			error = true;
		}
	}
	if (!error) /* 没有发生误判 */
	{
		printf("successful: all keys have been added into bloom filter.\n");
	}
	printf("number of keys: %d\n", filter->nr_items);
	printf("false positive rate: %.2f%%\n", ((double)err_cnt / filter->nr_items) * 100);

	/**
	 * false positive : 过滤器判断某个元素存在, 但实际上该元素并不存在.
	 *
	 * bitset的长度越长, 元素个数越少, false positive rate越低; 反之,
	 * bitset的长度越短, 元素个数越多, false positive rate越高.
	 */

	bloom_filter_free(filter);
}

vector<string> read_keys(const char *filename)
{
	vector<string> keys;
	char buff[BUFSIZE];
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		cerr << "No such file." << endl;
		exit(EXIT_FAILURE);
	}

	while (!feof(fp))
	{
		memset(buff, 0, BUFSIZE);
		fgets(buff, BUFSIZE, fp);
		buff[strlen(buff) - 1] = '\0'; /* 剔除末尾的'\n' */
		keys.push_back(buff);
	}

	fclose(fp);
	return keys;
}
