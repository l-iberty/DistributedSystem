#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <map>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "random.h"

template<typename Key>
class SkipList
{
private:
	struct Node;
	
public:
	SkipList();
	~SkipList();
	Node* NewNode(const Key& key, int height);
	Node* FindGreaterOrEqual(const Key& key, Node** prev);
	Node* FindEqual(const Key& key);
	Node* FindLessThan(const Key& key);
	Node* FindLast();
	void Insert(const Key& key);
	void Display();

private:
	enum{ kMaxHeight=10 };
	int max_height_;
	Node* head_;
	Random rnd_;

	int RandomHeight();
};

template<typename Key>
struct SkipList<Key>::Node
{
	Key key;
	Node* next[1];
	Node(const Key& k) :key(k) {}
};

template<typename Key>
SkipList<Key>::SkipList()
	: max_height_(1),
	  head_(NewNode(0, kMaxHeight))
{
	for (int i = 0; i < kMaxHeight; i++)
	{
		head_->next[i] = nullptr;
	}
}

template<typename Key>
SkipList<Key>::~SkipList()
{
	Node* x = head_;
	Node* next = nullptr;
	for (; x != nullptr; x = next)
	{
		next = x->next[0];
		delete x;
	}
}

template<typename Key>
typename SkipList<Key>::Node* SkipList<Key>::NewNode(const Key& key, int height)
{
	char *mem = new char[sizeof(Node) + sizeof(Node*) * (height - 1)];
	return new (mem)Node(key);
}

template<typename Key>
typename SkipList<Key>::Node* SkipList<Key>::FindGreaterOrEqual(const Key& key, Node** prev)
{
	Node* x = head_;
	Node* next = nullptr;
	int level = max_height_ - 1;
	for (;;)
	{
		next = x->next[level];
		if (next != nullptr && key > next->key)
		{
			x = next;
		}
		else
		{
			if (prev != nullptr) prev[level] = x;
			if (level == 0) break;
			else level--;
		}
	}
	return next;
}

template<typename Key>
typename SkipList<Key>::Node* SkipList<Key>::FindEqual(const Key& key)
{
	Node* x = FindGreaterOrEqual(key, NULL);
	return (x != nullptr && x->key == key) ? x : nullptr;
}

template<typename Key>
typename SkipList<Key>::Node* SkipList<Key>::FindLessThan(const Key& key)
{
	Node* x = head_;
	int level = max_height_ - 1;
	for (;;)
	{
		Node* next = x->next[level];
		if (next != nullptr && next->key < key)
		{
			x = next;
		}
		else
		{
			if (level == 0) break;
			else level--;
		}
	}
	return x;
}

template<typename Key>
typename SkipList<Key>::Node* SkipList<Key>::FindLast()
{
	Node* x = head_;
	int level = max_height_ - 1;
	for (;;)
	{
		Node* next = x->next[level];
		if (next != nullptr)
		{
			x = next;
		}
		else
		{
			if (level == 0) break;
			else level--;
		}
	}
	return x;
}

template<typename Key>
void SkipList<Key>::Insert(const Key& key)
{
	Node* prev[kMaxHeight];
	Node* next = FindGreaterOrEqual(key, prev);
	
	assert(next == nullptr || next->key != key);

	int height = RandomHeight();
	if (height > max_height_)
	{
		for (int i = max_height_; i < height; i++) prev[i] = head_;
		max_height_ = height;
	}

	Node* x = NewNode(key, height);
	for (int i = 0; i < height; i++)
	{
		x->next[i] = prev[i]->next[i];
		prev[i]->next[i] = x;
	}
}

template<typename Key>
int SkipList<Key>::RandomHeight()
{
	int h = rnd_.Get() % kMaxHeight;
	if (h == 0) h++;
	return h;
}

template<typename Key>
void SkipList<Key>::Display()
{
	Node* x = head_;
	for (int level = max_height_ - 1; level >= 0; level--)
	{
		Node* next = x->next[level];
		printf("\nL[%d] ", level);
		for (; next != nullptr; next = next->next[level])
		{
			printf("%d ", next->key);
		}
	}
}

#endif /* SKIPLIST_H */
