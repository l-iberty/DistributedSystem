#include <iostream>
#include <assert.h>
#include "skiplist.h"

int main()
{
	SkipList<int> list;

	assert(list.FindLast()->key == 0);
	assert(list.FindGreaterOrEqual(2, nullptr) == nullptr);
	assert(list.FindGreaterOrEqual(1000, nullptr) == nullptr);

	for (int i = 2; i <= 1000; i += 2) list.Insert(i);

	assert(list.FindLast()->key == 1000);
	assert(list.FindGreaterOrEqual(1, nullptr)->key == 2);
	assert(list.FindGreaterOrEqual(12, nullptr)->key == 12);
	assert(list.FindGreaterOrEqual(98, nullptr)->key == 98);
	assert(list.FindGreaterOrEqual(480, nullptr)->key == 480);
	assert(list.FindGreaterOrEqual(57, nullptr)->key == 58);
	assert(list.FindGreaterOrEqual(101, nullptr)->key == 102);
	assert(list.FindGreaterOrEqual(199, nullptr)->key == 200);
	assert(list.FindGreaterOrEqual(999, nullptr)->key == 1000);
	assert(list.FindGreaterOrEqual(1000, nullptr)->key == 1000);
	assert(list.FindGreaterOrEqual(1001, nullptr) == nullptr);

	assert(list.FindEqual(1) == nullptr);
	assert(list.FindEqual(57) == nullptr);
	assert(list.FindEqual(101) == nullptr);
	assert(list.FindEqual(199) == nullptr);
	assert(list.FindEqual(999) == nullptr);
	assert(list.FindEqual(1001) == nullptr);
	assert(list.FindEqual(2000) == nullptr);

	assert(list.FindEqual(2)->key == 2);
	assert(list.FindEqual(12)->key == 12);
	assert(list.FindEqual(92)->key == 92);
	assert(list.FindEqual(102)->key == 102);
	assert(list.FindEqual(1000)->key == 1000);

	assert(list.FindLessThan(1)->key == 0);
	assert(list.FindLessThan(57)->key == 56);
	assert(list.FindLessThan(101)->key == 100);
	assert(list.FindLessThan(199)->key == 198);
	assert(list.FindLessThan(1000)->key == 998);
	assert(list.FindLessThan(1001)->key == 1000);
	assert(list.FindLessThan(2000)->key == 1000);
}