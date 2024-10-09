#include "BaseBTNode.h"
#include "AIComponent.h"

const AIComponent* BaseBTNode::GetParent() const
{
    return Parent;
}

void BaseBTNode::SetParent(AIComponent* parent)
{
    Parent = parent;
}
