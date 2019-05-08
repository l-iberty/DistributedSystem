#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <assert.h>
#include <regex>
#include "consistentHash.h"
#include "simpleHash.h"

ConsistentHashing::ConsistentHashing()
{
}

ConsistentHashing::~ConsistentHashing()
{
}

/**
 * @brief 添加物理节点
 *
 * @param p_name 物理节点的名字
 * 
 * @return 添加是否成功
 */
bool ConsistentHashing::InsertPhysicalNode(string p_name)
{
	if (mRing.size() == UINT_MAX) { return false; } /* 哈希环已满 */

	unsigned int hv = Hash(p_name);

	/* 找到空槽位后插入 */
	while (mRing.size() < UINT_MAX)
	{
		if (mRing.find(hv) == mRing.end())
		{
			mRing[hv] = new PhysicalNode(p_name, hv);
			mRing[hv]->SetPhysicalNode(mRing[hv]);

			Node *prev = FindPrev(mRing[hv]);
			Node *next = FindNext(mRing[hv]);
			if (prev && next)
			{
				mRing[hv]->SetPrev(prev);
				next->SetPrev(mRing[hv]);
				RedispatchData(mRing[hv]);
			}

			UpdateFingerTable();
			return true;
		}
		hv = Hash(to_string(hv)); /* Hash collision, rehash */
	}

	assert(0); /* we should never arrive here. */
	return false;
}

/**
 * @brief 为某个物理节点添加虚拟节点
 *
 * @param p_name 物理节点的名字
 * @param n      添加几个虚拟节点
 * 
 * @return 添加是否成功
 */
bool ConsistentHashing::InsertVirtualNodes(string p_name, int n)
{
	if (mRing.size() == UINT_MAX) { return false; } /* 哈希环已满 */

	Node *physicalNode = mRing.find(LookupNode(p_name))->second;

	int i = physicalNode->GetVirtualNodes().size(); /* 虚拟节点的编号 */
	for (int j = 0; j < n; j++, i++)
	{
		/* 设置虚拟节点名, 并计算哈希值 */
		string v_name = "#[" + to_string(i) + "]" + p_name;
		unsigned int hv = Hash(v_name);

		/* 找到空槽位后插入 */
		while (mRing.size() < UINT_MAX)
		{
			if (mRing.find(hv) == mRing.end())
			{
				mRing[hv] = new VirtualNode(v_name, hv);
				mRing[hv]->SetPhysicalNode(physicalNode);
				physicalNode->InsertVirtualNode(mRing[hv]);

				Node *prev = FindPrev(mRing[hv]);
				Node *next = FindNext(mRing[hv]);
				if (prev && next)
				{
					mRing[hv]->SetPrev(prev);
					next->SetPrev(mRing[hv]);
					RedispatchData(mRing[hv]);
				}
				break;
			}
			hv = Hash(to_string(hv)); /* Hash collision, reHash */
		}
	}

	UpdateFingerTable();

	return true;
}

bool ConsistentHashing::RemovePhysicalNode(string p_name)
{
	unsigned int hv = LookupNode(p_name);
	if (!mRing[hv]->IsPhysicalNode()) return false;
	
	/**
	 * 删除一个物理节点前需要先将它的所有虚拟节点删除, 否则, 如果物理节点p1的后继是
	 * 虚拟节点v1, 而v1映射到p1, 那么将p1保存的数据转移到v1就会形成矛盾.
	 */
	vector<Node*> virtualNodes = mRing[hv]->GetVirtualNodes();
	for (Node *node : virtualNodes)
	{
		if (!RemoveVirtualNode(node->GetName()))
		{
			return false;
		}
	}

	/* 找到后继节点 */
	Node *next = FindNext(mRing[hv]);
	if (next == NULL) /* 哈希环上只有一个节点 */
	{
		printf("all nodes Removed.\n");
		mRing.erase(hv);
		return true;
	}

	/* 将数据拷贝到后继节点 */
	multimap<unsigned int, int> data = mRing[hv]->GetData();
	multimap<unsigned int, int>::iterator it = data.begin();
	for (; it != data.end(); it++)
	{
		next->InsertData(it->first, it->second);
	}

	next->SetPrev(FindPrev(mRing[hv]));
	mRing.erase(hv);
	UpdateFingerTable();

	printf("data moved from %s to %s.\n", p_name.c_str(), next->GetName().c_str());

	return true;
}

