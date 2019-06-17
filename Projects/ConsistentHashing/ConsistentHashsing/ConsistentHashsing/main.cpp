#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <regex>
#include "consistenthash.h"
using namespace std;

#define BUFSIZE 512

vector<string> read_nodes(const char *filename);
bool virtual_name(char *name);
void test1();
void test2();
void test3();
void test4();
void interact();

int main()
{
	int op;
	cout << " 1. run test1().\n";
	cout << " 2. run test2().\n";
	cout << " 3. run test3().\n";
	cout << " 4. run test4().\n";
	cout << " 5. run interact().\n";

	cout << "operation code? ";
	cin >> op;
	switch (op)
	{
	case 1: test1(); break;
	case 2: test2(); break;
	case 3: test3(); break;
	case 4: test4(); break;
	case 5: interact(); break;
	default:
		cout << "Invalid number.\n";
		break;
	}
}

vector<string> read_nodes(const char *filename)
{
	vector<string> nodes;
	char buff[BUFSIZE];
	int len = 0;
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
		len = strlen(buff);
		if (buff[len - 1] == '\n' || buff[len - 1] == '\r')
		{
			buff[len - 1] = '\0';
		}
		nodes.push_back(buff);
	}

	fclose(fp);
	return nodes;
}

bool virtual_name(char *name)
{
	regex pattern("#\\[[0-9]+\\].*");
	return regex_match(name, pattern);
}

/* 测试: 添加物理节点和虚拟节点, 添加数据, 查询数据, 删除物理节点 */
void test1()
{
	vector<string> nodes = read_nodes("nodes");
	ConsistentHashing ch;

	cout << "---- test1 ----\n";

	/* 添加物理节点 */
	cout << "inserting physical nodes...\n";
	for (string node : nodes)
	{
		if (!ch.InsertPhysicalNode(node))
		{
			cout << "error: inserting physical node " << node.c_str() << ".\n";
		}
	}

	/* 选择添加虚拟节点 */
	char c;
	int n;
	cout << "do you wanna add virtual nodes? (y/n) ";
	cin >> c;
	if (c == 'y')
	{
		cout << "how many virtual nodes for each physical node? ";
		cin >> n;

		cout << "inserting virtual nodes...\n";
		for (string node : nodes)
		{
			if (!ch.InsertVirtualNodes(node, n))
			{
				cout << "error: inserting virtual nodes for " << node.c_str() << ".\n";
			}
		}
	}

	/* 添加2倍于物理节点个数的数据 */
	cout << "inserting data...\n";
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		ch.InsertData(i);
	}
	ch.PrintRing();

	/* 查询数据. 需要对每一个节点、每一个数据都进行测试. */
	cout << "searching data...\n";
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		for (string node : nodes)
		{
			assert(ch.LookupData(node, i) != "");

			//printf("starting from %s, found %d at %s\n",
				//node.c_str(), i, ch.LookupData(node, i).c_str());
		}
		//cout << "searching data " << i << " done.\n";
		//cout << "press any key to continue...\n";
		//_getch();
	}

	ch.PrintRing();
	printf("balance factor: %.2f.\n", ch.GetBalanceFactor());

	cout << "---- test1 passed ----\n";
}

/* 测试: 数据删除 */
void test2()
{
	vector<string> nodes = read_nodes("nodes");
	ConsistentHashing ch;

	cout << "---- test2 ----\n";

	/* 添加物理节点 */
	cout << "inserting physical nodes...\n";
	for (string node : nodes)
	{
		if (!ch.InsertPhysicalNode(node))
		{
			cout << "error: inserting physical node " << node.c_str() << ".\n";
		}
	}

	/* 选择添加虚拟节点 */
	char c;
	int n;
	cout << "do you wanna add virtual nodes? (y/n) ";
	cin >> c;
	if (c == 'y')
	{
		cout << "how many virtual nodes for each physical node? ";
		cin >> n;

		cout << "inserting virtual nodes...\n";
		for (string node : nodes)
		{
			if (!ch.InsertVirtualNodes(node, n))
			{
				cout << "error: inserting virtual nodes for " << node.c_str() << ".\n";
			}
		}
	}

	/* 添加2倍于物理节点个数的数据 */
	cout << "inserting data...\n";
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		ch.InsertData(i);
	}
	ch.PrintRing();

	/* 选择一个节点, 以它为起点删除所有数据 */
	cout << "deleting all data from a specified node...\n";
	char startNode[32];
	cout << "start node? ";
	cin >> startNode;
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		string node = ch.RemoveData(startNode, i);
		assert(node != "");

		printf("data %d removed from %s.\n", i, node.c_str());
	}

	ch.PrintRing();
	printf("balance factor: %.2f.\n", ch.GetBalanceFactor());

	cout << "---- test2 passed ----\n";
}

/* 测试: 数据更新 */
void test3()
{
	vector<string> nodes = read_nodes("nodes");
	ConsistentHashing ch;

	cout << "---- test3 ----\n";

	/* 添加物理节点 */
	cout << "inserting physical nodes...\n";
	for (string node : nodes)
	{
		if (!ch.InsertPhysicalNode(node))
		{
			cout << "error: inserting physical node " << node.c_str() << ".\n";
		}
	}

	/* 选择添加虚拟节点 */
	char c;
	int n;
	cout << "do you wanna add virtual nodes? (y/n) ";
	cin >> c;
	if (c == 'y')
	{
		cout << "how many virtual nodes for each physical node? ";
		cin >> n;

		cout << "inserting virtual nodes...\n";
		for (string node : nodes)
		{
			if (!ch.InsertVirtualNodes(node, n))
			{
				cout << "error: inserting virtual nodes for " << node.c_str() << ".\n";
			}
		}
	}

	/* 添加2倍于物理节点个数的数据 */
	cout << "inserting data...\n";
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		ch.InsertData(i);
	}
	ch.PrintRing();

	/* 更新数据 */
	cout << "updating data...\n";
	char startNode[32];
	int oldData, newData;
	for (;;)
	{
		cout << "start node? (input \"quit\" to quit)";
		cin >> startNode;
		if (!strcmp(startNode, "quit")) { break; }

		cout << "oldData and newData? ";
		cin >> oldData >> newData;

		string node = ch.UpdateData(startNode, oldData, newData);
		if (node != "")
		{
			printf("oldData %d removed, newData %d inserted at %s.\n", oldData, newData, node.c_str());
		}
		else
		{
			printf("error: updating data from %d to %d.\n", oldData, newData);
		}
		ch.PrintRing();
	}

	cout << "---- test3 passed ----\n";
}

