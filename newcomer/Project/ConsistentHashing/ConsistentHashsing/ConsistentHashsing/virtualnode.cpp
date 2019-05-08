#include "virtualnode.h"

VirtualNode::VirtualNode(string name, unsigned int hashValue) :Node(name, hashValue)
{
}

VirtualNode::~VirtualNode()
{
}

bool VirtualNode::IsPhysicalNode()
{
	return false;
}

