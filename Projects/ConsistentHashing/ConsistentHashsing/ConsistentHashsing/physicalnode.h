#ifndef PHYSICAL_NODE_H
#define  PHYSICAL_NODE_H

#include "node.h"

class PhysicalNode :public Node
{
public:
	PhysicalNode(string name, unsigned int hashValue);
	virtual ~PhysicalNode();
	virtual bool IsPhysicalNode() override;
};

#endif /* PHYSICAL_NODE_H */
