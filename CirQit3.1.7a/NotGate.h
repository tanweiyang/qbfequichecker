#ifndef NOTGATE_H
#define NOTGATE_H

#include "DagNode.h"
#include "Circuit.h"

class NotGate : public DagNode
{
public:
	NotGate( string &name, Circuit *c );

	void initializeTrueFalseWatches();

	int notify(int value, short type, DagNode *watch, bool *remove);
	int propagate(short type);

	int getClauseSize(DagNode* child);
	Literal *getClauseLiteral(DagNode* child, int index);
	void printProofClause(DagNode *child);
};

#endif
