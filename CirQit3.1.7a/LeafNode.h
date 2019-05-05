#ifndef LEAFNODE_H
#define LEAFNODE_H

#include "DagNode.h"

class LeafNode : public DagNode
{
private:
public:
	LeafNode(string &name, Circuit *c);

	bool isLeaf() 
		{   return true;    };

	void initializeTrueFalseWatches();
	int notify(int value, short type, DagNode *watch, bool *remove);
	int propagate(short type);

	// Return true if this variable is suitable to be a decision variable
	bool isDecision();

	/**************************************************************************
	Scoring
	**************************************************************************/
	int VSIDS_score[VAL_TYPES];
	int cube_score;

	void incrementVSIDS(int type)
	{   VSIDS_score[type]++;   };

	void decayVSIDS(int type)
	{
		VSIDS_score[type] /= 2;
	}

};

#endif
