#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <assert.h>
#include "skiplist.h"

int main()
{
	int nums[] = { 46, 55, 37, -3, 17, 2, 21, -33, -19, 0, INT_MAX, INT_MIN };
	SkipList list(100);

	std::cout << "test data insertion..." << std::endl;
	for (int n : nums)
	{
		list.Insert(n);
	}
	list.Display();

	SkipList *copy = list.DeepCopy();
	
	std::cout << "test data search..." << std::endl;
	for (int n : nums)
	{
		Node *res = list.Find(n);
		assert(res);
	}

	for (int n : nums)
	{
		Node *res = list.Find(n + 100);
		assert(!res);
	}

	std::cout << "test data deletion..." << std::endl;
	int data;
	while (list.Size() > 0)
	{
		std::cout << "\ninput data to delete? ";
		std::cin >> data;
		if (list.Delete(data))
		{
			std::cout << "data " << data << " deleted.\n";
		}
		else
		{
			std::cout << "error deleting data " << data << ".\n";
		}
		list.Display();

		std::cin.clear();
		std::cin.sync();
	}
	assert(list.Size() == 0);

	std::cout << "deep copy:" << std::endl;
	copy->Display();
	delete copy;
}