#ifndef VIRTUAL_NODE_H
#define VIRTUAL_NODE_H

#include "node.h"

class VirtualNode :public Node
{
public:
	VirtualNode(string name, unsigned int hashValue);
	virtual ~VirtualNode();
	virtual bool IsPhysicalNode() override;
};

#endif /* VIRTUAL_NODE_H */
