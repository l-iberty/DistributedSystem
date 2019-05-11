#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <assert.h>
#include "skiplist.h"
using namespace std;

#define N 100
#define HEIGHT(x) (int)ceil((double)log(N) / log(2.0))

int main()
{
	int nums[] = { 46, 55, 37, -3, 17, 2, 21, -33, -19, 0, INT_MAX, INT_MIN };
	SkipList skipList(10);

	cout << "test data insertion..." << endl;
	for (int n : nums)
	{
		skipList.Insert(n);
	}
	skipList.Display();
	
	cout << "test data search..." << endl;
	for (int n : nums)
	{
		Node *res = skipList.Find(n);
		assert(res);
	}

	for (int n : nums)
	{
		Node *res = skipList.Find(n + 100);
		assert(!res);
	}

	cout << "test data deletion..." << endl;
	int data;
	while (skipList.Size() > 0)
	{
		cout << "\ninput data to delete? ";
		cin >> data;
		if (skipList.Delete(data))
		{
			cout << "data " << data << " deleted.\n";
		}
		else
		{
			cout << "error deleting data " << data << ".\n";
		}
		skipList.Display();

		cin.clear();
		cin.sync();
	}
	assert(skipList.Size() == 0);
}