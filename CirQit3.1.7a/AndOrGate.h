#ifndef ANDORGATE_H
#define ANDORGATE_H

#include "DagNode.h"
#include "Circuit.h"

class AndOrGate : public DagNode
{
private:
	DagNode * watch1[VAL_TYPES];
    DagNode * watch2[VAL_TYPES];
	int up_value; // The value that all children of this node must propagate upwards
	int down_value; // The value that != up_value, which must propagate downwards to all children

	// Should only be called if this gate's value is down_value and it's not justified!
	// Find and set watches that are existentials whose value != down_value;
	// Here we assume nothing about existing watches, and the algorithm will
	// work whether or not they are set correctly initially.
	// If only one existential watch found, make the second one the universal that 
	//         is scoped by it - this will ensure that we catch the case of a single 
	//         leading existential in any case: if the universal gets a value,
	//         or if we're backtracked to a case where other existentials are available
	// Raise conflicts or propagate the value if possible
	// Set the new watches. Assumes that (changedWatch==NULL) == (remove==NULL)
	// If changedWatch!=NULL and it needs to be replaced, set *remove to true instead
	// of removing self from its parent list.
	int makeExistWatches(short type, DagNode *changedWatch, bool *remove);
public:
	AndOrGate( int type, string &name, Circuit *c );
	// (*) verify structure

	void initializeTrueFalseWatches();

	/*
	The invariant for the watches for this type of gate:
	Either all watches are existential, or one watch is existential 
	and the other is a universal that is not scoped by it.
	That way, we can detect and force if there is a single existential child scoping all universal children
	*/
	int notify(int value, short type, DagNode *watch, bool *remove);
	int propagate(short type);

	int getClauseSize(DagNode* child);
	Literal *getClauseLiteral(DagNode* child, int index);
	void printProofClause(DagNode *child);
};

#endif