/**
 * 测试: 节点的添加导致数据重新分派.
 *
 * 先添加一个物理节点和全部数据, 然后依次添加剩余的物理节点, 每次加入新的节点都会
 * 引起数据的重新分派, 最终结果应该与test1一次性添加全部物理节点和数据的结果相同.
 * 同样地, 添加虚拟节点也会引起数据的重新分派, 如果添加的虚拟节点和test1一致,那么
 * 最终结果也应该相同.
 */
void test4()
{
	vector<string> nodes = read_nodes("nodes");
	ConsistentHashing ch;

	cout << "---- test4 ----\n";

	/* 添加一个物理节点和全部数据 */
	cout << "inserting physical node " << nodes[0].c_str() << ".\n";
	if (!ch.InsertPhysicalNode(nodes[0]))
	{
		cout << "error: inserting physical node " << nodes[0].c_str() << ".\n";
	}

	cout << "inserting data...\n";
	for (int i = 0; i < nodes.size() * 2; i++)
	{
		ch.InsertData(i);
	}
	ch.PrintRing();

	cout << "press any key to continue...\n";
	_getch();
	cout << "inserting remaining physical nodes...\n";
	for (int i = 1; i < nodes.size(); i++)
	{
		//cout << "inserting physical node " << nodes[i].c_str() << ". press any key to continue...\n";
		//_getch();
		if (!ch.InsertPhysicalNode(nodes[i]))
		{
			cout << "error: inserting physical node " << nodes[0].c_str() << ".\n";
		}
	}

	/* 选择添加虚拟节点 */
	char c;
	int n;
	cout << "do you wanna add virtual nodes? (y/n) ";
	cin >> c;
	if (c == 'y')
	{
		cout << "how many virtual nodes for each physical node? ";
		cin >> n;

		cout << "inserting virtual nodes...\n";
		for (string node : nodes)
		{
			if (!ch.InsertVirtualNodes(node, n))
			{
				cout << "error: inserting virtual nodes for " << node.c_str() << ".\n";
			}
		}
	}

	ch.PrintRing();
	printf("balance factor: %.2f.\n", ch.GetBalanceFactor());

	cout << "---- test4 passed ----\n";
}


/* 交互式操作 */
void interact()
{
	ConsistentHashing ch;
	int op, nr, oldData, newData;
	char name[32];
	string node;

	for (;;)
	{
		cout << "\n---- interact ----\n";
		cout << "1. Insert physical node.\n";
		cout << "2. Insert virtual node.\n";
		cout << "3. Insert data.\n";
		cout << "4. Remove physical node.\n";
		cout << "5. Remove virtual node.\n";
		cout << "6. Remove data.\n";
		cout << "7. Look up data.\n";
		cout << "8. Update data.\n";

		cout << "operation code? ";
		cin >> op;
		switch (op)
		{
		case 1: /* Insert physical node */
			cout << "name? ";
			cin >> name;
			if (ch.InsertPhysicalNode(name))
			{
				cout << "successful\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 2: /* Insert virtual node */
			cout << "which physical node needs virtual nodes? ";
			cin >> name;
			cout << "how many? ";
			cin >> nr;
			if (ch.InsertVirtualNodes(name, nr))
			{
				cout << "successful\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 3: /* Insert data */
			cout << "data? ";
			cin >> newData;
			node = ch.InsertData(newData);
			if (node != "")
			{
				cout << "data " << newData << " saved at " << node.c_str() << ".\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 4: /* Remove physical node */
			cout << "name? ";
			cin >> name;
			if (ch.RemovePhysicalNode(name))
			{
				cout << "successful\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 5: /* Remove virtual node */
			cout << "name? ";
			cin >> name;
			if (ch.RemoveVirtualNode(name))
			{
				cout << "successful\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 6: /* Remove data */
			cout << "data? ";
			cin >> newData;
			cout << "starting node? ";
			cin >> name;
			node = ch.RemoveData(name, newData);
			if (node != "")
			{
				cout << "data " << newData << " removed from " << node.c_str() << ".\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		case 7: /* Look up data */
			cout << "data? ";
			cin >> newData;
			cout << "starting node? ";
			cin >> name;
			node = ch.LookupData(name, newData);
			if (node != "")
			{
				cout << "data " << newData << " found at " << node.c_str() << ".\n";
			}
			else
			{
				cout << "error\n";
			}
			break;
		case 8: /* Update data */
			cout << "oldData and newData? ";
			cin >> oldData >> newData;
			cout << "starting node? ";
			cin >> name;
			node = ch.UpdateData(name, oldData, newData);
			if (node != "")
			{
				cout << "old data " << oldData << " removed.\n";
				cout << "new data " << newData << " inserted at " << node.c_str() << ".\n";
			}
			else
			{
				cout << "error\n";
			}
			ch.PrintRing();
			break;
		default:
			cout << "Invalid number!\n";
			cin.clear();
			cin.sync();
			break;
		}
	}
}