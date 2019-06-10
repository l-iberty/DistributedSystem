#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stdio.h>
#include <map>

#ifndef INT_MAX
#define INT_MAX (2147483647)
#endif

#ifndef INT_MIN
#define INT_MIN (-2147483647-1)
#endif


struct Node
{
	int Data;
	struct Node *Flink; /* forward link */
	struct Node *Blink; /* backward link */
	struct Node *Dlink; /* down link */
	Node(int data) :Flink(NULL), Blink(NULL), Dlink(NULL), Data(data) {}
};

class SkipList
{
public:
	SkipList(int maxSize);
	~SkipList();
	void Insert(int data);
	Node *Find(int data);
	bool Delete(int data);
	int Size();
	void Display();
	SkipList *DeepCopy();

private:
	int Random();
	Node *Insert(Node *head, int data);
	void PrintNodes(Node *head);
	Node *DeepCopy(Node *head, std::map<int, Node*>& m);

private:
	Node **mNodeHeaders;
	int mMaxLevel;
	int mMaxSize;
	int mTopLevel;
	int mSize;
	unsigned int mSeed;
};

#endif /* SKIP_LIST_H */
