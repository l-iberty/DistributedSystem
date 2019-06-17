#ifndef CONSISTENT_HASH_H
#define CONSISTENT_HASH_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "node.h"
#include "virtualnode.h"
#include "physicalnode.h"
using namespace std;

#define HASHING_SIZE 32

class ConsistentHashing
{
public:
	ConsistentHashing();
	~ConsistentHashing();
	bool InsertPhysicalNode(string p_name);
	bool InsertVirtualNodes(string p_name, int n);
	bool RemovePhysicalNode(string p_name);
	bool RemoveVirtualNode(string v_name);
	string InsertData(int data);
	string LookupData(string name, int data);
	string UpdateData(string name, int oldData, int newData);
	string RemoveData(string name, int data);
	void PrintRing();
	double GetBalanceFactor();

private:
	unsigned int Hash(string str);
	map<unsigned int, Node*>::iterator WrapAround(map<unsigned int, Node*>::iterator it);
	unsigned int LookupData(unsigned int node, int data);
	unsigned int LookupNode(string name);
	void UpdateFingerTable();
	Node *FindPrev(Node *current);
	Node *FindNext(Node *current);
	void RedispatchData(Node *current);

private:
	/**
	 * 哈希环, 仅保存物理节点和虚拟节点, 不保存数据. 对数据进行哈希后, 将其保存到对应的物理节点.
	 *
	 * 如何从名字上区分物理节点和虚拟节点?
	 * 如果节点名形如"#[vn]name", 那么这是一个虚拟节点, vn表示这是物理节点name的第几个
	 * 虚拟节点. 一个物理节点可以被多个虚拟节点映射, 例如"#[5]test_node"表示的是物理节
	 * 点test_node的第5个虚拟节点.
	 */
	map<unsigned int, Node*> mRing;
};

#endif /* CONSISTENT_HASH_H */
