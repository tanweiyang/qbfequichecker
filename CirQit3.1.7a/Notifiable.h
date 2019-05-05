
#ifndef NOTIFIABLE_H
#define NOTIFIABLE_H

#include "Common.h"


class DagNode;

class Notifiable
{
public:
	// Accept notification that this watch got a value.
	// value - the new value of the watch
	// watch - the watch that got the value
	// remove - into this boolean, put true if you wish to be removed
	//          from the child's notify list
	virtual int notify(int value, short type, DagNode * watch, bool *remove ) = 0;

    virtual int notifyJustified( DagNode * node, short type, bool *remove )
	{	return OK; };
    
	// Conditional notify - depending on the value, call 'notify' or 'notifyJustified'
	inline int notifyC(int value, short type, DagNode * watch, bool *remove )
	{
		if(value == VALUE_DONT_CARE)
			return notifyJustified(watch, type, remove);
		else
		{
			assert(value == VALUE_TRUE || value == VALUE_FALSE);
			return notify(value, type, watch, remove);
		}
	}

    virtual bool isLiteral()
    {   return false;   };

	// Using "child" to distinguish between possibly many clauses
	//    represented by this notifiable, return the number of clauses
	//       or a literal at the given location in the clause
	// TODO: (*) one for NOT, ANDOR and LEARNED_SET
	virtual int getClauseSize(DagNode* child) { return 0; };
	virtual Literal *getClauseLiteral(DagNode* child, int index) { return NULL; };
	virtual void printProofClause(DagNode* child) { return; };
    
    virtual ~Notifiable() {};
};

#endif