bool ConsistentHashing::RemoveVirtualNode(string v_name)
{
	unsigned int hv = LookupNode(v_name);
	if (mRing[hv]->IsPhysicalNode()) return false;

	Node *next = FindNext(mRing[hv]);
	if (next != NULL)
	{
		next->SetPrev(FindPrev(mRing[hv]));
	}

	mRing.erase(hv);
	UpdateFingerTable();

	return true;
}

/**
 * @brief 添加数据data. 计算data的哈希值来确定应该添加到哪个节点.
 *        如果是物理节点则直接存储, 如果是虚拟节点则将data存储到对应的物理节点.
 *
 * @param data 添加时需将其转换为string来计算哈希值
 *
 * @return 保存数据data的节点名
 */
string ConsistentHashing::InsertData(int data)
{
	unsigned int hv = Hash(to_string(data));
	
	/* 找到哈希环中第一个大于hv的节点, 按照顺时针原则, data应存储在该节点. */
	map<unsigned int, Node*>::iterator it = mRing.upper_bound(hv);
	it = WrapAround(it);
	if (it == mRing.end())
	{
		assert(0);
		return "";
	}

	Node *node = it->second;
	node->InsertData(hv, data);

	return node->GetName();
}

/**
 * @brief 查找保存数据data的节点.
 *        如果data不存在, LookupData返回应该保存data的节点, 除非指定的节点name不存在.
 *
 * @param name 查询的起始节点名
 *
 * @return 节点名
 */
string ConsistentHashing::LookupData(string name, int data)
{
	unsigned int hv = LookupData(LookupNode(name), data);
	if (hv == (unsigned int)-1) { return ""; }

	return mRing[hv]->GetName();
}

/**
 * @brief 修改数据
 *
 * @param name 查询的起始节点
 *
 * @return 保存新数据newData的节点名
 */
string ConsistentHashing::UpdateData(string name, int oldData, int newData)
{
	if (RemoveData(name, oldData) == "") { return ""; }

	InsertData(newData);

	return LookupData(name, newData);
}

/**
 * @brief 删除数据data. 删除时一定要到物理节点上删除.
 *
 * @param name 查询的起始节点
 *
 * @return 原先保存data的物理节点名
 */
string ConsistentHashing::RemoveData(string name, int data)
{
	unsigned int hv = LookupData(LookupNode(name), data);
	if (hv == (unsigned int)-1) { return ""; }

	Node *node = mRing[hv];
	node->RemoveData(Hash(to_string(data)), data);
	
	return node->GetName();
}

/* 打印每个物理节点上保存的数据 */
void ConsistentHashing::PrintRing()
{
	int n = 0;
	FILE *fp = fopen("log.txt", "w");

	map<unsigned int, Node*>::iterator it = mRing.begin();
	for (; it != mRing.end(); it++)
	{
		unsigned int hv = it->first;
		Node *node = it->second;

		if (node->IsPhysicalNode()) /* 仅打印物理节点的信息 */
		{
			multimap<unsigned int, int> data = node->GetData();
			fprintf(fp, "\n%s(%X){%d} -> ", node->GetName().c_str(), hv, data.size());

			multimap<unsigned int, int>::iterator it2 = data.begin();
			for (; it2 != data.end(); it2++)
			{
				fprintf(fp, "%d ", it2->second);
				//fprintf(fp, "%d(%X) ", it2->second, it2->first);
			}
			n += data.size();
		}
	}
	fprintf(fp, "\nnumber of data: %d", n); /* 数据总数 */
	
	fclose(fp);
}

/**
 * 集群的均衡因子越小, 数据在节点上的分布越平衡.
 * 设随机变量X表示节点上存储的数据的个数, 将X的方差定义为集群的均衡因子
 */
double ConsistentHashing::GetBalanceFactor()
{
	double s = 0, n = 0, t, avg, res;

	Node *node;
	multimap<unsigned int, int> data;
	map<unsigned int, Node*>::iterator it = mRing.begin();
	for (; it != mRing.end(); it++)
	{
		node = it->second;
		data = node->GetData();
		if (node->IsPhysicalNode())
		{
			n++;
			s += data.size();
		}
	}
	if (n <= 1) return 0;

	avg = s / n;
	s = 0;
	for (it = mRing.begin(); it != mRing.end(); it++)
	{
		node = it->second;
		data = node->GetData();
		if (node->IsPhysicalNode())
		{
			t = data.size();
			s += (t - avg) * (t - avg);
		}
	}
	res = s / (n - 1);
	return res;
}

unsigned int ConsistentHashing::Hash(string str)
{
	return murmur_hash(str.c_str());
}

