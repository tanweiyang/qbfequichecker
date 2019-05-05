#include "NotGate.h"

NotGate::NotGate( string &name, Circuit *c )
    : DagNode( name, c )
{
    type = NOT_GATE;
}

void NotGate::initializeTrueFalseWatches()
{
	for(int t = 0; t < VAL_TYPES; t++)
	{
		children[0]->addNotify( VALUE_TRUE, t, this );
		children[0]->addNotify( VALUE_FALSE, t, this );
	}
}

int NotGate::notify(int value, short type, DagNode *watch, bool *remove)
{
	(*remove) = false;

	int newval = VALUE_TRUE;
	if(value == VALUE_TRUE)
		newval = VALUE_FALSE;

	circuit->beginNewReason(this, newval == VALUE_TRUE? children[0] : NULL);

	Signal val;
	val.value = newval;
	val.type = type;
	val.justified = true;
	addToUPStack( this, val, circuit->last_reason, true );
	return OK;
}

int NotGate::propagate(short type)
{
	int retval = OK;
	// (*) universal conflict
	retval = notifyWatches(type);
	if( retval != OK ) return retval;

	assert(getValue(type) != VALUE_DONT_CARE );
	int child_val = VALUE_UNASSIGNED;
	switch( getValue(type) ) {
		case VALUE_TRUE:        
			child_val = VALUE_FALSE;
			break;
		case VALUE_FALSE:       
			child_val = VALUE_TRUE;
			break;
		default:
			assert(false);
	}

	if(children[0]->getValue(type) != child_val)
	{
		Signal val;
		val.value = child_val;
		val.type = type;
		val.justified = false;
		circuit->beginNewReason(this, val.value == VALUE_TRUE? children[0] : NULL);
		addToUPStack( children[0], val, circuit->last_reason, true);
	}

	return retval;
}

int NotGate::getClauseSize(DagNode* child)
{
	return 2;
};

Literal *NotGate::getClauseLiteral(DagNode* child, int index)
{
	DagNode* var = (index == 0? this : children[0]);
	int value = (child==NULL? VALUE_TRUE : VALUE_FALSE );
	return getLiteral(var, value);
}

void NotGate::printProofClause(DagNode *child)
{
	int num = (child == NULL? 1 : 0);
	Common::proof_file << "gate-" << name << "-" << num;
}

