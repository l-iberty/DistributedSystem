#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "bloomfilter.h"

bloom_filter* create_bloom_filter(int nr_bits, vector<hash_func>& hash_funcs)
{
	bloom_filter *filter = new bloom_filter;
	filter->bitset_len = ROUND_UP(nr_bits, CHAR_BIT);
	filter->bitset = new char[filter->bitset_len / CHAR_BIT];
	memset(filter->bitset, 0, filter->bitset_len / CHAR_BIT);
	filter->nr_items = 0;
	filter->hash_funcs = hash_funcs;

	return filter;
}

void bloom_filter_add(bloom_filter *filter, const char *key)
{
	for (hash_func hash : filter->hash_funcs)
	{
		SET_BIT(filter->bitset, hash(key) % filter->bitset_len);
	}
	filter->nr_items++;
}

bool bloom_filter_contain(bloom_filter *filter, const char *key)
{
	for (hash_func hash : filter->hash_funcs)
	{
		if (!GET_BIT(filter->bitset, hash(key) % filter->bitset_len))
		{
			return false;
		}
	}
	return true;
}

void bloom_filter_free(bloom_filter *filter)
{
	delete[] filter->bitset;
	delete filter;
}