/* 回绕 */
map<unsigned int, Node*>::iterator ConsistentHashing::WrapAround(map<unsigned int, Node*>::iterator it)
{
	if (it == mRing.end()) { it = mRing.begin(); }
	return it;
}

/**
 * @brief 查找保存数据data的物理/虚拟节点
 *
 * @param node 查询的起始物理/虚拟节点位置(哈希值)
 *
 * @return 物理/虚拟节点的位置(哈希值). 查找失败则返回(-1)
 */
unsigned int ConsistentHashing::LookupData(unsigned int node, int data)
{
	unsigned int res = (unsigned int)-1;
	unsigned int hv = Hash(to_string(data));

	if (mRing.find(node) == mRing.end()) { return res; }
	
	/* 判断node的后继节点next是否持有数据data, 但首先要进行边界检查 */
	map<unsigned int, Node*>::iterator next = mRing.upper_bound(node);
	map<unsigned int, Node*>::iterator current = mRing.find(node);

	if (next == mRing.end())
	{
		if (hv >= node || hv < mRing.begin()->first)
		{
			return mRing.begin()->first;
		}
		return LookupData(mRing.begin()->first, data);
	}
	else if (hv >= node && hv < next->first)
	{
		return next->first;
	}

	/* 从node的路由表的最远处开始查找, 但首先要进行边界检查 */
	multimap<unsigned int, unsigned int> fingerTable = mRing[node]->GetFingerTable();
	multimap<unsigned int, unsigned int>::reverse_iterator rit = fingerTable.rbegin();
	if (hv < current->first && hv < rit->first)
	{
		res = LookupData(rit->first, data);
		if (res != (unsigned int)-1) { return res; }

		return LookupData(mRing.begin()->first, data);
	}

	for (; rit != fingerTable.rend(); rit++)
	{
		unsigned int temp_node = rit->first;
		if (temp_node > node && temp_node <= hv)
		{
			res = LookupData(temp_node, data);
			if (res != (unsigned int)-1) { return res; }
		}
	}

	return res;
}

/**
 * @brief 在哈希环上查找节点
 *
 * @param name 物理/虚拟节点的名字
 *
 * @return 节点位置, 即哈希值. 若查找失败则返回(-1)
 */
unsigned int ConsistentHashing::LookupNode(string name)
{
	unsigned int hv = Hash(name);
	map<unsigned int, Node*>::iterator it = mRing.find(hv);

	if (it == mRing.end())
	{
		assert(0);
		return (unsigned int)-1;
	}

	Node *node = it->second;
	while (it != mRing.end() && node->GetName() != name)
	{
		hv = Hash(to_string(hv));
		it = mRing.find(hv);
		node = it->second;
	}

	if (it != mRing.end() && node->GetName() == name)
	{
		return hv;
	}

	assert(0);
	return (unsigned int)-1;
}

/* 更新节点的路由表. 每次插入/删除节点后都需要更新所有节点的路由表 */
void ConsistentHashing::UpdateFingerTable()
{
	map<unsigned int, Node*>::iterator it = mRing.begin();
	for (; it != mRing.end(); it++)
	{
		unsigned int hv = it->first;
		Node *node = it->second;
		node->ClearFingerTable();

		for (int i = 0; i < HASHING_SIZE; i++)
		{
			unsigned long long lhv = hv + (1 << i);
			map<unsigned int, Node*>::iterator next = mRing.upper_bound((unsigned int)(lhv % UINT_MAX));
			next = WrapAround(next);
			node->InsertFingerTableEntry(next->first, next->first);
		}
	}
}

/**
 * 在哈希环中找到current的前驱.
 * case1: 如果哈希环中只有curent一个节点, 则认为它没有前驱, 返回NULL.
 * case2: 如果存在两个及以上的节点, 且current是头节点, 根据回绕原则它的前驱为尾节点.
 * case3: 如果存在两个及以上的节点, 且current不是头节点, 顺序遍历即可.
 */
Node *ConsistentHashing::FindPrev(Node *current)
{
	map<unsigned int, Node*>::reverse_iterator last = mRing.rbegin();
	map<unsigned int, Node*>::iterator it = mRing.begin();

	/* case1 */
	if (mRing.size() <= 1)
	{
		return NULL;
	}

	/* case2 */
	if (it->first == current->GetHashValue())
	{
		return last->second;
	}

	/* case3 */
	for (; it != mRing.end(); it++)
	{
		if (it->first == current->GetHashValue())
		{
			return (--it)->second;
		}
	}

	assert(0); /* we should never arrive here. */
	return NULL;
}

