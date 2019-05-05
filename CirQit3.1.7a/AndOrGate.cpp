
#include "AndOrGate.h"

extern bool verbose;

AndOrGate::AndOrGate( int n_type, string &name, Circuit *c )
    : DagNode( name, c )
{
	assert(n_type == AND_GATE || n_type == OR_GATE);
    type = n_type;

	if(type == AND_GATE)
	{
		up_value = VALUE_FALSE;
		down_value = VALUE_TRUE;
	}
	else // OR_NODE
	{
		up_value = VALUE_TRUE;
		down_value = VALUE_FALSE;
	}
}

int AndOrGate::notify(int value, short type, DagNode *watch, bool *remove)
{
	(*remove) = false;
	if(value == up_value)
	{
		circuit->beginNewReason(this, watch);

		Signal value;
		value.value = up_value;
		value.type = type;
		value.justified = true;
		addToUPStack( this, value, circuit->last_reason, true );
		return OK;
	}
	else
	{
		if(getValue(type) == up_value && !isJustified(type))
		{
			return makeExistWatches(type, watch, remove);
		}
		else if(isJustified(type))
		{
			// If my value is down_value, then all children were set to down_value anyway
			//     and the procedure can force nothing
			// If my value is up_value and I'm justified, then there is a child who's up_value.
			//     can't force anything 
			// If justified because the gate is not needed, don't need to set any values
			return OK;
		}
		else
		{
			// Need to find another watch
			int retval = OK;

			DagNode **oldWatch;
			DagNode **otherWatch;

			if( watch == watch1[type] )
			{
				oldWatch = &(watch1[type]);
				otherWatch = &(watch2[type]);
			} else
			{
				oldWatch = &(watch2[type]);
				otherWatch = &(watch1[type]);
			}

			int new_watch_index = -1;
			for( int i = 0; i < num_children; i++ )
			{
				if(children[i] != (*otherWatch) && children[i]->getValue(type) != down_value)
				{
					new_watch_index = i;
					break;
				}
			}

			if( new_watch_index >= 0 ) 
			{	//Assign new watch
				(*remove) = true;
				children[ new_watch_index ]->addNotify( down_value, type, this );
				*oldWatch = children[new_watch_index];
			}
			else if( (*otherWatch)->getValue(type) == down_value )
			{
				// All children are 'down_value'

				circuit->beginNewReason(this, NULL);
				
				Signal value;
				value.value = down_value;
				value.type = type;
				value.justified = true;
				addToUPStack( this, value, circuit->last_reason, true );
			}
			else
			{
				// We should never have reached this point if the gate's value if up_value.
				// Instead, this should be covered in the case with makeExistWatches

				assert(getValue(type) != up_value);

				// a watch is now true
				// if the gate is 'up_value', then the last remaining watch must be the same value
				if( getValue(type) == up_value && !(*otherWatch)->getValue(type) != up_value)
				{
					
					circuit->beginNewReason(this, NULL);
					
					Signal value;
					value.value = up_value;
					value.type = type;
					value.justified = false;
					addToUPStack( *otherWatch, value, circuit->last_reason, true );
				}
			}

			return retval;
		}
	}
}

void AndOrGate::initializeTrueFalseWatches()
{
	for(int t = 0; t < VAL_TYPES; t++)
	{
		// Set the values of which all children must notify
		for( int i = 0; i < num_children; i++ )
		{
			children[i]->addNotify( up_value, t, this );
		}

		// set 2 other watches
		children[0]->addNotify( down_value, t, this );
		watch1[t] = children[0];
		children[1]->addNotify( down_value, t, this );
		watch2[t] = children[1];
	}
}

