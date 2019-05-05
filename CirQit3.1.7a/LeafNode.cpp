#include "LeafNode.h"

LeafNode::LeafNode( string &name, Circuit *c )
    : DagNode( name, c ) 
{
	type = LEAF;
}

void LeafNode::initializeTrueFalseWatches()
{
	// Do nothing
}

int LeafNode::notify( int , short, DagNode *, bool *remove )
{
	assert(false); // Can this ever happen?
	return OK;
}

int LeafNode::propagate(short type)
{
	for(int t = 0; t < VAL_TYPES; t++)
	{
		if(!isAssigned(t))
			continue;
		int result = notifyWatches(t);
		if(result != OK)
			return result;
	}
	return OK;
}

bool LeafNode::isDecision()
{
	if(isAssigned(TYPE_1LINE))
	{
		assert(isAssigned(TYPE_0LINE) || isJustified(TYPE_0LINE));
		return false;
	}

	if(isAssigned(TYPE_0LINE))
	{
		assert(isJustified(TYPE_1LINE));
		return false;
	}

	//(*) Consider picking off individually justified
	//return !isAllJustified();

	for(int t = 0; t < VAL_TYPES; t++)
	{
		if(isJustified(t))
			return false;
	}
	return true;
}
