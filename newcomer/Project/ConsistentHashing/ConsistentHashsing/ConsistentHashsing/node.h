#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <map>
using namespace std;

class Node
{
public:
	Node(string name, unsigned int hashValue);
	virtual ~Node();
	virtual bool IsPhysicalNode() = 0;
	string GetName();
	unsigned int GetHashValue();
	multimap<unsigned int, int> GetData();
	void InsertData(unsigned int k, int v);
	bool RemoveData(unsigned int k, int v);
	bool UpdateData(unsigned int k, int v);
	multimap<unsigned int, unsigned int> GetFingerTable();
	void InsertFingerTableEntry(unsigned int k, unsigned int v);
	void ClearFingerTable();
	Node *GetPrev();
	void SetPrev(Node *prev);
	Node *GetPhysicalNode();
	void SetPhysicalNode(Node *node);
	vector<Node*> GetVirtualNodes();
	void InsertVirtualNode(Node *node);

protected:
	string mName;
	unsigned int mHashValue;
	multimap<unsigned int, int> mData; /* 对mData的操作都会映射到物理节点上, 调用者无需区分是物理节点还是虚拟节点 */
	multimap<unsigned int, unsigned int> mFingerTable;
	Node *mPrev; /* 当前节点在哈希环中的前驱 */
	Node *mPhysicalNode; /* 当前节点对应的物理节点 */
	vector<Node*> mVirtualNodes; /* 如果当前节点是物理, 则保存对应的全部虚拟节点 */
};

#endif /* NODE_H */
