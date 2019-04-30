#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include "cuckoofilter.h"
#include "simplehash.h"
using namespace std;

#define NR_BUCKETS 8
#define NR_SLOTS 4
#define MAX_KICKS 100
#define BUFSIZE 512

FILE *log_file;

vector<string> read_keys(const char *filename);

int main()
{
	vector<string> keys = read_keys("keys");

	/* 日志文件, 记录每一次扩容时的空间利用率(storage utilization). */
	log_file = fopen("log.txt", "w");

	cuckoo_filter *filter = create_cuckoo_filter(NR_BUCKETS, NR_SLOTS,
		rs_hash, fingerprint, MAX_KICKS);

	/* 将所有元素添加进去, 每次添加都不应出错. */
	for (string key : keys)
	{
		if (!cuckoo_filter_add(filter, key.c_str()))
		{
			printf("error: adding \"%s\".\n", key.c_str());
			exit(EXIT_FAILURE);
		}
	}

	printf(">>Cuckoo Filter<<\n");
	bool error = false;
	for (string key : keys)
	{
		/* 这些key是确实存在的, 和Bloom Filter一样, 不应发生误判. */
		if (!cuckoo_filter_contain(filter, key.c_str()))
		{
			printf("error: not found \"%s\".\n", key.c_str());
			error = true;
		}
	}
	if (!error) /* 没有发生误判 */
	{
		printf("successful: all keys have been added into cuckoo filter.\n");
	}
	printf("number of items: %d\n", filter->nr_items);
	printf("size of buckets: %d * %d\n", filter->nr_buckets, filter->nr_slots);
	printf("storage utilization: %.2f%%\n",
		(double)filter->nr_items / (filter->nr_buckets * filter->nr_slots) * 100);

	/* 删除所有元素 */
	for (string key : keys)
	{
		if (!cuckoo_filter_remove(filter, key.c_str()))
		{
			printf("error: removing \"%s\".\n", key.c_str());
			exit(EXIT_FAILURE);
		}
	}
	/* 确认删除成功 */
	for (string key : keys)
	{
		if (cuckoo_filter_contain(filter, key.c_str()))
		{
			printf("error: \"%s\" still exists.\n", key.c_str());
			exit(EXIT_FAILURE);
		}
	}
	assert(filter->nr_items == 0);
	printf("successful: all keys have been removed.\n");

	cuckoo_filter_free(filter);
	fclose(log_file);
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
