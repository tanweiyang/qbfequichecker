#include "PropRecord.h"


PropRecord::PropRecord()
{
};

void PropRecord::set(DagNode * nvariable, Signal nvalue, Reason * nreason, bool nforced )
{
	variable = nvariable;
	value = nvalue;
	reason = nreason;
	forced = nforced;
}


int PropRecord::propagate1(Solver *s, bool &ignore) // Everything except for variable propagation
{  
	assert(!forced || (reason != NULL && reason->getSize() > 0) || s->getDLevel() == 1);
	//bool ignore;
	bool store = false;
	int retval;

	retval = variable->assign( value, reason, ignore, forced, s->getDLevel(), s->getTrailPos() );
	store |= (retval == OK) && !ignore;

	if(retval == OK && variable->isLeaf())
	{
		assert(!value.justified);
		short otherType = (value.type == TYPE_1LINE? TYPE_0LINE : TYPE_1LINE);
		Signal otherVal(value);
		otherVal.type = otherType;
		bool ignore2;
		retval = variable->assign(otherVal, reason, ignore2, false, s->getDLevel(), s->getTrailPos() );
		store |= retval == OK && !ignore2;
		ignore = ignore && ignore2;
	}

	if(store)
		s->undo_stack.addNew( UNDO_ASSIGNMENT, variable, value.type );

	return retval;
}


const int UPstack::num_first = FIRST_BATCH;
const int UPstack::num_inc = INCREMENT;

UPstack::UPstack() : data(num_first)
{
	upTo = 0;
}

inline int UPstack::next()
{
	if(data.size() <= upTo)
	{
		data.resize(upTo + num_inc);
	}
	return upTo++;
}

void UPstack::addNew(DagNode * variable, Signal value, Reason * reason, bool forced)
{
	data[next()].set(variable, value, reason, forced);
}

bool UPstack::empty()
{
	return upTo == 0;
}

int UPstack::size()
{
	return upTo;
}

int UPstack::propAndPop(Solver *solver)
{
	assert(!empty());

	int retval;
	bool ignore;

	retval = data[upTo-1].propagate1(solver, ignore);
	DagNode *variable = data[upTo-1].variable;
	short type = data[upTo-1].value.type;
	upTo--;

	if(retval == OK && !ignore)
		retval = variable->propagate(type);
	return retval;
}

void UPstack::clear()
{
	upTo = 0;
}
