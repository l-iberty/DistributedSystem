#ifndef SKIP_LIST_H
#define SKIP_LIST_H

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
	SkipList(int maxLevel);
	~SkipList();
	void Insert(int data);
	Node *Find(int data);
	bool Delete(int data);
	int Size();
	void Display();

private:
	int Random();
	Node *Insert(Node *head, int data);
	void PrintNodes(Node *head);

private:
	Node **mNodeHeaders;
	int mMaxLevel;
	int mTopLevel;
	int mSize;
	unsigned int mSeed;
};

#endif /* SKIP_LIST_H */
