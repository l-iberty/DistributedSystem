#include <assert.h>
#include "node.h"

Node::Node(string name, unsigned int hashValue)
{
	mName = name;
	mHashValue = hashValue;
	mPhysicalNode = NULL;
	mPrev = NULL;
}

Node::~Node()
{
}

string Node::GetName()
{
	return mName;
}

unsigned int Node::GetHashValue()
{
	return mHashValue;
}

multimap<unsigned int, int> Node::GetData()
{
	assert(mPhysicalNode);

	return mPhysicalNode->mData;
}

void Node::InsertData(unsigned int k, int v)
{
	assert(mPhysicalNode);

	mPhysicalNode->mData.insert(pair<unsigned int, int>(k, v));
}

bool Node::RemoveData(unsigned int k, int v)
{
	assert(mPhysicalNode);

	pair<multimap<unsigned int, int>::iterator, multimap<unsigned int, int>::iterator> ret;
	ret = mPhysicalNode->mData.equal_range(k);
	multimap<unsigned int, int>::iterator it = ret.first;
	for (; it != ret.second; it++)
	{
		if (it->second == v)
		{
		 	mPhysicalNode->mData.erase(it);
			return true;
		}
	}

	return false; /* data not found */
}

bool Node::UpdateData(unsigned int k, int v)
{
	assert(mPhysicalNode);

	if (!RemoveData(k, v)) { return false; }

	InsertData(k, v);
	return true;
}

multimap<unsigned int, unsigned int> Node::GetFingerTable()
{
	return mFingerTable;
}

void Node::InsertFingerTableEntry(unsigned int k, unsigned int v)
{
	mFingerTable.insert(pair<unsigned int, unsigned int>(k, v));
}

void Node::ClearFingerTable()
{
	mFingerTable.clear();
}

Node *Node::GetPrev()
{
	return mPrev;
}

void Node::SetPrev(Node *prev)
{
	mPrev = prev;
}

Node *Node::GetPhysicalNode()
{
	return mPhysicalNode;
}

void Node::SetPhysicalNode(Node *node)
{
	mPhysicalNode = node;
}

vector<Node*> Node::GetVirtualNodes()
{
	return mVirtualNodes;
}

void Node::InsertVirtualNode(Node *node)
{
	mVirtualNodes.push_back(node);
}
