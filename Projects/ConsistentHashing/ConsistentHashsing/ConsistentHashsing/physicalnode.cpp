#include "physicalnode.h"

PhysicalNode::PhysicalNode(string name, unsigned int hashValue) :Node(name, hashValue)
{
}

PhysicalNode::~PhysicalNode()
{
}

bool PhysicalNode::IsPhysicalNode()
{
	return true;
}