int AndOrGate::makeExistWatches(short type, DagNode *changedWatch, bool *remove)
{
	int retval = OK;

	assert(getValue(type) == up_value && !isJustified(type));
	int fw = -1;
	int sw = -1;
	// The child that has the minimum universal block, and its block
	int uniMinChild = -1;
	unsigned minUniBlock = (unsigned)-1;

	for(int i = 0; i < num_children; i++)
	{
		if(children[i]->getValue(type) == up_value)
		{
			// This can only be true if the justification record hasn't reached us yet
			// But we are justified and soon we'll know it.
			assert(children[i]->getDLevel(type) == getDLevel(type));
			return OK;
		}

		if(children[i]->getValue(type) != down_value)
		{
			if(children[i]->isUniversal(type))
			{
				if(children[i]->getBlockNum() < minUniBlock)
				{
					minUniBlock = children[i]->getBlockNum();
					uniMinChild = i;
				}
			}
			else
			{
				if(fw == -1)
					fw = i;
				else
				{
					sw = i;
					break;
				}
			}
		}
	}

	if(sw == -1)
	{
		if(fw == -1)
		{
			// No such children found
			//circuit->newLearnedSet(num_children + 1); (*)
			int result;
			if(type == TYPE_1LINE)
			{
				if( circuit->verbose ) 
					cout << "conflict " << name << " has only universal chil dren" << endl; //(*) fix spelling

				result = CONFLICT;
			}
			else
			{
				assert(type == TYPE_0LINE);
				if( circuit->verbose ) 
					cout << "solution " << name << " has only universal chil dren" << endl; //(*) fix spelling

				result = QSAT; //(*) Can eliminate QSAT and CONFLICT! Since it's now incorporated into learnedSet
			}

			circuit->resetLearnedSet(type, this, NULL);

			return result;
		}
		else
		{
			// A single existential child
			if(children[fw]->getBlockNum() < minUniBlock)
			{
				// Can force the last existential
				circuit->beginNewReason(this, NULL);

				Signal value;
				value.value = up_value;
				value.type = type;
				value.justified = false;
				addToUPStack( children[fw], value, circuit->last_reason, true );
			}
			else
			{
				// Nothing can be propagated
				sw = uniMinChild;
			}
		}
	}

	if(retval == OK && sw != -1)
	{
		// Set the watches
		// Don't remove from the list of protectWatch

		if(children[fw] == watch2[type] || children[sw] == watch1[type]) // switch fw and sw
		{	int tmp = fw;
			fw = sw;
			sw = tmp;
		}

		// Couldn't have created the mismatch because fw!=sw and we assume unique children
		// and watch1!=watch2

		if(children[fw] != watch1[type])
		{
			if(watch1[type] != changedWatch)
			{
				// This would happen in two scenarios:
				// - The gate just got a value and calls this method
				//   because the watches now need to include information about existentials
				// - One watch was the last existential and the other - leading universal
				//   Because of a backtrack, we now have more than one unset existential
				watch1[type]->removeNotify(down_value, type, this); //(*) Test this
			}
			else
				(*remove) = true;

			children[fw]->addNotify(down_value,type, this);
			watch1[type] = children[fw];
		}

		if(children[sw] != watch2[type])
		{
			if(watch2[type] != changedWatch)
			{
				watch2[type]->removeNotify(down_value, type, this); // (*) Test this
			}
			else
				(*remove) = true;

			children[sw]->addNotify(down_value, type, this);
			watch2[type]=children[sw];
		}
	}
	return retval;
}

int AndOrGate::propagate(short type)
{
	int retval = OK;
	// (*) universal conflict

	retval = notifyWatches(type);
	if( retval != OK ) return retval;

	int value = getValue(type);
	// these cases only propagate on the first assignment
	if( value == down_value)
	{
		// Propagate value to all children		
		for( int i = 0; i < num_children; i++ ) {
			// (*) Make sure only add those that don't already have the value - everywhere! (*)
			if(children[i]->getValue(type) != down_value)
			{
				Signal value;
				value.value = down_value;
				value.type = type;
				value.justified = false;
				circuit->beginNewReason( this, children[i] );
				addToUPStack( children[i], value, circuit->last_reason, true );
			}

		}
	}
	else if(value == up_value)
	{
		if(!isJustified(type))	// If we are justified, already know 
							//that at least one child is up_value
		{
			int retval = makeExistWatches(type, NULL, NULL);
			if(retval != OK)
				return retval;
		}
	}

	return retval;
}

int AndOrGate::getClauseSize(DagNode* child)
{
	if(child == NULL)
		return num_children + 1;
	else
		return 2;
}

Literal *AndOrGate::getClauseLiteral(DagNode* child, int index)
{
	assert(child != this);

	if(child == NULL)
	{
		// This is the clause with all the children
		if(index == 0)
			return getLiteral(this, up_value);
		return getLiteral(children[index-1], down_value);
	}
	if(index == 0)
		return getLiteral(this, down_value);
	assert(index == 1);
	return getLiteral(child, up_value);
}

void AndOrGate::printProofClause(DagNode *child)
{
	int num = num_children;
	if(child != NULL)
	{
		do
		{
			num--;
		}
		while(children[num] != child);
	}

	Common::proof_file << "gate-" << name << "-" << num;
}