/**
 * 在哈希环中找到current的后继.
 * case1: 如果哈希环中只有current一个节点, 则认为它没有后继, 返回NULL.
 * case2: 如果存在两个及以上的节点, 且current是尾节点, 根据回绕原则它的后继为头节点
 * case3: 如果存在两个及以上的节点, 且current不是尾节点, 直接调用`upper_bound()'即可.
 */
Node *ConsistentHashing::FindNext(Node *current)
{
	map<unsigned int, Node*>::reverse_iterator last = mRing.rbegin();
	map<unsigned int, Node*>::iterator head = mRing.begin();
	
	/* case1 */
	if (mRing.size() == 1)
	{
		return NULL;
	}

	/* case2 */
	if (last->first == current->GetHashValue())
	{
		return head->second;
	}

	/* case3 */
	return mRing.upper_bound(current->GetHashValue())->second;
}

/**
 * 在Chord环上添加节点current后, 需要重新分派相关节点上的数据. 调用该函数前,
 * 应该设置好current及其后继的前驱. 注意到新添加的节点current尚未保存有数据.
 */
void ConsistentHashing::RedispatchData(Node *current)
{
	Node *prev, *next;
	multimap<unsigned int, int> data;
	multimap<unsigned int, int>::iterator it;
	unsigned int k;
	int v;

	prev = current->GetPrev(); /* prev为NULL只有一种情况: 哈希环中只有current一个节点 */
	next = FindNext(current);  /* next为NULL只有一种情况: 哈希环中只有current一个节点 */

	if (prev && next)
	{
		/* 重新分派节点上的数据 */
		if (prev->GetHashValue() == next->GetHashValue()) /* prev和next是同一个节点 */
		{
			/**
			 * 要进行两个数据转移:
			 * -------------------------------------
			 * |      区间       |     转移方向     |
			 * -------------------------------------
			 * | [current, next) | current -> next |
			 * -------------------------------------
			 * | [next, current) | next -> current |
			 * -------------------------------------
			 * 按顺时针方向, 这两个区间都可分为“跨越边界”和“未跨越边界”两种
			 */
			
			/* 数据转移: current -> next */
			if (current->GetHashValue() < next->GetHashValue()) /* [current, next)未跨越边界 */
			{
				data = current->GetData();
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= current->GetHashValue() && k < next->GetHashValue())
					{
						current->RemoveData(k, v);
						next->InsertData(k, v);
					}
				}
			}
			else if (current->GetHashValue() > next->GetHashValue()) /* [current, next)跨越边界 */
			{
				data = current->GetData();
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= current->GetHashValue() || k < next->GetHashValue())
					{
						current->RemoveData(k, v);
						next->InsertData(k, v);
					}
				}
			}
			
			/* 数据转移: next -> current */
			if (next->GetHashValue() < current->GetHashValue()) /* [next, current)未跨越边界 */
			{
				data = next->GetData();
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= next->GetHashValue() && k < current->GetHashValue())
					{
						next->RemoveData(k, v);
						current->InsertData(k, v);
					}
				}
			}
			else if (next->GetHashValue() > current->GetHashValue()) /* [next, current)跨越边界 */
			{
				data = next->GetData();
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= next->GetHashValue() || k < current->GetHashValue())
					{
						next->RemoveData(k, v);
						current->InsertData(k, v);
					}
				}
			}
		}
		else /* prev和next是两个不同的节点 */
		{
			/**
			 * 要进行数据转移:
			 * -------------------------------------
			 * |      区间       |     转移方向     |
			 * -------------------------------------
			 * | [prev, current) | next -> current |
			 * -------------------------------------
			 * 按顺时针方向, 该区间可分为“跨越边界”和“未跨越边界”两种
			 */

			data = next->GetData();
			if (prev->GetHashValue() < current->GetHashValue()) /* [prev, current)未跨越边界 */
			{
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= prev->GetHashValue() && k < current->GetHashValue())
					{
						next->RemoveData(k, v);
						current->InsertData(k, v);
					}
				}
			}
			else if (prev->GetHashValue() > current->GetHashValue()) /* [prev, current)跨越边界 */
			{
				for (it = data.begin(); it != data.end(); it++)
				{
					k = it->first, v = it->second;
					if (k >= prev->GetHashValue() || k < current->GetHashValue())
					{
						next->RemoveData(k, v);
						current->InsertData(k, v);
					}
				}
			}
		}
	}
}
